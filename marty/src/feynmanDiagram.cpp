#include "feynmanDiagram.h"
#include "quantumField.h"
#include "expander.h"

namespace mty {

    FeynmanDiagram::FeynmanDiagram(
            mty::Model const &t_model,
            csl::Expr  const &t_expression,
            diagram_t  const &t_diagram
            )
        :model(&t_model),
        expression(csl::DeepCopy(t_expression)),
        diagram(t_diagram)
    {
        updateParticleData();
    }

    std::vector<mty::Particle> const &FeynmanDiagram::getParticles(
            DiagramParticleType type
            ) const
    {
        switch(type) {
            case External: return externalParticles;
            case Mediator: return mediatorParticles;
            case Loop:     return loopParticles;
            default:       return allParticles;
        }
    }

    std::vector<mty::Particle> &FeynmanDiagram::getParticles(
            DiagramParticleType type
            )
    {
        return const_cast<std::vector<mty::Particle>&>(
                const_cast<mty::FeynmanDiagram const*>(this)->getParticles(type)
                );
    }

    bool FeynmanDiagram::contains(
            mty::Particle const &part,
            DiagramParticleType  type
            ) const
    {
        std::vector<mty::Particle> const &container = getParticles(type);
        return std::find_if(begin(container), end(container), 
                [&](mty::Particle const &p) {
                    return part->contains(p.get());
                }) != end(container);
    }

    int FeynmanDiagram::getNLoops() const
    {
        return diagram->getNLoops();
    }

    bool FeynmanDiagram::isTopology(int topology) const 
    {
        return (topology & Topology::legsToTopology(cycleLength)) != 0;
    }

    void FeynmanDiagram::addParticle(
            mty::Particle const &part,
            DiagramParticleType  type
            )
    {
        if (contains(part, type))
            return;
        getParticles(type).push_back(part);
    }

    void FeynmanDiagram::updateParticleData()
    {
        auto const &vertices = diagram->getVertices();
        std::vector<csl::Tensor> points(vertices.size());
        auto first = vertices[0][0];
        int path = mty::wick::Graph::walk(
                points.begin(), points.begin(), first, vertices
                );
        cycleLength = (path == -1) ? 0 : path;
        std::for_each(begin(vertices), end(vertices), 
        [&](mty::wick::Vertex const &vertex) {
            if (vertex.isExternal()) {
                // External Field
                addParticle(*vertex[0]->field, External);
            }
            else {
                // Internal Fields
                std::for_each(begin(vertex), end(vertex), 
                [&](std::shared_ptr<mty::wick::Node> const &node) {
                    csl::Tensor pA = node->field->getPoint();
                    csl::Tensor pB = node->partner.lock()->field->getPoint();
                    auto posA = std::find(begin(points), end(points), pA);
                    auto posB = std::find(begin(points), end(points), pB);
                    auto type = (posA == end(points) || posB == end(points)) ? 
                        Mediator : Loop;
                    addParticle(*node->field, type);
                });
            }
        });
        mergeParticles();
    }

    void FeynmanDiagram::mergeParticles()
    {
        allParticles.reserve(
                externalParticles.size()
                + mediatorParticles.size()
                + loopParticles.size()
                );
        allParticles.insert(
                end(allParticles),
                begin(externalParticles),
                end(externalParticles));
        allParticles.insert(
                end(allParticles),
                begin(mediatorParticles),
                end(mediatorParticles));
        allParticles.insert(
                end(allParticles),
                begin(loopParticles),
                end(loopParticles));
        auto last = std::unique(begin(allParticles), end(allParticles));
        allParticles.erase(last, end(allParticles));
    }

}
