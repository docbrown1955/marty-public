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

#include "drawer.h"
#include "graph.h"

namespace mty {

drawer::LatexLinker Drawer::buildDiagram(
        std::shared_ptr<wick::Graph> const& graph,
        bool                                   external)
{
    csl::ScopedProperty idIndices(&csl::option::printIndexIds, false);
    auto diagram = convertGraph(graph, true, external);
    std::vector<wick::Vertex> const &vertices = graph->getVertices();
    const size_t N = vertices.size();
    drawer::LatexLinker link(diagram);
    for (size_t i = 0; i != N; ++i) {
        if (vertices[i][0]->field->isExternal()) {
            std::string name 
                = std::string(vertices[i][0]->field->getPoint()->getLatexName());
            for (char &c : name)
                if (c == 'X')
                    c = 'p';
            link.setVertexName(i, std::string(vertices[i][0]->field->getLatexName())
                    + "(" + name + ")");
        }
        for (size_t j = 0; j != vertices[i].size(); ++j)
            for (size_t k = j+1; k < vertices[i].size(); ++k) 
                if (vertices[i][j]->field->getPoint().get()
                    != vertices[i][k]->field->getPoint().get())
                    link.addOperator(i);

        for (const auto& nodeA : vertices[i]) {
            for (size_t j = i; j != N; ++j) 
                for (const auto& nodeB : vertices[j]) 
                    if (nodeA->partner.lock() == nodeB) {
                        auto type = getType(*nodeA->field);
                        std::string edgeName = 
                            (vertices[i][0]->field->isExternal() 
                             or vertices[j][0]->field->isExternal()) ?
                            "" : std::string(nodeA->field->getLatexName());
                        if (!nodeA->field->isComplexConjugate())
                            link.setParticlesType(i, j, type, edgeName, false);
                        else
                            link.setParticlesType(i, j, type, edgeName, true);
                    }
        }
    } 
    return link;
}

drawer::LatexLinker Drawer::buildDiagram(
        std::shared_ptr<wick::Graph> const& graph,
        drawer::Graph                   const& model,
        std::map<size_t, size_t>             & mapping)
{
    auto vertices = graph->getVertices();
    csl::ScopedProperty idIndices(&csl::option::printIndexIds, false);
    const size_t N = vertices.size();
    auto newGraph = model;
    for (size_t i = 0; i != N; ++i) {
        if (vertices[i][0]->field->isExternal()) {
            newGraph.setSide(
                    mapping[i],
                    (vertices[i][0]->field->isIncoming() ? -1 : 1));
        }
        else
            newGraph.setSide(
                    mapping[i], 
                    0);
    }
    newGraph.project(true, true);
    drawer::LatexLinker link(newGraph);
    for (size_t i = 0; i != N; ++i) {
        if (vertices[i][0]->field->isExternal()) {
            std::string name 
                = std::string(vertices[i][0]->field->getPoint()->getLatexName());
            for (char &c : name)
                if (c == 'X')
                    c = 'p';
            link.setVertexName(mapping[i], 
                    std::string(vertices[i][0]->field->getLatexName())
                    + "(" + name + ")");
        }
        for (size_t j = 0; j != vertices[i].size(); ++j)
            for (size_t k = j+1; k < vertices[i].size(); ++k) 
                if (vertices[i][j]->field->getPoint().get()
                    != vertices[i][k]->field->getPoint().get())
                    link.addOperator(i);
        for (const auto& nodeA : vertices[i]) {
            for (size_t j = i; j != N; ++j) 
                for (const auto& nodeB : vertices[j]) 
                    if (nodeA->partner.lock() == nodeB) {
                        auto type = getType(*nodeA->field);
                        std::string edgeName = 
                            (vertices[i][0]->field->isExternal() 
                             or vertices[j][0]->field->isExternal()) ?
                            "" : std::string(nodeA->field->getLatexName());
                        if (!nodeA->field->isComplexConjugate())
                            link.setParticlesType(
                                    mapping[i], 
                                    mapping[j], 
                                    type, 
                                    edgeName,
                                    false);
                        else
                            link.setParticlesType(
                                    mapping[i], 
                                    mapping[j],
                                    type, 
                                    edgeName,
                                    true);
                    }
        }
    } 
    return link;
}

// drawer::LatexLinker Drawer::buildDiagram(
//         std::shared_ptr<wick::Graph> const& graph,
//         std::shared_ptr<wick::Graph> const& modelGraph,
//         drawer::LatexLinker             const& linker)
// {
//     return buildDiagram(graph);
//     std::map<csl::Tensor, csl::Tensor> constraints ;
//     HEPAssert(graph->compare(*modelGraph, constraints, true),
//             mty::error::RuntimeError,
//             "Layout not recognized for " + toString(graph) + ".");
//     auto vertices = graph->getVertices();
//     auto vertexModel = modelGraph->getVertices();
//     std::vector<size_t> posMap(vertices.size());
//     for (size_t i = 0; i != posMap.size(); ++i)
//         for (size_t j = 0; j != posMap.size(); ++j)
//             if (constraints[vertices[j][0]->field->getPoint()].get()
//                     == vertexModel[i][0]->field->getPoint().get()) {
//                 posMap[j] = i;
//                 break;
//             }
//     csl::ScopedProperty idIndices(&csl::Index::printIds, false);
//     auto diagram = linker.getGraph();
//     const size_t N = vertices.size();
//     drawer::LatexLinker link(diagram);
//     for (size_t i = 0; i != N; ++i) {
//         if (vertices[i][0]->field->isExternal()) {
//             link.setVertexName(posMap[i], 
//                     std::string(vertices[i][0]->field->getLatexName())
//                     + "(" + std::string(vertices[i][0]->field->getPoint()->getLatexName()) 
//                     + ")");
//         }
//         for (const auto& nodeA : vertices[i]) {
//             for (size_t j = i; j != N; ++j) 
//                 for (const auto& nodeB : vertices[j]) 
//                     if (nodeA->partner.lock() == nodeB) {
//                         auto type = getType(*nodeA->field);
//                         std::string edgeName = 
//                             (vertices[i][0]->field->isExternal() 
//                              or vertices[j][0]->field->isExternal()) ?
//                             "" : std::string(nodeA->field->getLatexName());
//                         if (nodeA->field->isComplexConjugate())
//                             link.setParticlesType(
//                                     posMap[j], posMap[i], type, edgeName);
//                         else
//                             link.setParticlesType(
//                                     posMap[i], posMap[j], type, edgeName);
//                     }
//         }
//     } 
//     return link;
// }

std::vector<drawer::LatexLinker> Drawer::buildDiagrams(
        std::vector<std::shared_ptr<wick::Graph>> const& graphs)
{
    for (const auto &g : graphs)
        HEPAssert(g,
                mty::error::ValueError,
                "Null graph encountered ! ")
    std::vector<drawer::LatexLinker> links(graphs.size());
    std::vector<drawer::Graph> graphsD;
    std::map<size_t, size_t> mapping;
    for (size_t i = 0; i != graphs.size(); ++i) {
        auto g = convertGraph(graphs[i], false);
        bool found = false;
        for (const auto& gD : graphsD) {
            mapping.clear();
            if (g.isSame(gD, mapping)) {
                found = true;
                links[i] = buildDiagram(graphs[i], gD, mapping);
                break;
            }
        }
        if (not found) {
            auto link = buildDiagram(graphs[i], false);
            // links[i] = link;
            graphsD.push_back(link.getGraph());
            --i;
        }
    }

    return links;
}

static constexpr bool enableApp = true;

void Drawer::launchViewer(
        std::vector<drawer::LatexLinker> const& links)
{
    if constexpr (enableApp) {
        drawer::LatexLinker::saveMultiple("graphs.json", links);
        int res = system("grafed graphs.json &>/dev/null");
        res = system("sleep 0.5");
        if (res != 0) {
            std::cerr << "GRAFED launching failed ...\n";
        }
    }
}

void Drawer::launchViewer(
        std::vector<std::shared_ptr<wick::Graph>> const& graphs)
{
    launchViewer(buildDiagrams(graphs));
}

std::string Drawer::buildLatex(
        std::shared_ptr<wick::Graph> const& graph)
{
    auto link = buildDiagram(graph);
    std::ostringstream latex;
    latex << link;
    return latex.str();
}

void Drawer::exportPDF(
        std::string const&,
        std::vector<std::shared_ptr<wick::Graph>> const& graphs,
        std::string const&)
{
    if (graphs.empty())
        return;
    // size_t nDigits = std::ceil(std::log10(1+graphs.size()));
    // auto getName = [&](size_t n)
    // {
    //     std::string str = toString(n);
    //     for (size_t i = 0; str.size() < nDigits; ++i)
    //         str = 'd0' + str;
    //     return str;
    // };

    for (size_t i = 0; i != graphs.size(); ++i) {
        auto link = buildDiagram(graphs[i]);
        // link.exportPDF(name + "_" + getName(i), path);
    }
}

void Drawer::exportPNG(
        std::string const& name,
        std::vector<std::shared_ptr<wick::Graph>> const& graphs,
        std::string const& path)
{
    if (graphs.empty())
        return;
    size_t nDigits = std::ceil(std::log10(1+graphs.size()));
    auto getName = [&](size_t n)
    {
        std::string str = toString(n);
        for (size_t i = 0; str.size() < nDigits; ++i)
            str = '0' + str;
        return str;
    };

    for (size_t i = 0; i != graphs.size(); ++i) {
        auto link = buildDiagram(graphs[i]);
        link.exportPNG(name + "_" + getName(i), path);
    }
}

drawer::ParticleType Drawer::getType(
        mty::QuantumField const& field)
{
    if (field.getQuantumParent()->getDrawType() != drawer::ParticleType::None)
        return field.getQuantumParent()->getDrawType();
    int spinDim = field.getSpinDimension();
    if (spinDim == 1)
        return field.isSelfConjugate() ?
            drawer::ParticleType::Scalar : drawer::ParticleType::ChargedScalar;
    if (spinDim == 2)
        return field.isSelfConjugate() ?
            drawer::ParticleType::Majorana : drawer::ParticleType::Fermion;
    if (spinDim == 3)
        return drawer::ParticleType::Vector;

    CallHEPError(mty::error::ValueError,
                 "Spin dimension " + toString(spinDim) + " not recognized.");

    return drawer::ParticleType::Scalar;
}

drawer::Graph Drawer::convertGraph(
        std::shared_ptr<wick::Graph> const& graph,
        bool                                   minimize,
        bool                                   external)
{
    std::vector<wick::Vertex> vertices = graph->getVertices();
    const size_t N = vertices.size();
    drawer::Graph diagram(N);
    std::vector<std::shared_ptr<wick::Node>> alreadySeen;
    for (size_t i = 0; i != N; ++i)  {
        if (external and vertices[i][0]->field->isExternal()) 
            diagram.setSide(i, vertices[i][0]->field->isIncoming() ? -1 : 1);
        for (const auto& nodeA : vertices[i]) {
            alreadySeen.push_back(nodeA);
            for (size_t j = 0; j != N; ++j) 
                for (const auto& nodeB : vertices[j])  {
                    auto pos =
                        std::find(alreadySeen.begin(), alreadySeen.end(), nodeB);
                    if (pos != alreadySeen.end() 
                            and nodeA->partner.lock() == nodeB) {
                        diagram.addEdge(i, j);
                    }
                }
        }
    }
    if (minimize)
        diagram.minimize();

    return diagram;
}

}
