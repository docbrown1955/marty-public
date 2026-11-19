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

/**
 * @file transform.h
 * @brief Transformations, evaluation, numeric conversion and printing of
 *        `ExprHandle` for the Python API.
 *
 * Every function takes its inputs by const reference and returns a new
 * expression: no input is mutated. Python names are given in each comment.
 * The printing functions are declared here too, so that the transformations
 * and their output share one facade.
 */

#ifndef CSL_API_TRANSFORM_H_INCLUDED
#define CSL_API_TRANSFORM_H_INCLUDED

#include "expr.h"

#include <string>
#include <utility>
#include <vector>

namespace csl::api {

/// Pair (from, to) of a substitution, applied in order.
using Substitution = std::pair<ExprHandle, ExprHandle>;

/// Pair (name, value) of a value given by symbol name.
using NamedValue = std::pair<std::string, ExprHandle>;

/// Python: `expand()`. Full (deep) expansion of products and integer powers
/// of sums. The non-deep CSL `Expanded` leaves `(x+y)^2` unchanged.
[[nodiscard]] ExprHandle expanded(ExprHandle const &expr);

/// Python: `factor(full=full)`. Factors the common factors out of sums.
[[nodiscard]] ExprHandle factored(ExprHandle const &expr, bool full = false);

/// Python: `factor(by, full=full)`. Factors `factor` out of the sums.
[[nodiscard]] ExprHandle factoredBy(ExprHandle const &expr,
                                    ExprHandle const &factor,
                                    bool              full = false);

/// Python: `collect(factors, full=full)`. Groups the terms of sums by the
/// given factors. Throws TypeError for an indexed factor (CSL restriction).
[[nodiscard]] ExprHandle collected(ExprHandle const              &expr,
                                   std::vector<ExprHandle> const &factors,
                                   bool full = false);

/// Python: `diff(symbol)`. Derivative with respect to a symbol.
[[nodiscard]] ExprHandle derived(ExprHandle const &expr,
                                 ExprHandle const &symbol);

/// Python: `subs(mapping)`. Replaces every `from` by its `to` simultaneously
/// (a swap of two symbols is valid, see SubsSymbolSwap). Compound keys such
/// as `x*y` are matched structurally (see SubsCompoundKeyBehaviourPinned).
[[nodiscard]] ExprHandle
replaced(ExprHandle const                &expr,
         std::vector<Substitution> const &substitutions);

/// Evaluation modes of `evaluate`, one bit per CSL `eval` flag. Modes combine
/// with `|`. `All` is the union of the other modes and is the default.
enum class EvalMode : unsigned {
    Base         = 0,
    Literal      = 1,
    Numerical    = 2,
    Indicial     = 4,
    Abbreviation = 8,
    All          = 15
};

constexpr EvalMode operator|(EvalMode lhs, EvalMode rhs)
{
    return static_cast<EvalMode>(static_cast<unsigned>(lhs)
                                 | static_cast<unsigned>(rhs));
}

constexpr EvalMode operator&(EvalMode lhs, EvalMode rhs)
{
    return static_cast<EvalMode>(static_cast<unsigned>(lhs)
                                 & static_cast<unsigned>(rhs));
}

/// Python: `evaluate(mapping, mode=...)`. Replaces the symbols by their
/// values, then evaluates the operations selected by `mode`. Free symbols are
/// kept.
[[nodiscard]] ExprHandle evaluated(ExprHandle const                &expr,
                                   std::vector<Substitution> const &values,
                                   EvalMode mode = EvalMode::All);

/// Python: `evaluate(**by_name, mode=...)`. Each name is matched against the free
/// symbols of `expr`. Throws ValueError if a name matches several distinct
/// symbols. A name that matches no symbol is ignored.
[[nodiscard]] ExprHandle
evaluatedByName(ExprHandle const &expr, std::vector<NamedValue> const &values,
                EvalMode mode = EvalMode::All);

/// Python: `conjugate()`. Complex conjugate (CSL `GetComplexConjugate`).
[[nodiscard]] ExprHandle conjugated(ExprHandle const &expr);

/// Python: `real_part()`. Real part. For a node without a rule, returns the
/// input (CSL `GetRealPart`).
[[nodiscard]] ExprHandle realPart(ExprHandle const &expr);

/// Python: `imag_part()`. Imaginary part. For a node without a rule, returns
/// zero (CSL `GetImaginaryPart`).
[[nodiscard]] ExprHandle imagPart(ExprHandle const &expr);

/// Modulus of a complex expression, `sqrt(re^2 + im^2)` (CSL
/// `GetComplexModulus`).
[[nodiscard]] ExprHandle modulus(ExprHandle const &expr);

/// Argument (phase) of a complex expression, in (-pi, pi] (CSL
/// `GetComplexArgument`).
[[nodiscard]] ExprHandle argument(ExprHandle const &expr);

/// Python: `depends_on(symbol)`. True if `expr` depends on `symbol`.
[[nodiscard]] bool dependsOn(ExprHandle const &expr, ExprHandle const &symbol);

/// Python: `free_symbols`. Distinct constants and variables of `expr`,
/// sorted by name, then by identity. Symbols are distinct when their
/// identities differ, even if their names are equal.
[[nodiscard]] std::vector<ExprHandle> freeSymbols(ExprHandle const &expr);

/// Python: `name`. Name of a constant or a variable. Throws TypeError for
/// any other expression.
[[nodiscard]] std::string symbolName(ExprHandle const &symbol);

/// Exact value of an expression without free symbols, after numerical
/// evaluation. Throws ValueError listing the free symbols if some remain,
/// and ValueError if the expression is not a number.
[[nodiscard]] Number numericValue(ExprHandle const &expr);

/// Python: `str(expr)`. Text output of CSL (`Abstract::print`).
[[nodiscard]] std::string text(ExprHandle const &expr);

/// Python: `to_latex()`. LaTeX output of CSL (`Abstract::printLaTeX`).
[[nodiscard]] std::string latex(ExprHandle const &expr);

} // namespace csl::api

#endif
