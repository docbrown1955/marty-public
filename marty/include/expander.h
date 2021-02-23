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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#ifndef LAGRANGIANEXPENDER_H_INCLUDED
#define LAGRANGIANEXPENDER_H_INCLUDED

#include <atomic>
#include <vector>
#include <functional>
#include <map>
#include "csl.h"
#include "lagrangian.h"
#include "callback.h"
#include "feynmanRule.h"
#include "quantumField.h"

namespace mty::wick{
class Graph;
}


namespace mty {

struct Amplitude;

enum Order {

    TreeLevel,
    OneLoop,
    TwoLoops,
    ThreeLoops,
};

class Expander {
    
    public:

    using TermType = Lagrangian::TermType;

    class Options {

        friend class Expander;

        public:

        using TermChoice = std::function<bool(mty::InteractionTerm const&)>;

        Options();

        Options(Options const& other) = default;

        ~Options() {}

        void clear();

        size_t getMaxLoop() const;

        size_t getExpansionOrder(size_t nInsertions) const;

        void setMaxLoops(size_t nLoops);

        void setMaxExpansionOrder(size_t order);

        void setMaxOrderForCoupling(
                csl::Expr const& coupling,
                size_t      order);

        void disableParticle(mty::Particle const& particle);

        void enableParticle(mty::Particle const& particle);

        void forceParticle(mty::Particle const& particle);

        void unForceParticle(mty::Particle const& particle);

        void setTermValidity(TermChoice choice);

        void setConservedQuantumNumbers(
                std::vector<mty::QuantumNumber> const& n);

        private:

        bool isValid(mty::QuantumField const& particle) const;

        bool isValid(TermType const& term) const;

        std::vector<TermType> filter(
                std::vector<TermType>& init,
                std::vector<mty::QuantumField> const &model) const;

        std::vector<TermType> filter(
                std::vector<TermType>& init) const;

        std::vector<FeynmanRule const*> filter(
                std::vector<FeynmanRule> const& init) const;

        std::vector<size_t> filterExternalConnections(
                std::vector<FeynmanRule>  const& init,
                std::vector<QuantumField> const &extLegs
                ) const;

        bool isValid(
                std::vector<size_t>                const& involvedTerms,
                size_t                                    newTerm,
                std::vector<TermType>      & interactions,
                std::vector<mty::QuantumField>     const& insertions) const;

        bool isValidFinal(
                std::vector<QuantumField const*> const& correlator) const;

        bool testQuantumNumbers(
                std::vector<mty::QuantumNumber>    const& toTest,
                std::vector<size_t>                const& involvedTerms,
                size_t                                    newTerm,
                std::vector<TermType>      & interactions,
                std::vector<mty::QuantumField>     const& insertions) const;

        private:

        size_t                     maxLoops;

        size_t                     maxExpansionOrder;

        std::map<csl::Expr, size_t>     maxOrderCoupling;

        std::vector<mty::Particle> forcedParticles;

        std::vector<QuantumNumber> conservedNumbers;

        TermChoice                 validTerm;

        CallBack<void(
                std::vector<std::vector<size_t>> &
                )> nonZeroCallBack;
    };


    public:

    static Amplitude expandLagrangian(
            const std::vector<csl::Tensor>& vertices,
            const csl::vector_expr&         insertions,
            std::vector<csl::Tensor>& impulsions,
            mty::Lagrangian&                lagrangian,
            mty::Expander::Options          t_options,
            bool                            t_ruleMode = false);

    static Amplitude expandLagrangian(
            const std::vector<csl::Tensor>&  vertices,
            const csl::vector_expr&          insertions,
            std::vector<csl::Tensor>&  impulsions,
            const std::vector<FeynmanRule>&  feynmanRules,
            mty::Expander::Options           t_options,
            bool                             t_ruleMode = false);

    private:

    Expander(const std::vector<csl::Tensor>&  vertices,
             const std::vector<QuantumField>& insertions,
             std::vector<csl::Tensor>&  impulsions,
             mty::Lagrangian&                 lagrangian,
             mty::Expander::Options           t_options,
             bool                             t_ruleMode);

    Expander(const std::vector<csl::Tensor>&  vertices,
             const std::vector<QuantumField>& insertions,
             std::vector<csl::Tensor>&  impulsions,
             const std::vector<FeynmanRule>&  feynmanRules,
             mty::Expander::Options           t_options,
             bool                             t_ruleMode);

    ~Expander(){};

    Amplitude expandLagrangian();

    void findNonZeroDiagrams();

    bool contractionPossible(
            size_t                     posVertex,
            std::vector<QuantumField> const& newFields,
            std::vector<bool>&         newPairing,
            int                        order,
            int                        maxOrder
            );

    void addVertexCarefully(
            size_t                            iTerm,
            size_t                            iVertex,
            std::vector<std::vector<size_t>>& numbers,
            std::vector<std::vector<bool>>&   newPairings,
            const std::vector<bool>&          pairing
            );

    private:

    bool                           ruleMode = false;
    Lagrangian*                    L;
    std::vector<FeynmanRule> const&feynmanRules;
    std::vector<QuantumField>      insertions;
    std::vector<csl::Tensor> impulsions;
    std::vector<csl::Tensor>       vertices;
    const int                      order;
    size_t                         maxInteractionLegs;

    Options                            options;
    std::vector<TermType>              effInteraction;
    std::vector<FeynmanRule const*>    effFeynRules;
    std::vector<std::vector<bool>>     insertionsPaired;
    std::vector<std::vector<size_t>>   numberVertices;
    std::map<std::vector<size_t>, bool> vertexMap;
};

} // End of namespace mty

#endif
