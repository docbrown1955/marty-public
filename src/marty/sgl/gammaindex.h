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
 * @file gammaindex.h
 * @brief Gamma matrices symbolic expression for SGL.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "../core/diracology.h"
#include "generalizedindex.h"

namespace sgl {

inline csl::Space const *        minkoSpace = &csl::Minkowski;
inline csl::Expr                 DMinko     = csl::DMinko;
inline csl::Space const *        diracSpace = &mty::dirac4;
inline std::map<int, csl::Index> m_easyIndex;

class GammaIndex : public AbstractGeneralizedIndex {

  public:
    GammaIndex();
    GammaIndex(csl::Index const &t_index);
    GammaIndex(std::vector<csl::Index> const &t_indices);

    bool      isZero() const override;
    csl::Expr getFactor() const override;
    GExpr     getTerm() const override;

    bool isDelta() const
    {
        return m_indices.empty();
    }
    bool isGammaMu() const
    {
        return m_indices.size() == 1
               && (m_indices[0].getType() != cslIndex::Fixed
                   || m_indices[0].getValue() < diracSpace->getDim());
    }
    bool isSigma() const
    {
        return m_indices.size() == 2;
    }
    bool isGamma5() const
    {
        return m_indices.size() == 1
               && m_indices[0].getType() == cslIndex::Fixed
               && m_indices[0] == 5;
    }
    bool isP_L() const
    {
        return m_indices.size() == 1
               && m_indices[0].getType() == cslIndex::Fixed
               && m_indices[0] == 6;
    }
    bool isP_R() const
    {
        return m_indices.size() == 1
               && m_indices[0].getType() == cslIndex::Fixed
               && m_indices[0] == 7;
    }
    bool isChiral() const
    {
        return isGamma5() || isP_L() || isP_R();
    }
    bool isC() const
    {
        return m_indices.size() == 1
               && m_indices[0].getType() == cslIndex::Fixed
               && m_indices[0] == 8;
    }

    bool leftOf(GammaIndex const &other) const;

    GExpr copy() const override;
    GExpr refresh() const override;

    csl::Expr buildTensor(TensorSet const & tensors,
                          csl::Index const &a,
                          csl::Index const &b) const;

    csl::Expr toCSL(TensorSet const &) const override;

    void print(std::ostream &out = std::cout) const override;

    bool equalTo(GammaIndex const &other) const;
};

GExpr gammaindex_s();
GExpr gammaindex_s(csl::Index const &index);
GExpr gammaindex_s(std::vector<csl::Index> const &indices);
} // namespace sgl
