#pragma once

#include "csl.h"

namespace mty::fierz {

    enum FierzMode {
        Left,
        Right
    };

    bool isFermionic(csl::Expr const &arg);
    std::vector<csl::Index> spinorIndex(
            csl::Expr const &prod,
            FierzMode        mode
            );

    size_t nBasisElement();

    std::pair<csl::Expr, csl::Expr> basisElements(
            size_t            i,
            csl::Index const &a,
            csl::Index const &b,
            csl::Index const &c,
            csl::Index const &d
            );

    void insertIdentity(
            csl::Expr        &prod,
            csl::Index const &a1,
            csl::Index const &a2,
            csl::Index const &b1,
            csl::Index const &b2
            );

    void doApplyFierz(
            csl::Expr &prod,
            csl::Index const &alpha,
            csl::Index const &beta,
            FierzMode         mode
            );

    void applyFierz(
            csl::Expr &prod, 
            FierzMode  mode
            );

} // namespace mty::fierz
