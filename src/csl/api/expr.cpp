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

#include "expr.h"

#include "../abstract.h"
#include "../enum.h"
#include "../literal.h"
#include "../numerical.h"
#include "../operations.h"

#include <cstddef>
#include <string>

namespace csl::api {

namespace detail {

void throwCSLError(CSLError kind, std::string_view what)
{
    std::string const operation(what);
    switch (kind) {
    case CSLError::ZeroDivisionError:
        throw ZeroDivisionError(operation + ": division by zero");
    case CSLError::ValueError:
        throw ValueError(operation + ": value not allowed");
    case CSLError::RuntimeError:
        throw ValueError(operation
                         + ": runtime error in the computer algebra system");
    case CSLError::TypeError:
        throw TypeError(operation + ": type not allowed");
    case CSLError::NotImplementedError:
        throw NotImplementedError(operation
                                  + ": not implemented in the computer "
                                    "algebra "
                                    "system");
    default:
        throw Error(operation
                    + ": internal error in the computer algebra system");
    }
}

} // namespace detail

ExprHandle::ExprHandle(Expr expr) : m_expr(std::move(expr))
{
}

ExprHandle ExprHandle::integer(long long value)
{
    return ExprHandle(csl::int_s(value));
}

ExprHandle ExprHandle::rational(long long num, long long den)
{
    return ExprHandle(csl::intfraction_s(num, den));
}

ExprHandle ExprHandle::real(long double value)
{
    return ExprHandle(csl::float_s(value));
}

ExprHandle ExprHandle::complex(ExprHandle const &re, ExprHandle const &im)
{
    return ExprHandle(csl::complex_s(re.raw(), im.raw()));
}

ExprHandle ExprHandle::imaginaryUnit()
{
    return ExprHandle(CSL_I);
}

ExprHandle ExprHandle::infinity()
{
    return ExprHandle(CSL_INF);
}

ExprHandle ExprHandle::pi()
{
    return ExprHandle(CSL_PI);
}

ExprHandle ExprHandle::e()
{
    return ExprHandle(CSL_E);
}

ExprHandle ExprHandle::constant(std::string const   &name,
                                csl::ComplexProperty complexProperty)
{
    return ExprHandle(csl::constant_s(name, complexProperty));
}

ExprHandle ExprHandle::variable(std::string const   &name,
                                csl::ComplexProperty complexProperty)
{
    return ExprHandle(csl::variable_s(name, complexProperty));
}

std::optional<Number> ExprHandle::number() const
{
    switch (m_expr->getType()) {
    case csl::Type::Integer:
        return Number{NumberKind::Integer,
                      static_cast<long long>(m_expr->evaluateScalar())};
    case csl::Type::IntFraction: {
        auto const *fraction
            = dynamic_cast<csl::IntFraction const *>(m_expr.get());
        if (fraction == nullptr)
            return std::nullopt;
        return Number{
            NumberKind::Rational, fraction->getNum(), fraction->getDenom()};
    }
    case csl::Type::Float:
        return Number{NumberKind::Real, 0, 1, m_expr->evaluateScalar(), 0};
    case csl::Type::Complex: {
        auto const *complex = dynamic_cast<csl::Complex const *>(m_expr.get());
        if (complex == nullptr)
            return std::nullopt;
        std::optional<Expr> const re = complex->getRealPart();
        Expr const                im = complex->getImaginaryPart();
        return Number{NumberKind::Complex,
                      0,
                      1,
                      re ? (*re)->evaluateScalar() : 0,
                      im->evaluateScalar()};
    }
    case csl::Type::Imaginary:
        return Number{NumberKind::Complex, 0, 1, 0, 1};
    default:
        return std::nullopt;
    }
}

bool ExprHandle::isSymbol() const
{
    auto const type = m_expr->getType();
    return type == csl::Type::Constant or type == csl::Type::Variable;
}

std::uintptr_t ExprHandle::symbolIdentity() const
{
    return reinterpret_cast<std::uintptr_t>(m_expr->getParent_info());
}

void ExprHandle::checkHashable() const
{
    if (number() or isSymbol())
        return;
    throw TypeError("compound expressions are not hashable, "
                    "use structural_key() instead");
}

bool ExprHandle::equals(ExprHandle const &other) const
{
    return detail::guarded(
        "comparison", [&] { return m_expr->operator==(other.m_expr.get()); });
}

Structure ExprHandle::structure() const
{
    Structure result;
    if (auto value = number()) {
        result.shape  = Shape::Number;
        result.number = value;
        return result;
    }
    result.type = static_cast<int>(m_expr->getType());
    if (isSymbol()) {
        result.shape = Shape::Symbol;
        result.expr  = *this;
        return result;
    }
    std::size_t const count = m_expr->size();
    if (count == 0) {
        result.shape = Shape::Opaque;
        result.expr  = *this;
        return result;
    }
    auto const type      = m_expr->getType();
    bool const unordered = type == csl::Type::Sum or type == csl::Type::Prod;
    result.shape         = unordered ? Shape::Unordered : Shape::Ordered;
    for (std::size_t i = 0; i < count; ++i) {
        result.children.push_back(
            ExprHandle(m_expr->getArgument(static_cast<int>(i))).structure());
    }
    return result;
}

std::vector<ExprHandle> ExprHandle::arguments() const
{
    std::vector<ExprHandle> result;
    int const               count = m_expr->getNArgs();
    result.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        result.emplace_back(m_expr->getArgument(i));
    }
    return result;
}

ExprHandle ExprHandle::argument(std::size_t index) const
{
    if (index >= static_cast<std::size_t>(m_expr->getNArgs())) {
        throw LookupError("argument: index " + std::to_string(index)
                          + " is out of range");
    }
    return ExprHandle(m_expr->getArgument(static_cast<int>(index)));
}

ExprHandle ExprHandle::operand() const
{
    if (not m_expr->isAnOperator()) {
        throw TypeError("operand: expression is not an operator");
    }
    return detail::guarded("operand",
                           [&] { return ExprHandle(m_expr->getOperand()); });
}

ExprHandle ExprHandle::numericalFactor() const
{
    return detail::guarded("numerical factor", [&] {
        return ExprHandle(m_expr->getNumericalFactor());
    });
}

std::optional<ExprHandle> ExprHandle::term() const
{
    return detail::guarded("term", [&] {
        std::optional<Expr> const term = m_expr->getTerm();
        if (not term)
            return std::optional<ExprHandle>();
        return std::optional<ExprHandle>(ExprHandle(*term));
    });
}

std::vector<ExprHandle> ExprHandle::factors() const
{
    return detail::guarded("factors", [&] {
        std::vector<ExprHandle> result;
        for (auto const &factor : m_expr->getFactors()) {
            result.emplace_back(factor);
        }
        return result;
    });
}

std::vector<ExprHandle>
ExprHandle::exponents(std::vector<ExprHandle> const &factors) const
{
    return detail::guarded("exponents", [&] {
        std::vector<Expr> given;
        given.reserve(factors.size());
        for (auto const &factor : factors) {
            given.push_back(factor.raw());
        }
        std::vector<Expr> found(given.size(), CSL_0);
        m_expr->getExponents(given, found);
        std::vector<ExprHandle> result;
        result.reserve(found.size());
        for (auto const &exponent : found) {
            result.emplace_back(exponent);
        }
        return result;
    });
}

ExprHandle operator+(ExprHandle const &lhs, ExprHandle const &rhs)
{
    return detail::guarded("addition", [&] {
        return ExprHandle(csl::operator+(lhs.raw(), rhs.raw()));
    });
}

ExprHandle operator-(ExprHandle const &lhs, ExprHandle const &rhs)
{
    return detail::guarded("subtraction", [&] {
        return ExprHandle(csl::operator-(lhs.raw(), rhs.raw()));
    });
}

ExprHandle operator-(ExprHandle const &expr)
{
    return detail::guarded(
        "negation", [&] { return ExprHandle(csl::operator-(expr.raw())); });
}

ExprHandle operator*(ExprHandle const &lhs, ExprHandle const &rhs)
{
    return detail::guarded("multiplication", [&] {
        return ExprHandle(csl::operator*(lhs.raw(), rhs.raw()));
    });
}

ExprHandle operator/(ExprHandle const &lhs, ExprHandle const &rhs)
{
    return detail::guarded("division", [&] {
        return ExprHandle(csl::operator/(lhs.raw(), rhs.raw()));
    });
}

ExprHandle power(ExprHandle const &base, ExprHandle const &exponent)
{
    return detail::guarded("power", [&] {
        return ExprHandle(csl::pow_s(base.raw(), exponent.raw()));
    });
}

} // namespace csl::api
