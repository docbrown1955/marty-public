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
        
    std::vector<std::vector<size_t>> Expander::findNonZeroDiagrams()
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
            std::cout << "No representation !" << '\n';
            return {};
        }
        const int order  = options.getExpansionOrder();
        const int nLoops = options.getLoopOrder();
        HEPAssert(order > 0,
                mty::error::ValueError,
                "Order should be positive in Expander::expandLagrangian().");

        size_t maxInteractionLegs = 0;
        for (const auto &term : effectiveLagrangian) {
            size_t nLegs = term->getContent().size();
            if (nLegs > maxInteractionLegs)
                maxInteractionLegs = nLegs;
        }
        std::map<std::vector<size_t>, bool> vertexMap;
        std::vector<std::vector<bool>>   insertionsPaired(
                1, std::vector<bool>(insertions.size(), false)
                );
        std::vector<std::vector<size_t>> numberVertices(
                1, std::vector<size_t>{}
                );
        std::vector<std::vector<size_t>> fixedSets;
        if (options.verboseAmplitude) {
            std::cout << "Expanding Lagrangian at order " << order << " ...\n";
        }
        csl::ProgressBar bar(order);
        for (int iter = 0; iter != order; ++iter) {
            if (options.verboseAmplitude) {
                bar.progress(iter);
            }
            std::vector<std::vector<size_t>> newNumbers;
            std::vector<std::vector<bool>>   newInsertionPairing;
            if (order > 1) {
                newNumbers.reserve(
                        std::min(
                            static_cast<size_t>(1e6),
                            effectiveLagrangian.size()*numberVertices.size()
                            )
                        );
                newInsertionPairing.reserve(
                        std::min(
                            static_cast<size_t>(1e6),
                            effectiveLagrangian.size()*numberVertices.size()
                            )
                        );
            }

            for (size_t iTerm = 0; iTerm != effectiveLagrangian.size(); ++iTerm) {
                std::vector<QuantumField> const &newFields 
                    = effectiveLagrangian[iTerm]->getContent();
                for (size_t iV = 0; iV != numberVertices.size(); ++iV) {
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
                    for (size_t pos : numbers) {
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
            std::vector<size_t>       const& fieldPos,
            std::vector<bool>         const& insertionsPaired,
            std::vector<QuantumField> const& newFields,
            std::vector<bool>&               newPairing,
            int                              order,
            int                              maxOrder,
            size_t                           nLoops,
            size_t                           maxInteractionLegs
            )
    {
        if (order >= maxOrder - 2) {
            std::vector<QuantumField const*> finalCorrelator;
            finalCorrelator.reserve(insertions.size()
                                + newFields.size()
                                + fieldPos.size()*5);
            for (auto& field : insertions)
                finalCorrelator.push_back(&field);
            for (size_t pos : fieldPos) {
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
                if (n - n_connected > nLoops + 1)
                    return false;
            }
        }
        bool complete = true;
        bool insertionFiled = false;
        newPairing = insertionsPaired;
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
            size_t                  iTerm,
            std::vector<size_t>        const& terms,
            std::vector<std::vector<size_t>>& numbers,
            std::vector<std::vector<bool>>&   newPairing,
            const std::vector<bool>&     pairing,
            std::map<std::vector<size_t>, bool> &vertexMap
            )
    {
        // We insert the new number, keeping elements sorted in ascending order.
        std::vector<size_t> newNumbers(terms);
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
