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

#include "interactionTerm.h"
#include "diracology.h"
#include "mrtError.h"

using namespace csl;
using namespace std;

namespace mty {

///////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////

InteractionTerm::InteractionTerm(csl::Expr const& t_term,
                                 bool        copy)
{
    term = (copy) ? DeepCopy(t_term) : t_term;
    DeepExpandIf(term, [&](csl::Expr const &sum)
    {
        return AnyOfLeafs(sum, [&](csl::Expr const &sub)
        {
            return IsOfType<QuantumField>(sub);
        });
    });
    csl::Factor(term);
    factors.reserve(10);
    permutations.reserve(10);
    content.reserve(5);
    gatherFieldContent();
    gatherFactorsAndSymmetries();
}

InteractionTerm::InteractionTerm(
        csl::Expr                 const& t_term,
        vector<QuantumField> const& t_content)
    :term(DeepCopy(t_term)),
    content(t_content),
    forcedContent(true)
{
    factors.reserve(10);
    permutations.reserve(10);
    gatherFieldContent();
    gatherFactorsAndSymmetries();
}

InteractionTerm::InteractionTerm(InteractionTerm const& other)
    :InteractionTerm(other.getFullExpression(), false)
{

}

InteractionTerm InteractionTerm::copy() const
{
    csl::Expr expr = getFullExpression();
    RenameIndices(expr);
    return InteractionTerm(expr, false);
}

InteractionTerm& InteractionTerm::operator=(InteractionTerm const& other)
{
    clear();
    term = other.getFullExpression();
    gatherFieldContent();
    gatherFactorsAndSymmetries();
    return *this;
}

void abbreviantLINT(csl::Expr &prod)
{
    auto mayBeAbbreviate = [&](csl::Expr const &expr)
    {
        if (csl::IsNumerical(expr) or csl::IsIndexed(expr))
            return false;
        return csl::AllOfLeafs(expr, [&](csl::Expr const &sub)
        {
            return (!IsOfType<QuantumField>(sub));
        });
    };

    std::vector<std::size_t> abbreviate;
    abbreviate.reserve(prod->size());
    for (std::size_t i = 0; i != prod->size(); ++i) 
        if (mayBeAbbreviate(prod[i]))
            abbreviate.push_back(i);

    if (abbreviate.size() > 1) {
        std::vector<csl::Expr> abbreviation;
        abbreviation.reserve(abbreviate.size());
        for (std::size_t i : abbreviate) {
            abbreviation.push_back(prod[i]);
            prod[i] = CSL_1;
        }
        prod[abbreviate[0]] = csl::Abbrev::makeAbbreviation(
                "L_term",
                csl::prod_s(abbreviation, true)
                );
        csl::Refresh(prod);
    }
}

void InteractionTerm::fillContent(
        csl::Expr const &prod,
        std::vector<mty::QuantumField> &fields
        )
{
    InteractionTerm term;
    term.term = prod;
    term.gatherFieldContent();
    fields.reserve(5);
    for (const auto &f : term.content)
        fields.push_back(std::move(f));
}

vector<InteractionTerm::TermType> 
    InteractionTerm::createAndDispatch(
        csl::Expr const& expression
        )
{
    csl::Expr abbreviated = expression;
    csl::Expr expanded = DeepExpandedIf_lock(abbreviated, [&](csl::Expr const &sum)
    {
        return AnyOfLeafs(sum, [&](csl::Expr const &sub)
        {
            return sub->isIndexed()
                or IsOfType<QuantumField>(sub);
        });
    });
    if (expanded->getType() != csl::Type::Sum) {
            TermType interac = std::make_shared<InteractionTerm>(expanded);
            if (interac->content.empty())
                return vector<TermType>();
            return vector<TermType>(1, interac);
        }

    std::vector<std::vector<mty::QuantumField>> contents(expanded->size());
    std::vector<csl::Expr> fieldProducts(contents.size());
    for (std::size_t i = 0; i != contents.size(); ++i) {
        fillContent(expanded[i], contents[i]);
    }
    std::vector<std::size_t> indicesLeft(contents.size());
    for (std::size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;
    vector<TermType> interac;
    interac.reserve(expanded->size());
    while (!indicesLeft.empty()) {
        std::vector<csl::Expr> terms {expanded[indicesLeft[0]]};
        for (std::size_t j = 1; j != indicesLeft.size(); ++j) {
            if (areSimilarContent(contents[indicesLeft[0]],
                                  contents[indicesLeft[j]])) {
                terms.push_back(expanded[indicesLeft[j]]);
                indicesLeft.erase(indicesLeft.begin() + j);
                --j;
            }
        }
        csl::Expr term = csl::sum_s(terms, true);
        interac.push_back(
                std::make_shared<InteractionTerm>(term, false)
                );
        indicesLeft.erase(indicesLeft.begin());
    }
    return interac;
    std::vector<std::size_t> toRefresh;
    interac.reserve(expanded->size());
    for (auto& term : *expanded) {
        TermType foo = std::make_shared<InteractionTerm>(term);
        if (foo->content.empty())
            continue;
        bool found = false;
        for (std::size_t i = 0; i != interac.size(); ++i) 
            if (foo->hasSameContent(*interac[i])) {
                found = true;
                interac[i]->term += term;
                toRefresh.push_back(i);
                break;
            }
        if (not found) {
            interac.push_back(std::make_shared<InteractionTerm>(term));
        }
    }
    //csl::Abbrev::enableGenericEvaluation("L_term");
    //for (auto &i : interac)
    //    i = std::make_shared<InteractionTerm>(
    //            csl::Evaluated(i->getTerm())
    //            );
    for (std::size_t i : toRefresh)
        interac[i] = std::make_shared<InteractionTerm>(interac[i]->getTerm());

    return interac;
}

///////////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////////

std::size_t InteractionTerm::size() const
{
    return content.size();
}

std::size_t InteractionTerm::nTerms() const
{
    return permutations.size();
}

void InteractionTerm::clear()
{
    forcedContent = false;
    content.clear();
    globalFactor = CSL_1;
    globalPermutation.clear();
    factors.clear();
    permutations.clear();
}

bool InteractionTerm::hasSameContent(InteractionTerm const& other) const
{
    return hasSameContent(other.content);
}

bool InteractionTerm::hasSameContent(
        std::vector<mty::QuantumField> const &fields
        ) const
{
    return areSimilarContent(content, fields);
}

csl::Expr InteractionTerm::getTerm() const 
{
    return term;
}

csl::Expr InteractionTerm::getGlobalFactor() const
{
    return globalFactor;
}

csl::Expr InteractionTerm::getTotalFactor() const
{
    return globalFactor * ((factors.empty()) ? CSL_1 : sum_s(factors));
}

csl::Expr InteractionTerm::getMass() const
{
    HEPAssert(content.size() == 2,
            mty::error::TypeError,
            "Cannot obtain a mass from the non-mass term " + toString(*this))
    csl::Expr mass = csl::DeepCopy(globalFactor);
    csl::ForEachLeaf(mass, [&](csl::Expr &sub) {
        if (csl::IsIndicialTensor(sub) && mty::dirac4.isCMatrix(sub)) {
            if (sub->getIndexStructureView()[0] 
                    == content[0].getIndexStructureView().back()) {
                sub = CSL_1;
            }
            else {
                sub = CSL_M_1;
            }
        }
    });
    auto part = content[0].getQuantumParent();
    if (part->getSpinDimension() != 3)
        mass = -mass;
    if (part->isSelfConjugate())
        mass = 2 * mass;
    csl::DeepExpand(mass);
    csl::DeepFactor(mass);
    if (part->isBosonic())
        mass = csl::sqrt_s(mass);
    return mass;
}

vector<QuantumField>& InteractionTerm::getContent()
{
    return content;
}

vector<QuantumField> const& InteractionTerm::getContent() const
{
    return content;
}

vector<Particle> InteractionTerm::getParticles() const
{
    vector<Particle> particles;
    particles.reserve(content.size());
    for (const auto& field : content)
        particles.push_back(dynamic_pointer_cast<QuantumFieldParent>
                (field.getParent()));

    return particles;
}

std::size_t InteractionTerm::count(QuantumFieldParent* particle) const
{
    std::size_t count = 0;
    for (const auto& field : content)
        if (field.getParent_info() == particle)
            ++count;

    return count;
}

std::size_t InteractionTerm::getExponent(csl::Expr const& parameter) const 
{
    std::size_t count = 0;
    csl::AnyOfLeafs(globalFactor, [&count, &parameter](csl::Expr const& el) 
    {
        if (el == parameter) {
          ++count;
          return true;
        }
        else if (el->getType() == csl::Type::Pow) {
          if (el->getArgument(0) == el
                  and el->getArgument(1)->isInteger()) {
            count += el->getArgument(1)->evaluateScalar();
            return true;
          }
        }
        return false;
    });

    return count;
}

vector<QuantumFieldParent*> InteractionTerm::getParticlesInfo() const
{
    vector<QuantumFieldParent*> particles;
    particles.reserve(content.size());
    for (const auto& field : content)
        particles.push_back(dynamic_cast<QuantumFieldParent*>
                (field.getParent().get()));

    return particles;
}

csl::Expr InteractionTerm::applyFactorAndSymmetriesOn(csl::Expr const& init) const
{
    std::map<csl::Index, csl::Index> constraints;
    csl::Expr global = prod_s(DeepCopy(globalFactor),
                        applyPermutation(globalPermutation, init, constraints),
                        true);
    if (init->getType() == csl::Type::Sum)
        Expand(global);
    if (factors.empty()) {
        RenameIndices(global);
        return global;
    }

    csl::vector_expr terms(factors.size());
    for (std::size_t i = 0; i != factors.size(); ++i) {
        terms[i] = prod_s(DeepCopy(factors[i]),
                         applyPermutation(permutations[i], global, constraints),
                         true);
        // terms[i] = Expanded(DeepRefreshed(terms[i]));
    }
    csl::Expr res = sum_s(terms, true);
    RenameIndices(res);
    return res;
}

csl::Tensor InteractionTerm::getPoint() const
{
    if (content.empty())
        return nullptr;
    return content[0].getPoint();
}

void InteractionTerm::setPoint(csl::Tensor const& point)
{
    if (getPoint().get() == point.get())
        return;
    term = Replaced(term,
                   getPoint(),
                   point);
    for (auto& field : content)
        field.setPoint(point);
}

bool InteractionTerm::contains(const QuantumFieldParent* f) const
{
    for (const auto& field : content)
        if (field.getQuantumParent()->contains(f))
            return true;

    return false;
}

bool InteractionTerm::containsWeakly(const QuantumFieldParent* f) const
{
    for (const auto& field : content)
        if (field.getQuantumParent()->contains(f)
                || f->contains(field.getQuantumParent()))
            return true;

    return false;
}

bool InteractionTerm::containsExactly(const QuantumFieldParent* f) const
{
    for (const auto& field : content)
        if (field.getParent_info() == f)
            return true;

    return false;
}

csl::Expr InteractionTerm::getFieldProduct() const
{
    csl::vector_expr terms(content.size());
    std::transform(
            content.begin(),
            content.end(),
            terms.begin(),
            [](QuantumField const& t_field){
                 QuantumField field = t_field;
                 csl::IndexStructure structure 
                      = field.getDerivativeStructure();
                 field.setDerivativeStructure(csl::IndexStructure());
                 csl::Expr res = field.copy();
                 for (const auto& index : structure)
                     res = partialMinko(index,
                                        field.getPoint()) * res;
                 return res;
            });

    return prod_s(terms);
}

csl::Expr InteractionTerm::getFullExpression() const
{
    csl::Expr res = applyFactorAndSymmetriesOn(getFieldProduct());
    res = DeepRefreshed(res);
    if (csl::Abbrev::getFreeStructure(res->getIndexStructure()).size() > 0) {
        std::cerr << "Term : " << getTerm() << '\n';
        std::cerr << "Res  : " << res << '\n';
        CallHEPError(mty::error::IndexError, "")
    }
    return res;
}

bool InteractionTerm::operator==(const InteractionTerm& other) const
{
    if (not hasSameContent(other))
        return false;
    return term == other.term;
}

bool InteractionTerm::operator==(const csl::Expr& other) const
{
    return term == other;
}

///////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////

void InteractionTerm::gatherFieldContent()
{
    gatherFieldContent(term, 1, csl::IndexStructure());
    if (term->getType() == csl::Type::Sum) {
        globalFactor = CSL_1;
        globalPermutation.clear();
    }
}

bool InteractionTerm::gatherFieldContent(
        csl::Expr               const& expr,
        std::size_t                    multiplicity,
        csl::IndexStructure const& structure)
{
    switch(expr->getType()) {

        case csl::Type::Prod:
        {
        bool gathered = false;
        for (const auto& arg : *expr) {
            if (gatherFieldContent(arg, multiplicity, structure))
                gathered = true;
        }
        return gathered;
        }

        case csl::Type::Pow:
        {
        bool gathered = gatherFieldContent(
                expr->getArgument(0),
                multiplicity*expr->getArgument(1)->evaluateScalar(),
                structure);
        if (gathered) {
            HEPAssert(expr->getArgument(1)->isInteger()
                    and expr->getArgument(1)->evaluateScalar() > 0,
                    mty::error::ValueError,
                    "Power of fields allowed only with" + 
                    static_cast<string>(" positive integer power."));
        }
        return gathered;
        }

        case csl::Type::TensorFieldElement:
        if (IsOfType<QuantumField>(expr)) {
            auto [copyField, copyStructure] = buildContent(
                    ConvertTo<QuantumField>(expr),
                    structure);
            recallIndices(globalPermutation,
                          expr->getIndexStructureView(),
                          copyField->getIndexStructureView());
            recallIndices(globalPermutation,
                          structure,
                          copyStructure);
            QuantumField staticField = ConvertTo<QuantumField>(copyField);
            staticField.setDerivativeStructure(copyStructure);
            for (std::size_t i = 0; i != multiplicity; ++i) {
                content.push_back(staticField);
            }
            return true;
        }
        return false;
        break;

        case csl::Type::TDerivativeElement:
        return gatherFieldContent(
                expr->getOperand(),
                multiplicity,
                structure + expr->getIndexStructureView()[0]);

        default:
        return false;
    }
}

bool InteractionTerm::containsQuantumField(csl::Expr const& expr)
{
    if (IsOfType<QuantumField>(expr))
        return true;
    else if (expr->size() > 0){
        for (std::size_t i = 0; i != expr->size(); ++i)
            if (containsQuantumField(expr->getArgument(i)))
                return true;
    }
    else if (expr->isAnOperator())
        return containsQuantumField(expr->getOperand());

    return false;
}

pair<csl::Expr, csl::IndexStructure> InteractionTerm::buildContent(
        QuantumField              field,
        csl::IndexStructure const& structure)
{
    if (not forcedContent) {
        csl::Expr copyField = field.copy();
        csl::IndexStructure copyStructure = structure;
        copyField->resetIndexStructure();
        copyStructure.reset();

        return make_pair(copyField, copyStructure);
    }

    field.setDerivativeStructure(structure);
    for (auto iter = content.begin(); iter != content.end(); ++iter)
        if (areSimilarField(field, *iter)) {
            csl::IndexStructure derivStruct = iter->getDerivativeStructure();
            csl::Expr exprField = iter->copy();
            content.erase(iter);
            return make_pair(exprField, derivStruct);
        }

    cout << "For field: ";
    field.print();
    cout << field.getDerivativeStructure() << endl;

    for (const auto& c : content) {
        c.print();
        cout << c.getDerivativeStructure() << endl;
    }

    CallHEPError(mty::error::RuntimeError,
            static_cast<string>("Corresponding field content not found in ")
            + "InteractionTerm::buildContent()");

    return make_pair(nullptr, csl::IndexStructure());
}

void InteractionTerm::recallIndices(permutation             & perm,
                                    csl::IndexStructure const& old,
                                    csl::IndexStructure const& structure)

{
    HEPAssert(old.size() == structure.size(),
            mty::error::ValueError,
            "Size of IndexStructure mismatch: " + toString(old.size())
            + " and " + toString(structure) + ".");
    for (std::size_t i = 0; i != old.size(); ++i) {
        perm.push_back(make_pair(structure[i], old[i]));
    }
}

void InteractionTerm::gatherFactorsAndSymmetries()
{
    if (term->getType() == csl::Type::Prod) {
        for (const auto& arg : *term)
            if (not containsQuantumField(arg))
                globalFactor = globalFactor * arg;
    }
    if (term->getType() == csl::Type::Sum
            and containsQuantumField(term))
        gatherSymmetries(term);
    else if (term->getType() == csl::Type::Prod) {
        for (const auto& arg : *term)
            if (arg->getType() == csl::Type::Sum
                    and containsQuantumField(arg)) {
                gatherSymmetries(arg);
                break;
            }
    }
    if (abbreviateFactors) {
        csl::ForEachNode(globalFactor, [&](csl::Expr &sub)
        {
            if (csl::IsProd(sub))
                abbreviantLINT(sub);
            else if (csl::IsSum(sub)) {
                if (csl::AnyOfLeafs(sub, [&](csl::Expr const &term)
                    {
                        return IsOfType<QuantumField>(term)
                            or csl::IsIndexed(term);
                    }))
                    return;
                sub = sub->getNumericalFactor()
                    * csl::Abbrev::makeAbbreviation(
                        "L_term",
                        csl::Refreshed(csl::GetTerm(sub))
                        );
            }
        });
        for (auto &f : factors)
            csl::ForEachNode(f, [&](csl::Expr &sub)
            {
                if (csl::IsProd(sub))
                    abbreviantLINT(sub);
                else if (csl::IsSum(sub)) {
                    if (csl::AnyOfLeafs(sub, [&](csl::Expr const &term)
                        {
                            return IsOfType<QuantumField>(term)
                                or csl::IsIndexed(term);
                        }))
                        return;
                    sub = sub->getNumericalFactor()
                        * csl::Abbrev::makeAbbreviation(
                            "L_term",
                            csl::Refreshed(csl::GetTerm(sub))
                            );
                }
            });
    }
    for (std::size_t i = 0; i != factors.size(); ++i) {
        for (std::size_t j = i+1; j < factors.size(); ++j)
            if (std::is_permutation(
                        permutations[i].begin(),
                        permutations[i].end(),
                        permutations[j].begin(),
                        permutations[j].end()
                        )) {
                factors[i] += factors[j];
                factors.erase(factors.begin() + j);
                permutations.erase(permutations.begin() + j);
                --j;
            }
    }
}

void InteractionTerm::gatherSymmetries(csl::Expr const& sum)
{
    if (sum->empty())
        return;
    csl::Expr arg = sum->getArgument(0);
    InteractionTerm worker(arg, false);
    vector<QuantumField> &newContent = worker.content;
    content.insert(content.end(),
                   newContent.begin(),
                   newContent.end());
    factors.push_back(std::move(worker.globalFactor));
    permutations.push_back(std::move(worker.globalPermutation));

    for (std::size_t i = 1; i != sum->size(); ++i) {
        csl::Expr arg = sum->getArgument(i);
        InteractionTerm worker(arg, newContent);
        factors.push_back(std::move(worker.globalFactor));
        permutations.push_back(std::move(worker.globalPermutation));
    }
}

bool InteractionTerm::areSimilarField(QuantumField const& A,
                                      QuantumField const& B)
{
    return (A.getParent_info() == B.getParent_info()
        and (A.getComplexProperty() == csl::ComplexProperty::Real
               or A.getConjugated() == B.getConjugated())
        and A.getDerivativeStructure().size()
            == B.getDerivativeStructure().size());
}

bool InteractionTerm::areSimilarContent(
        std::vector<QuantumField> const &A,
        std::vector<QuantumField> const &B
        )
{
    const std::size_t szA = A.size();
    if (szA != B.size())
        return false;
    std::vector<std::size_t> indicesLeft(szA);
    for (std::size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;

    for (std::size_t i = 0; i != szA; ++i) {
        const auto &fieldA = A[i];
        bool match = false;
        for (std::size_t k = 0; k != indicesLeft.size(); ++k) {
            const auto &fieldB = B[indicesLeft[k]];
            if (areSimilarField(fieldA, fieldB)) {
                indicesLeft.erase(indicesLeft.begin() + k);
                --k;
                match = true;
                break;
            }
            else if (fieldA.isAntiCommuting() && fieldB.isAntiCommuting()) {
                return false;
            }
        }
        if (!match)
            return false;
    }
    return true;
}

csl::Expr InteractionTerm::applyPermutation(
        permutation const& perm,
        csl::Expr        const& init,
        std::map<csl::Index, csl::Index>& constraints)

{
    auto getIndex = [](csl::Index const& init,
                       std::map<csl::Index, csl::Index>& constraints)
    {
        for (const auto& [A, B] : constraints)
            if (A == init) {
                auto newIndex = B;
                newIndex.setSign(init.getSign());
                newIndex.setFree(init.getFree());
                return init;// return newIndex;
            }
        constraints[init] = init.rename();
        auto newIndex = constraints[init];
        newIndex.setSign(init.getSign());
        newIndex.setFree(init.getFree());
        return init; // return newIndex;
    };
    if (perm.empty())
        return init;
    csl::Expr res = init->copy();
    csl::ForEachNode(res, [&](csl::Expr &sub)
    {
        for (const auto& replacement : perm)
            Replace(
                    sub,
                    replacement.first,
                    getIndex(replacement.second, constraints),
                    false
                    );
    });

    return res;
}

ostream& operator<<(ostream              & out,
                    InteractionTerm const& term)
{
    out << "Initial term : ";
    term.term->print();
    out << "Content:\n";
    for (const auto& field : term.content) {
        field.print(1); 
        out << " " << field.getDerivativeStructure();
        out << endl;
    }
    out << "global factor = ";
    term.globalFactor->print();

    out << "global perm : ";
    for (const auto& pair : term.globalPermutation)
        out << "( "<< pair.first << " , " << pair.second << " )  ";
    out << endl;

    out << term.factors.size() << " terms:"<< endl;
    for (std::size_t i = 0; i != term.factors.size(); ++i) {
        out << "Term " << i << ": \n";
        term.factors[i]->print();
        for (const auto& pair : term.permutations[i])
            out << "( "<< pair.first << " , " << pair.second << " )  ";
        out << endl;
    }

    return out;
}

} // End of namespace mty
