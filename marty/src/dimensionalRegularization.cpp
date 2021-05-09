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
#include "feynmanIntegral.h"
#include "mrtError.h"
#include "mrtUtils.h"
#include "mrtOptions.h"
#include <csl.h>

namespace mty::simpli {


    void addLocalTerms(csl::Expr &res)
    {
        bool integral = csl::AnyOfNodes(res, [&](csl::Expr const &sub)
        {
            return IsOfType<FeynmanIntegral>(sub);
        });
        if (!option::addLocalTerms || !integral) {
            csl::Replace(res, csl::DMinko, csl::int_s(4));
            return;
        }
        csl::DeepRefresh(res);
        auto isEmitter = [&](csl::Expr const &sub) {
            return sub == csl::DMinko;
        };
        auto isReceiver = [&](csl::Expr const &sub) {
            auto ptr = dynamic_cast<FeynmanIntegral const*>(sub.get());
            return ptr && ptr->getDivergentFactor() != CSL_0;
        };
        csl::DeepPartialExpand(res, isEmitter, isReceiver);
        csl::ForEachNode(res, [&](csl::Expr &sub) {
            if (csl::IsProd(sub)) {
                int integral = -1;
                int dpos = -1;
                for (size_t i = 0; i < sub->size(); ++i) {
                    if (IsOfType<FeynmanIntegral>(sub[i]))
                        integral = i;
                    else if (sub[i] == csl::DMinko)
                        dpos = i;
                }
                if (dpos != -1 && integral != -1) {
                    sub[dpos] = CSL_1;
                    sub[integral] = 4*sub[integral] + 
                        ConvertToPtr<FeynmanIntegral>(sub[integral])
                            ->getDivergentFactor();
                    csl::Refresh(sub);
                }
            }
        });
    }

}
