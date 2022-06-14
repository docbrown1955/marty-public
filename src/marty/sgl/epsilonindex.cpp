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

 #include "epsilonindex.h"
#include "metricindex.h"
#include "exceptions.h"
#include "sgloperations.h"
#include "indexchain.h"
#include "contraction.h"
#include "typecast.h"
#include "cslexpr.h"
#include "debuglog.h"

namespace sgl {

    EpsilonIndex::EpsilonIndex(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &rho,
            csl::Index const &sigma
            )
        :AbstractGeneralizedIndex({mu, nu, rho, sigma})
    {

    }

    bool EpsilonIndex::isZero() const 
    {
        return hasSelfContraction();
    }

    GExpr EpsilonIndex::copy() const 
    {
        return std::make_shared<EpsilonIndex>(
                m_indices[0],
                m_indices[1],
                m_indices[2],
                m_indices[3]
                );
    }

    GExpr EpsilonIndex::refresh() const
    {
        return epsilonindex_s(
                m_indices[0],
                m_indices[1],
                m_indices[2],
                m_indices[3]
                );
    }

    csl::Expr EpsilonIndex::getFactor() const
    {
        if (isZero())
            return CSL_0;
        return CSL_1;
    }

    GExpr EpsilonIndex::getTerm() const 
    {
        return (isZero() ? CSL_0 : copy());
    }

    csl::Expr EpsilonIndex::toCSL(TensorSet const &) const
    {
        return m_indices[0].getSpace()->getEpsilon()(m_indices);
    }

    bool EpsilonIndex::hasPropertyWith(GExpr const &other) const
    {
        if (!(IsType<IndexChain>(other) || IsType<EpsilonIndex>(other)))
            return false;
        for (const auto &selfIndex : m_indices)
            if (other->contains(selfIndex))
                return true;
        return false;
    }

    GExpr EpsilonIndex::chisholmIdentity1(
            csl::Index const &mu,
            csl::Index const &a,
            csl::Index const &b
            ) const
    {
        SCOPELOG
        LOG("Chisholm idendity for", mu, "in", copy(), ": indices", a, b)
        size_t i_mu = 0;
        std::vector<csl::Index> nu;
        for (size_t i = 0; i != m_indices.size(); ++i)  {
            if (m_indices[i] == mu) {
                i_mu = i;
            }
            else {
                nu.push_back(m_indices[i]);
            }
        }
        if (nu.size() != 3) {
            LOG("Less that three other indices...")
            throw Exception::MathError;
        }
        int sign = ((4 - i_mu) & 1) ? 1 : -1;
        GExpr factor = cslexpr_s(-sign*CSL_I);
        LOG("Factor :", factor)
        std::vector<GExpr> terms;
        terms.reserve(4);
        terms.push_back(indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s(nu[0]),
                        gammaindex_s(nu[1]), 
                        gammaindex_s(nu[2]), 
                        gammaindex_s(5)
                    },
                    a, b));
        terms.push_back(
                -metricindex_s(nu[1], nu[2])
                * indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({nu[0]}), 
                        gammaindex_s(5)
                    },
                    a, b));
        terms.push_back(
                metricindex_s(nu[0], nu[2])
                * indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({nu[1]}), 
                        gammaindex_s(5)
                    },
                    a, b));
        terms.push_back(
                -metricindex_s(nu[0], nu[1])
                * indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({nu[2]}), 
                        gammaindex_s(5)
                    },
                    a, b));
        LOG("Structure res :", sgl::sum_s(terms))
        LOG("End Chisholm")
        return factor * sgl::sum_s(terms);
    }


    GExpr EpsilonIndex::chisholmIdentity2(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &a,
            csl::Index const &b
            ) const
    {
        SCOPELOG
        LOG("Chisholm idendity for", mu, "and", nu, "in", copy(), ": indices", a, b)
        size_t i_mu = -1;
        size_t i_nu = -1;
        std::vector<csl::Index> rho;
        for (size_t i = 0; i != m_indices.size(); ++i)  {
            if (m_indices[i] == mu) {
                i_mu = i;
            }
            else if (m_indices[i] == nu) {
                i_nu = i;
            }
            else {
                rho.push_back(m_indices[i]);
            }
        }
        if (rho.size() != 2) {
            if (rho.size() != 3) {
                LOG("Not two indices ...")
                throw Exception::MathError;
            }
            const size_t m_one = static_cast<size_t>(-1);
            int sign = (i_mu == m_one) ? -1 : 1;
            const size_t i = (i_mu != m_one) ? i_mu : i_nu;
            const csl::Index tau = (i_mu != m_one) ? nu : mu;
            if (!(i & 1))
                sign *= -1;
            return sign*chisholmIdentity2B(rho, tau, a, b);
        }
        int sign = (i_nu & 1) ?  1 : -1;
        sign *=    (i_mu & 1) ? -1 :  1;
        if (i_mu < i_nu)
            sign *= -1;
        return sign * chisholmIdentity2A(rho, a, b);
    }

    GExpr EpsilonIndex::chisholmIdentity2A(
            std::vector<csl::Index> const &rho,
            csl::Index const &a,
            csl::Index const &b
            ) const
    {
        LOG("Factor :", cslexpr_s(-CSL_I*(sgl::DMinko - 2)))
        LOG("Structure res: ", (
                    indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({rho[0], rho[1]}),
                        gammaindex_s(5)
                    },
                    a, b
                    )))
        return cslexpr_s(-CSL_I*(sgl::DMinko - 2))
            * indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({rho[0], rho[1]}),
                        gammaindex_s(5)
                    },
                    a, b
                    );
    }

    GExpr EpsilonIndex::chisholmIdentity2B(
            std::vector<csl::Index> const &rho,
            csl::Index const &nu,
            csl::Index const &a,
            csl::Index const &b
            ) const
    {
        LOG("Factor :", cslexpr_s(-CSL_I))
        LOG("Structure res: ", (
                    metricindex_s(nu, rho[2])
                    *indexchain_s(
                    std::vector<GExpr>{
                        gammaindex_s({rho[0], rho[1]}),
                        gammaindex_s(5)
                    },
                    a, b
                    )))
        auto const f = cslexpr_s(-CSL_I);
        std::vector<GExpr> terms;
        terms.reserve(5);
        terms.push_back(
                f*metricindex_s(rho[0], rho[1])*indexchain_s(
                    {gammaindex_s({rho[2], nu}),
                    gammaindex_s(5)},
                    a, b
                    )
                );
        terms.push_back(
                -f*metricindex_s(rho[0], rho[2])*indexchain_s(
                    {gammaindex_s({rho[1], nu}),
                    gammaindex_s(5)},
                    a, b
                    )
                );
        terms.push_back(
                f*metricindex_s(rho[1], rho[2])*indexchain_s(
                    {gammaindex_s({rho[0], nu}),
                    gammaindex_s(5)},
                    a, b
                    )
                );
        terms.push_back(
                f/2 * indexchain_s(
                    {gammaindex_s(rho[0]),
                    gammaindex_s(rho[1]),
                    gammaindex_s(rho[2]),
                    gammaindex_s(nu)},
                    a, b
                    )
                );
        terms.push_back(
                -f/2 * indexchain_s(
                    {gammaindex_s(nu),
                    gammaindex_s(rho[0]),
                    gammaindex_s(rho[1]),
                    gammaindex_s(rho[2])},
                    a, b
                    )
                );
        return sgl::sum_s(terms);
    }

    GExpr EpsilonIndex::propertyWith(GExpr const &other) const
    {
        if (IsType<EpsilonIndex>(other)) {
            return epsilonContraction(
                    indices(),
                    other->indices()
                    );
        }
        std::shared_ptr<IndexChain> indexChain = ConvertTo<IndexChain>(other);
        for (size_t i = 0; i != indexChain->size(); ++i) {
            auto const &arg = indexChain->argument(i);
            for (const auto &index : arg->indices())
                if (contains(index)) {
                    auto const &indices = arg->indices();
                    std::pair<GExpr, IndexChain> cut = indexChain->cut(i);
                    auto [a, b] = cut.second.getBorderIndices();
                    if (indices.size() == 1)
                        return cut.first * chisholmIdentity1(indices[0], a, b);
                    else if (indices.size() == 2)
                        return cut.first * chisholmIdentity2(indices[0], indices[1], a, b);
                }
        }
        throw Exception::MathError;
    }

    void EpsilonIndex::print(std::ostream &out) const
    {
        out << "eps[";
        out << m_indices[0] << " " << m_indices[1] << " ";
        out << m_indices[2] << " " << m_indices[3];
        out << "]";
    }

    GExpr epsilonindex_s(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &rho,
            csl::Index const &sigma
            )
    {
        auto index = std::make_shared<EpsilonIndex>(mu, nu, rho, sigma);
        if (index->isZero())
            return cslexpr_s(CSL_0);
        return index;
    }
}
