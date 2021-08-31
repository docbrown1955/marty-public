#include "interface.h"
#include "abreviation.h"

namespace csl {

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
    auto step = [&](size_t pos) {
        contractedIndices = free(
                contractedIndices + tensors[pos]->getIndexStructureView());
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

int matchBOnA(csl::Expr const& A, csl::Expr &B)
{
    std::vector<csl::Expr> tensorsInA;
    std::vector<csl::Expr> tensorsInB;
    csl::VisitEachLeaf(A, [&](csl::Expr const& el)
    {
        if (el->isIndexed())
            tensorsInA.push_back(el);
    });
    csl::VisitEachLeaf(B, [&](csl::Expr const& el)
    {
        if (el->isIndexed())
            tensorsInB.push_back(el);
    });
    if (tensorsInA.size() != tensorsInB.size()) {
        return tensorsInA.size() < tensorsInB.size();
    }
    sortTensors(tensorsInA);
    sortTensors(tensorsInB);
    std::vector<std::pair<csl::Index, csl::Index>> mapping;
    for (size_t i = 0; i != tensorsInA.size(); ++i) {
        if (tensorsInA[i]->getParent_info() 
                != tensorsInB[i]->getParent_info()) {
            return tensorsInA[i]->getName() < tensorsInB[i]->getName();
        }
        else {
            csl::IndexStructure Astruct = tensorsInA[i]->getIndexStructure();
            auto last = std::remove_if(Astruct.begin(), Astruct.end(),
                    [&](csl::Index const &i) { return i.getFree(); });
            Astruct.erase(last, Astruct.end());
            csl::IndexStructure Bstruct = tensorsInB[i]->getIndexStructure();
            last = std::remove_if(Bstruct.begin(), Bstruct.end(),
                    [&](csl::Index const &i) { return i.getFree(); });
            Bstruct.erase(last, Bstruct.end());
            for (size_t j = 0; j != Astruct.size(); ++j) {
                auto pos = std::find_if(
                        mapping.begin(),
                        mapping.end(),
                        [&](std::pair<csl::Index, csl::Index> const& p)
                        {
                            return p.second == Astruct[j];
                        });
                if (pos == mapping.end())
                    mapping.push_back({ Bstruct[j], Astruct[j] });
            }
        }
    }
    std::vector<csl::Index> intermediateIndices;
    intermediateIndices.reserve(mapping.size());
    for (const auto &mappy : mapping)
        intermediateIndices.push_back(mappy.first.rename());
    size_t index = 0;
    for (auto& mappy : mapping) {
        B = csl::Replaced(
                B, 
                mappy.first, 
                intermediateIndices[index], 
                false);
        if (mappy.first.getSpace()->getSignedIndex())
            B = csl::Replaced(
                    B,
                    mappy.first.getFlipped(),
                    intermediateIndices[index].getFlipped(),
                    false);
        ++index;
    }
    index = 0;
    for (auto& mappy : mapping) {
        B = csl::Replaced(
                B, 
                intermediateIndices[index], 
                mappy.second, 
                false);
        if (mappy.first.getSpace()->getSignedIndex())
            B = csl::Replaced(
                    B,
                    intermediateIndices[index].getFlipped(),
                    mappy.second.getFlipped(),
                    false);
        ++index;
    }
    csl::DeepRefresh(B);
    return -1;
}

static bool hardComparison_impl(
        csl::Expr const &A,
        csl::Expr       &B)
{
    const int match = matchBOnA(A, B);
    if (match != -1)
        return false;
    auto res = A->compareWithDummy(B.get());
    return res;
}

bool hardComparison(
        csl::Expr const& A,
        csl::Expr const& B
        )
{
    auto B_renameIndices = csl::DeepCopy(B);
    csl::RenameIndices(B_renameIndices);
    return hardComparison_impl(A, B_renameIndices);
}

static bool hardOrdering_impl(
        csl::Expr const &A,
        csl::Expr       &B)
{
    const int match = matchBOnA(A, B);
    if (match != -1)
        return match;
    return A < B;
}

bool hardOrdering(
        csl::Expr const& A,
        csl::Expr const& B
        )
{
    auto B_renameIndices = csl::DeepCopy(B);
    csl::RenameIndices(B_renameIndices);
    return hardOrdering_impl(A, B_renameIndices);
}

}
