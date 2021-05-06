// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

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
