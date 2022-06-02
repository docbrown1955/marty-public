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

#include "latexLink.h"
#include "jsonLoader.h"
#include <iomanip>
#include <fstream>

namespace drawer {

std::ostream& operator<<(std::ostream& out,
                         ParticleType  type)
{
    switch(type) { 
        case ParticleType::None:          out << "";               break;
        case ParticleType::Scalar:        out << "scalar";         break;
        case ParticleType::ChargedScalar: out << "charged scalar"; break;
        case ParticleType::Fermion:       out << "fermion";        break;
        case ParticleType::Vector:        out << "boson";          break;
        case ParticleType::Gluon:         out << "gluon";          break;
        case ParticleType::Majorana:      out << "";               break;
        case ParticleType::Gaugino:       out << "Gaugino";        break;
        case ParticleType::Gluino:        out << "Gluino";         break;
        case ParticleType::Ghost:         out << "ghost";          break;
    }

    return out;
}

LatexLinker::LatexLinker(Graph const& t_graph)
    :graph(t_graph),
    edges(0),
    nodes(t_graph.size())
{
    graph.center();
    graph.flip(1);
    graph.placeTopLeft();
}

LatexLinker::~LatexLinker()
{

}

void LatexLinker::clear()
{
    graph.clear();
    nodes.clear();
    edges.clear();
    multiplicity.clear();
}

drawer::Graph const &LatexLinker::getGraph() const
{
    return graph;
}

drawer::Graph &LatexLinker::getGraph() 
{
    return graph;
}

void LatexLinker::setGraph(drawer::Graph const& t_graph)
{
    graph = t_graph;
}

void LatexLinker::setParticlesType(size_t       i,
                                   size_t       j,
                                   ParticleType type,
                                   std::string const& t_name,
                                   bool               sign,
                                   double             curve,
                                   bool               flipped,
                                   Color              color,
                                   int                lineWidth)
{
    // if (i > j) {
    //     setParticlesType(j, i, type, t_name, !sign, curve, !flipped);
    //     return;
    // }
    if (i == j) {
        // Tadpole structure
        for (size_t k = 0; k != graph.size(); ++k)
            if (graph.getAdjacency()(i, k) == 2) {
                if (i > k)
                    std::swap(i, k);
                auto pos = std::find_if(edges.begin(), edges.end(),
                        [&](Edge const& e) {
                          return e.i == i and e.j == k;
                        });
                if (pos == edges.end()) {
                    setParticlesType(
                            i, k, type, t_name, true, 1, false, color, lineWidth);
                }
                else {
                    setParticlesType(
                            i, k, type, "", true, -1, false, color, lineWidth);

                }
                return;
            }
    }
    edges.push_back(
            {i, j, type, t_name, sign, curve, flipped, color, lineWidth});
}

void LatexLinker::addOperator(size_t pos)
{
    nodes[pos].type = NodeType::Operator;
}

void LatexLinker::replaceParticlesType(size_t       i,
                                       size_t       j,
                                       ParticleType type,
                                       bool         sign)
{
    for (auto& e : edges)
        if (e.i == i and e.j == j) {
            e.type = type;
            e.sign = sign;
        }
}

void LatexLinker::removeEdge(size_t i, size_t j)
{
    for (size_t k = 0; k != edges.size(); ++k)
        if ((edges[k].i == i and edges[k].j == j)
                or (edges[k].j == i and edges[k].j == i)) {
            edges.erase(edges.begin() + k);
            --k;
        }
}

std::string LatexLinker::getNameExternal(size_t pos) const
{
    return nodes[pos].name;
}

void LatexLinker::setVertexName(size_t             pos,
                                std::string const& name)
{
    nodes[pos].name = name;
}

void LatexLinker::setEdgeName(size_t             i,
                              size_t             j,
                              std::string const& name)
{
    for (auto& e : edges)
        if (e.i == i and e.j == j) {
            e.name = name;
            return;
        }
}

void LatexLinker::setEdgeSign(size_t i,
                              size_t j,
                              bool   sign)
{
    for (auto& e : edges)
        if (e.i == i and e.j == j) {
            e.sign = sign;
            return;
        }
}

void LatexLinker::write(std::ostream& out)
{
   out << "\\begin{tikzpicture}\n";
   out << "  \\begin{feynman}[large]\n";
   out << "    \\vertex (" << nameOrigin << ");\n";
   for (size_t i = 0; i != graph.size(); ++i)
       writeVertex(
               out,
               i,
               graph.nodes[i],
               getNameVertex(i),
               nodes[i].name);

   out << "    \\diagram*{\n";
   if (autoLabel)
       getFlippedEdgeLabels();
   for (const auto& e : edges)
       writeLoopEdge(
           out,
           e.type,
           getNameVertex(e.i),
           getNameVertex(e.j),
           e.curve,
           e.sign,
           e.name,
           e.flipped);
   out << "    };\n";
   out << "  \\end{feynman}\n";
   out << "\\end{tikzpicture}\n";
}

void LatexLinker::exportPDF(std::string const& fileName,
                            std::string const& path)
{
#pragma GCC diagnostic ignored "-Wunused-result"
    std::ofstream tex(fileName + ".tex");
    if (not tex) {
        std::cerr << "Unable to create file \"" << fileName 
            << ".tex\".\n";
        return;
    }
    tex << "\\documentclass[preview]{standalone}\n";
    tex << "\\usepackage{tikz-feynman}\n";
    tex << "\\begin{document}\n";
    tex << *this;
    tex << "\\end{document}\n";
    tex.close();
    std::string command = "lualatex -interaction=nonstopmode " 
        + fileName + ".tex > /dev/null 2>&1";
    system(command.c_str());
    command = "rm " + fileName + ".tex " + fileName + ".aux "
        + fileName + ".out " + fileName + ".log > /dev/null 2>&1";
    system(command.c_str());
    command = "mv " + fileName + ".pdf " + path + " > /dev/null 2>&1";
    system(command.c_str());
}

void LatexLinker::exportPNG(std::string const& fileName,
                            std::string const& path)
{
#pragma GCC diagnostic ignored "-Wunused-result"
    std::string pdfName = "tmp_" + fileName;
    exportPDF(pdfName);
    std::string command 
        = "convert -density 400 -quality 100% " + pdfName 
        + ".pdf " + fileName + ".png > /dev/null 2>&1";
    system(command.c_str());
    command = "rm " + pdfName + ".pdf > /dev/null 2>&1; mv " 
        + fileName + ".png " + path + "/" + fileName + ".png > /dev/null 2>&1";
    system(command.c_str());
}

void LatexLinker::removeNode(size_t pos)
{
    for (size_t i = 0; i != edges.size(); ++i)
        if (edges[i].i >= pos or edges[i].j >= pos) {
            if (edges[i].i == pos or edges[i].j == pos) {
                edges.erase(edges.begin()+i);
                --i;
                continue;
            }
            if (edges[i].i > pos)
                --edges[i].i;
            if (edges[i].j > pos)
                --edges[i].j;
        }
    nodes.erase(nodes.begin() + pos);
}

void LatexLinker::addNode()
{
    nodes.push_back({});
}

void LatexLinker::scale(float factor)
{
    scaleFactor *= factor;
}

std::string LatexLinker::getNameVertex(size_t pos)
{
    std::string res = "";
    if (pos >= 26)
        res +=  char('a' + pos % 26) + getNameVertex((pos - pos % 26)/26);
    else 
        res += 'a' + pos;
    return res;
}

void LatexLinker::getFlippedEdgeLabels() const
{
    std::vector<Point> labelPositions;
    labelPositions.reserve(edges.size());

    auto getPosLabel = [&](size_t i, size_t j, double curve, bool flipped)
    {
        Point A = graph.nodes[i];
        Point B = graph.nodes[j];
        Point diff = B - A;
        Point dirLeft(-diff.y, diff.x);
        if (curve > 0.45) {
            // Left curve
            diff += dirLeft;
        }
        else if (curve < -0.45) {
            diff -= dirLeft;
        }
        int sign = (!flipped) ? 1 : -1;
        return A + (diff / 2. + sign * dirLeft / 5.);
    };

    auto getMinDistance = [&](std::vector<Point> const& points,
                              Point              const& A,
                              Edge               const& edge)
    {
        double min = 1000;
        for (const auto& B : points)
            if (auto d = distance(A, B); d < min)
                min = d;
        for (const auto& otherEdge : edges)
            if (&edge != &otherEdge) {
                Point mean = (graph.getNodes()[otherEdge.i]
                            + graph.getNodes()[otherEdge.j]) / 2;
                auto d = distance(mean, A);
                if (d < min)
                    min = d;
            }
        return min;
    };
    
    multiplicity.clear();
    for (auto& e : edges)
        if (graph.getAdjacency()(e.i, e.j) > 1) {
            multiplicity[
                std::make_pair(std::min(e.i, e.j), std::max(e.i, e.j))]++;
            // // if (e.i < e.j)
            // //     mode = 1-mode;
            // e.curve = mode;
            // if (e.curve == 0)
            //     e.curve = -1;
            // if (e.curve == 2)
            //     e.curve = 0;
        }
    const static std::array<double, 5> curvatureDef = {
        0, 1, -1, 0.5, -0.5
    };
    for (const auto &[pos, m] : multiplicity) {
        if (m < 2)
            continue;
        auto i = pos.first;
        auto j = pos.second;
        bool alreadySet = false;
        for (const auto &e : edges) {
            if ((e.i == i and e.j == j)
                    or (e.i == j and e.j == i)) {
                if (e.curve != 0) {
                    alreadySet = true;
                    break;
                }
            }
        }
        if (!alreadySet) {
            int start = (m == 3 or m == 5) ? 0 : 1;
            for (auto &e : edges) {
                if ((e.i == i and e.j == j)) {
                    e.curve = curvatureDef[start++];
                }
                else if ((e.i == j and e.j == i)) {
                    e.curve = curvatureDef[start++];
                }
            }
        }
    }
    double minDistance = 0.7;
    for (size_t i = 0; i != edges.size(); ++i) {
        auto& edge = edges[i];
        if (edge.name == "") {
            continue;
        }
        auto posL = getPosLabel(edge.i, edge.j, edge.curve, false);
        auto posR = getPosLabel(edge.i, edge.j, edge.curve, true);
        Point test[2];
        bool reverted = false;
        if ((posL-graph.getCenter()).norm()
                >= (posR-graph.getCenter()).norm()) {
            test[0] = posL;
            test[1] = posR;
        }
        else {
            reverted = true;
            test[0] = posR;
            test[1] = posL;
        }
        bool posLabel = 0;
        if (auto d = getMinDistance(labelPositions, test[0], edge); 
                d < minDistance)
            if (auto d2 = getMinDistance(labelPositions, test[1], edge); 
                    d2 > d) 
                posLabel = 1;
        edge.flipped = reverted ? !posLabel : posLabel;
        labelPositions.push_back(test[posLabel]);
    }
}

void LatexLinker::getStringValue(
        std::ostream&  out,
        double         value)
{
    out << std::fixed << std::setprecision(2) 
        << scaleFactor * std::abs(value);
}

void LatexLinker::writeVertex(std::ostream     & out,
                              size_t             i,
                              Point       const& vertex,
                              std::string const& nameVertex,
                              std::string const& nameParticle)
{
    std::string specH = (vertex.x < 0) ? "left" : "right";
    std::string specV = (vertex.y < 0) ? "above" : "below";
    out << "    \\vertex[" << specH << "=";
    getStringValue(out, vertex.x);
    out << "cm of " << nameOrigin << "](" << nameVertex << "_it);\n";
    out << "    \\vertex[" << specV << "=";
    getStringValue(out, std::abs(vertex.y));
    if (nameParticle.empty())
        out << "cm of " << nameVertex << "_it](" << nameVertex << ");\n";
    else 
        for (size_t j = 0; j != graph.size(); ++j)
            if (i != j and graph.adjacency(i, j) != 0) {
                auto angleLabel =
                    -int(180*(graph.nodes[i]-graph.nodes[j]).angle()/M_PI);
                out << "cm of " << nameVertex << "_it, label=" 
                    << angleLabel << ":\\(" << nameParticle << "\\)](" 
                    << nameVertex << ");\n";
                break;
            }
}

void LatexLinker::writeEdge(std::ostream&      out,
                            ParticleType       type,
                            std::string const& nameA,
                            std::string const& nameB,
                            bool               sign,
                            std::string const& nameEdge,
                            bool               flipLabel)

{
    if (not sign) {
        writeEdge(out, type, nameB, nameA, true, nameEdge, !flipLabel);
        return;
    }
    if (nameEdge == "")
        out << "      (" << nameA << ") -- [" 
            << type << "] (" << nameB << "),\n";
    else
        out << "      (" << nameA << ") -- [" << type << ", edge label"
            << ((flipLabel) ? "\'" : "") << "=\\("
            << nameEdge << "\\)] (" << nameB << "),\n";
}

void LatexLinker::writeLoopEdge(std::ostream&      out,
                                ParticleType       type,
                                std::string const& nameA,
                                std::string const& nameB,
                                int                mode,
                                bool               sign,
                                std::string const& nameEdge,
                                bool               flipLabel)

{
    if (not sign) {
        if (mode != 0 and mode % 2 == 0)
            --mode;
        else if (mode % 2 == 1)
            ++mode;
        writeLoopEdge(
                out, type, nameB, nameA, mode, true, nameEdge, !flipLabel
                );
        return;
    }
    
    std::string curve = "";
    if (mode == 1) {
        curve = "half left, ";
    }
    else if (mode == 2) {
        curve = "half right, ";
    }
    else if (mode == 3) {
        curve = "quarter left, ";
    }
    else if (mode == 4) {
        curve = "quarter right, ";
    }
    if (nameEdge == "")
        out << "      (" << nameA << ") -- [" << curve << type << "] (" 
            << nameB << "),\n";
    else
        out << "      (" << nameA << ") -- [" << curve << type << ", edge label"
            << ((flipLabel) ? "\'" : "") << "=\\(" << nameEdge << "\\)] (" 
            << nameB << "),\n";
}

void LatexLinker::saveToNode(JSON::Node *node) const
{
    if (autoLabel)
        getFlippedEdgeLabels();
    auto nameLeaf = JSON::Leaf<std::string>::make("name", name);
    auto scaleLeaf = JSON::Leaf<double>::make("scaleFactor", scaleFactor); 
    node->addChild(nameLeaf);
    node->addChild(scaleLeaf);

    auto graphNode = JSON::List::make("graph");
    for (size_t i = 0; i != graph.getNodes().size(); ++i) {
        auto const &node = graph.getNodes()[i];
        auto jsonNode = JSON::Node::make("graph");
        auto nameLeaf = JSON::Leaf<std::string>::make("name", nodes[i].name);
        auto typeLeaf = JSON::Leaf<int>::make("type", int(nodes[i].type));
        auto xLeaf = JSON::Leaf<double>::make("x", node.x);
        auto yLeaf = JSON::Leaf<double>::make("y", node.y);
        auto colorLeaf = JSON::Leaf<std::string>::make(
                "color", nodes[i].color.toStr());
        auto sizeLeaf  = JSON::Leaf<int>::make("size", nodes[i].size);
        // if (auto pos = std::find(operators.begin(), operators.end(), i);  
        //         pos != operators.end()) {
        //     auto opLeaf = JSON::Leaf<int>::make("op", 1);
        //     jsonNode->addChild(opLeaf);
        // }
        jsonNode->addChild(nameLeaf);
        jsonNode->addChild(typeLeaf);
        jsonNode->addChild(xLeaf);
        jsonNode->addChild(yLeaf);
        jsonNode->addChild(colorLeaf);
        jsonNode->addChild(sizeLeaf);
        graphNode->addChild(jsonNode);
    }
    node->addChild(graphNode);

    auto edgesNode = JSON::List::make("edges");
    for (const auto& e : edges) {
        int i(e.i), j(e.j);
        bool sign = false;
        if (e.i > e.j) {
            std::swap(i, j);
            sign = true;
        }
        auto iLeaf    = JSON::Leaf<int>::make("i", i);
        auto jLeaf    = JSON::Leaf<int>::make("j", j);
        auto typeLeaf = JSON::Leaf<int>::make("type", int(e.type));
        std::string correct_name;
        for (char c : e.name) {
            correct_name += c;
            // if (c == '\\')
            //     correct_name += '\\';
        }
        auto nameLeaf  = JSON::Leaf<std::string>::make("name", correct_name);
        auto flipLeaf  = JSON::Leaf<int>::make("flip", e.flipped);
        auto curveLeaf = JSON::Leaf<double>::make("curve", e.curve);
        auto signLeaf  = JSON::Leaf<int>::make("sign", e.sign xor sign);
        auto colorLeaf = JSON::Leaf<std::string>::make(
                "color", e.color.toStr());
        auto lineWidthLeaf = JSON::Leaf<int>::make("lineWidth", e.lineWidth);

        auto eNode = JSON::Node::make("edges");
        eNode->addChild(iLeaf);
        eNode->addChild(jLeaf);
        eNode->addChild(typeLeaf);
        eNode->addChild(nameLeaf);
        eNode->addChild(flipLeaf);
        eNode->addChild(curveLeaf);
        eNode->addChild(signLeaf);
        eNode->addChild(colorLeaf);
        eNode->addChild(lineWidthLeaf);

        edgesNode->addChild(eNode);
    }
    node->addChild(edgesNode);

    //auto nameNode = JSON::List::make("names");
    //for (const auto& name : vertexNames) {
    //    std::string correct_name;
    //    for (char c : name) {
    //        correct_name += c;
    //        // if (c == '\\')
    //        //     correct_name += '\\';
    //    }
    //    auto leaf = JSON::Leaf<std::string>::make("names", correct_name);
    //    nameNode->addChild(leaf);
    //}
    //node->addChild(nameNode);
}

void LatexLinker::save(std::string const& fileName) const
{
    JSON::Node tree("root");
    saveToNode(&tree);
    JSON::Reader::saveToFile(fileName, &tree);
}

void LatexLinker::loadFromNode(JSON::Node *node)
{
    clear();
    std::optional<std::string> nameLeaf
        = JSON::Parser::parseArgument<std::string>(node, "name");
    std::optional<double> scaleLeaf 
        = JSON::Parser::parseArgument<double>(node, "scaleFactor"); 
    name = nameLeaf.value_or("");
    scaleFactor = scaleLeaf.value_or(1); 

    JSON::Node* graphNode = JSON::Parser::parseNode(node, "graph");
    size_t nNodes = 0;
    if (graphNode) {
        for (const auto& point : *graphNode) {
            double x
                = *JSON::Parser::parseArgument<double>(point, "x", true);
            double y
                = *JSON::Parser::parseArgument<double>(point, "y", true);
            std::optional<int> op
                = JSON::Parser::parseArgument<int>(point, "op");
            std::optional<std::string> color
                = JSON::Parser::parseArgument<std::string>(point, "color");
            std::optional<std::string> name 
                = JSON::Parser::parseArgument<std::string>(point, "name");
            std::optional<int> size
                = JSON::Parser::parseArgument<int>(point, "size");
            std::optional<int> type
                = JSON::Parser::parseArgument<int>(point, "type");
            graph.addNode(x, y);
            nodes.push_back({
                    name.value_or(""),
                    static_cast<NodeType>(type.value_or(0)),
                    Color::fromStr(color.value_or("")),
                    size.value_or(0)
                    });
            if (op and *op == 1)
                addOperator(nNodes);
            ++nNodes;
        }
    }

    JSON::Node* edgesNode = JSON::Parser::parseNode(node, "edges");
    for (const auto& e : *edgesNode) {
        
        int i       = *JSON::Parser::parseArgument<int>(e, "i", true);
        int j       = *JSON::Parser::parseArgument<int>(e, "j", true);
        int type    = *JSON::Parser::parseArgument<int>(e, "type", true);
        int flipped = *JSON::Parser::parseArgument<int>(e, "flip", true);
        double curve = *JSON::Parser::parseArgument<double>(e, "curve", true);
        int sign    = *JSON::Parser::parseArgument<int>(e, "sign", true);
        std::string name 
            = *JSON::Parser::parseArgument<std::string>(e, "name", true);
        std::optional<std::string> color
            = JSON::Parser::parseArgument<std::string>(e, "color");
        std::optional<int> lineWidth 
            = JSON::Parser::parseArgument<int>(e, "lineWidth");

        edges.push_back({
                size_t(i), 
                size_t(j), 
                ParticleType(type),
                name, 
                sign,
                curve,
                static_cast<bool>(flipped),
                Color::fromStr(color.value_or("")),
                lineWidth.value_or(3)});
        ++graph.getAdjacency()(i, j);
        ++graph.getAdjacency()(j, i);
    }

//    JSON::Node* nameNode = JSON::Parser::parseNode(node, "names");
//    for (const auto& node : *nameNode) {
//        std::string name = JSON::Parser::interpretObject<std::string>(node.get());
//        vertexNames.push_back(name);
//    }
//
}

void LatexLinker::load(std::string const& fileName)
{
    std::unique_ptr<JSON::Node> tree_unique 
        = JSON::Reader::loadFromFile(fileName);
    loadFromNode(tree_unique.get());
}

void LatexLinker::saveMultiple(
        std::string const& fileName,
        std::vector<LatexLinker> const& links)
{
    JSON::Node tree("root");
    auto listDiagrams = JSON::List::make("diagrams");
    for (const auto& l : links) {
        auto node_l = JSON::Node::make("diagrams");
        l.saveToNode(node_l.get());
        listDiagrams->addChild(node_l);
    }
    tree.addChild(listDiagrams);
    JSON::Reader::saveToFile(fileName, &tree);
}

std::vector<LatexLinker> LatexLinker::loadMultiple(
        std::string const& fileName)
{
    std::unique_ptr<JSON::Node> tree_unique 
        = JSON::Reader::loadFromFile(fileName);
    JSON::Node *tree = tree_unique.get();
    JSON::Node *listDiagrams = JSON::Parser::parseNode(tree, "diagrams");
    std::vector<LatexLinker> links;
    if (listDiagrams) {
        for (const auto& node : *listDiagrams) {
            links.emplace_back();
            links.back().loadFromNode(JSON::Parser::convert(node));
        }
    }
    return links;
}

std::ostream& operator<<(std::ostream& out,
                         LatexLinker&  linker)
{
    linker.write(out);
    return out;
}

}
