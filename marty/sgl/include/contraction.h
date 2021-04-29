#pragma once

#include "gammaindex.h"

namespace sgl {

    class IndexChain;

    GExpr contraction(
            GammaIndex        mu,
            GammaIndex        nu,
            csl::Index const &a,
            csl::Index const &b
            );

    GExpr epsilonContraction(
            std::vector<csl::Index> A,
            std::vector<csl::Index> B
            );

    GExpr momentumGammaContraction(
            IndexChain  const &init,
            csl::Tensor        p,
            csl::Index  const &mu,
            csl::Index  const &nu,
            size_t             i,
            size_t             j
            );
}
