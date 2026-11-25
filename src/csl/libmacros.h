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
// along with MARTY. If not, see <http://www.gnu.org/licenses/>.

#ifndef CSL_LIBMACROS_H_INCLUDED
#define CSL_LIBMACROS_H_INCLUDED

/*! \file libmacros.h
 * \brief Names of the macros used by the numerical code printed for the
 * generated C and C++ libraries (see Abstract::printLib()).
 *
 * The macros themselves are defined by the library generator in the private
 * header mty_numerical_defs.h, which resolves them for the target language
 * (C or C++) and precision (double or quadruple). Printing code only uses the
 * names below, so that it never depends on the library generator.
 */

namespace csl::libmacro {

//! Function of a real argument, e.g. MTY_FN(exp)(x) gives exp(x)
inline constexpr const char *fn = "MTY_FN";
//! Function of a complex argument, e.g. MTY_CFN(exp)(z) gives cexp(z)
inline constexpr const char *cfn = "MTY_CFN";
//! Floating literal, e.g. MTY_LIT(2.0) is 2.0 in double or 2.0Q in quadruple
inline constexpr const char *lit = "MTY_LIT";
//! Imaginary unit
inline constexpr const char *imag = "MTY_I";
//! Complex value from its real and imaginary parts
inline constexpr const char *cplx = "MTY_CPLX";
//! Factorial, e.g. MTY_FACT(n) gives n! as Gamma(n+1)
inline constexpr const char *fact = "MTY_FACT";

} // namespace csl::libmacro

#endif
