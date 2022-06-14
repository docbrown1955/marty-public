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

 #include "ordering.h"
#include "typecast.h"
#include "tensorset.h"
#include "indexchain.h"
#include "gammaindex.h"
#include "momentumindex.h"
#include "sglfield.h"
#include "exceptions.h"
#include "debuglog.h"

namespace sgl {

    static bool hasCommonIndex(
            GExpr const &arg,
            GammaIndex const &mu
            )
    {
        auto const &indices = mu.indices();
        return std::any_of(
                indices.begin(), 
                indices.end(), 
                [&](csl::Index const &i) {
                    return arg->contains(i);
                });
    }

    bool freeOrder(
            std::vector<csl::Index> const &mu,
            std::vector<csl::Index> const &nu
            )
    {
        const size_t mu_size = mu.size();
        const size_t nu_size = nu.size();
        if (mu_size != nu_size)
            return mu_size > nu_size;
        if (mu_size == 1) {
            const bool mu_fixed = (mu[0].getType() == cslIndex::Fixed);
            const bool nu_fixed = (nu[0].getType() == cslIndex::Fixed);
            if (mu_fixed != nu_fixed)
                return nu_fixed;
        }
        return csl::IndexStructure(mu) > csl::IndexStructure(nu);
    }

    bool contractedOrder(
            GExpr const &A,
            GExpr const &B
            )
    {
        if (IsType<IndexChain>(A) || IsType<GammaIndex>(A)
                || IsType<IndexChain>(B) || IsType<GammaIndex>(B)) {
            std::cerr << "Contracted Order received a GammaIndex or an ";
            std::cerr << "IndexChain, should'nt." << std::endl;
            std::cerr << A << std::endl;
            std::cerr << B << std::endl;
            throw Exception::MathError;
        }
        // The tensorset is guaranteed to be useless, default constructed
        TensorSet tset;
        return A->toCSL(tset) < B->toCSL(tset);
    }

    bool isOrdered(
            GExpr      const &prod,
            GammaIndex const &mu,
            GammaIndex const &nu
            )
    {
        constexpr size_t npos = -1;
        size_t i_mu = npos;
        size_t i_nu = npos;
        for (size_t i = 0; i != prod->size(); ++i) {
            auto const &arg = prod[i];
            if (IsType<IndexChain>(arg))
                continue;
            if (hasCommonIndex(arg, mu))
                i_mu = i;
            if (hasCommonIndex(arg, nu))
                i_nu = i;
        }
        if (i_mu == npos && i_nu == npos) {
            // Both free indices
            return freeOrder(mu.indices(), nu.indices());
        }
        if (i_mu == npos) {
            // mu is free, should be on the right
            return false;
        }
        if (i_nu == npos) {
            // nu is free, is indeed on the right
            return true;
        }
        // mu and nu are contrated
        return contractedOrder(prod[i_mu], prod[i_nu]);
    }

    size_t simplest(
            GExpr const &prod,
            GExpr const &chain,
            size_t       begin
            )
    {
        size_t simplest = begin;
        for (size_t i = begin+1; i < chain->size(); ++i) {
            auto const &A = *ConvertTo<GammaIndex>(chain[simplest]);
            auto const &B = *ConvertTo<GammaIndex>(chain[i]);
            if (!isOrdered(prod, A, B)) {
                std::cout << B.copy() << " simpler than " << A.copy() << " !" << '\n';
                simplest = i;
            }
        }
        return simplest;
    }

}
