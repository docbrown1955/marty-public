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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief
 */
#ifndef DRAWER_H_INCLUDED
#define DRAWER_H_INCLUDED

#include "graph.h"
#include "latexLink.h"

namespace drawer {
    class Graph;
}

namespace mty {

class Drawer {

    public:

    Drawer() = delete;

    static drawer::LatexLinker buildDiagram(
            std::shared_ptr<wick::Graph> const& graph,
            bool                                   external = true);

    static drawer::LatexLinker buildDiagram(
            std::shared_ptr<wick::Graph> const& graph,
            drawer::Graph                   const& model,
            std::map<size_t, size_t>             & mapping);

    static std::vector<drawer::LatexLinker> buildDiagrams(
            std::vector<std::shared_ptr<wick::Graph>> const& graph);

    static void launchViewer(
            std::vector<drawer::LatexLinker> const& links);

    static void launchViewer(
            std::vector<std::shared_ptr<wick::Graph>> const& links);

    static std::string buildLatex(
            std::shared_ptr<wick::Graph> const& graph);

    static void exportPDF(
            std::string const& name,
            std::vector<std::shared_ptr<wick::Graph>> const& graphs,
            std::string const& path = "diagrams");

    static void exportPNG(
            std::string const& name,
            std::vector<std::shared_ptr<wick::Graph>> const& graphs,
            std::string const& path = "diagrams");
    
    protected:

    static drawer::ParticleType getType(
            mty::QuantumField const& field);

    static drawer::Graph convertGraph(
            std::shared_ptr<wick::Graph> const& graph,
            bool                                   minimize = true,
            bool                                   external = true);
};

}

#endif
