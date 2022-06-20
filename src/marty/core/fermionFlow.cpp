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

#include "fermionFlow.h"
#include "diracology.h"
#include "graph.h"
#include "mrtError.h"
#include "quantumField.h"

namespace mty {

using FieldList = ConjugationInfo::FieldList;

void removeBosons(FieldList &init)
{
    auto last = std::remove_if(
        init.begin(), init.end(), [&](QuantumField const *field) {
            return field->isBosonic();
        });
    init.erase(last, init.end());
}

std::pair<FieldList, FieldList> separatePartners(FieldList const &init)
{
    size_t    N = init.size() / 2;
    FieldList fields(N);
    FieldList partners(N);
    for (size_t i = 0; i != N; ++i) {
        fields[i]   = init[2 * i];
        partners[i] = init[2 * i + 1];
        if (!fields[i]->isExternal() and partners[i]->isExternal())
            std::swap(fields[i], partners[i]);
    }

    return {fields, partners};
}

bool compareFields(mty::QuantumField const &A, mty::QuantumField const &B)
{
    if (A.isSelfConjugate() != B.isSelfConjugate()) {
        if (A.isSelfConjugate())
            return B.isComplexConjugate();
        if (B.isSelfConjugate())
            return !A.isComplexConjugate();
    }
    if (A.isComplexConjugate() != B.isComplexConjugate())
        return !A.isComplexConjugate();
    if (A.getName() < B.getName())
        return true;
    else if (A.getName() == B.getName())
        return A.getPoint()->getName() < B.getPoint()->getName();
    return false;
}

FieldList::iterator getExternalBegin(FieldList &fields)
{
    auto iter         = fields.begin();
    auto iter_simpler = fields.end();
    while (iter != fields.end()) {
        if ((**iter).isExternal()) {
            if (iter_simpler == fields.end()
                or compareFields(**iter, **iter_simpler))
                iter_simpler = iter;
        }
        ++iter;
    }
    return iter_simpler;
}

FieldList::iterator getInternalBegin(FieldList &fields)
{
    auto iter = fields.begin();
    while (iter != fields.end()) {
        if (!(**iter).isComplexConjugate())
            return iter;
        ++iter;
    }
    return fields.begin();
}

FieldList::iterator getBegin(FieldList &fields)
{
    auto begin = getExternalBegin(fields);
    return (begin == fields.end()) ? getInternalBegin(fields) : begin;
}

FieldList::iterator
getPartner(FieldList::iterator A, FieldList &fieldsA, FieldList &partners)
{
    return partners.begin() + std::distance(fieldsA.begin(), A);
}

FieldList::iterator getNext(QuantumField const *field,
                            FieldList &         fields,
                            FieldList &         partners,
                            bool                recurs = false)
{
    auto        iter       = fields.begin();
    bool        conjugated = (*field).isComplexConjugate();
    csl::Tensor X          = (*field).getPoint();
    // First search a match with proper conjugation
    while (iter != fields.end()) {
        if ((**iter).getPoint() == X
            and (**iter).isComplexConjugate() != conjugated)
            return iter;
        ++iter;
    }
    // Then search without proper conjugation
    iter = fields.begin();
    while (iter != fields.end()) {
        if ((**iter).getPoint() == X)
            return iter;
        ++iter;
    }

    if (recurs)
        return iter;

    iter       = getNext(field, partners, fields, true);
    auto iter2 = getPartner(iter, partners, fields);
    std::swap(*iter, *iter2);
    return iter2;
}

static constexpr bool displayDebug = false;

bool isSameOrder(QuantumField const *  first,
                 QuantumField const *  second,
                 FieldList const &     ruleOrder,
                 [[maybe_unused]] bool ruleMode)
{
    for (const auto &field : ruleOrder) {
        if (field == first) {
            return true;
        }
        else if (field == second)
            return false;
    }
    CallHEPError(mty::error::RuntimeError,
                 "Fields " + toString(*first) + " and " + toString(*second)
                     + " not found !") return false;
}

csl::Index diracIndexOf(QuantumField const *field)
{
    csl::IndexStructure const &structure = field->getIndexStructureView();
    for (const auto &index : structure)
        if (index.getSpace() == &mty::dirac4)
            return index;
    CallHEPError(mty::error::IndexError,
                 "Field " + toString(*field)
                     + " has no Dirac index !") return csl::Index();
}

void testBeginExternalLeg(QuantumField const *          ext,
                          std::vector<ConjugationInfo> &info)
{
    if (ext->isExternal() and ext->isComplexConjugate()) {
        // std::cout << ext->copy() << " conjugated !" << std::endl;
        info.emplace_back(diracIndexOf(ext), true);
    }
}

void testEndExternalLeg(QuantumField const *          ext,
                        std::vector<ConjugationInfo> &info)
{
    if (ext->isExternal() and !ext->isComplexConjugate()) {
        // std::cout << ext->copy() << " conjugated !" << std::endl;
        info.emplace_back(diracIndexOf(ext), false);
    }
}

void testPropagator(QuantumField const *          first,
                    QuantumField const *          second,
                    csl::Index const &            preIndex,
                    csl::Index const &            postIndex,
                    FieldList const &             initialOrder,
                    std::vector<ConjugationInfo> &info)
{
    if (!second->isComplexConjugate() and first->isComplexConjugate()) {
        info.emplace_back(
            diracIndexOf(first), diracIndexOf(second), preIndex, postIndex);
    }
    else if (first->isComplexConjugate() == second->isComplexConjugate()) {
        for (const auto &field : initialOrder)
            if (field == first)
                return;
            else if (field == second) {
                info.emplace_back(diracIndexOf(first),
                                  diracIndexOf(second),
                                  preIndex,
                                  postIndex);
            }
    }
}

void testVertexFields(QuantumField const *          first,
                      QuantumField const *          second,
                      csl::Index const &            preIndex,
                      csl::Index const &            postIndex,
                      FieldList const &             ruleOrder,
                      std::vector<ConjugationInfo> &info,
                      bool                          ruleMode)
{
    if (!isSameOrder(first, second, ruleOrder, ruleMode)) {
        // std::cout << "VERTEX" << std::endl;
        // std::cout << first->copy() << std::endl;
        // std::cout << second->copy() << std::endl;

        info.emplace_back(
            diracIndexOf(first), diracIndexOf(second), preIndex, postIndex);
    }
}

void testFields(QuantumField const *first,
                QuantumField const *second,
                csl::Index const &  preIndex,
                csl::Index const &  postIndex,
                FieldList const &, // initialOrder,
                FieldList const &             ruleOrder,
                std::vector<ConjugationInfo> &info,
                bool                          ruleMode)
{
    if (first->getPoint() == second->getPoint())
        return testVertexFields(
            first, second, preIndex, postIndex, ruleOrder, info, ruleMode);
    // if (!ruleMode)
    //     testPropagator(first, second, preIndex, postIndex, initialOrder,
    //     info);
}

#define FERMIONFLOWASSERT(cond)         \
    HEPAssert((cond),                   \
              mty::error::RuntimeError, \
              "Fermion flow not resolved in amplitude.")

std::vector<FermionLine> chainFermions(FieldList const &init)
{
    auto [fields, partners] = separatePartners(init);
    std::vector<FermionLine> res;
    res.reserve(ConjugationInfo::maxFermionLines);
    while (!fields.empty() and !partners.empty()) {
        auto field   = getBegin(fields);
        auto partner = getPartner(field, fields, partners);
        FERMIONFLOWASSERT(field != fields.end())
        FERMIONFLOWASSERT(partner != partners.end())
        FieldList line;
        line.reserve(init.size());
        line.push_back(*field);
        line.push_back(*partner);

        QuantumField const *save_partner = *partner;
        fields.erase(field);
        partners.erase(partner);
        while ((field = getNext(save_partner, fields, partners))
               != fields.end()) {
            partner = getPartner(field, fields, partners);
            line.push_back(*field);
            line.push_back(*partner);
            save_partner = *partner;
            fields.erase(field);
            partners.erase(partner);
        }
        std::vector<csl::Index> indices(line.size());
        for (size_t i = 0; i != line.size(); ++i)
            indices[i] = diracIndexOf(line[i]);

        res.push_back({line, indices});
    }

    return res;
}

void f(FieldList const &init, FieldList const &line)
{
    std::cout << "Initial Wick theorem order" << std::endl;
    for (const auto &f : init)
        std::cout << *f << std::endl;
    std::cout << "Line fermion order" << std::endl;
    for (const auto &f : line)
        std::cout << *f << std::endl;
    std::cout << "Sign = " << wick::getCommutationSign(init, line)
              << std::endl;
}

ConjugationList ConjugationInfo::resolveFermionLines(
    FieldList init, FieldList const &ruleOrder, bool ruleMode)
{
    HEPAssert(init.size() % 2 == 0,
              mty::error::ValueError,
              "Expecting even number of fields, " + toString(init.size())
                  + " given.")

        std::vector<ConjugationInfo>
            res;
    res.reserve(maxFermionLines);

    std::vector<QuantumField const *> bosons;
    for (const auto &field : init) {
        if (field->isBosonic()) {
            bosons.push_back(field);
        }
    }
    removeBosons(init);
    std::vector<FermionLine> lines = chainFermions(init);
    if constexpr (displayDebug) {
        std::cout << lines.size() << " lines" << std::endl;
    }
    int sign = 1;
    for (const auto &line : lines) {
        if constexpr (displayDebug) {
            for (const auto &f : line.fields)
                std::cout << *f << "  ";
            std::cout << std::endl;
        }
        bool loop = !line.fields[0]->isExternal();
        testBeginExternalLeg(line.fields[0], res);
        if (loop and line.fields.size() == 2) {
            csl::Index a = diracIndexOf(line.fields[0]);
            csl::Index b = diracIndexOf(line.fields[1]);
            testVertexFields(line.fields[1],
                             line.fields[0],
                             a,
                             b,
                             ruleOrder,
                             res,
                             ruleMode);
            continue;
        }
        for (size_t i = (loop) ? 0 : 1; i < line.fields.size() - 1; ++i) {
            mty::QuantumField const *A        = line.fields[i];
            mty::QuantumField const *B        = line.fields[i + 1];
            size_t                   pre      = (i - 1) % line.fields.size();
            size_t                   post     = (i + 2) % line.fields.size();
            csl::Index               preIndex = diracIndexOf(line.fields[pre]);
            if (preIndex == diracIndexOf(A))
                preIndex
                    = diracIndexOf(line.fields[(i - 2) % line.fields.size()]);
            csl::Index postIndex = diracIndexOf(line.fields[post]);
            if (postIndex == diracIndexOf(B))
                postIndex
                    = diracIndexOf(line.fields[(i + 3) % line.fields.size()]);
            testFields(
                A, B, preIndex, postIndex, init, ruleOrder, res, ruleMode);
        }
        if (loop) {
            csl::Index preIndex
                = diracIndexOf(line.fields[line.fields.size() - 2]);
            if (preIndex == diracIndexOf(line.fields.back()))
                preIndex = diracIndexOf(line.fields[(line.fields.size() - 3)
                                                    % line.fields.size()]);
            csl::Index postIndex = diracIndexOf(line.fields[1]);
            if (postIndex == diracIndexOf(line.fields[0]))
                postIndex = diracIndexOf(line.fields[2 % line.fields.size()]);
            testFields(line.fields.back(),
                       line.fields[0],
                       preIndex,
                       postIndex,
                       init,
                       ruleOrder,
                       res,
                       ruleMode);
        }
        testEndExternalLeg(line.fields.back(), res);
        if constexpr (displayDebug) {
            f(ruleOrder, line.fields);
        }
    }
    if constexpr (displayDebug) {
        std::cout << res.size() << " conjugations" << std::endl;
        for (const auto &c : res)
            std::cout << c << std::endl;
        std::cout << "sign = " << sign << std::endl;
    }

    return ConjugationList(res, sign, lines, bosons);
}

ConjugationList ConjugationInfo::resolveFermionLines(
    FieldList const &                init,
    std::vector<QuantumField> const &ruleOrder,
    bool                             ruleMode)
{
    FieldList ruleOrder_new(ruleOrder.size());
    for (size_t i = 0; i != ruleOrder.size(); ++i)
        ruleOrder_new[i] = &ruleOrder[i];

    return resolveFermionLines(init, ruleOrder_new, ruleMode);
}

void ConjugationInfo::simplify(ConjugationList &info, bool ruleMode)
{
    for (size_t i = 0; i != info.size(); ++i) {
        if (info[i].getType() != Internal) {
            if (ruleMode) {
                info.erase(info.begin() + i);
                --i;
            }
            continue;
        }
        while (i + 1 < info.size() and info[i + 1].getType() == Internal) {
            if (info[i].getB() == info[i + 1].getA()) {
                info[i].setInfo(info[i].getA(),
                                info[i + 1].getB(),
                                info[i].getPreA(),
                                info[i + 1].getPostB());
                info.erase(info.begin() + i + 1);
            }
            else
                break;
        }
    }
    for (size_t i = 0; i != info.size(); ++i)
        if (info[i].getType() == Internal
            and info[i].getA() == info[i].getB()) {
            info.erase(info.begin() + i);
            --i;
        }
    for (size_t i = 0; i != info.size(); ++i) {
        if (info[i].getType() != External)
            continue;
        for (size_t j = 0; j < info.size(); ++j) {
            if (i == j or info[j].getType() != Internal)
                continue;
            if (info[i].getConjugated() and info[i].getA() == info[j].getA()) {
                info[j].setInfo(csl::Index(),
                                info[j].getB(),
                                info[j].getPreA(),
                                info[j].getPostB());
                info.erase(info.begin() + i);
                --i;
                break;
            }
            else if (!info[i].getConjugated()
                     and info[i].getA() == info[j].getB()) {
                info[j].setInfo(info[j].getA(),
                                csl::Index(),
                                info[j].getPreA(),
                                info[j].getPostB());
                info.erase(info.begin() + i);
                --i;
                break;
            }
        }
    }
    for (size_t i = 0; i != info.size(); ++i) {
        if (info[i].getA() == csl::Index()) {
            if (info[i].getB() == csl::Index()
                or info[i].getType() == External) {
                info.erase(info.begin() + i);
                --i;
            }
        }
    }
}

void ConjugationInfo::applyConjugation(ConjugationList infoList,
                                       csl::Expr &     expr,
                                       bool            ruleMode)
{
    if (expr == CSL_0)
        return;
    simplify(infoList, ruleMode);
    {
        csl::ScopedProperty prop(&csl::option::applySelfContractions, false);
        if constexpr (displayDebug) {
            std::cout << "Applying on " << expr << std::endl;
        }
        for (const auto &i : infoList) {
            if constexpr (displayDebug) {
                std::cout << i << std::endl;
            }
            if (!ruleMode or i.getType() == Internal)
                i.apply(expr, ruleMode, infoList.lines);
        }
        if constexpr (displayDebug) {
            std::cout << "RES = " << expr << std::endl;
        }
        if (infoList.size() > 0)
            csl::DeepRefresh(expr);
    }
}

ConjugationInfo::ConjugationInfo() : type(Undefined)
{
}

ConjugationInfo::ConjugationInfo(csl::Index const &t_A, bool t_conjugated)
{
    setInfo(t_A, t_conjugated);
}

ConjugationInfo::ConjugationInfo(csl::Index const &t_A,
                                 csl::Index const &t_B,
                                 csl::Index const &t_preA,
                                 csl::Index const &t_postB)
{
    setInfo(t_A, t_B, t_preA, t_postB);
}

void ConjugationInfo::setInfo(csl::Index const &t_A, bool t_conjugated)
{
    type       = External;
    A          = t_A;
    conjugated = t_conjugated;
}

void ConjugationInfo::setInfo(csl::Index const &t_A,
                              csl::Index const &t_B,
                              csl::Index const &t_preA,
                              csl::Index const &t_postB)
{
    type  = Internal;
    A     = t_A;
    B     = t_B;
    preA  = t_preA;
    postB = t_postB;
}

void ConjugationInfo::apply(csl::Expr &                     expr,
                            bool                            ruleMode,
                            std::vector<FermionLine> const &lines) const
{
    // std::cout << "Applying " << *this << " on " << expr << std::endl;
    csl::Tensor C = mty::dirac4.C_matrix;
    csl::Index  I, J;
    switch (type) {
    case External:
        insertExternal(expr);
        break;

    case Internal:
        if (ruleMode)
            insertInChainForRule(expr, lines);
        else
            insertInChain(expr);
        break;

    case Undefined:
        break;
    }
}

bool getNextIndexInChain(csl::Index &     first,
                         csl::Index &     prev,
                         csl::Expr const &expr)
{
    bool found = false;
    csl::VisitEachNodeCut(expr, [&](csl::Expr const &sub) {
        csl::IndexStructure structure = sub->getIndexStructure(&mty::dirac4);
        if (found)
            return true;
        if (structure.size() != 2
            or sub->getName().find("EXT") != std::string::npos)
            return false;
        if (structure[0] == first and structure[1] != prev) {
            prev  = first;
            first = structure[1];
            found = true;
            return true;
        }
        if (structure[1] == first and structure[0] != prev) {
            prev  = first;
            first = structure[0];
            found = true;
            return true;
        }
        return false;
    });

    return found;
}

bool findNextIndexBefore(csl::Index        first,
                         csl::Index const &target,
                         csl::Index const &end,
                         csl::Index        prev,
                         csl::Expr const & expr)
{
    if (first == target)
        return true;
    if (first == end)
        return false;
    // std::cout << "Searching " << target << " from " << first << "(" << prev
    // << ")" << std::endl;
    do {
        if (!getNextIndexInChain(first, prev, expr))
            break;
    } while (first != end and first != target);
    if (first == target) {
        // std::cout << "Found !" << std::endl;
    }

    return first == target;
}

void ConjugationInfo::insertInChainForRule(
    csl::Expr &expr, std::vector<FermionLine> const &lines) const
{
    csl::Index IA = A.rename();
    csl::Index IB = B.rename();
    if (A != csl::Index())
        csl::Replace(expr, A, IA);
    if (B != csl::Index())
        csl::Replace(expr, B, IB);
    csl::Tensor C = mty::dirac4.C_matrix;
    for (const auto &line : lines) {
        bool left = false;
        for (const auto &index : line.indices) {
            if (index == A and A != csl::Index()) {
                if (left) {
                    expr = expr * C({A, IA});
                }
                else {
                    expr = expr * C({IA, A});
                }
            }
            else if (index == B and B != csl::Index()) {
                if (left) {
                    expr = -expr * C({B, IB});
                }
                else {
                    expr = -expr * C({IB, B});
                }
            }
            left = !left;
        }
    }
}

void ConjugationInfo::insertInChain(csl::Expr &expr) const
{
    csl::Tensor C = mty::dirac4.C_matrix;
    bool        foundA
        = (A == csl::Index()) or csl::FirstOfNode(expr, [&](csl::Expr &sub) {
              csl::IndexStructure structure
                  = sub->getIndexStructure(&mty::dirac4);
              if (structure.size() == 2 and structure[1] == A
                  and findNextIndexBefore(
                      structure[0], preA, postB, structure[1], expr)) {
                  csl::Index I = A.rename();
                  csl::Replace(sub, A, I);
                  sub = sub * C({I, A});
                  return true;
              }
              if (structure.size() == 2 and structure[0] == A
                  and findNextIndexBefore(
                      structure[1], preA, postB, structure[0], expr)) {
                  csl::Index I = A.rename();
                  csl::Replace(sub, A, I);
                  sub = sub * C({I, A});
                  return true;
              }
              return false;
          });

    bool foundB
        = (B == csl::Index()) or csl::FirstOfNode(expr, [&](csl::Expr &sub) {
              csl::IndexStructure structure
                  = sub->getIndexStructure(&mty::dirac4);
              if (structure.size() == 2 and structure[0] == B
                  and findNextIndexBefore(
                      structure[1], postB, preA, structure[0], expr)) {
                  csl::Index I = B.rename();
                  csl::Replace(sub, B, I);
                  sub = -sub * C({I, B});
                  return true;
              }
              if (structure.size() == 2 and structure[1] == B
                  and findNextIndexBefore(
                      structure[0], postB, preA, structure[1], expr)) {
                  csl::Index I = B.rename();
                  csl::Replace(sub, B, I);
                  sub = -sub * C({I, B});
                  return true;
              }
              return false;
          });
    if (!foundA or !foundB) {
        csl::IndexStructure structure = expr->getIndexStructure();
        if (not foundA) {
            size_t match = 0;
            for (const auto &index : structure)
                if (index == A)
                    ++match;
            if (match == 1) {
                csl::Index I = A.rename();
                csl::Replace(expr, A, I);
                expr   = expr * C({A, I});
                foundA = true;
            }
        }
        if (not foundB) {
            size_t match = 0;
            for (const auto &index : structure)
                if (index == A)
                    ++match;
            if (match == 1) {
                csl::Index I = B.rename();
                csl::Replace(expr, B, I);
                expr   = expr * -C({I, B});
                foundB = true;
            }
        }
    }
    HEPAssert(foundA,
              mty::error::RuntimeError,
              "Index " + toString(A) + " cannot be inserted with Cmatrix.")
        HEPAssert(foundB,
                  mty::error::RuntimeError,
                  "Index " + toString(B) + " cannot be inserted with Cmatrix.")
}

void ConjugationInfo::insertExternal(csl::Expr &expr) const
{
    // std::cout << *this << std::endl;
    // std::cout << "applying on " << expr << std::endl;
    csl::Tensor C     = mty::dirac4.C_matrix;
    bool        found = csl::FirstOfLeaf(expr, [&](csl::Expr &sub) {
        if (!IsOfType<QuantumField>(sub))
            return false;
        csl::IndexStructure const &structure = sub->getIndexStructureView();
        for (const auto &index : structure) {
            if (index == A) {
                csl::Index I = A.rename();
                csl::Replace(sub, A, I);
                sub = C({A, I});
                if (conjugated)
                    sub = -sub;
                return true;
            }
        }
        return false;
    });
    if (!found) {
        size_t match     = 0;
        auto   structure = expr->getIndexStructure();
        for (const auto &index : structure)
            if (index == A)
                ++match;
        if (match == 1) {
            csl::Index I = A.rename();
            csl::Replace(expr, A, I);
            // std::cout << "A" << std::endl;
            expr = expr * C({I, A});
            if (conjugated)
                expr = -expr;
            found = true;
        }
    }
    HEPAssert(found,
              mty::error::RuntimeError,
              "Index " + toString(A) + " cannot be inserted with Cmatrix.")
}

std::ostream &operator<<(std::ostream &out, ConjugationInfo::Type type)
{
    //  std::cout << "**********************" << std::endl;
    //  std::cout << "**    APPLYING      **" << std::endl;
    //  std::cout << "**********************" << std::endl;
    switch (type) {
    case ConjugationInfo::External:
        out << "External";
        break;
    case ConjugationInfo::Internal:
        out << "Internal";
        break;
    case ConjugationInfo::Undefined:
        out << "Undefined";
        break;
    }
    //  std::cout << "**********************" << std::endl;
    //  std::cout << "**       END        **" << std::endl;
    //  std::cout << "**********************" << std::endl;

    return out;
}

std::ostream &operator<<(std::ostream &out, ConjugationInfo const &info)
{
    out << "Conjugation of type " << info.getType() << '\n';
    switch (info.getType()) {
    case ConjugationInfo::External:
        out << info.getA() << " ";
        if (info.getConjugated())
            out << "(*)";
        out << "\n";
        break;
    case ConjugationInfo::Internal:
        out << info.getA() << " - " << info.getB() << '\n';
        break;
    case ConjugationInfo::Undefined:
        break;
    }

    return out;
}

} // End of namespace mty
