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

#include <numeric>
#include "abreviation.h"
#include "indicial.h"
#include "scopedProperty.h"
#include "algo.h"
#include "utils.h"
#include "interface.h"
#include "error.h"
#include "space.h"
#include "equation.h"
#include "literal.h"
#include "numerical.h"
#include "comparison.h"
#include "tensorField.h"
#include "equation.h"
#include "property.h"
#include "commutation.h"
#include "counter.h"
#include "options.h"

using namespace std;

namespace csl {

bool IsIndicialTensor(const Expr& expr)
{
    return IsIndicialTensor(expr.get());
}

bool IsIndicialTensor(Expr_info expr)
{
    return expr->getType() == csl::Type::TensorElement
        or expr->getType() == csl::Type::TensorFieldElement
        or expr->getType() == csl::Type::TDerivativeElement;
}

csl::vector_expr getAllPermutations(const Expr& expr)
{
    if (expr->getType() == csl::Type::Prod) {
        if (expr->size() == 2
                and (*expr)[0]->getPrimaryType() == csl::PrimaryType::Numerical
                and IsIndicialTensor((*expr)[1])) {
            csl::vector_expr permutations = (*expr)[1]->getPermutations(false);
            for (auto& el : permutations)
                el = (*expr)[0] * el;

            return permutations;
        }
    }
    else if (IsIndicialTensor(expr))
        return expr->getPermutations(false);

    CALL_SMERROR_SPEC(CSLError::TypeError,
                 "Calling getAllPermutations(expr) must be for a tensor "
                 +static_cast<string>("or a number times a tensor."));

    return {};
}

void nameTensor(const string& name, Expr& tensor, bool first)
{
     if (tensor->getPrimaryType() != csl::PrimaryType::Vectorial)
         return;
    // if (tensor->getPrimaryType() != csl::PrimaryType::Vectorial)
    //     callError(cslError::UndefinedBehaviour,
    //             "nameTensor(const string&, Expr&)");
    if (tensor->getDim() == 1) {
        if (first)
            for (auto arg=tensor->begin(); arg!=tensor->end(); ++arg) {
                ostringstream sout;
                sout<<distance(tensor->begin(), arg);
                (*arg) = constant_s(name+"_"+sout.str());
                // (*arg)->setElementary(true);
            }
        else
            for (auto arg=tensor->begin(); arg!=tensor->end(); ++arg) {
                ostringstream sout;
                sout<<distance(tensor->begin(), arg);
                (*arg) = constant_s(name+","+sout.str()+"}");
                // (*arg)->setElementary(true);
            }
    }
    else {
        if (first)
            for (auto arg=tensor->begin(); arg!=tensor->end(); ++arg) {
                ostringstream sout;
                sout<<distance(tensor->begin(), arg);
                nameTensor(name+"_{"+sout.str(), *arg, false);
            }
        else
            for (auto arg=tensor->begin(); arg!=tensor->end(); ++arg) {
                ostringstream sout;
                sout<<distance(tensor->begin(), arg);
                nameTensor(name+","+sout.str(), *arg, false);
            }
    }
}

Expr generateTensor(const string& name, const vector<const Space*>& spaces)
{
    vector<int> shape(0);
    for (const auto& s : spaces)
        shape.push_back(s->getDim());

    Expr tensor = vectorialtensor_s(shape);
    nameTensor(name, tensor);

    return tensor;
}

///////////////////////////////////////////////////
/*************************************************/
// Class SelfContraction                         //
/*************************************************/
///////////////////////////////////////////////////

bool SelfContraction::comparePairs(const vector<pair<int,Index>>& A,
                                   const vector<pair<int,Index>>& B)
{
    if (A.size() != B.size())
        return false;

    vector<size_t> indicesLeft(A.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;

    for (size_t i = 0; i != A.size(); ++i) {
        bool matched = false;
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            size_t index = indicesLeft[j];
            if (A[i] == B[index]) {
                matched = true;
                indicesLeft.erase(indicesLeft.begin() + j);
                break;
            }
        }
        if (not matched)
            return false;
    }
    return true;
}

SelfContraction::SelfContraction(
        const Expr& A,
        const Expr& B,
        optional<function<bool(Expr_info, Expr_info)>> t_condition)
    :SelfContraction(A.get(), B.get(), t_condition)
{
}

SelfContraction::SelfContraction(
        Expr_info A,
        Expr_info B,
        optional<function<bool(Expr_info, Expr_info)>> t_condition)
    :condition(t_condition)
{
    Expr tensorA = csl::GetTerm(A->copy());
    Expr tensorB = csl::GetTerm(B->copy());
    if (tensorA->getParent_info() != tensorB->getParent_info())
        swappable = false;
    IndexStructure const &index_A = tensorA->getIndexStructureView();
    IndexStructure const &index_B = tensorB->getIndexStructureView();
    vector<bool> matchedA(index_A.size(), false);
    vector<bool> matchedB(index_B.size(), false);
    for (size_t i=0; i!=index_A.size(); ++i) {
        if (index_A[i].getType() == cslIndex::Fixed)
            specialAValues.push_back(make_pair(i, index_A[i]));
        for (size_t j=0; j!=index_B.size(); ++j) {
            if (i == 0 and index_B[j].getType() == cslIndex::Fixed)
                specialBValues.push_back(make_pair(j, index_B[j]));
            if (index_A[i] == index_B[j]
                    and index_A[i].getType() != cslIndex::Fixed) {
                matchedA[i] = true;
                matchedB[j] = true;
                contraction.push_back(make_pair(i,j));
                if (i > 0)
                    break;
            }
        }
    }
    for (size_t i = 0; i != index_A.size(); ++i)
        if (not matchedA[i])
            freeStructures[0] += index_A[i];
    for (size_t j = 0; j != index_B.size(); ++j)
        if (not matchedB[j])
            freeStructures[1] += index_B[j];
}

bool SelfContraction::empty() const
{
    return contraction.empty();
}

bool SelfContraction::conditionAppliesOn(Expr_info A,
                                         Expr_info B) const
{
    if (condition)
        return condition.value()(A, B);
    return true;
}

bool SelfContraction::conditionAppliesOn(const Expr& A,
                                         const Expr& B) const
{
    return conditionAppliesOn(A.get(), B.get());
}

IndexStructure SelfContraction::getFreeContractionIndex(
        const Expr& expr,
        int         index) const
{
    if (index != 0 and index != 1)
        callError(cslError::UndefinedBehaviour,
                "SelfContraction::getFreeContractionIndex()");
    IndexStructure structure;
    IndexStructure exprStructure = expr->getIndexStructure();
    for (size_t i = 0; i != exprStructure.size(); ++i) {
        bool found = false;
        for (const auto& c : contraction) {
            if (index == 0 and c.first == (int)i) {
                found = true;
                break;
            }
            else if (index == 1 and c.second == (int)i) {
                found = true;
                break;
            }
        }
        if (not found)
            structure += exprStructure[i];
    }
    return structure;
}

Expr SelfContraction::applyIndices(const Expr&            A,
                                   const Expr&            B,
                                   const SelfContraction& targetContraction,
                                   const Expr&            res) const
{
    // Should be called IIF (*this == targetContraction)
    if (swappable and not compare(targetContraction))
        return getSwapped().applyIndices(A, B, targetContraction, res);

    IndexStructure structA = getFreeContractionIndex(A, 0);
    IndexStructure structB = getFreeContractionIndex(B, 1);
    Expr replaced = DeepCopy(res);
    csl::RenameIndices(replaced);
    if (freeStructures[0].size() != structA.size()
            or freeStructures[1].size() != structB.size()) {
        A->print();
        B->print();
        std::cout << *this << std::endl;
        std::cout << targetContraction << std::endl;
        res->print();
        callError(cslError::UndefinedBehaviour,
                "SelfContraction::applyIndices()");
    }

    csl::IndexStructure intermediateA = structA;
    csl::IndexStructure intermediateB = structB;
    for (auto& index : intermediateA)
        index = index.rename();
    for (auto& index : intermediateB)
        index = index.rename();

    replaced = ReplaceIndices(replaced, freeStructures[0], structA);
    replaced = ReplaceIndices(replaced, freeStructures[1], structB);
    // for (size_t i = 0; i != freeStructures[0].size(); ++i) {
    //     replaced = ReplaceIndex(replaced, freeStructures[0][i], intermediateA[i]);
    // }
    // for (size_t i = 0; i != freeStructures[1].size(); ++i) {
    //     replaced = ReplaceIndex(replaced, freeStructures[1][i], intermediateB[i]);
    // }
    // for (size_t i = 0; i != intermediateA.size(); ++i) {
    //     replaced = ReplaceIndex(replaced, intermediateA[i], structA[i]);
    // }
    // for (size_t i = 0; i != intermediateB.size(); ++i) {
    //     replaced = ReplaceIndex(replaced, intermediateB[i], structB[i]);
    // }

    return replaced;
}

SelfContraction SelfContraction::getSwapped() const
{
    CSL_ASSERT_SPEC(swappable,
            CSLError::RuntimeError,
            "A non swappable self contraction is being swapped !");
    SelfContraction res;
    res.specialAValues = specialBValues;
    res.specialBValues = specialAValues;
    for (const auto& c : contraction) {
        res.contraction.push_back(c);
        pair<int, int>& newContraction =
            res.contraction[res.contraction.size()-1];
        std::swap(newContraction.first, newContraction.second);
    }
    res.freeStructures[1] = freeStructures[0];
    res.freeStructures[0] = freeStructures[1];

    return res;
}

bool SelfContraction::compare(const SelfContraction& other) const
{
    const size_t size = contraction.size();
    if (size > other.contraction.size()
            or specialAValues.size() != other.specialAValues.size()
            or specialBValues.size() != other.specialBValues.size())
        return false;

    if (not comparePairs(specialAValues, other.specialAValues)
            or not comparePairs(specialBValues, other.specialBValues))
        return false;

    vector<int> pairsLeft(other.contraction.size());
    for (size_t i=0; i!=other.contraction.size(); ++i)
        pairsLeft[i] = i;

    for (size_t i=0; i!=size; ++i) {
        bool matched = false;
        pair<int,int> p1 = contraction[i];
        for (size_t j=0; j!=pairsLeft.size(); ++j) {
            pair<int,int> p2 = other.contraction[pairsLeft[j]];
            if (p1 == p2) {
                pairsLeft.erase(pairsLeft.begin()+j);
                matched = true;
                break;
            }
        }
        if (not matched) {
            return false;
        }
    }

    return true;
}

bool SelfContraction::operator==(const SelfContraction& other) const
// returns true if *this is a sub contraction of other
{
    if (not compare(other)) {
        if (swappable)
            return compare(other.getSwapped());
        return false;
    }
    return true;
}

ostream& operator<<(ostream& fout, const SelfContraction& c)
{
    fout<<"SelfContraction with "<<c.contraction.size()<<" elements:\n";
    for (size_t i=0; i!=c.contraction.size(); ++i)
        fout<<" -"<<i+1<<": "<<c.contraction[i].first
            <<"<->"<<c.contraction[i].second<<endl;

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ContractionChain                        //
/*************************************************/
///////////////////////////////////////////////////

ContractionChain::ContractionChain()
    :contractedTensors(),
    structures(),
    contraction()
{

}

ContractionChain::ContractionChain(
        csl::vector_expr&           t_contractedTensors,
        vector<IndexStructure>& t_structures,
        vector<array<int, 4>>&  t_contraction)
    :contractedTensors(std::move(t_contractedTensors)),
    structures        (std::move(t_structures)),
    contraction       (std::move(t_contraction))
{

}

ContractionChain::ContractionChain(const csl::vector_expr& args,
                                   const Expr&         res)
    :resultOfContraction(res)
{
    if (res != CSL_UNDEF)
        specialContraction = true;
    for (const auto& arg : args) {
        if (not arg->isIndexed())
            scalarFactor = scalarFactor * arg;
        else {
            contractedTensors.push_back(arg);
            structures.push_back(arg->getIndexStructure());
        }
    }
    gatherContractions();
}

ContractionChain::ContractionChain(const Expr& expr,
                                   const Expr& res)
{
    if (expr->getType() == csl::Type::Prod) {
        *this = ContractionChain(expr->getVectorArgument(), res);
    }
    else if (not IsIndicialTensor(expr) and expr != CSL_0) {
        expr->print();
        callError(cslError::UndefinedBehaviour,
                "ContractionChain::ContractionChain()",
                "Only indicial tensors can be treated");
    }
    else {
        *this = ContractionChain(csl::vector_expr(1, expr), res);
    }
}

Expr ContractionChain::getResult() const
{
    if (not specialContraction)
        callError(cslError::UndefinedBehaviour,
                "ContractionChain::getResult()",
                "No defined contraction...");
    if (resultOfContraction == CSL_UNDEF and cycleTrace) {
        return scalarFactor * traceSpace->calculateTrace(contractedTensors); 
    }
    return resultOfContraction;
}

void ContractionChain::gatherContractions()
{
    for (size_t s1 = 0; s1 != structures.size(); ++s1) {
        for (size_t s2 = s1; s2 != structures.size(); ++s2) {
            for (size_t i1 = 0; i1 != structures[s1].size(); ++i1) {
                const Index index1 = structures[s1][i1];
                if (index1.getType() == cslIndex::Fixed)
                    continue;
                size_t start2 = (s1 == s2) ? i1 + 1 : 0;
                for (size_t i2 = start2; i2 != structures[s2].size(); ++i2) {
                    const Index index2 = structures[s2][i2];
                    if (index1 == index2) {
                        contraction.push_back(array<int,4>({(int)s1,
                                                            (int)i1,
                                                            (int)s2,
                                                            (int)i2}));
                        break;
                    }
                }
            }
        }
    }
}

vector<vector<int>> ContractionChain::getPermutations(
        const ContractionChain& other) const
{
    vector<vector<int>> permutations(1, vector<int>());
    for (size_t i = 0; i != contractedTensors.size(); ++i) {
        vector<vector<int>> newPermutations;
        for (size_t j = 0; j != other.contractedTensors.size(); ++j) {
            if (IsIndicialTensor(contractedTensors[i])
                    and IsIndicialTensor(other.contractedTensors[j])
                    and contractedTensors[i]->getParent()
                        == other.contractedTensors[j]->getParent()) {
                for (const auto& perm : permutations) {
                    auto pos = find(perm.begin(),
                                    perm.end(),
                                    j);
                    if (pos == perm.end()) {
                        newPermutations.push_back(perm);
                        newPermutations[newPermutations.size()-1].push_back(j);
                    }
                }
            }
        }
        if (newPermutations.empty())
            return newPermutations;
        permutations = std::move(newPermutations);
    }

    return permutations;
}

optional<ContractionChain> ContractionChain::createChain(
        int                nChains,
        const vector<int>& chainNumber) const
{
    for (int chain = 0; chain != nChains; ++chain) {
        map<int, int> indexMap;
        csl::vector_expr           newTensors;
        vector<IndexStructure> newStructure;
        vector<array<int, 4>>  newContraction;
        int nTensors = 0;
        bool hasSpecialProperty = false;
        csl::vector_expr remnants;
        for (size_t j = 0; j != contractedTensors.size(); ++j)
            if (chainNumber[j] == chain) {
                if (contractedTensors[j]->hasChainContractionProperty())
                    hasSpecialProperty = true;
                newTensors.  push_back(contractedTensors[j]);
                newStructure.push_back(structures[j]);
                indexMap[j] = nTensors++;
            }
            else
                remnants.push_back(contractedTensors[j]);
        if (not hasSpecialProperty)
            continue;
        for (const auto& cont : contraction) {
            int indexField1 = cont[0];
            int indexField2 = cont[2];
            if (chainNumber[indexField1] == chain) {
                array<int, 4> newCont({indexMap[indexField1],
                                       cont[1],
                                       indexMap[indexField2],
                                       cont[3]});
                newContraction.push_back(newCont);
            }
        }
        auto newT = newTensors;
        ContractionChain newChains = ContractionChain(newTensors,
                                                      newStructure,
                                                      newContraction);
        if (newChains.detectSpecialContraction()) {
            remnants.push_back(scalarFactor);
            remnants.push_back(newChains.scalarFactor);
            newChains.scalarFactor = prod_s(remnants, true);
            return newChains;
        }
    }

    return nullopt;
}

void ContractionChain::mergeChains(vector<int>& chainNumber,
                                   int          chain1,
                                   int          chain2)
{
    int chainMin = (chain1 < chain2) ? chain1 : chain2;
    for (auto& number : chainNumber) {
        if (number == chain1)
            number = chain2;
        if (number > chainMin)
            --number;
    }
}

optional<ContractionChain> ContractionChain::splitAndEvaluate() const
{
    int nChains = 0;
    vector<int> chainNumber(contractedTensors.size(), -1);
    for (const auto& cont : contraction) {
        int indexField1 = cont[0];
        int indexField2 = cont[2];
        if (chainNumber[indexField1] == -1) {
            if (chainNumber[indexField2] == -1) {
                chainNumber[indexField1] = nChains;
                chainNumber[indexField2] = nChains;
                ++nChains;
            }
            else
                chainNumber[indexField1] = chainNumber[indexField2];
        }
        else {
            if (chainNumber[indexField2] == -1)
                chainNumber[indexField2] = chainNumber[indexField1];
            else {
                int chain1 = chainNumber[indexField1];
                int chain2 = chainNumber[indexField2];
                chainNumber[indexField2] = chainNumber[indexField1];
                mergeChains(chainNumber, chain1, chain2);
            }
        }
    }

    return createChain(nChains, chainNumber);
}

bool ContractionChain::compareContractions(const array<int, 4>& c1,
                                           const array<int, 4>& c2) const
{
    return (
              (
                   c1[0] == c2[0]
               and c1[1] == c2[1]
               and c1[2] == c2[2]
               and c1[3] == c2[3]
              )
              or
              (
                   c1[0] == c2[2]
               and c1[1] == c2[3]
               and c1[2] == c2[0]
               and c1[3] == c2[1]
              )
           );
}

optional<vector<int>> ContractionChain::comparison(
        const ContractionChain& other,
        Expr&                   remnant) const
{
    remnant = CSL_UNDEF;
    if (contractedTensors.size() > other.contractedTensors.size()
            or contraction.size() > other.contraction.size())
        return nullopt;

    vector<vector<int>> permutations = getPermutations(other);
    for (const auto& perm : permutations) {
        bool matchTotal = true;
        vector<size_t> indicesLeft(other.contraction.size());
        for (size_t i = 0; i != indicesLeft.size(); ++i)
            indicesLeft[i] = i;

        for (size_t i = 0; i != contraction.size(); ++i) {
            bool matched = false;
            for (size_t j = 0; j != indicesLeft.size(); ++j) {
                size_t i2 = indicesLeft[j];
                array<int, 4> modifiedContraction(
                        {perm[contraction[i][0]],
                        contraction[i][1],
                        perm[contraction[i][2]],
                        contraction[i][3]}
                        );
                if (compareContractions(modifiedContraction,
                                        other.contraction[i2])) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
            }
            if (not matched) {
                matchTotal = false;
                break;
            }
        }
        if (matchTotal) {
            csl::vector_expr remnants;
            for (size_t i = 0; i != other.contractedTensors.size(); ++i)
                if (auto pos = find(perm.begin(), perm.end(), i);
                        pos == perm.end())
                    remnants.push_back(other.contractedTensors[i]);
            remnant = prod_s(remnants);
            return perm;
        }
    }
    return nullopt;
}

bool ContractionChain::operator==(const ContractionChain& other) const
// Return true if *this is a sub contraction of other
{
    Expr foo;
    return bool(comparison(other, foo));
}

bool ContractionChain::operator<(ContractionChain const& other) const
{
    return contraction.size() < other.contraction.size();
}

bool ContractionChain::detectSpecialContraction()
{
    bool checkCycle = false;
    for (size_t i = 0; i != structures.size(); ++i) {
        if (not checkCycle) {
            for (const auto& index : structures[i])
                if (index.getSpace()->keepCycles) {
                    checkCycle = true;
                    break;
                }
        }
    }
    if (checkCycle) {
        vector<const Space*> cycleSpaces;
        for (size_t i = 0; i != structures.size(); ++i) {
            for (const auto& index : structures[i])
                if (index.getSpace()->keepCycles)
                    if (auto pos = find(cycleSpaces.begin(),
                                        cycleSpaces.end(),
                                        index.getSpace());
                            pos == cycleSpaces.end())
                        cycleSpaces.push_back(index.getSpace());
        }
        for (const auto& space : cycleSpaces)
            if (detectCycle(space))
                return true;
    }
    for (size_t i = 0; i != contractedTensors.size(); ++i) {
        Expr& tensor = contractedTensors[i];
        if (tensor->hasChainContractionProperty()) {
            vector<ContractionChain> properties
                = tensor->getContractionProperties();
            for (const auto& cont : properties) {
                if (auto perm = cont.comparison(*this, resultOfContraction);
                        perm) {
                    auto& otherTensors = cont.contractedTensors;
                    map<Index, Index> replacement;
                    for (size_t i = 0; i != otherTensors.size(); ++i)
                    {
                        IndexStructure a = otherTensors[i]
                            ->getIndexStructure();
                        IndexStructure b = contractedTensors[perm.value()[i]]
                            ->getIndexStructure();
                        for (size_t j = 0; j != a.size(); ++j) {
                            if (replacement.find(a[j]) != replacement.end())
                                replacement.erase(a[j]);
                            else
                                replacement[a[j]] = b[j];
                        }
                    }
                    Expr res = cont.getResult();
                    RenameIndices(res);
                    for (const auto& r : replacement) {
                        res = Replaced(res, r.first, r.second, false);
                    }
                    resultOfContraction = scalarFactor
                                        * resultOfContraction
                                        * res;
                    specialContraction = true;
                    return true;
                }
            }
        }
    }
    return false;
}

bool isInContraction(
        std::vector<std::array<int, 4>> const &contractions,
        int iTensor,
        int iIndex
        )
{
    for (const auto &c : contractions)
        if ((c[0] == iTensor and c[1] == iIndex)
                or (c[2] == iTensor and c[3] == iIndex))
            return true;
    return false;
}

bool ContractionChain::detectCycle(const Space* space)
{
    std::vector<std::vector<size_t>> cycles = getCycles(space);
    for (const auto &cycle : cycles) {
        csl::vector_expr newTensors(cycle.size());
        size_t i = 0;
        for (size_t pos : cycle)
            newTensors[i++] = contractedTensors[pos];

        if (not space->hasSpecialTraceProperty(newTensors)) {
            continue;
        }
        std::vector<csl::IndexStructure> newStructures(cycle.size());
        for (size_t i = 0; i != cycle.size(); ++i)
            newStructures[i] 
                = contractedTensors[cycle[i]]->getIndexStructure();
        std::vector<Expr> remnants;
        remnants.reserve(contractedTensors.size());
        for (size_t i = 0; i != contractedTensors.size(); ++i)
            if (auto pos = std::find(cycle.begin(), cycle.end(), i);
                    pos == cycle.end())
                remnants.push_back(contractedTensors[i]);
        std::map<int, int> indexMap;
        for (int i = 0; i != int(cycle.size()); ++i) {
            indexMap[cycle[i]] = i;
        }
        vector<array<int, 4>>  newContraction;
        for (const auto& cont : contraction) {
            if (structures[cont[0]][cont[1]].getSpace() == space) {
                newContraction.push_back(
                        array<int, 4>({
                            indexMap[cont[0]],
                            cont[1],
                            indexMap[cont[2]],
                            cont[3]
                        }));
            }
        }

        remnants.push_back(scalarFactor);
        scalarFactor       = prod_s(remnants, true);
        cycleTrace         = true;
        traceSpace         = space;
        specialContraction = true;
        contractedTensors  = std::move(newTensors);
        structures         = std::move(newStructures);
        
        return true;
    }

    return false;
}

bool ContractionChain::isGoodIndex(
        csl::Space const *space,
        size_t            iTensor,
        size_t            iIndex) const
{
    return (structures[iTensor][iIndex].getSpace() == space
                and isInContraction(contraction, iTensor, iIndex));
}


std::pair<csl::Index, csl::Index> ContractionChain::getContractedIndices(
        csl::Space const *space,
        size_t            iTensor
        ) const
{
    size_t i(-1);
    size_t j(-1);
    for (size_t k = 0; k != structures[iTensor].size(); ++k)
        if (isGoodIndex(space, iTensor, k)) {
            if (i == size_t(-1))
                i = k;
            else
                j = k;
        }
    CSL_ASSERT_SPEC(i != size_t(-1) && j != size_t(-1),
            CSLError::RuntimeError,
            "Tensor " + toString(contractedTensors[iTensor]))

    return {structures[iTensor][i], structures[iTensor][j]};
}

std::vector<std::vector<size_t>> ContractionChain::getCycles(
        const Space *space
        )
{
    std::vector<size_t> indicesLeft(contractedTensors.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;
    for (size_t i = contractedTensors.size(); i --> 0 ;) {
        int nIndices = 0;
        for (size_t j = 0; j != structures[i].size(); ++j) 
            if (isGoodIndex(space, i, j)) {
                ++nIndices;
            }
        if (nIndices != 2) {
            indicesLeft.erase(indicesLeft.begin() + i);
        }
    }
    std::vector<std::vector<size_t>> res;
    res.reserve(3);
    while (!indicesLeft.empty()) {
        std::vector<size_t> tensors;
        tensors.reserve(10);
        size_t first = indicesLeft[0];
        tensors.push_back(first);
        indicesLeft.erase(indicesLeft.begin());
        auto [i, j] = getContractedIndices(space, first);
        for (size_t k = 0; k != indicesLeft.size(); ++k) {
            auto [i_2, j_2] = getContractedIndices(space, indicesLeft[k]);
            if (i_2 == j) {
                j = j_2;
                tensors.push_back(indicesLeft[k]);
                indicesLeft.erase(indicesLeft.begin() + k);
                k = size_t(-1);
            }
            else if (j_2 == j) {
                j = i_2;
                tensors.push_back(indicesLeft[k]);
                indicesLeft.erase(indicesLeft.begin() + k);
                k = size_t(-1);
            }
        }
        if (i == j) {
            res.push_back(tensors);
        }
    }

    return res;
}

ostream& operator<<(ostream& fout, const ContractionChain& c)
{
    fout << "Contraction of: ";
    c.scalarFactor->print();
    for (size_t i = 0; i != c.contractedTensors.size(); ++i)
        if (i != c.contractedTensors.size()-1)
            fout << c.contractedTensors[i] << ", ";
        else
            fout << c.contractedTensors[i] << endl;
    fout << " --> contractions: - ";
    for (size_t i = 0; i != c.contraction.size(); ++i) {
        for (const auto& el : c.contraction[i])
            fout << el << "  ";
        if (i != c.contraction.size()-1)
            fout << "\n                   - ";
        else
            fout << endl;
    }
    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Class TensorParent                        //
/*************************************************/
///////////////////////////////////////////////////

TensorParent::TensorParent():
    AbstractParent(),
    space(vector<const Space*>(0)),
    symmetry(),
    fullySymmetric(false),
    fullyAntiSymmetric(false),
    valued(false),
    tensor(CSL_0),
    trace(CSL_UNDEF),
    conjugateProperty(nullopt)
{}

TensorParent::TensorParent(const string& t_name)
    :AbstractParent(t_name),
    space(vector<const Space*>(0)),
    symmetry(),
    fullySymmetric(false),
    fullyAntiSymmetric(false),
    valued(false),
    tensor(CSL_0),
    trace(CSL_UNDEF),
    conjugateProperty(nullopt)
{}

TensorParent::TensorParent(const string& t_name,
                           const Space*  t_space)
    :AbstractParent(t_name),
    space(vector<const Space*>(1,t_space)),
    covariant(vector<bool>(1, false)),
    symmetry(),
    fullySymmetric(false),
    fullyAntiSymmetric(false),
    valued(false),
    tensor(CSL_0),
    trace(CSL_UNDEF),
    conjugateProperty(nullopt)
{
    valued = true;
    tensor = generateTensor(name,space);
}

TensorParent::TensorParent(const string&                    t_name,
                           const std::vector<const Space*>& t_space)
    :AbstractParent(t_name),
    space(t_space),
    covariant(vector<bool>(t_space.size(), false)),
    symmetry(),
    fullySymmetric(false),
    fullyAntiSymmetric(false),
    valued(false),
    tensor(CSL_0),
    trace(CSL_UNDEF),
    conjugateProperty(nullopt)
{
    valued = true;
    if (space.size() <= 4)
        tensor = generateTensor(name,space);
}

TensorParent::TensorParent(const string&                    t_name,
                           const std::vector<const Space*>& t_space,
                           const Expr&                      t_tensor)
    :AbstractParent(t_name),
    space(t_space),
    covariant(vector<bool>(t_space.size(), false)),
    symmetry(),
    fullySymmetric(false),
    fullyAntiSymmetric(false),
    valued(true),
    tensor(t_tensor),
    trace(CSL_UNDEF),
    conjugateProperty(nullopt)
{}

TensorParent::TensorParent(const string& t_name,
                           const Space*  t_space,
                           const Expr&   t_tensor)
    :TensorParent(t_name, vector<const Space*>(1, t_space), t_tensor)
{}

TensorParent::~TensorParent()
{}

cslParent::PrimaryType TensorParent::getPrimaryType() const
{
    return cslParent::Indicial;
}

cslParent::Type TensorParent::getType() const
{
    return cslParent::GenericIndicial;
}

void TensorParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Tensor " << regName << "("
        << "\"" << regLite << "\", {";
    for (size_t i = 0; i != space.size(); ++i) {
        out << csl::Abstract::regularName(space[i]->getName());
        if (i+1 != space.size())
            out << ", ";
    }
    out << "});\n";
    printPropDefinition(out, indentSize, header);
}

int TensorParent::getDim(const Space* t_space) const
{
    int dim = 0;
    for (const auto& s : space)
        if (s == t_space)
            ++dim;
    return dim;
}

vector<const Space*> TensorParent::getSpace() const
{
    return space;
}

Symmetry TensorParent::getSymmetry() const
{
    return symmetry;
}

bool TensorParent::getFullySymmetric() const {
    return fullySymmetric;
}
bool TensorParent::getFullyAntiSymmetric() const {
    return fullyAntiSymmetric;
}

bool TensorParent::isValued() const
{
    return valued;
}

Expr TensorParent::getTensor() const
{
    return tensor;
}

Expr TensorParent::getTensor(Expr_info self) const
{
    Expr res = DeepCopy(tensor);
    const auto& structure = self->getIndexStructureView();
    for (size_t i = 0; i != space.size(); ++i)
        if (space[i]->getSignedIndex()
                and covariant[i] == structure[i].getSign()) {
            // Bad index place, we apply metric.
            res = space[i]->applyMetricOnTensor(res, i, covariant[i]);
        }

    return res;
}

Expr TensorParent::getTrace() const
{
    return trace;
}

vector<Permutation> TensorParent::getPermutation() const {
    return symmetry.getPermutation();
}

void TensorParent::addSpace(const Space* t_space)
{
    space.push_back(t_space);
}

void TensorParent::setComplexProperty(csl::ComplexProperty t_prop)
{
    if (complexProp != t_prop) {
        AbstractParent::setComplexProperty(t_prop);
        if (valued)
            tensor->setComplexProperty(t_prop);
    }
}

void TensorParent::setKeepBestPermutation(bool keep)
{
    keepBestPermutation = keep;
}

void TensorParent::setFullySymmetric() {
    fullySymmetric = true;
    fullyAntiSymmetric = false;
}
void TensorParent::setFullyAntiSymmetric() {
    fullySymmetric = false;
    fullyAntiSymmetric = true;
}


bool TensorParent::isTraceLessIn(csl::Space const* t_space) const
{
    return std::find(
            traceLessNess.begin(),
            traceLessNess.end(),
            t_space) != traceLessNess.end();
}
void TensorParent::addTraceLessNess(csl::Space const* t_space)
{
    traceLessNess.push_back(t_space);
}
void TensorParent::removeTraceLessNess(csl::Space const* t_space) 
{
    auto pos = std::find(
            traceLessNess.begin(), 
            traceLessNess.end(), 
            t_space);
    if (pos != traceLessNess.end())
        traceLessNess.erase(pos);
}

void TensorParent::addSymmetry(int i1, int i2)
{
    if (fullySymmetric)
        return;
    if (fullyAntiSymmetric)
        fullyAntiSymmetric = false;

    int dim = space.size();
    if (i1 < 0 or i2 < 0 or
        i1 >= dim or i2 >= dim)
        callError(cslError::OutOfBounds,"TensorParent::addSymmetry(int i1, int i2)",
                (i1<0 or i1>=dim) ? i1 : i2);
    symmetry.addSymmetry(Permutation(dim,{i1,i2}), 1);
}
void TensorParent::addAntiSymmetry(int i1, int i2)
{
    if (fullyAntiSymmetric) return;
    if (fullySymmetric) fullySymmetric = false;
    int dim = space.size();
    if (i1 < 0 or i2 < 0 or
        i1 >= dim or i2 >= dim)
        callError(cslError::OutOfBounds,"TensorElement::addAntiSymmetry(int i1, int i2)",
                (i1<0 or i1>=dim) ? i1 : i2);
    symmetry.addSymmetry(Permutation(dim,{i1,i2}), -1);
}

bool TensorParent::dependsOn(Expr_info expr) const
{
    if (IsIndicialTensor(expr))
        return expr->getParent_info() == this;
    if (valued)
        return tensor->dependsOn(expr);
    return false;
}

bool TensorParent::dependsExplicitlyOn(Expr_info expr) const
{
    if (IsIndicialTensor(expr))
        return expr->getParent_info() == this;
    if (valued)
        return tensor->dependsExplicitlyOn(expr);
    return false;
}

const vector<Equation*>& TensorParent::getProperties() const
{
    return props;
}

void TensorParent::addSelfContraction(
        const Expr& A,
        const Expr& B,
        const Expr& res,
        optional<function<bool(Expr_info, Expr_info)>> condition)
{
    if (A->getNumericalFactor() != CSL_1) {
        addSelfContraction(
                A->getTerm().value(),
                B,
                res / A->getNumericalFactor(),
                condition);
        return;
    }
    if (B->getNumericalFactor() != CSL_1) {
        addSelfContraction(
                A,
                B->getTerm().value(),
                res / B->getNumericalFactor(),
                condition);
        return;
    }
    if (not IsIndicialTensor(A) or not IsIndicialTensor(B))
        callError(cslError::BadSelfContraction,
"TensorParent::addSelfContraction(const Expr&, const Expr&, const Expr&)");
    if (A->getParent_info() != this and B->getParent_info() != this)
        callError(cslError::BadSelfContraction,
"TensorParent::addSelfContraction(const Expr&, const Expr&, const Expr&)");
    if (A->getParent_info() != this) {
        addSelfContraction(B, A, res, condition);
        return;
    }

    // We get the contraction structure between A and B
    csl::vector_expr A_perm = getAllPermutations(A);
    csl::vector_expr B_perm = getAllPermutations(B);

    Expr factor = A->getNumericalFactor() * B->getNumericalFactor();
    SelfContraction c(A, B, condition);
    if (not c.empty())  {
        for (size_t iA = 0; iA != A_perm.size(); ++iA)
            for (size_t iB = 0; iB != B_perm.size(); ++iB) {
                Expr new_factor = (A_perm[iA]->getNumericalFactor()
                                 * B_perm[iB]->getNumericalFactor()) / factor;
                Expr tensorA = GetTerm(A_perm[iA]);
                Expr tensorB = GetTerm(B_perm[iB]);
                SelfContraction c(
                        tensorA,
                        tensorB,
                        condition);
                std::vector<std::pair<SelfContraction, Expr>>&
                    contraction = (c.isSwappable()) ? 
                    selfContraction : extContraction;
                auto pos = contraction.begin();
                while (pos != contraction.end()) {
                    if (c == pos->first) {
                        if (c.isSwappable())
                            break;
                        auto parent = externalTensors[
                            std::distance(contraction.begin(), pos)
                        ];
                        if (parent == B->getParent_info())
                            break;
                    }
                    ++pos;
                }
                if (pos == contraction.end()) {
                    contraction.push_back(make_pair(c, new_factor*res));
                    if (!c.isSwappable())
                        externalTensors.push_back(B->getParent_info());
                }
            }
    }
}

void TensorParent::removeSelfContraction(
        const Expr& A,
        const Expr& B
        )
{
    auto c = SelfContraction(A, B);
    std::vector<std::pair<SelfContraction, Expr>>&
        contraction = (c.isSwappable()) ?  selfContraction : extContraction;
    for (auto iter = contraction.begin();
            iter != contraction.end();
            ++iter) {
        if (iter->first == c and c == iter->first) {
            if (!c.isSwappable())
                externalTensors.erase(
                        externalTensors.begin()
                        + std::distance(contraction.begin(), iter));
            contraction.erase(iter);
            return;
        }
    }
}

bool TensorParent::hasChainContractionProperty() const
{
    return (not chainContraction.empty());
}

vector<ContractionChain> TensorParent::getContractionProperties() const
{
    return chainContraction;
}

void TensorParent::addContractionProperty(
        csl::vector_expr const& leftHandSide,
        const Expr& rightHandSide)
{
    // if (rightHandSide != CSL_0
    //         and leftHandSide->getFreeIndexStructure()
    //             != rightHandSide->getFreeIndexStructure())
    //     callError(cslError::UndefinedBehaviour,
    //             "TensorParent::addContractionProperty");
    //
    chainContraction.push_back(ContractionChain(prod_s(leftHandSide),
                                                rightHandSide));
}

void TensorParent::removeContractionProperty(
        csl::vector_expr const& leftHandSide,
        const Expr& rightHandSide)
{
    auto contraction = ContractionChain(prod_s(leftHandSide), rightHandSide);
    for (auto iter = chainContraction.begin();
            iter != chainContraction.end();
            ++iter)
        if (contraction == *iter) {
            chainContraction.erase(iter);
            return;
        }
}

void TensorParent::applyProperty(
        Expr_info self,
        IndexStructure const& structure,
        Expr& res) const
{
    res = DeepCopy(res);
    RenameIndices(res);
    auto initStructure = self->getIndexStructure();
    res = ReplaceIndices(res, structure, initStructure);
    // auto intermediate  = initStructure;
    // for (size_t i = 0; i != structure.size(); ++i)
    //     intermediate[i] = intermediate[i].rename();
    // for (size_t i = 0; i != structure.size(); ++i) {
    //     res = Replaced(res, structure[i], intermediate[i]);
    // }
    // for (size_t i = 0; i != structure.size(); ++i) {
    //     res = Replaced(res, intermediate[i], initStructure[i]);
    // }
}

optional<Expr> TensorParent::getComplexProperty(
        Expr_info self) const
{
    if (conjugateProperty) {
        auto structure = conjugateProperty.value().first;
        auto res = conjugateProperty.value().second;
        applyProperty(self, structure, res);
        return res;
    }
    return nullopt;
}

optional<Expr> TensorParent::getHermitianProperty(
        Expr_info  self,
        const Space* t_space) const
{
    if (auto pos = hermitianProperty.find(t_space);
            pos != hermitianProperty.end()) {
        auto structure = pos->second.first;
        auto res = pos->second.second;
        applyProperty(self, structure, res);
        return res;
    }
    return nullopt;
}

optional<Expr> TensorParent::getTransposedProperty(
        Expr_info  self,
        const Space* t_space) const
{
    if (auto pos = transposedProperty.find(t_space);
            pos != transposedProperty.end()) {
        auto structure = pos->second.first;
        auto res = pos->second.second;
        applyProperty(self, structure, res);
        return res;
    }
    return nullopt;
}

optional<Expr> TensorParent::evaluate(
        Expr_info self,
        csl::eval::mode
        ) const
{
    csl::IndexStructure const &index = self->getIndexStructureView();
    for (size_t i = 0; i != index.size(); ++i) 
        for (size_t j = 0; j != index.size(); ++j) 
            if (i != j and index[i] == index[j]) {
                if (fullyAntiSymmetric
                        or symmetry.getSymmetryOf(i, j) == -1
                        or isTraceLessIn(index[i].getSpace())) {
                    return  CSL_0;
                }
            }
    return std::nullopt;
}

void TensorParent::addComplexProperty(const Expr& init,
                                        const Expr& res)
{
    // const IndexStructure& structure = res->getFreeIndexStructure();
    // checkIndexRequest(structure.getIndex());
    conjugateProperty = make_pair(init->getIndexStructure(), res);
}

void TensorParent::addHermitianProperty(const Space* t_space,
                                          const Expr& init,
                                          const Expr&  res)
{
    if (getDim(t_space) > 2)
        callError(cslError::InvalidITensor,
                "addHermitianProperty()",
                "hermitian property for more than 2 indices");
    IndexStructure structure = init->getIndexStructure();
    int pos = -1;
    for (size_t i = 0; i != structure.size(); ++i) {
        if (structure[i].getSpace() == t_space) {
            if (pos == -1) {
                pos = i;
            }
            else {
                std::swap(structure[pos], structure[i]);
                hermitianProperty[t_space]
                    = make_pair(structure, res);
                return;
            }
        }
    }
    CALL_SMERROR_SPEC(
            CSLError::RuntimeError,
            "Tensor " + toString(init) + " invalid for hermitian property "
            "in space " + toString(t_space->getName()));
}

void TensorParent::addTransposedProperty(const Space* t_space,
                                           const Expr& init,
                                           const Expr&  res)
{
    if (getDim(t_space) != 2)
        callError(cslError::InvalidITensor,
                "addTransposedProperty()",
                "transposed property for more than 2 indices");
    IndexStructure structure = init->getIndexStructure();
    int pos = -1;
    for (size_t i = 0; i != structure.size(); ++i) {
        if (structure[i].getSpace() == t_space) {
            if (pos == -1) {
                pos = i;
            }
            else {
                std::swap(structure[pos], structure[i]);
                transposedProperty[t_space]
                    = make_pair(init->getIndexStructure(), res);
                return;
            }
        }
    }
    CALL_SMERROR_SPEC(
            CSLError::RuntimeError,
            "Tensor " + toString(init) + " invalid for transposed property "
            "in space " + toString(t_space->getName()));
}

void TensorParent::setSymmetry(const Symmetry& t_symmetry)
{
    if (t_symmetry == Symmetry())
        symmetry.clear();
    else if (t_symmetry.getDim() != (int)space.size())
        callError(cslError::BadSymmetry,
                "TensorParent::setSymmetry(const Symmetry& t_symmetry)");
    symmetry = t_symmetry;
    fullySymmetric = false;
    fullyAntiSymmetric = false;
}

void TensorParent::setTensor(const Expr& t_tensor)
{
    if (valued and not tensor->matchShape(t_tensor.get(), true)) {
        cout << *this << endl;
        t_tensor->print();
        callError(cslError::InvalidIndicialParent,
                "TensorParent::setTensor(const Expr&)");
    }
    size_t i = 0;
    for (const auto& dim : t_tensor->getShape()) {
        if (dim <= 1)
            continue;
        if (dim != space[i++]->getDim()) {
            cout << dim << "  " << space[i-1]->getDim() << endl;
            cout << *this << endl;
            t_tensor->print();
            callError(cslError::InvalidIndicialParent,
                    "TensorParent::setTensor(const Expr&)");
        }
    }
    valued = true;
    tensor = t_tensor;
}

void TensorParent::setTrace(const Expr& t_trace)
{
    trace = t_trace;
}

void TensorParent::setElementary(bool t_elementary)
{
    if (not valued)
        callError(cslError::UndefinedBehaviour,
                "TensorParent::setElementary(bool)");
    tensor->setElementary(t_elementary);
}

template<typename T>
typename vector<pair<T, Expr>>::iterator findContraction(
        typename vector<pair<T, Expr>>::iterator it,
        typename vector<pair<T, Expr>>::iterator last,
        T element
        )
{
    if (it == last)
        return last;
    if (it->first == element)
        return it;
    while (++it != last)
        if (it->first == element)
            return it;
    return last;
}

template<typename T>
typename vector<pair<T, Expr>>::const_iterator findContraction(
        typename vector<pair<T, Expr>>::const_iterator it,
        typename vector<pair<T, Expr>>::const_iterator last,
        T element
        )
{
    if (it == last)
        return last;
    if (it->first == element)
        return it;
    while (++it != last)
        if (it->first == element)
            return it;
    return last;
}

bool TensorParent::hasContractionProperty(
        const Abstract* self,
        Expr_info B) const
{
    if (IsIndicialTensor(B) 
            and B->getParent_info() == this
            and not selfContraction.empty()) {
        // We determine which contraction takes place between self and B
        // and see if it appears in the list of contractions that have special
        // properties.
        SelfContraction c(self, B);
        for (const auto& contraction : selfContraction) {
            if (contraction.first == c
                    and contraction.first.conditionAppliesOn(self, B))
                return true;
        }
    }
    else if (IsIndicialTensor(B)
            and B->getParent_info() != this
            and not extContraction.empty()) {
        SelfContraction c(self, B);
        for (size_t i = 0; i != extContraction.size(); ++i) 
            if (extContraction[i].first == c
                    and externalTensors[i] == B->getParent_info()
                    and extContraction[i].first.conditionAppliesOn(self, B))
                return true;
    }
    return false;
}

Expr TensorParent::contraction(const Abstract* self, Expr_info B) const
{
    // This function should be called only if the contraction property has
    // been verified with TensorParent::hasContractionProperty()

    SelfContraction c_expr(self, B);
    if (c_expr.isSwappable()) {
        for (const auto& c : selfContraction) {
            if (c.first == c_expr
                    and c.first.conditionAppliesOn(self, B)) {
                return c.first.applyIndices(
                        Copy(self), Copy(B), c_expr, c.second);
            }
        }
    }
    else {
        for (size_t i = 0; i != extContraction.size(); ++i) 
            if (extContraction[i].first == c_expr
                    and externalTensors[i] == B->getParent_info()
                    and extContraction[i].first.conditionAppliesOn(self, B)) {
                Expr result = extContraction[i].first.applyIndices(
                        Copy(self), Copy(B), c_expr, extContraction[i].second);
                return result;
            }
    }

    self->print();
    B->print();
    // We did not find the proper contraction: Error
    callError(cslError::NoContractionProperty,
            "TensorParent::contraction(const Expr&, const Expr&)");
    return CSL_UNDEF;
}

void TensorParent::checkIndexRequest(const Index& index)
{
    checkIndexRequest(vector<Index>(1, index));
}
void TensorParent::checkIndexRequest(const vector<Index>& request)
{
    if (request.size() != space.size()) {
        std::cout << "Indices :" << std::endl;
        for (const auto& i : request)
            std::cout << i << std::endl;
        cout << *this << endl;
        ostringstream sizeMismatch;
        sizeMismatch << request.size() << " given, " << space.size() << " requested.";
        callError(cslError::InvalidITensor,
    "TensorParent::checkIndexRequest(const vector<Index>& request) const"
    + (string)": size does not match: " + sizeMismatch.str());
    }
    for (auto index=request.begin(); index!=request.end(); ++index)
        if (index->getSpace() != space[distance(request.begin(), index)]) {
            std::cout << "Indices :" << std::endl;
            for (const auto& i : request)
                std::cout << i << std::endl;
            cout << *this << endl;
            callError(cslError::InvalidITensor,
    "TensorParent::checkIndexRequest(const vector<Index>& request) const"
    + (string)": space \"" + index->getSpace()->getName() + "\" mismatch.");
        }
}

void TensorParent::createFixedIndices(Index& index) const
{
    if (not (index.getType() == cslIndex::Fixed))
        return;
    if (space.size() > 0)
        index.setSpace(space[0]);
}

void TensorParent::createFixedIndices(vector<Index>& indices) const
{
    if (indices.size() != space.size())
        return;
    for (size_t i = 0; i != indices.size(); ++i)
        if (indices[i].getType() == cslIndex::Fixed)
            indices[i].setSpace(space[i]);
}

void fillKeptIndices(vector<vector<vector<int>>>   & keptIndices,
                     vector<size_t>::const_iterator posBroken,
                     vector<size_t>::const_iterator endPosBroken,
                     vector<vector<int>>     const& toInsert);

void fillKeptIndices(vector<vector<vector<int>>>  & keptIndices,
                     vector<size_t>::const_iterator posBroken,
                     vector<size_t>::const_iterator endPosBroken,
                     vector<size_t>          const& pieces)
{
    vector<vector<int>> toInsert(pieces.size());
    size_t pos = 0;
    for (size_t i = 0; i != pieces.size(); ++i) {
        toInsert[i] = vector<int>(pieces[i]);
        std::generate(toInsert[i].begin(),
                      toInsert[i].end(),
                      [&pos]() {
                          return pos++;
                      });
    }
    fillKeptIndices(keptIndices, posBroken, endPosBroken, toInsert);
    // const size_t N = sqrt(keptIndices.size());
    // for (size_t j = 0; j != N - 1; ++j)
    //     for (size_t i = j + 1; i != N; ++i)
    //         std::swap(keptIndices[i + j*N], keptIndices[i*N + j]);
    // std::sort(keptIndices.begin(),
    //           keptIndices.end(),
    //           [](const vector<vector<int>>& A,
    //              const vector<vector<int>>& B) {
    //               size_t maxi = min(A.size(), B.size());
    //               for (size_t i = 0; i != maxi; ++i) {
    //                 size_t maxi_recursive = min(A[i].size(), B[i].size());
    //                 for (size_t j = 0; j != maxi_recursive; ++j)
    //                   if (A[i][j] < B[i][j])
    //                     return true;
    //                   else if (A[i][j] > B[i][j])
    //                     return false;
    //                  if (A[i].size() < B[i].size())
    //                    return true;
    //                  else if (A[i].size() > B[i].size())
    //                    return false;
    //               }
    //               if (A.size() < B.size())
    //                 return true;
    //               return false;
    //           });
}
void fillKeptIndices(vector<vector<vector<int>>>   & keptIndices,
                     vector<size_t>::const_iterator posBroken,
                     vector<size_t>::const_iterator endPosBroken,
                     vector<vector<int>>     const& toInsert)
{
    if (posBroken == endPosBroken)
        return;

    const size_t keptInitSize = keptIndices.size();
    keptIndices.resize(keptInitSize * toInsert.size());
    for (size_t i = 0; i != toInsert.size(); ++i) {
        for (size_t j = 0; j != keptInitSize; ++j) {
            if (i > 0)
                keptIndices[i*keptInitSize + j] = keptIndices[j];
            keptIndices[i*keptInitSize + j][*posBroken] = toInsert[i];
        }
    }
    fillKeptIndices(keptIndices,
                    ++posBroken,
                    endPosBroken,
                    toInsert);
}

void fillNewSpaces(
        vector<vector<const Space*>> & newSpace,
        vector<size_t>::const_iterator posBroken,
        vector<size_t>::const_iterator endPosBroken,
        vector<const Space*>    const& fillingSpace);

vector<vector<const Space*>> fillNewSpaces(
        vector<const Space*>    const& init,
        vector<size_t>::const_iterator posBroken,
        vector<size_t>::const_iterator endPosBroken,
        vector<const Space*>    const& fillingSpace)
{
    vector<vector<const Space*>> newSpace(1, init);
    fillNewSpaces(newSpace, posBroken, endPosBroken, fillingSpace);
    for (auto& s : newSpace) {
        for (size_t i = 0; i != s.size(); ++i)
            if (not s[i]) {
                s.erase(s.begin() + i);
                --i;
            }
    }
    return newSpace;
}

void fillNewSpaces(
        vector<vector<const Space*>> & newSpace,
        vector<size_t>::const_iterator posBroken,
        vector<size_t>::const_iterator endPosBroken,
        vector<const Space*>    const& fillingSpace)
{
    if (posBroken == endPosBroken)
        return;

    const size_t initSize = newSpace.size();
    newSpace.resize(initSize * fillingSpace.size());
    for (size_t i = 0; i != fillingSpace.size(); ++i) {
        for (size_t j = 0; j != initSize; ++j) {
            if (i > 0)
                newSpace[i*initSize + j] = newSpace[j];
            newSpace[i*initSize + j][*posBroken] = fillingSpace[i];
        }
    }
    fillNewSpaces(newSpace,
                  ++posBroken,
                  endPosBroken,
                  fillingSpace);
}

std::vector<Parent> TensorParent::getBrokenParts(
        const Space* broken) const
{
    return brokenParts[broken];
}

std::string getBrokenName(
        std::string                   const &initName,
        std::vector<std::vector<int>> const &indices)
{
    std::string name = initName;
    if (!indices.empty())
        name += "_B_";
    for (size_t i = 0; i != indices.size(); ++i) {
        const auto &range = indices[i];
        if (range.size() > 1)
            name += toString(range[0]) + "_" + toString(range.back());
        else if (range[0] != -1)
            name += toString(range[0]);
        else
            name += "a";
        if (i + 1 != indices.size())
            name += "__";
    }

    return name;
}

vector<Parent> TensorParent::breakSpace(
        const Space*                broken,
        const vector<const Space*>& newSpaces,
        const vector<size_t>&       pieces
        ) const
{
    if (auto pos = brokenParts.find(broken);
            pos != brokenParts.end())
        return pos->second;

    if (std::accumulate(pieces.begin(), pieces.end(), 0)
            != broken->getDim())
        callError(cslError::UndefinedBehaviour,
                "TensorParent::breakSpace()");

    vector<Parent> res;
    vector<size_t> posBroken;
    vector<vector<vector<int>>> keptIndices;
    if (valued)
        keptIndices.push_back(
                vector<vector<int>>(space.size(), {-1}));
    for (size_t i = 0; i != space.size(); ++i)
        if (space[i] == broken) {
            posBroken.push_back(i);
        }
    if (valued)
        fillKeptIndices(keptIndices,
                        posBroken.begin(),
                        posBroken.end(),
                        pieces);

    size_t i = 0;
    vector<vector<const Space*>> spaceNewParents
        = fillNewSpaces(space, posBroken.begin(), posBroken.end(), newSpaces);
    for (const auto& updatedSpace : spaceNewParents) {
        if (getType() == cslParent::Delta
                and updatedSpace.size() == 2
                and updatedSpace[0] == updatedSpace[1]) {
            res.push_back(updatedSpace[0]->getDelta());
        }
        else if (getType() == cslParent::Delta) {
            if (updatedSpace.empty())
                res.push_back(tensor_s(
                            getBrokenName(name, keptIndices[i]), 
                            updatedSpace)
                        );
            else
                res.push_back(nullptr);
        }
        else if (getType() == cslParent::Metric
                and updatedSpace.size() == 2
                and updatedSpace[0] == updatedSpace[1]) {
            res.push_back(updatedSpace[0]->getMetric());
        }
        else if (getPrimaryType() == cslParent::Field){
            res.push_back(tensorfield_s(name, getFieldSpace(), updatedSpace));
        }
        else
            res.push_back(tensor_s(
                        getBrokenName(name, keptIndices[i]),
                        updatedSpace)
                    );
        if (res.back())
            res.back()->setSymmetry(Symmetry());
        if (valued) {
            Expr brokenTensor = tensor->getSubVectorial(keptIndices[i++]);
            if (brokenTensor == vectorialtensor_s(brokenTensor->getShape()))
                res.back() = nullptr;
            else
                res.back()->setTensor(brokenTensor);
        }
    }
    brokenParts[broken] = res;

    return res;
}

Expr TensorParent::operator()(Index index)
{
    // Operator() for TensorParent that generates an TensorElement of index index.
    // Like X(mu) for an TensorParent X returns an TensorElement.
    createFixedIndices(index);
    checkIndexRequest(index);

    Index copyIndex = index;
    if (keepBestPermutation)
        return tensorelement_s(copyIndex, self());
    return csl::make_shared<TensorElement>(copyIndex, self());
}

Expr TensorParent::operator()(const vector<int>& indices)
{
    return (*this)(integerToIndices(indices));
}

Expr TensorParent::operator()(vector<Index> indices)
{
    // Operator() for TensorParent that generates an TensorElement of indices
    // indices. Like g({mu,nu}) for an TensorParent g returns an TensorElement of
    // rank 2.
    createFixedIndices(indices);
    checkIndexRequest(indices);
    vector<Index> copyIndices(0);
    for (const auto& index : indices)
        copyIndices.push_back(index);

    if (keepBestPermutation)
        return tensorelement_s(copyIndices, self());
    return csl::make_shared<TensorElement>(copyIndices, self());
}

bool TensorParent::operator==(const TensorParent& other) const
{
    return (name == other.getName()
            and space == other.getSpace()
            and symmetry == other.getSymmetry()
            and (not valued or tensor == other.getTensor()));
}

bool TensorParent::operator!=(const TensorParent& other) const
{
    return not (other == *this);
}

std::ostream& operator<<(std::ostream& fout, const TensorParent& i)
{
    fout<<i.name;
    if (not i.commutable)
        fout<<", not commutable";
    fout<<": ";
    for (size_t j = 0; j != i.space.size(); ++j)
        fout<<i.space[j]->getName()<<"  ";
    fout<<"\n";
    if (i.symmetry != Symmetry())
        fout<<i.symmetry;
    if (i.valued)
        i.tensor->print();

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// MetricParent Class                            //
/*************************************************/
///////////////////////////////////////////////////

MetricParent::MetricParent(): TensorParent(){}

MetricParent::MetricParent(const DeltaParent& delta)
    :TensorParent("delta",
                    {delta.getSpace()[0], delta.getSpace()[0]},
                    delta.getTensor())
{}

// Constructor for a metric,
// By default 2 indices, symmetric, in the same space t_space.
MetricParent::MetricParent(const Space* t_space, const string& t_name)
    :TensorParent(t_name, {t_space,t_space})
{
    // Trace(metric) = D, D the space-time dimension
    trace = int_s(space[0]->getDim());
    // metric is fully sym-metric
    setFullySymmetric();
}

// Constructor for a metric,
// By default 2 indices, symmetric, in the same space t_space.
MetricParent::MetricParent(
        const Space* t_space, 
        const Expr& t_tensor,
        const string& t_name)
    :TensorParent(t_name, {t_space,t_space}, t_tensor)
{
    if (t_tensor->getType() != csl::Type::Matrix
            or t_tensor->getShape() != vector<int>(2,t_space->getDim())) {
        std::cout << t_tensor->getType() << std::endl;
        t_tensor->print();
        callError(cslError::InvalidIndicialParent,
                "TensorParent(const string&, const Space*, const Expr&)");
    }
    // Trace(metric) = D, D the space-time dimension
    trace = int_s(space[0]->getDim());
    valued = true;
    // metric is fully sym-metric
    setFullySymmetric();
}

cslParent::Type MetricParent::getType() const
{
    return cslParent::Metric;
}

bool MetricParent::hasContractionProperty(const Abstract* self,
                                          Expr_info B) const
{
    if (not Space::applyMetric)
        return false;
    if (not B->isIndexed())
        return false;
    return (not (self->getIndexStructure()[0].getType() == cslIndex::Fixed)
            or not (self->getIndexStructure()[1].getType() == cslIndex::Fixed));
}

Expr MetricParent::contraction(const Abstract* self, Expr_info B) const
{
    if (not hasContractionProperty(self,B)) {
        self->print();
        B->print();
        callError(cslError::NoContractionProperty,
                "TensorParent::contraction(Expr_info, Expr_info)");
    }
    if (self == B
            and self->getIndexStructure()[0].getType() == cslIndex::Dummy
            and self->getIndexStructure()[1].getType() == cslIndex::Dummy) {
        // Self contraction: returns d
        return getTrace();
    }
    IndexStructure self_struct = self->getIndexStructure();
    Expr copy_B = Copy(B);
    IndexStructure B_struct = copy_B->getIndexStructure();
    for (auto& B_index : B_struct)
        // For all indices in the contracted tensor, if it is contracted with
        // the metric (index present in self_struct) then we apply the metric
        // rule, i.e. raise or lower the index and remove the metric term.
        if (self_struct[0] == B_index) {
            copy_B = ReplaceIndex(copy_B, B_index,self_struct[1]);
            return copy_B;
        }
        else if (self_struct[1] == B_index) {
            copy_B = ReplaceIndex(copy_B, B_index,self_struct[0]);
            return copy_B;
        }
    // No contraction has been found: this function should be called only when
    // hasContractionProperty() has checked that there is indeed contraction
    // property.

    self->print();
    B->print();
    callError(cslError::NoContractionProperty,
            "TensorParent::contraction(const Expr&, const Expr&)");
    return CSL_UNDEF;
}

void MetricParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(
            name + "_" + space[0]->getName());
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Tensor " << regName << " = csl::GetMetric("
        << csl::Abstract::regularName(space[0]->getName()) << ");\n";
}

Expr MetricParent::operator()(vector<Index> indices)
{
    // unsigned space: no metric, we take delta
    if (not space[0]->getSignedIndex())
        return (*(*space[0]).getDelta())(indices);
    createFixedIndices(indices);
    checkIndexRequest(indices);

    vector<Index> copyIndices(0);
    for (const auto& index : indices)
        copyIndices.push_back(index);

    // Signed space : g^i_j = delta^i_j
    if (copyIndices[0].getSign() != copyIndices[1].getSign())
        return (*(*space[0]).getDelta())(indices);
    if (indices[0].getType() == cslIndex::Fixed
            and indices[1].getType() == cslIndex::Fixed)
        if (valued) {
            return tensor->getArgument({indices[0].getValue(),
                                        indices[1].getValue()});
        }

    return tensorelement_s(copyIndices, self());
}



///////////////////////////////////////////////////
/*************************************************/
// DeltaParent Class                            //
/*************************************************/
///////////////////////////////////////////////////

DeltaParent::DeltaParent(): TensorParent(){}

DeltaParent::DeltaParent(const Space* t_space)
    :TensorParent("delta", {t_space,t_space},identity_s(t_space->getDim()))
{
    trace = int_s(space[0]->getDim());
    valued = true;
    setFullySymmetric();
}

bool DeltaParent::hasContractionProperty(const Abstract* self,
                                         Expr_info B) const
{
    if (not Space::applyMetric)
        return false;
    if (not B->isIndexed())
        return false;
    return (not (self->getIndexStructure()[0].getType() == cslIndex::Fixed)
            or not (self->getIndexStructure()[1].getType() == cslIndex::Fixed));
}

cslParent::Type DeltaParent::getType() const
{
    return cslParent::Delta;
}

Expr DeltaParent::contraction(const Abstract* self, Expr_info B) const
{
    if (not hasContractionProperty(self,B)) {
        self->print();
        B->print();
        callError(cslError::NoContractionProperty,
                "TensorParent::contraction(Expr_info, Expr_info)");
    }
    if (self == B
            and self->getIndexStructure()[0].getType() == cslIndex::Dummy
            and self->getIndexStructure()[1].getType() == cslIndex::Dummy) {
        // Self contraction: returns d
        return getTrace();
    }
    IndexStructure self_struct = self->getIndexStructure();
    Expr copy_B = Copy(B);
    IndexStructure B_struct = copy_B->getIndexStructure();
    for (auto& B_index : B_struct)
        // For all indices in the contracted tensor, if it is contracted with
        // the delta (index present in self_struct) then we apply the delta
        // rule, i.e. raise or lower the index and remove the delta term.
        if (self_struct[0] == B_index) {
            copy_B = ReplaceIndex(copy_B, B_index,self_struct[1]);
            return copy_B;
        }
        else if (self_struct[1] == B_index) {
            copy_B = ReplaceIndex(copy_B, B_index,self_struct[0]);
            return copy_B;
        }
    // No contraction has been found: this function should be called only when
    // hasContractionProperty() has checked that there is indeed contraction
    // property.
    self->print();
    copy_B->print();
    callError(cslError::NoContractionProperty,
            "TensorParent::contraction(const Expr&, const Expr&)");
    return CSL_UNDEF;
}

void DeltaParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(
            name + "_" + space[0]->getName());
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Tensor " << regName << " = csl::GetDelta("
        << csl::Abstract::regularName(space[0]->getName()) << ");\n";
}

Expr DeltaParent::operator()(vector<Index> indices)
{
    createFixedIndices(indices);
    checkIndexRequest(indices);

    vector<Index> copyIndices(0);
    for (const auto& index : indices)
        copyIndices.push_back(index);

    // Signed space : delta^{ij} = g^{ij}, delta_{ij} = g_{ij}
    if (space[0]->getSignedIndex()
            and (copyIndices[0].getSign() == copyIndices[1].getSign()))
        return (*(*space[0]).getMetric())(indices);

    // If the two indices have special values, we return 0 or 1
    if (indices[0].getType() == cslIndex::Fixed
            and indices[1].getType() == cslIndex::Fixed) {
        if (indices[0].getValue() == indices[1].getValue())
            return CSL_1;
        return CSL_0;
    }

    return tensorelement_s(copyIndices, self());
}


///////////////////////////////////////////////////
/*************************************************/
// EpsilonParent Class                            //
/*************************************************/
///////////////////////////////////////////////////

EpsilonParent::EpsilonParent(const Space* t_space)
    :TensorParent(
            "eps",
            std::vector<const csl::Space*>(t_space->getDim(),t_space)
            )
{
    valued = true;
    setFullyAntiSymmetric();
    int dim = t_space->getDim();
    if (dim < 5) {
        vector<int> dimensions(dim, dim);
        Expr epsilonTensor = highdtensor_s(dimensions);
        fillEpsilonTensor(epsilonTensor, dim);
        setTensor(epsilonTensor);
    }
}

bool EpsilonParent::hasContractionProperty(
        const Abstract* self,
        Expr_info B) const
{
    return TensorParent::hasContractionProperty(self, B);
}

cslParent::Type EpsilonParent::getType() const
{
    return cslParent::Epsilon;
}

Expr EpsilonParent::contraction(const Abstract* self, Expr_info B) const
{
    Expr res = TensorParent::contraction(self, B);
    return DeepRefreshed(res);
}

void EpsilonParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(
            name + "_" + space[0]->getName());
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Tensor " << regName << " = csl::GetEpsilon("
        << csl::Abstract::regularName(space[0]->getName()) << ");\n";
}

Expr EpsilonParent::operator()(vector<Index> indices)
{
    createFixedIndices(indices);
    checkIndexRequest(indices);
    for (size_t i = 0; i != indices.size(); ++i)
        for (size_t j = i+1; j < indices.size(); ++j)
            if (indices[i] == indices[j])
                return CSL_0;

    return tensorelement_s(indices, self());
}

///////////////////////////////////////////////////
/*************************************************/
// Class TensorElement                                 //
/*************************************************/
///////////////////////////////////////////////////

TensorElement::TensorElement(const Index&          t_index,
                 const Parent& t_parent)
    :AbstractElement(t_parent),
    index({t_index})
{
    selfCheckIndexStructure();
}

TensorElement::TensorElement(const vector<Index>&  indices,
                 const Parent& t_parent)
    :AbstractElement(t_parent),
    index(indices)
{
    selfCheckIndexStructure();
}

TensorElement::TensorElement(const IndexStructure& indices,
                 const Parent& t_parent)
    :AbstractElement(t_parent),
    index(indices)
{
    selfCheckIndexStructure();
}

// Constructor crashes if expr is not an TensorElement
TensorElement::TensorElement(const Abstract*& expr)
    :TensorElement(expr->getIndexStructure(),
             expr->getParent())
{
    selfCheckIndexStructure();
}


// Constructor crashes if expr is not an TensorElement
TensorElement::TensorElement(const Expr& expr)
    :AbstractElement(expr->getParent()),
    index(expr->getIndexStructure())
{
    if (parent and parent->getPrimaryType() != cslParent::Indicial)
        callError(cslError::InvalidITensor,
                "TensorElement::TensorElement(const string&,bool,\
                const Index&,AbstractParent*const");
    selfCheckIndexStructure();
}

size_t TensorElement::memoryOverhead() const
{
    return index.size() * sizeof(csl::Index);
}

Index TensorElement::getIndex(int i) const
{
    if (i >= 0 and i < (int)index.size())
        return index[i];
    callError(cslError::OutOfBounds,"TensorElement::getIndex(int i) const",i);
    return Index();
}

bool TensorElement::askTerm(Expr_info expr, bool exact) const
{
    // If not exact, we simply need a dummy comparison of this and expr.
    if (not exact)
        return Comparator::freeIndexComparison(this, expr);
    return operator==(expr);
}

Expr& TensorElement::applySelfStructureOn(Expr& expr) const
{
    const IndexStructure& structure = expr->getFreeIndexStructure();
    if (not (index.size() == structure.size())) {
        print();
        expr->print();
        callError(cslError::InvalidITensor,
                "TensorElement::applySelfStructureOn()",
                "wrong number of free indices to apply structure");
    }
    expr = ReplaceIndices(expr, structure, index);
    // for (size_t i = 0; i != index.size(); ++i) {
    //     Index copy_index = index[i];
    //     expr = ReplaceIndex(expr, structure[i], copy_index);
    // }

    return expr;
}

optional<Expr> TensorElement::getComplexConjugate() const
{
    optional<Expr> conjugate = parent->getComplexProperty(this);
    if (conjugate)
        return conjugate.value();
    return AbstractElement::getComplexConjugate();
}

optional<Expr> TensorElement::getHermitianConjugate(const Space* t_space) const
{
    optional<Expr> hconjugate = parent->getHermitianProperty(this, t_space);
    if (hconjugate) {
        return GetTransposed(hconjugate.value(), t_space, false);
    }
    optional<Expr> cconjugate = parent->getComplexProperty(this);
    if (cconjugate)
        return GetTransposed(cconjugate.value(), t_space);
    optional<Expr> transposed = getTransposed(t_space);
    if (transposed)
        return GetComplexConjugate(transposed.value());
    return GetTransposed(GetComplexConjugate(copy()), t_space);
}

optional<Expr> TensorElement::getHermitianConjugate(
        const vector<const Space*>& t_space) const
{
    for (const auto& s : t_space) {
        optional<Expr> hconjugate = parent->getHermitianProperty(this, s);
        if (hconjugate) {
            return GetTransposed(hconjugate.value(), t_space, false);
        }
    }
    optional<Expr> cconjugate = parent->getComplexProperty(this);
    if (cconjugate)
        return GetTransposed(cconjugate.value(), t_space);
    optional<Expr> transposed = getTransposed(t_space);
    if (transposed)
        return GetComplexConjugate(transposed.value());
    return GetTransposed(GetComplexConjugate(copy()), t_space);
}

optional<Expr> TensorElement::getTransposed(
        const Space* t_space,
        bool         applyProp) const
{
    if (applyProp) {
        optional<Expr> conjugate = parent->getTransposedProperty(
                this, t_space);
        if (conjugate) {
            int pos = -1;
            IndexStructure newStructure(index);
            for (size_t i = 0; i != newStructure.size(); ++i)
                if (newStructure[i].getFree()
                        and newStructure[i].getSpace() == t_space) {
                    if (pos == -1)
                        pos = i;
                    else {
                        Replace(
                                *conjugate,
                                newStructure[pos],
                                newStructure[i]);
                        return *conjugate;
                    }
                }
            return *conjugate;
        }
    }
    int pos = -1;
    IndexStructure newStructure(index);
    for (size_t i = 0; i != newStructure.size(); ++i)
        if (newStructure[i].getFree()
                and newStructure[i].getSpace() == t_space) {
            if (pos == -1)
                pos = i;
            else {
                swap(newStructure[pos], newStructure[i]);
                return tensorelement_s(newStructure, parent);
            }
        }

    return nullopt;
}

optional<Expr> TensorElement::getTransposed(
        const vector<const Space*>& t_spaces,
        bool                        applyProp) const
{
    if (t_spaces.empty())
        return nullopt;
    optional<Expr> transposed;
    for (const auto& s : t_spaces) {
        Expr_info expr = ((transposed) ? transposed.value().get() : this);
        if (transposed)
            transposed = expr->getTransposed(s, applyProp)
                .value_or(*transposed);
        else
            transposed = expr->getTransposed(s, applyProp);
    }
    return transposed;
}

bool TensorElement::dependsOn(Expr_info expr) const
{
    return parent->dependsOn(expr);
}

bool TensorElement::dependsOn(Parent_info t_parent) const
{
    return parent.get() == t_parent;
}

bool TensorElement::dependsExplicitlyOn(Expr_info expr) const
{
    return parent->dependsExplicitlyOn(expr);
}

const std::vector<Equation*>& TensorElement::getProperties() const
{
    return parent->getProperties();
}

void TensorElement::addProperty(Equation* property)
{
    parent->addProperty(property);
}

void TensorElement::removeProperty(Equation* property)
{
    parent->removeProperty(property);
}

bool TensorElement::compareWithDummy(Expr_info       expr,
                               map<Index,Index>& constraints,
                               bool keepAllCosntraints) const
{
    // Comparison disregarding name of dummy indices, i.e. the two expressions
    // are equals even if dummy indices have not the same names in *this and
    // expr.
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (getType() != expr->getType())
        return false;
    if (not Complexified::operator==(expr))
        return false;
    if (parent.get() != expr->getParent_info())
        return false;
    return index.compareWithDummy(expr->getIndexStructureView(),
                                      constraints,
                                      keepAllCosntraints);
}

bool TensorElement::checkIndexStructure(const vector<Index>& t_indices) const
{
    const size_t nIndices = index.size();
    if (nIndices != t_indices.size())
        return false;
    vector<int> indicesLeft(nIndices);
    for (size_t i=0; i<nIndices;i++)
        indicesLeft[i] = i;

    for (size_t i=0; i<nIndices; i++) {
        if (index[i].getFree()) {
            bool matched = 0;
            for (size_t j=0; j<indicesLeft.size(); j++) {
                Index foo = t_indices[indicesLeft[j]];
                if (not foo.getFree() or index[i] == foo) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = 1;
                    break;
                }
            }
            if (!matched)
                return false;
        }
    }
    return true;
}

void TensorElement::selfCheckIndexStructure()
{
    for (size_t i=0; i!=index.size(); ++i)
        for (size_t j=i+1; j!=index.size(); ++j)
            if (index[i].testContraction(index[j])) {
                index[i].setFree(false);
                index[j].setFree(false);
            }
}

void TensorElement::adjustMetricDeltaParent()
{
    if (parent->getType() == cslParent::Metric) {
        if (index[0].getSpace()
                and index[0].getSign() != index[1].getSign())
            setParent(index[0].getSpace()->getDelta());
    }
    else if (parent->getType() == cslParent::Delta) {
        if (index[0].getSpace()
                and index[0].getSpace()->getSignedIndex()
                and index[0].getSign() == index[1].getSign())
            setParent(index[0].getSpace()->getMetric());
    }
}


void TensorElement::replaceIndexInPlace(
        Index const &oldIndex,
        Index const &newIndex)
{
    for (size_t i = 0; i != index.size(); ++i) {
        if (index[i].exactMatch(oldIndex)) {
            index[i] = newIndex;
            return;
        }
    }
}

optional<Expr> TensorElement::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndex,
        bool         refresh) const
{
    if (indexToReplace.exactMatch(newIndex))
        return nullopt;
    bool found = false;
    unique_Expr other = copy_unique();
    for (size_t i = 0; i != index.size(); ++i) 
        if (index[i].exactMatch(indexToReplace)) {
            found = true;
            other->getIndexStructureView()[i] = newIndex;
        }

    if (found) {
        if (refresh)
            return Refreshed(std::move(other));
        return Expr(std::move(other));
    }
    return nullopt;
}

optional<Expr> TensorElement::replaceIndices(
        std::vector<csl::Index> const &oldIndices,
        std::vector<csl::Index> const &newIndices,
        bool         refresh,
        bool         flipped) const
{
    bool found = false;
    unique_Expr other = copy_unique();
    for (size_t i = 0; i != oldIndices.size(); ++i) 
        if (!oldIndices[i].exactMatch(newIndices[i]))
            for (const auto& ind : index)
                if (ind.exactMatch(oldIndices[i])) {
                    found = true;
                    other->replaceIndexInPlace(oldIndices[i], newIndices[i]);
                }
    if (flipped)
        for (size_t i = 0; i != oldIndices.size(); ++i) 
            if (!oldIndices[i].exactMatch(newIndices[i]))
                for (const auto& ind : index)
                    if (ind.exactMatch(oldIndices[i].getFlipped())) {
                        found = true;
                        other->replaceIndexInPlace(
                                oldIndices[i].getFlipped(), 
                                newIndices[i].getFlipped());
                    }

    if (found) {
        if (refresh)
            return Refreshed(std::move(other));
        return Expr(std::move(other));
    }
    return nullopt;
}

void fillStructures(vector<IndexStructure>&        toFill,
                    const vector<Index>&           indices,
                    vector<size_t>::const_iterator pos,
                    vector<size_t>::const_iterator end,
                    size_t                         iter = 0)
{
    if (end == pos)
        return;
    const size_t n       = indices.size();
    const size_t divisor = pow(n, iter);
    for (size_t i = 0; i != toFill.size(); ++i) {
        const size_t posIndex = (i / divisor) % n;
        toFill[i][*pos] = indices[posIndex].rename();
    }
    fillStructures(toFill,
                   indices,
                   ++pos,
                   end,
                   ++iter);
}

void clearNullIndices(vector<IndexStructure>& toClear)
{
    Index nullIndex;
    for (auto& structure : toClear)
        for (size_t i = 0; i != structure.size(); ++i)
            if (structure[i] == nullIndex) {
                structure.erase(structure.begin()+i);
                --i;
            }
}

csl::vector_expr TensorElement::applyBrokenIndices(
        vector<Parent>&             brokenParents,
        const Space*                broken,
        const vector<const Space*>& newSpaces,
        const vector<Index>&        indices
        ) const
{
    vector<size_t> posBrokenIndices;
    posBrokenIndices.reserve(index.size());
    for (size_t i = 0; i != index.size(); ++i)
        if (index[i].getSpace() == broken)
            posBrokenIndices.push_back(i);
    vector<IndexStructure> correspondingStructure
        = vector<IndexStructure>(pow(newSpaces.size(),
                                     posBrokenIndices.size()),
                                 index);
    fillStructures(correspondingStructure,
                   indices,
                   posBrokenIndices.begin(),
                   posBrokenIndices.end());
    clearNullIndices(correspondingStructure);
    if (brokenParents.size() != correspondingStructure.size()) {
        print();
        cout << brokenParents.size() << " for "
             << correspondingStructure.size() << " structures.\n";
        callError(cslError::UndefinedBehaviour,
        "applyBrokenIndices()");
    }

    csl::vector_expr res(brokenParents.size());
    for (size_t i = 0; i != res.size(); ++i) {
        if (not brokenParents[i]) {
            res[i] = nullptr;
            continue;
        }
        if (correspondingStructure[i].empty()
                and getPrimaryType() != csl::PrimaryType::Field)
            res[i] = brokenParents[i]->getTensor();
        else {

            if (getPrimaryType() == csl::PrimaryType::Field) {
                if (not correspondingStructure[i].empty())
                    res[i] = brokenParents[i](
                            correspondingStructure[i].getIndex(),
                            getPoint());
                else
                    res[i] = brokenParents[i](getPoint());
            }
            else
                res[i] = brokenParents[i](
                        correspondingStructure[i].getIndex());
        }
        if (conjugated)
            res[i] = GetComplexConjugate(res[i]);
    }

    return res;
}

csl::vector_expr TensorElement::breakSpace(
        const Space*                brokenSpace,
        const vector<const Space*>& newSpaces,
        const vector<string>&       indexNames
        ) const
{
    if (parent->getDim(brokenSpace) == 0)
        return csl::vector_expr();
    vector<size_t> pieces(newSpaces.size(), 1);
    for (size_t i = 0; i != newSpaces.size(); ++i)
        if (newSpaces[i])
            pieces[i] = newSpaces[i]->getDim();
    vector<Parent> brokenParents = parent->breakSpace(
            brokenSpace,
            newSpaces,
            pieces);
    vector<Index> indices(newSpaces.size());
    for (size_t i = 0; i != indices.size(); ++i) {
        if (newSpaces[i])
            indices[i] = newSpaces[i]->generateIndex(indexNames[i]);
    }

    return applyBrokenIndices(brokenParents,
                              brokenSpace,
                              newSpaces,
                              indices);
}

void TensorElement::resetIndexStructure()
{
    index.reset();
}

void TensorElement::setIndexStructure(const IndexStructure& t_index)
{
    const size_t nIndices = index.size();
    if (nIndices != t_index.size())
        callWarning(cslError::InvalidDimension,
                "TensorElement::setIndexStructure(const std::vector<Index>&)",
                t_index.size());
    else
        index = IndexStructure(t_index);
    adjustMetricDeltaParent();
}

bool TensorElement::hasContractionProperty(Expr_info B) const
{
    return parent->hasContractionProperty(this,B);
}

vector<ContractionChain> TensorElement::getContractionProperties() const
{
    return parent->getContractionProperties();
}

Expr TensorElement::contraction(Expr_info B) const
{
    return parent->contraction(this,B);
}

bool TensorElement::hasChainContractionProperty() const
{
    if (parent->hasChainContractionProperty())
        return true;
    for (const auto& s : parent->getSpace())
        if (s->keepCycles)
            return true;
    return false;
}

Expr TensorElement::applyPermutation(const Permutation& permutation) const
{
    const int nIndices = index.size();
    if (nIndices != (int)permutation.size())
        callWarning(cslError::InvalidDimension,
                "TensorElement::applyPermutation(const vector<int>& permutations) const",
                permutation.size());
    else {
        IndexStructure newIndex(nIndices);
        for (int i=0; i!=nIndices; ++i)
            newIndex[i] = index[permutation[i]];
        Expr res = copy();
        res->setIndexStructure(newIndex);
        return res;
    }

    return CSL_0;
}

csl::vector_expr TensorElement::getPermutations(bool optimize) const
{
    csl::ScopedProperty p(&csl::option::fullComparison, true);
    csl::vector_expr res(1,Copy(this));
    if (getDim() == 1)
        return res;
    if (parent->getFullySymmetric() or parent->getFullyAntiSymmetric()) {
        if (!optimize) {
            const int nIndices = index.size();
            Permutation initPerm(nIndices);
            for (size_t i = 0; i != index.size(); ++i) 
                initPerm[i] = i;
            vector<Permutation> perm = permutations(initPerm);
            res = csl::vector_expr(0);
            bool getSign = parent->getFullyAntiSymmetric();
            for (size_t i=0; i!=perm.size(); ++i)
                if (getSign)
                    res.push_back(
                            prod_s(int_s(perm[i].getSign()),
                                   applyPermutation(perm[i]),
                                   true)
                            );
                else
                    res.push_back(applyPermutation(perm[i]));
            return res;
        }
        csl::IndexStructure copy_index(index);
        Permutation initPerm(index.size());
        for (size_t i = 0; i != index.size(); ++i) 
            initPerm[i] = i;
        bool sign = false;
        for (size_t i = 0; i+1 < index.size(); ++i) {
            size_t mini = i;
            for (size_t j = i+1; j < index.size(); ++j) {
                if (copy_index[j] < copy_index[mini]) {
                    mini = j;
                }
            }
            if (i != mini) {
                sign = !sign;
                std::swap(copy_index[i], copy_index[mini]);
                std::swap(initPerm[i],   initPerm[mini]);
            }
        }
        auto factor = ((parent->getFullyAntiSymmetric() && sign) ? CSL_M_1 : CSL_1) ;
        res[0]->setIndexStructure(copy_index);
        if (factor != CSL_1)
            res[0] *= factor;
        return res;
    }
    else {
        vector<Permutation> perm = parent->getPermutation();
        if (perm.size() > 0) {
            res.clear();
            for (size_t i=0; i!=perm.size(); ++i) {
                res.push_back(applyPermutation(perm[i]));
                if (perm[i].getSymmetry() == -1)
                    res[i] = prod_s(CSL_M_1, res[i], true);
            }
        }
        return res;
    }
}

bool TensorElement::comparePermutations(Expr_info perm1,
                                  Expr_info perm2) const
{
    if (IsIndicialTensor(perm1) xor
            IsIndicialTensor(perm2))
        return false;

    std::map<csl::Index, csl::Index> constraints;
    if (IsIndicialTensor(perm1)) {
        bool res = perm1->compareWithDummy(perm2, constraints);
        for (const auto& c : constraints)
            if (c.first != c.second)
                return false;
        return res;
    }
    bool res =
        perm1->getNumericalFactor()
             == perm2->getNumericalFactor()
        and (perm1->getTerm().value()->compareWithDummy(
                    perm2->getTerm().value().get(), constraints));
    for (const auto& c : constraints)
        if (c.first != c.second)
            return false;
    return res;
}

Expr TensorElement::getCanonicalPermutation() const
{
    csl::vector_expr permutations(getPermutations());
    if (permutations.empty())
        return Copy(this);
    size_t mini = 0;
    for (size_t i = 0; i+1 != permutations.size(); ++i)
        for (size_t j = i+1; j != permutations.size(); ++j)
            if (permutations[i] == CSL_0
                    or comparePermutations(permutations[i].get(),
                                         (-permutations[j]).get()))
                return CSL_0;
    for (size_t i = 1; i != permutations.size(); ++i)
        if (*permutations[i] < permutations[mini]) {
            mini = i;
        }

    return permutations[mini];
}

void TensorElement::print(
        int mode,
        std::ostream& out,
        bool) const
{
    const int nIndices = index.size();
    out<<getName();
    if (nIndices > 0) {
        out<<"_";
        out<<index;
    }
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void TensorElement::printCode(
        int,
        std::ostream &out
        ) const
{
    auto name = parent->getName();
    auto type = parent->getType();
    if (type == cslParent::Metric
            or type == cslParent::Delta
            or type == cslParent::Epsilon) {
        name += "_" + index[0].getSpace()->getName();
    }
    name = regularName(name);
    if (index.empty()) {
        if (conjugated)
            out << "csl::GetComplexConjugate(;";
        out << name << "()";
        if (conjugated)
            out << ")";
        return;
    }
    if (conjugated)
        out << "csl::GetComplexConjugate(;";
    out << name << "({";
    for (size_t i = 0; i != index.size(); ++i) {
        if (index[i].getSign())
            out << '+';
        out << index[i].getIndexCodeName();
        if (i+1 != index.size())
            out << ", ";
    }
    out << "})";
    if (conjugated)
        out << ")";
}

string TensorElement::printLaTeX(int mode) const
{
    const int nIndices = index.size();
    ostringstream sout;
    sout << getLatexName();
    if (nIndices > 0) {
        sout<<"_";
        sout<<index;
    }
    printProp(sout);
    if (mode == 0)
        sout<<endl;

    return sout.str();
}

std::vector<Parent> TensorElement::getSubSymbols() const
{
    auto const &pointed = *parent;
    if (typeid(pointed) == typeid(Abbreviation<TensorParent>)) {
        std::vector<Parent> dep = dynamic_cast<
            Abbreviation<TensorParent> const*>(
                parent.get())->getEncapsulated()->getSubSymbols();
        dep.push_back(parent);
        return dep;
    }
    return {parent};
}

optional<Expr> TensorElement::derive(Expr_info expr) const
{
    if (expr->getType() != getType() or parent != expr->getParent())
        return CSL_0;
    IndexStructure exprStruct = expr->getIndexStructureView();
    Expr res = CSL_1;
    for (size_t i = 0; i != index.size(); ++i)
        res = res * (*index[i].getSpace()->getDelta())
            ({index[i],exprStruct[i].getFlipped()});
    return res;
}

optional<Expr> TensorElement::evaluate(
        csl::eval::mode user_mode
        ) const
{
    if (auto parentEval = AbstractElement::evaluate(user_mode);
            parentEval)
        return parentEval;
    if (index.size() == 2) {
        Index i = index[0];
        Index j = index[1];
        if (i == j and i.getType() == cslIndex::Dummy)
            if (*parent->getTrace() != CSL_UNDEF)
                return parent->getTrace();
    }
    if (parent->isValued()) {
        bool allFixed = true;
        for (const auto& i : index)
            if (not (i.getType() == cslIndex::Fixed)) {
                allFixed = false;
                break;
            }
        if (allFixed) {
            vector<int> values;
            for (const auto& i : index) {
                if (i.getValue() >= 0)
                    values.push_back(i.getValue());
                else
                    values.push_back(-i.getValue());
            }
            return parent->getTensor(this)->getArgument(values);
        }
    }

    return nullopt;
}

unique_Expr TensorElement::copy_unique() const
{
    unique_Expr res = make_unique<TensorElement>(index, parent);
    applyComplexPropertiesOn(res);

    return res;
}

Expr TensorElement::refresh() const
{
    Expr res = tensorelement_s(index, parent);
    if (IsIndicialTensor(res)) {
        applyComplexPropertiesOn(res);
    }
    else if (res->getType() == csl::Type::Prod
            and res->size() == 2
            and IsIndicialTensor((*res)[1])) {
        applyComplexPropertiesOn((*res)[1]);
    }
    return res;
}

Expr TensorElement::deepRefresh() const
{
    return refresh();
}

bool TensorElement::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != getType())
        return false;
    if (not Complexified::operator==(expr))
        return false;

    if (parent != expr->getParent())
        return false;

    if (Comparator::getFreeIndexComparisonActive())
        return index.compareWithDummy(expr->getIndexStructureView(),
                Comparator::indexCorrespondance);
    else
        return index.exactMatch(expr->getIndexStructureView());
}

///////////////////////////////////////////////////
/*************************************************/
// Class ISum                                   //
/*************************************************/
///////////////////////////////////////////////////

ISum::ISum(): Sum()
{

}

ISum::ISum(const csl::vector_expr& t_argument, bool explicitSum)
    :Sum(t_argument, explicitSum)
{
    if (not explicitSum)
        selfCheckIndexStructure();
}

ISum::ISum(const Expr& leftOperand,
             const Expr& rightOperand,
             bool explicitSum)
    :Sum(leftOperand, rightOperand, explicitSum)
{
    if (not explicitSum)
        selfCheckIndexStructure();
}

bool ISum::isIndexed() const
{
    return true;
}

IndexStructure ISum::getIndexStructure() const
{
    if (argument.size() > 0)
        return argument[0]->getIndexStructure();
    return IndexStructure();
}

void ISum::selfCheckIndexStructure()
{
    // Checking if all terms have the same indexStructure.
    if (size() == 0) return;
    IndexStructure structure;
    size_t i = 0;
    do {
        structure = argument[i]->getFreeIndexStructure();
    } while (argument[i] == CSL_0 and i++ != argument.size());
    if (i == argument.size())
        return;
    for (auto arg=argument.begin(); arg!=argument.end(); ++arg)
        if (**arg != CSL_0
                and structure != (**arg).getFreeIndexStructure()) {
            DeepRefresh(*arg);
            DeepRefresh(argument[i]);
            if (*arg == CSL_0 or argument[i] == CSL_0) {
                mergeTerms();
                selfCheckIndexStructure();
                return;
            }
            std::cout << argument[i] << std::endl;
            std::cout << *arg << std::endl;
            cout << structure << endl;
            cout << (**arg).getFreeIndexStructure() << endl;
            callError(cslError::InvalidIndicialSum,
                    "Sum::selfCheckIndexStructure() const");
        }
}

optional<Expr> ISum::getHermitianConjugate(const Space* space) const
{
    return getHermitianConjugate(vector<const Space*>(1,space));
}

optional<Expr> ISum::getHermitianConjugate(const vector<const Space*>& space) const
{
    vector<optional<Expr>> newArg(argument.size());
    bool hermitian = true;
    for (size_t i = 0; i != argument.size(); ++i) {
        newArg[i] = argument[i]->getHermitianConjugate(space);
        if (hermitian or newArg[i])
            hermitian = false;
    }
    if (not hermitian) {
        Expr res = copy();
        for (size_t i = 0; i != argument.size(); ++i)
            res->setArgument(chooseOptional(newArg[i], argument[i]), i);
        return res;// ->refresh();
    }

    return nullopt;
}

optional<Expr> ISum::getTransposed(const Space* space,
                                   bool         applyProp) const
{
    return getTransposed(vector<const Space*>(1, space), applyProp);
}

optional<Expr> ISum::getTransposed(
        const vector<const Space*>& space,
        bool                        applyProp) const
{
    vector<optional<Expr>> newArg(argument.size());
    bool symmetric = true;
    for (size_t i = 0; i != argument.size(); ++i) {
        newArg[i] = argument[i]->getTransposed(space, applyProp);
        if (symmetric or newArg[i])
            symmetric = false;
    }
    if (not symmetric) {
        Expr res = copy();
        for (size_t i = 0; i != argument.size(); ++i)
            res->setArgument(chooseOptional(newArg[i], argument[i]), i);
        return res->refresh();
    }

    return nullopt;
}

optional<Expr> ISum::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndices,
        bool         refresh) const
{
    if (indexToReplace.exactMatch(newIndices))
        return nullopt;
    bool contraction = false;
    csl::vector_expr newArg(argument.size());
    size_t i = 0;
    for (auto const& arg : argument) {
        optional<Expr> opt =
            arg->replaceIndex(indexToReplace, newIndices, refresh);
        newArg[i++] = opt.value_or(CSL_0);
        if (opt) {
            contraction = true;
        }
    }
    if (contraction)
        return sum_s(newArg, not refresh);
    return std::nullopt;
}

optional<Expr> ISum::replaceIndices(
        std::vector<csl::Index> const &oldIndices,
        std::vector<csl::Index> const &newIndices,
        bool         refresh,
        bool         flipped) const
{
    bool contraction = false;
    csl::vector_expr newArg(argument.size());
    size_t i = 0;
    for (auto const& arg : argument) {
        optional<Expr> opt = arg->replaceIndices(
                oldIndices, newIndices, refresh, flipped
                );
        newArg[i++] = opt.value_or(CSL_0);
        if (opt) {
            contraction = true;
        }
    }
    if (contraction)
        return sum_s(newArg, not refresh);
    return std::nullopt;
}

void uniformizeIndices(csl::vector_expr& terms)
{
    if (terms.empty())
        return;
    IndexStructure structure = terms[0]->getFreeIndexStructure();
    for (size_t i = 1; i != terms.size(); ++i) {
        IndexStructure other = terms[i]->getFreeIndexStructure();
        for (size_t j = 0; j != structure.size(); ++j)
            terms[i] = Replaced(terms[i], other[j], structure[j]);
    }
}

csl::vector_expr ISum::breakSpace(
        const Space*                     brokenSpace,
        const std::vector<const Space*>& newSpaces,
        const std::vector<std::string>&  indexNames
        ) const
{
    if (argument.empty())
        return csl::vector_expr();
    if (csl::Abbrev::getFreeStructure(getIndexStructure()).size() == 0) {
        csl::vector_expr tot;
        for (size_t i = 0; i != argument.size(); ++i) {
            csl::vector_expr inter = argument[i]
                ->breakSpace(brokenSpace, newSpaces, indexNames);
            if (inter.empty())
                tot.push_back(argument[i]);
            else
                tot.insert(tot.end(), inter.begin(), inter.end());
        }
        return csl::vector_expr(1, Expanded(sum_s(tot)));
    }
    csl::vector_expr intermediate = argument[0]
        ->breakSpace(brokenSpace, newSpaces, indexNames);
    vector<csl::vector_expr> summed(max(size_t(1), intermediate.size()));
    for (size_t i = 0; i != summed.size(); ++i)
        summed[i] = csl::vector_expr(argument.size());

    bool broken = false;
    for (size_t i = 0; i != argument.size(); ++i) {
        csl::vector_expr intermediate = argument[i]
            ->breakSpace(brokenSpace,
                         newSpaces,
                         indexNames);
        if (intermediate.empty()) {
            intermediate = csl::vector_expr(summed.size());
            for (size_t j = 0; j != summed.size(); ++j)
                intermediate[j] = Copy(argument[i]);
        }
        else
            broken = true;
        for (size_t j = 0; j != intermediate.size(); ++j)
            summed[j][i] = intermediate[j];
    }
    if (broken) {
        csl::vector_expr res(summed.size());
        for (size_t i = 0; i != res.size(); ++i) {
            uniformizeIndices(summed[i]);
            res[i] = sum_s(summed[i]);
        }
        return res;
    }
    return csl::vector_expr();
}


void ISum::checkIndicialFactors(csl::vector_expr& factors) const
{
    Expr copy = Copy(this);
    csl::vector_expr copyFactors(factors);
    IndexStructure totStruct = copy->getIndexStructure();
    IndexStructure freeStruct = totStruct.getFreeStructure();
    for (size_t i = 0; i != factors.size(); ++i) {
        if (not factors[i]->isIndexed())
            continue;
        IndexStructure structure = factors[i]->getIndexStructure();
        for (size_t index = 0; index != structure.size(); ++index) {
            if (structure[index].getFree() and
                    freeStruct.find(structure[index]) == freeStruct.end())
                copyFactors[i] = ReplaceIndex(copyFactors[i], 
                        structure[index],
                        !structure[index]);
        }

        for (const auto& arg : *copy)
            if (not arg->askTerm(copyFactors[i].get())) {
                factors.erase(factors.begin()+i);
                --i;
                break;
            }
    }
}

void ISum::gatherFactors(
        csl::vector_expr& factors,
        csl::vector_expr& arg,
        bool full
        ) const
{
    if (argument.empty())
        return;
    arg = argument;
    for (size_t i = 0; i != argument.size(); ++i) 
        arg[i] = DeepCopy(argument[i]);
    // If full, we factor first the arguments independently
    if (full) {
        for (size_t i = 0; i < size(); i++)
            Factor(arg[i], full);
    }

    size_t mini = 0;
    size_t nFactorsMini = arg[0]->getNFactor();
    size_t nFactorsTot = nFactorsMini;
    for (size_t i = 1; i < argument.size(); i++) {
        size_t nF = arg[i]->getNFactor();
        if (!GetCommutable(arg[mini])
                or (GetCommutable(arg[i]) 
                and nFactorsMini >nF )) {
            mini = i;
            nFactorsMini = nF;
        }
        nFactorsTot += nF;
    }
    if (!GetCommutable(arg[mini])) {
        return;
    }
    factors = arg[mini]->getFactors();

    bool checkIndices = isIndexed();
    csl::vector_expr indicialFactors;
    indicialFactors.reserve(std::min(size_t(1000), nFactorsTot));
    if (factors.size() == 0)
        factors = csl::vector_expr(1,arg[mini]);
    for (size_t i = 0; i < size(); i++) {
        if (i != mini) {
            for (size_t j=0; j<factors.size(); j++) {
                if (!arg[i]->askTerm(factors[j].get())) {
                    if (checkIndices and factors[j]->isIndexed())
                        indicialFactors.push_back(factors[j]);
                    factors.erase(factors.begin()+j);
                    j--;
                }
            }
            if (factors.size() == 0)
                break;
        }
    }
    for (auto& f : factors)
        f = Copy(f);
    if (not indicialFactors.empty()) {
        checkIndicialFactors(indicialFactors);
    }
    std::sort(factors.begin(), factors.end());
}

void ISum::clearRedundantIndicialFactors(csl::vector_expr& factors) const
{
    if (factors.size() == 0)
        return;
    for (size_t i = 0; i < factors.size()-1; ++i) {
        if (not IsIndicialTensor(factors[i]))
            continue;
        for (size_t j = i+1; j != factors.size(); ++j) {
            map<Index, Index> constraints;
            if (factors[i]->compareWithDummy(factors[j].get(), constraints)) {
                factors.erase(factors.begin() + j);
                --j;
            }
        }
    }
}

optional<Expr> ISum::factor(bool full) const
{
    if (argument.empty())
        return CSL_0;

    csl::vector_expr arg;
    csl::vector_expr factors;
    gatherFactors(factors, arg, full);
    clearRedundantFactors(factors);
    clearRedundantIndicialFactors(factors);
    if (factors.empty())
        return nullopt;

    //std::sort(factors.begin(), factors.end());

    Expr sum = copy();
    Expr product = CSL_1;
    for (const auto& f : factors) {
        Expr expanded = Factored(sum, f.get());
        if (expanded->getType() == csl::Type::Prod) {
            bool toDevelop = false;
            for (size_t i = 0; i != expanded->size(); ++i) {
                if (expanded[i]->getType() == csl::Type::Sum)  {
                    if (expanded[i] != f) {
                        toDevelop = true;
                        sum = expanded[i];
                    }
                    else
                        product = product * expanded[i];
                }
                else
                    product = product * expanded[i];
            }
            if (toDevelop)
                return Factored(sum) * product;
            else
                return expanded;
        }
    }
    if (product != CSL_1)
        return sum * product;
    return sum;
}

bool ISum::compareWithDummy(Expr_info        expr,
                             map<Index, Index>& constraints,
                             bool) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    const size_t sz = size();
    if (sz == 1)
        return *argument[0]==expr;
    const auto exprType = expr->getType();
    if (exprType == csl::Type::Polynomial)
        return *this==expr->getRegularExpression().get();
    if (exprType != csl::Type::Sum)
        return false;
    if (sz != expr->size())
        return false;
    vector<size_t> indicesLeft(sz);
    for (size_t i = 0; i < sz;i++)
        indicesLeft[i] = i;

    map<Index, Index> copyConstraints = constraints;
    for (size_t i = 0; i < sz; i++) {
        auto const &arg = argument[i];
        bool matched = false;
        for (size_t j = 0; j < indicesLeft.size(); j++) {
            Expr const &foo = expr->getArgument(indicesLeft[j]);
            const bool argIndicial = IsIndicialTensor(arg);
            if ((not argIndicial) and *arg == foo.get()) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
            else if (argIndicial and IsIndicialTensor(foo)) {
                if (arg->compareWithDummy(foo.get(),
                                                  constraints,
                                                  true)) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
            }
            else if (arg->getType() == csl::Type::Prod
                    and arg->isIndexed())
                if (arg->compareWithDummy(foo.get(),
                                          constraints,
                                          true)) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
        }
        if (not matched) {
            return false;
        }
    }
    return true;
}

bool ISum::operator==(Expr_info expr) const
{
    map<Index, Index> constraints;
    if (not compareWithDummy(expr, constraints)) {
        return false;
    }
    if (not constraints.empty()) {
        if (Comparator::getFreeIndexComparisonActive())
            return true;
        for (const auto& c : constraints)
            if (c.first != c.second)
                return false;
    }

    return true;
}

///////////////////////////////////////////////////
/*************************************************/
// Class IProd                                  //
/*************************************************/
///////////////////////////////////////////////////

IProd::IProd(): Prod(){}

IProd::IProd(const csl::vector_expr& t_argument,
             bool                explicitProd)
{
    argument = t_argument;
    if (not option::freezeMerge and not explicitProd and size() > 0) {
        mergeProducts();
        selfCheckIndexStructure();
        if (option::applyChainContractions and not explicitProd)
            applyContractionChains();
    }
    else {
        mergeProducts();
        selfCheckIndexStructure(true);
    }
}

IProd::IProd(const Expr& leftOperand,
             const Expr& rightOperand,
             bool        explicitProd)
{
    if (not option::freezeMerge
        and not explicitProd
        and leftOperand->getType() == csl::Type::Prod
        and rightOperand->getType() != csl::Type::Prod)
    {
        argument = leftOperand->getVectorArgument();
        insert(rightOperand, true); // rightOperand inserted to the right
                                    // of leftOperand
    }
    else if (not option::freezeMerge
             and not explicitProd
             and rightOperand->getType() == csl::Type::Prod
             and leftOperand->getType() != csl::Type::Prod)
    {
        argument = rightOperand->getVectorArgument();
        insert(leftOperand, false); // leftOperand inserted to the left
                                    // of rightOperand
    }
    else {
        argument = csl::vector_expr(2);
        argument[0] = leftOperand;
        argument[1] = rightOperand;
        mergeProducts();
    }
    selfCheckIndexStructure(option::freezeMerge or explicitProd);
    if (not option::freezeMerge
            and not explicitProd
            and option::applyChainContractions) {
        applyContractionChains();
    }
}

bool IProd::isIndexed() const
{
    return true;
}

IndexStructure IProd::getIndexStructure() const
{
    IndexStructure structure;
    for (size_t i = 0; i != size(); ++i) {
        if (argument[i] == CSL_0)
            return csl::IndexStructure();
        IndexStructure s = argument[i]->getIndexStructure();
        structure += s;
    }

    return structure;
}

Expr IProd::suppressTerm(Expr_info term) const
{
    if (not term->isIndexed() or term->getIndexStructure().size() == 0)
        return Prod::suppressTerm(term);
    map<Index,Index> constraints;
    bool matched = false;
    vector<Expr> newArgs;
    newArgs.reserve(size());
    for (size_t i = 0; i < size(); i++) {
        if (not matched) {
            if (term->getType() != csl::Type::Pow
                    and term->compareWithDummy(argument[i].get(),
                                               constraints)) {
                matched = true;
            }
            else
                newArgs.push_back(Copy(argument[i]));
        }
        else
            newArgs.push_back(Copy(argument[i]));
    }
    if (matched) {
        for (const auto& el : constraints) {
            Index oldIndex = el.second;
            Index newIndex = el.first;
            if (newIndex.getSpace()->getSignedIndex()) {
                newIndex.setSign(not newIndex.getSign());
                oldIndex.setSign(not oldIndex.getSign());
            }
            Index foo = oldIndex.rename();
            for (auto &arg : newArgs) {
                arg = ReplaceIndex(arg, oldIndex, foo);
                arg = ReplaceIndex(arg, newIndex, oldIndex);
                if (newIndex.getSpace()->getSignedIndex())
                    arg = ReplaceIndex(arg, 
                            newIndex.getFlipped(),
                            oldIndex.getFlipped());
                arg = ReplaceIndex(arg, foo, newIndex);
            }
        }
    }

    return prod_s(newArgs);
}

void IProd::setArgument(const Expr& t_argument, int iArg)
{
    if (iArg < 0 or iArg >= (int)argument.size()) {
        print();
        callError(cslError::OutOfBounds,
                "Expr& AbstractMultiFunc::getArgument(int iArg) const", iArg);
    }

    argument[iArg] = t_argument;
    //selfCheckIndexStructure();
}

void keep_duplicates(vector<Index>& vec)
{
    if (vec.empty())
        return;
    for (size_t i = 0; i != vec.size(); ++i)
        if (vec[i].getType() == cslIndex::Fixed) {
            vec.erase(vec.begin()+i);
            --i;
        }
    Index value = vec[0];
    size_t pos = 0;
    bool found = false;
    if (vec.size() == 1) {
        vec.clear();
        return;
    }
    for (size_t i = 1; i != vec.size(); ++i) {
        if (vec[i] == value) {
            found = true;
            vec.erase(vec.begin() + pos);
            --i;
        }
        else {
            if (not found) {
                vec.erase(vec.begin() + pos);
                --i;
            }
            pos = i;
            value = vec[pos];
            found = false;
        }
    }
    if (vec.size() > 0 and not found) {
        vec.erase(vec.end()-1);
    }
}

optional<Expr> IProd::evaluate(csl::eval::mode user_mode) const
{
    if (eval::isContained(user_mode, eval::indicial)) {
        Expr copy_arg = copy();
        for (auto& arg : *copy_arg)
            arg = Evaluated(arg, user_mode);
        vector<Index> vec;
        for (const auto& term : *copy_arg) {
            vector<Index> l;
            if (term->getType() != csl::Type::Pow)
                l = term->getIndexStructure().getIndex();
            vec.insert(vec.end(), l.begin(), l.end());
        }
        std::sort(vec.begin(), vec.end());
        keep_duplicates(vec);
        if (vec.empty())
            return Refreshed(copy_arg);
        // Here vec contains uniquely indices in the product

        list<size_t> maxima;
        for (auto& i : vec) {
            maxima.push_back(i.getSpace()->getDim());
        }

        counter<Index> index_counter(maxima);
        index_counter.setCount(vec.begin());
        index_counter.restart();

        csl::vector_expr expressions;
        do {
            Expr partial = copy_arg;
            size_t i = 0;
            for (const auto& index : index_counter) {
                partial = Replaced(partial,
                                  vec[i],
                                  index,
                                  false);
                if (vec[i].getSpace()->getSignedIndex())
                    partial = Replaced(partial,
                                      vec[i++].getFlipped(),
                                      index.getFlipped(),
                                      false);
            }
            partial = Evaluated(partial, user_mode);
            if (partial != CSL_0)
                expressions.push_back(Evaluated(partial, user_mode));
        } while (++index_counter);

        return sum_s(expressions);
    }
    else
        return Prod::evaluate(user_mode);
}

void contract(Expr &arg1,
              Expr &arg2,
              csl::Index const &index1,
              csl::Index const &index2)
{
      arg1 = ContractIndex(arg1, index1);
      arg2 = ContractIndex(arg2, index2);
      if (arg1 == CSL_0 or arg2 == CSL_0)
          arg1 = CSL_0;
      else
          arg1 = arg1->contraction(arg2.get());
      arg2 = CSL_1;
}

void IProd::selfCheckIndexStructure(bool explicitTimes)
{
  bool tensorContracted = false;
  vector<IndexStructure> indexArgument(argument.size());
  size_t iArg = 0;
  for (auto arg=argument.begin(); arg!=argument.end(); ++arg) {
    if (not explicitTimes and IsIndicialTensor(*arg)) {
        optional<Expr> eval = (*arg)->evaluate();
        if (eval) {
            tensorContracted = true;
            *arg = eval.value();
        }
    }
    IndexStructure fooStruct = (*arg)->getIndexStructure();
    if ((*arg)->isIndexed()) {
      for (size_t k = 0; k != fooStruct.size(); ++k) {
        auto arg2 = argument.begin();
        for (auto iterIndex = indexArgument.begin();
                iterIndex != indexArgument.end();
                ++iterIndex, ++arg2) {
          if (!(*arg2)->isIndexed())
              continue;
          bool breakValue = false;
          for (auto index = iterIndex->begin();
                  index != iterIndex->end();
                  ++index) {
            if (*index == fooStruct[k]
              and not (fooStruct[k].getType() == cslIndex::Fixed)){
              if (not fooStruct[k].getFree()
                    or index->testContraction(
                        fooStruct[k])) {
                if (not explicitTimes
                        and option::applySelfContractions and
                        (*arg)->hasContractionProperty(
                            (*arg2).get())) {
                  (*arg2) = ContractIndex(
                          (*arg2),
                          *index
                          );
                  csl::IndexStructure &ref = (**arg).getIndexStructureView();
                  for (auto& iref : ref)
                      if (iref == fooStruct[k])
                          iref.setFree(false);
                  bool commut = !GetCommutable(**arg2);
                  *arg = ContractIndex(*arg,
                                       fooStruct[k]);
                  if (*arg == CSL_0 or (*arg2) == CSL_0)
                      *arg = CSL_0;
                  else
                      *arg = (*arg)->contraction((*arg2).get());
                  (*arg2) = CSL_1;
                  if (commut)
                      std::swap(*arg, *arg2);
                  iterIndex->clear();
                  fooStruct = (*arg)->getIndexStructure();
                  tensorContracted = true;
                  k = -1;
                }
                else if (not explicitTimes
                        and option::applySelfContractions
                        and (*arg2)->
                        hasContractionProperty(arg->get())) {
                  csl::IndexStructure &ref = (**arg2).getIndexStructureView();
                  for (auto& iref : ref)
                      if (iref == *index)
                          iref.setFree(false);
                  bool commut = !GetCommutable(**arg2);
                  *arg = ContractIndex(*arg,
                                       fooStruct[k]);
                  if (CSL_0 == *arg or CSL_0 == *arg2)
                      *arg = CSL_0;
                  else
                      *arg = ((*arg2))->contraction(arg->get());
                  (*arg2) = CSL_1;
                  if (commut)
                      std::swap(*arg, *arg2);
                  iterIndex->clear();
                  fooStruct = (*arg)-> getIndexStructure();
                  tensorContracted = true;
                  k = -1;
                }
                else if (fooStruct[k].getFree()){
                    (*arg2) = ContractIndex(
                            (*arg2),
                            *index
                            );
                  *arg = ContractIndex(*arg,
                                       fooStruct[k]);
                }
              }
              breakValue = true;
              break;
            }
          }
          if (breakValue)
              break;
        }
      }
      // We add fooStruct to the vector of structures
    }
      indexArgument[iArg++] = std::move(fooStruct);
  }
  for (size_t i = 0; i != argument.size(); ++i)
      if (argument[i] == CSL_1) {
          argument.erase(argument.begin() + i);
          --i;
      }
  if (not explicitTimes and tensorContracted) {
      // We refresh arguments
      mergeProducts();
      selfCheckIndexStructure();
      return;
  }
  if (not explicitTimes) {
      orderTerms();
      mergeTerms();
  }
}

optional<Expr> IProd::getTransposed(
        const Space* space,
        bool         applyProp) const
{
    return getTransposed(vector<const Space*>(1, space), applyProp);
}

optional<Expr> IProd::getTransposed(
        const vector<const Space*>& space,
        bool                        applyProp) const
{
    vector<optional<Expr>> newArg(argument.size());
    bool symmetric = true;
    for (size_t i = 0; i != argument.size(); ++i) {
        newArg[i] = argument[i]->getTransposed(space, applyProp);
        if (symmetric or newArg[i])
            symmetric = false;
    }
    if (not symmetric) {
        Expr res = copy();
        for (size_t i = 0; i != argument.size(); ++i) {
            res->setArgument(chooseOptional(newArg[i], argument[i]),
                             argument.size()-1-i);
        }
        return res->refresh();
    }

    return nullopt;
}

void IProd::applyContractionChains()
{
    bool hasProperty = false;
    for (const auto& arg : argument)
        if (option::applyChainContractions and
                arg->hasChainContractionProperty()) {
            hasProperty = true;
            break;
        }
    if (not hasProperty) {
        return;
    }
    ContractionChain chain(argument);
    optional<ContractionChain> res = chain.splitAndEvaluate();
    if (res) {
        Expr exprRes = res.value().getResult();
        if (exprRes->getType() == csl::Type::Prod) {
            argument = exprRes->getVectorArgument();
        }
        else {
            argument = csl::vector_expr(1, exprRes);
        }
    }
}

bool IProd::hasSeparableIndicialDenominator() const
{
    bool denominatorPresent = false;
    for (size_t i = 0; i != size(); ++i) {
        Expr const &arg = argument[i];
        if (arg->getType() == csl::Type::Pow
                and arg->getArgument(1) == -CSL_1
                and arg->getArgument()->isIndexed()) {
            for (size_t j = 0; j < size(); ++j) {
                Expr const &arg2 = argument[j];
                if (arg2->getType() == csl::Type::Pow
                        and arg2->getArgument(1) == -CSL_1)
                    continue;
                if (option::checkCommutations
                        and Commutation(arg, arg2) != CSL_0)
                    return false;
            }
            denominatorPresent = true;
        }

    }
    return denominatorPresent;
}

bool IProd::compareDenominatorSeparately(Expr_info other) const
{
    csl::vector_expr num;
    csl::vector_expr denom;
    num.reserve(argument.size());
    denom.reserve(argument.size());
    for (const auto& arg : argument)
        if (arg->getType() == csl::Type::Pow
                and arg->getArgument(1) == -CSL_1)
            denom.push_back(arg->getArgument(0));
        else
            num.push_back(arg);
    const auto numerator   = IProd(num,   true);
    const auto denominator = IProd(denom, true);
    
    num  .clear();
    denom.clear();
    for (const auto& arg : *other)
        if (arg->getType() == csl::Type::Pow
                and arg->getArgument(1) == -CSL_1)
            denom.push_back(arg->getArgument(0));
        else
            num.push_back(arg);
    const auto otherNumerator   = IProd(num,   true);
    const auto otherDenominator = IProd(denom, true);

    return (numerator == &otherNumerator
            and denominator == &otherDenominator);
}

optional<Expr> IProd::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndex,
        bool         refresh) const
{
    bool replaced = false;
    csl::vector_expr newArg(argument.size());
    size_t i = 0;
    for (auto const& arg : argument) {
        auto opt = arg->replaceIndex(indexToReplace, newIndex, refresh);
        newArg[i++] = opt.value_or(arg);
        if (opt) 
            replaced = true;
    }
    if (replaced)
        return prod_s(newArg, not refresh);
    return nullopt;
}

optional<Expr> IProd::replaceIndices(
        std::vector<csl::Index> const &oldIndices,
        std::vector<csl::Index> const &newIndices,
        bool         refresh,
        bool         flipped) const
{
    bool replaced = false;
    csl::vector_expr newArg(argument.size());
    size_t i = 0;
    for (auto const& arg : argument) {
        auto opt = arg->replaceIndices(oldIndices, newIndices, refresh, flipped);
        newArg[i++] = opt.value_or(arg);
        if (opt) 
            replaced = true;
    }
    if (replaced)
        return prod_s(newArg, not refresh);
    return nullopt;
}

void fillPosition(vector<vector<size_t>>& positions,
                  size_t Nspaces)
{
    if (positions.empty())
        return;
    vector<size_t> count(positions[0].size(), 0);
    for (size_t i = 0; i != positions.size(); ++i) {
        for (size_t c = 0; c != count.size(); ++c)
            count[c] = (i / (size_t)pow(Nspaces, c)) % Nspaces;
        positions[i] = count;
    }
}

Expr getBrokenExpr(const Expr&           init,
                   const csl::vector_expr&   brokenExpr,
                   const vector<Index>&  brokenIndices,
                   const vector<Index>&  replacement,
                   const vector<size_t>& positions,
                   const size_t          Nspaces)
{
    if (brokenExpr.empty())
        return init;

    IndexStructure structure = init->getIndexStructure();
    vector<vector<size_t>> posInStruct(brokenIndices.size());
    vector<size_t> sortedPosInStruct;
    for (size_t i = 0; i != brokenIndices.size(); ++i) {
        bool found = false;
        for (size_t j = 0; j != structure.size(); ++j)
            if (brokenIndices[i] == structure[j]) {
                if (found) {
                    //posInStruct[i].push_back(j);
                    sortedPosInStruct.erase(sortedPosInStruct.end()-1);
                    posInStruct[i].clear();
                    break;
                }
                else {
                    //posInStruct[i].reserve(2);
                    sortedPosInStruct.push_back(j);
                    posInStruct[i].push_back(j);
                    found = true;
                }
            }
    }
    std::sort(sortedPosInStruct.begin(), sortedPosInStruct.end());

    size_t indexBrokenExpr = 0;
    for (size_t i = 0; i != posInStruct.size(); ++i) {
        for (size_t j = 0; j != posInStruct[i].size(); ++j) {
            auto posIndex = std::find(
                    sortedPosInStruct.begin(),
                    sortedPosInStruct.end(),
                    posInStruct[i][j]);
            size_t exponent = distance(sortedPosInStruct.begin(), posIndex);
            indexBrokenExpr += pow(Nspaces, exponent) * positions[i];
        }
    }
    Expr res = brokenExpr[indexBrokenExpr];

    vector<size_t> nullIndices;
    nullIndices.reserve(structure.size());
    Index nullIndex;
    for (size_t i = 0; i != brokenIndices.size(); ++i)
        if (replacement[i] == nullIndex) {
            for (size_t pos : posInStruct[i])
                nullIndices.push_back(pos);
            posInStruct[i].clear();
        }
    std::sort(nullIndices.begin(), nullIndices.end());
    for (auto& el : posInStruct)
        for (auto& pos : el) {
            size_t delta = 0;
            for (const auto& null : nullIndices)
                if (pos > null)
                    ++delta;
                else
                    break;
            pos -= delta;
        }
    IndexStructure resStruct = res->getIndexStructure();
    IndexStructure oldStruct = resStruct;
    for (size_t i = 0; i != replacement.size(); ++i)
        if (replacement[i] != nullIndex) {
            for (const auto& pos : posInStruct[i])
                if (oldStruct[pos] != replacement[i]) {
                    res = Replaced(res, oldStruct[pos], replacement[i]);
                }
        }

    return res;
}

std::vector<Expr> sumDummyIndices(
        std::vector<Expr> const &init,
        std::vector<std::vector<size_t>> const &posIndices,
        std::vector<size_t> const &dummyIndices
        )
{
    auto areEquivalent = [&](
            std::vector<size_t> const &setA,
            std::vector<size_t> const &setB
            )
    {
        for (size_t posIndex = 0; posIndex != setA.size(); ++posIndex) {
            auto pos = std::find(
                    dummyIndices.begin(),
                    dummyIndices.end(),
                    posIndex);
            if (pos == dummyIndices.end()) {
                if (setA[posIndex] != setB[posIndex])
                    return false;
            }
        }
        return true;
    };

    std::vector<size_t> indicesLeft(init.size());
    for (size_t i = 0; i != init.size(); ++i)
        indicesLeft[i] = i;

    std::vector<Expr> res;
    while (!indicesLeft.empty()) {
        std::vector<Expr> terms;
        terms.push_back(init[indicesLeft[0]]);
        for (size_t i = 1; i != indicesLeft.size(); ++i) {
            if (areEquivalent(
                        posIndices[indicesLeft[0]],
                        posIndices[indicesLeft[i]])) {
                terms.push_back(init[indicesLeft[i]]);
                indicesLeft.erase(indicesLeft.begin() + i);
                --i;
            }
        }
        indicesLeft.erase(indicesLeft.begin());
        res.push_back(sum_s(terms));
    }
    return res;
}

csl::vector_expr IProd::breakSpace(
        const Space*                brokenSpace,
        const vector<const Space*>& newSpaces,
        const vector<string>&       indexNames
        ) const
{
    IndexStructure structure = getIndexStructure();
    vector<Index>  brokenIndices;
    vector<size_t> dummyIndices;
    brokenIndices.reserve(structure.size());
    csl::IndexStructure internalDummyIndices;
    for (const auto &arg : *this) {
        csl::IndexStructure argStructure = arg->getIndexStructure();
        for (size_t i = 0; i != argStructure.size(); ++i)
            for (size_t j = i+1; j < argStructure.size(); ++j)
                if (argStructure[i] == argStructure[j]) {
                    internalDummyIndices.push_back(argStructure[i]);
                }
    }
    for (size_t i = 0; i != structure.size(); ++i)
        if (structure[i].getSpace() == brokenSpace) {
            auto pos = std::find(
                    internalDummyIndices.begin(),
                    internalDummyIndices.end(),
                    structure[i]);
            if (pos != internalDummyIndices.end())
                continue;
            if (auto pos = find(brokenIndices.begin(),
                                brokenIndices.end(),
                                structure[i]);
                    pos == brokenIndices.end())
                brokenIndices.push_back(structure[i]);
            else 
                dummyIndices.push_back(
                        std::distance(brokenIndices.begin(), pos)
                        );
        }

    vector<csl::vector_expr> brokenExpr(argument.size());
    for (size_t i = 0; i != argument.size(); ++i) {
        brokenExpr[i] = argument[i]->breakSpace(
                brokenSpace,
                newSpaces,
                indexNames
                );
        if (brokenExpr[i].empty())
            brokenExpr[i].push_back(argument[i]);
    }

    vector<vector<Index>> newIndices(brokenIndices.size(),
                                     vector<Index>(newSpaces.size()));
    for (auto& newSet : newIndices) {
        for (size_t i = 0; i != newSpaces.size(); ++i)
            if (newSpaces[i])
                newSet[i] = newSpaces[i]->generateIndex(
                        indexNames[i]);
            else
                newSet[i] = Index();
    }
    vector<vector<size_t>> posIndices(
            pow(newSpaces.size(), brokenIndices.size()),
            vector<size_t>(brokenIndices.size()));
    fillPosition(posIndices, newSpaces.size());

    vector<csl::vector_expr> terms(posIndices.size());
    for (size_t i = 0; i != posIndices.size(); ++i) {
        vector<Index> replacement(brokenIndices.size());
        for (size_t j = 0; j != replacement.size(); ++j) {
            replacement[j] = newIndices[j][posIndices[i][j]];
        }

        for (size_t k = 0; k != argument.size(); ++k) {
            terms[i].push_back(getBrokenExpr(
                        argument[k],
                        brokenExpr[k],
                        brokenIndices,
                        replacement,
                        posIndices[i],
                        newSpaces.size()));
        }
    }
    csl::vector_expr sumTerms(posIndices.size());
    for (size_t i = 0; i != posIndices.size(); ++i) {
        sumTerms[i] = prod_s(terms[i]);
    }

    auto res = sumDummyIndices(sumTerms, posIndices, dummyIndices);

    return res;
}

void IProd::leftInsert(const Expr& expr)
{
    // If not numerical, we search for a similar term
    int max = getNArgs();
    Expr term, exponent;
    getExponentStructure(expr, term, exponent);
    for (size_t i = 0; i < size(); i++) {
        // We do not merge indicial expressions
        // Ai.Ai does not give (Ai)^2
        if (argument[i]->isIndexed())
            continue;
        Expr term2, exponent2;
        getExponentStructure(argument[i], term2, exponent2);
        if (option::checkCommutations
                and *Commutation(expr, argument[i]) == CSL_0) {
            // If we found the right term, it's done
            if (*term == term2) {
                argument[i] = pow_s(term, exponent->addition_own(exponent2));
                if (*argument[i] == CSL_1) {
                    argument.erase(argument.begin()+i);
                }
                return;
            }
        }
        else if (*term == term2) {
            argument[i] = pow_s(term, exponent->addition_own(exponent2));
            if (*argument[i] == CSL_1) {
                argument.erase(argument.begin()+i);
            }
            return;
        }
        else {
            // max is the position not reachable because of commutation not trivial
            max = i;
            break;
        }
    }

    // No term corresponds, we order correctly the new term in the sum
    for (int i = 0; i < max; i++)
        if (expr < argument[i] or
                (option::checkCommutations and
                    *Commutation(expr, argument[i]) != CSL_0)) {
            argument.insert(argument.begin()+i, expr);
            return;
        }

    // no term is more complicated than expr, we put it at the end
    argument.insert(argument.begin()+max, expr);
}
void IProd::rightInsert(const Expr& expr)
{
    // If not numerical, we search for a similar term
    int max = -1;
    Expr term, exponent;
    getExponentStructure(expr, term, exponent);
    for (int i = static_cast<int>(size())-1; i >= 0; --i) {
        // We do not merge indicial expressions
        // Ai.Ai does not give (Ai)^2
        if (argument[i]->isIndexed())
            continue;
        Expr term2, exponent2;
        getExponentStructure(argument[i], term2, exponent2);
        if (option::checkCommutations and
                *Commutation(expr, argument[i]) == CSL_0) {
            // If we found the right term, it's done
            if (*term == term2) {
                argument[i] = pow_s(term, exponent->addition_own(exponent2));
                if (*argument[i] == CSL_1) {
                    argument.erase(argument.begin()+i);
                }
                return;
            }
        }
        else if (*term == term2) {
            argument[i] = pow_s(term, exponent->addition_own(exponent2));
            if (*argument[i] == CSL_1) {
                argument.erase(argument.begin()+i);
            }
            return;
        }
        else {
            // max is the position not reachable because of commutation not trivial
            max = i;
            break;
        }
    }

    // No term corresponds, we order correctly the new term in the sum
    for (int i = getNArgs()-1; i>max; --i)  {
        if (expr > argument[i] or (option::checkCommutations
                    and *Commutation(expr, argument[i]) != CSL_0)) {
            argument.insert(argument.begin()+i+1, expr);
            return;
        }
    }

    // no term is more complicated than expr, we put it at the end
    argument.insert(argument.begin()+max+1, expr);
}

bool IProd::mergeTerms()
{
    bool simplified = mergeNumericals();
    Expr factor, term;
    bool remergeNumericals = false;
    for (size_t i = 0; i+1 < argument.size(); i++) {
        // We do not merge indicial expressions
        // Ai.Ai does not give (Ai)^2
        if (argument[i]->isIndexed())
            continue;

        factor = CSL_1;
        if (argument[i]->getType() == csl::Type::Pow) { //Pow
            term = argument[i]->getArgument(1);
            if (term->getPrimaryType() == csl::PrimaryType::Numerical) {
                factor = term;
                term = argument[i]->getArgument();
            }
            else
                term = argument[i];
        }
        else term = argument[i];
        Expr factor2 = CSL_1;
        Expr term2;
        if (argument[i+1]->getType() == csl::Type::Pow) {  //Pow
            term2 = argument[i+1]->getArgument(1);
            if (term2->getPrimaryType() == csl::PrimaryType::Numerical) {
                factor2 = term2;
                term2 = argument[i+1]->getArgument();
            }
            else
                term2 = argument[i+1];
        }
        else
            term2 = argument[i+1];
        if (*term==term2) {
            factor = factor->addition_own(factor2);
            if (term->isInteger() 
                    and factor->getType() == csl::Type::IntFraction)
                continue;
            argument.erase(argument.begin()+i+1);
            argument[i] = pow_s(term, factor);
            if (*argument[i] == CSL_1 and argument.size() > 1){
                argument.erase(argument.begin()+i);
            }
            else if (argument[i]->getPrimaryType()
                        == csl::PrimaryType::Numerical)
                remergeNumericals = true;
            --i;
            simplified = true;
        }
    }
    if (remergeNumericals)
        mergeNumericals();

    return simplified;
}

bool IProd::compareWithDummy(Expr_info        expr,
                             map<Index, Index>& constraints,
                             bool keepAllCosntraints) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    const size_t sz = size();
    const size_t expr_sz = expr->size();
    if (sz == 1)
        return *argument[0]==expr;
    if (expr->getType() != csl::Type::Prod)
        return false;
    if (sz != expr_sz)
        if (not Comparator::getDummyComparisonActive()
                or not (sz > expr_sz))
        return false;
    if (hasSeparableIndicialDenominator())
        return compareDenominatorSeparately(expr);

    vector<size_t> indicesLeft(expr_sz);
    for (size_t i=0; i<expr_sz;i++)
        indicesLeft[i] = i;

    bool dummySearch = false;
    if (Comparator::getDummyComparisonActive()) {
        dummySearch = true;
        Comparator::setDummyComparisonActive(false);
    }
    for (size_t i = 0; i < sz; i++) {
        bool matched = false;
        Expr const &arg = argument[i];
        for (size_t j=0; j<indicesLeft.size(); j++) {
            Expr const &foo = expr->getArgument(indicesLeft[j]);
            const bool indicialArg = IsIndicialTensor(arg);
            if (not indicialArg and *arg == foo.get()) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
            else if (indicialArg and IsIndicialTensor(foo))  {
                if (arg->compareWithDummy(foo.get(), constraints,
                                                  keepAllCosntraints)) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
            }
            else if (arg->getType() == csl::Type::Sum
                    and arg->isIndexed()) {
                if (arg->compareWithDummy(foo.get(), 
                                                  constraints,
                                                  keepAllCosntraints)) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
            }
            else if (indicialArg
                    and foo->getName() == Comparator::dummyName()
                    and *arg == foo.get()) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
            if (option::checkCommutations
                    and *Commutation(arg, foo) != CSL_0
                    and not matched)
                break;
        }
        if (not matched) {
            if (not dummySearch or Comparator::getDummyComparisonActive()) {
                return false;
            }
            Comparator::setDummyComparisonActive(true);
            --i;
        }
        else if (dummySearch) {
            if (indicesLeft.empty()) {
                Comparator::setDummyComparisonActive(true);
                return true;
            }
        }
    }
    if (dummySearch)
        Comparator::setDummyComparisonActive(true);

    return true;
}

bool IProd::operator==(Expr_info expr) const
{
    map<Index, Index> constraints;
    if (not compareWithDummy(expr, constraints))
        return false;
    if (not constraints.empty())  {
        for (const auto& [f, s] : constraints)
            if (f != s)
                return Comparator::getFreeIndexComparisonActive();
        return true;
        // Old state:
        // return Comparator::getFreeIndexComparisonActive();

        //Expr foo = DeepRefreshed(this);
        //IndexStructure structure = foo->getFreeIndexStructure();
        //for (auto it=constraints.begin(); it!=constraints.end(); ++it)
        //    if (it->first != it->second
        //            or not (structure.find(it->first)  == structure.end())
        //            or not (structure.find(it->second) == structure.end()))
        //        return false;
    }
    return true;
}

bool IProd::partialComparison(Expr_info expr) const
{
    if (size() == 1)
        return *argument[0]==expr;
    if (expr->getType() != csl::Type::Prod)
        return false;

    int t_nArgs = expr->getNArgs();
    vector<int> indicesLeft(t_nArgs);
    for (int i=0; i<t_nArgs;i++)
        indicesLeft[i] = i;

    Expr foo;
    map<Index,Index> constraints;
    bool checkIndexExpressions = false;
    if (isIndexed())
        checkIndexExpressions = true;
    for (size_t i = 0; i < size(); i++) {
        bool matched = false;
        for (size_t j=0; j<indicesLeft.size(); j++) {
            foo = expr->getArgument(indicesLeft[j]);
            if (!GetCommutable(argument[i]) and
                !GetCommutable(foo) and
                *argument[i]!=foo)
                if (argument[i]->getType() != csl::Type::TensorElement
                        or foo->getType() != csl::Type::TensorElement
                        or foo->getName() != argument[i]->getName())
                    break;
            if ((not checkIndexExpressions
                        or not (IsIndicialTensor(argument[i])))
                    and *argument[i] == foo) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
            else if (checkIndexExpressions
                and IsIndicialTensor(argument[i])
                and IsIndicialTensor(foo))  {

                if (argument[i]->compareWithDummy(foo.get(), constraints)) {
                    indicesLeft.erase(indicesLeft.begin()+j);
                    matched = true;
                    break;
                }
            }

        }
        if (not matched)
            return false;
        else if (indicesLeft.size() == 0)
            break;
    }

    // If there is some constraints on dummy indices left (we found only
    // one dummy of the pair), we check that there are the same.
    // Ex: A_i.B_%j != A_i.B_%k but A_i.B_%j == A_i.B_%j
    if (checkIndexExpressions and not constraints.empty())
        for (auto it=constraints.begin(); it!=constraints.end(); ++it)
            if (it->first != it->second)
                return false;

    return true;
}


} // End of namespace csl
