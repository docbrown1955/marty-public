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
#include "wilson.h"

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

mty::Expander::Options const &Model::getOptions() const
{
    return options;
}
mty::Expander::Options &Model::getOptions()
{
    return options;
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

void Model::initSquaredMomenta(
        std::vector<csl::Tensor>              &momenta,
        std::vector<mty::QuantumField>  const &fieldInsertions
        )
{
    initMomentums();
    csl::Index mu = csl::Minkowski.generateIndex();
    for (size_t i = 0; i != fieldInsertions.size(); ++i) {
        for (size_t j = 0; j != fieldInsertions.size(); ++j) {
            momenta[i]->removeSelfContraction(
                    momenta[i](mu),
                    momenta[j](+mu)
                    );
            if (i != j)
                momenta[j]->removeSelfContraction(
                        momenta[i](mu),
                        momenta[j](+mu)
                        );
        }
    }
    for (size_t i = 0; i != fieldInsertions.size(); ++i)
        for (size_t j = 0; j != fieldInsertions.size(); ++j) {
            if (i != j or !fieldInsertions[i].isOnShell()) {
                momenta[i]->addSelfContraction(
                        momenta[i](mu),
                        momenta[j](+mu),
                        momentaSquared[{i, j}]
                        );
            }
            else if (i == j) {
                momenta[i]->addSelfContraction(
                        momenta[i](mu),
                        momenta[i](+mu),
                        fieldInsertions[i].getSquaredMass()
                        );
            }
        }
}

mty::Amplitude Model::computeAmplitude(
        int                       order,
        std::vector<csl::Expr>  const &insertions,
        std::vector<csl::Tensor> &t_momenta,
        bool                      ruleMode
        )
{
    if (!ruleMode and feynmanRules.empty())
        computeFeynmanRules();
    std::vector<csl::Tensor> vertices;
    options.setMaxLoops(order);
    if (ruleMode)
        options.setMaxExpansionOrder(1);
    for (size_t i = 1; i <= options.getExpansionOrder(insertions.size()); ++i) {
        std::string name = "V_" + toString(i);
        csl::Expr tensor = csl::generateTensor(name, {&csl::Minkowski});
        vertices.push_back(csl::tensor_s(name, &csl::Minkowski, tensor));
    }
    initSquaredMomenta(
            t_momenta, 
            recoverQuantumInsertions(insertions)
            );
    if (not ruleMode)
        return Expander::expandLagrangian(
                vertices,
                insertions,
                t_momenta,
                feynmanRules,
                options,
                ruleMode);
    else
        return Expander::expandLagrangian(
                vertices,
                insertions,
                t_momenta,
                L,
                options,
                ruleMode);
}

mty::Amplitude Model::computeAmplitude(
        int                           order,
        std::vector<Insertion> const &insertions,
        bool                          ruleMode
        )
{
    std::vector<csl::Expr> expressions = GetExpression(insertions);
    return computeAmplitude(order, expressions, ruleMode);
}

mty::Amplitude Model::computeAmplitude(
        int                      order,
        std::vector<csl::Expr> const &insertions,
        bool                     ruleMode
        ) 
{
    return computeAmplitude(
            order,
            insertions,
            momenta,
            ruleMode);
}

csl::Expr Model::computeSquaredAmplitude(
        Amplitude const &ampl,
        bool             averageOverIncomingSpins
        )
{
    csl::ScopedProperty commut(&csl::option::checkCommutations, false);
    HEPAssert(!option::decomposeInOperators or !option::decomposeInLocalOperator,
            mty::error::ValueError,
            "Cannot (yet) calculate a cross-section with an amplitude "
            "that has been decomposed in operators. Please set mty::option::"
            "decomposeInOperators or mty::option::decomposeInLocalOperator to "
            "false.")
    if (ampl.expressions.empty())
        return CSL_0;
    csl::vector_expr eval(ampl.expressions.size());
    csl::Abbrev::enableGenericEvaluation("EXT");
    csl::Abbrev::enableGenericEvaluation("Fc");
    csl::Abbrev::enableGenericEvaluation("Color");
    csl::Abbrev::enableGenericEvaluation("P");
    for (size_t i = 0; i != ampl.expressions.size(); ++i)  {
        eval[i] = csl::Evaluated(ampl.expressions[i]);
        csl::DeepFactor(eval[i]);
    }
    csl::Abbrev::disableGenericEvaluation("EXT");
    csl::Abbrev::disableGenericEvaluation("Fc");
    csl::Abbrev::disableGenericEvaluation("Color");
    csl::Abbrev::disableGenericEvaluation("P");
    int incomingSpin = 1;
    if (averageOverIncomingSpins) {
        for (const auto &ins : ampl.insertions) {
            if (ins.isIncoming())
                incomingSpin *= ins.getQuantumParent()->getNDegreesOfFreedom();
        }
    }
    csl::vector_expr res;
    size_t index = 0;
    std::cout << "Squaring amplitude ..." << std::endl;
    csl::ProgressBar bar(ampl.expressions.size()*(ampl.expressions.size() + 1)/2);
    for (size_t i = 0; i != ampl.expressions.size(); ++i) {
        for (size_t j = i; j != ampl.expressions.size(); ++j) {
            if (mty::option::verboseAmplitude)
                bar.progress(index++);
            csl::Expr renamed = csl::Factored(csl::DeepCopy(eval[j]));
            csl::RenameIndices(renamed);
            csl::Expr prod = renamed
                * csl::GetHermitianConjugate(eval[i], &dirac4)
                / csl::int_s(incomingSpin);
            // csl::DeepPartialExpand(
            //         prod,
            //         [&](csl::Expr const &expr) {
            //             return IsOfType<PolarizationField>(expr);
            //         });
            auto predic = [&](csl::Expr const &sub) {
                return IsOfType<PolarizationField>(sub);
            };
            csl::DeepExpandIf_lock(prod, predic);
            // Expand(prod);
            // prod = dirac4.CalculateTrace(prod);
            AmplitudeCalculator(ampl.insertions, ampl.momenta).simplify(
                    prod, AmplitudeCalculator::Square
                    );
            // csl::DeepHardFactor(prod);
            // testFactor(prod);
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

csl::Expr Model::computeSquaredAmplitude(
        int                       order,
        std::vector<csl::Expr>  const &insertions,
        std::vector<csl::Tensor> &t_momenta,
        bool                      averageOverIncomingSpins
        )
{
    return computeSquaredAmplitude(
            computeAmplitude(order, insertions, t_momenta, averageOverIncomingSpins));
}

std::vector<Wilson> Model::getWilsonCoefficients(
        Amplitude const &ampl,
        csl::Expr        factor,
        OperatorBasis    basis
        )
{
    csl::ScopedProperty commut(&csl::option::checkCommutations, false);
    std::vector<csl::Expr> amplitudesEff;
    std::vector<csl::Expr> amplitudesfull(ampl.expressions);
    auto a = DiracIndices(2);
    for (auto &ampl : amplitudesfull) {
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
        ampl = csl::Evaluated(csl::Expanded(ampl / factor), csl::eval::abbreviation);
        csl::DeepExpandIf_lock(ampl, [&](csl::Expr const &e) { 
            return IsOfType<PolarizationField>(e); 
        });
    }
    EffModel Eff_PMSSM(*this, {});
    auto wilsons = Eff_PMSSM.match(
            amplitudesEff,
            amplitudesfull,
            CSL_1
            );

    for (auto &w : wilsons) {
        csl::Expr a = csl::Replaced(
                        w.coef.getCoefficient(),
                        csl::DMinko,
                        csl::int_s(4));
        AmplitudeCalculator(
                ampl.insertions, ampl.momenta
                ).simplify(a, AmplitudeCalculator::Coef);
        // csl::matcher::compress(a, 2);
        w.coef.setCoefficient(a);
    }

    return wilsons;
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

void Model::resetExpanderOptions()
{
    options = Expander::Options();
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
        feynmanRules.emplace_back(*this, interacTerm, momenta);
    }
    for (const auto &f : feynmanRules)
        HEPAssert(f.getDiagram(),
                mty::error::TypeError,
                "Null diagram encountered !")
    filterFeynmanRules();
    for (const auto &f : feynmanRules)
        HEPAssert(f.getDiagram(),
                mty::error::TypeError,
                "Null diagram encountered !")
    resetExpanderOptions();
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
