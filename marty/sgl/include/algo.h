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
