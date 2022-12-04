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

#include "sglcommutation.h"
#include "cslexpr.h"
#include "debuglog.h"
#include "exceptions.h"
#include "indexchain.h"
#include "metricindex.h"

namespace sgl {

std::ostream &operator<<(std::ostream &out, Commutation const &com)
{
    out << "Commutation(" << com.factor << " * " << com.left.copy() << ", "
        << com.right.copy() << "; " << com.remnant << ")";
    return out;
}

Commutation commute(GammaIndex const &mu,
                    GammaIndex const &nu,
                    csl::Index const &a,
                    csl::Index const &b,
                    bool              inverseCall)
{
    SCOPELOG
    LOG("Commuting", mu.copy(), "and", nu.copy())
    auto factor = [&](csl::Expr const &init) {
        return cslexpr_s((inverseCall) ? 1 / init : init);
    };
    auto        sign = [&]() { return cslexpr_s((inverseCall) ? -1 : 1); };
    const GExpr s    = sign();
    if (mu.isSigma() && nu.isSigma()) {
        auto res = Commutation{
            factor(1),
            nu,
            mu,
            2 * s * metricindex_s(mu.indices()[0], nu.indices()[0])
                    * metricindex_s(mu.indices()[1], nu.indices()[1])
                    * indexchain_s(a, b)
                - 2 * s * metricindex_s(mu.indices()[0], nu.indices()[1])
                      * metricindex_s(mu.indices()[1], nu.indices()[0])
                      * indexchain_s(a, b)};
        LOG("Res :", res)
        return res;
    }
    if (mu.isGammaMu() && nu.isSigma()) {
        auto res = Commutation{
            factor(1),
            nu,
            mu,
            2 * s * metricindex_s(mu.indices()[0], nu.indices()[0])
                    * indexchain_s(gammaindex_s(nu.indices()[1]), a, b)
                - 2 * s * metricindex_s(mu.indices()[0], nu.indices()[1])
                      * indexchain_s(gammaindex_s(nu.indices()[0]), a, b)};
        LOG("Res :", res)
        return res;
    }
    if (mu.isGammaMu() && nu.isGammaMu()) {
        auto res
            = Commutation{factor(-1),
                          nu,
                          mu,
                          2 * metricindex_s(mu.indices()[0], nu.indices()[0])
                              * indexchain_s(a, b)};
        LOG("Res :", res)
        return res;
    }
    if (mu.isGammaMu() && nu.isGamma5()) {
        auto res = Commutation{factor(-1), nu, mu, cslexpr_s(CSL_0)};
        LOG("Res :", res)
        return res;
    }
    if (mu.isGammaMu() && nu.isP_L()) {
        auto res = Commutation{factor(1), GammaIndex(7), mu, cslexpr_s(CSL_0)};
        LOG("Res :", res)
        return res;
    }
    if (mu.isGammaMu() && nu.isP_R()) {
        auto res = Commutation{factor(1), GammaIndex(6), mu, cslexpr_s(CSL_0)};
        LOG("Res :", res)
        return res;
    }
    if ((mu.isSigma() && nu.isGamma5()) || (mu.isSigma() && nu.isP_L())
        || (mu.isSigma() && nu.isP_R()) || (mu.isGamma5() && nu.isGamma5())
        || (mu.isGamma5() && nu.isP_L()) || (mu.isGamma5() && nu.isP_R())
        || (mu.isP_L() && nu.isP_L()) || (mu.isP_L() && nu.isP_R())
        || (mu.isP_R() && nu.isP_R())) {
        auto res = Commutation{factor(1), nu, mu, cslexpr_s(CSL_0)};
        LOG("Res :", res)
        return res;
    }
    if (inverseCall)
        throw MathError("Called twice inverse call: would result in infinite recursion");
    LOG("Taking swapped commutation")
    auto res = commute(nu, mu, a, b, true);
    std::swap(res.left, res.right);
    LOG("Swapped res :", res)
    return res;
}
} // namespace sgl
