#include "abreviation.h"
#include "interface.h"
#include "multipermutation.h"
#include <numeric>

namespace csl {

static MultiPermutation
getAllPossiblePermutations(std::vector<csl::Expr> const &tensors)
{
    std::vector<std::vector<size_t>> permutations;
    permutations.reserve(tensors.size());
    std::vector<size_t> indicesLeft(tensors.size());
    std::iota(indicesLeft.begin(), indicesLeft.end(), 0);
    while (!indicesLeft.empty()) {
        auto const &tensor1 = tensors[indicesLeft[0]];
        permutations.push_back({indicesLeft[0]});
        indicesLeft.erase(indicesLeft.begin());
        if (!tensor1->getCommutable())
            continue;
        for (size_t i = 0; i != indicesLeft.size(); ++i) {
            auto const &tensor2 = tensors[indicesLeft[i]];
            if (tensor1->getParent_info() == tensor2->getParent_info()
                && tensor1->isComplexConjugate()
                       == tensor2->isComplexConjugate()) {
                permutations.back().push_back(i);
                indicesLeft.erase(indicesLeft.begin() + i);
                --i;
            }
        }
    }
    return MultiPermutation{permutations};
}

static void sortTensors(std::vector<csl::Expr> &tensors)
{
    auto free = [&](csl::IndexStructure const &index) {
        return csl::Abbrev::getFreeStructure(index);
    };
    std::sort(tensors.begin(), tensors.end());
    std::reverse(tensors.begin(), tensors.end());
    std::vector<csl::Expr> sorted;
    sorted.reserve(tensors.size());
    csl::IndexStructure contractedIndices;
    auto                step = [&](size_t pos) {
        contractedIndices
            = free(contractedIndices + tensors[pos]->getIndexStructureView());
        sorted.push_back(tensors[pos]);
        tensors.erase(tensors.begin() + pos);
    };

    while (!tensors.empty()) {
        if (contractedIndices.empty()) {
            step(0);
            continue;
        }
        bool foundCommon = false;
        for (size_t i = 0; i != tensors.size(); ++i) {
            if (csl::IsTensorField(tensors[i]))
                continue;
            auto const &index = tensors[i]->getIndexStructureView();
            if (contractedIndices.hasCommonIndex(index)) {
                step(i);
                foundCommon = true;
                break;
            }
        }
        if (!foundCommon)
            step(0);
    }
    tensors = std::move(sorted);
}

static std::pair<std::vector<csl::Expr>, std::vector<csl::Expr>>
getSortedTensors(csl::Expr const &A, csl::Expr const &B)
{
    std::vector<csl::Expr> tensorsInA;
    std::vector<csl::Expr> tensorsInB;
    csl::VisitEachNodeCut(A, [&](csl::Expr const &el) {
        if (csl::IsIndicialTensor(el))
            tensorsInA.push_back(el);
        return csl::IsSum(el);
    });
    csl::VisitEachNodeCut(B, [&](csl::Expr const &el) {
        if (csl::IsIndicialTensor(el))
            tensorsInB.push_back(el);
        return csl::IsSum(el);
    });
    // if (tensorsInA.size() != tensorsInB.size()) {
    //     return tensorsInA.size() < tensorsInB.size();
    // }
    sortTensors(tensorsInA);
    sortTensors(tensorsInB);

    return {tensorsInA, tensorsInB};
}

int matchBOnA(csl::Expr &                   B,
              std::vector<csl::Expr> const &tensorsInA,
              std::vector<csl::Expr> const &tensorsInB)
{
    if (tensorsInA.size() != tensorsInB.size()) {
        return tensorsInA.size() < tensorsInB.size();
    }
    std::vector<std::pair<csl::Index, csl::Index>> mapping;
    for (size_t i = 0; i != tensorsInA.size(); ++i) {
        if (tensorsInA[i]->getParent_info()
            != tensorsInB[i]->getParent_info()) {
            return tensorsInA[i]->getName() < tensorsInB[i]->getName();
        }
        else {
            csl::IndexStructure Astruct = tensorsInA[i]->getIndexStructure();
            auto                last    = std::remove_if(
                Astruct.begin(), Astruct.end(), [&](csl::Index const &i) {
                    return i.getFree();
                });
            Astruct.erase(last, Astruct.end());
            csl::IndexStructure Bstruct = tensorsInB[i]->getIndexStructure();
            last                        = std::remove_if(
                Bstruct.begin(), Bstruct.end(), [&](csl::Index const &i) {
                    return i.getFree();
                });
            Bstruct.erase(last, Bstruct.end());
            for (size_t j = 0; j != Astruct.size(); ++j) {
                auto pos = std::find_if(
                    mapping.begin(),
                    mapping.end(),
                    [&](std::pair<csl::Index, csl::Index> const &p) {
                        return p.second == Astruct[j];
                    });
                if (pos == mapping.end())
                    mapping.push_back({Bstruct[j], Astruct[j]});
            }
        }
    }
    std::vector<csl::Index> intermediateIndices;
    intermediateIndices.reserve(mapping.size());
    for (const auto &mappy : mapping)
        intermediateIndices.push_back(mappy.first.rename());
    size_t index = 0;
    for (auto &mappy : mapping) {
        B = csl::Replaced(B, mappy.first, intermediateIndices[index], false);
        if (mappy.first.getSpace()->getSignedIndex())
            B = csl::Replaced(B,
                              mappy.first.getFlipped(),
                              intermediateIndices[index].getFlipped(),
                              false);
        ++index;
    }
    index = 0;
    for (auto &mappy : mapping) {
        B = csl::Replaced(B, intermediateIndices[index], mappy.second, false);
        if (mappy.first.getSpace()->getSignedIndex())
            B = csl::Replaced(B,
                              intermediateIndices[index].getFlipped(),
                              mappy.second.getFlipped(),
                              false);
        ++index;
    }
    csl::DeepRefresh(B);
    return -1;
}

int matchBOnA(csl::Expr const &A, csl::Expr &B)
{
    auto [tensorsInA, tensorsInB] = getSortedTensors(A, B);
    if (tensorsInA.size() != tensorsInB.size()) {
        return tensorsInA.size() < tensorsInB.size();
    }
    return matchBOnA(B, tensorsInA, tensorsInB);
}

static bool hardComparison_impl(csl::Expr const &A, csl::Expr const &B)
{
    auto [tensorsInA, tensorsInB] = getSortedTensors(A, B);
    MultiPermutation permutation  = getAllPossiblePermutations(tensorsInB);
    do {
        csl::Expr B_cpy           = csl::DeepCopy(B);
        auto      tensorsInB_perm = permutation.applyPermutation(tensorsInB);
        const int match = matchBOnA(B_cpy, tensorsInA, tensorsInB_perm);
        if (match != -1)
            return false;
        if (A->compareWithDummy(B_cpy.get()))
            return true;
    } while (permutation.nextPermutation());

    return false;
}

bool hardComparison(csl::Expr const &A, csl::Expr const &B)
{
    auto B_renameIndices = csl::DeepCopy(B);
    csl::RenameIndices(B_renameIndices);
    return hardComparison_impl(A, B_renameIndices);
}

static bool hardOrdering_impl(csl::Expr const &A, csl::Expr &B)
{
    const int match = matchBOnA(A, B);
    if (match != -1)
        return match;
    return A < B;
}

bool hardOrdering(csl::Expr const &A, csl::Expr const &B)
{
    auto B_renameIndices = csl::DeepCopy(B);
    csl::RenameIndices(B_renameIndices);
    return hardOrdering_impl(A, B_renameIndices);
}

} // namespace csl
