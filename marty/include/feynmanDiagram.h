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

    class FeynmanDiagram {

    public:
        using diagram_t = std::shared_ptr<mty::wick::Graph>;
        template<typename T>
        using isNotParticle_t = typename std::enable_if_t<
            !std::is_same_v<mty::Particle, std::decay_t<T>>
                >;

        enum DiagramParticleType {
            External, Mediator, Loop, Any
        };

        FeynmanDiagram(
                mty::Model const &t_model,
                csl::Expr  const &t_expression,
                diagram_t  const &t_diagram
                );

        bool isZero() const {
            return CSL_0 == expression;
        }

        csl::Expr const &getExpression() const { return expression; }
        csl::Expr       &getExpression()       { return expression; }

        diagram_t const &getDiagram() const { return diagram; }

        std::vector<mty::Particle> const &getParticles(
                DiagramParticleType type = Any
                ) const;

        bool contains(
                mty::Particle const &part,
                DiagramParticleType  type = Any
                ) const;

        bool isExternal(mty::Particle const &particle) const {
            return contains(particle, External);
        }
        bool isMediator(mty::Particle const &particle) const {
            return contains(particle, Mediator);
        }
        bool isInLoop(mty::Particle const &particle) const {
            return contains(particle, Loop);
        }
        bool isInternal(mty::Particle const &particle) const {
            return isMediator(particle) || isInLoop(particle);
        }

        bool isTreeLevel() const {
            return getNLoops() == 0;
        }
        bool isLoop() const {
            return !isTreeLevel();
        }
        int getNLoops() const;

        bool isTopology(int topology) const;

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

        template<typename T, typename = isNotParticle_t<T>>
        bool isExternal(T &&particle) const {
            return isExternal(model->getParticle(std::forward<T>(particle)));
        }
        template<typename T, typename = isNotParticle_t<T>>
        bool isMediator(T &&particle) const {
            return isMediator(model->getParticle(std::forward<T>(particle)));
        }
        template<typename T, typename = isNotParticle_t<T>>
        bool isInLoop(T &&particle) const {
            return isInLoop(model->getParticle(std::forward<T>(particle)));
        }
        template<typename T, typename = isNotParticle_t<T>>
        bool isInternal(T &&particle) const {
            return isInternal(model->getParticle(std::forward<T>(particle)));
        }

    private:

        std::vector<mty::Particle> &getParticles(
                DiagramParticleType type = Any
                );

        void addParticle(
                mty::Particle const &part, 
                DiagramParticleType  type
                );

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

        void updateParticleData();

        void mergeParticles();

    private:

        Model const *model;
        csl::Expr    expression;
        diagram_t    diagram;
        int          cycleLength;
        std::vector<mty::Particle> allParticles;
        std::vector<mty::Particle> externalParticles;
        std::vector<mty::Particle> mediatorParticles;
        std::vector<mty::Particle> loopParticles;
    };

}
