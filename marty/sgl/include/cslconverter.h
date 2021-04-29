#pragma once

#include "abstractgammasym.h"

namespace sgl {

    struct TensorSet;

    GExpr csl_simplified(
            GExpr const &expr,
            TensorSet const &tensors
            );

    csl::Expr sgl_to_csl(
            GExpr const &expr,
            TensorSet const &tensors
            );

    GExpr csl_to_sgl(
            csl::Expr const &expr,
            TensorSet const &tensors
            );
}
