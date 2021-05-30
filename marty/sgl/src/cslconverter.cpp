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

#include "cslconverter.h"
#include "tensorset.h"
#include "exceptions.h"
#include "sgloperations.h"
#include "sglinterface.h"
#include "cslexpr.h"
#include "sglfield.h"
#include "indexchain.h"
#include "gammaindex.h"
#include "metricindex.h"
#include "epsilonindex.h"
#include "momentumindex.h"
#include "debuglog.h"
#include "mrtUtils.h"
#include "quantumField.h"
#include "polarization.h"

namespace sgl {

    GExpr csl_simplified(
            GExpr const &expr,
            TensorSet const &tensors
            )
    {
        return sgl::DeepRefreshed(
                csl_to_sgl(
                    csl::DeepRefreshed(
                        sgl_to_csl(
                            expr,
                            tensors
                            )
                        ),
                    tensors
                    )
                );
    }

    csl::Expr sgl_to_csl(
            GExpr const &expr,
            TensorSet const &tensors
            )
    {
        SCOPELOG
        LOG("SGL to CSL")
        LOG(expr)
        LOG(expr->toCSL(tensors))
        return expr->toCSL(tensors);
    }

    GExpr csl_to_sgl(
            csl::Expr const &expr,
            TensorSet const &tensors
            )
    {
        SCOPELOG
        LOG("CSL to SGL")
        LOG(expr)
        if (expr->size() > 0) {
            std::vector<GExpr> args(expr->size());
            for (size_t i = 0; i != args.size(); ++i) 
                args[i] = csl_to_sgl(expr[i], tensors);
            if (csl::IsSum(expr)) {
                LOG(sgl::sum_s(args))
                return sgl::sum_s(args);
            }
            if (csl::IsProd(expr)) {
                LOG(sgl::prod_s(args))
                return sgl::prod_s(args);
            }
            if (csl::IsPow(expr)) {
                return sgl::cslexpr_s(expr);
            }
        }
        if (!csl::IsIndicialTensor(expr)) {
            LOG(cslexpr_s(expr))
            return cslexpr_s(expr);
        }
        auto ptr = dynamic_cast<mty::QuantumField const*>(expr.get());
        if (ptr) {
            if (ptr->getQuantumParent()->isFermionic()) {
                auto partnerShip = ptr->getPartnerShip();
                auto res = indexchain_s(Field(
                            expr, 
                            ptr->isParticle(),
                            ptr->isIncoming(),
                            ptr->isOnShell(),
                            {partnerShip.self, partnerShip.partner, partnerShip.isLeft}
                            ));
                LOG("Res : ", res)
                return res;
            }
            else
                return cslexpr_s(expr);
        }
        auto parent = expr->getParent();
        if (parent == tensors.gamma_chir)
        {
            LOG(indexchain_s(
                    gammaindex_s(5), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    ))
            return indexchain_s(
                    gammaindex_s(5), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    );
        }
        if (parent == tensors.P_L)
        {
            LOG(indexchain_s(
                    gammaindex_s(6), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    ))
            return indexchain_s(
                    gammaindex_s(6), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    );
        }
        if (parent == tensors.P_R)
        {
            LOG(indexchain_s(
                    gammaindex_s(7), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    ))
            return indexchain_s(
                    gammaindex_s(7), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    );
        }
        if (parent == tensors.C)
        {
            LOG(indexchain_s(
                    gammaindex_s(8), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    ))
            return indexchain_s(
                    gammaindex_s(8), 
                    expr->getIndex(0), 
                    expr->getIndex(1)
                    );
        }
        for (const auto iter : tensors.gamma)
            if (iter.second == parent) {
                std::vector<csl::Index> minkoIndices 
                    = expr->getIndexStructureView().getIndex();
                const size_t nI = expr->getIndexStructureView().size();
                csl::Index a = expr->getIndexStructureView()[nI-2];
                csl::Index b = expr->getIndexStructureView()[nI-1];
                minkoIndices.erase(minkoIndices.end() - 2, minkoIndices.end());
                LOG(indexchain_s(
                        gammaindex_s(minkoIndices),
                        a,
                        b
                        ))
                return indexchain_s(
                        gammaindex_s(minkoIndices),
                        a,
                        b
                        );
            }
        if (parent == minkoSpace->getDelta()
                || parent == minkoSpace->getMetric()) {
            LOG(metricindex_s(expr->getIndex(0), expr->getIndex(1)))
            return metricindex_s(expr->getIndex(0), expr->getIndex(1));
        }
        if (parent == minkoSpace->getEpsilon()) {
            LOG(epsilonindex_s(
                    expr->getIndex(0), 
                    expr->getIndex(1), 
                    expr->getIndex(2), 
                    expr->getIndex(3)
                    ))
            return epsilonindex_s(
                    expr->getIndex(0), 
                    expr->getIndex(1), 
                    expr->getIndex(2), 
                    expr->getIndex(3)
                    );
        }
        if (parent->getSpace().size() == 1 
                && parent->getSpace()[0] == minkoSpace) {
            LOG(momentumindex_s(
                    std::dynamic_pointer_cast<csl::TensorParent>(parent), 
                    expr->getIndexStructureView()[0]))
            return momentumindex_s(
                    std::dynamic_pointer_cast<csl::TensorParent>(parent), 
                    expr->getIndexStructureView()[0]);
        }
        LOG(cslexpr_s(expr))
        return cslexpr_s(expr);
    }
}
