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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief File containing a little looptools extension in order to call
 * functions of this librairy more easily from std::vector of data directly,
 * with double, complex numbers of csl expressions.
 */
#ifndef LOOPTOOLS_EXTENSION_H_INCLUDED
#define LOOPTOOLS_EXTENSION_H_INCLUDED

#include "../../csl/csl.h"
#include <complex>

namespace mty {

using ltComplexType = std::complex<double>;
using ltRealType  = double;

///////////////////////////////////////////////////
// Conversion functions
///////////////////////////////////////////////////

csl::Expr complexToExpr(ltComplexType const &complexNumber);

ltComplexType exprToComplexType(csl::Expr const &init);

double exprToRealType(csl::Expr const &init);

std::vector<ltComplexType> exprToComplexType(std::vector<csl::Expr> const &init);

std::vector<double> exprToRealType(std::vector<csl::Expr> const &init);

std::optional<std::vector<ltRealType>>
isReal(std::vector<ltComplexType> const &init);

///////////////////////////////////////////////////
// Functions taking ltRealType
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<ltRealType> const &arguments);

ltComplexType B0i_runtime(int id, std::vector<ltRealType> const &arguments);

ltComplexType C0i_runtime(int id, std::vector<ltRealType> const &arguments);

ltComplexType D0i_runtime(int id, std::vector<ltRealType> const &arguments);

ltComplexType E0i_runtime(int id, std::vector<ltRealType> const &arguments);

///////////////////////////////////////////////////
// Fucntions taking complex
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<ltComplexType> const &arguments);

ltComplexType B0i_runtime(int id, std::vector<ltComplexType> const &arguments);

ltComplexType C0i_runtime(int id, std::vector<ltComplexType> const &arguments);

ltComplexType D0i_runtime(int id, std::vector<ltComplexType> const &arguments);

ltComplexType E0i_runtime(int id, std::vector<ltComplexType> const &arguments);

///////////////////////////////////////////////////
// Functions taking csl::Expr
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<csl::Expr> const &arguments);

ltComplexType B0i_runtime(int id, std::vector<csl::Expr> const &arguments);

ltComplexType C0i_runtime(int id, std::vector<csl::Expr> const &arguments);

ltComplexType D0i_runtime(int id, std::vector<csl::Expr> const &arguments);

ltComplexType E0i_runtime(int id, std::vector<csl::Expr> const &arguments);

} // End of namespace mty

#endif /* ifndef LOOPTOOLS_EXTENSION_H_INCLUDED */
