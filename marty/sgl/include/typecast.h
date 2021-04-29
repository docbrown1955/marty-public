#pragma once

#include "abstractgammasym.h"

namespace sgl {

    template<class Type>
    bool IsType(GExpr const &expr)
    {
        return dynamic_cast<Type const*>(expr.get());
    }

    template<class Type>
    std::shared_ptr<Type> ConvertTo(GExpr const &expr)
    {
        return std::dynamic_pointer_cast<Type>(expr);
    }
}
