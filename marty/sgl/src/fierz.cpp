#include "fierz.h"
#include "indexchain.h"
#include "operations.h"
#include "simplify.h"
#include "interface.h"
#include "algo.h"
#include "sgloptions.h"

namespace sgl {

    void fixFierz(GExpr &expr)
    {
        sgl::for_each<IndexChain>(expr, [&](GExpr &chain) {
            auto &indexChain = *sgl::ConvertTo<IndexChain>(chain);
            if (indexChain.isHappy())
                indexChain.setFixFierz(true);
        });
    }
    bool areCanonical(IndexChain const &A, IndexChain const &B)
    {
        bool commonIndex = false;
        for (const auto &argA : A) {
            for (const auto &argB : B) {
                for (const auto &i : argA->indices()) {
                    for (const auto &j : argB->indices()) {
                        if (i == j) {
                            commonIndex = true;
                            break;
                        }
                    }
                    if (commonIndex) break;
                }
                if (commonIndex) break;
            }
            if (commonIndex) break;
        }
        if (!commonIndex)
            return true;
        if (A.size() > 2 || B.size() > 2)
            return false;
        if ((A.size() < 2 || IndexChain().isChiral(A.argument(1)))
                && (B.size() < 2 || IndexChain().isChiral(B.argument(1)))) {
            return true;
        }
        return false;
    }

    void applyGeneralFierz(GExpr &expr, bool twice)
    {
        bool transformed = sgl::transform<sgl::Prod>(expr, [&](GExpr &sub)
        {
            for (size_t i = 0; i != sub->size(); ++i) {
                if (!IsType<sgl::IndexChain>(sub[i]))
                    continue;
                for (size_t j = i+1; j < sub->size(); ++j) {
                    if (!IsType<sgl::IndexChain>(sub[j]))
                        continue;
                    auto const &c1 = *ConvertTo<sgl::IndexChain>(sub[i]);
                    auto const &c2 = *ConvertTo<sgl::IndexChain>(sub[j]);
                    // If double application, we continue only if the two
                    // chains are composed of the fermions they want AND are
                    // canonical 
                    if (twice 
                            && areCanonical(c1, c2)
                            && c1.isHappy() 
                            && c2.isHappy())
                        continue;
                    // Is simple application, chains must not be happy otherwise
                    // there is nothing to do
                    if (!twice
                            && (c1.isHappy() || c2.isHappy()))
                        continue;
                    GExpr res;
                    if (twice)
                        res = c1.applyGeneralFierzTwice(c2);
                    else
                        res = c1.applyGeneralFierz(c2);
                    sub[i] = res;
                    sub[j] = cslexpr_s(1);
                    return true;
                }
            }
            return false;
        });
        if (transformed) {
            expr = CSLSimplified(expr);
        }
    }

    void applyGeneralFierzTwice(GExpr &expr)
    {
        applyGeneralFierz(expr, true);
    }
}
