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

#include "SU_N_algebra.h"

namespace gell_mann {

csl::Expr generate_f_ABC()
{
    csl::Expr f = csl::highdtensor_s({8, 8, 8}, CSL_0);
    f->setArgument(CSL_1, {0, 1, 2});
    f->setArgument(CSL_HALF, {0, 3, 6});
    f->setArgument(CSL_HALF, {1, 3, 5});
    f->setArgument(CSL_HALF, {1, 4, 6});
    f->setArgument(CSL_HALF, {2, 3, 4});
    f->setArgument(-CSL_HALF, {0, 4, 5});
    f->setArgument(-CSL_HALF, {2, 5, 6});
    f->setArgument(csl::sqrt_s(csl::int_s(3)) / 2, {3, 4, 7});
    f->setArgument(csl::sqrt_s(csl::int_s(3)) / 2, {5, 6, 7});

    return f->antisymmetrise();
}

csl::Expr generate_d_ABC()
{
    csl::Expr d = csl::highdtensor_s({8, 8, 8}, CSL_0);
    d->setArgument(CSL_HALF, {0, 3, 5});
    d->setArgument(CSL_HALF, {0, 4, 6});
    d->setArgument(-CSL_HALF, {1, 3, 6});
    d->setArgument(CSL_HALF, {1, 4, 5});
    d->setArgument(CSL_HALF, {2, 3, 3});
    d->setArgument(CSL_HALF, {2, 4, 4});
    d->setArgument(-CSL_HALF, {2, 5, 5});
    d->setArgument(-CSL_HALF, {2, 6, 6});
    d->setArgument(1 / csl::sqrt_s(csl::int_s(3)), {0, 0, 7});
    d->setArgument(1 / csl::sqrt_s(csl::int_s(3)), {1, 1, 7});
    d->setArgument(1 / csl::sqrt_s(csl::int_s(3)), {2, 2, 7});
    d->setArgument(-1 / csl::sqrt_s(csl::int_s(3)), {7, 7, 7});
    d->setArgument(-1 / (2 * csl::sqrt_s(csl::int_s(3))), {3, 3, 7});
    d->setArgument(-1 / (2 * csl::sqrt_s(csl::int_s(3))), {4, 4, 7});
    d->setArgument(-1 / (2 * csl::sqrt_s(csl::int_s(3))), {6, 6, 7});
    d->setArgument(-1 / (2 * csl::sqrt_s(csl::int_s(3))), {6, 6, 7});

    return d->symmetrise();
}

} // namespace gell_mann
