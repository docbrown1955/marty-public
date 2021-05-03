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
#include "model.h"
#include "insertion.h"
#include "graph.h"
#include "mrtError.h"
#include "amplitude.h"
#include "interactionTerm.h"
#include "diracology.h"
#include "expander.h"
#include "mrtOptions.h"
#include "mrtUtils.h"
#include "quantumFieldTheory.h"
#include "jsonToPhysics.h"
#include "mrtInterface.h"
#include "goldstoneField.h"
#include "polarization.h"
#include "fermionOrder.h"
#include "wilson.h"

#include "amplitudeInitializer.h"
#include "amplitudeSimplification.h"
#include "kinematics.h"
#include "feynOptions.h"

namespace mty {

size_t Model::getFeynmanRulesNumber() const
{
    return feynmanRules.size();
}

mty::FeynmanRule& Model::getFeynmanRule(size_t i)
{
    HEPAssert(i < feynmanRules.size(),
            mty::error::IndexError,
            "Index " + toString(i) + " out of bounds for "
            + toString(feynmanRules.size()) + " feynman rules.");
    return feynmanRules[i];
}

mty::FeynmanRule const& Model::getFeynmanRule(size_t i) const
{
    HEPAssert(i < feynmanRules.size(),
            mty::error::IndexError,
            "Index " + toString(i) + " out of bounds for "
            + toString(feynmanRules.size()) + " feynman rules.");
    return feynmanRules[i];
}

std::vector<mty::QuantumField> Model::recoverQuantumInsertions(
        std::vector<csl::Expr> const &expr
        )
{
    std::vector<mty::QuantumField> fields(expr.size());
    auto getField = [&](csl::Expr const &expr) {
        mty::QuantumField field;
        csl::Expr copy = expr;
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

mty::Amplitude Model::computeAmplitude(
        int                         order,
        std::vector<mty::Insertion> insertions,
        FeynOptions                 options
        )
{
    Kinematics kinematics { insertions };
    return computeAmplitude(order, insertions, kinematics, options);
}

mty::Amplitude Model::computeAmplitude(
        int                         order,
        std::vector<mty::Insertion> insertions,
        Kinematics           const &kinematics,
        FeynOptions                 options
        )
{
    auto fermionOrder = defaultFermionOrder(insertions);
    if (!options.getFeynRuleCalculation())
        options.setLoopOrder(order, static_cast<int>(insertions.size()));
    if (options.getFeynRuleCalculation()) {
        std::vector<Lagrangian::TermType> lagrangian = L.interaction;
        options.applyFilters(lagrangian);
        return computeAmplitude(
                lagrangian, insertions, fermionOrder, kinematics, options
                );
    }
    else {
        std::vector<FeynmanRule> const &rules = getFeynmanRules();
        auto filteredRules = options.applyFilters(rules);
        return computeAmplitude(
                filteredRules, insertions, fermionOrder, kinematics, options
                );
    }
}

mty::Amplitude Model::computeAmplitude(
        int                         order,
        std::vector<mty::Insertion> insertions,
        std::vector<int>     const &fermionOrder,
        FeynOptions                 options
        )
{
    Kinematics kinematics { insertions };
    return computeAmplitude(
            order, insertions, fermionOrder, kinematics, options
            );
}

mty::Amplitude Model::computeAmplitude(
        int                         order,
        std::vector<mty::Insertion> insertions,
        std::vector<int>     const &fermionOrder,
        Kinematics           const &kinematics,
        FeynOptions                 options
        )
{
    if (!options.getFeynRuleCalculation())
        options.setLoopOrder(order, static_cast<int>(insertions.size()));
    if (options.getFeynRuleCalculation()) {
        std::vector<Lagrangian::TermType> lagrangian = L.interaction;
        options.applyFilters(lagrangian);
        return computeAmplitude(
                lagrangian, insertions, fermionOrder, kinematics, options
                );
    }
    else {
        std::vector<FeynmanRule> const &rules = getFeynmanRules();
        auto filteredRules = options.applyFilters(rules);
        return computeAmplitude(
                filteredRules, insertions, fermionOrder, kinematics, options
                );
    }
}

mty::Amplitude Model::computeAmplitude(
        std::vector<Lagrangian::TermType> &lagrangian,
        std::vector<mty::Insertion>        insertions,
        std::vector<int>                   fermionOrder,
        Kinematics                  const &kinematics,
        FeynOptions                 const &options,
        std::vector<FeynmanRule const*>    rules
        )
{
    if (fermionOrder.empty())
        fermionOrder = defaultFermionOrder(insertions);
    if (options.orderExternalFermions)
        applyFermionOrder(insertions, fermionOrder);
    auto quantumInsertions = recoverQuantumInsertions(GetExpression(insertions));
    size_t i = 1;
    for (auto& field : quantumInsertions) {
        csl::Tensor Xi = csl::tensor_s("X_"+toString(i++), &csl::Minkowski);
        field.setPoint(Xi);
    }
    std::vector<std::vector<size_t>> terms = Expander::getDiagrams(
            this,
            options,
            lagrangian, 
            quantumInsertions
            );
    if (rules.empty()) {
        return AmplitudeInitializer::getAmplitude(
                terms,
                this,
                options,
                kinematics,
                quantumInsertions,
                lagrangian
                );
    }
    else {
        return AmplitudeInitializer::getAmplitude(
                terms,
                this,
                options,
                kinematics,
                quantumInsertions,
                rules
                );
    }
}

mty::Amplitude Model::computeAmplitude(
        std::vector<FeynmanRule const*> &feynRules,
        std::vector<Insertion>    const &insertions,
        std::vector<int>          const &fermionOrder,
        Kinematics                const &kinematics,
        FeynOptions               const &options
        )
{
    auto quantumInsertions = recoverQuantumInsertions(GetExpression(insertions));
    std::vector<Lagrangian::TermType> lagrangian(feynRules.size());
    for (size_t i = 0; i != lagrangian.size(); ++i) 
        lagrangian[i] = feynRules[i]->getInteractionTerm();
    return computeAmplitude(
            lagrangian, insertions, fermionOrder, kinematics, options, feynRules
            );
}

csl::Expr Model::computeSquaredAmplitude(
        Amplitude const &ampl,
        bool              applyDegreesOfFreedomFactor
        )
{
    csl::ScopedProperty prop(&mty::option::decomposeInLocalOperator, false);
    auto wilsons = getWilsonCoefficients(ampl, CSL_1, OperatorBasis::None, true);
    return computeSquaredAmplitude(
            wilsons,
            applyDegreesOfFreedomFactor
            );

}

csl::Expr Model::computeSquaredAmplitude(
        WilsonSet const &ampl,
        bool             applyDegreesOfFreedomFactor
        )
{
    csl::ScopedProperty commut(&csl::option::checkCommutations, false);
    HEPAssert(!option::decomposeInOperators or !option::decomposeInLocalOperator,
            mty::error::ValueError,
            "Cannot (yet) calculate a cross-section with an amplitude "
            "that has been decomposed in operators. Please set mty::option::"
            "decomposeInOperators or mty::option::decomposeInLocalOperator to "
            "false.")
    auto insertions  = ampl.kinematics.getInsertions();
    auto momenta     = ampl.kinematics.getMomenta();
    if (ampl.empty())
        return CSL_0;
    csl::Abbrev::enableGenericEvaluation("EXT");
    csl::Abbrev::enableGenericEvaluation("Fc");
    csl::Abbrev::enableGenericEvaluation("Color");
    csl::Abbrev::enableGenericEvaluation("P");
    std::vector<csl::Expr> eval(ampl.size());
    for (size_t i = 0; i != ampl.size(); ++i)  {
        eval[i] = csl::Evaluated(ampl[i].op.getOp());
        // csl::DeepFactor(eval[i]);
    }
    csl::Abbrev::disableGenericEvaluation("EXT");
    csl::Abbrev::disableGenericEvaluation("Fc");
    csl::Abbrev::disableGenericEvaluation("Color");
    csl::Abbrev::disableGenericEvaluation("P");
    csl::Expr incomingDof = CSL_1;
    if (applyDegreesOfFreedomFactor) {
        incomingDof = ampl.kinematics.getDegreesOfFreedomFactor();
    }
    csl::vector_expr res;
    size_t index = 0;
    std::cout << "Squaring amplitude ..." << std::endl;
    csl::ProgressBar bar(ampl.size()*(ampl.size() + 1)/2);
    for (size_t i = 0; i != ampl.size(); ++i) {
        for (size_t j = i; j != ampl.size(); ++j) {
            if (mty::option::verboseAmplitude)
                bar.progress(index++);
            csl::Expr renamed = csl::Factored(csl::DeepCopy(eval[j]));
            csl::RenameIndices(renamed);
            csl::Expr prod = renamed
                * csl::GetHermitianConjugate(eval[i], &dirac4)
                / incomingDof;
            auto predic = [&](csl::Expr const &sub) {
                return IsOfType<PolarizationField>(sub);
            };
            csl::DeepExpandIf_lock(prod, predic);
            simpli::simplify(
                    prod, 
                    recoverQuantumInsertions(GetExpression(insertions)),
                    momenta,
                    ampl.options,
                    simpli::SquaredAmplitude
                    );
            csl::Expr factor = csl::Abbrev::makeAbbreviation(
                    csl::GetComplexConjugate(ampl[i].op.getFactor())
                    * ampl[j].op.getFactor()
                    );
            prod = csl::prod_s({
                    prod, 
                    factor, 
                    csl::GetComplexConjugate(ampl[i].coef.getCoefficient()), 
                    ampl[j].coef.getCoefficient()
                    });
            res.push_back(prod);
            if (i != j) {
                res.push_back(csl::GetHermitianConjugate(prod, &dirac4));
            }
        }
    }
    auto squared = csl::Factored(csl::sum_s(res));
    csl::matcher::compress(squared, 2);
    return squared;
}

WilsonSet Model::getWilsonCoefficients(
        Amplitude const &ampl,
        csl::Expr        factor,
        OperatorBasis    basis,
        bool             squaredAfter
        )
{
    csl::ScopedProperty commut(&csl::option::checkCommutations, false);
    std::vector<csl::Expr> amplitudesfull(ampl.obtainExpressions());
    auto a = DiracIndices(2);
    csl::Abbrev::enableGenericEvaluation("EXT");
    for (auto &ampl : amplitudesfull) {
        if (!squaredAfter)
            ampl = CSL_I * csl::Copy(ampl);
        if (basis == OperatorBasis::Chiral) {
            csl::Replace(
                    ampl,
                    dirac4.gamma_chir({a[0], a[1]}),
                    dirac4.P_R({a[0], a[1]}) - dirac4.P_L({a[0], a[1]})
                    );
        }
        else if (basis == OperatorBasis::Standard) {
            csl::Replace(
                    ampl,
                    dirac4.P_L({a[0], a[1]}),
                    CSL_HALF*dirac4.getDelta()({a[0], a[1]}) 
                    - CSL_HALF*dirac4.gamma_chir({a[0], a[1]})
                    );
            csl::Replace(
                    ampl,
                    dirac4.P_R({a[0], a[1]}),
                    CSL_HALF*dirac4.getDelta()({a[0], a[1]}) 
                    + CSL_HALF*dirac4.gamma_chir({a[0], a[1]})
                    );
        }
        csl::Evaluate(ampl);
        csl::DeepExpandIf_lock(ampl, [&](csl::Expr const &e) { 
            return bool(dynamic_cast<mty::QuantumField const*>(e.get())); 
        });
    }
    csl::Abbrev::disableGenericEvaluation("EXT");
    auto wilsons = match(amplitudesfull, factor, squaredAfter);
    auto const &insertions = ampl.getKinematics().getInsertions();
    auto const &momenta    = ampl.getKinematics().getMomenta();
    for (auto &w : wilsons) {
        csl::Expr a = w.coef.getCoefficient();
        if (!squaredAfter)
            csl::Replace(a, csl::DMinko, csl::int_s(4));
        simpli::simplify(
                a, 
                recoverQuantumInsertions(GetExpression(insertions)),
                momenta,
                ampl.getOptions(),
                (squaredAfter) ? 
                    simpli::SquaredAmplitude : simpli::WilsonCoefficient
                );
        csl::matcher::compress(a, 2);
        if (squaredAfter) {
            a = csl::Abbrev::makeAbbreviation("Cw", a);
        }
        w.coef.setCoefficient(a);
    }
    WilsonSet res(wilsons.begin(), wilsons.end());
    res.options    = ampl.getOptions();
    res.kinematics = ampl.getKinematics();

    return res;
}

WilsonSet Model::computeWilsonCoefficients(
        int                           order,
        std::vector<Insertion> const &insertions,
        csl::Expr              const &factor,
        OperatorBasis                 basis
        )
{
    if (order == 0) {
        return computeWilsonCoefficients_default(order, insertions, factor, basis);
    }
    size_t nF{0}, nV{0};
    size_t nTot = insertions.size();
    std::for_each(insertions.begin(), insertions.end(), 
            [&](Insertion const &ins) {
                switch(ins.getField()->getSpinDimension()) {
                // case 1: --nTot; break; // Do not count spin 0
                case 2: ++nF; break;
                case 3: ++nV; break;
                }
            });
    if (nTot == 3 && nF == 2 && nV == 1)
        return computeWilsonCoefficients_2Fermions_1Vector(
                order, insertions, factor, basis
                );
    if (nTot == 4 && nF == 4)
        return computeWilsonCoefficients_4Fermions(
                order, insertions, factor, basis
                );
    return computeWilsonCoefficients_default(
            order, insertions, factor, basis
            );
}

WilsonSet Model::computeWilsonCoefficients_default(
        int                           order,
        std::vector<Insertion> const &insertions,
        csl::Expr              const &factor,
        OperatorBasis                 basis
        )
{
    auto ampl = computeAmplitude(
            order, insertions
            );
    auto wilsons = computeWilsonCoefficients(order, insertions, factor, basis);
    return wilsons;
}

WilsonSet Model::computeWilsonCoefficients_2Fermions_1Vector(
        int                           order,
        std::vector<Insertion> const &insertions,
        csl::Expr              const &factor,
        OperatorBasis                 basis 
        )
{
    auto ampl = computeAmplitude(
            order, insertions
            );
    auto wilsons = computeWilsonCoefficients(order, insertions, factor, basis);
    // Replace (q.A) by the (chromo-)magnetic operator
    return wilsons;
}

WilsonSet Model::computeWilsonCoefficients_4Fermions(
        int                           order,
        std::vector<Insertion> const &insertions,
        csl::Expr              const &,//factor,
        OperatorBasis                 //basis 
        )
{
    auto bosons = getPhysicalParticles([&](Particle const &p) { 
        return p->isBosonic(); 
    });
    std::array<size_t, 4> fermionPos;
    auto first = fermionPos.begin();
    for (size_t i = 0; i != insertions.size(); ++i) {
        if (insertions[i].getField()->getSpinDimension() == 2)
            *first++ = i;
    }
    constexpr std::array<std::array<size_t, 4>, 6> pairs {{
        {0, 1, 2, 3},
        {0, 2, 1, 3},
        {0, 3, 1, 2},
        {1, 2, 0, 3},
        {1, 3, 0, 2},
        {2, 3, 0, 1}
    }};
    std::vector<Amplitude> amplitudes;
    amplitudes.reserve(bosons.size());
    for (const mty::Particle &mediator : bosons) {
        for (const auto &[first, second, other1, other2] : pairs) {
            std::vector<Insertion> ins = {
                insertions[fermionPos[first]], 
                insertions[fermionPos[second]],
                Incoming(mediator)
            };
            auto conjugated = Outgoing(mediator);
            auto treeCoupling = computeAmplitude(
                    Order::TreeLevel,
                    ins
                    );
            if (treeCoupling.empty() && !mediator->isSelfConjugate()) {
                ins = {
                    insertions[fermionPos[first]], 
                    insertions[fermionPos[second]],
                    Outgoing(mediator)
                };
                conjugated = Incoming(mediator);
                treeCoupling = computeAmplitude(
                        Order::TreeLevel,
                        ins
                        );
            }
            if (treeCoupling.empty())
                continue;
            auto loopCoupling = computeAmplitude(
                    order,
                    {insertions[other1], insertions[other2], conjugated}
                    );
            if (loopCoupling.empty())
                continue;
        }
    }
    return WilsonSet{};
}


void Model::filterFeynmanRules()
{
    size_t index = 0;
    for (size_t i = 0; i < feynmanRules.size(); ++i) {
        if (feynmanRules[i].isEmpty() or feynmanRules[i].isZero()) {
            L.interaction.erase(L.interaction.begin()+index);
        }
        else
            ++index;
    }
    for (size_t i = 0; i < feynmanRules.size(); ++i) {
        if (feynmanRules[i].isEmpty() or feynmanRules[i].isZero()) {
            feynmanRules.erase(feynmanRules.begin()+i);
            --i;
            continue;
        }
        for (size_t j = i+1; j < feynmanRules.size(); ++j)
            if (feynmanRules[i].isSame(feynmanRules[j])) {
                feynmanRules.erase(feynmanRules.begin()+j);
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
    for (const auto& interacTerm : L) {
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

std::ostream& operator<<(
        std::ostream &out,
        Model const  &obj
        )
{
    return out << *static_cast<ModelData const*>(&obj);
}

} // End of namespace mty
