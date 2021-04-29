#pragma once

#include "gammaindex.h"

namespace sgl {

    struct Commutation {
        GExpr      factor;
        GammaIndex left;
        GammaIndex right;
        GExpr remnant;
    };

    std::ostream &operator<<(
            std::ostream &out,
            Commutation const &com
            );

    Commutation commute(
            GammaIndex const &mu,
            GammaIndex const &nu,
            csl::Index const &a,
            csl::Index const &b,
            bool              inverseCall = false
            );
}
