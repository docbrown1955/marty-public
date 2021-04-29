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

struct WilsonSet;
class Insertion;
class FeynOptions;
class Kinematics;
class Amplitude;

enum class OperatorBasis {

    Chiral,
    Standard,
    None,
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

    ///////////////////////////////////////////////////
    // Modifiers
    ///////////////////////////////////////////////////

    void filterFeynmanRules();

    ///////////////////////////////////////////////////
    // Computation utilities
    ///////////////////////////////////////////////////

    void computeFeynmanRules();

    mty::Amplitude computeAmplitude(
            int                         order,
            std::vector<mty::Insertion> insertions,
            FeynOptions                 options = {}
            );

    mty::Amplitude computeAmplitude(
            int                         order,
            std::vector<mty::Insertion> insertions,
            Kinematics           const &kinematics,
            FeynOptions                 options = {}
            );

    mty::Amplitude computeAmplitude(
            int                         order,
            std::vector<mty::Insertion> insertions,
            std::vector<int>     const &fermionOrder,
            FeynOptions                 options = {}
            );

    mty::Amplitude computeAmplitude(
            int                         order,
            std::vector<mty::Insertion> insertions,
            std::vector<int>     const &fermionOrder,
            Kinematics           const &kinematics,
            FeynOptions                 options = {}
            );

    /**
     * @brief Main method for the calculation of amplitude.
     *
     * @details This overload is the only one performing the calculation, all 
     * the other overloads are simply redirections to simplify the interface, 
     * setting default parameters etc.
     *
     * @param lagrangian    Set of interaction terms to use in the calculation.
     * @param insertions    External fields.
     * @param fermionOrder  Order for external fermions in the resulting amplitude.
     * @param kinematics    Kinematics of the process, (squared) momenta etc.
     * @param options       Options for the calculation.
     * @param rules         Set of feynman rules if the calculation must be done
     * using rules, otherwise empty vector.
     *
     * @return The result of the amplitude calculation.
     */
    mty::Amplitude computeAmplitude(
            std::vector<Lagrangian::TermType> &lagrangian,
            std::vector<Insertion>             insertions,
            std::vector<int>                   fermionOrder,
            Kinematics                  const &kinematics,
            FeynOptions                 const &options,
            std::vector<FeynmanRule const*>    rules = {}
            );

    mty::Amplitude computeAmplitude(
            std::vector<FeynmanRule const*> &feynRules,
            std::vector<Insertion>    const &insertions,
            std::vector<int>          const &fermionOrder,
            Kinematics                const &kinematics,
            FeynOptions               const &options
            );

    csl::Expr computeSquaredAmplitude(
            Amplitude const &ampl,
            bool             applyDegreesOfFreedomFactor = true
            );

    csl::Expr computeSquaredAmplitude(
            WilsonSet const &ampl,
            bool            applyDegreesOfFreedomFactor = true
            );

    WilsonSet getWilsonCoefficients(
            Amplitude const &ampl,
            csl::Expr         factor = CSL_1,
            OperatorBasis     basis = OperatorBasis::Chiral,
            bool              squaredAfter = false
            );

    WilsonSet computeWilsonCoefficients(
            int                           order,
            std::vector<Insertion> const &insertions,
            csl::Expr              const &factor,
            OperatorBasis    basis = OperatorBasis::Chiral
            );

    WilsonSet computeWilsonCoefficients_default(
            int                           order,
            std::vector<Insertion> const &insertions,
            csl::Expr              const &factor,
            OperatorBasis    basis = OperatorBasis::Chiral
            );

    WilsonSet computeWilsonCoefficients_2Fermions_1Vector(
            int                           order,
            std::vector<Insertion> const &insertions,
            csl::Expr              const &factor,
            OperatorBasis    basis = OperatorBasis::Chiral
            );

    WilsonSet computeWilsonCoefficients_4Fermions(
            int                           order,
            std::vector<Insertion> const &insertions,
            csl::Expr              const &factor,
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

protected:

    std::vector<mty::FeynmanRule> feynmanRules;
};

} // End of namespace mty

#endif /* MODE_H_INCLUDED */
