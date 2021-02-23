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

#include <thread>
#include <atomic>
#include <mutex>
#include "expander.h"
#include "amplitude.h"
#include "lagrangian.h"
#include "wick.h"
#include "mrtlog.h"
#include "graph.h"
#include "mrtOptions.h"
#include "mrtUtils.h"
#include "polarization.h"
#include "feynmanRule.h"
#include "drawer.h"
#include "mrtInterface.h"
#include "wilson.h"
#include "effModel.h"
#include "diracology.h"

using namespace std;
using namespace csl;

namespace mty {

Amplitude Expander::expandLagrangian(
        const std::vector<Tensor>& vertices,
        const csl::vector_expr&     insertions,
        std::vector<Tensor>& impulsions,
        Lagrangian&                 lagrangian,
        Options                     options,
        bool                        ruleMode)
{
    vector<QuantumField> fields;
    for (const auto& f : insertions) {
        HEPAssert(IsOfType<QuantumField>(f),
                mty::error::TypeError,
                "Insertion \"" + f->printLaTeX(1)
                + "\" in correlator should be a quantum field!");
        fields.push_back(ConvertTo<QuantumField>(f));
        fields[fields.size()-1].setExternal(true); // External field
    }

    Expander exp(vertices, 
                 fields, 
                 impulsions, 
                 lagrangian, 
                 options, 
                 ruleMode);
    return exp.expandLagrangian();
}

Amplitude Expander::expandLagrangian(
        const std::vector<Tensor>&     vertices,
        const csl::vector_expr&         insertions,
        std::vector<Tensor>&     impulsions,
        const std::vector<FeynmanRule>& feynmanRule,
        Options                         options,
        bool                            ruleMode)
{
    vector<QuantumField> fields;
    for (const auto& f : insertions) {
        HEPAssert(IsOfType<QuantumField>(f),
                mty::error::TypeError,
                "Insertion \"" + f->printLaTeX(1)
                + "\" in correlator should be a quantum field!");
        fields.push_back(ConvertTo<QuantumField>(f));
        fields[fields.size()-1].setExternal(true); // External field
    }
    Expander exp(vertices, 
                 fields, 
                 impulsions, 
                 feynmanRule, 
                 options, 
                 ruleMode);
    return exp.expandLagrangian();
}

static const std::vector<FeynmanRule> emptyFR;

Expander::Expander(const std::vector<Tensor>&      t_vertices,
                   const std::vector<QuantumField>& t_insertions,
                   std::vector<Tensor>&      t_impulsions,
                   Lagrangian&                      t_lagrangian,
                   Options                          t_options,
                   bool t_ruleMode)
    :ruleMode(t_ruleMode),
    L(&t_lagrangian), 
    feynmanRules(emptyFR),
    insertions(t_insertions),
    impulsions(t_impulsions),
    vertices(t_vertices),
    order(t_options.getExpansionOrder(t_insertions.size())),
    options(t_options),
    insertionsPaired(vector<vector<bool>>(1, 
                vector<bool>(insertions.size(), false))),
    numberVertices(vector<vector<size_t>>(1, vector<size_t>()))
{
    HEPAssert(order == (int)vertices.size(),
            mty::error::ValueError,
            "Wrong number of vertices for order " + toString(order));
    size_t i = 0;
    for (auto& field : insertions) {
        // std::cout << impulsions[i]->getName() << std::endl;
        csl::Tensor Xi = csl::tensor_s("X_"+toString(++i), &csl::Minkowski);
        field.setPoint(Xi);
        // field.print();
    }
}

Expander::Expander(const std::vector<Tensor>&      t_vertices,
                   const std::vector<QuantumField>& t_insertions,
                   std::vector<Tensor>&      t_impulsions,
                   const std::vector<FeynmanRule>&  t_feynmanRules,
                   Options                          t_options,
                   bool t_ruleMode)
    :ruleMode(t_ruleMode),
    L(nullptr),
    feynmanRules(t_feynmanRules),
    insertions(t_insertions),
    impulsions(t_impulsions),
    vertices(t_vertices),
    order(t_options.getExpansionOrder(t_insertions.size())),
    options(t_options),
    insertionsPaired(vector<vector<bool>>(1, 
                vector<bool>(insertions.size(), false))),
    numberVertices(vector<vector<size_t>>(1, vector<size_t>()))
{
    HEPAssert(order == (int)vertices.size(),
            mty::error::ValueError,
            "Wrong number of vertices for order " + toString(order));
    size_t i = 0;
    for (auto& field : insertions) {
        // std::cout << impulsions[i]->getName() << std::endl;
        csl::Tensor Xi = csl::tensor_s("X_"+toString(++i), &csl::Minkowski);
        field.setPoint(Xi);
        // field.print();
    }
}

bool containsIntegral(const csl::Expr& expr)
{
    if (expr->getType() != csl::Type::Prod and expr->getType() != csl::Type::Sum)
        return expr->getType() == csl::Type::VectorIntegral;
    for (const auto& arg : expr->getVectorArgument())
        if (arg->getType() == csl::Type::VectorIntegral)
            return true;
    return false;
}

Amplitude Expander::expandLagrangian()
{
    findNonZeroDiagrams();
    options.nonZeroCallBack.apply(numberVertices);
    csl::vector_expr exprInsertions(insertions.size());
    for (size_t i = 0; i != insertions.size(); ++i) {
        exprInsertions[i] = insertions[i].copy();
    }
    AmplitudeCalculator calculator(exprInsertions, impulsions);
    if (ruleMode)
        calculator.setRuleMode(true);

    size_t iTrial = 0;
    size_t number = 0;
    Amplitude amplitude;
    amplitude.insertions = insertions;
    amplitude.momenta = impulsions;
    if (numberVertices.empty())
        return amplitude;
    /////////////
    if (false and !ruleMode) {
        std::vector<size_t> tot;
        for (const auto &number : numberVertices)
            tot.insert(tot.end(), number.begin(), number.end());
        std::sort(tot.begin(), tot.end());
        auto last = std::unique(tot.begin(), tot.end());
        tot.erase(last, tot.end());

        std::set<mty::QuantumFieldParent const*> particles;
        for (size_t i : tot) {
            for (const auto &field : effFeynRules[i]->getFieldProduct()) {
                particles.insert(field.getQuantumParent());
            }
            std::cout << *effFeynRules[i] << std::endl;
        }
        for (const auto &field : particles)
            std::cout << field->getName() << ": " << field->getMass() << std::endl;

        std::cin.get();
    }
    /////////////
    for (size_t i = 0; i != numberVertices.size(); ++i) {
        auto& numbers = numberVertices[i];
        if (false && !ruleMode) {
            std::cout << "For rules" << std::endl;
            for (size_t n : numbers)
                std::cout << *effFeynRules[n] << std::endl;
        }
        ++iTrial;
        if (L == nullptr) {
            vector<FeynmanRule> rules(numbers.size());
            for (size_t i = 0; i != numbers.size(); ++i) {
                rules[i] = *effFeynRules[numbers[i]];
            }
            calculator.setFeynmanRules(rules);
        }
        else {
            vector<TermType> interactionTerms(numbers.size());
            for (size_t i = 0; i != numbers.size(); ++i) {
                interactionTerms[i] = std::make_shared<InteractionTerm>(
                        *effInteraction[numbers[i]]);
            }
            calculator.setInteractionTerms(interactionTerms);
        }
        Amplitude res;
        if (options.getMaxLoop() == 0) {
            csl::ScopedProperty p1(
                    &option::evaluateFermionTraces, 
                    false);
            csl::ScopedProperty p2(
                    &option::dimensionalRegularization,
                    false);
            res = calculator.getDiagrams();
        }
        else 
            res = calculator.getDiagrams();
        for (size_t j = 0; j != res.size(); ++j) {
            if (mty::option::verboseAmplitude
                    && !ruleMode 
                    && res.expressions[j] != CSL_0) {
                cout << "Particle amplitude found number " << ++number << ", ";
                cout << "trial " << iTrial << " / " << numberVertices.size()
                     << endl;
                std::cout << "Memory : " 
                    << csl::MemorySizeOf(res.expressions[j]) << "B\n";
            }
            if (res.expressions[j] != CSL_0) {
                csl::Expr finalRes;
                if(mty::option::expandAbbreviations) 
                    finalRes = csl::Evaluated(
                            res.expressions[j], 
                            csl::eval::abbreviation
                            );
                else
                    finalRes = res.expressions[j];
                amplitude.push_back(std::make_pair(
                            finalRes,
                            res.diagrams[j]
                            ));
            }
        }
    }
    if (not ruleMode) {
        if (mty::option::showDiagrams) {
            std::vector<std::shared_ptr<wick::Graph>> graphs;
            graphs.reserve(amplitude.size());
            for (const auto& g : amplitude.diagrams)
                graphs.push_back(g);
            Show(graphs);
        }
        if (!amplitude.empty())
            cout << endl;
    }

    if (!ruleMode 
            and !mty::option::amputateExternalLegs 
            and  mty::option::simplifyAmplitudes
            and  mty::option::decomposeInOperators) {
        std::vector<Wilson> wilsons;
        csl::Abbrev::enableGenericEvaluation("Fc");
        csl::Abbrev::enableGenericEvaluation("EXT");
        size_t ind = 0;
        if (mty::option::verboseAmplitude)
            std::cout << "Decomposing on an operator basis ..." << std::endl;
        csl::ProgressBar bar(amplitude.size());
        for (auto &ampl : amplitude.expressions) {
            if (mty::option::verboseAmplitude)
                bar.progress(ind++);
            HEPAssert(csl::TestIndexSanity(ampl),
                    mty::error::RuntimeError,
                    "Bad index property for " + toString(ampl))
            csl::Evaluate(ampl);
            auto predicate = [&](csl::Expr const &sub)
            {
                return IsOfType<mty::PolarizationField>(sub);
            };
            csl::Lock::lock(ampl, predicate);
            csl::DeepExpandIf(ampl, [&](csl::Expr const &sub) {
                return csl::AnyOfLeafs(sub, predicate);
            });
            csl::Lock::unlock(ampl);
            HEPAssert(csl::TestIndexSanity(ampl),
                    mty::error::RuntimeError,
                    "Bad index property for " + toString(ampl))
            std::vector<Wilson> newWilsons 
                = parseExpression(ampl, CSL_1, false);
            for (const auto &w : newWilsons) {
                EffModel::addWilson(w, wilsons, false);
            }
        }
        EffModel::mergeWilsons(wilsons);
        csl::Abbrev::disableGenericEvaluation("Fc");
        csl::Abbrev::disableGenericEvaluation("EXT");
        amplitude.expressions.clear();
        ind = 0;
        if (mty::option::verboseAmplitude)
            std::cout << "Merging operators" << std::endl;
        bar.reset(wilsons.size());
        for (const auto &w : wilsons) {
            if (mty::option::verboseAmplitude)
                bar.progress(ind++);
            amplitude.expressions.
                push_back(csl::DeepFactored(
                            w.coef.getCoefficient()*w.op.getExpression()));
            csl::Replace(
                    amplitude.expressions.back(),
                    csl::DMinko,
                    csl::int_s(4)
                    );
            // amplitude.expressions.back() = mty::dirac4
            //     .calculateTrace(amplitude.expressions.back());
            // AmplitudeCalculator::expandMomenta(
            //         amplitude.expressions.back(),
            //         impulsions,
            //         insertions
            //         );
            calculator.simplify(
                    amplitude.expressions.back(),
                    AmplitudeCalculator::Coef
                    );
            csl::matcher::compress(amplitude.expressions.back(), 2);
            HEPAssert(csl::TestIndexSanity(amplitude.expressions.back()),
                    mty::error::RuntimeError,
                    "Bad index property for " + toString(amplitude.expressions.back()))
        }
    }

    return amplitude;
}

void Expander::findNonZeroDiagrams()
{
    if (insertions.empty())
        return;
    auto gauge = insertions.front().getQuantumParent()->getGauge();
    if (
            !L 
            && mty::option::testRepresentations 
            && !gauge->containsTrivialRep(insertions)
            ) 
    {
        return;
    }
    HEPAssert(order > 0,
            mty::error::ValueError,
            "Order should be positive in Expander::expandLagrangian().");

    if (L == nullptr && effFeynRules.empty()) {
        effFeynRules = options.filter(feynmanRules);
        effInteraction = std::vector<TermType>(effFeynRules.size());
        for (size_t i = 0; i != effFeynRules.size(); ++i) 
            effInteraction[i] = effFeynRules[i]->getInteractionTerm();
    }
    else {
        effInteraction = options.filter(L->interaction);
        // effInteraction.clear();
        // for (const auto &f : insertions)
        //     std::cout << f << std::endl;
        // for (const auto &term : L->interaction) {
        //     bool containsAll = true;
        //     for (const auto &f : insertions)
        //         if (!term->contains(f.getQuantumParent())) {
        //             containsAll = false;
        //             break;
        //         }
        //     if (containsAll)
        //         effInteraction.push_back(term);
        // }
    }
    maxInteractionLegs = 0;
    for (const auto &term : effInteraction) {
        size_t nLegs = term->getContent().size();
        if (nLegs > maxInteractionLegs)
            maxInteractionLegs = nLegs;
    }
    vector<vector<size_t>> fixedSets;
    for (int iter = 0; iter != order; ++iter) {
        vector<vector<size_t>> newNumbers;
        if (order > 1)
            newNumbers.reserve(
                    std::min(
                        size_t(1e6),
                        effInteraction.size()*numberVertices.size()
                        )
                    );
        vector<vector<bool>>   newInsertionPairing;
        if (order > 1)
            newInsertionPairing.reserve(
                    std::min(
                        size_t(1e6),
                        effInteraction.size()*numberVertices.size()
                        )
                    );

        for (size_t iTerm = 0; iTerm != effInteraction.size(); ++iTerm) {
            if (not ruleMode) {
                int percent = iTerm * 100. / effInteraction.size();
                int percent_old = (iTerm-1) * 100. / effInteraction.size();
                if (percent != percent_old
                        and (iTerm > 1 or percent % 5 == 0)) {
                    //std::cout << percent << " % " << std::endl;
                }
            }
            vector<QuantumField> const &newFields 
                = effInteraction[iTerm]->getContent();
            for (size_t iV = 0; iV != numberVertices.size(); ++iV) {
                if (not ruleMode and 
                        not options.isValid(numberVertices[iV],
                                        iTerm,
                                        effInteraction,
                                        insertions)) {
                    continue;
                }
                vector<bool> pairing;
                if (contractionPossible(iV,
                                        newFields,
                                        pairing,
                                        iter,
                                        order)) 
                        addVertexCarefully(
                                iTerm,
                                iV,
                                newNumbers,
                                newInsertionPairing,
                                pairing);
            }
        }
        numberVertices   = std::move(newNumbers);
        insertionsPaired = std::move(newInsertionPairing);
        if (not ruleMode) {
            //std::cout << numberVertices.size() << " sets of vertices" << std::endl;
        }

        if (iter != order-1) {
            for (const auto& numbers : numberVertices) {

                vector<QuantumField const*> finalCorrelator;
                finalCorrelator.reserve(insertions.size()
                                    + numbers.size()*5);
                for (auto& field : insertions)
                    finalCorrelator.push_back(&field);
                for (size_t pos : numbers) {
                    std::vector<QuantumField> const& content 
                        = effInteraction[pos]->getContent();
                    for (auto const& field : content)
                        finalCorrelator.push_back(&field);
                }

                if (not wick::WickCalculator::
                        isContractionZero(finalCorrelator, 
                                          options.getMaxLoop(),
                                          insertions.size()+numbers.size())
                        and options.isValidFinal(finalCorrelator)) 
                    fixedSets.push_back(numbers);
            }
        }
    }
    numberVertices.insert(numberVertices.end(),
                          fixedSets.begin(),
                          fixedSets.end());
}

bool Expander::contractionPossible(size_t                posVertex,
                                   vector<QuantumField> const& newFields,
                                   vector<bool>&         newPairing,
                                   int                   order,
                                   int                   maxOrder)
{
    if (order >= maxOrder - 2) {
        vector<QuantumField const*> finalCorrelator;
        finalCorrelator.reserve(insertions.size()
                            + newFields.size()
                            + numberVertices[posVertex].size()*5);
        for (auto& field : insertions)
            finalCorrelator.push_back(&field);
        for (size_t pos : numberVertices[posVertex]) {
            std::vector<QuantumField> const& content 
                = effInteraction[pos]->getContent();
            for (auto& field : content)
                finalCorrelator.push_back(&field);
        }
        for (auto& field : newFields)
            finalCorrelator.push_back(&field);

        if (order == maxOrder - 1) {
            bool res = 
                not wick::WickCalculator::isContractionZero(finalCorrelator);
            res = res and options.isValidFinal(finalCorrelator);
            return res;
        }
        else {
            std::vector<size_t> indicesLeft(finalCorrelator.size());
            for (size_t i = 0; i != indicesLeft.size(); ++i)
                indicesLeft[i] = i;
            for (size_t i = 0; i < indicesLeft.size(); ++i) {
                for (size_t j = i+1; j < indicesLeft.size(); ++j) {
                    if (finalCorrelator[indicesLeft[i]]
                            ->isExactlyContractiblewith(
                                *finalCorrelator[indicesLeft[j]]
                                )) {
                        indicesLeft.erase(indicesLeft.begin() + j);
                        indicesLeft.erase(indicesLeft.begin() + i);
                        --i;
                        break;
                    }
                }
            }
            if (indicesLeft.size() > maxInteractionLegs)
                return false;
            size_t n = newFields.size();
            size_t n_connected = 0;
            for (const auto &f :newFields)
                for (size_t i = 0; i != finalCorrelator.size() - n; ++i) 
                    if (f.isExactlyContractiblewith(
                                *finalCorrelator[i]
                                )) {
                        ++n_connected;
                        break;
                    }
            if (n - n_connected > options.maxLoops + 1)
                return false;
        }
    }
    bool complete = true;
    bool insertionFiled = false;
    newPairing = insertionsPaired[posVertex];
    for (const auto& newF : newFields) {
        for (size_t j = 0; j != newPairing.size(); ++j) {
            if (not newPairing[j]) {
                complete = false;
                if (insertions[j].isExactlyContractiblewith(newF)) {
                    newPairing[j] = true;
                    insertionFiled = true;
                    break;
                }
            }
        }
    }
    if (not complete and not insertionFiled) {
        return false;
    }
    if (not complete)
        return true;

    std::vector<size_t> insertionLeft(insertions.size());
    for (size_t i = 0; i != insertions.size(); ++i)
        insertionLeft[i] = i;

    // Here insertions are all filled, we look for other fields
    for (size_t iF = 0;
                iF < numberVertices[posVertex].size();
                iF++) {
        std::vector<QuantumField> const& content 
            = effInteraction[numberVertices[posVertex][iF]]->getContent();
        for (const auto& fieldB : newFields)
            for (const auto& fieldA : content)
                if (fieldA.isExactlyContractiblewith(fieldB)) {
                    return true;
                }
    }
    return false;
}

void Expander::addVertexCarefully(size_t                  iTerm,
                                  size_t                  iVertex,
                                  vector<vector<size_t>>& numbers,
                                  vector<vector<bool>>&   newPairing,
                                  const vector<bool>&     pairing)
{
    // We insert the new number, keeping elements sorted in ascending order.
    vector<size_t> newNumbers(numberVertices[iVertex]);
    bool inserted = false;
    for (auto iter = newNumbers.begin(); iter != newNumbers.end(); ++iter)
        if (*iter > iTerm) {
            inserted = true;
            newNumbers.insert(iter, iTerm);
            break;
        }
    if (not inserted) {
        newNumbers.insert(newNumbers.end(), iTerm);
    }

    // We search if the sequence of numbers already appears in "numbers"
    // If so, the sequence of terms has already been seen, we do not add it
    if (auto pos = vertexMap.find(newNumbers); 
            pos != vertexMap.end() and (pos->second)) {
        return;
    }
    vertexMap[newNumbers] = true;
    numbers.push_back(newNumbers);
    newPairing.push_back(pairing);
}


} // End of namespace mty
