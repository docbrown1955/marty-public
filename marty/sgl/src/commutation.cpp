#include "commutation.h"
#include "metricindex.h"
#include "indexchain.h"
#include "exceptions.h"
#include "cslexpr.h"
#include "debuglog.h"

namespace sgl {


    std::ostream &operator<<(
            std::ostream &out,
            Commutation const &com
            )
    {
        out << "Commutation(" << com.factor << " * " << com.left.copy() << ", "
            << com.right.copy() << "; " << com.remnant << ")";
        return out;
    }

    Commutation commute(
            GammaIndex const &mu,
            GammaIndex const &nu,
            csl::Index const &a,
            csl::Index const &b,
            bool              inverseCall
            )
    {
        SCOPELOG
        LOG("Commuting", mu.copy(), "and", nu.copy())
        auto factor = [&](csl::Expr const &init) {
            return cslexpr_s((inverseCall) ? 1/init : init);
        };
        auto sign = [&]() { 
            return cslexpr_s((inverseCall) ? -1 : 1);
        };
        const GExpr s = sign();
        if (mu.isSigma() && nu.isSigma()) {
            auto res = Commutation{
                factor(1),
                nu, 
                mu,
                2 * s * metricindex_s(mu.indices()[0], nu.indices()[0])
                    * metricindex_s(mu.indices()[1], nu.indices()[1])
                    * indexchain_s(a, b)
                -2 * s * metricindex_s(mu.indices()[0], nu.indices()[1])
                    * metricindex_s(mu.indices()[1], nu.indices()[0])
                    * indexchain_s(a, b)
            };
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
                -2 * s * metricindex_s(mu.indices()[0], nu.indices()[1])
                    * indexchain_s(gammaindex_s(nu.indices()[0]), a, b)
            };
            LOG("Res :", res)
            return res;
        }
        if (mu.isGammaMu() && nu.isGammaMu()) {
            auto res = Commutation{
                factor(-1),
                nu, 
                mu,
                2 * metricindex_s(mu.indices()[0], nu.indices()[0])
                    * indexchain_s(a, b)
            };
            LOG("Res :", res)
            return res;
        }
        if (mu.isGammaMu() && nu.isGamma5()) {
            auto res = Commutation{
                factor(-1),
                nu, 
                mu,
                cslexpr_s(CSL_0)
            };
            LOG("Res :", res)
            return res;
        }
        if (mu.isGammaMu() && nu.isP_L()) {
            auto res = Commutation{
                factor(1),
                GammaIndex(7), 
                mu,
                cslexpr_s(CSL_0)
            };
            LOG("Res :", res)
            return res;
        }
        if (mu.isGammaMu() && nu.isP_R()) {
            auto res = Commutation{
                factor(1),
                GammaIndex(6), 
                mu,
                cslexpr_s(CSL_0)
            };
            LOG("Res :", res)
            return res;
        }
        if ((mu.isSigma() && nu.isGamma5())
                || (mu.isSigma() && nu.isP_L())
                || (mu.isSigma() && nu.isP_R())
                || (mu.isGamma5() && nu.isGamma5())
                || (mu.isGamma5() && nu.isP_L())
                || (mu.isGamma5() && nu.isP_R())
                || (mu.isP_L() && nu.isP_L())
                || (mu.isP_L() && nu.isP_R())
                || (mu.isP_R() && nu.isP_R())) {
            auto res = Commutation{
                factor(1),
                nu, 
                mu,
                cslexpr_s(CSL_0)
            };
            LOG("Res :", res)
            return res;
        }
        if (inverseCall)
            throw Exception::MathError;
        LOG("Taking swapped commutation")
        auto res = commute(nu, mu, a, b, true);
        std::swap(res.left, res.right);
        LOG("Swapped res :", res)
        return res;
    }
}
