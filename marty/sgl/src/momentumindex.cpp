#include "momentumindex.h"
#include "typecast.h"
#include "exceptions.h"
#include "cslexpr.h"
#include "tensorset.h"
#include "indexchain.h"

namespace sgl {

    MomentumIndex::MomentumIndex(
            csl::Tensor const &t_p,
            csl::Index  const &mu
            )
        :AbstractGeneralizedIndex(mu),
        p(t_p)
    {}

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

    csl::Expr MomentumIndex::toCSL(TensorSet const&) const
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
        bool left = chain.getPsiL()
                 && chain.getPsiL()->p == p 
                 && chain.getPsiL()->isOnShell();
        bool right = chain.getPsiR()
                  && chain.getPsiR()->p == p 
                  && chain.getPsiR()->isOnShell();
        if (!left && !right) {
            // No field with a corresponding momentum
            return false;
        }
        // Searching for a common index with a gamma matrix
        constexpr size_t npos = static_cast<size_t>(-1);
        size_t posIndex = npos;
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
        if (IsType<MomentumIndex>(other)
                && m_indices[0] == other->indices()[0]) {
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
            return cslexpr_s(toCSL(TensorSet{})*other->toCSL(TensorSet{}));
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

    GExpr momentumindex_s(
            csl::Tensor const &p,
            csl::Index  const &mu
            )
    {
        return std::make_shared<MomentumIndex>(p, mu);
    }
}
