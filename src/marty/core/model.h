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
 * \version 1.3
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

#include "doc_brown_link.h"
#include "expander.h"
#include "feynmanRule.h"
#include "gaugedGroup.h"
#include "graph.h"
#include "group.h"
#include "kinematics.h"
#include "lagrangian.h"
#include "modelBuilder.h"
#include "operatorBasis.h"

namespace mty::wick {
class Graph;
}

namespace mty {

struct WilsonSet;
class Insertion;
class FeynOptions;
class Kinematics;
class Amplitude;

enum class DecompositionMode { Minimal, BasisProjection, Matching };

/*!
 * \brief Contains all objects in the theory.
 * In particular QuantumField objects, Gauge, Flavor, Particle...
 * \details All utilities for model building / computations are present in this
 * class. For the user, the most important functions are reimplemented in a
 * class-independent manner in the file hepInterface.h. For the more advanced
 * user that wants to handle specific types of objects, methods of this class
 * provide more features than the interface.
 */
class Model : public ModelBuilder {

  public:
    inline static Model *current = nullptr;

  public:
    ///////////////////////////////////////////////////
    // Constructors
    ///////////////////////////////////////////////////

    template <class... Args>
    explicit Model(Args &&... args) : ModelBuilder(std::forward<Args>(args)...)
    {
        current = this;
    }

    ~Model() override = default;

    Model(Model &&other) = delete;
    Model &operator=(Model &&other) = delete;
    Model(Model const &other)       = delete;
    Model &operator=(Model const &other) = delete;

    ///////////////////////////////////////////////////
    // Getters
    ///////////////////////////////////////////////////

    size_t getFeynmanRulesNumber() const;

    std::vector<mty::FeynmanRule> const &getFeynmanRules();

    mty::FeynmanRule &      getFeynmanRule(size_t pos);
    mty::FeynmanRule const &getFeynmanRule(size_t pos) const;

    ///////////////////////////////////////////////////
    // Modifiers
    ///////////////////////////////////////////////////

    void filterFeynmanRules();

    ///////////////////////////////////////////////////
    // Computation utilities
    ///////////////////////////////////////////////////

    void computeFeynmanRules();

    mty::Amplitude computeAmplitude(int                         order,
                                    std::vector<mty::Insertion> insertions,
                                    FeynOptions &               options);

    mty::Amplitude computeAmplitude(int                         order,
                                    std::vector<mty::Insertion> insertions,
                                    Kinematics const &          kinematics,
                                    FeynOptions &               options);

    mty::Amplitude computeAmplitude(int                         order,
                                    std::vector<mty::Insertion> insertions);

    mty::Amplitude computeAmplitude(int                         order,
                                    std::vector<mty::Insertion> insertions,
                                    Kinematics const &          kinematics);

    mty::Amplitude
    computePartialAmplitude(int                         order,
                            std::vector<mty::Insertion> insertions,
                            FeynOptions                 options = {});

    mty::Amplitude
    computePartialAmplitude(int                         order,
                            std::vector<mty::Insertion> insertions,
                            Kinematics const &          kinematics,
                            FeynOptions                 options = {});

    /**
     * @brief Main method for the calculation of amplitude.
     *
     * @details This overload is the only one performing the calculation, all
     * the other overloads are simply redirections to simplify the interface,
     * setting default parameters etc.
     *
     * @param lagrangian    Set of interaction terms to use in the calculation.
     * @param insertions    External fields.
     * @param fermionOrder  Order for external fermions in the resulting
     * amplitude.
     * @param kinematics    Kinematics of the process, (squared) momenta etc.
     * @param options       Options for the calculation.
     * @param rules         Set of feynman rules if the calculation must be
     * done using rules, otherwise empty vector.
     *
     * @return The result of the amplitude calculation.
     */
    mty::Amplitude
    computeAmplitude(std::vector<Lagrangian::TermType> &lagrangian,
                     std::vector<Insertion>             insertions,
                     Kinematics const &                 kinematics,
                     FeynOptions                        options,
                     std::vector<FeynmanRule const *>   rules = {});

    mty::Amplitude
    computeAmplitude(std::vector<FeynmanRule const *> &feynRules,
                     std::vector<Insertion> const &    insertions,
                     Kinematics const &                kinematics,
                     FeynOptions const &               options);

    csl::Expr computeSquaredAmplitude(Amplitude const &ampl,
                                      bool applyDegreesOfFreedomFactor = true);

    csl::Expr computeSquaredAmplitude(WilsonSet const &ampl,
                                      bool applyDegreesOfFreedomFactor = true);

    csl::Expr computeSquaredAmplitude(Amplitude const &amplL,
                                      Amplitude const &amplR,
                                      bool applyDegreesOfFreedomFactor = true);

    csl::Expr computeSquaredAmplitude(WilsonSet const &amplL,
                                      WilsonSet const &amplR,
                                      bool applyDegreesOfFreedomFactor = true);

    csl::Expr
    squaredAmplitudeToPartialWidth(csl::Expr const &squaredAmplitude,
                                   std::vector<Insertion> const &insertions,
                                   mty::Amplitude const &initialAmplitude,
                                   bool applyMassCondition = true);

    csl::Expr computeWidth(int                   orderLeft,
                           int                   orderRight,
                           mty::Insertion const &particle,
                           mty::FeynOptions      options = {});

    csl::Expr computeWidth(int                   order,
                           mty::Insertion const &particle,
                           mty::FeynOptions      options = {});

    void computeModelWidths(int                                orderLeft,
                            int                                orderRight,
                            std::vector<mty::Insertion> const &particles,
                            mty::FeynOptions                   options = {});

    void computeModelWidths(int              orderLeft,
                            int              orderRight,
                            mty::FeynOptions options = {});

    void computeModelWidths(int                                order,
                            std::vector<mty::Insertion> const &particles,
                            mty::FeynOptions                   options = {});

    void computeModelWidths(int order, mty::FeynOptions options = {});

    WilsonSet getWilsonCoefficients(Amplitude const &  ampl,
                                    FeynOptions const &feynOptions,
                                    DecompositionMode  mode
                                    = DecompositionMode::Matching);

    WilsonSet getWilsonCoefficients(Amplitude const & ampl,
                                    DecompositionMode mode
                                    = DecompositionMode::Matching);

    WilsonSet
    computeWilsonCoefficients(int                           order,
                              std::vector<Insertion> const &insertions,
                              FeynOptions                   feynOptions = {},
                              bool disableFermionOrdering = false);

    WilsonSet
    computeWilsonCoefficients_default(int                           order,
                                      std::vector<Insertion> const &insertions,
                                      FeynOptions const &feynOptions = {});

    WilsonSet computeWilsonCoefficients_2Fermions_1Vector(
        std::vector<Insertion> const &insertions,
        FeynOptions const &           feynOptions = {});

    WilsonSet computeWilsonBoxes_4Fermions(Kinematics const &kinematics,
                                           FeynOptions       feynOptions = {});

    WilsonSet computeSingleWilsonPenguin_4Fermions(
        Kinematics const &               kinematics,
        std::pair<size_t, size_t> const &treeCoupling,
        std::pair<size_t, size_t> const &loopCoupling,
        Insertion const &                mediator,
        FeynOptions                      feynOptions = {});

    WilsonSet computeWilsonPenguins_4Fermions(Kinematics const &kinematics,
                                              FeynOptions feynOptions = {});

    WilsonSet
    computeWilsonCoefficients_4Fermions(std::vector<Insertion> insertions,
                                        FeynOptions feynOptions = {});

    Amplitude connectAmplitudes(Amplitude const &  M1,
                                Amplitude const &  M2,
                                FeynOptions const &options = {});

    static void projectOnBasis(csl::Expr &expr, OperatorBasis basis);

    friend std::ostream &operator<<(std::ostream &out, Model const &obj);

  protected:
    std::vector<mty::QuantumField>
    recoverQuantumInsertions(std::vector<csl::Expr> const &insertions);

    struct KinematicLink {
        Kinematics                      kinematics;
        csl::Tensor                     pL;
        csl::Tensor                     pR;
        std::pair<csl::Expr, csl::Expr> pL_replacement;
        std::pair<csl::Expr, csl::Expr> pR_replacement;
        mty::Particle                   mediator;

        /**
         * @return 0 if **expr** is not a mediator.
         * @return -1 if **expr** is the left mediator.
         * @return 1 if **expr** is the right mediator.
         */
        int isMediator(csl::Expr const &expr) const;
    };

    KinematicLink connectKinematics(Amplitude &M1, Amplitude &M2) const;

    std::pair<csl::Expr, csl::Expr>
    getMomentumReplacement(Amplitude const &M, size_t replacedMomentum) const;

    void replaceMomentumForLink(
        Amplitude &                            M,
        std::pair<csl::Expr, csl::Expr> const &pReplacement) const;

    bool mediatorToPropagator(csl::Expr &          prod,
                              KinematicLink const &link) const;

    csl::Expr connectMediator(csl::Expr const &    M1,
                              csl::Expr const &    M2,
                              KinematicLink const &link) const;

  protected:
    std::vector<mty::FeynmanRule> feynmanRules;
};

int operatorDegeneracy(std::vector<mty::Insertion> const &insertions);

int matchingFermionSign(std::vector<int> fermionOrder);

int fermionSign(std::vector<Insertion> const &model,
                std::vector<Insertion>        order);

} // End of namespace mty

#endif /* MODE_H_INCLUDED */
