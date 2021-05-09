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
 * @file sglalgo.h
 * @brief Contains recursive algorithms for SGL.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include "abstractgammasym.h"
#include "typecast.h"

namespace sgl {

    template<typename Type>
        void for_each(
                GExpr &expr,
                std::function<void(GExpr &)> f
                )
        {
            const size_t sz = expr->size();
            for (size_t i = 0; i != sz; ++i) 
                sgl::for_each<Type>(expr->argument(i), f);
            if (IsType<Type>(expr))
                f(expr);
        }

    template<typename Type>
        bool transform(
                GExpr &expr,
                std::function<bool(GExpr &)> f
                )
        {
            const size_t sz = expr->size();
            bool transformed = false;
            for (size_t i = 0; i != sz; ++i) 
                if (sgl::transform<Type>(expr->argument(i), f))
                    transformed = true;
            if (IsType<Type>(expr)) {
                transformed = f(expr) || transformed;
            }
            if (transformed)
                expr = expr->refresh();
            return transformed;
        }
}
