#pragma once

#include "model.h"
#include "feynmanRule.h"
#include "feynmanDiagram.h"
#include <algorithm>

namespace mty::filter {

    template<class ParticleIterator>
    auto disableParticles(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (InteractionTerm const &rule) {
                return std::any_of(begin(particles), end(particles),
                [&](Particle const &p) {
                    return rule.contains(p);
                });
            };
    }

    inline auto disableParticles(std::vector<mty::Particle> const &particles)
    {
        return disableParticles(begin(particles), end(particles));
    }

    inline auto disableParticles(std::initializer_list<mty::Particle> particles)
    {
        return disableParticles(begin(particles), end(particles));
    }

    inline auto disableParticles(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return disableParticles(model.getParticles(names));
    }

    inline auto disableParticle(mty::Particle const &particle)
    {
        return disableParticles({particle});
    }

    inline auto disableParticle(
            std::string_view  name,
            mty::Model const &model = *Model::current
            )
    {
        return disableParticle(model.getParticle(name));
    }

    template<class ParticleIterator>
    auto disableParticleCombination(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &rule) {
                return std::all_of(begin(particles), end(particles),
                [&](Particle const &p) {
                    return rule.contains(p);
                });
            };
    }

    inline auto disableParticleCombination(
            std::vector<mty::Particle> const &particles
            )
    {
        return disableParticleCombination(begin(particles), end(particles));
    }

    inline auto disableParticleCombination(
            std::initializer_list<mty::Particle> particles
            )
    {
        return disableParticleCombination(begin(particles), end(particles));
    }

    inline auto disableParticleCombination(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return disableParticleCombination(model.getParticles(names));
    }


    template<class ParticleIterator>
    auto forceAllParticles(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &diagram) {
                return std::any_of(begin(particles), end(particles), 
                [&](Particle const &p) {
                    return !diagram.contains(p);
                });
            };
    }

    inline auto forceAllParticles(std::vector<mty::Particle> const &particles)
    {
        return forceAllParticles(begin(particles), end(particles));
    }

    inline auto forceAllParticles(std::initializer_list<mty::Particle> particles)
    {
        return forceAllParticles(begin(particles), end(particles));
    }

    inline auto forceAllParticles(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return forceAllParticles(model.getParticles(names));
    }

    inline auto forceParticle(mty::Particle const &particle)
    {
        return forceAllParticles({particle});
    }

    inline auto forceParticle(
            std::string_view  name,
            mty::Model const &model = *Model::current
            )
    {
        return forceParticle(model.getParticle(name));
    }

    template<class ParticleIterator>
    auto forceAnyParticle(
            ParticleIterator first,
            ParticleIterator last
            )
    {
        return [particles = std::vector<mty::Particle>(first, last)]
            (FeynmanDiagram const &diagram) {
                return std::all_of(begin(particles), end(particles), 
                [&](Particle const &p) {
                    return !diagram.contains(p);
                });
            };
    }

    inline auto forceAnyParticle(std::vector<mty::Particle> const &particles)
    {
        return forceAnyParticle(begin(particles), end(particles));
    }

    inline auto forceAnyParticle(std::initializer_list<mty::Particle> particles)
    {
        return forceAnyParticle(begin(particles), end(particles));
    }

    inline auto forceAnyParticle(
            std::initializer_list<std::string_view> names,
            mty::Model                       const &model = *Model::current
            )
    {
        return forceAnyParticle(model.getParticles(names));
    }

}
