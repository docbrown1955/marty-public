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
 * @file functions.h
 * @brief Elementary mathematical functions of the CSL API (Python neutral).
 *
 * Each function names its Python spelling in the comment. The names are
 * camel case so that they do not clash with the C library functions.
 */

#ifndef CSL_API_FUNCTIONS_H_INCLUDED
#define CSL_API_FUNCTIONS_H_INCLUDED

#include "expr.h"

namespace csl::api {

/// Abbreviation named `name` for `expr`. Python: `make_abbreviation(name,
/// expr)`. Evaluating with `EvalMode::Abbreviation` (or `All`) replaces it by
/// `expr`.
[[nodiscard]] ExprHandle makeAbbreviation(std::string const &name,
                                          ExprHandle const  &expr);

/// Square root, `sqrt(e)`.
[[nodiscard]] ExprHandle Sqrt(ExprHandle const &e);
/// Absolute value, `abs(e)`.
[[nodiscard]] ExprHandle Abs(ExprHandle const &e);
/// Exponential, `exp(e)`.
[[nodiscard]] ExprHandle Exp(ExprHandle const &e);
/// Natural logarithm, `log(e)`.
[[nodiscard]] ExprHandle Log(ExprHandle const &e);
/// Sine, `sin(e)`.
[[nodiscard]] ExprHandle Sin(ExprHandle const &e);
/// Cosine, `cos(e)`.
[[nodiscard]] ExprHandle Cos(ExprHandle const &e);
/// Tangent, `tan(e)`.
[[nodiscard]] ExprHandle Tan(ExprHandle const &e);
/// Arc sine, `asin(e)`.
[[nodiscard]] ExprHandle Asin(ExprHandle const &e);
/// Arc cosine, `acos(e)`.
[[nodiscard]] ExprHandle Acos(ExprHandle const &e);
/// Arc tangent, `atan(e)`.
[[nodiscard]] ExprHandle Atan(ExprHandle const &e);
/// Hyperbolic sine, `sinh(e)`.
[[nodiscard]] ExprHandle Sinh(ExprHandle const &e);
/// Hyperbolic cosine, `cosh(e)`.
[[nodiscard]] ExprHandle Cosh(ExprHandle const &e);
/// Hyperbolic tangent, `tanh(e)`.
[[nodiscard]] ExprHandle Tanh(ExprHandle const &e);
/// Inverse hyperbolic sine, `asinh(e)`.
[[nodiscard]] ExprHandle Asinh(ExprHandle const &e);
/// Inverse hyperbolic cosine, `acosh(e)`.
[[nodiscard]] ExprHandle Acosh(ExprHandle const &e);
/// Inverse hyperbolic tangent, `atanh(e)`.
[[nodiscard]] ExprHandle Atanh(ExprHandle const &e);

} // namespace csl::api

#endif
