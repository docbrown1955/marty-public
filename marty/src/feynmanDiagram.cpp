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

#include "feynmanDiagram.h"
#include "quantumField.h"
#include "expander.h"

namespace mty {

    FeynmanDiagram::FeynmanDiagram(
            mty::Model const &t_model
            )
        :model(&t_model),
        expression(CSL_UNDEF)
    {

    }

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
        nLoops = diagram->getNLoops();
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
        return nLoops;
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

    void FeynmanDiagram::loadParticlesFromVertices(
            std::vector<mty::wick::Vertex> const &vertices
            )
    {
        std::vector<csl::Tensor> loopVertices(vertices.size());
        auto first = vertices[0][0];
        int path = mty::wick::Graph::walk(
                loopVertices.begin(), loopVertices.begin(), first, vertices
                );
        if (path == -1) {
            cycleLength = 0;
            loopVertices.clear();
        }
        else {
            cycleLength = path;
        }
        for (const auto &vertex : vertices) {
            for (const auto &node : vertex) {
                // Internal Fields
                if (node->field->isExternal()
                        || node->partner.lock()->field->isExternal()) {
                    addParticle(*node->field, External);
                    continue;
                }
                csl::Tensor pA = node->field->getPoint();
                csl::Tensor pB = node->partner.lock()->field->getPoint();
                auto posA = std::find(
                        begin(loopVertices), end(loopVertices), pA);
                auto posB = std::find(
                        begin(loopVertices), end(loopVertices), pB);
                auto type = (posA == end(loopVertices) 
                            && posB == end(loopVertices)) ? 
                    Mediator : Loop;
                addParticle(*node->field, type);
            }
        }
    }

    void FeynmanDiagram::updateParticleData()
    {
        updateParticleData(diagram->getVertices());
    }

    void FeynmanDiagram::updateParticleData(
            std::vector<mty::wick::Vertex> const &vertices
            )
    {
        loadParticlesFromVertices(vertices);
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

    FeynmanDiagram FeynmanDiagram::copy() const
    {
        return FeynmanDiagram { 
            *model, 
            csl::DeepCopy(expression),
            std::make_shared<mty::wick::Graph>(*diagram)
        };
    }

    FeynmanDiagram FeynmanDiagram::combine(
            FeynmanDiagram const &A,
            FeynmanDiagram const &B,
            Particle       const &mediator
            )
    {
        std::vector<mty::wick::Vertex> newVertices = A.diagram->getVertices();
        newVertices.insert(
                newVertices.end(),
                B.diagram->getVertices().begin(),
                B.diagram->getVertices().end()
                );
        auto isMediator = [&](mty::wick::Vertex const &vertex) {
            if (vertex.size() > 1)
                return false;
            auto const &node = vertex[0];
            return node->field->getQuantumParent() == mediator.get();
        };
        bool mediatorFound = false;
        for (std::size_t i = 0; i != newVertices.size(); ++i) {
            if (isMediator(newVertices[i])) {
                for (std::size_t j = i+1; j < newVertices.size(); ++j) {
                    if (isMediator(newVertices[j])) {
                        newVertices[i].push_back(newVertices[j][0]);
                        newVertices[i].setExternal(false);
                        newVertices.erase(newVertices.begin() + j);
                        mediatorFound = true;
                        break;
                    }
                }
                if (mediatorFound)
                    break;
            }
        }
        HEPAssert(mediatorFound,
                mty::error::RuntimeError,
                "Mediator \"" + mediator->getName() + "\" not found to connect"
                " diagrams.")

        FeynmanDiagram res(*A.model);
        res.updateParticleData(newVertices);
        res.diagram = (A.getNLoops() > B.getNLoops()) ? A.diagram : B.diagram;
        res.nLoops  = A.nLoops + B.nLoops;
        return res;
    }

}
