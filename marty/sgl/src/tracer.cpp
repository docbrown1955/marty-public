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

 #include "tracer.h"
#include "cslexpr.h"
#include "gammaindex.h"
#include "metricindex.h"
#include "exceptions.h"
#include "epsilonindex.h"
#include "debuglog.h"
#include "sgloperations.h"

namespace sgl::tracer {

    static std::vector<csl::Index> gammas(
            std::vector<GammaIndex> const &indices
            )
    {
        std::vector<csl::Index> gamma;
        gamma.reserve(indices.size());
        for (const auto &i : indices)
            gamma.insert(gamma.end(), i.indices().begin(), i.indices().end());
        return gamma;
    }

    static bool isAntiSymmetric(
            std::size_t i,
            std::size_t j,
            std::vector<GammaIndex> const &indices
            )
    {
        if (j < i)
            return isAntiSymmetric(j, i, indices);
        std::size_t n = 0;
        bool i_found = false;
        for (const auto &genindex : indices) {
            for ([[maybe_unused]]const auto &index : genindex.indices()) {
                if (n == i) {
                    i_found = true;
                }
                if (i_found && n == j) {
                    return true;
                }
                ++n;
            }
            if (i_found)
                break;
        }
        return false;
    }

    static void cutIndex(
            std::vector<GammaIndex> &init,
            std::size_t i
            )
    {
        std::size_t n = 0;
        for (std::size_t k = 0; k != init.size(); ++k) {
            auto &genIndex = init[k];
            if (i < n + genIndex.indices().size()) {
                genIndex.indices().erase(genIndex.indices().begin() + i - n);
                if (genIndex.indices().empty())
                    init.erase(init.begin() + k);
                return;
            }
            n += genIndex.indices().size();
        }
        throw Exception::MathError;
    }

    static void cutIndices(
            std::vector<GammaIndex> &init,
            std::size_t i,
            std::size_t j
            )
    {
        if (j < i) {
            cutIndices(init, j, i);
            return;
        }
        cutIndex(init, j);
        cutIndex(init, i);
    }

    static void cutIndex(
            std::vector<GammaIndex> &init,
            std::size_t i,
            std::size_t j
            )
    {
        if (init[i].indices().size() == 1) {
            init.erase(init.begin() + i);
        }
        else {
            init[i].indices().erase(init[i].indices().begin() + j);
        }
    }

    static std::vector<GammaIndex> cutIndices(
            std::vector<GammaIndex> const &init,
            std::size_t i1,
            std::size_t j1,
            std::size_t i2,
            std::size_t j2
            )
    {
        if (i2 < i1)
            return cutIndices(init, i2, j2, i1, j1);
        auto res = init;
        cutIndex(res, i2, j2);
        cutIndex(res, i1, j1);
        return res;
    }

    [[maybe_unused]] static 
    std::string toString(
            std::vector<GammaIndex> const &v
            )
    {
        std::ostringstream out;
        for (const auto i : v) {
            i.print(out);
            out << ' ';
        }
        return out.str();
    }

    GExpr standardTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            )
    {
        SCOPELOG
        LOG("Standard Trace for", toString(indices))
        auto gam = gammas(indices);
        const std::size_t N = gam.size();
        LOG(N, "gamma matrices")
        if (N == 0)
            return cslexpr_s(Ddirac);
        if (N % 2 == 1 || indices.size() == 1)
            return cslexpr_s(CSL_0);
        if (N == 2) {
            LOG("Res :")
            LOG(4*metricindex_s(indices[0].indices()[0], indices[1].indices()[0]))
            return 4 * metricindex_s(
                    indices[0].indices()[0],
                    indices[1].indices()[0]);
        }
        std::vector<GExpr> terms;
        terms.reserve(N);
        int sign = (indices[0].indices().size() & 1) ? 1 : -1;
        for (std::size_t i = 1; i != indices.size(); ++i) {
            for (std::size_t l = 0; l != indices[i].indices().size(); ++l) {
                GExpr g = metricindex_s(
                        indices[0].indices()[0],
                        indices[i].indices()[l]
                        );
                auto newIndex = cutIndices(
                        indices,
                        0, 0,
                        i, l
                        );
                LOG("New term :", cslexpr_s(sign) * g);
                terms.push_back(
                        cslexpr_s(sign) * g * standardTrace(newIndex, Ddirac)
                        );
                sign *= -1;
            }
        }
        LOG("Final res :", sgl::sum_s(terms))
        return sgl::sum_s(terms);
    }

    GExpr chiralTrace(
            std::vector<GammaIndex> const &indices,
            csl::Expr               const &Ddirac
            )
    {
        SCOPELOG
        LOG("Chiral Trace for", toString(indices))
        const std::vector<csl::Index> gam = gammas(indices);
        const std::size_t N = gam.size();
        const static auto zero = cslexpr_s(CSL_0);
        if (N % 2 == 1 || N < 4)
            return zero;
        if (N == 4) {
            return -Ddirac * CSL_I 
                * epsilonindex_s(gam[0], gam[1], gam[2], gam[3]);
        }
        std::vector<GExpr> terms;
        terms.reserve(N);
        for (std::size_t i = 0; i != gam.size()-1; ++i) {
            int globalSign = (i%4 >= 2) ? -1 : 1;
            for (std::size_t j = i+1; j != gam.size(); ++j) {
                if (isAntiSymmetric(i, j, indices))
                    continue;
                int sign = ((i+j+1)%2 == 1) ? -globalSign : globalSign;
                std::vector<GammaIndex> recursiveMinkoIndices(indices);
                cutIndices(recursiveMinkoIndices, i, j);
                terms.push_back(
                            sgl::prod_s({
                                cslexpr_s(sign),
                                metricindex_s(gam[i], gam[j]),
                                chiralTrace(recursiveMinkoIndices, Ddirac)
                                })
                            );
                LOG("New term :", terms.back())
            }
        }
        LOG("Final res :", sgl::sum_s(terms))
        return sgl::sum_s(terms);
    }
}
