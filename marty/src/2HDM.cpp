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

#include "2HDM.h"

namespace mty {

std::pair<csl::Expr, csl::Expr> Z2_coef(
        int         charge)
{
    if (charge == 1)
        return { CSL_0, CSL_1 };
    return { CSL_1, CSL_0 };
}
csl::Expr Z2_mass_coef(
        csl::Expr const &v1,
        csl::Expr const &v2,
        int         charge)
{
    if (charge == 1)
        return csl::sqrt_s(2) / v2;
    return csl::sqrt_s(2) / v1;
}

} // End of namespace mty
