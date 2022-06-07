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

#include "partialExpand.h"
#include "indicial.h"
#include "utils.h"
#include "algo.h"

namespace csl {

    struct ExpansionChain {
        csl::Expr const *emitter;
        std::vector<std::size_t> posToExpand;
    };

    bool haveCommonIndices(
            csl::Expr const &a, 
            csl::Expr const &b,
            csl::Space const *space
            )
    {
        if (!b->isIndexed())
            return false;
        csl::IndexStructure indexA = a->getIndexStructure();
        csl::IndexStructure indexB = b->getIndexStructure();
        for (const auto &iA : indexA)
            if (iA.getSpace() == space)
                for (const auto &iB : indexB)
                    if (iA == iB)
                        return true;
        return false;
    }

    bool alreadyInChain(
            std::size_t pos,
            std::vector<ExpansionChain> const &chains
            )
    {
        auto first = chains.begin();
        const auto end = chains.end();
        while (first != end) {
            for (std::size_t i : first->posToExpand) {
                if (i == pos)
                    return true;
            }
            ++first;
        }
        return false;
    }

    csl::Expr const *getEmitter(
            csl::Expr const &expr,
            ExpanderEmitter const &isEmitter
            )
    {
        csl::Expr const *res = nullptr;
        csl::VisitFirstOfNode(expr, [&res, &isEmitter](csl::Expr const &sub) {
            if (isEmitter(sub)) {
                res = &sub;
                return true;
            }
            return false;
        });
        return res;
    }
    bool any_of(
            csl::Expr const *emi, 
            csl::Expr const &rec, 
            ExpanderReceiver const &isReceiver
            )
    {
        return csl::AnyOfNodes(rec, [&](csl::Expr const &sub) {
            return isReceiver(*emi, sub) || &sub == emi;
        });
    }

    std::size_t getNTerms(
            csl::Expr const &prod,
            ExpansionChain   const &toExpand,
            ExpanderReceiver const &isReceiver
            )
    {
        csl::Expr emitter = nullptr;
        std::size_t nTerms = 1;
        for (std::size_t pos : toExpand.posToExpand) {
            auto const &arg = prod[pos];
            if (csl::IsSum(arg)) {
                const std::size_t sum_sz = arg->size();
                std::size_t nSumTerms = 0;
                for (const auto &sum_arg : arg) {
                    if (any_of(toExpand.emitter, sum_arg, isReceiver)) {
                        ++nSumTerms;
                    }
                }
                if (nSumTerms < sum_sz)
                    ++nSumTerms;
                nTerms *= nSumTerms;
            }
            if (!emitter) {
                emitter = arg;
            }
        }
        return nTerms;
    }

    csl::Expr applyExpansion(
            csl::Expr const &prod,
            ExpansionChain   const &toExpand,
            ExpanderEmitter  const &isEmitter,
            ExpanderReceiver const &isReceiver,
            bool applyRecursively
            )
    {
        const std::size_t sz = toExpand.posToExpand.size();
        csl::Expr const *emitter = toExpand.emitter;
        const std::size_t nTerms = getNTerms(prod, toExpand, isReceiver);
        std::vector<std::vector<csl::Expr>> res;
        res.reserve(nTerms);
        res.push_back({});
        res[0].reserve(sz);
        for (std::size_t pos : toExpand.posToExpand) {
            auto const &arg = prod[pos];
            if (csl::IsSum(arg)) {
                const std::size_t sum_sz = arg->size();
                std::vector<csl::Expr> expanded;
                std::vector<csl::Expr> notExpanded;
                notExpanded.reserve(sum_sz);
                expanded.reserve(sum_sz);
                for (const auto &sum_arg : arg) {
                    if (!any_of(emitter, sum_arg, isReceiver)) {
                        notExpanded.push_back(sum_arg);
                    }
                    else
                        expanded.push_back(sum_arg);
                }
                csl::Expr still = (notExpanded.size() == 1) ? 
                        notExpanded[0] 
                        :csl::sum_s(notExpanded);
                expanded.push_back(still);
                const std::size_t nTerms  = expanded.size();
                const std::size_t resSize = res.size();
                for (std::size_t i = 0; i != (nTerms - 1) * resSize; ++i) {
                    res.push_back({});
                    auto &vec = res.back();
                    vec.reserve(sz);
                    vec = res[i % resSize];
                }
                for (std::size_t i = 0; i != nTerms; ++i) {
                    for (std::size_t j = 0; j != resSize; ++j) {
                        res[resSize * i + j].push_back(expanded[i]);
                    }
                }
            }
            else {
                for (auto &terms : res)
                    terms.push_back(arg);
            }
        }

        std::vector<csl::Expr> sumTerms(res.size());
        for (std::size_t i = 0; i != res.size(); ++i)  {
            sumTerms[i] = csl::prod_s(res[i]);
            if (applyRecursively) {
                csl::PartialExpandImplementation(sumTerms[i], isEmitter, isReceiver);
            }
        }
        return csl::sum_s(sumTerms);
    }

    bool hasCommonIndex(
            std::vector<std::size_t> const &sortedA,
            std::vector<std::size_t> const &sortedB
            )
    {
        auto firstA = sortedA.begin();
        auto firstB = sortedB.begin();
        const auto endA = sortedA.end();
        const auto endB = sortedB.end();
        while (firstA != endA && firstB != endB) {
            if (*firstA == *firstB) {
                return true;
                ++firstA;
                ++firstB;
            }
            else if (*firstA > *firstB) {
                ++firstB;
            }
            else {
                ++firstA;
            }
        }

        return false;
    }

    bool mergeChains(
            std::vector<ExpansionChain> &toExpand
            )
    {
        if (toExpand.size() < 2)
            return false;
        bool chainsLeft = false;
        for (std::size_t i = 0; i != toExpand.size(); ++i) {
            for (std::size_t j = i+1; j < toExpand.size(); ++j) {
                if (toExpand[i].posToExpand == toExpand[j].posToExpand) {
                    toExpand.erase(toExpand.begin() + j);
                    --j;
                }
                else if (hasCommonIndex(
                            toExpand[i].posToExpand, 
                            toExpand[j].posToExpand
                            )) {
                    chainsLeft = true;
                    toExpand.erase(toExpand.begin() + j);
                    --j;
                }
            }
        }
        return chainsLeft;
    }

    void PartialExpandImplementation(
            Expr &prod,
            ExpanderEmitter  const &isEmitter,
            ExpanderReceiver const &isReceiver
            )
    {
        if (!csl::IsProd(prod))
            return;
        // std::cout << "PARTIAL EXPANSION" << std::endl;
        // std::cout << prod << std::endl;
        const std::size_t sz = prod->size();
        if (sz < 2)
            return;
        std::vector<ExpansionChain> toExpand;
        toExpand.reserve(sz);
        for (std::size_t i = 0; i != sz; ++i) {
            auto const &argi = prod[i];
            if (alreadyInChain(i, toExpand))
                continue;
            auto emitter = getEmitter(argi, isEmitter);
            if (!emitter)
                continue;
            bool receiverFound = false;
            for (std::size_t j = 0; j != sz; ++j) {
                if (i == j)
                    continue;
                auto const &argj = prod[j];
                if (any_of(emitter, argj, isReceiver)) {
                    if (!receiverFound) {
                        toExpand.push_back({emitter, {}});
                        auto &vec = toExpand.back().posToExpand;
                        vec.reserve(sz);
                        vec.push_back(i);
                        vec.push_back(j);
                        receiverFound = true;
                    }
                    else {
                        toExpand.back().posToExpand.push_back(j);
                    }
                }
            }
        }
        for (auto &chain : toExpand)
            std::sort(chain.posToExpand.begin(), chain.posToExpand.end());
        bool chainsLeft = mergeChains(toExpand);
        for (std::size_t i = 0; i != toExpand.size(); ++i) {
            bool hasSum = false;
            for (std::size_t pos : toExpand[i].posToExpand) {
                if (csl::IsSum(prod[pos])) {
                    hasSum = true;
                    break;
                }
            }
            if (!hasSum) {
                toExpand.erase(toExpand.begin() + i);
                --i;
            }
        }
        if (toExpand.empty())
            return;
        // std::cout << toExpand.size() << " chains" << std::endl;
        // for (const auto &chain : toExpand) {
        //     for (std::size_t i : chain.posToExpand)
        //         std::cout << i << " ; ";
        //     std::cout << std::endl;
        // }
        std::vector<bool> notExpanded(sz, true);
        for (const auto &chain : toExpand) {
            for (std::size_t i : chain.posToExpand)
                notExpanded[i] = false;
        }
        std::vector<csl::Expr> prodTerms;
        prodTerms.reserve(sz);
        for (std::size_t i = 0; i != sz; ++i) 
            if (notExpanded[i]) {
                prodTerms.push_back(prod[i]);
            }
        for (const auto &chain : toExpand) {
            prodTerms.push_back(
                    applyExpansion(
                        prod, 
                        chain,
                        isEmitter,
                        isReceiver,
                        chainsLeft)
                    );
        }
        if (prodTerms.size() == 1)
            prod = prodTerms[0];
        else 
            prod = csl::prod_s(prodTerms);
        PartialExpandImplementation(prod, isEmitter, isReceiver);
    }

    void PartialExpandImplementation(
            Expr &prod,
            ExpanderEmitter const &isEmitter,
            ExpanderEmitter const &isReceiver
            )
    {
        PartialExpandImplementation(
                prod,
                isEmitter,
                [&isReceiver](csl::Expr const &, csl::Expr const& a) -> bool {
                    return bool(isReceiver(a));
                });
    }

    Expr PartialExpanded(
                    Expr const &init,
                    ExpanderEmitter const &isEmitter,
                    ExpanderReceiver const &isReceiver
                    )
    {
        csl::Expr copy = csl::Copy(init);
        PartialExpand(copy, isEmitter, isReceiver);
        return copy;
    }
    void PartialExpand(
                    Expr &init,
                    ExpanderEmitter const &isEmitter,
                    ExpanderReceiver const &isReceiver
                    )
    {
        if (csl::IsProd(init))
            PartialExpandImplementation(init, isEmitter, isReceiver);
    }
    Expr DeepPartialExpanded(
                    Expr const &init,
                    ExpanderEmitter const &isEmitter,
                    ExpanderReceiver const &isReceiver
                    )
    {
        csl::Expr copy = csl::DeepCopy(init);
        DeepPartialExpand(copy, isEmitter, isReceiver);
        return copy;
    }
    void DeepPartialExpand(
                    Expr &init,
                    ExpanderEmitter const &isEmitter,
                    ExpanderReceiver const &isReceiver
                    )
    {
        csl::ForEachNode(init, [&isEmitter, &isReceiver](csl::Expr &prod) {
            if (csl::IsProd(prod)) {
                PartialExpandImplementation(prod, isEmitter, isReceiver);
            }
        });
    }

}
