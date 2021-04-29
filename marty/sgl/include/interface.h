#pragma once

#include "abstractgammasym.h"

namespace sgl {

    GExpr DeepCopy(GExpr const &expr);
    GExpr DeepRefreshed(GExpr const &expr);
}
