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

#include "model.h"
#include "amplitude.h"
#include "diracology.h"
#include "expander.h"
#include "fermionOrder.h"
#include "ghostField.h"
#include "goldstoneField.h"
#include "graph.h"
#include "insertion.h"
#include "interactionTerm.h"
#include "jsonToPhysics.h"
#include "mrtError.h"
#include "mrtInterface.h"
#include "mrtOptions.h"
#include "mrtUtils.h"
#include "penguinpatch.h"
#include "polarization.h"
#include "quantumFieldTheory.h"
#include "wilson.h"
#include <numeric>

#include "amplitudeInitializer.h"
#include "amplitudeSimplification.h"
#include "feynOptions.h"
#include "kinematics.h"

namespace mty {

size_t Model::getFeynmanRulesNumber() const
{
    return feynmanRules.size();
}

mty::FeynmanRule &Model::getFeynmanRule(size_t i)
{
    HEPAssert(i < feynmanRules.size(),
              mty::error::IndexError,
              "Index " + toString(i) + " out of bounds for "
                  + toString(feynmanRules.size()) + " feynman rules.");
    return feynmanRules[i];
}

mty::FeynmanRule const &Model::getFeynmanRule(size_t i) const
{
    HEPAssert(i < feynmanRules.size(),
              mty::error::IndexError,
              "Index " + toString(i) + " out of bounds for "
                  + toString(feynmanRules.size()) + " feynman rules.");
    return feynmanRules[i];
}

std::vector<mty::QuantumField>
Model::recoverQuantumInsertions(std::vector<csl::Expr> const &expr)
{
    std::vector<mty::QuantumField> fields(expr.size());
    auto                           getField = [&](csl::Expr const &expr) {
        mty::QuantumField field;
        csl::Expr         copy = expr;
        csl::FirstOfLeaf(copy, [&](csl::Expr const &sub) {
            if (IsOfType<mty::QuantumField>(sub)) {
                field = ConvertTo<mty::QuantumField>(sub);
                return true;
            }
            return false;
        });
        return field;
    };

    for (size_t i = 0; i != expr.size(); ++i)
        fields[i] = getField(expr[i]);
    return fields;
}

mty::Amplitude Model::computeAmplitude(int                         order,
                                       std::vector<mty::Insertion> insertions)
{
    FeynOptions options;
    return computeAmplitude(order, insertions, options);
}

mty::Amplitude Model::computeAmplitude(int                         order,
                                       std::vector<mty::Insertion> insertions,
                                       Kinematics const &          kinematics)
{
    FeynOptions options;
    return computeAmplitude(order, insertions, kinematics, options);
}

mty::Amplitude Model::computeAmplitude(int                         order,
                                       std::vector<mty::Insertion> insertions,
                                       FeynOptions &               options)
{
    Kinematics kinematics{insertions};
    auto       res = computeAmplitude(order, insertions, kinematics, options);
    options        = res.getOptions();
    return res;
}

mty::Amplitude Model::computeAmplitude(int                         order,
                                       std::vector<mty::Insertion> insertions,
                                       Kinematics const &          kinematics,
                                       FeynOptions &               options)
{
    if (!options.getFeynRuleCalculation())
        options.setLoopOrder(order, static_cast<int>(insertions.size()));
    if (options.getFeynRuleCalculation()) {
        std::vector<Lagrangian::TermType> lagrangian = L.interaction;
        options.applyFilters(lagrangian);
        auto res
            = computeAmplitude(lagrangian, insertions, kinematics, options);
        options = res.getOptions();
        return res;
    }
    else {
        std::vector<FeynmanRule> const &rules = getFeynmanRules();
        auto filteredRules                    = options.applyFilters(rules);
        auto res
            = computeAmplitude(filteredRules, insertions, kinematics, options);
        options = res.getOptions();
        return res;
    }
}

mty::Amplitude
Model::computeAmplitude(std::vector<Lagrangian::TermType> &lagrangian,
                        std::vector<mty::Insertion>        insertions,
                        Kinematics const &                 kinematics,
                        FeynOptions                        options,
                        std::vector<FeynmanRule const *>   rules)
{
    std::vector<int> fermionOrder = options.getFermionOrder();
    if (fermionOrder.empty())
        fermionOrder = defaultFermionOrder(insertions);
    options.setFermionOrder(fermionOrder);
    if (options.orderExternalFermions)
        applyFermionOrder(insertions, fermionOrder);
    auto quantumInsertions
        = recoverQuantumInsertions(GetExpression(insertions));
    size_t i = 1;
    for (auto &field : quantumInsertions) {
        csl::Tensor Xi = csl::tensor_s("X_" + toString(i++), &csl::Minkowski);
        field.setPoint(Xi);
    }
    std::vector<std::vector<size_t>> terms
        = Expander::getDiagrams(this, options, lagrangian, quantumInsertions);
    if (rules.empty()) {
        return AmplitudeInitializer::getAmplitude(
            terms, this, options, kinematics, quantumInsertions, lagrangian);
    }
    else {
        return AmplitudeInitializer::getAmplitude(
            terms, this, options, kinematics, quantumInsertions, rules);
    }
}

mty::Amplitude
Model::computeAmplitude(std::vector<FeynmanRule const *> &feynRules,
                        std::vector<Insertion> const &    insertions,
                        Kinematics const &                kinematics,
                        FeynOptions const &               options)
{
    auto quantumInsertions
        = recoverQuantumInsertions(GetExpression(insertions));
    std::vector<Lagrangian::TermType> lagrangian(feynRules.size());
    for (size_t i = 0; i != lagrangian.size(); ++i)
        lagrangian[i] = feynRules[i]->getInteractionTerm();
    return computeAmplitude(
        lagrangian, insertions, kinematics, options, feynRules);
}

mty::Amplitude Model::computePartialAmplitude(
    int order, std::vector<mty::Insertion> insertions, FeynOptions options)
{
    options.partialCalculation = true;
    return computeAmplitude(order, insertions, options);
}

mty::Amplitude
Model::computePartialAmplitude(int                         order,
                               std::vector<mty::Insertion> insertions,
                               Kinematics const &          kinematics,
                               FeynOptions                 options)
{
    options.partialCalculation = true;
    return computeAmplitude(order, insertions, kinematics, options);
}

csl::Expr Model::computeSquaredAmplitude(Amplitude const &ampl,
                                         bool applyDegreesOfFreedomFactor)
{
    return computeSquaredAmplitude(ampl, ampl, applyDegreesOfFreedomFactor);
}

csl::Expr Model::computeSquaredAmplitude(Amplitude const &amplL,
                                         Amplitude const &amplR,
                                         bool applyDegreesOfFreedomFactor)
{
    csl::ScopedProperty prop(&mty::option::decomposeInLocalOperator, false);

    FeynOptions optionsL = amplL.getOptions();
    // optionsL.setWilsonOperatorBasis(OperatorBasis::None);
    auto wilsonsL
        = getWilsonCoefficients(amplL, optionsL, DecompositionMode::Minimal);

    if (&amplL == &amplR) { // same ampl, no need to recalculate
        return computeSquaredAmplitude(
            wilsonsL, wilsonsL, applyDegreesOfFreedomFactor);
    }
    Amplitude amplR_copy = amplR;
    amplR_copy.setKinematics(amplL.getKinematics());
    FeynOptions optionsR = amplR_copy.getOptions();
    // optionsR.setWilsonOperatorBasis(OperatorBasis::None);
    auto wilsonsR = getWilsonCoefficients(
        amplR_copy, optionsR, DecompositionMode::Minimal);

    return computeSquaredAmplitude(
        wilsonsL, wilsonsR, applyDegreesOfFreedomFactor);
}

static std::vector<csl::Expr> evalForSquare(WilsonSet const &wilsons)
{
    csl::Abbrev::enableGenericEvaluation("EXT");
    csl::Abbrev::enableGenericEvaluation("Fc");
    csl::Abbrev::enableGenericEvaluation("Color");
    csl::Abbrev::enableGenericEvaluation("P");
    std::vector<csl::Expr> eval(wilsons.size());
    for (size_t i = 0; i != wilsons.size(); ++i) {
        eval[i] = csl::Evaluated(wilsons[i].op.getOp());
    }
    csl::Abbrev::disableGenericEvaluation("EXT");
    csl::Abbrev::disableGenericEvaluation("Fc");
    csl::Abbrev::disableGenericEvaluation("Color");
    csl::Abbrev::disableGenericEvaluation("P");

    return eval;
}

csl::Expr Model::computeSquaredAmplitude(WilsonSet const &ampl,
                                         bool applyDegreesOfFreedomFactor)
{
    return computeSquaredAmplitude(ampl, ampl, applyDegreesOfFreedomFactor);
}

csl::Expr Model::computeSquaredAmplitude(WilsonSet const &amplL,
                                         WilsonSet const &amplR,
                                         bool applyDegreesOfFreedomFactor)
{
    csl::ScopedProperty      commut(&csl::option::checkCommutations, false);
    HEPAssert(!option::decomposeInOperators
                  or !option::decomposeInLocalOperator,
              mty::error::ValueError,
              "Cannot (yet) calculate a cross-section with an amplitude "
              "that has been decomposed in operators. Please set mty::option::"
              "decomposeInOperators or mty::option::decomposeInLocalOperator "
              "to "
              "false.") auto insertions
        = amplL.kinematics.getInsertions();
    auto momenta = amplL.kinematics.getMomenta();
    if (amplL.empty() || amplR.empty())
        return CSL_0;
    const bool             areSame = (&amplL == &amplR);
    std::vector<csl::Expr> evalL   = evalForSquare(amplL);
    std::vector<csl::Expr> evalR   = (areSame) ? evalL : evalForSquare(amplR);
    csl::Expr              incomingDof = CSL_1;
    if (applyDegreesOfFreedomFactor) {
        incomingDof = amplL.kinematics.getDegreesOfFreedomFactor();
    }
    csl::vector_expr res;
    size_t           index = 0;
    std::cout << "Squaring amplitude ..." << std::endl;
    csl::ProgressBar bar(amplL.size() * (amplR.size() + 1) / 2);
    for (size_t i = 0; i != amplR.size(); ++i) {
        // If same start at i and add complex conjugate
        for (size_t j = areSame * i; j != amplL.size(); ++j) {
            if (mty::option::verboseAmplitude)
                bar.progress(index++);
            csl::Expr renamed = csl::Factored(csl::DeepCopy(evalL[j]));
            csl::RenameIndices(renamed);
            csl::Expr prod = renamed
                             * csl::GetHermitianConjugate(evalR[i], &dirac4)
                             / incomingDof;
            auto predic = [&](csl::Expr const &sub) {
                return IsOfType<PolarizationField>(sub);
            };
            csl::DeepExpandIf_lock(prod, predic);
            simpli::simplify(
                prod,
                recoverQuantumInsertions(GetExpression(insertions)),
                momenta,
                amplL.options,
                simpli::SquaredAmplitude);
            csl::Expr factor = csl::Abbrev::makeAbbreviation(
                csl::GetComplexConjugate(amplR[i].op.getFactor())
                * amplL[j].op.getFactor());
            prod = csl::prod_s(
                {prod,
                 factor,
                 csl::GetComplexConjugate(amplR[i].coef.getCoefficient()),
                 amplL[j].coef.getCoefficient()});
            csl::Replace(prod, csl::DMinko, 4);
            res.push_back(prod);
            if (areSame && i != j) {
                res.push_back(csl::GetHermitianConjugate(prod, &dirac4));
            }
        }
    }
    auto squared = csl::Factored(csl::sum_s(res));
    csl::matcher::compress(squared, 2);
    return squared;
}

static std::vector<std::vector<mty::Insertion>>
getIndependentDecays(std::vector<mty::Insertion> const &insertions)
{
    HEPAssert(insertions.size() == 3,
              mty::error::TypeError,
              "This function should only be used for 1->2 processes, "
                  + std::to_string(insertions.size()) + " insertions given.")
        std::vector<std::vector<mty::Insertion>>
            res;
    res.reserve(4);
    res.push_back(insertions);
    if (!insertions[1].getField()->isSelfConjugate()) {
        res.push_back({insertions[0], AntiPart(insertions[1]), insertions[2]});
        if (!insertions[2].getField()->isSelfConjugate()) {
            if (insertions[1].getField() != insertions[2].getField())
                res.push_back(
                    {insertions[0], insertions[1], AntiPart(insertions[2])});
            res.push_back({insertions[0],
                           AntiPart(insertions[1]),
                           AntiPart(insertions[2])});
        }
    }
    else if (!insertions[2].getField()->isSelfConjugate()) {
        res.push_back({insertions[0], insertions[1], AntiPart(insertions[2])});
    }
    return res;
}

csl::Expr
Model::squaredAmplitudeToPartialWidth(csl::Expr const &squaredAmplitude,
                                      std::vector<Insertion> const &insertions,
                                      mty::Amplitude const &initialAmplitude,
                                      bool                  applyMassCondition)
{
    csl::Expr  squared  = csl::DeepCopy(squaredAmplitude);
    auto       pi       = initialAmplitude.getKinematics().getOrderedMomenta();
    csl::Index mu       = MinkowskiIndex();
    csl::Expr  s12_init = pi[0](mu) * pi[1](+mu);
    csl::Expr  s13_init = pi[0](mu) * pi[2](+mu);
    csl::Expr  s23_init = pi[1](mu) * pi[2](+mu);
    csl::Expr  M        = insertions[0].getField()->getMass();
    csl::Expr  m1       = insertions[1].getField()->getMass();
    csl::Expr  m2       = insertions[2].getField()->getMass();
    csl::Expr  E1       = (M * M + m1 * m1 - m2 * m2) / (2 * M);
    csl::Expr  E2       = (M * M + m2 * m2 - m1 * m1) / (2 * M);
    csl::Expr  p2       = E1 * E1 - m1 * m1;
    csl::Expr  s12      = csl::Abbrev::makeAbbreviation(M * E1);
    csl::Expr  s13      = csl::Abbrev::makeAbbreviation(M * E2);
    csl::Expr  s23      = csl::Abbrev::makeAbbreviation(E1 * E2 + p2);
    csl::Replace(squared, s12_init, s12);
    csl::Replace(squared, s13_init, s13);
    csl::Replace(squared, s23_init, s23);
    squared *= csl::sqrt_s(csl::abs_s(p2)) / (8 * CSL_PI * M * M);
    if (applyMassCondition) {
        csl::Expr final = csl::booleanOperator_s(
            csl::BooleanOperator::GreaterThanOrEqualTo,
            M,
            m1 + m2,
            squared,
            CSL_0);
        return final;
    }

    return squared;
}

csl::Expr Model::computeWidth(int                   orderLeft,
                              int                   orderRight,
                              mty::Insertion const &particle,
                              mty::FeynOptions      options)
{
    if (orderRight < orderLeft)
        return computeWidth(orderRight, orderLeft, particle, options);
    options.verboseAmplitude = false;
    if (particle.getField()->getMass() == CSL_0) {
        // No width for massless particles
        return CSL_0;
    }
    csl::ScopedProperty    silent(&mty::option::verboseAmplitude, false);
    std::vector<csl::Expr> contributions;
    contributions.reserve(100);
    const auto &physicalParticles
        = getPhysicalParticles([](Particle const &p) {
              return !IsOfType<GoldstoneBoson>(p) && !IsOfType<GhostBoson>(p);
          });
    for (size_t i = 0; i != physicalParticles.size(); ++i) {
        const auto &p1 = physicalParticles[i];
        for (size_t j = i; j != physicalParticles.size(); ++j) {
            const auto &p2         = physicalParticles[j];
            auto        insertions = getIndependentDecays(
                {Incoming(particle), Outgoing(p1), Outgoing(p2)});
            for (const auto &ins : insertions) {
                mty::Amplitude amplL
                    = computeAmplitude(orderLeft, ins, options);
                if (amplL.empty())
                    continue;
                csl::Expr squared;
                if (orderRight != orderLeft) {
                    auto amplR = computeAmplitude(orderRight, ins, options);
                    squared    = computeSquaredAmplitude(amplL, amplR);
                }
                else {
                    squared = computeSquaredAmplitude(amplL);
                }
                if (squared != 0) {
                    std::cout << "Found decay ";
                    std::cout << ins[0].getField()->getName() << " -> ";
                    std::cout << ins[1].getField()->getName() << " + ";
                    std::cout << ins[2].getField()->getName() << std::endl;
                    contributions.push_back(
                        squaredAmplitudeToPartialWidth(squared, ins, amplL));
                }
            }
        }
    }
    std::cout << contributions.size() << " independent "
              << particle.getField()->getName() << " decays found.\n";
    return csl::sum_s(contributions);
}
csl::Expr Model::computeWidth(int                   order,
                              mty::Insertion const &particle,
                              mty::FeynOptions      options)
{
    return computeWidth(order, order, particle, options);
}

void Model::computeModelWidths(int                                orderLeft,
                               int                                orderRight,
                               std::vector<mty::Insertion> const &insertions,
                               mty::FeynOptions                   options)
{
    options.discardLowerOrders = false;
    for (const auto p : insertions) {
        auto width = computeWidth(orderLeft, orderRight, p, options);
        if (width != CSL_0) {
            std::string name      = "Gamma_" + p.getField()->getName();
            csl::Expr abbreviated = csl::Abbrev::makeAbbreviation(name, width);
            addAbbreviatedMassExpression(abbreviated);
            p.getField()->setWidth(csl::constant_s(name));
        }
    }
}

void Model::computeModelWidths(int              orderLeft,
                               int              orderRight,
                               mty::FeynOptions options)
{
    auto                   particles = getPhysicalParticles([&](Particle p) {
        return !IsOfType<GhostBoson>(p) && !IsOfType<GoldstoneBoson>(p);
    });
    std::vector<Insertion> insertions;
    insertions.reserve(particles.size());
    for (size_t i = 0; i != particles.size(); ++i) {
        insertions.push_back(particles[i]);
    }
    computeModelWidths(orderLeft, orderRight, insertions, options);
}

void Model::computeModelWidths(int                                order,
                               std::vector<mty::Insertion> const &particles,
                               mty::FeynOptions                   options)
{
    computeModelWidths(order, order, particles, options);
}

void Model::computeModelWidths(int order, mty::FeynOptions options)
{
    computeModelWidths(order, order, options);
}

void Model::projectOnBasis(csl::Expr &expr, OperatorBasis basis)
{
    auto a = DiracIndices(2);
    if (basis == OperatorBasis::Chiral) {
        csl::Replace(expr,
                     dirac4.gamma_chir({a[0], a[1]}),
                     dirac4.P_R({a[0], a[1]}) - dirac4.P_L({a[0], a[1]}));
    }
    else if (basis == OperatorBasis::Standard) {
        csl::Replace(expr,
                     dirac4.P_L({a[0], a[1]}),
                     CSL_HALF * dirac4.getDelta()({a[0], a[1]})
                         - CSL_HALF * dirac4.gamma_chir({a[0], a[1]}));
        csl::Replace(expr,
                     dirac4.P_R({a[0], a[1]}),
                     CSL_HALF * dirac4.getDelta()({a[0], a[1]})
                         + CSL_HALF * dirac4.gamma_chir({a[0], a[1]}));
    }
}

WilsonSet Model::getWilsonCoefficients(Amplitude const & ampl,
                                       DecompositionMode mode)
{
    return getWilsonCoefficients(ampl, ampl.getOptions(), mode);
}

int operatorDegeneracy(std::vector<mty::Insertion> const &insertions)
{
    std::vector<size_t> indicesLeft(insertions.size());
    std::iota(indicesLeft.begin(), indicesLeft.end(), 0);
    int factor = 1;
    while (!indicesLeft.empty()) {
        int nFields = 1;
        for (size_t i = 1; i != indicesLeft.size(); ++i) {
            if (insertions[indicesLeft[0]].isEquivalent(
                    insertions[indicesLeft[i]])) {
                indicesLeft.erase(indicesLeft.begin() + i);
                ++nFields;
                --i;
            }
        }
        indicesLeft.erase(indicesLeft.begin());
        factor *= nFields;
    }
    return factor;
}

static int nPermutations(std::vector<int> &fermionOrder)
{
    for (size_t i = 0; i + 1 < fermionOrder.size(); ++i) {
        if (fermionOrder[i + 1] < fermionOrder[i]) {
            std::swap(fermionOrder[i], fermionOrder[i + 1]);
            return 1 + nPermutations(fermionOrder);
        }
    }
    return 0;
}

int matchingFermionSign(std::vector<int> fermionOrder)
{
    return (nPermutations(fermionOrder) & 1) ? -1 : 1;
}

WilsonSet Model::getWilsonCoefficients(Amplitude const &  ampl,
                                       FeynOptions const &feynOptions,
                                       DecompositionMode  mode)
{
    mty::option::displayAbbreviations = false;
    mty::cachedWilson.clear();
    csl::ScopedProperty    commut(&csl::option::checkCommutations, false);
    std::vector<csl::Expr> amplitudesfull(ampl.obtainExpressions());
    const auto             basis  = OperatorBasis::Standard;
    const auto             factor = feynOptions.getWilsonOperatorCoefficient();
    csl::Abbrev::enableGenericEvaluation("EXT");
    bool isMinimal = (mode == DecompositionMode::Minimal);
    for (auto &ampl : amplitudesfull) {
        ampl = csl::DeepCopy(ampl);
        if (mode == DecompositionMode::Matching) {
            ampl = CSL_I * ampl;
        }
        if (!isMinimal) {
            projectOnBasis(ampl, basis);
        }
        csl::Evaluate(ampl);
        csl::DeepExpandIf_lock(ampl, [&](csl::Expr const &e) {
            return bool(dynamic_cast<mty::QuantumField const *>(e.get()))
                   or (csl::IsIndicialTensor(e)
                       && e->getFreeIndexStructure().size() > 0);
        });
    }
    csl::Abbrev::disableGenericEvaluation("EXT");
    auto        wilsons    = match(amplitudesfull,
                         factor,
                         !isMinimal && (basis == OperatorBasis::Standard),
                         isMinimal);
    auto const &insertions = ampl.getKinematics().getInsertions();
    auto const &momenta    = ampl.getKinematics().getMomenta();
    const int   degeneracy = operatorDegeneracy(insertions);
    const int   effSign = matchingFermionSign(feynOptions.getFermionOrder());
    csl::ProgressBar       bar(wilsons.size());
    size_t                 index = 0;
    std::vector<csl::Expr> coefs;
    coefs.reserve(wilsons.size());
    for (auto &w : wilsons) {
        if (!feynOptions.getFeynRuleCalculation()) {
            bar.progress(index++);
        }
        csl::Expr a = w.coef.getCoefficient();
        if (!isMinimal)
            csl::Replace(a, csl::DMinko, csl::int_s(4));
        if (amplitudesfull.size() < 2000) {
            // Only if less than 2000 amplitudes
            simpli::simplify(
                a,
                recoverQuantumInsertions(GetExpression(insertions)),
                momenta,
                ampl.getOptions(),
                (isMinimal) ? simpli::SquaredAmplitude
                            : simpli::WilsonCoefficient);
            csl::DeepHardFactor(a);
        }
        coefs.push_back(a);
        csl::matcher::compress(a, 2);
    }
    for (size_t i = 0; i != wilsons.size(); ++i) {
        csl::Expr &a = coefs[i];
        Wilson &   w = wilsons[i];
        csl::matcher::compress(a, 2);
        if (isMinimal) {
            a = csl::Abbrev::makeAbbreviation("Cw", a);
        }
        else if (mode == DecompositionMode::Matching) {
            // Taking into account degeneracy if it is a real Wilson
            // coefficient calculation
            a *= csl::intfraction_s(effSign, degeneracy);
        }
        w.coef.setCoefficient(a);
    }
    // WilsonSet res(wilsons.begin(), wilsons.end());
    // res.sort();
    // res.mergeSorted();
    wilsons.options    = feynOptions;
    wilsons.kinematics = ampl.getKinematics();
    wilsons.graphs     = ampl.obtainGraphs();

    return wilsons;
}

WilsonSet
Model::computeWilsonCoefficients(int                           order,
                                 std::vector<Insertion> const &insertions,
                                 FeynOptions                   feynOptions,
                                 bool disableFermionOrdering)
{
    if (!disableFermionOrdering) {
        feynOptions.orderExternalFermions = true;
    }
    if (order == TreeLevel) {
        return computeWilsonCoefficients_default(
            order, insertions, feynOptions);
    }

    // One-loop calculation here
    size_t nF{0}, nV{0};
    size_t nTot = insertions.size();
    std::for_each(
        insertions.begin(), insertions.end(), [&](Insertion const &ins) {
            switch (ins.getField()->getSpinDimension()) {
            // case 1: --nTot; break; // Do not count spin 0
            case 2:
                ++nF;
                break;
            case 3:
                ++nV;
                break;
            }
        });
    if (nTot == 3 && nF == 2 && nV == 1)
        return computeWilsonCoefficients_2Fermions_1Vector(insertions,
                                                           feynOptions);
    if (nTot == 4 && nF == 4) {
        if (feynOptions.getFermionOrder().empty()) {
            CallHEPError(mty::error::RuntimeError,
                         "A fermion order order must be provided for the dim "
                         "6 Wilson "
                         "coefficient calculation through the FeynOptions "
                         "object.")
        }
        return computeWilsonCoefficients_4Fermions(insertions, feynOptions);
    }
    return computeWilsonCoefficients_default(OneLoop, insertions, feynOptions);
}

WilsonSet Model::computeWilsonCoefficients_default(
    int                           order,
    std::vector<Insertion> const &insertions,
    FeynOptions const &           feynOptions)
{
    auto option_cpy = feynOptions;
    auto ampl       = computeAmplitude(order, insertions, option_cpy);
    auto wilsons    = getWilsonCoefficients(ampl, option_cpy);
    return wilsons;
}

WilsonSet Model::computeWilsonCoefficients_2Fermions_1Vector(
    std::vector<Insertion> const &insertions, FeynOptions const &feynOptions)
{
    return computeWilsonCoefficients_default(OneLoop, insertions, feynOptions);
}

WilsonSet Model::computeWilsonBoxes_4Fermions(Kinematics const &kinematics,
                                              FeynOptions       feynOptions)
{
    csl::ScopedProperty prop(&mty::option::keepOnlyFirstMassInLoop, true);
    // feynOptions.verboseAmplitude = false;
    feynOptions.setTopology(feynOptions.getTopology() & Topology::Box);
    auto ampl = computeAmplitude(
        OneLoop, kinematics.getInsertions(), kinematics, feynOptions);
    // ampl.setKinematics(kinematics.alignedWith(ampl.getKinematics()));
    auto wilsons = getWilsonCoefficients(ampl); //, feynOptions);
    return wilsons;
}

int fermionSign(std::vector<Insertion> const &model,
                std::vector<Insertion>        order)
{
    // Supposes only fermion insertions with same size model && order
    int nPerm = 0;
    for (size_t i = 0; i != model.size(); ++i) {
        size_t index = i;
        for (size_t j = i + 1; j != order.size(); ++j) {
            if (model[i] == order[j]) {
                index = j;
                break;
            }
        }
        if (index != i) {
            std::swap(order[i], order[index]);
            nPerm += index - i;
        }
    }
    return (nPerm & 1) ? -1 : 1;
}

WilsonSet Model::computeSingleWilsonPenguin_4Fermions(
    Kinematics const &               kinematics,
    std::pair<size_t, size_t> const &treeCoupling,
    std::pair<size_t, size_t> const &loopCoupling,
    Insertion const &                mediator,
    FeynOptions                      feynOptions)
{
    // feynOptions.verboseAmplitude = false;
    auto const &           insertions     = kinematics.getInsertions();
    std::vector<Insertion> treeInsertions = {insertions[treeCoupling.first],
                                             insertions[treeCoupling.second],
                                             Mediator(mediator)};
    std::vector<Insertion> loopInsertions
        = {insertions[loopCoupling.first],
           insertions[loopCoupling.second],
           mediator.isIncoming() ? Mediator(Outgoing(mediator))
                                 : Mediator(Incoming(mediator))};
    std::vector<int> fermionOrder = feynOptions.getFermionOrder();
    feynOptions.setFermionOrder({});
    auto treeAmplitude
        = computePartialAmplitude(TreeLevel, treeInsertions, feynOptions);
    if (treeAmplitude.empty()) {
        return {};
    }
    const bool          massless = mediator.getField()->getMass() == CSL_0;
    csl::ScopedProperty prop(&mty::option::keepOnlyFirstMassInLoop,
                             !massless
                                 && mty::option::useMassiveSimplifications);
    auto                loopAmplitude
        = computePartialAmplitude(OneLoop, loopInsertions, feynOptions);
    feynOptions.applyFilters(loopAmplitude.getDiagrams(), true);
    if (loopAmplitude.empty()) {
        return {};
    }
    if (massless) {
        csl::ScopedProperty propWilson(&mty::option::decomposeInLocalOperator,
                                       false);
        auto                wil = getWilsonCoefficients(loopAmplitude,
                                         DecompositionMode::BasisProjection);
        applyPenguinPatch(wil, loopAmplitude.getKinematics());
        loopAmplitude.getDiagrams() = std::vector<FeynmanDiagram>(
            wil.size(), loopAmplitude.getDiagrams()[0]);
        for (size_t i = 0; i != wil.size(); ++i)
            loopAmplitude.getDiagrams()[i].getExpression()
                = wil[i].getExpression();
    }
    Amplitude connexion
        = connectAmplitudes(treeAmplitude, loopAmplitude, feynOptions);
    connexion.setKinematics(kinematics.alignedWith(connexion.getKinematics()));
    connexion.setKinematics(kinematics, false); // Do not apply any replacement
    feynOptions.setFermionOrder(fermionOrder);
    std::vector<Insertion> order = {insertions[treeCoupling.first],
                                    insertions[treeCoupling.second],
                                    insertions[loopCoupling.first],
                                    insertions[loopCoupling.second]};
    int sign = fermionSign(kinematics.getInsertions(), order);
    if (sign != -1) {
        for (auto &diag : connexion.getDiagrams())
            diag.getExpression() *= sign;
    }
    auto res   = getWilsonCoefficients(connexion, feynOptions);
    res.graphs = loopAmplitude.obtainGraphs();
    return res;
}

WilsonSet Model::computeWilsonPenguins_4Fermions(Kinematics const &kinematics,
                                                 FeynOptions       feynOptions)
{
    auto bosons = getPhysicalParticles(
        [&](Particle const &p) { return p->isBosonic(); });
    constexpr std::array<std::array<size_t, 4>, 6> pairs{{{0, 1, 2, 3},
                                                          {0, 2, 1, 3},
                                                          {0, 3, 1, 2},
                                                          {1, 2, 0, 3},
                                                          {1, 3, 0, 2},
                                                          {2, 3, 0, 1}}};
    std::vector<Amplitude>                         amplitudes;
    amplitudes.reserve(bosons.size());
    WilsonSet res;
    for (const mty::Particle &mediator : bosons) {
        for (const auto &[first, second, other1, other2] : pairs) {
            WilsonSet contrib
                = computeSingleWilsonPenguin_4Fermions(kinematics,
                                                       {first, second},
                                                       {other1, other2},
                                                       mediator,
                                                       feynOptions);
            if (!contrib.empty() && mty::option::verboseAmplitude) {
                std::cout << "Found penguins for \"" << mediator->getName()
                          << "\" mediator !" << std::endl;
                for (const auto &wil : contrib)
                    addWilson(wil, res, false);
                res.graphs.insert(res.graphs.end(),
                                  contrib.graphs.begin(),
                                  contrib.graphs.end());
            }
            if (!mediator->isSelfConjugate()) {
                WilsonSet contrib
                    = computeSingleWilsonPenguin_4Fermions(kinematics,
                                                           {first, second},
                                                           {other1, other2},
                                                           AntiPart(mediator),
                                                           feynOptions);
                if (!contrib.empty() && mty::option::verboseAmplitude) {
                    std::cout << "Found penguins for \"" << mediator->getName()
                              << "\" mediator !" << std::endl;
                    for (const auto &wil : contrib)
                        addWilson(wil, res, false);
                    res.graphs.insert(res.graphs.end(),
                                      contrib.graphs.begin(),
                                      contrib.graphs.end());
                }
            }
        }
    }
    return res;
}

WilsonSet
Model::computeWilsonCoefficients_4Fermions(std::vector<Insertion> insertions,
                                           FeynOptions            feynOptions)
{
    if (mty::option::verboseAmplitude)
        std::cout << "Using special 4-fermion calculation" << std::endl;
    feynOptions.orderInsertions = false;
    Kinematics kinematics{insertions};
    WilsonSet  res;
    res.kinematics = kinematics;
    res.options    = feynOptions;
    WilsonSet contrib;
    if (feynOptions.getTopology() & Topology::Box) {
        if (mty::option::verboseAmplitude)
            std::cout << "Box calculation ..." << std::endl;
        contrib = computeWilsonBoxes_4Fermions(kinematics, feynOptions);
        for (const auto &wil : contrib)
            addWilson(wil, res, false);
        res.graphs.insert(
            res.graphs.end(), contrib.graphs.begin(), contrib.graphs.end());
    }
    if (feynOptions.getTopology() & (Topology::Mass | Topology::Triangle)) {
        if (mty::option::verboseAmplitude)
            std::cout << "Penguin calculation ..." << std::endl;
        contrib = computeWilsonPenguins_4Fermions(kinematics, feynOptions);
        for (const auto &wil : contrib)
            addWilson(wil, res, false);
        res.graphs.insert(
            res.graphs.end(), contrib.graphs.begin(), contrib.graphs.end());
    }

    res.merge();
    res.sort();

    return res;
}

static int linkPosition(Kinematics const &M)
{
    auto const &insertions = M.getInsertions();
    for (size_t i = 0; i != insertions.size(); ++i) {
        if (insertions[i].isMediator())
            return static_cast<int>(i);
    }

    return -1;
}

std::pair<csl::Expr, csl::Expr>
Model::getMomentumReplacement(Amplitude const &M,
                              size_t           replacedMomentum) const
{
    std::vector<mty::QuantumField> fields;
    fields.reserve(M.getKinematics().size());
    for (const auto &ins : M.getKinematics().getInsertions()) {
        fields.push_back(
            *dynamic_cast<mty::QuantumField *>(ins.getExpression().get()));
    }
    return mty::simpli::getMomentumReplacement(
        fields, M.getKinematics().getMomenta(), replacedMomentum);
}

void Model::replaceMomentumForLink(
    Amplitude &M, std::pair<csl::Expr, csl::Expr> const &pReplacement) const
{
    for (auto &diag : M.getDiagrams()) {
        csl::Replace(
            diag.getExpression(), pReplacement.first, pReplacement.second);
    }
}

int Model::KinematicLink::isMediator(csl::Expr const &expr) const
{
    if (!csl::IsIndicialTensor(expr))
        return 0;
    const auto parent = expr->getParent();
    const auto point  = expr->getPoint();
    if (parent != mediator)
        return 0;
    if (point == pL)
        return -1;
    return point == pR;
}

Model::KinematicLink Model::connectKinematics(Amplitude &M1,
                                              Amplitude &M2) const
{
    Kinematics &k1 = M1.getKinematics();
    k1.sortFromIndices();
    Kinematics &k2 = M2.getKinematics();
    k2.sortFromIndices();
    const int linkM1 = linkPosition(k1);
    const int linkM2 = linkPosition(k2);
    HEPAssert(linkM1 != -1 && linkM2 != -1,
              mty::error::TypeError,
              "To connect two amplitudes they must contain exactly one "
              "mediator "
              "each, see mty::Mediator() for mty::Insertion objects.")
        size_t const mediatorIndex
        = k1.size() + k2.size() - 1;
    size_t              maxIndex = 0;
    std::vector<size_t> newK1Indices(k1.size() - 1);
    std::vector<size_t> newK2Indices(k2.size() - 1);
    for (size_t &i : newK1Indices)
        i = ++maxIndex;
    for (size_t &i : newK2Indices)
        i = ++maxIndex;
    newK1Indices.insert(newK1Indices.begin() + linkM1, mediatorIndex);
    newK2Indices.insert(newK2Indices.begin() + linkM2, mediatorIndex);

    Kinematics newK1 = k1.applyIndices(newK1Indices);
    M1.setKinematics(newK1);
    auto       pL_replacement = getMomentumReplacement(M1, linkM1);
    Kinematics newK2          = k2.applyIndices(newK2Indices);
    M2.setKinematics(newK2);
    auto pR_replacement = getMomentumReplacement(M2, linkM2);

    csl::Tensor   pL       = newK1.momentum(linkM1);
    csl::Tensor   pR       = newK2.momentum(linkM2);
    mty::Particle mediator = getParticle(newK1.insertion(linkM1).getField());

    return {Kinematics::merge(newK1, newK2),
            pL,
            pR,
            pL_replacement,
            pR_replacement,
            mediator};
}

bool Model::mediatorToPropagator(csl::Expr &          prod,
                                 KinematicLink const &link) const
{
    std::pair<int, int> mediatorPos{-1, -1};
    for (size_t i = 0; i != prod->size(); ++i) {
        int isMed = link.isMediator(prod[i]);
        if (isMed == -1) {
            mediatorPos.first = i;
        }
        else if (isMed == 1) {
            mediatorPos.second = i;
        }
    }
    if (mediatorPos.first == -1 || mediatorPos.second == -1)
        return false;
    auto phiA = *dynamic_cast<mty::QuantumField const *>(
        prod[mediatorPos.first].get());
    phiA.getIndexStructureView().erase(phiA.getIndexStructureView().begin());
    auto phiB = *dynamic_cast<mty::QuantumField const *>(
        prod[mediatorPos.second].get());
    phiB.getIndexStructureView().erase(phiB.getIndexStructureView().begin());
    phiB.setPoint(phiA.getPoint());
    phiA.setExternal(false);
    phiB.setExternal(false);
    csl::Tensor p    = link.pL;
    auto        prop = phiA.getPropagator(phiB, p);
    csl::Replace(prop, link.pL_replacement.first, link.pL_replacement.second);
    prod[mediatorPos.first]  = prop;
    prod[mediatorPos.second] = CSL_1;
    csl::Refresh(prod);
    return true;
}

csl::Expr Model::connectMediator(csl::Expr const &    M1,
                                 csl::Expr const &    M2,
                                 KinematicLink const &link) const
{
    csl::Abbrev::enableGenericEvaluation("EXT");
    csl::Expr M1_copy = csl::DeepCopy(M1);
    csl::Expr M2_copy = csl::DeepCopy(M2);
    csl::Expr full    = M1_copy;
    RenameIndices(full);
    full *= M2_copy;
    csl::Evaluate(full);
    csl::Abbrev::disableGenericEvaluation("EXT");
    auto isMediator
        = [&](csl::Expr const &sub) { return link.isMediator(sub) != 0; };
    csl::DeepPartialExpand(full, isMediator, isMediator);
    csl::Transform(full, [&](csl::Expr &node) {
        if (csl::IsProd(node)) {
            mediatorToPropagator(node, link);
            return true;
        }
        return false;
    });
    return full;
}

Amplitude Model::connectAmplitudes(Amplitude const &  M1,
                                   Amplitude const &  M2,
                                   FeynOptions const &options)
{
    Amplitude     M1_copy       = M1.copy();
    Amplitude     M2_copy       = M2.copy();
    KinematicLink kinematicLink = connectKinematics(M1_copy, M2_copy);
    replaceMomentumForLink(M1_copy, kinematicLink.pL_replacement);
    replaceMomentumForLink(M2_copy, kinematicLink.pR_replacement);

    Amplitude res(M1.getOptions(), kinematicLink.kinematics);
    for (const auto &diag1 : M1_copy.getDiagrams()) {
        for (const auto &diag2 : M2_copy.getDiagrams()) {
            csl::Expr full = connectMediator(
                diag1.getExpression(), diag2.getExpression(), kinematicLink);
            FeynmanDiagram combinedDiag = FeynmanDiagram::combine(
                diag1, diag2, kinematicLink.mediator);
            combinedDiag.getExpression() = full;
            res.add({combinedDiag});
        }
    }
    options.applyFilters(res.getDiagrams());

    return res;
}

void Model::filterFeynmanRules()
{
    size_t index = 0;
    for (size_t i = 0; i < feynmanRules.size(); ++i) {
        if (feynmanRules[i].isEmpty() or feynmanRules[i].isZero()) {
            L.interaction.erase(L.interaction.begin() + index);
        }
        else
            ++index;
    }
    for (size_t i = 0; i < feynmanRules.size(); ++i) {
        if (feynmanRules[i].isEmpty() or feynmanRules[i].isZero()) {
            feynmanRules.erase(feynmanRules.begin() + i);
            --i;
            continue;
        }
        for (size_t j = i + 1; j < feynmanRules.size(); ++j)
            if (feynmanRules[i].isSame(feynmanRules[j])) {
                feynmanRules.erase(feynmanRules.begin() + j);
                --j;
            }
    }
    std::sort(feynmanRules.begin(), feynmanRules.end());
}

void Model::computeFeynmanRules()
{
    csl::ScopedProperty prop(&mty::option::enableAntiChiralProps, false);
    feynmanRules.clear();
    feynmanRules.shrink_to_fit();
    feynmanRules.reserve(L.size());
    size_t index = 0;
    std::cout << "Computing Feynman Rules ..." << std::endl;
    csl::ProgressBar bar(L.size());
    for (const auto &interacTerm : L) {
        bar.progress(index++);
        feynmanRules.emplace_back(*this, interacTerm);
    }
    filterFeynmanRules();
    for (const auto &f : feynmanRules)
        HEPAssert(f.getDiagram(),
                  mty::error::TypeError,
                  "Null diagram encountered !")
}

std::vector<mty::FeynmanRule> const &Model::getFeynmanRules()
{
    if (feynmanRules.empty() and not L.empty())
        computeFeynmanRules();
    return feynmanRules;
}

std::ostream &operator<<(std::ostream &out, Model const &obj)
{
    return out << *static_cast<ModelData const *>(&obj);
}

} // End of namespace mty
