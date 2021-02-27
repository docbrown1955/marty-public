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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Contains the mty::Model class. It contains all objects in the theory.
 * In particular QuantumField objects, Gauge, Flavor, Particle...
 * \details All utilities for model building / computations are present in this
 * class. For the user, the most important functions are reimplemented in a 
 * class-independent manner in the file hepInterface.h. For the more advanced
 * user that wants to handle specific types of objects, methods of this class 
 * provide more features than the interface.
 */
#ifndef MODE_H_INCLUDED
#define MODE_H_INCLUDED

#include "modelBuilder.h"
#include "lagrangian.h"
#include "expander.h"
#include "group.h"
#include "feynmanRule.h"
#include "graph.h"
#include "gaugedGroup.h"
#include "doc_brown_link.h"

namespace mty::wick{
class Graph;
}

namespace mty {

struct Wilson;
class Insertion;

enum class OperatorBasis {

    Chiral,
    Standard,
};

/*!
 * \brief Contains all objects in the theory.
 * In particular QuantumField objects, Gauge, Flavor, Particle...
 * \details All utilities for model building / computations are present in this
 * class. For the user, the most important functions are reimplemented in a 
 * class-independent manner in the file hepInterface.h. For the more advanced
 * user that wants to handle specific types of objects, methods of this class 
 * provide more features than the interface.
 */
class Model: public ModelBuilder {

public:

    inline static Model *current = nullptr;

public:

    ///////////////////////////////////////////////////
    // Constructors
    ///////////////////////////////////////////////////

    template<class ...Args>
    explicit Model(Args &&...args)
        :ModelBuilder(std::forward<Args>(args)...) 
    {
        current = this;
    }

    ~Model() override = default;

    Model(Model&& other) = delete;
    Model& operator=(Model&& other) = delete;
    Model(Model const &other) = delete;
    Model &operator=(Model const &other) = delete;

    ///////////////////////////////////////////////////
    // Getters
    ///////////////////////////////////////////////////

    size_t getFeynmanRulesNumber() const;

    std::vector<mty::FeynmanRule> const &getFeynmanRules();

    mty::FeynmanRule& getFeynmanRule(size_t pos);
    mty::FeynmanRule const& getFeynmanRule(size_t pos) const;

    mty::Expander::Options const &getOptions() const;
    mty::Expander::Options &getOptions();

    ///////////////////////////////////////////////////
    // Modifiers
    ///////////////////////////////////////////////////

    void filterFeynmanRules();

    void resetExpanderOptions();

    ///////////////////////////////////////////////////
    // Computation utilities
    ///////////////////////////////////////////////////

    void computeFeynmanRules();

    virtual
    mty::Amplitude computeAmplitude(
            int                       order,
            std::vector<csl::Expr>  const &insertions,
            std::vector<csl::Tensor> &momenta,
            bool                      ruleMode = false
            );

    virtual
    mty::Amplitude computeAmplitude(
            int                      order,
            std::vector<csl::Expr> const &insertions,
            bool                     ruleMode = false
            );

    mty::Amplitude computeAmplitude(
            int                           order,
            std::vector<Insertion> const &insertions,
            bool                          ruleMode = false
            );

    csl::Expr computeSquaredAmplitude(
            Amplitude const &ampl,
            bool             averageOverIncomingSpins = true
            );

    csl::Expr computeSquaredAmplitude(
            int                       order,
            std::vector<csl::Expr>  const &insertions,
            std::vector<csl::Tensor> &momenta,
            bool                      averageOverIncomingSpins = true);

    std::vector<Wilson> getWilsonCoefficients(
            Amplitude const &ampl,
            csl::Expr        factor,
            OperatorBasis    basis = OperatorBasis::Chiral
            );

    friend
    std::ostream& operator<<(
            std::ostream &out,
            Model const  &obj
            );
protected:

    std::vector<mty::QuantumField> recoverQuantumInsertions(
            std::vector<csl::Expr> const &insertions
            );

    void initSquaredMomenta(
            std::vector<csl::Tensor>              &t_momenta,
            std::vector<mty::QuantumField>  const &fieldInsertions
            );

protected:

    std::vector<mty::FeynmanRule> feynmanRules;

    Expander::Options options;
};

} // End of namespace mty

#endif /* MODE_H_INCLUDED */
