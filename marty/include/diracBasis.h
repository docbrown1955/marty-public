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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#pragma once

#include "diracology.h"

namespace mty {

struct DiracPoint {
    csl::Expr factor;
    csl::Expr gamma;
};

struct BiDiracPoint {
    csl::Expr factor;
    csl::Expr gammaL;
    csl::Expr gammaR;
};

class DiracBasis {
public:
    virtual size_t size() const = 0;
    virtual csl::Expr getFactor() const = 0;
    virtual std::vector<csl::Index> addElement(
            size_t                   i, 
            std::vector<csl::Index> &tensors
            ) const = 0;
    virtual csl::Expr getConjugateExpr(
            size_t                         i,
            std::vector<csl::Index> const &mu,
            csl::Index              const &a, 
            csl::Index              const &b
            ) const = 0;
};

class StandardBasis: public DiracBasis {

public:
    size_t size() const override { return 5; }
    csl::Expr getFactor() const override { return csl::int_s(4); }
    std::vector<csl::Index> addElement(
            size_t                   i, 
            std::vector<csl::Index> &tensors
            ) const override;
    csl::Expr getConjugateExpr(
            size_t                         i,
            std::vector<csl::Index> const &mu,
            csl::Index              const &a, 
            csl::Index              const &b
            ) const override;
};

class ChiralBasis: public DiracBasis {

public:
    size_t size() const override { return 5; }
    csl::Expr getFactor() const override { return csl::int_s(2); }
    std::vector<csl::Index> addElement(
            size_t                   i, 
            std::vector<csl::Index> &tensors
            ) const override;
    csl::Expr getConjugateExpr(
            size_t                         i,
            std::vector<csl::Index> const &mu,
            csl::Index              const &a, 
            csl::Index              const &b
            ) const override;
};

inline csl::Expr pointToExpr(DiracPoint const &p)
{
    return p.factor * p.gamma;
}
inline csl::Expr pointToExpr(std::vector<DiracPoint> const &pts)
{
    std::vector<csl::Expr> args;
    args.reserve(pts.size());
    for (const auto &p : pts) {
        args.push_back(pointToExpr(p));
    }
    return csl::Factored(csl::DeepExpanded(csl::DeepExpanded(csl::sum_s(args))));
}

inline csl::Expr pointToExpr(BiDiracPoint const &p)
{
    return csl::DeepRefreshed(p.factor * p.gammaL * p.gammaR);
}
inline csl::Expr pointToExpr(std::vector<BiDiracPoint> const &pts)
{
    std::vector<csl::Expr> args;
    args.reserve(pts.size());
    for (const auto &p : pts) {
        args.push_back(pointToExpr(p));
    }
    return csl::Factored(csl::DeepExpanded(csl::DeepExpanded(csl::sum_s(args))));
}

std::vector<DiracPoint> projectOnStandardBasis(
        std::vector<csl::Index> const &tensors,
        csl::Index              const &a,
        csl::Index              const &b
        );

std::vector<DiracPoint> projectOnChiralBasis(
        std::vector<csl::Index> const &tensors,
        csl::Index              const &a,
        csl::Index              const &b
        );

std::vector<BiDiracPoint> applyFiertz(
        std::vector<csl::Index> const &tLeft,
        std::vector<csl::Index> const &tRight,
        csl::Index              const &aL,
        csl::Index              const &bL,
        csl::Index              const &aR,
        csl::Index              const &bR,
        DiracBasis              const &basis
        );

std::vector<BiDiracPoint> applyFiertzTwice(
        std::vector<csl::Index> const &tLeft,
        std::vector<csl::Index> const &tRight,
        csl::Index              const &aL,
        csl::Index              const &bL,
        csl::Index              const &aR,
        csl::Index              const &bR,
        DiracBasis              const &basis
        );

struct GeneralizedIndex {

    IMPLEMENTS_STD_VECTOR(csl::Index, indices)

    bool isEmpty() const;
    bool isZero() const;

    bool contains(csl::Index const &index) const {
        return std::find(begin(), end(), index) != end();
    }

    std::vector<csl::Index> indices;
};

struct MetricTerm {

    bool applyOn(GeneralizedIndex &Gamma) const;

    csl::Index first;
    csl::Index second;
};

struct DiracChain {

    bool isZero() const {
        return factor == CSL_0;
    }

    void nulliFy();
    void applyMetrics();
    void refresh();

    std::vector<DiracChain> contractInnerIndices() const;

    csl::Expr                     factor;
    std::vector<MetricTerm>       g;
    std::vector<GeneralizedIndex> eps;
    std::vector<GeneralizedIndex> gamma;
};

} // namespace mty
