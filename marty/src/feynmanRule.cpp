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

#include "feynmanRule.h"
#include "interactionTerm.h"
#include "quantumFieldTheory.h"
#include "diracology.h"
#include "amplitude.h"
#include "amplitudeSimplification.h"
#include "mrtOptions.h"
#include "mrtInterface.h"
#include "model.h"
#include "wilson.h"

using namespace csl;
namespace mty {

FeynmanRule::FeynmanRule()
    :expr(CSL_0)
{

}

FeynmanRule::FeynmanRule(FeynmanRule const& other)
{
    *this = other;
}

FeynmanRule& FeynmanRule::operator=(FeynmanRule const& other)
{
    fieldProduct = other.fieldProduct;
    setInteractionTerm(other.term);
    diagram = other.diagram;
    expr = DeepRefreshed(other.expr);
    renameIndices();

    return *this;
}

FeynmanRule::FeynmanRule(
        mty::Model               &model,
        TermType           const &t_term)
    :FeynmanRule()
{
    std::vector<csl::Tensor> momenta(t_term->getContent().size());
    setInteractionTerm(t_term);
    std::vector<csl::Expr> insertions;
    auto terms = InteractionTerm::createAndDispatch(
            GetHermitianConjugate(term->getTerm(), &dirac4));
    if (terms.empty()) {
        return;
    }
    auto hermitic = std::make_shared<InteractionTerm>(*terms[0]);
    fieldProduct = hermitic->getContent();

    std::vector<QuantumFieldParent*> nonphysical;
    bool fieldProjected = false;
    for (size_t i = 0; i != fieldProduct.size(); ++i) {
        auto const &pointed = *fieldProduct[i].getParent();
        if (typeid(FieldStrength) == typeid(pointed)) {
            auto vectorBoson = dynamic_cast<FieldStrength*>(
                    fieldProduct[i].getParent().get())->getVectorParent();
            bool conjug = fieldProduct[i].isComplexConjugate();
            fieldProduct[i] = ConvertTo<QuantumField>(
                    vectorBoson->getInstance());
            fieldProduct[i].setConjugated(conjug);
        }
        else if (typeid(WeylFermion) == typeid(pointed)) 
            fieldProjected = true;
        if (not fieldProduct[i].isPhysical()) {
            nonphysical.push_back(fieldProduct[i].getQuantumParent());
            fieldProduct[i].getQuantumParent()->setPhysical(true);
        }
        fieldProduct[i].setParticle(fieldProduct[i].isComplexConjugate());
        fieldProduct[i].setIncoming(true);
        fieldProduct[i].setExternal(true);
        momenta[i] = csl::Tensor("p_" + std::to_string(i+1), &csl::Minkowski);
        fieldProduct[i].setPoint(momenta[i]);
        fieldProduct[i].resetIndexStructure();
        fieldProduct[i].setDerivativeStructure(csl::IndexStructure());
        csl::Expr shared = csl::make_shared<QuantumField>(fieldProduct[i]);
        insertions.push_back(shared);
    }
    for (size_t i = 0; i != std::ceil(fieldProduct.size() / 2.); ++i) {
        size_t i1 = i;
        size_t i2 = fieldProduct.size() - 1 - i;
        if (i1 != i2) {
            std::swap(insertions[i1], insertions[i2]);
            std::swap(momenta[i1], momenta[i2]);
            fieldProduct[i2].conjugate();
        }
        fieldProduct[i1].conjugate();
    }
    csl::ScopedProperty p1(&mty::option::applyMomentumConservation, false);
    csl::ScopedProperty p2(&mty::option::applyInsertionOrdering,    false);
    FeynOptions options;
    options.setFeynmanRuleMode();
    options.setWilsonOperatorBasis(OperatorBasis::None);
    Kinematics kinematics(GetInsertion(insertions), momenta);
    auto res = model.computeAmplitude(
            Order::TreeLevel,
            GetInsertion(insertions),
            kinematics,
            options
            );
    if (res.empty()) {
        expr = CSL_0;
        diagram = csl::make_shared<wick::Graph>();
        return;
    }
    auto wilsons = model.getWilsonCoefficients(res, options, true);
    diagram = res.getDiagrams()[0].getDiagram();
    std::vector<csl::Expr> expressions = wilsons.obtainExpressions();
    for (auto& ampl : expressions) {
        csl::Transform(ampl, [&](csl::Expr& el)
        {
            if (el->getType() == csl::Type::TensorElement) {
                if (fieldProjected and dirac4.isProjector(el)) {
                // If the field is projected we do not project the vertex also
                    el->setParent(dirac4.getDelta()); 
                    return true;
                }
            }
            return false;
        });
    }
    expr = csl::Factored(csl::sum_s(expressions));
    for (auto fieldParent : nonphysical)
        fieldParent->setPhysical(false);
}

std::vector<QuantumField>& FeynmanRule::getFieldProduct()
{
    return fieldProduct;
}

std::vector<QuantumField> const& FeynmanRule::getFieldProduct() const
{
    return fieldProduct;
}

csl::Expr FeynmanRule::getFieldProduct(
        csl::Tensor const& point,
        std::vector<csl::Tensor>::iterator &first,
        std::vector<csl::Tensor>::iterator last)
{
    HEPAssert(size_t(last-first) == fieldProduct.size(),
            mty::error::ValueError,
            "Not enough witness vertices for rule " + toString(*this)
            + "-> " + toString(size_t(last-first)) + " given.");

    std::vector<csl::Expr> product(getSize() + 1);
    auto iter = product.begin();
    for (auto field = fieldProduct.rbegin(); 
            field != fieldProduct.rend(); 
            ++field) {
        csl::ForEachLeaf(expr, [&](csl::Expr const& el)
        {
            if (IsOfType<QuantumField>(el)
                    and el->getPoint().get() == field->getPoint().get()) {
                el->setPoint(*first);
            }
            else if (el->getType() == csl::Type::TensorElement
                    and el->getParent().get() == field->getPoint().get())
                el->setParent(*first);
        });
        field->setPoint(*first);
        mty::QuantumField other = *field;
        other.setExternal(false);
        other.setPoint(point);
        csl::Expr fieldExpr = other.copy();
        for (auto& index : fieldExpr->getIndexStructureView())
            index = index.getFlipped();
        //fieldExpr = GetComplexConjugate(fieldExpr);
        *iter++ = fieldExpr;
        ++first;
    }
    *iter = expr;
    csl::Expr res = ReplaceXiGauge(csl::prod_s(product));
    // csl::RenameIndices(res);
    return res;
}

FeynmanRule::TermType& FeynmanRule::getInteractionTerm()
{
    return term;
}

FeynmanRule::TermType const& FeynmanRule::getInteractionTerm() const
{
    return term;
}

std::shared_ptr<wick::Graph>& FeynmanRule::getDiagram()
{
    return diagram;
}

std::shared_ptr<wick::Graph> const& FeynmanRule::getDiagram() const
{
    return diagram;
}

csl::Expr FeynmanRule::getExpr() const
{
    return expr;
}

bool FeynmanRule::contains(QuantumFieldParent *parent) const
{
    for (const auto& field : fieldProduct)
        if (field.getParent_info() == parent)
            return true;
    return false;
}

size_t FeynmanRule::count(QuantumFieldParent *parent) const
{
    size_t c = 0;
    for (const auto& field : fieldProduct)
        if (field.getParent_info() == parent)
            ++c;
    return c;
}

void FeynmanRule::setFieldProduct(std::vector<QuantumField> const& t_product)
{
    fieldProduct = t_product;
}

void FeynmanRule::setInteractionTerm(TermType const& t_term)
{
    term = csl::make_shared<InteractionTerm>(*t_term);
}

void FeynmanRule::setDiagram(std::shared_ptr<wick::Graph> const& t_diagram)
{
    diagram = t_diagram;
}

void FeynmanRule::setExpr(csl::Expr const& t_expr)
{
    expr = t_expr;
}

void FeynmanRule::renameIndices()
{
    std::vector<std::pair<csl::Index, csl::Index>> change;
    change.reserve(5 * fieldProduct.size());
    for (auto &field : fieldProduct) {
        for (auto &index : field.getIndexStructureView()) {
            change.push_back({index, index.rename()});
            index = change.back().second;
        }
    }
    for (const auto &[oldIndex, newIndex] : change) {
        csl::Replace(expr, oldIndex, newIndex);
    }
    csl::RenameIndices(expr); // only for dummy indices
}

bool FeynmanRule::isSame(FeynmanRule const& other) const
{
    if (other.fieldProduct.size() != fieldProduct.size())
        return false;

    return std::is_permutation(
            fieldProduct.begin(),
            fieldProduct.end(),
            other.fieldProduct.begin(),
            other.fieldProduct.end(),
            [](QuantumField const& A, QuantumField const& B) 
            {
                return A.getParent().get() == B.getParent().get()
                    and (A.isComplexConjugate() == B.isComplexConjugate()
                            or A.isSelfConjugate());
            });
}

bool FeynmanRule::isZero() const
{
    return expr == CSL_0;
}

bool FeynmanRule::isEmpty() const
{
    return fieldProduct.empty();
}

size_t FeynmanRule::getSize() const
{
    return fieldProduct.size();
}

bool FeynmanRule::operator==(FeynmanRule const& other) const
{
    return isSame(other) and expr == other.expr;
}

bool FeynmanRule::operator!=(FeynmanRule const& other) const
{
    return not (*this == other);
}

bool FeynmanRule::operator<(FeynmanRule const& other) const
{
    if (fieldProduct.size() != other.fieldProduct.size())
        return fieldProduct.size() < other.fieldProduct.size();

    for (size_t i = 0; i != fieldProduct.size(); ++i) {
        const size_t n = i;
        if (fieldProduct[n] < other.fieldProduct[n])
            return true;
        else if (other.fieldProduct[n] < fieldProduct[n])
            return false;
    }
    return false;
}

bool FeynmanRule::operator>(FeynmanRule const& other) const
{
    return other < *this;
}

bool FeynmanRule::operator<=(FeynmanRule const& other) const
{
    return !(other < *this);
}

bool FeynmanRule::operator>=(FeynmanRule const& other) const
{
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& out,
                         FeynmanRule const& rule)
{
    out << "Rule for ";
    for (const auto& f : rule.fieldProduct) {
        f.getConjugatedField().print(1, out);
        out << " ";
    }
    out << "  ==>  ";
    out << rule.expr << " \n\t = ";
    out << csl::Evaluated(rule.expr, csl::eval::abbreviation) <<'\n';

    return out;
}

}
