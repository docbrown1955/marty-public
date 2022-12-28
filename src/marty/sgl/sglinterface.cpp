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

#include "sglinterface.h"

namespace sgl {

GExpr DeepCopy(GExpr const &expr)
{
    GExpr res = expr->copy();
    for (size_t i = 0; i != res->size(); ++i) {
        res->argument(i) = sgl::DeepCopy(res->argument(i));
    }
    return res->copy();
}

GExpr DeepRefreshed(GExpr const &expr)
{
    GExpr res = expr->copy();
    for (size_t i = 0; i != res->size(); ++i) {
        res->argument(i) = sgl::DeepRefreshed(res->argument(i));
    }
    return res->refresh();
}

} // namespace sgl
