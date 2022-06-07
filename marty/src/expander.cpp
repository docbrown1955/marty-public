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

#include "expander.h"
#include "mrtOptions.h"
#include "mrtError.h"
#include "quantumField.h"
#include "model.h"

namespace mty {

    Expander::Expander(
            mty::Model  const *t_model,
            FeynOptions const &t_options,
            std::vector<mty::Lagrangian::TermType> const &lagrangian,
            std::vector<mty::QuantumField>         const &t_insertions
            )
        :model(t_model),
        options(t_options),
        effectiveLagrangian(lagrangian),
        insertions(t_insertions)
    {

    }
        
    std::vector<std::vector<std::size_t>> Expander::findNonZeroDiagrams()
    {
        if (insertions.empty()) {
            return {};
        }
        auto gauge = insertions.front().getQuantumParent()->getGauge();
        std::vector<mty::QuantumField> fields(insertions.size());
        if (
                !options.getFeynRuleCalculation()
                && mty::option::testRepresentations 
                && !gauge->containsTrivialRep(insertions)
                ) 
        {
            return {};
        }
        const int order  = options.getExpansionOrder();
        const int nLoops = options.getLoopOrder();
        HEPAssert(order > 0,
                mty::error::ValueError,
                "Order should be positive in Expander::expandLagrangian().");

        std::size_t maxInteractionLegs = 0;
        for (const auto &term : effectiveLagrangian) {
            std::size_t nLegs = term->getContent().size();
            if (nLegs > maxInteractionLegs)
                maxInteractionLegs = nLegs;
        }
        std::map<std::vector<std::size_t>, bool> vertexMap;
        std::vector<std::vector<bool>>   insertionsPaired(
                1, std::vector<bool>(insertions.size(), false)
                );
        std::vector<std::vector<std::size_t>> numberVertices(
                1, std::vector<std::size_t>{}
                );
        std::vector<std::vector<std::size_t>> fixedSets;
        if (options.verboseAmplitude) {
            std::cout << "Expanding Lagrangian at order " << order << " ...\n";
        }
        csl::ProgressBar bar(order);
        for (int iter = 0; iter != order; ++iter) {
            if (options.verboseAmplitude) {
                bar.progress(iter);
            }
            std::vector<std::vector<std::size_t>> newNumbers;
            std::vector<std::vector<bool>>   newInsertionPairing;
            if (order > 1) {
                newNumbers.reserve(
                        std::min(
                            static_cast<std::size_t>(1e6),
                            effectiveLagrangian.size()*numberVertices.size()
                            )
                        );
                newInsertionPairing.reserve(
                        std::min(
                            static_cast<std::size_t>(1e6),
                            effectiveLagrangian.size()*numberVertices.size()
                            )
                        );
            }

            for (std::size_t iTerm = 0; iTerm != effectiveLagrangian.size(); ++iTerm) {
                std::vector<QuantumField> const &newFields 
                    = effectiveLagrangian[iTerm]->getContent();
                for (std::size_t iV = 0; iV != numberVertices.size(); ++iV) {
                    std::vector<bool> pairing;
                    if (contractionPossible(numberVertices[iV],
                                            insertionsPaired[iV],
                                            newFields,
                                            pairing,
                                            iter,
                                            order,
                                            nLoops,
                                            maxInteractionLegs)) 
                            addVertexCarefully(
                                    iTerm,
                                    numberVertices[iV],
                                    newNumbers,
                                    newInsertionPairing,
                                    pairing,
                                    vertexMap
                                    );
                }
            }
            numberVertices   = std::move(newNumbers);
            insertionsPaired = std::move(newInsertionPairing);

            if (iter != order-1) {
                for (const auto& numbers : numberVertices) {

                    std::vector<QuantumField const*> finalCorrelator;
                    finalCorrelator.reserve(insertions.size()
                                        + numbers.size()*5);
                    for (auto& field : insertions)
                        finalCorrelator.push_back(&field);
                    for (std::size_t pos : numbers) {
                        std::vector<QuantumField> const& content 
                            = effectiveLagrangian[pos]->getContent();
                        for (auto const& field : content)
                            finalCorrelator.push_back(&field);
                    }

                    if (not wick::WickCalculator::
                            isContractionZero(finalCorrelator, 
                                              nLoops,
                                              insertions.size()+numbers.size())) 
                        fixedSets.push_back(numbers);
                }
            }
        }
        numberVertices.insert(numberVertices.end(),
                              fixedSets.begin(),
                              fixedSets.end());
        if (options.verboseAmplitude) {
            const auto plural = (numberVertices.size() == 1) ? "" : "s";
            std::cout << numberVertices.size() << " possible set"
                << plural << " of vertices found."<< '\n';
        }

        return numberVertices;
    }

    bool Expander::contractionPossible(
            std::vector<std::size_t>       const& fieldPos,
            std::vector<bool>         const& insertionsPaired,
            std::vector<QuantumField> const& newFields,
            std::vector<bool>&               newPairing,
            int                              order,
            int                              maxOrder,
            std::size_t                           nLoops,
            std::size_t                           maxInteractionLegs
            )
    {
        if (order >= maxOrder - 2) {
            std::vector<QuantumField const*> finalCorrelator;
            finalCorrelator.reserve(insertions.size()
                                + newFields.size()
                                + fieldPos.size()*5);
            for (auto& field : insertions)
                finalCorrelator.push_back(&field);
            for (std::size_t pos : fieldPos) {
                std::vector<QuantumField> const& content 
                    = effectiveLagrangian[pos]->getContent();
                for (auto& field : content)
                    finalCorrelator.push_back(&field);
            }
            for (auto& field : newFields) {
                finalCorrelator.push_back(&field);
            }

            if (order == maxOrder - 1) {
                return !wick::WickCalculator::isContractionZero(finalCorrelator);
            }
            else {
                std::vector<std::size_t> indicesLeft(finalCorrelator.size());
                for (std::size_t i = 0; i != indicesLeft.size(); ++i)
                    indicesLeft[i] = i;
                for (std::size_t i = 0; i < indicesLeft.size(); ++i) {
                    for (std::size_t j = i+1; j < indicesLeft.size(); ++j) {
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
                std::size_t n = newFields.size();
                std::size_t n_connected = 0;
                for (const auto &f :newFields)
                    for (std::size_t i = 0; i != finalCorrelator.size() - n; ++i) 
                        if (f.isExactlyContractiblewith(
                                    *finalCorrelator[i]
                                    )) {
                            ++n_connected;
                            break;
                        }
                if (n - n_connected > nLoops + 1)
                    return false;
            }
        }
        bool complete = true;
        bool insertionFiled = false;
        newPairing = insertionsPaired;
        for (const auto& newF : newFields) {
            for (std::size_t j = 0; j != newPairing.size(); ++j) {
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

        std::vector<std::size_t> insertionLeft(insertions.size());
        for (std::size_t i = 0; i != insertions.size(); ++i)
            insertionLeft[i] = i;

        // Here insertions are all filled, we look for other fields
        for (std::size_t iF = 0;
                    iF < fieldPos.size();
                    iF++) {
            std::vector<QuantumField> const& content 
                = effectiveLagrangian[fieldPos[iF]]->getContent();
            for (const auto& fieldB : newFields)
                for (const auto& fieldA : content)
                    if (fieldA.isExactlyContractiblewith(fieldB)) {
                        return true;
                    }
        }
        return false;
    }

    void Expander::addVertexCarefully(
            std::size_t                  iTerm,
            std::vector<std::size_t>        const& terms,
            std::vector<std::vector<std::size_t>>& numbers,
            std::vector<std::vector<bool>>&   newPairing,
            const std::vector<bool>&     pairing,
            std::map<std::vector<std::size_t>, bool> &vertexMap
            )
    {
        // We insert the new number, keeping elements sorted in ascending order.
        std::vector<std::size_t> newNumbers(terms);
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

} // namespace mty
