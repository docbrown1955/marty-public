#pragma once

#include "generalizedindex.h"

namespace sgl {

    class IndexChain;

    class MomentumIndex: public AbstractGeneralizedIndex {

    public:

        MomentumIndex(
                csl::Tensor const &p,
                csl::Index  const &mu
                );

        csl::Tensor getP() const {
            return p;
        }

        csl::Expr square() const {
            auto p_cpy = p;
            return p_cpy(m_indices[0])*p_cpy(m_indices[0].getFlipped());
        }
        csl::Expr mass() const {
            return csl::sqrt_s(square());
        }

        bool isZero() const override;

        GExpr copy() const override;
        GExpr refresh() const override;

        csl::Expr toCSL(TensorSet const&) const override;

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

        bool hasPropertyWith(GExpr const &other) const override;
        GExpr propertyWith(GExpr const &other) const override;

        void print(std::ostream &out = std::cout) const override;

    protected:

        bool hasEOMWith(IndexChain const &chain) const;

    protected:

        csl::Tensor p;
    };

    GExpr momentumindex_s(
            csl::Tensor const &p,
            csl::Index  const &mu
            );
}
