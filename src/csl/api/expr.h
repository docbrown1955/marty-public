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
 * @file expr.h
 * @brief Python neutral handle on CSL expressions: numbers and symbols.
 *
 * `ExprHandle` has value semantics and is never mutated. Every operation that
 * reaches the computer algebra system translates the CSL error enums
 * (`CSLError`, which carry no message) into the typed errors of this API,
 * with a message that names the operation and the cause.
 */

#ifndef CSL_API_EXPR_H_INCLUDED
#define CSL_API_EXPR_H_INCLUDED

#include "../abstract.h" // csl::Expr is defined there
#include "../enum.h"     // csl::ComplexProperty
#include "../error.h"    // CSLError
#include "error.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace csl::api {

/// Kind of a numerical value.
enum class NumberKind { Integer, Rational, Real, Complex };

/// Exact value of a numerical expression, as the Python layer converts it.
struct Number {
    NumberKind  kind = NumberKind::Integer;
    long long   num  = 0; ///< Integer value, or numerator of a rational.
    long long   den  = 1; ///< Denominator of a rational.
    long double real = 0; ///< Real value, or real part of a complex number.
    long double imag = 0; ///< Imaginary part of a complex number.
};

/// Shape of a node of a structural description.
enum class Shape {
    Number,   ///< Numerical value, compared by value.
    Symbol,   ///< Constant or variable, compared by identity.
    Opaque,   ///< Leaf of another kind, compared by CSL equality.
    Ordered,  ///< Compound node whose children keep their order.
    Unordered ///< Sum or product: children are a multiset.
};

class ExprHandle;

/// Canonical description of an expression tree, see ExprHandle::structure().
struct Structure;

/// Immutable handle on a CSL expression.
class ExprHandle {
  public:
    /// Wraps an existing CSL expression.
    explicit ExprHandle(Expr expr);

    /// Underlying CSL expression.
    [[nodiscard]] Expr const &raw() const noexcept
    {
        return m_expr;
    }

    /// Integer number.
    [[nodiscard]] static ExprHandle integer(long long value);
    /// Rational number num/den in lowest terms (den must not be 0).
    [[nodiscard]] static ExprHandle rational(long long num, long long den);
    /// Floating point number.
    [[nodiscard]] static ExprHandle real(long double value);
    /// Complex number real + i * imag.
    [[nodiscard]] static ExprHandle complex(ExprHandle const &re,
                                            ExprHandle const &im);
    /// Imaginary unit I.
    [[nodiscard]] static ExprHandle imaginaryUnit();
    /// Infinity oo (CSL_INF).
    [[nodiscard]] static ExprHandle infinity();
    /// Archimedes' constant pi (CSL_PI).
    [[nodiscard]] static ExprHandle pi();
    /// Euler's number e (CSL_E).
    [[nodiscard]] static ExprHandle e();
    /// Constant symbol with a complex property. Each call creates a new
    /// symbol, whatever the name.
    [[nodiscard]] static ExprHandle
    constant(std::string const   &name,
             csl::ComplexProperty complexProperty
             = csl::ComplexProperty::Real);
    /// Variable symbol with a complex property. Each call creates a new
    /// symbol, whatever the name.
    [[nodiscard]] static ExprHandle
    variable(std::string const   &name,
             csl::ComplexProperty complexProperty
             = csl::ComplexProperty::Real);

    /// Exact numerical value, or nothing if the expression is not a number.
    [[nodiscard]] std::optional<Number> number() const;
    /// True for a constant or a variable.
    [[nodiscard]] bool isSymbol() const;
    /// Identity of the symbol: the address of its parent. Requires isSymbol().
    [[nodiscard]] std::uintptr_t symbolIdentity() const;
    /// Throws TypeError unless the expression is a number or a symbol.
    void checkHashable() const;
    /// CSL equality (Abstract::operator==).
    [[nodiscard]] bool equals(ExprHandle const &other) const;
    /// Canonical description of the expression tree.
    [[nodiscard]] Structure structure() const;

    /// Arguments of a compound expression, in order. Empty for a leaf.
    [[nodiscard]] std::vector<ExprHandle> arguments() const;
    /// Argument at \p index. Throws LookupError when out of range.
    [[nodiscard]] ExprHandle argument(std::size_t index) const;
    /// Operand of an operator (derivative, real part, ...). Throws TypeError
    /// for an expression that is not an operator.
    [[nodiscard]] ExprHandle operand() const;
    /// Numerical factor in front of the expression, 1 if there is none.
    [[nodiscard]] ExprHandle numericalFactor() const;
    /// Expression without its numerical factor, nothing if there is none.
    [[nodiscard]] std::optional<ExprHandle> term() const;
    /// Candidate factors of a product, or of the expression itself.
    [[nodiscard]] std::vector<ExprHandle> factors() const;
    /// Exponent of each given factor in the expression, 0 when absent.
    [[nodiscard]] std::vector<ExprHandle>
    exponents(std::vector<ExprHandle> const &factors) const;

  private:
    Expr m_expr;
};

/// Canonical description of an expression tree.
struct Structure {
    Shape                     shape = Shape::Opaque;
    int                       type  = 0; ///< Value of csl::Type of the node.
    std::optional<Number>     number;    ///< Set for Shape::Number.
    std::optional<ExprHandle> expr;      ///< Set for Symbol and Opaque nodes.
    std::vector<Structure> children; ///< Set for Ordered and Unordered nodes.
};

/// Sum of two expressions.
[[nodiscard]] ExprHandle operator+(ExprHandle const &lhs,
                                   ExprHandle const &rhs);
/// Difference of two expressions.
[[nodiscard]] ExprHandle operator-(ExprHandle const &lhs,
                                   ExprHandle const &rhs);
/// Opposite of an expression.
[[nodiscard]] ExprHandle operator-(ExprHandle const &expr);
/// Product of two expressions.
[[nodiscard]] ExprHandle operator*(ExprHandle const &lhs,
                                   ExprHandle const &rhs);
/// Quotient of two expressions. Division by zero gives oo (CSL behaviour).
[[nodiscard]] ExprHandle operator/(ExprHandle const &lhs,
                                   ExprHandle const &rhs);
/// Power base ** exponent.
/// Throws ZeroDivisionError for zero raised to a negative numerical power,
/// and ValueError for oo as base.
[[nodiscard]] ExprHandle power(ExprHandle const &base,
                               ExprHandle const &exponent);

namespace detail {

/// Throws the typed error that corresponds to a CSL error kind.
/// The message is "<what>: <cause>".
[[noreturn]] void throwCSLError(CSLError kind, std::string_view what);

/// Runs `call`, translating CSL error enums into typed API errors.
template <class Call>
auto guarded(std::string_view what, Call &&call) -> decltype(call())
{
    try {
        return call();
    }
    catch (CSLError const &kind) {
        throwCSLError(kind, what);
    }
}

} // namespace detail

} // namespace csl::api

#endif
