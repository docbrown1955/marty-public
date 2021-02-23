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

namespace mty {

    Expander::Options::Options()
        :maxLoops(0),
        maxExpansionOrder(-1),
        validTerm([](mty::InteractionTerm const&) { return true; })
    {

    }

    void Expander::Options::clear()
    {
        *this = Options();
    }

    size_t Expander::Options::getMaxLoop() const
    {
        return maxLoops;
    }

    size_t Expander::Options::getExpansionOrder(size_t nInsertions) const 
    {
        return std::min(maxExpansionOrder,
                        2 * (maxLoops - 1) + nInsertions);
    }

    void Expander::Options::setMaxLoops(size_t nLoops)
    {
        maxLoops = nLoops;
    }

    void Expander::Options::setMaxExpansionOrder(size_t order)
    {
        maxExpansionOrder = order;
    }

    void Expander::Options::setMaxOrderForCoupling(
            csl::Expr const& coupling,
            size_t      order)
    {
        maxOrderCoupling[coupling] = order;
    }

    void Expander::Options::disableParticle(mty::Particle const& particle)
    {
        particle->setEnabledInDiagrams(false);
    }

    void Expander::Options::enableParticle(mty::Particle const& particle)
    {
        particle->setEnabledInDiagrams(true);
    }

    void Expander::Options::forceParticle(mty::Particle const& particle)
    {
        auto pos = std::find_if(forcedParticles.begin(),
                                forcedParticles.end(),
                                [&particle](Particle const& p) {
                                  return particle.get() == p.get();
                                });
        if (pos == forcedParticles.end())
            forcedParticles.push_back(particle);
    }

    void Expander::Options::unForceParticle(mty::Particle const& particle)
    {
        auto pos = std::find_if(forcedParticles.begin(),
                                forcedParticles.end(),
                                [&particle](Particle const& p) {
                                  return particle.get() == p.get();
                                });
        if (pos != forcedParticles.end())
            forcedParticles.erase(pos);
    }

    void Expander::Options::setTermValidity(TermChoice choice)
    {
        validTerm = choice;
    }

    void Expander::Options::setConservedQuantumNumbers(
            std::vector<mty::QuantumNumber> const& numbers)
    {
        conservedNumbers = numbers;
    }

    bool Expander::Options::isValid(mty::QuantumField const& particle) const
    {
        return particle.getQuantumParent()->isEnabledInDiagrams();
    }

    bool Expander::Options::isValid(Expander::TermType const& term) const
    {
        std::vector<mty::QuantumField> const& content = term->getContent();
        for (const auto& field : content) {
            if (not isValid(field)) {
                return false;
            }
        }
        return validTerm(*term);
    }

    bool Expander::Options::isValidFinal(
            std::vector<QuantumField const*> const& correlator) const
    {
        bool found{false};
        for (const auto& forced : forcedParticles) {
            auto pos = std::find_if(
                    correlator.begin(),
                    correlator.end(),
                    [&](QuantumField const* field) {
                        return field->getQuantumParent() == forced.get();
                    });
            if (pos != correlator.end()) {
                found = true;
                break;
            }
        }
        if (!found and !forcedParticles.empty())
            return false;
        for (const auto& particle : correlator) {
            if (not isValid(*particle))
                return false;
        }
        return true;
    }

    std::vector<Expander::TermType> Expander::Options::filter(
            std::vector<Expander::TermType>& init) const
    {
        std::vector<TermType> res;
        for (auto& term : init)
            if (isValid(term))
                res.push_back(term);

        return res;
    }

    std::vector<Expander::TermType> Expander::Options::filter(
            std::vector<Expander::TermType>& init,
            std::vector<mty::QuantumField> const &model) const
    {
        std::vector<mty::QuantumField> fields(model);
        for (auto &f : fields)
            f.conjugate();
        std::vector<TermType> res;
        for (auto& term : init) {
            if (term->hasSameContent(fields)) {
                res.push_back(term);
            }
        }

        return res;
    }

    std::vector<FeynmanRule const*> Expander::Options::filter(
            std::vector<mty::FeynmanRule> const& init) const
    {
        std::vector<FeynmanRule const*> res;
        res.reserve(init.size());
        for (auto const& term : init)
            if (isValid(term.getInteractionTerm()))
                res.push_back(&term);

        return res;
    }

    std::vector<size_t> Expander::Options::filterExternalConnections(
            std::vector<mty::FeynmanRule>  const &init,
            std::vector<mty::QuantumField> const &extLegs) const
    {
        std::vector<size_t> res;
        res.reserve(init.size());
        for (size_t i = 0; i != init.size(); ++i) {
            auto const &term = init[i];
            bool containsExt = false;
            for (const auto &f : extLegs)
                if (term.contains(f.getQuantumParent())) {
                    containsExt = true;
                    break;
                }
            if (!containsExt)
                continue;
            if (isValid(term.getInteractionTerm()))
                res.push_back(i);
        }

        return res;
    }

    bool Expander::Options::isValid(
            std::vector<size_t>                const& involvedTerms,
            size_t                                    newTerm,
            std::vector<Expander::TermType>         & interactions,
            std::vector<mty::QuantumField>     const& insertions) const
    {
        if (not isValid(interactions[newTerm]))
            return false;
        // Testing number of loops
        const size_t nVertices = insertions.size() + involvedTerms.size() + 1;
        size_t nEdges = insertions.size();
        for (size_t pos : involvedTerms)
            nEdges += interactions[pos]->size();
        nEdges += interactions[newTerm]->size();
        if (nEdges/2 > maxLoops + nVertices - 1) {
            return false;
        }

        // Testing order of coulings
        for (const auto& [coupling, maxOrder] : maxOrderCoupling) {
            size_t count = interactions[newTerm]->getExponent(coupling);
            if (count > maxOrder) {
                return false;
            }
            for (size_t pos : involvedTerms) {
                count += interactions[pos]->getExponent(coupling);
                if (count > maxOrder) {
                    return false;
                }
            }
        }

        std::vector<QuantumNumber> toTest;
        toTest.reserve(conservedNumbers.size());
        for (const auto& q : conservedNumbers)
            if (not q.appearsIn(insertions))
                toTest.push_back(q);
        if (toTest.size() > maxLoops)
            return testQuantumNumbers(toTest,
                                      involvedTerms,
                                      newTerm,
                                      interactions,
                                      insertions);
        return true;
    }

    bool Expander::Options::testQuantumNumbers(
            std::vector<mty::QuantumNumber>    const& toTest,
            std::vector<size_t>                const& involvedTerms,
            size_t                                    newTerm,
            std::vector<Expander::TermType>         & interactions,
            std::vector<mty::QuantumField>     const&) const
    {
        size_t nLoops = 0;
        for (const auto& number : toTest) {
            if (not number.appearsIn(interactions[newTerm]->getContent())) {
                for (size_t pos : involvedTerms)
                    if (number.appearsIn(interactions[pos]->getContent())) {
                        ++nLoops;
                        break;
                    }
            }
            else 
                ++nLoops;
            if (nLoops > maxLoops) {
                return false;
            }
        }
        for (const auto& number : toTest) {
            if (not number.appearsIn(interactions[newTerm]->getContent())) {
                size_t n = 0;
                for (size_t pos : involvedTerms)
                    if (number.appearsIn(interactions[pos]->getContent())) {
                        ++n;
                    }
                if (n == 1)
                    return false;
            }
        }

        return true;
    }

}
