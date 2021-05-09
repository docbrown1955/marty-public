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
 * @file latexLink.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#pragma once

#include "planargraph.h"
#include <map>
#include <sstream>

namespace JSON {
    class Node;
}

namespace drawer {

enum class ParticleType {

    None,
    Scalar,
    ChargedScalar,
    Fermion,
    Majorana,
    Vector,
    Gluon,
    Gaugino,
    Gluino,
    Ghost,
};

std::ostream& operator<<(std::ostream& out,
                         ParticleType  type);

class LatexLinker {

    public:

    struct Color {
        using Type = unsigned char;

        Type R = 0;
        Type G = 0;
        Type B = 0;
        Type A = 255;

        static Color fromStr(std::string const &str) {
            if (str.empty())
                return Color();
            std::istringstream sin(str);
            Color c;
            int a;
            if (sin.eof())
                return c;
            sin >> a;
            c.R = a;
            if (sin.eof())
                return c;
            sin >> a;
            c.G = a;
            if (sin.eof())
                return c;
            sin >> a;
            c.B = a;
            if (sin.eof())
                return c;
            sin >> a;
            c.A = a;

            return c;
        }
        std::string toStr() const {
            std::ostringstream sout;
            sout << int(R) << " " << int(G) << " " << int(B) << " " << int(A);
            return sout.str();
        }
    };

    inline static constexpr Color Black{0, 0, 0, 255};
    inline static constexpr Color White{255, 255, 255, 255};
    inline static constexpr Color Red{255, 0, 0, 255};
    inline static constexpr Color Blue{0, 0, 255, 255};
    inline static constexpr Color Green{0, 255, 0, 255};

    enum class NodeType {
        None = 0,
        Operator,
        Cross,
        PlainDisk,
        HatchedDisk
    };

    struct Node {
        std::string name = "";
        NodeType    type = NodeType::None;
        Color       color = Black;
        int         size = 0;
    };
    struct Edge {
        size_t       i    = 0;
        size_t       j    = 0;
        ParticleType type = ParticleType::Scalar;
        std::string  name = "";
        int          sign = true;
        int          curve = 0;
        bool         flipped = false;
        Color        color = Black;
        int          lineWidth = 3;
    };

    LatexLinker() = default;

    LatexLinker(Graph const& graph);

    ~LatexLinker();

    LatexLinker(LatexLinker const&) = default;
    LatexLinker(LatexLinker&&)      = default;
    LatexLinker& operator=(LatexLinker const&) = default;
    LatexLinker& operator=(LatexLinker&&)      = default;

    drawer::Graph const&getGraph() const;
    drawer::Graph &getGraph();

    std::string getName() const {
        return name;
    }

    void setName(std::string const &t_name) {
        name = t_name;
    }

    void clear();

    void setGraph(drawer::Graph const& t_graph);

    void setParticlesType(size_t       i,
                          size_t       j,
                          ParticleType type = ParticleType::Scalar,
                          std::string const& t_name = "",
                          bool               sign = true,
                          int                curve = 0,
                          bool               flipped = false,
                          Color              color = Black,
                          int                lineWidth = 3);

    void addOperator(size_t pos);

    void replaceParticlesType(size_t       i,
                              size_t       j,
                              ParticleType type = ParticleType::Scalar,
                              bool         sign = true);

    void removeEdge(size_t i, size_t j);

    void setVertexName(size_t             pos,
                       std::string const& name);

    void setEdgeName(size_t             i,
                     size_t             j,
                     std::string const& name);

    void setEdgeSign(size_t i,
                     size_t j,
                     bool   sign);

    void write(std::ostream& out);

    void saveToNode(JSON::Node *node) const;

    void save(std::string const& fileName) const;

    void loadFromNode(JSON::Node *node);

    void load(std::string const& fileName);

    static void saveMultiple(std::string const& fileName,
                             std::vector<LatexLinker> const& links);

    static std::vector<LatexLinker> loadMultiple(std::string const& fileName);

    void exportPDF(std::string const& fileName,
                   std::string const& path = ".");

    void exportPNG(std::string const& fileName,
                   std::string const& path = ".");

    void scale(float factor);

    std::vector<Node>& getNodes() {
        return nodes;
    }

    std::vector<Node> const &getNodes() const {
        return nodes;
    }

    std::vector<Edge>& getEdges() { 
        return edges;
    };

    std::vector<Edge> const& getEdges() const {
        return edges;
    }

    void removeNode(size_t pos);

    void addNode();

    std::string getNameExternal(size_t i) const;

    private:

    static
    std::string getNameVertex(size_t pos);

    void getFlippedEdgeLabels() const;

    void getStringValue(std::ostream&     out,
                        double value);

    void writeVertex(std::ostream     & out,
                     size_t             i,
                     Point       const& vertex,
                     std::string const& nameVertex,
                     std::string const& nameParticle = "");

    void writeEdge(std::ostream     & out,
                   ParticleType       type,
                   std::string const& nameA,
                   std::string const& nameB,
                   bool               sign = true,
                   std::string const& nameEdge = "",
                   bool               flipLabel = false);

    void writeLoopEdge(std::ostream     & out,
                       ParticleType       type,
                       std::string const& nameA,
                       std::string const& nameB,
                       int                mode,
                       bool               sign = true,
                       std::string const& nameEdge = "",
                       bool               flipLabel = false);

    friend
    std::ostream& operator<<(std::ostream& out,
                             LatexLinker&  linker);

    public:

    inline static bool autoLabel = true;

    inline static const std::string nameOrigin = "O";

    private:

    std::string name;

    float scaleFactor = 1.3;

    Graph graph;

    mutable
    std::vector<Edge> edges;

    mutable
    std::map<std::pair<size_t, size_t>, int> multiplicity;

    std::vector<Node> nodes;
};

}
