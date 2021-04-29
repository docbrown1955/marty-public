#pragma once

#include "generalizedindex.h"

namespace sgl {

    class MetricIndex: public AbstractGeneralizedIndex {

    public:

        MetricIndex(
                csl::Index const &a,
                csl::Index const &b
                );

        bool isZero() const override;

        GExpr copy() const override;
        GExpr refresh() const override;

        csl::Expr toCSL(TensorSet const&) const override;

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

        bool hasPropertyWith(GExpr const &other) const override;
        GExpr propertyWith(GExpr const &other) const override;

        void print(std::ostream &out = std::cout) const override;
    };

    GExpr metricindex_s(csl::Index const &a, csl::Index const &b);
}
