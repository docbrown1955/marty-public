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

#include "wilsonOperator.h"
#include "model.h"
#include "error.h"

namespace mty {

    csl::Expr getWilsonCoefficient(
            WilsonSet           const &wilsons,
            std::vector<Wilson> const &contributions
            )
    {
        std::vector<csl::Expr> terms;
        terms.reserve(wilsons.size());
        for (const auto &wil : wilsons) {
            for (const auto &contrib : contributions) {
                if (wil.op == contrib.op) 
                    terms.push_back(contrib.coef.getCoefficient() 
                                  * wil    .coef.getCoefficient());
            }
        }
        return csl::sum_s(terms);
    }

    csl::Expr getWilsonCoefficient(
            WilsonSet              const &wilsons,
            std::vector<csl::Expr> const &contributions
            )
    {
        std::vector<Wilson> wilcontrib(contributions.size());
        for (size_t i = 0; i != contributions.size(); ++i) {
            wilcontrib[i] = {CSL_1, contributions[i]};
        }
        return getWilsonCoefficient(wilsons, wilcontrib);
    }

    csl::Expr getWilsonCoefficient(
            WilsonSet const &wilsons,
            Wilson    const &contributions
            )
    {
        return getWilsonCoefficient(
                wilsons, 
                std::vector<Wilson>{contributions});
    }

    csl::Expr getWilsonCoefficient(
            WilsonSet const &wilsons,
            csl::Expr const &contributions
            )
    {
        return getWilsonCoefficient(
                wilsons,
                std::vector<csl::Expr>{contributions}
                );
    }


    csl::Expr getWilsonCoefficient(
            Model     const &model,
            WilsonSet const &wilsons,
            DiracCoupling coupling
            )
    {
        return getWilsonCoefficient(
                wilsons,
                chromoMagneticOperator(model, wilsons, coupling)
                );
    }

} // End of namespace mty
