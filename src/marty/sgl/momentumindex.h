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

/**
 * @file momentumindex.h
 * @brief sgl::MomentumIndex, symbolic representation of of momentum in SGL.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "generalizedindex.h"

namespace sgl {

class IndexChain;

class MomentumIndex : public AbstractGeneralizedIndex {

  public:
    MomentumIndex(csl::Tensor const &p, csl::Index const &mu);

    csl::Tensor getP() const
    {
        return p;
    }

    csl::Expr square() const
    {
        auto p_cpy = p;
        return p_cpy(m_indices[0]) * p_cpy(m_indices[0].getFlipped());
    }
    csl::Expr mass() const
    {
        return csl::sqrt_s(square());
    }

    bool isZero() const override;

    GExpr copy() const override;
    GExpr refresh() const override;

    csl::Expr toCSL(TensorSet const &) const override;

    csl::Expr getFactor() const override;
    GExpr     getTerm() const override;

    bool  hasPropertyWith(GExpr const &other) const override;
    GExpr propertyWith(GExpr const &other) const override;

    void print(std::ostream &out = std::cout) const override;

  protected:
    bool hasEOMWith(IndexChain const &chain) const;

  protected:
    csl::Tensor p;
};

GExpr momentumindex_s(csl::Tensor const &p, csl::Index const &mu);
} // namespace sgl
