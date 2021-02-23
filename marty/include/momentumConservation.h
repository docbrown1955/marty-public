#pragma once

#include <vector>
#include "quantumField.h"

namespace mty {

    class MomentumConservater {

    public:

        MomentumConservater(
                std::vector<mty::QuantumField> const &insertions,
                std::vector<csl::Tensor>       const &t_momenta
                );
        bool apply(csl::Expr &expr);

    private:

        int findMomentum(csl::Expr const &sub);
        std::vector<csl::Expr> isMomentumSum(csl::Expr const &sub);
        std::optional<csl::Expr> applyOnMomentumSum(
                std::vector<csl::Expr> &factors
                );

        std::pair<int, csl::Expr> getMomentumStructure(csl::Expr const &term);
        bool simplify(std::vector<csl::Expr> &factors);
        csl::Expr recoverExpr(std::vector<csl::Expr> const &factors);

    private:

        std::vector<bool>        signs;
        std::vector<csl::Tensor> momenta;
        size_t                   posSimplestMomenta;
        csl::Index               mu;
    };
}
