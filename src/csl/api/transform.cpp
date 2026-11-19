// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "transform.h"

#include "../abstract.h"
#include "../algo.h"
#include "../evaluation.h"
#include "../interface.h"
#include "../replace.h"
#include "../utils.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace csl::api {

namespace {

/// Collects the distinct symbols of `expr`, keyed by identity.
void collectSymbols(Expr const                           &expr,
                    std::map<std::uintptr_t, ExprHandle> &found)
{
    ExprHandle const handle(expr);
    if (handle.isSymbol()) {
        found.emplace(handle.symbolIdentity(), handle);
        return;
    }
    for (std::size_t i = 0; i < expr->size(); ++i) {
        collectSymbols(expr->getArgument(static_cast<int>(i)), found);
    }
}

/// Raw CSL expressions of a list of handles.
std::vector<Expr> rawList(std::vector<ExprHandle> const &handles)
{
    std::vector<Expr> result;
    result.reserve(handles.size());
    for (auto const &handle : handles) {
        result.push_back(handle.raw());
    }
    return result;
}

/// Replaces the `from` expressions by the `to` expressions in a new copy of
/// `expr`. The input is not modified.
ExprHandle replacedInCopy(ExprHandle const                &expr,
                          std::vector<Substitution> const &substitutions)
{
    std::vector<Expr> from;
    std::vector<Expr> to;
    for (auto const &[source, target] : substitutions) {
        from.push_back(source.raw());
        to.push_back(target.raw());
    }
    return ExprHandle(csl::Replaced(expr.raw(), from, to));
}

} // namespace

ExprHandle expanded(ExprHandle const &expr, bool full)
{
    return detail::guarded("expand", [&] {
        return ExprHandle(csl::Expanded(expr.raw(), full, false));
    });
}

ExprHandle rebuilt(ExprHandle const              &expr,
                   std::vector<ExprHandle> const &arguments)
{
    if (arguments.size() != expr.arguments().size())
        throw ValueError("rebuilt: " + std::to_string(expr.arguments().size())
                         + " arguments expected, "
                         + std::to_string(arguments.size()) + " given");
    return detail::guarded("rebuilt", [&] {
        csl::Expr copy = csl::Copy(expr.raw());
        for (std::size_t i = 0; i != arguments.size(); ++i)
            copy->setArgument(arguments[i].raw(), i);
        return ExprHandle(csl::Refreshed(copy));
    });
}

ExprHandle factored(ExprHandle const &expr, bool full)
{
    return detail::guarded(
        "factor", [&] { return ExprHandle(csl::Factored(expr.raw(), full)); });
}

ExprHandle
factoredBy(ExprHandle const &expr, ExprHandle const &factor, bool full)
{
    return detail::guarded("factor", [&] {
        return ExprHandle(csl::Factored(expr.raw(), factor.raw(), full));
    });
}

ExprHandle collected(ExprHandle const              &expr,
                     std::vector<ExprHandle> const &factors,
                     bool                           full)
{
    std::vector<Expr> const raw = rawList(factors);
    return detail::guarded("collect", [&] {
        if (full) {
            return ExprHandle(csl::DeepCollected(expr.raw(), raw));
        }
        return ExprHandle(csl::Collected(expr.raw(), raw));
    });
}

ExprHandle derived(ExprHandle const &expr, ExprHandle const &symbol)
{
    return detail::guarded("derivative", [&] {
        return ExprHandle(csl::Derived(expr.raw(), symbol.raw()));
    });
}

ExprHandle replaced(ExprHandle const                &expr,
                    std::vector<Substitution> const &substitutions)
{
    return detail::guarded(
        "substitution", [&] { return replacedInCopy(expr, substitutions); });
}

ExprHandle evaluated(ExprHandle const                &expr,
                     std::vector<Substitution> const &values,
                     EvalMode                         mode)
{
    return detail::guarded("evaluation", [&] {
        ExprHandle const substituted = replacedInCopy(expr, values);
        csl::eval::mode const cslMode(static_cast<unsigned>(mode));
        return ExprHandle(csl::Evaluated(substituted.raw(), cslMode));
    });
}

ExprHandle evaluatedByName(ExprHandle const              &expr,
                           std::vector<NamedValue> const &values,
                           EvalMode                       mode)
{
    std::vector<ExprHandle> const symbols = freeSymbols(expr);
    std::vector<Substitution>     substitutions;
    for (auto const &[name, value] : values) {
        std::vector<ExprHandle> matches;
        for (auto const &symbol : symbols) {
            if (symbolName(symbol) == name) {
                matches.push_back(symbol);
            }
        }
        if (matches.size() > 1) {
            throw ValueError("evaluate: the name '" + name
                             + "' matches several distinct symbols");
        }
        if (matches.size() == 1) {
            substitutions.emplace_back(matches.front(), value);
        }
    }
    return evaluated(expr, substitutions, mode);
}

ExprHandle conjugated(ExprHandle const &expr)
{
    return detail::guarded("conjugate", [&] {
        return ExprHandle(csl::GetComplexConjugate(expr.raw()));
    });
}

ExprHandle realPart(ExprHandle const &expr)
{
    return detail::guarded(
        "real part", [&] { return ExprHandle(csl::GetRealPart(expr.raw())); });
}

ExprHandle imagPart(ExprHandle const &expr)
{
    return detail::guarded("imaginary part", [&] {
        return ExprHandle(csl::GetImaginaryPart(expr.raw()));
    });
}

ExprHandle modulus(ExprHandle const &expr)
{
    return detail::guarded("modulus", [&] {
        return ExprHandle(csl::GetComplexModulus(expr.raw()));
    });
}

ExprHandle argument(ExprHandle const &expr)
{
    return detail::guarded("argument", [&] {
        return ExprHandle(csl::GetComplexArgument(expr.raw()));
    });
}

bool dependsOn(ExprHandle const &expr, ExprHandle const &symbol)
{
    return detail::guarded("depends_on", [&] {
        return csl::DependsOn(expr.raw(), symbol.raw());
    });
}

std::vector<ExprHandle> freeSymbols(ExprHandle const &expr)
{
    std::map<std::uintptr_t, ExprHandle> found;
    collectSymbols(expr.raw(), found);

    std::vector<ExprHandle> result;
    result.reserve(found.size());
    for (auto const &entry : found) {
        result.push_back(entry.second);
    }
    std::stable_sort(result.begin(),
                     result.end(),
                     [](ExprHandle const &a, ExprHandle const &b) {
                         return symbolName(a) < symbolName(b);
                     });
    return result;
}

std::string symbolName(ExprHandle const &symbol)
{
    if (!symbol.isSymbol()) {
        throw TypeError("name: only constants and variables have a name");
    }
    return symbol.raw()->getName();
}

Number numericValue(ExprHandle const &expr)
{
    ExprHandle const value = evaluated(expr, {});

    std::vector<ExprHandle> const symbols = freeSymbols(value);
    if (!symbols.empty()) {
        std::string list;
        for (auto const &symbol : symbols) {
            if (!list.empty()) {
                list += ", ";
            }
            list += symbolName(symbol);
        }
        throw ValueError("numeric value: free symbols remain: " + list);
    }

    auto const number = value.number();
    if (!number) {
        throw ValueError("numeric value: the expression is not a number");
    }
    return *number;
}

std::string text(ExprHandle const &expr)
{
    return detail::guarded("printing", [&] {
        std::ostringstream out;
        expr.raw()->print(0, out);
        return out.str();
    });
}

std::string latex(ExprHandle const &expr)
{
    return detail::guarded("printing",
                           [&] { return expr.raw()->printLaTeX(0); });
}

} // namespace csl::api
