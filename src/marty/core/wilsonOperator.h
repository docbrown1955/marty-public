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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef MTY_WILSONOPERATOR_H_INCLUDED
#define MTY_WILSONOPERATOR_H_INCLUDED

#include "builtinOperators.h"
#include "diracology.h"
#include "polarization.h"
#include "wilson.h"

namespace mty {

csl::Expr getWilsonCoefficient(WilsonSet const &          wilsons,
                               std::vector<Wilson> const &contributions);

csl::Expr getWilsonCoefficient(WilsonSet const &             wilsons,
                               std::vector<csl::Expr> const &contributions);

csl::Expr getWilsonCoefficient(WilsonSet const &wilsons,
                               Wilson const &   contributions);

csl::Expr getWilsonCoefficient(WilsonSet const &wilsons,
                               csl::Expr const &contributions);

csl::Expr getWilsonCoefficient(Model const &    model,
                               WilsonSet const &wilsons,
                               DiracCoupling    coupling);

} // End of namespace mty

#endif /* ifndef MTY_WILSONOPERATOR_H_INCLUDED */
