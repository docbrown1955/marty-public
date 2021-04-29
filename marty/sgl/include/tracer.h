#pragma once

#include "abstractgammasym.h"

namespace sgl {
    class GammaIndex;
}

namespace sgl::tracer {

    /**
     * @brief Calculates the traces of gamma matrices without chiral operator.
     *
     * @param indices Set of generalized indices representing gamma matrices.
     * @param Ddirac  Dirac space dimension.
     *
     * @return The expression corresponding to the trace.
     */
    GExpr standardTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            );

    /**
     * @brief Calculates the traces of gamma matrices with the chiral operator.
     *
     * @param indices Set of generalized indices representing gamma matrices.
     * @param Ddirac  Dirac space dimension.
     *
     * @return The expression corresponding to the trace.
     */
    GExpr chiralTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            );
}
