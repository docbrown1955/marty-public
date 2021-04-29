#pragma once

#include "generalizedindex.h"

namespace sgl {

    class EpsilonIndex: public AbstractGeneralizedIndex {

    public:

        EpsilonIndex(
                csl::Index const &mu,
                csl::Index const &nu,
                csl::Index const &rho,
                csl::Index const &sigma
                );

        bool isZero() const override;

        GExpr copy() const override;
        GExpr refresh() const override;

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

        csl::Expr toCSL(TensorSet const&) const override;

        bool hasPropertyWith(GExpr const &other) const override;
        GExpr propertyWith(GExpr const &other) const override;

        GExpr chisholmIdentity1(
                csl::Index const &mu,
                csl::Index const &a,
                csl::Index const &b
               ) const;
        GExpr chisholmIdentity2(
                csl::Index const &mu,
                csl::Index const &nu,
                csl::Index const &a,
                csl::Index const &b
               ) const;
        GExpr chisholmIdentity2A(
                std::vector<csl::Index> const &rho,
                csl::Index const &a,
                csl::Index const &b
               ) const;
        GExpr chisholmIdentity2B(
                std::vector<csl::Index> const &rho,
                csl::Index const &nu,
                csl::Index const &a,
                csl::Index const &b
                ) const;

        void print(std::ostream &out = std::cout) const override;
    };

    GExpr epsilonindex_s(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &rho,
            csl::Index const &sigma
            );

}
