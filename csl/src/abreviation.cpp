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

#include "abreviation.h"
#include "comparison.h"
#include "parent.h"
#include "replace.h"
#include "variableParent.h"
#include "literal.h"
#include "indicial.h"
#include "algo.h"
#include "utils.h"
#include "scopedProperty.h"
#include "hardComparison.h"
#include "dichotomy.h"

namespace csl {

std::map<std::string, size_t> Abbrev::id_name;

bool compareString(std::string_view a, std::string_view b)
{
    size_t size = std::min(a.size(), b.size());
    for (size_t i = 0; i != size; ++i)
        if (a[i] == '_' or b[i] == '_')
            return true;
        else if (a[i] != b[i])
            return false;
    return true;
}

size_t dichoFinder(
        csl::Expr                    const &expr,
        std::vector<AbstractParent*> const &v
        )
{
    auto iter = csl::dichotomyFindIf(v.begin(), v.end(), 
            [&](AbstractParent const *parent) {
                const auto &encaps = parent->getEncapsulated();
                if (expr < encaps)
                    return +1;
                else if (encaps < expr)
                    return -1;
                return 0;
            });
    return iter - v.begin();
    // size_t first = 0;
    // size_t last = v.size();
    // while (last != first) {
    //     size_t mid = (first + last) / 2;
    //     auto const &midExpr = v[mid]->getEncapsulated();
    //     if (expr < midExpr)
    //         last = mid;
    //     else if (midExpr < expr) {
    //         if (mid == first)
    //             ++first;
    //         else
    //             first = mid;
    //     }
    //     else
    //         return mid;
    //     if (first + 1 == mid) {
    //         return (expr < v[first]->getEncapsulated()) ? first : mid;
    //     }
    // }
    // return first;
}

bool Abbrev::compareParents::operator()(
        AbstractParent* const& A,
        AbstractParent* const& B)
{
    return compare(A->getName(), B->getName()) == -1;
}

std::vector<AbstractParent*> &Abbrev::getAbbreviationsForName(
        std::string_view name
        )
{
    for (auto iter = abbreviationData.begin();
            iter != abbreviationData.end();
            ++iter) {
        if (compareString(name, iter->first.data()))
            return iter->second;
    }
    return abbreviationData[std::string(name)];
}

void Abbrev::cleanEmptyAbbreviation()
{
    auto iter = abbreviationData.begin();
    while (iter != abbreviationData.end()) {
        if (iter->second.empty())
            iter = abbreviationData.erase(iter);
        else
            ++iter;
    }
}

void Abbrev::addAbbreviation(
        AbstractParent* ptr,
        std::string const &name
        )
{
    // CSL_ASSERT_SPEC(not find_opt(ptr->getName()),
    //           CSLError::KeyError,
    //           "Abbreviation " + std::string(ptr->getName())
    //           + " already exists.");
    auto &abbreviations = getAbbreviationsForName(name.data());
    auto encapsulated = ptr->getEncapsulated();
    size_t insertionPos;
    if (useDichotomy) {
        insertionPos = dichoFinder(encapsulated, abbreviations);
        abbreviations.insert(abbreviations.begin() + insertionPos, ptr);
    }
    else {
        insertionPos = abbreviations.size();
        abbreviations.push_back(ptr);
    }
}

void Abbrev::removeAbbreviation(
        AbstractParent* ptr,
        std::string const &name
        )
{
    auto &abbreviations = getAbbreviationsForName(name.data());
    if (auto pos = std::find(abbreviations.begin(), abbreviations.end(), ptr);
            pos != abbreviations.end())
        abbreviations.erase(pos);
    if (abbreviations.empty())
        abbreviationData.erase(name);
}

void Abbrev::removeAbbreviations(std::string const &name)
{
    if (auto pos = abbreviationData.find(name); pos != abbreviationData.end())
        abbreviationData.erase(pos);
}

bool Abbrev::isAnAbbreviation(Expr const &ab) {
    auto type = ab->getType();
    switch(type) {
        case csl::Type::Constant:
        case csl::Type::Variable:
        case csl::Type::TensorElement:
        case csl::Type::TensorFieldElement:
            return ab->getParent_info()->isAnAbbreviation();
        default:
            break;
    }
    return false;
}

AbstractParent* Abbrev::find(std::string_view name)
{
    AbstractParent* res = find_opt(name);
    CSL_ASSERT_SPEC(res,
              CSLError::KeyError,
              "Abbreviation " + std::string(name) + " not found.");

    return res;
}

AbstractParent* Abbrev::find(Expr const& abreviation)
{
    return find(abreviation->getName());
}

AbstractParent* Abbrev::find_opt(std::string_view name) 
{
    for (const auto &el : abbreviationData) {
        for (auto ab : el.second)
            if (ab->getName() == name)
                return ab;
    }

    return nullptr;
}

AbstractParent* Abbrev::find_opt(Expr const& abreviation) 
{
    return find_opt(abreviation->getName());
}

void Abbrev::compressAbbreviations(std::string const &name)
{
    if (name.empty()) {
        for (auto &el : abbreviationData)
            compressAbbreviations_impl(el.second);
    }
    else {
        compressAbbreviations_impl(getAbbreviationsForName(name));
    }
}

static std::vector<csl::Expr> commonFactors(
        csl::Expr const &prod1,
        csl::Expr const &prod2
        )
{
    auto iter1 = prod1->begin();
    auto iter2 = prod2->begin();
    const auto end1 = prod1->end();
    const auto end2 = prod2->end();
    std::vector<csl::Expr> factors;
    factors.reserve(std::min(end1 - iter1, end2 - iter2));
    while (iter1 != end1 && iter2 != end2) {
        if (csl::IsNumerical(*iter1) || *iter1 < *iter2) {
            ++iter1;
        }
        else if (csl::IsNumerical(*iter2) || *iter2 < *iter1) {
            ++iter2;
        }
        else {
            if (*iter1 == *iter2) { // Should not need it if total order ...
                factors.push_back(*iter1);  // ... but we never know
            }
            ++iter1;
            ++iter2;
        }
    }
    return factors;
}

void Abbrev::compressAbbreviations_impl(
        std::vector<AbstractParent*> &abbreviations
        )
{
    for (size_t i = 0; i != abbreviations.size(); ++i) {
        csl::Expr encaps_i = abbreviations[i]->getEncapsulated();
        if (!csl::IsProd(encaps_i) || csl::IsIndexed(encaps_i))
            continue;
        for (size_t j = i+1; j < abbreviations.size(); ++j) {
            csl::Expr encaps_j = abbreviations[i]->getEncapsulated();
            if (!csl::IsProd(encaps_j) || csl::IsIndexed(encaps_j))
                continue;
            const int diff = std::abs(
                    static_cast<int>(csl::Size(encaps_i)) 
                    - static_cast<int>(csl::Size(encaps_j))
                    );
            if (diff <= 2) {
                [[maybe_unused]]
                std::vector<csl::Expr> factors = commonFactors(
                        encaps_i, encaps_j);
            }
        }
    }
}

std::string Abbrev::getFinalName(std::string_view initialName)
{
    std::string init(initialName);
    if (id_name.find(init) == id_name.end()) {
        id_name[init] = 0;
        return init;
    }
    std::string newName = toString(++id_name[init]);
    for (size_t i = 0; newName.size() < 4; ++i)
        newName = '0' + newName;
    return init + "_" + newName;
}

void Abbrev::printAbbreviations(std::ostream& fout)
{
    for (const auto &data : abbreviationData) {
        printAbbreviations(data.first, fout);
    }
}

void Abbrev::printAbbreviations(
        std::string_view name,
        std::ostream    &fout
        )
{
    auto abbreviations = getAbbreviationsForName(name);
    std::sort(abbreviations.begin(), abbreviations.end(), 
            [&](AbstractParent const *left, AbstractParent const *right) {
                return left->getName() < right->getName();
            });
    fout << abbreviations.size() << " \"" << name << "\" abbreviations:\n";
    for (const auto& ab : abbreviations) {
        fout << ab->getName() << " = " 
             << ab->getEncapsulated() << std::endl;
    }
}

void Abbrev::enableEvaluation(std::string_view name)
{
    find(name)->enableEvaluation();
}

void Abbrev::disableEvaluation(std::string_view name)
{
    find(name)->disableEvaluation();
}

void Abbrev::toggleEvaluation(std::string_view name)
{
    find(name)->toggleEvaluation();
}

void Abbrev::enableGenericEvaluation(std::string_view name)
{
    auto &abbreviations = getAbbreviationsForName(name);
    if (abbreviations.empty()) {
        abbreviationData.erase(std::string(name));
        return;
    }
    for (auto& ab : abbreviations)
        ab->enableEvaluation();
}

void Abbrev::disableGenericEvaluation(std::string_view name)
{
    auto &abbreviations = getAbbreviationsForName(name);
    if (abbreviations.empty()) {
        abbreviationData.erase(std::string(name));
        return;
    }
    for (auto& ab : abbreviations)
        ab->disableEvaluation();
}

void Abbrev::toggleGenericEvaluation(std::string_view name)
{
    auto &abbreviations = getAbbreviationsForName(name);
    if (abbreviations.empty()) {
        abbreviationData.erase(std::string(name));
        return;
    }
    for (auto& ab : abbreviations)
        ab->toggleEvaluation();
}

void Abbrev::enableEvaluation(Expr const& abreviation)
{
    find(abreviation)->enableEvaluation();
}

void Abbrev::disableEvaluation(Expr const& abreviation)
{
    find(abreviation)->disableEvaluation();
}

void Abbrev::toggleEvaluation(Expr const& abreviation)
{
    find(abreviation)->toggleEvaluation();
}

csl::IndexStructure Abbrev::getFreeStructure(
        csl::IndexStructure const& structure)
{
    auto freeStructure = structure;
    for (size_t i = 0; i != freeStructure.size(); ++i) 
        if (freeStructure[i].getType() == cslIndex::Fixed) {
            freeStructure.erase(freeStructure.begin() + i);
            --i;
        }
        else if (i < freeStructure.size()-1 
                and freeStructure[i].getType() == cslIndex::Dummy) {
            for (size_t j = i + 1; j != freeStructure.size(); ++j)
                if (freeStructure[i] == freeStructure[j]) {
                    freeStructure.erase(freeStructure.begin() + j);
                    freeStructure.erase(freeStructure.begin() + i);
                    --i;
                    break;
                }
        }
    return freeStructure;
}

csl::IndexStructure Abbrev::getFreeStructure(
        Expr const& expr)
{
    auto structure = expr->getIndexStructure();
    return getFreeStructure(structure);
}

std::optional<Expr> Abbrev::findExisting(
        std::string_view name,
        Expr        const& encapsulated)
{
    auto &abbreviations = getAbbreviationsForName(name);
    auto structure = getFreeStructure(encapsulated);
    if (structure.empty()) {
        if (useDichotomy and !encapsulated->isIndexed()) {
            size_t pos = dichoFinder(encapsulated, abbreviations);
            if (pos < abbreviations.size() 
                    && abbreviations[pos]->getEncapsulated()->compareWithDummy(
                        encapsulated.get())) {
                return dynamic_cast<LiteralParent*>(abbreviations[pos])->generateInstance();
            }
            if (pos-1 < abbreviations.size() 
                    && abbreviations[pos-1]->getEncapsulated()->compareWithDummy(
                        encapsulated.get())) {
                return dynamic_cast<LiteralParent*>(abbreviations[pos-1])->generateInstance();
            }
            if (pos+1 < abbreviations.size() 
                    && abbreviations[pos+1]->getEncapsulated()->compareWithDummy(
                        encapsulated.get())) {
                return dynamic_cast<LiteralParent*>(abbreviations[pos+1])->generateInstance();
            }
        }
        else {
            for (const auto& ab : abbreviations) {
                if (ab->getEncapsulated() == encapsulated) {
                    auto ptr = dynamic_cast<LiteralParent*>(ab);
                    if (ptr)
                        return ptr->generateInstance();
                }
            }
        }
    }
    else {
        for (const auto& ab : abbreviations) {
            Expr comparison = ab->getEncapsulated();
            auto ab_ptr = dynamic_cast<Abbreviation<TensorParent>*>(ab);
            if (not ab_ptr) {
                continue;
            }
            if (structure.size() != ab_ptr->initialStructure.size()) {
                continue;
            }
            if (comparison->getType() != encapsulated->getType()
                    || comparison->size() != encapsulated->size()) {
                continue;
            }
            bool diff = false;
            for (size_t i = 0; i != encapsulated->size(); ++i) 
                if (encapsulated[i]->getType() != comparison[i]->getType()) {
                    diff = true;
                    break;
                }
            if (diff) {
                continue;
            }
            csl::Replace(comparison, ab_ptr->initialStructure, structure);
            //for (size_t i = 0; i != ab_ptr->initialStructure.size(); ++i)
            //    Replace(comparison,
            //            ab_ptr->initialStructure[i],
            //            intermediate[i],
            //            false);
            //for (size_t i = 0; i != ab_ptr->initialStructure.size(); ++i)
            //    Replace(comparison,
            //            intermediate[i],
            //            structure[i],
            //            false);
            std::map<csl::Index, csl::Index> mapping;
            if (encapsulated->compareWithDummy(comparison.get(), mapping))
                return (*ab)(structure.getIndex());
        }
    }

    return std::nullopt;
}

Expr termWithoutExponent(Expr const &expr)
{
    if (expr->getType() == csl::Type::Pow) { //Pow 
        if (expr->getArgument(1)->getPrimaryType() == csl::PrimaryType::Numerical
                or expr->getArgument() == CSL_E) {
            return expr->getArgument();
        }
    }
    return expr;
}

Expr Abbrev::makeSubAbbrev(
        std::vector<Expr> const &encapsulated,
        bool                     isProd
        )
{
    if (encapsulated.empty())
        return (isProd) ? CSL_1 : CSL_0;
    if (encapsulated.size() == 1 && encapsulated[0]->size() == 0)
        return encapsulated[0];

    return (isProd) ? 
        makeAbbreviation(prod_s(encapsulated), false)
        :makeAbbreviation(sum_s(encapsulated), false);
}

Expr Abbrev::makeAbbreviation(std::string name,
                              Expr const& encapsulated,
                              bool        split)
{
    // Important refresh to ensure the mathematical equivalence between the
    // expression tested here and the entering later the abbreviation
    Expr encaps = DeepRefreshed(encapsulated);
    if (encaps->size() == 0) // nothing to abbreviate
        return encaps;
    if (csl::IsProd(encaps) && csl::IsNumerical(encaps[0])
            && csl::Size(encaps) > 2) {
        auto prod = csl::prod_s(
                std::vector<csl::Expr>(encaps->begin()+1, encaps->end()), true);
        auto prodAbbrev = makeAbbreviation(name, prod, split);
        return makeAbbreviation(name, prodAbbrev*encaps[0], split);
    }
    if (name == "Ab" 
            && split 
            && (csl::IsSum(encaps) || csl::IsProd(encaps))) {
        const size_t size = csl::Size(encaps);
        std::vector<csl::Expr> abbrevs;
        std::vector<csl::Expr> numericals;
        std::vector<csl::Expr> funcs;
        std::vector<csl::Expr> multis;
        std::vector<csl::Expr> tensors;
        std::vector<csl::Expr> others;
        for (size_t i = 0; i != size; ++i) { 
            auto const &arg = encaps[i];
            if (isAnAbbreviation(arg)) {
                abbrevs.push_back(arg);
            }
            else {
                auto term = termWithoutExponent(arg);
                if (term->size() > 0)
                    multis.push_back(arg);
                else if (csl::IsIndicialTensor(term))
                    tensors.push_back(arg);
                else if (csl::AllOfLeafs(arg, [&](Expr const &s) { 
                            return csl::IsNumerical(s); 
                            }))
                    numericals.push_back(arg);
                else if (csl::IsScalarFunction(arg))
                    funcs.push_back(arg);
                else
                    others.push_back(arg);
            }
        }
        bool isProd = csl::IsProd(encaps);
        std::vector<csl::Expr> last(6);
        last[0] = makeSubAbbrev(abbrevs, isProd);
        last[1] = makeSubAbbrev(funcs,   isProd);
        last[2] = makeSubAbbrev(multis,  isProd);
        last[3] = makeSubAbbrev(tensors, isProd);
        last[4] = makeSubAbbrev(others,  isProd);
        last[5] = (isProd) ? prod_s(numericals) : sum_s(numericals);
        auto comb = isProd ? csl::prod_s(last) : csl::sum_s(last);
        if (isAnAbbreviation(comb)) {
            return comb;
        }
        return makeAbbreviation(name, comb, false);
        // Replace by this to enable composed abbreviations
        // Ab_3 = Ab_1*Ab_2*Ab_3
        // Increases the total number of abbreviations ...
        //
        // auto res = makeAbbreviation(name, comb, false);
        // return res;
    }
    if (avoidDuplicates) {
        if (auto ab = findExisting(name, encaps); ab)
            return ab.value();
    }
    csl::ComplexProperty prop;
    if (encaps->isReal())
        prop = csl::ComplexProperty::Real;
    else if (encaps->isPurelyImaginary())
        prop = csl::ComplexProperty::Imaginary;
    else
        prop = csl::ComplexProperty::Complex;

    IndexStructure freeStructure = getFreeStructure(encaps);

    if (freeStructure.empty()) {
        if (csl::AllOfLeafs(encaps,
                    [](const Expr& expr) {
                      return  expr->getPrimaryType() == csl::PrimaryType::Numerical
                        or (expr->getPrimaryType() == csl::PrimaryType::Literal
                                and expr->getType() != csl::Type::Variable);
                          },
                     -1)) {

            std::shared_ptr<ConstantParent> parent 
                = csl::make_shared<Abbreviation<ConstantParent>>(
                    encaps,
                    name);
            Expr res = csl::make_shared<Constant, alloc_constant>(parent);
            res->setComplexProperty(prop);
            return res;
        }
        std::shared_ptr<VariableParent> parent 
            = csl::make_shared<Abbreviation<VariableParent>>(
                encaps,
                name);
        Expr res = csl::make_shared<Variable, alloc_variable>(parent);
        res->setComplexProperty(prop);
        return res;
    }

    csl::IndexStructure fullStructure = encaps->getIndexStructure();
    std::vector<const Space*> spaces(freeStructure.size());
    for (size_t i = 0; i != spaces.size(); ++i)
        spaces[i] = freeStructure[i].getSpace();
    Tensor parent 
        = csl::make_shared<Abbreviation<TensorParent>>(
            encaps,
            name,
            spaces,
            CSL_UNDEF);
    parent->setComplexProperty(prop);

    return parent(freeStructure.getIndex());
}

Expr Abbrev::makeAbbreviation(Expr const& encapsulated, bool split)
{
    return makeAbbreviation("Ab",
                           encapsulated,
                           split);
}

} // End of namespace csl
