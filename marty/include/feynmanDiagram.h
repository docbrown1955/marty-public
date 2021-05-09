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
/**
 * @file feynmanDiagram.h
 * @brief Contains the mty::FeynmanDiagram class.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-05
 */

#pragma once

#include <csl.h>
#include "model.h"
#include "topology.h"

namespace mty {

    namespace wick {
        class Graph;
    }

    class Model;
    class Particle;

    /**
     * @brief Class containing a Feynman diagram, symbolic expression and graph
     * included.
     *
     * @details The purpose of this class is to provide a container for the 
     * result of a diagram calculation, and a simple interface to obtain 
     * relevant information about the process. When an amplitude calculation is
     * performed, the result contains a range of Feynman diagrams, each one (an
     * instance of this class) containing as said above the graph that can be 
     * displayed in GRAFED, and the fully simplified symbolic expression, main
     * result of MARTY.
     */
    class FeynmanDiagram {

    public:

        /**
         * @brief Type definition for the type used to store graphs.
         */
        using diagram_t = std::shared_ptr<mty::wick::Graph>;

        /**
         * @brief Helper type definition ensuring that the type T is not a 
         * mty::Particle.
         *
         * @tparam T 
         */
        template<typename T>
        using isNotParticle_t = typename std::enable_if_t<
                !std::is_same_v<mty::Particle, std::decay_t<T>>
                >;

        /**
         * @brief Possible types of particles in a diagram.
         */
        enum DiagramParticleType {

            External, /*!< External particles. */
            Mediator, /*!< Internal particles outside loops. */
            Loop,     /*!< Internal particles in loops. */
            Any       /*!< Any type of particle. */ 
        };

        FeynmanDiagram(
                mty::Model const &t_model,
                csl::Expr  const &t_expression,
                diagram_t  const &t_diagram
                );

        /**
         * @brief Tells if the diagram vanishes.
         *
         * @note A bare output of MARTY should in principle not contain null
         * diagrams.
         *
         * @return \b True  if the diagram's expression is equal to zero.
         * @return \b False else.
         */
        bool isZero() const {
            return CSL_0 == expression;
        }

        /**
         * @brief Returns the diagrams's expression as a const reference.
         *
         * @return #expression
         */
        csl::Expr const &getExpression() const { return expression; }

        /**
         * @brief Returns the diagrams's expression as a reference.
         *
         * @return #expression
         */
        csl::Expr       &getExpression()       { return expression; }

        /**
         * @brief Returns the diagram's graph as a reference.
         *
         * @return #diagram
         */
        diagram_t &getDiagram() { return diagram; }

        /**
         * @brief Returns the diagram's graph as a const reference.
         *
         * @return #diagram
         */
        diagram_t const &getDiagram() const { return diagram; }

        /**
         * @brief Returns the set of particles corresponding to a given type.
         *
         * @param type Type of the particle range to return. The default value
         * is #Any, meaning all the particles in the diagram.
         *
         * @return #externalParticles if type is #External.
         * @return #mediatorParticles if type is #Mediator.
         * @return #loopParticles     if type is #Loop.
         * @return #allParticles      if type is #Any.
         */
        std::vector<mty::Particle> const &getParticles(
                DiagramParticleType type = Any
                ) const;

        /**
         * @brief Tells if one particle is contained in the diagram for a given
         * type.
         *
         * @details This function is a general one, that is called by several
         * specialization: isExternal(), isMediator(), isInLoop(), isInternal().
         * Given a particle and a type (DiagramParticleType), this function 
         * returns true if the particle is found is the range of particles of 
         * the same type in the diagram.
         * This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      bool containsZ = diagram.contains("Z");
         *      // shortcut for the other overload given a model "model":
         *      // bool containsZ = diagram.contains(model.getParticle("Z"));
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         *
         * @param part Particle to search in the diagram.
         * @param type Type that the particle must be. The default is #Any 
         * meaning that this function searches in the whole diagram (all 
         * particles, external and internal ones).
         *
         * @return \b True if the particle has been found.
         * @return \b False else.
         */
        bool contains(
                mty::Particle const &part,
                DiagramParticleType  type = Any
                ) const;

        /**
         * @brief Tells if a particle is an external particle in the diagram.
         *
         * @param particle Particle to test.
         *
         * @return \b True if one external leg corresponds to \b particle.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        bool isExternal(mty::Particle const &particle) const {
            return contains(particle, External);
        }
        /**
         * @brief Tells if a particle is a mediator particle in the diagram.
         *
         * @param particle Particle to test.
         *
         * @return \b True if one mediator corresponds to \b particle.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        bool isMediator(mty::Particle const &particle) const {
            return contains(particle, Mediator);
        }
        /**
         * @brief Tells if a particle is a looped particle in the diagram.
         *
         * @param particle Particle to test.
         *
         * @return \b True if one loop particle corresponds to \b particle.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        bool isInLoop(mty::Particle const &particle) const {
            return contains(particle, Loop);
        }
        /**
         * @brief Tells if a particle is an internal particle in the diagram.
         *
         * @param particle Particle to test.
         *
         * @return \b True if one mediator or looped particle corresponds to 
         * \b particle.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        bool isInternal(mty::Particle const &particle) const {
            return isMediator(particle) || isInLoop(particle);
        }

        /**
         * @return \b True if the diagram is at the tree level.
         * @return \b False else.
         */
        bool isTreeLevel() const {
            return getNLoops() == 0;
        }

        /**
         * @return \b True if the diagram has at least one loop.
         * @return \b False else.
         */
        bool isLoop() const {
            return !isTreeLevel();
        }

        /**
         * @return The number of loops of the diagram.
         */
        int getNLoops() const;

        /**
         * @brief Tells if the diagram is of a given topology.
         *
         * @details This function takes as a parameter an integer, not a 
         * mty::Topology::Type object to allow combinations of topologies. A 
         * single mty::Topology::Type can also be given as it is implicitly
         * convertible into an int. This interface allows then the following
         * uses:
         * \code
         *      bool isTadpole = diagram.isTopology(mty::Topology::Tadpole);
         *      bool isTriangleOrBox = diagram.isTopology(mty::Topology::Triangle | mty::Topology::Box);
         *      // Finally, in the limit of at most 5 external legs :
         *      bool alwaysTrue = diagram.isTopology(mty::Topology::Any);
         * \endcode
         *
         * @param topology Integer representing one or several topologies.
         *
         * @return \b True if the diagram's topology is included in \b topology.
         * @return \b False else.
         */
        bool isTopology(int topology) const;

        /**
         * @brief Tells if one particle is contained in the diagram for a given
         * type.
         *
         * @details This function is a general one, that is called by several
         * specialization: isExternal(), isMediator(), isInLoop(), isInternal().
         * Given a particle and a type (DiagramParticleType), this function 
         * returns true if the particle is found is the range of particles of 
         * the same type in the diagram.
         *
         * @param part Particle to search in the diagram.
         * @param type Type that the particle must be. The default is #Any 
         * meaning that this function searches in the whole diagram (all 
         * particles).
         *
         * @return \b True if the particle has been found.
         * @return \b False else.
         */
        template<typename T, typename = isNotParticle_t<T>>
        bool contains(
                T                 &&part,
                DiagramParticleType type = Any
                ) const {
            return contains(
                    model->getParticle(std::forward<T>(part)),
                    type
                    );
        }

        /**
         * @brief Tells if a particle is an external particle in the diagram.
         *
         * @details This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      bool containsZ = diagram.isExternal("Z");
         *      // shortcut for the other overload given a model "model":
         *      // bool containsZ = diagram.isExternal(model.getParticle("Z"));
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         * @param particle Particle to search for.
         *
         * @return \b True if \b particle represents an external particle 
         * (mediator or looped particle) in the diagram.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        template<typename T, typename = isNotParticle_t<T>>
        bool isExternal(T &&particle) const {
            return isExternal(model->getParticle(std::forward<T>(particle)));
        }
        /**
         * @brief Tells if a particle is a mediator particle in the diagram.
         *
         * @details This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      bool containsZ = diagram.isMediator("Z");
         *      // shortcut for the other overload given a model "model":
         *      // bool containsZ = diagram.isMediator(model.getParticle("Z"));
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         * @param particle Particle to search for.
         *
         * @return \b True if \b particle represents a mediator particle 
         * (mediator or looped particle) in the diagram.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        template<typename T, typename = isNotParticle_t<T>>
        bool isMediator(T &&particle) const {
            return isMediator(model->getParticle(std::forward<T>(particle)));
        }
        /**
         * @brief Tells if a particle is a looped particle in the diagram.
         *
         * @details This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      bool containsZ = diagram.isInLoop("Z");
         *      // shortcut for the other overload given a model "model":
         *      // bool containsZ = diagram.isInLoop(model.getParticle("Z"));
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         * @param particle Particle to search for.
         *
         * @return \b True if \b particle represents a looped particle 
         * (mediator or looped particle) in the diagram.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        template<typename T, typename = isNotParticle_t<T>>
        bool isInLoop(T &&particle) const {
            return isInLoop(model->getParticle(std::forward<T>(particle)));
        }
        /**
         * @brief Tells if a particle is an internal particle in the diagram.
         *
         * @details This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      bool containsZ = diagram.isInternal("Z");
         *      // shortcut for the other overload given a model "model":
         *      // bool containsZ = diagram.isInternal(model.getParticle("Z"));
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         * @param particle Particle to search for.
         *
         * @return \b True if \b particle represents an internal particle 
         * (mediator or looped particle) in the diagram.
         * @return \b False else.
         *
         * @sa contains(), DiagramParticleType
         */
        template<typename T, typename = isNotParticle_t<T>>
        bool isInternal(T &&particle) const {
            return isInternal(model->getParticle(std::forward<T>(particle)));
        }

    private:

        /**
         * @brief Private non-const overload for getParticles().
         *
         * @param type Type of the particles from which the range must be 
         * returned.
         *
         * @return The range of particles of type \b type.
         */
        std::vector<mty::Particle> &getParticles(
                DiagramParticleType type = Any
                );

        /**
         * @brief Adds a particle to a given range (depending its type).
         *
         * @param part Particle to add to the diagram.
         * @param type Type determining the range in which the particle is 
         * inserted (#externalParticles, #loopParticles, #mediatorParticles).
         */
        void addParticle(
                mty::Particle const &part, 
                DiagramParticleType  type
                );

        /**
         * @brief Template overload for non-mty::Particle objects.
         *
         * @details This function can take any type that can be forwarded to
         * mty::Model::getParticle(). In particular, it may take char arrays
         * as parameter knowing the name of the particle:
         * \code
         *      addParticle("Z", someType);
         *      // shortcut for the other overload given a model "model":
         *      // addParticle(model.getParticle("Z"), someType);
         * \endcode
         *
         * @tparam T Type that can be converted to a particle by a mty::Model.
         * @tparam Constraint : T must not be a mty::Particle.
         * @param part Particle to add to the diagram.
         * @param type Type determining the range in which the particle is 
         */
        template<typename T, typename = isNotParticle_t<T>>
        void addParticle(
                T                 &&part, 
                DiagramParticleType type
                )
        {
            addParticle(
                    model->getParticle(std::forward<T>(part)),
                    type
                    );
        }

        /**
         * @brief Gathers particles from the graph given in initialization.
         */
        void updateParticleData();

        /**
         * @brief Merges all particles found by updateParticleData() in the 
         * member #allParticles.
         */
        void mergeParticles();

    private:

        /**
         * @brief Pointer to the model in which the calculation has been done.
         */
        Model const *model;

        /**
         * @brief Expression result of the calculation.
         */
        csl::Expr    expression;

        /**
         * @brief Graph to forward to GRAFED to display it on screen.
         */
        diagram_t    diagram;

        /**
         * @brief Length of the cycle (momentum integral) if the is one.
         */
        int          cycleLength;

        /**
         * @brief List of all particles in the process.
         */
        std::vector<mty::Particle> allParticles;

        /**
         * @brief List of external particles in the process.
         */
        std::vector<mty::Particle> externalParticles;

        /**
         * @brief List of mediator particles in the process.
         *
         * @details Mediator particles are defined here as internal particles
         * that do not appear in loops. An internal particle is then either
         * a mediator or a looped particle.
         *
         * @sa #loopParticles
         */
        std::vector<mty::Particle> mediatorParticles;

        /**
         * @brief List of particles in loops in the process.
         *
         * @sa #mediatorParticles
         */
        std::vector<mty::Particle> loopParticles;
    };

}
