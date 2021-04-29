#include "interface.h"

namespace sgl {

    GExpr DeepCopy(GExpr const &expr)
    {
        GExpr res = expr->copy();
        for (size_t i = 0; i != expr->size(); ++i) {
            expr->argument(i) = sgl::DeepCopy(expr->argument(i));
        }
        return expr->copy();
    }

    GExpr DeepRefreshed(GExpr const &expr)
    {
        GExpr res = expr->refresh();
        for (size_t i = 0; i != expr->size(); ++i) {
            expr->argument(i) = sgl::DeepRefreshed(expr->argument(i));
        }
        return expr->refresh();
    }

}
