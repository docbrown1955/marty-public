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

#include "simplify.h"
#include "cslconverter.h"
#include "debuglog.h"
#include "fierz.h"
#include "indexchain.h"
#include "ordering.h"
#include "sglalgo.h"
#include "sglinterface.h"
#include "sgloperations.h"
#include "sgloptions.h"
#include "tensorset.h"

namespace sgl {

void SimplifyChain(GExpr &init)
{
    SCOPELOG
    LOG("Simplify Chain for", init)
    bool simplified = false;
    sgl::transform<IndexChain>(init, [&simplified](GExpr &sub) {
        auto const          *ptr = dynamic_cast<IndexChain const *>(sub.get());
        std::optional<GExpr> reduced = ptr->reduceStep();
        if (reduced) {
            sub = *reduced;
            SimplifyChain(sub);
            simplified = true;
            return true;
        }
        return false;
    });
    init = sgl::DeepRefreshed(init);
    LOG("Res :", init)
    if (simplified) {
        LOG("Recursive call for Simplify chain")
        SimplifyChain(init);
    }
}

void SimplifyMomenta(GExpr &init)
{
    sgl::transform<sgl::Prod>(init, [](GExpr &sub) {
        std::vector<sgl::MomentumIndex>      p;
        std::vector<size_t>                  ppos;
        std::vector<sgl::IndexChain const *> chains;
        std::vector<size_t>                  cpos;
        for (size_t i = 0; i != sub->size(); ++i) {
            auto const &arg = sub[i];
            if (IsType<MomentumIndex>(arg)) {
                p.push_back(*ConvertTo<MomentumIndex>(arg));
                ppos.push_back(i);
            }
            else if (IsType<IndexChain>(arg)) {
                chains.push_back(ConvertTo<IndexChain>(arg).get());
                cpos.push_back(i);
            }
        }
        if (p.size() < 2 || chains.size() < 1)
            return false;
        for (size_t ic = 0; ic != chains.size(); ++ic) {
            for (size_t ip1 = 0; ip1 != p.size(); ++ip1) {
                for (size_t ip2 = ip1 + 1; ip2 < p.size(); ++ip2) {
                    auto opt = chains[ic]->reduceMomenta(p[ip1], p[ip2]);
                    if (opt) {
                        sub[cpos[ic]]  = cslexpr_s(CSL_1);
                        sub[ppos[ip1]] = cslexpr_s(CSL_1);
                        sub[ppos[ip2]] = cslexpr_s(CSL_1);
                        sub            = sub * opt.value();
                        SimplifyMomenta(sub);
                        return true;
                    }
                }
            }
        }
        return false;
    });
    init = sgl::DeepRefreshed(init);
}

void SimplifyTrace(GExpr &init)
{
    sgl::transform<IndexChain>(init, [](GExpr &sub) {
        auto const *ptr = dynamic_cast<IndexChain const *>(sub.get());
        if (ptr->isTrace()) {
            sub = ptr->calculateTrace();
            return true;
        }
        return false;
    });
    init = sgl::DeepRefreshed(init);
}

bool OrderChainsImpl(GExpr &arg, GExpr const &prod)
{
    auto const &chain = ConvertTo<IndexChain>(arg);
    if (chain->isTrace())
        return false;
    auto conjugation = std::find_if(arg->begin(), arg->end(), IndexChain::isC);
    if (conjugation != arg->end())
        return false;
    for (size_t i = 0; i != arg->size(); ++i) {
        size_t simp = simplest(prod ? prod : cslexpr_s(CSL_UNDEF), arg, i);
        if (simp != i) {
            arg = chain->moveIndex(simp, i);
            return true;
        }
    }
    return false;
}

bool OrderChains(GExpr &init, GExpr const &containerProd)
{
    const size_t sz          = init->size();
    bool         transformed = false;
    bool         isProd      = IsType<sgl::Prod>(init);
    for (size_t i = 0; i != sz; ++i)
        if (OrderChains(init->argument(i), isProd ? init : nullptr))
            transformed = true;
    if (IsType<IndexChain>(init)) {
        transformed = OrderChainsImpl(init, containerProd) || transformed;
    }
    if (transformed)
        init = init->refresh();
    return transformed;
}

void ReorderFermionChains(GExpr &init)
{
    sgl::transform<sgl::IndexChain>(init, [&](GExpr &sub) {
        const auto &chain = ConvertTo<IndexChain>(sub);
        if (chain->isReversed()) {
            auto [sign, newChain] = chain->conjugated();
            sub                   = cslexpr_s(sign) * newChain.copy();
            return true;
        }
        return false;
    });
}

void Simplify(GExpr &init, bool applyFierzTwice)
{
    init = sgl::DeepRefreshed(init);
    SimplifyChain(init);
    SimplifyTrace(init);
    // OrderChains(init);
    SimplifyMomenta(init);
    applyGeneralFierz(init, applyFierzTwice);
    init = sgl::DeepRefreshed(init);
    ReorderFermionChains(init);
    // OrderChains(init);
    SimplifyMomenta(init);
}

GExpr Simplified(GExpr const &init, bool applyFierzTwice)
{
    GExpr copy = sgl::DeepCopy(init);
    Simplify(copy, applyFierzTwice);
    return copy;
}

GExpr CSLSimplified(GExpr const &init)
{
    csl::ScopedProperty permissiveIndices(
        &csl::option::permissiveCovariantIndices, true);
    auto      tensorset = buildTensorSet(minkoSpace, diracSpace);
    csl::Expr expr      = sgl_to_csl(init, tensorset);
    if (!sgl::option::keepEvanescentOperators) {
        csl::Replace(expr, sgl::DMinko, csl::int_s(4));
    }
    csl::DeepRefresh(expr);
    sgl::GExpr simplified = csl_to_sgl(expr, tensorset);
    return sgl::DeepRefreshed(simplified);
}

void CSLSimplify(GExpr &init)
{
    init = CSLSimplified(init);
}

csl::Expr SGLSimplified(csl::Expr const &init, TensorSet const &t)
{
    GExpr expr = csl_to_sgl(init, t);
    sgl::Simplify(expr);
    csl::Expr simplified = sgl_to_csl(expr, t);
    return csl::DeepRefreshed(simplified);
}

void SGLSimplify(csl::Expr &init, TensorSet const &t)
{
    init = SGLSimplified(init, t);
}
} // namespace sgl
