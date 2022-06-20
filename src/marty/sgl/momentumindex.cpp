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

#include "momentumindex.h"
#include "cslexpr.h"
#include "exceptions.h"
#include "indexchain.h"
#include "tensorset.h"
#include "typecast.h"

namespace sgl {

MomentumIndex::MomentumIndex(csl::Tensor const &t_p, csl::Index const &mu)
    : AbstractGeneralizedIndex(mu), p(t_p)
{
}

bool MomentumIndex::isZero() const
{
    return false;
}

GExpr MomentumIndex::copy() const
{
    return std::make_shared<MomentumIndex>(p, m_indices[0]);
}

GExpr MomentumIndex::refresh() const
{
    return momentumindex_s(p, m_indices[0]);
}

csl::Expr MomentumIndex::toCSL(TensorSet const &) const
{
    csl::Tensor p_cpy = p;
    return p_cpy(m_indices[0]);
}

csl::Expr MomentumIndex::getFactor() const
{
    return CSL_1;
}

GExpr MomentumIndex::getTerm() const
{
    return copy();
}

bool MomentumIndex::hasEOMWith(IndexChain const &chain) const
{
    bool left = chain.getPsiL() && chain.getPsiL()->p == p
                && chain.getPsiL()->isOnShell();
    bool right = chain.getPsiR() && chain.getPsiR()->p == p
                 && chain.getPsiR()->isOnShell();
    if (!left && !right) {
        // No field with a corresponding momentum
        return false;
    }
    // Searching for a common index with a gamma matrix
    constexpr size_t npos     = static_cast<size_t>(-1);
    size_t           posIndex = npos;
    for (size_t i = 0; i != chain.size(); ++i) {
        auto const &arg = chain.argument(i);
        if (arg->indices().size() > 2)
            continue;
        for (const auto &index : arg->indices())
            if (contains(index)) {
                posIndex = i;
                break;
            }
        if (posIndex != npos)
            break;
    }
    if (posIndex == npos)
        return false;

    // Checking that there is no conjugation matrix on the way !
    std::vector<GExpr>::const_iterator first, last;
    if (left) {
        first = chain.begin();
        last  = first + posIndex;
    }
    else {
        first = chain.begin() + posIndex + 1;
        last  = chain.end();
    }
    return !std::any_of(first, last, IndexChain::isC);
}

bool MomentumIndex::hasPropertyWith(GExpr const &other) const
{
    if (IsType<MomentumIndex>(other) && m_indices[0] == other->indices()[0]) {
        return true;
    }
    if (IsType<IndexChain>(other)) {
        auto chain = ConvertTo<IndexChain>(other);
        return hasEOMWith(*chain);
    }
    return false;
}

GExpr MomentumIndex::propertyWith(GExpr const &other) const
{
    if (IsType<MomentumIndex>(other)) {
        // Apply pi.pj = sij
        return cslexpr_s(toCSL(TensorSet{}) * other->toCSL(TensorSet{}));
    }
    else {
        // EOM
        return ConvertTo<IndexChain>(other)->applyEOM(*this);
    }
}

void MomentumIndex::print(std::ostream &out) const
{
    out << p->getName() << "(" << m_indices[0] << ")";
}

GExpr momentumindex_s(csl::Tensor const &p, csl::Index const &mu)
{
    return std::make_shared<MomentumIndex>(p, mu);
}
} // namespace sgl
