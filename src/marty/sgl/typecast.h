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
 * @file typecast.h
 * @brief RTTI utilities for SGL (run-time type information).
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "abstractgammasym.h"

namespace sgl {

template <class Type>
bool IsType(GExpr const &expr)
{
    return dynamic_cast<Type const *>(expr.get());
}

template <class Type>
std::shared_ptr<Type> ConvertTo(GExpr const &expr)
{
    return std::dynamic_pointer_cast<Type>(expr);
}
} // namespace sgl
