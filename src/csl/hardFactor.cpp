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

#include "hardFactor.h"
#include "algo.h"
#include "indicial.h"
#include "linear_map.h"
#include "utils.h"

#include <algorithm>

namespace csl {

// Tests if two factors are redundant
// returns 0 if not
// returns 1 if fR contains fL
// returns -1 if fL contains fR
int isSuperFactor(Expr const &fL, Expr const &fR)
{
    const auto lNum = csl::IsNumerical(fL);
    const auto rNum = csl::IsNumerical(fR);
    if (lNum || rNum) {
        if (!(lNum && rNum))
            return 0;
        if (csl::IsInteger(fL->division_own(fR)))
            return -1;
        if (csl::IsInteger(fR->division_own(fL)))
            return 1;
        return 0;
    }
    const bool      lPow  = csl::IsPow(fL) && csl::IsInteger(fL[1]);
    const bool      rPow  = csl::IsPow(fR) && csl::IsInteger(fR[1]);
    const csl::Expr lTerm = lPow ? fL[0] : fL;
    const csl::Expr rTerm = rPow ? fR[0] : fR;
    if (lTerm != rTerm)
        return false;
    const int lExponent = lPow ? fL[1]->evaluateScalar() : 1;
    const int rExponent = rPow ? fR[1]->evaluateScalar() : 1;
    if (lExponent * rExponent < 0)
        return 0;
    return ((lExponent / rExponent) == 0) ? 1 : -1;
}

// Number of identical values in A and B, sorted uniquely
// O(N)
size_t nSimilar(std::vector<size_t> const &sortedA,
                std::vector<size_t> const &sortedB)
{
    size_t     n      = 0;
    auto       firstA = sortedA.begin();
    auto       firstB = sortedB.begin();
    const auto endA   = sortedA.end();
    const auto endB   = sortedB.end();
    while (firstA != endA && firstB != endB) {
        if (*firstA == *firstB) {
            ++n;
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

    return n;
}

// Max number of match between pos and an element of m
// O(N*M)
size_t maxSimilarity(std::vector<size_t> const &pos,
                     csl::linear_map<csl::Expr, std::vector<size_t>> const &m)
{
    size_t max = 0;
    for (const auto &el : m) {
        auto const &pos2 = el.second;
        if (&pos2 != &pos) {
            size_t n = nSimilar(pos, pos2);
            if (n > max)
                max = n;
        }
    }
    return max;
}

using FactorType = std::pair<csl::Expr, std::vector<size_t>>;
FactorType const *
getBestFactor(csl::linear_map<csl::Expr, std::vector<size_t>> const &m)
{
    std::vector<FactorType const *> candidates;
    candidates.reserve(m.size());
    for (const auto &el : m) {
        candidates.push_back(&el);
    }
    if (candidates.size() == 1) {
        return (candidates[0]->second.size() > 1) ? candidates[0] : nullptr;
    }
    auto isNumerical
        = [](FactorType const *f) { return csl::IsNumerical(f->first); };
    bool allNumeric
        = std::all_of(candidates.begin(), candidates.end(), isNumerical);
    if (!allNumeric) {
        for (size_t i = 0; i != candidates.size(); ++i)
            if (isNumerical(candidates[i])) {
                candidates.erase(candidates.begin() + i);
                --i;
            }
    }
    size_t maxSize = 0;
    for (const auto &el : candidates) {
        const size_t sz = el->second.size();
        if (sz > maxSize)
            maxSize = sz;
    }
    if (maxSize < 2)
        return nullptr;
    for (size_t i = 0; i != candidates.size(); ++i)
        if (candidates[i]->second.size() < maxSize) {
            candidates.erase(candidates.begin() + i);
            --i;
        }
    size_t            maxSim = 0;
    FactorType const *best   = candidates[0];
    for (const auto &el : candidates) {
        size_t maxSim_el = maxSimilarity(el->second, m);
        if (maxSim_el > maxSim) {
            maxSim = maxSim_el;
            best   = el;
        }
    }
    return best;
}

std::pair<bool, bool>
hasRecursiveFactors(FactorType const *best,
                    csl::linear_map<csl::Expr, std::vector<size_t>> const &m)
{
    if (m.size() == 1)
        return {false, false};
    bool innerRecursion = false;
    bool outerRecursion = false;
    for (const auto &el : m) {
        if (&el != best) {
            const size_t nCommon = nSimilar(best->second, el.second);
            if ((el.second.size() - nCommon) > 1) {
                outerRecursion = true;
            }
            else if (nCommon > 1) {
                innerRecursion = true;
            }
        }
    }
    return {innerRecursion, outerRecursion};
}

csl::linear_map<csl::Expr, std::vector<size_t>>::iterator
linear_find(csl::linear_map<csl::Expr, std::vector<size_t>> &m,
            csl::Expr const &                                expr)
{
    auto       first = m.begin();
    const auto end   = m.end();
    while (first != end) {
        if (first->first == expr)
            break;
        ++first;
    }
    return first;
}

bool HardFactorImplementation(Expr &sum, bool applyRecursively)
{
    // static size_t nRecurs = 0;
    // struct SS { SS() { ++nRecurs; } ~SS() { --nRecurs; } };
    // SS witness;
    if (!csl::IsSum(sum))
        return false;
    const size_t sz = sum->size();
    if (sz == 0) {
        sum = CSL_0;
        return false;
    }
    csl::linear_map<csl::Expr, std::vector<size_t>> posFactors;
    auto                                            first = sum->begin();
    const auto                                      end   = sum->end();
    size_t                                          index = 0;
    while (first != end) {
        auto factors = (**first).getFactors();
        for (const auto &f : factors) {
            if (f->isIndexed() || f == CSL_1 || f == CSL_M_1)
                continue;
            if (csl::option::canonicalSumNumericalFactor
                && csl::IsNumerical(f))
                continue;
            auto pos = linear_find(posFactors, f);
            if (pos == posFactors.end()) {
                for (auto mapiter = posFactors.begin();
                     mapiter != posFactors.end();
                     ++mapiter) {
                    const int similar = isSuperFactor(f, mapiter->first);
                    if (similar != 0) {
                        pos = mapiter;
                        if (similar == 1) {
                            pos->first = f;
                        }
                        break;
                    }
                }
                if (pos == posFactors.end()) {
                    posFactors[f].reserve(sz);
                    pos = linear_find(posFactors, f);
                    if (pos == posFactors.end()) {
                        std::cerr << f << std::endl;
                        std::cerr << "NOT FOUND IN :" << std::endl;
                        for (const auto &el : posFactors)
                            std::cerr << el.first << std::endl;
                        throw 6;
                    }
                }
            }
            auto &container = pos->second;
            if (container.empty() || container.back() != index)
                pos->second.push_back(index);
        }
        ++index;
        ++first;
    }
    for (auto it = posFactors.begin(); it != posFactors.end();) {
        if (it->second.size() < 2)
            it = posFactors.erase(it);
        else
            ++it;
    }
    if (posFactors.empty())
        return false;

    auto best = getBestFactor(posFactors);
    if (!best)
        return false;

    auto const & positionsFactored = best->second;
    auto const & factor            = best->first;
    const size_t nFactored         = positionsFactored.size();
    if (csl::option::canonicalSumNumericalFactor
        && csl::IsNumerical(best->first) && nFactored == sz)
        return false;
    csl::Expr              copySum = DeepCopy(sum);
    std::vector<csl::Expr> outerTerms;
    std::vector<csl::Expr> innerTerms;
    innerTerms.reserve(nFactored);
    if (sz > nFactored)
        outerTerms.reserve(sz - nFactored);
    first               = sum->begin();
    index               = 0;
    size_t posInPosList = 0;
    bool   factored     = false;
    while (first != end) {
        if (!factored && posInPosList < positionsFactored.size()
            && index == positionsFactored[posInPosList]) {
            if (csl::IsNumerical(factor)) {
                if (csl::IsSum(*first))
                    innerTerms.push_back(csl::Expanded((*first) / factor));
                else
                    innerTerms.push_back((*first) / factor);
            }
            else if (csl::IsProd(factor) || csl::IsPow(factor)) {
                innerTerms.push_back(*first / factor);
            }
            else {
                innerTerms.push_back((**first).suppressTerm(factor.get()));
            }
            ++posInPosList;
        }
        else {
            outerTerms.push_back(*first);
        }
        ++first;
        ++index;
    }
    auto innerSum = csl::sum_s(innerTerms);
    auto [innerRecursion, outerRecursion]
        = hasRecursiveFactors(best, posFactors);
    if (applyRecursively && innerRecursion) {
        HardFactorImplementation(innerSum, true);
    }
    outerTerms.push_back(csl::prod_s(factor, innerSum));
    sum = (outerTerms.size() == 1) ? outerTerms[0] : csl::sum_s(outerTerms);
    if (applyRecursively && outerRecursion) {
        HardFactorImplementation(sum, true);
    }

    return true;
}

void HardFactor(Expr &init)
{
    if (csl::IsSum(init))
        HardFactorImplementation(init, true);
}

void DeepHardFactor(Expr &init)
{
    csl::ForEachNode(init, [](csl::Expr &expr) {
        if (csl::IsSum(expr)) {
            HardFactorImplementation(expr, true);
        }
        else if (csl::IsProd(expr)) {
            csl::Refresh(expr);
        }
    });
}

Expr HardFactored(Expr const &init)
{
    csl::Expr copy = csl::Copy(init);
    csl::HardFactor(copy);
    return copy;
}

Expr DeepHardFactored(Expr const &init)
{
    csl::Expr copy = csl::DeepCopy(init);
    csl::DeepHardFactor(copy);
    return copy;
}
} // namespace csl
