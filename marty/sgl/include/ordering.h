#pragma once

#include "abstractgammasym.h"

namespace sgl {

    class GammaIndex;

    bool freeOrder(
            std::vector<csl::Index> const &mu,
            std::vector<csl::Index> const &nu
            );

    bool contractedOrder(
            GExpr const &A,
            GExpr const &B
            );

    bool isOrdered(
            GExpr      const &prod,
            GammaIndex const &mu,
            GammaIndex const &nu
            );

    size_t simplest(
            GExpr const &prod,
            GExpr const &chain,
            size_t       begin = 0
            );
}
