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

#include "dimensionalRegularization.h"
#include "../../csl/csl.h"
#include "feynmanIntegral.h"
#include "mrtError.h"
#include "mrtOptions.h"
#include "mrtUtils.h"

namespace mty::simpli {

static int getDMinkoPower(csl::Expr const &expr)
{
    if (expr == csl::DMinko) {
        return 1;
    }
    if (csl::IsPow(expr) && expr[0] == csl::DMinko) {
        HEPAssert(csl::IsInteger(expr[1]),
                  mty::error::RuntimeError,
                  "Expected integer power of D, got " + toString(expr));
        return static_cast<int>(expr[1]->evaluateScalar());
    }
    return 0;
}

void addLocalTerms(csl::Expr &res)
{
    bool integral = csl::AnyOfNodes(res, [&](csl::Expr const &sub) {
        return IsOfType<FeynmanIntegral>(sub);
    });
    if (!option::addLocalTerms || !integral) {
        csl::Replace(res, csl::DMinko, csl::int_s(4));
        return;
    }
    csl::DeepRefresh(res);
    auto isEmitter  = [&](csl::Expr const &sub) { return sub == csl::DMinko; };
    auto isReceiver = [&](csl::Expr const &sub) {
        auto ptr = dynamic_cast<FeynmanIntegral const *>(sub.get());
        return ptr && ptr->getDivergentFactor() != CSL_0;
    };
    csl::DeepPartialExpand(res, isEmitter, isReceiver);
    csl::ForEachNode(res, [&](csl::Expr &sub) {
        if (csl::IsProd(sub)) {
            int integral = -1;
            int dpos     = -1;
            int dpower   = 0;
            for (size_t i = 0; i < sub->size(); ++i) {
                if (IsOfType<FeynmanIntegral>(sub[i]))
                    integral = i;
                else if (int power = getDMinkoPower(sub[i]); power > 0) {
                    dpower = power;
                    dpos   = i;
                }
            }
            if (dpos != -1 && integral != -1) {
                sub[dpos] = CSL_1;
                sub[integral]
                    = csl::pow_s(4, dpower)
                      * (sub[integral]
                         + csl::intfraction_s(dpower, 4)
                               * ConvertToPtr<FeynmanIntegral>(sub[integral])
                                     ->getDivergentFactor());
                csl::Refresh(sub);
            }
        }
    });
    csl::Replace(res, csl::DMinko, csl::int_s(4));
}

} // namespace mty::simpli
