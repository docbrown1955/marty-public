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

#include "effectiveTheory.h"
#include "interactionTerm.h"
#include "polarization.h"
#include "mrtOptions.h"
#include "feynmanRule.h"
#include "diracology.h"
#include "wick.h"
#include "graph.h"
#include "amplitude.h"

using namespace csl;

namespace mty {


///////////////////////////////////////////////////
/*************************************************/
// Class EffOp                                   //
/*************************************************/
///////////////////////////////////////////////////

EffOp::EffOp(mty::Model*          t_model,
             mty::Particle const& t_particle,
             size_t               t_maxDim)
    :model(t_model),
    particle(t_particle),
    maxDimension(t_maxDim)
{

}

std::optional<EffOp> EffOp::addTerm(
        mty::FeynmanRule const& term,
        size_t                  posTerm)
{
    size_t nOccurrences = term.count(particle.get());
    csl::Expr newDim = dimension 
        + static_cast<int>(nOccurrences) * 
            (4 - 2 * particle->getEnergyDimension()) / 2;

    if (newDim->evaluateScalar() > maxDimension)
        return std::nullopt;

    EffOp newOp(*this);
    newOp.dimension    = newDim;
    newOp.termsInOp.push_back(posTerm);
    ++newOp.nVertices;

    return newOp;
}

std::vector<EffOp> EffOp::applyExternal(const size_t maxLoop) const
{
    if (empty())
        return std::vector<EffOp>();
    std::vector<mty::QuantumField> integratedOut;
    std::vector<mty::QuantumField> otherFields;

    for (size_t place : termsInOp) {
        auto& content = model->getFeynmanRule(place).getFieldProduct();
        csl::Tensor X = csl::tensor_s("X",
                                       &csl::Minkowski);
        for (const auto& field : content)
            if (field.getParent_info() == particle.get()) {
                integratedOut.push_back(field);
                integratedOut.back().setPoint(X);
                integratedOut.back().setExternal(false);
            }
            else
                otherFields.push_back(field);
    }

    if (wick::WickCalculator::isContractionZero(integratedOut)) {
        // No way to contract the integrated out particles in a single
        // connected graph -> will not give any operator
        return std::vector<EffOp>();
    }

    std::vector<std::vector<mty::QuantumField>> external(
            1, std::vector<mty::QuantumField>(otherFields.size()));
    for (size_t i = 0; i != external[0].size(); ++i) {
        csl::Tensor X = csl::tensor_s("X_" + toString(i),
                                       &csl::Minkowski);
        external[0][i] = ConvertTo<mty::QuantumField>(
                GetComplexConjugate(otherFields[i].copy()));
        external[0][i].setDerivativeStructure(csl::IndexStructure());
        external[0][i].setExternal(true);
        external[0][i].setPoint(X);
        external[0][i].setConjugated(!external[0][i].getConjugated());
    }

    std::vector<EffOp> result;
    int nLoops = computeNLoops(integratedOut.size(), termsInOp.size());
    if (nLoops > static_cast<int>(maxLoop))
        return result;
    do {
        if (nLoops >= 0) {
            for (const auto& ext : external) {
                result.push_back(*this);
                result.back().externalLegs = ext;
                result.back().nLoops       = nLoops;
            }
        }
        updateExternal(external);
        nLoops += 1;
    } while (nLoops <= static_cast<int>(maxLoop));

    return result;
}

csl::vector_expr EffOp::applyDerivatives() const
{
    std::vector<FeynmanRule> terms(termsInOp.size());
    for (size_t i = 0; i != terms.size(); ++i)
        terms[i] = model->getFeynmanRule(termsInOp[i]);

    std::vector<csl::Tensor> momentum(externalLegs.size());
    for (size_t i = 0; i != externalLegs.size(); ++i)
        momentum[i] = csl::tensor_s("P_" + toString(i),
                                    &csl::Minkowski);

    particle->integrateOut();

    csl::ScopedProperty p1(&mty::option::applyMomentumConservation, false);
    csl::ScopedProperty p2(&mty::option::searchAbreviations, false);
    mty::AmplitudeCalculator amplitude(externalLegs,
                             terms,
                             momentum);
    csl::Expr res = amplitude.getResult();
    csl::Expand(res, true);
    replaceMomentumAndExternalLegs(res);

    if (res->getType() == csl::Type::Sum)
        return res->getVectorArgument();
    return csl::vector_expr(1, res);
}

bool EffOp::empty() const
{
    return termsInOp.empty();
}

bool EffOp::isZero() const
{
    size_t count = 0;
    size_t count_conjugate = 0;
    for (size_t i : termsInOp) {
        auto& content = model->getFeynmanRule(i).getFieldProduct();
        for (const auto& field : content)
            if (field.getQuantumParent() == particle.get()) {
                ++count;
                if (field.isComplexConjugate())
                    ++count_conjugate;
            }
    }

    return (count % 2 == 1) or (count != 2 * count_conjugate);
}

csl::Expr EffOp::getDimension() const
{
    csl::Expr dim = CSL_0;
    for (const auto& field : externalLegs)
        dim = dim + field.getEnergyDimension();

    return dim;
}

bool EffOp::operator==(EffOp const& other) const
{
    if (nVertices == other.nVertices)
        return std::is_permutation(termsInOp.begin(),
                                   termsInOp.end(),
                                   other.termsInOp.begin());
    return false;
}

int EffOp::computeNLoops(size_t nIntegrated,
                         size_t nVertices)
{
    return 1 - static_cast<int>(nVertices) + nIntegrated / 2;
}

std::vector<mty::QuantumField> EffOp::getContractibleFields(
        std::vector<mty::QuantumField> const& ext)
{
    std::vector<mty::QuantumField> contractible(ext);
    for (size_t i = 0; i < contractible.size(); ++i) {
        int partner = -1;
        for (size_t j = i+1; j < contractible.size(); ++j) {
            if (contractible[i].isExactlyContractiblewith(
                        contractible[j])) {
                partner = j;
                break;
            }
        }
        if (partner == -1) {
            contractible.erase(contractible.begin() + i);
            --i;
        }
        else {
            contractible.erase(contractible.begin() + partner);
        }
    }

    return contractible;
}

void EffOp::removeContractiblePair(
        std::vector<mty::QuantumField> & fields,
        mty::QuantumField         const& toRemove)
{
    bool regularRemoved    = false;
    bool conjugatedRemoved = false;
    for (size_t i = 0; i != fields.size(); ++i) {
        bool removed = false;
        if (not conjugatedRemoved
                and toRemove.isExactlyContractiblewith(fields[i])) {
            fields.erase(fields.begin() + i);
            conjugatedRemoved = true;
            removed = true;
        }
        if (removed and i == fields.size())
            break;
        if (not regularRemoved
                and toRemove.isContractibleWith(fields[i])
                and toRemove.isComplexConjugate() 
                == fields[i].isComplexConjugate()) {
            fields.erase(fields.begin() + i);
            regularRemoved = true;
            removed = true;
        }
        if (removed) {
            if (regularRemoved and conjugatedRemoved)
                break;
            --i;
        }
    }
    HEPAssert(regularRemoved and conjugatedRemoved,
            mty::error::RuntimeError,
            "Did not find fields to remove in EffOp::removeContractiblePair()");
}

void EffOp::updateExternal(
        std::vector<std::vector<mty::QuantumField>>& external)
{
    std::vector<std::vector<mty::QuantumField>> newExternal;
    for (const auto& ext : external) {
        if (ext.empty())
            continue;
        std::vector<mty::QuantumField> contractible 
            = getContractibleFields(ext);
        for (const auto& field : contractible) {
            newExternal.push_back(ext);
            removeContractiblePair(newExternal.back(), field);
        }
    }
    external = std::move(newExternal);
}

void EffOp::clearAmplitude(csl::Expr& amplitude)
{
    if (amplitude->getType() == csl::Type::Sum) {
        for (auto& term : *amplitude)
            clearAmplitude(term);
        amplitude = Refreshed(amplitude);
        return;
    }
    auto keepForMomentumSum = 
        [](csl::Expr const& expr) {
            if (expr->getPrimaryType() == csl::PrimaryType::Numerical
                    or expr->getType() == csl::Type::Imaginary)
                return true;
            if (expr->getType() == csl::Type::TensorElement
                    and expr->getName()[0] == 'P')
                return true;
            if (expr->getType() != csl::Type::Sum)
                return false;
            for (const auto& el : *expr)
                if (el->getType() != csl::Type::TensorElement
                        or el->getName()[0] != 'P')
                    return false;
            return true;
        };

    csl::vector_expr momentumSums;
    if (amplitude->getType() == csl::Type::Prod) {
        for (const auto& term : *amplitude)
            if (keepForMomentumSum(term))
                momentumSums.push_back(term);
    }
    else if (keepForMomentumSum(amplitude))
        momentumSums.push_back(amplitude);

    amplitude = prod_s(momentumSums);
}

int EffOp::getMomentumNumber(csl::Expr const& expr) const
{
    if (expr->getType() == csl::Type::TensorElement
            and expr->getIndexStructureView().size() == 1
            and expr->getIndexStructureView()[0].getSpace()
            == &csl::Minkowski) {
        auto name = std::string(expr->getName());
        if (name[0] == 'P' and name[1] == '_') {
            name.erase(0, 2);
            return atoi(name.c_str());
        }
    }
    return -1;
}

void EffOp::replaceMomentumAndExternalLegs(
        csl::Expr& amplitude) const
{
    
    if (amplitude->getType() == csl::Type::Sum)
        for (auto& term : *amplitude)
            replaceMomentumAndExternalLegs(term);
    else if (amplitude->getType() == csl::Type::Prod) {
        csl::Expr factor = CSL_1;
        std::map<int, csl::vector_expr> derivatives;
        for (auto& term : *amplitude)
            if (int n = getMomentumNumber(term); n >= 0) {
                if (externalLegs[n].isIncoming())
                    factor = factor * CSL_I;
                else
                    factor = factor * -CSL_I;
                csl::Expr derivative = mty::partialMinko(
                        term->getIndexStructureView()[0],
                        model->getLagrangian().getPoint());
                derivatives[n].push_back(derivative);
                term = CSL_1;
            }
        replaceExternalLegs(amplitude, derivatives);
        amplitude = factor * amplitude;
    }
    amplitude = Refreshed(amplitude);
}

void EffOp::replaceExternalLegs(
        csl::Expr                           & amplitude,
        std::map<int, csl::vector_expr>& derivatives) const
{
    csl::Tensor X = model->getLagrangian().getPoint();
    for (auto& expr : *amplitude) {
        if (IsOfType<mty::PolarizationField>(expr)) {
            auto& polar = ConvertTo<mty::PolarizationField>(expr);
            auto index = polar.getIndexStructure();
            if (polar.getQuantumParent()->getSpinDimension() != 1)
                index.erase(index.begin());
            csl::Expr newExpr = (polar.getParent())(index.getIndex(), X);
            if (polar.isIncomingAntiParticle()
                    or polar.isOutgoingParticle())
                newExpr = GetComplexConjugate(newExpr);
            int n = getMomentumNumber((*polar.getPoint())
                    (csl::Minkowski.generateIndex()));
            if (derivatives.find(n) != derivatives.end())
                for (const auto& d : derivatives[n])
                    newExpr = d * newExpr;
            expr = newExpr;
        }
    }
}

std::ostream& operator<<(std::ostream& fout,
                         EffOp const&  op)
{
    fout << "Operator of dimension " << op.dimension << " (max = ";
    fout << op.maxDimension << "), " << op.nLoops << " loops:\n";
    for (const auto& ext : op.externalLegs)
        ext.print(1);
    for (size_t i : op.termsInOp) {
        auto content = op.model->getFeynmanRule(i).getFieldProduct();
        for (const auto& field : content)
            field.print(1);
    }
    fout << std::endl;

    return fout;
}


///////////////////////////////////////////////////
/*************************************************/
// Class EffectiveTheory                         //
/*************************************************/
///////////////////////////////////////////////////

void EffectiveTheory::integrateOutParticle(
        mty::Model&    t_model,
        mty::Particle& t_particle,
        size_t         t_maxDim,
        size_t         t_nLoops,
        size_t         t_maxDimOperator)
{
    clear();
    model    = &t_model;
    particle = t_particle;
    maxDim   = t_maxDim;
    nLoops   = t_nLoops;
    maxDimOperator = t_maxDimOperator;
    integrateOutParticle();
}

void EffectiveTheory::clear()
{
    placeInLagrangian.clear();
}

void EffectiveTheory::integrateOutParticle()
{
    findOperators();
    applyExternalLegs();
    if (maxDimOperator != 0)
        cutHigherDimensionalOperators();

    for (const auto& op : operators)
        std::cout << op << std::endl;
    std::cout << "Total: " << operators.size() << " operators.\n";

    auto newTerms = applyDerivatives();
    model->integrateOutParticle(particle,
                                newTerms);
}

void EffectiveTheory::findOperators()
{
    csl::Expr field1 = particle->getInstance();
    csl::Expr field2 = GetComplexConjugate(particle->getInstance());

    for (size_t i = 0; i != model->getFeynmanRulesNumber(); ++i) {
        if (model->getFeynmanRule(i).contains(particle.get())
                or (particle->hasFieldStrength()
                    and model->getFeynmanRule(i).contains(
                        particle->getFieldStrength().get()))) {
            placeInLagrangian.push_back(i);
        }
    }

    operators = std::vector<EffOp>(1, EffOp(model, particle, maxDim));

    size_t beginNewOps = 0;
    size_t endNewOps   = 1;
    while (beginNewOps < endNewOps) {
        std::vector<EffOp> newOps;
        newOps.reserve(operators.size());
        for (size_t i = beginNewOps; i != endNewOps; ++i) {
            for (size_t place : placeInLagrangian) {
                FeynmanRule& t = model->getFeynmanRule(place);
                std::optional<EffOp> op = operators[i].addTerm(
                        t, place);
                if (op) {
                    if (find(newOps.begin(), newOps.end(), op.value())
                            == newOps.end()) {
                        newOps.push_back(op.value());
                    }
                }
            }
        }
        beginNewOps = operators.size();
        endNewOps   = operators.size() + newOps.size();
        operators.insert(operators.end(), newOps.begin(), newOps.end());
    }
    for (size_t i = 0; i != operators.size(); ++i)
        if (operators[i].isZero()) {
            operators.erase(operators.begin() + i);
            --i;
        }
}

void EffectiveTheory::applyExternalLegs()
{
    std::vector<EffOp> newOps;
    newOps.reserve(operators.size());
    for (const auto& op : operators) {
        auto newOperators = op.applyExternal(nLoops);
        newOps.insert(newOps.end(), 
                      newOperators.begin(),
                      newOperators.end());
    }
    operators = std::move(newOps);
}

void EffectiveTheory::cutHigherDimensionalOperators()
{
    size_t nCut = 0;
    for (size_t i = 0; i != operators.size(); ++i)
        if (operators[i].getDimension()->evaluateScalar() > maxDimOperator) {
            ++nCut;
        }
    std::vector<EffOp> newOp;
    newOp.reserve(operators.size() - nCut);
    for (auto& op : operators)
        if (op.getDimension()->evaluateScalar() <= maxDimOperator)
            newOp.push_back(std::move(op));
    operators = std::move(newOp);
}

csl::vector_expr EffectiveTheory::applyDerivatives()
{
    csl::vector_expr newOps;
    newOps.reserve(operators.size());
    for (const auto& op : operators) {
        auto newOperators = op.applyDerivatives();
        newOps.insert(newOps.end(), 
                      newOperators.begin(),
                      newOperators.end());
    }
    return newOps;
}

csl::Expr EffectiveTheory::getDimension(csl::Expr const& fieldExpression)
{
    csl::Expr total = CSL_0;
    csl::VisitEachLeaf(
            fieldExpression,
            [&total](const csl::Expr& element) {
              if (IsIndicialTensor(element)
                      and IsOfType<QuantumField>(element))
                total = total + ConvertTo<QuantumField>(element)
                                    .getEnergyDimension();
            });

    return total;
}

} // End of namespace mty
