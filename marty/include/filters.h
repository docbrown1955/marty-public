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
 * @file filters.h
 *
 * @brief Contains built-in filters (Lagrangian and diagram filters) for the 
 * mty::FeynOptions object.
 *
 * @details All (possibly template) functions in this file return functions.
 * The return values of these functions can then be passed to the 
 * mty::FeynOptions::addFilter() and mty::FeynOptions::addFilters() member
 * functions directly. Filters can be fully general, the built-in ones are 
 * simply useful particular cases to avoid writing always the same functions.
 * Calculating an amplitude forcing the top quark and disabling the Z boson 
 * for example would look like
 * \code
 *      mty::FeynOptions options;
 *      options.addFilters(
 *          mty::filter::disableParticle("Z"),
 *          mty::filter::forceParticle("t")
 *      );
 *      auto res = model.computeAmplitude(order, insertions, options);
 * \endcode
 * with appropriate model, order and insertions. In case those built-in filters
 * are not enough one can write a custom filter following the definition of 
 * mty::FeynOptions::LagrangianFilter and mty::FeynOptions::DiagramFilter, using the documentation of 
 * mty::InteractionTerm and mty::FeynmanDiagram objects. Writing the above 
 * filters by hand could look like (using C++ lambda functions):
 * \code
 *      mty::FeynOptions options;
 *      Particle Z = model.getParticle("Z");
 *      Particle t = model.getParticle("t");
 *      options.addFilters(
 *          [&](mty::InteractionTerm const &term) { // disable Z, mty::FeynOptions::LagrangianFilter
 *              return !term.contains(Z); // keep if Z absent
 *          },
 *          [&](mty::FeynmanDiagram const &diagram) { // force t, mty::FeynOptions::DiagramFilter
 *              return diagram.contains(t); // keep if t present
 *          }
 *      );
 *      auto res = model.computeAmplitude(order, insertions, options);
 * \endcode
 * In the example above, disabling the Z particle could also be a 
 * mty::FeynOptions::DiagramFilter but this would imply unnecessary calculations because 
 * vertices can be filtered before searching for any diagram containing it. 
 * However forcing the top quark must be a mty::FeynOptions::DiagramFilter because one cannot
 * remove vertices that contain the top quark, the condition is that the whole
 * diagram must contain at least one top quark.
 *
 * @note To enable the calculation without filtering but select specific 
 * contributions at the end, see mty::Amplitude::filterOut() instead.
 *
 * @sa mty::InteractionTerm, mty::FeynmanDiagram, mty::FeynOptions, 
 * mty::FeynOptions::DiagramFilter, mty::FeynOptions::LagrangianFilter,
 * mty::FeynOptions::addFilter(), mty::FeynOptions::addFilters().
 *
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-05
 */
#pragma once

#include "model.h"
#include "feynmanRule.h"
#include "feynmanDiagram.h"
#include <algorithm>

/**
 * @brief Namespace containing built-in filters in MARTY.
 */
namespace mty::filter {

    /**
     * @brief Filter disabling a range of particles provided begin and end 
     * iterators for the given range.
     *
     * @details This function does not accept ranges of other objects than 
     * mty::Particle. To get filters directly from particle names, see the other
     * overloads. This function is the only one actually doing the job, the 
     * other overloads are delegating to it.
     *
     * @tparam ParticleIterator Iterator type, must point to mty::Particle 
     * objects.
     * @param first Begin iterator.
     * @param last  End iterator
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out all 
     * particles contained between \b first and \b last.
     */
    template<class ParticleIterator>
    auto disableParticles(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (InteractionTerm const &rule) {
                return std::all_of(begin(particles), end(particles),
                [&](Particle const &p) {
                    return !rule.containsWeakly(p);
                });
            };
    }

    /**
     * @brief Filter disabling a range of particles in a std::vector.
     *
     * @param particles Range of particles to disable.
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out all 
     * particles contained in the \b particles range.
     */
    inline auto disableParticles(std::vector<mty::Particle> const &particles)
    {
        return disableParticles(begin(particles), end(particles));
    }

    /**
     * @brief Filter disabling a range of particles in an initializer_list.
     *
     * @param particles Range of particles to disable.
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out all 
     * particles contained in the \b particles range.
     */
    inline auto disableParticles(std::initializer_list<mty::Particle> particles)
    {
        return disableParticles(begin(particles), end(particles));
    }

    /**
     * @brief Filter disabling a range of particles from their names
     * in an initializer_list.
     *
     * @param names Range of particle names to disable.
     * @param model Model in which particles are taken, default is the 
     * current mty::Model.
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out all 
     * particles contained in the \b particles range.
     */
    inline auto disableParticles(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return disableParticles(model.getParticles(names));
    }

    /**
     * @brief Filter disabling one particle.
     *
     * @param particle Particle to disable.
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out the
     * particle \b particle.
     */
    inline auto disableParticle(mty::Particle const &particle)
    {
        return disableParticles({particle});
    }

    /**
     * @brief Filter disabling one particle given its name.
     *
     * @param name  Name of the particle to disable.
     * @param model Model in which the particle is taken, default is the 
     * current mty::Model.
     *
     * @return A mty::FeynOptions::LagrangianFilter filter filtering out the
     * particle named \b particle in the model \b model.
     */
    inline auto disableParticle(
            std::string_view  name,
            mty::Model const &model = *Model::current
            )
    {
        return disableParticle(model.getParticle(name));
    }

    /**
     * @brief Filter disabling a particular combination of particles.
     *
     * @details This function does not accept ranges of other objects than 
     * mty::Particle. To get filters directly from particle names, see the other
     * overloads. This function is the only one actually doing the job, the 
     * other overloads are delegating to it.
     *
     * @tparam ParticleIterator Iterator type, must point to mty::Particle 
     * objects.
     * @param first Begin iterator.
     * @param last  End iterator.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams 
     * containing \b all particles contained in the range.
     */
    template<class ParticleIterator>
    auto disableParticleCombination(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &rule) {
                return std::any_of(begin(particles), end(particles),
                [&](Particle const &p) {
                    return !rule.contains(p);
                });
            };
    }

    /**
     * @brief Filter disabling a particular combination of particles from a 
     * std::vector.
     *
     * @param particles Range of particles, combination to disable in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams 
     * containing \b all particles contained in the range.
     */
    inline auto disableParticleCombination(
            std::vector<mty::Particle> const &particles
            )
    {
        return disableParticleCombination(begin(particles), end(particles));
    }

    /**
     * @brief Filter disabling a particular combination of particles from a 
     * std::initializer_list.
     *
     * @param particles Range of particles, combination to disable in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams 
     * containing \b all particles contained in the range.
     */
    inline auto disableParticleCombination(
            std::initializer_list<mty::Particle> particles
            )
    {
        return disableParticleCombination(begin(particles), end(particles));
    }

    /**
     * @brief Filter disabling a particular combination of particle from their
     * names.
     *
     * @param names Range of particle names, combination to disable in 
     * diagrams.
     * @param model Model in which the particles are taken, default is the 
     * current mty::Model.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams 
     * containing \b all particles contained in the range.
     */
    inline auto disableParticleCombination(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return disableParticleCombination(model.getParticles(names));
    }


    /**
     * @brief Filter forcing a particular combination of particles.
     *
     * @details This function does not accept ranges of other objects than 
     * mty::Particle. To get filters directly from particle names, see the other
     * overloads. This function is the only one actually doing the job, the 
     * other overloads are delegating to it.
     *
     * @tparam ParticleIterator Iterator type, must point to mty::Particle
     * objects.
     * @param first Begin iterator.
     * @param last  End   iterator.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain all the particles given in the range.
     */
    template<class ParticleIterator>
    auto forceParticleCombination(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &diagram) {
                return std::all_of(begin(particles), end(particles), 
                [&](Particle const &p) {
                    return diagram.contains(p);
                });
            };
    }

    /**
     * @brief Filter forcing a particular combination of particles given in a
     * std::vector.
     *
     * @param particles Range of particles, combination to force in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain all the particles given in the range.
     */
    inline auto forceParticleCombination(
            std::vector<mty::Particle> const &particles
            )
    {
        return forceParticleCombination(begin(particles), end(particles));
    }

    /**
     * @brief Filter forcing a particular combination of particles given in a
     * std::initializer_list.
     *
     * @param particles Range of particles, combination to force in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain all the particles given in the range.
     */
    inline auto forceParticleCombination(
            std::initializer_list<mty::Particle> particles
            )
    {
        return forceParticleCombination(begin(particles), end(particles));
    }

    /**
     * @brief Filter forcing a particular combination of particles given their
     * names.
     *
     * @param names Range of particle names, combination to force in diagrams.
     * @param model Model in which particles are taken, default is 
     * mty::Model::current.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain all the particles given in the range.
     */
    inline auto forceParticleCombination(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return forceParticleCombination(model.getParticles(names));
    }

    /**
     * @brief Filter forcing one particle in diagrams.
     *
     * @param particle Particle to force in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out all diagrams that
     * do not contain the given particle.
     */
    inline auto forceParticle(mty::Particle const &particle)
    {
        return forceParticleCombination({particle});
    }

    /**
     * @brief Filter forcing one particle in diagrams given its name.
     *
     * @param name  Particle name to force in diagrams.
     * @param model Model in which the particle is taken, default is 
     * mty::Model::current.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out all diagrams that
     * do not contain the given particle.
     */
    inline auto forceParticle(
            std::string_view  name,
            mty::Model const &model = *Model::current
            )
    {
        return forceParticle(model.getParticle(name));
    }

    /**
     * @brief Filter forcing particles in diagrams.
     *
     * @details This function does not accept ranges of other objects than 
     * mty::Particle. To get filters directly from particle names, see the other
     * overloads. This function is the only one actually doing the job, the 
     * other overloads are delegating to it.
     *
     * @tparam ParticleIterator Iterator type, must point to mty::Particle
     * objects.
     * @param first Begin iterator.
     * @param last  End   iterator.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain any of the particles given in the range.
     */
    template<class ParticleIterator>
    auto forceParticles(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &diagram) {
                return std::any_of(begin(particles), end(particles), 
                [&](Particle const &p) {
                    return diagram.contains(p);
                });
            };
    }

    /**
     * @brief Filter forcing particles in diagrams given in a std::vector.
     *
     * @param particles Particle range to force in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain any of the particles given in the range.
     */
    inline auto forceParticles(std::vector<mty::Particle> const &particles)
    {
        return forceParticles(begin(particles), end(particles));
    }

    /**
     * @brief Filter forcing particles in diagrams given in a
     * std::initializer_list.
     *
     * @param particles Particle range to force in diagrams.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain any of the particles given in the range.
     */
    inline auto forceParticles(std::initializer_list<mty::Particle> particles)
    {
        return forceParticles(begin(particles), end(particles));
    }

    /**
     * @brief Filter forcing particles in diagrams given their names.
     *
     * @param names Range of particle names to force in diagrams.
     * @param model Model in which particles are taken, default is 
     * mty::Model::current.
     *
     * @return A mty::FeynOptions::DiagramFilter filtering out diagrams that do 
     * not contain any of the particles given in the range.
     */
    inline auto forceParticles(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return forceParticles(model.getParticles(names));
    }

}
