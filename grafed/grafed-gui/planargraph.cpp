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

#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <fstream>
#include "planargraph.h"
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>

namespace drawer {

Graph::Graph()
    :Graph(1)
{

}

Graph::Graph(std::size_t N)
    :nInsertions(0),
    nodes(N),
    loop(N),
    external(N),
    side(N),
    adjacency(N)
{
    static bool first = true;
    if (first) {
        first = false;
        srand(time(0));
    }
    for (auto& n : nodes) {
        initPoint(n, N);
    }
    initEnergies();
}

Graph::Graph(std::size_t N,
             std::vector<std::pair<int, int>> const& init)
    :Graph(N)
{
    for (const auto& [left, right] : init)
        addEdge(left, right);
}

void Graph::reinit()
{
    for (auto& n : nodes)
        initPoint(n, nodes.size());
}

void Graph::clear()
{
    nInsertions = 0;
    adjacency.clear();
    nodes.clear();
    loop.clear();
    external.clear();
    side.clear();
}

std::vector<Point>& Graph::getNodes() 
{
    return nodes;
}

std::vector<Point> const& Graph::getNodes() const
{
    return nodes;
}

Matrix<int>& Graph::getAdjacency()
{
    return adjacency;
}

Matrix<int> const& Graph::getAdjacency() const
{
    return adjacency;
}

bool totalAdjacency(Matrix<int> const& A,
                    std::size_t             pos)
{
    const std::size_t N = A.size();
    std::size_t adj = 0;
    for (std::size_t i = 0; i < N; ++i)
        adj += A(pos, i);
    return adj;
}

bool walked(std::vector<std::size_t> const& walked,
            std::size_t pos)
{
    return std::find(walked.begin(), walked.end(), pos) != walked.end();
}

struct next {
    std::size_t pos;
    int    adjacency;

    bool operator==(next const& other) const {
        return adjacency == other.adjacency;
    }

    bool operator<(next const& other) const {
        return pos < other.pos;
    }
};

std::vector<std::vector<next>> getSorted(std::vector<next> const& init)
{
    std::vector<std::vector<next>> sorted;
    for (const auto& n : init) {
        bool filled = false;
        for (auto& s : sorted)
            if (s[0].adjacency == n.adjacency) {
                s.push_back(n);
                filled = true;
                break;
            }
        if (not filled)
            sorted.push_back({n});
    }
    for (auto& s : sorted)
        std::sort(s.begin(), s.end());
    std::sort(sorted.begin(),
              sorted.end(),
              [&](std::vector<next> const& A,
                  std::vector<next> const& B)
              {
                return A[0].adjacency < B[0].adjacency;
              });
    return sorted;
}

bool next_permutation(std::vector<std::vector<next>>& sorted)
{
    for (std::size_t i = 0; i != sorted.size(); ++i) {
        if (sorted[i].size() == 1) 
            continue;
        if (std::next_permutation(sorted[i].begin(), sorted[i].end()))
            return true;
        std::reverse(sorted[i].begin(), sorted[i].end());
    }
    return false;
}

bool walkerStep(Matrix<int> const& A,
                Matrix<int> const& B,
                std::size_t iA,
                std::size_t iB,
                std::vector<std::size_t>& A_walked,
                std::vector<std::size_t>& B_walked,
                std::map<std::size_t, std::size_t>& mapAtoB)
{
    std::vector<next> nextA;
    std::vector<next> nextB;
    const std::size_t N = A.size();
    for (std::size_t i = 0; i != N; ++i) {
        if (A(iA, i) > 0 and not walked(A_walked, i))
            nextA.push_back({i, A(iA, i)});
        if (B(iB, i) > 0 and not walked(B_walked, i))
            nextB.push_back({i, B(iB, i)});
    }
    std::vector<std::size_t> newWalkedA(A_walked);
    for (const auto& n : nextA)
        newWalkedA.push_back(n.pos);
    std::vector<std::size_t> newWalkedB(B_walked);
    for (const auto& n : nextB)
        newWalkedB.push_back(n.pos);

    std::vector<std::vector<next>> sortedA = getSorted(nextA);
    std::vector<std::vector<next>> sortedB = getSorted(nextB);
    if (sortedA.size() != sortedB.size())
        return false;
    for (std::size_t i = 0; i != sortedA.size(); ++i)
        if (sortedA[i].size() != sortedB[i].size()
                or sortedA[i][0].adjacency != sortedB[i][0].adjacency)
            return false;

    auto saveWalkA = newWalkedA;
    auto saveWalkB = newWalkedB;
    auto saveMap = mapAtoB;
    do {
        bool breakValue = false;
        for (std::size_t i = 0; i != sortedA.size(); ++i) { 
            for (std::size_t j = 0; j != sortedB[i].size(); ++j) {
                if (!walkerStep(A, B, 
                                sortedA[i][j].pos, sortedB[i][j].pos,
                                newWalkedA, newWalkedB,
                                mapAtoB)) {
                    breakValue = true;
                    break;
                }
            }
            if (breakValue)
                break;
        }
        if (not breakValue) {
            for (std::size_t i = 0; i != sortedA.size(); ++i)
                for (std::size_t j = 0; j != sortedA[i].size(); ++j)
                    mapAtoB[sortedA[i][j].pos] = sortedB[i][j].pos;
            A_walked = std::move(newWalkedA);
            B_walked = std::move(newWalkedB);
            return true;
        }
        newWalkedA = saveWalkA;
        newWalkedB = saveWalkB;
        mapAtoB = saveMap;
    } while (next_permutation(sortedB));

    return false;
}

bool compare(Matrix<int> const& A,
             Matrix<int> const& B,
             std::size_t iA,
             std::size_t iB,
             std::map<std::size_t, std::size_t>& mapping)
{
    std::vector<std::size_t> walkedA = {iA};
    std::vector<std::size_t> walkedB = {iB};
    bool res = walkerStep(A, B, iA, iB, walkedA, walkedB, mapping);
    if (res)
        mapping[iA] = iB;

    return res;
}

bool Graph::isSame(Graph const& other) const
{
    std::map<std::size_t, std::size_t> mapping;
    return isSame(other, mapping);
}

bool Graph::isSame(Graph const& other,
                   std::map<std::size_t, std::size_t>& mapping) const
{
    if (nodes.size() != other.nodes.size())
        return false;
    std::size_t nEdgesA = 0;
    std::size_t nEdgesB = 0;
    const std::size_t N = nodes.size();
    for (std::size_t i = 0; i < N; ++i)
        for (std::size_t j = i+1; j < N; ++j) {
            nEdgesA += adjacency(i, j);
            nEdgesB += other.adjacency(i, j);
        }
    if (nEdgesA != nEdgesB)
        return false;

    for (std::size_t i = 0; i != N; ++i) {
        if (not external[i])
            continue;
        for (std::size_t j = 0; j != N; ++j) {
            if (not external[j])
                continue;
            mapping.clear();
            if (compare(adjacency, other.adjacency, i, j, mapping))
                return true;
        }
    }

    return false;
}

void Graph::addEdge(int left, int right)
{
    if (left == right) {
        const std::size_t N = nodes.size();
        Matrix<int, int> adjacency_new(N+1);
        for (std::size_t i = 0; i != N; ++i) {
            for (std::size_t j = 0; j != N; ++j)
                adjacency_new(i, j) = adjacency(i, j);
        }
        adjacency = adjacency_new;
        adjacency(left, N) = 2;
        adjacency(N, left) = 2;
        loop.push_back(true);
        loop[left] = true;
        nodes.push_back(Point());
        initPoint(nodes.back(), N);
        external.push_back(false);
        side.push_back(0);
        return;
    }
    ++adjacency(left, right);
    ++adjacency(right, left);
    if (adjacency(left, right) >= 2) {
        loop[left]  = true;
        loop[right] = true;
    }
}

void Graph::setSide(int pos, int t_side)
{
    if (side[pos] == 0 and t_side != 0) {
        ++nInsertions;
        external[pos] = true;
    }
    else if (side[pos] != 0 and t_side == 0) {
        external[pos] = false;
        --nInsertions;
    }
    side[pos] = t_side;
}

double Graph::xMin() const
{
    if (size() == 0)
        return 0;
    auto xmin = nodes[0].x;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].x < xmin)
            xmin = nodes[i].x;
    return xmin;
}

double Graph::xMax() const
{
    if (size() == 0)
        return 0;
    auto xmax = nodes[0].x;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].x > xmax)
            xmax = nodes[i].x;
    return xmax;
}

double Graph::yMin() const
{
    if (size() == 0)
        return 0;
    auto ymin = nodes[0].y;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].y < ymin)
            ymin = nodes[i].y;
    return ymin;
}

double Graph::yMax() const
{
    if (size() == 0)
        return 0;
    auto ymax = nodes[0].y;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].y > ymax)
            ymax = nodes[i].y;
    return ymax;
}

double Graph::zMin() const
{
    if (size() == 0)
        return 0;
    auto zmin = nodes[0].z;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].z < zmin)
            zmin = nodes[i].z;
    return zmin;
}

double Graph::zMax() const
{
    if (size() == 0)
        return 0;
    auto zmax = nodes[0].z;
    for (std::size_t i = 1; i != nodes.size(); ++i)
        if (nodes[i].z > zmax)
            zmax = nodes[i].z;
    return zmax;
}

double Graph::computeEnergy(bool verbose)
{
    double Energy = 0;
    for (const auto& func : energies) {
        Energy += func.compute(*this, verbose);
    }
    return Energy;
}

int Graph::minimize(int mode) 
{
    if (size() == 0)
        return 0;
    gatherExternalNodes();
    static bool first = true;
    if (first) {
        srand(time(0));
        first = false;
    }
    if (mode == 0) {
        enable3D = true;
        angleFactor = 0;
        absAngleFactor = 0;
        extensionFactor = 0;
        minimize(2);
        angleFactor = defaultAngleFactor;
        absAngleFactor = defaultAbsAngleFactor;
        extensionFactor = defaultExtensionFactor;
        enable3D = false;
        project();
        if (writeResToFile)
            write(fileNameProject);
        int res = minimize(1);
        project(true);
        tryFlip();
        return res;
    }

    auto f = [](const gsl_vector *var, void* graph)
    {
        Graph *G = static_cast<Graph*>(graph);
        G->readGslVector(var);

        return G->computeEnergy();
    };

    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;
  
    /* Starting point */
    x = allocGslVector();
    fillGslVector(x);

    /* Set initial step sizes to 1 */
    ss = allocGslVector();
    if (mode == 2) {
        gsl_vector_set_all (ss, 10.0);
    }
    else
        gsl_vector_set_all (ss, 10.0);
  
    /* Initialize method and iterate */
    minex_func.n = getNVar();
    minex_func.f = f;
    minex_func.params = this;
  
    s = gsl_multimin_fminimizer_alloc (T, getNVar());
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);
  
    std::size_t iter = 0;
    int status;
    double size;
    do {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);
        if (status)
          break;
        size = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, 1e-3);

        if (writeResToFile and mode == 2 and iter == 10)
            write(fileNameBegin);
  
    } while (status == GSL_CONTINUE && iter < 5000);
    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);

    center();
    if (writeResToFile)
        write((mode == 2) ? fileName3DRes : fileName2DRes);

    return status;
}

int Graph::minimize2D() 
{
    if (size() == 0)
        return 0;
    project(true, true);
    gatherExternalNodes();

    auto f = [](const gsl_vector *var, void* graph)
    {
        Graph *G = static_cast<Graph*>(graph);
        G->readGslVector(var);

        return G->computeEnergy();
    };

    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;
  
    /* Starting point */
    x = allocGslVector();
    fillGslVector(x);

    /* Set initial step sizes to 1 */
    ss = allocGslVector();
    gsl_vector_set_all (ss, 1);
  
    /* Initialize method and iterate */
    minex_func.n = getNVar();
    minex_func.f = f;
    minex_func.params = this;
  
    s = gsl_multimin_fminimizer_alloc (T, getNVar());
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);
  
    std::size_t iter = 0;
    int status;
    double size;
    do {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);
        if (status)
          break;
        size = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, 1e-2);
    } while (status == GSL_CONTINUE && iter < 1000);
    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);

    center();
    project(true);

    return status;
}

void Graph::project(bool only2DRotation,
                    bool keepExternal)
{
    center();
    extensionFactor = 0;
    angleFactor = 0;
    nodeSizeFactor = 0;
    double Emin = computeEnergy();
    double theta = 0;
    double phi   = 0;
    std::size_t nStep = 30;
    if (not only2DRotation) {
        for (std::size_t i = 0; i < nStep / 2; ++i)
            for (std::size_t j = 0; j < nStep; ++j) {
                Graph copy = *this;
                for (auto& node : copy.nodes) {
                    node.rotate(i*2*M_PI/nStep, 0);
                    node.rotate(j*2*M_PI/nStep, 1);
                }
                auto Enew = copy.computeEnergy();
                if (Enew < Emin) {
                    theta = i*2*M_PI/nStep;
                    phi   = j*2*M_PI/nStep;
                    Emin = Enew;
                }
            }
        for (auto& node : nodes) {
            node.rotate(theta, 0);
            node.rotate(phi, 1);
        }
    }
    absAngleFactor = defaultAbsAngleFactor;
    legsRepulsion  = 0;
    nodeLegsRepulsion = 0;
    externalRepulsion = 0;
    theta = 0;
    nStep = 500;
    for (std::size_t i = 0; i < nStep; ++i) {
        Graph copy = *this;
        for (auto& node : copy.nodes) {
            node.rotate(i*2*M_PI/nStep, 2);
        }
        auto Enew = copy.computeEnergy();
        if (Enew < Emin) {
            theta = i*2*M_PI/nStep;
            Emin = Enew;
        }
    }
    for (auto& node : nodes) {
        node.rotate(theta, 2);
    }
    absAngleFactor = 0;
    specialAnglesValue = defaultSpecialAnglesValue;
    externalLayout = (keepExternal) ? externalLayout : 0;
    nStep = 250;
    double maxAngle = (keepExternal) ? M_PI : M_PI / 5;
    double mini = -maxAngle;
    double maxi = maxAngle;
    for (std::size_t i = 0; i < nStep; ++i) {
        Graph copy = *this;
        for (auto& node : copy.nodes) {
            node.rotate(mini + i*(maxi-mini)/nStep, 2);
        }
        auto Enew = copy.computeEnergy();
        if (Enew < Emin) {
            theta = mini + i*(maxi-mini)/nStep;
            Emin = Enew;
        }
    }
    for (auto& node : nodes) {
        node.rotate(theta, 2);
    }
    specialAnglesValue = 0;

    externalLayout = defaultExternalLayout;
    extensionFactor = defaultExtensionFactor;
    angleFactor = defaultAngleFactor;
    nodeSizeFactor = defaultNodeSizeFactor;
    legsRepulsion  = defaultLegsRepulsion;
    nodeLegsRepulsion = defaultNodeLegsRepulsion;
    externalRepulsion = defaultExternalRepulsion;
}

void Graph::center()
{
    auto mean = getCenter();
    move(-mean.x, -mean.y);
}

void Graph::centerMargin()
{
    move(-(xMin() + xMax()) / 2, -(yMin() + yMax()) / 2);
}

void Graph::flip(int axis)
{
    if (axis < 0) {
        flip(0);
        flip(1);
        flip(2);
        return;
    }
    if (axis > 2) {
        std::cerr << "Flipping axis " << axis << " not possible: axis 2 max.\n";
        return;
    }
    for (auto& point : nodes) {
        if (axis == 0)
            point.x *= -1;
        else if (axis == 1)
            point.y *= -1;
        else
            point.z *= -1;
    }
}

void Graph::tryFlip()
{
    double supLength = 0;
    double infLength = 0;
    double ratio = 1.;
    for (std::size_t i = 0; i != nodes.size(); ++i) {
        double d = (nodes[i].x < 0) ? ratio*nodes[i].y : nodes[i].y;
        if (d < 0)
            infLength += -d;
        else
            supLength += d;
        for (std::size_t j = i+1; j < nodes.size(); ++j)
            if (not external[j] and adjacency(i, j) != 0) {
                double d = (nodes[i].y + nodes[j].y) / 2;
                d = (nodes[i].x/2 + nodes[j].x/2 < 0) ? ratio*d : d;
                if (d < 0)
                    infLength += -d;
                else
                    supLength += d;
            }
    }
    if (supLength < infLength) {
        flip(1);
    }
}

void Graph::scale(double s)
{
    for (auto& n : nodes) {
        n.x *= s;
        n.y *= s;
    }
}

void Graph::rotate(double theta)
{
    for (auto& n : nodes)
        n.rotate(theta);
}

void Graph::move(double x, double y)
{
    for (auto& n : nodes) {
        n.x += x;
        n.y += y;
    }
}

Point Graph::getCenter() const
{
    Point center;
    for (const auto& p : nodes)
        center += p;
    center /= nodes.size();

    return center;
}

void Graph::placeTopLeft()
{
    Point topLeft {xMin(), yMin(), zMin()};
    for (auto& p : nodes)
        p -= topLeft;
}

std::size_t Graph::getNVar()
{
    if (enable3D)
        return 3 * nodes.size();
    else
        return 2 * nodes.size();
}

gsl_vector* Graph::allocGslVector()
{
    if (enable3D)
        return gsl_vector_alloc(3 * nodes.size());
    else
        return gsl_vector_alloc(2 * nodes.size());
}

void Graph::fillGslVector(gsl_vector* x)
{
    std::size_t pos = 0;
    for (const auto& p : nodes) {
        gsl_vector_set(x, pos++, p.x);
        gsl_vector_set(x, pos++, p.y);
        if (enable3D)
            gsl_vector_set(x, pos++, p.z);
    }
}

void Graph::readGslVector(gsl_vector const* x)
{
    std::size_t pos = 0;
    for (auto& p : nodes) {
        p.x = gsl_vector_get(x, pos++);
        p.y = gsl_vector_get(x, pos++);
        if (enable3D)
            p.z = gsl_vector_get(x, pos++);
    }
}

void Graph::initExternalDistance()
{
    for (std::size_t i = 0; i != nodes.size(); ++i) {
        if (external[i] and side[i] != 0) {
            Vector<int> walker(nodes.size());
            walker[i] = 1;
            bool completed = false;
            std::size_t step = 0;
            do {
                walker = adjacency * walker;
                for (std::size_t j = 0; j != walker.size(); ++j)
                    if (walker[j] != 0 
                            and external[j] 
                            and side[j]*side[i] < 0) {
                        completed = true;
                        break;
                    }
                ++step;
            } while (not completed);
            side[i] *= step / side[i];
        }
    }
}

void Graph::initPoint(Point& p, std::size_t N)
{
    p.x = rand()*1./RAND_MAX * 2*N - N;
    p.y = rand()*1./RAND_MAX * 2*N - N;
    if (enable3D)
        p.z = rand()*1./RAND_MAX * 2*N - N;
    else
        p.z = 0;
}

void Graph::gatherExternalNodes()
{
    for (std::size_t i = 0; i != nodes.size(); ++i) {
        std::size_t edges = 0;
        for (std::size_t j = 0; j != nodes.size(); ++j)
            if (adjacency(i, j) != 0) {
                ++edges;
                if (edges > 1)
                    break;
            }
        if (edges == 1)
            external[i] = true;
        else
            external[i] = false;
    }
}

bool Graph::tryPermutation(int order, double energy)
{
    if (order <= 0)
        return false;
    if (energy < 0)
        energy = computeEnergy();
    for (std::size_t i = 0; i != nodes.size(); ++i)
        if (external[i]) {
            for (std::size_t j = i + 1; j < nodes.size(); ++j) {
                if (not external[j] or side[i] != side[j])
                    continue;
                std::swap(nodes[i], nodes[j]);
                if (computeEnergy() < energy) {
                    return true;
                }
                if (tryPermutation(order-1, energy))
                    return true;
                std::swap(nodes[i], nodes[j]);
            }
        }
    return false;
}

void Graph::write(std::string const& fileName) const
{
    std::string layout = fileName + "_layout.txt";
    std::string graph  = fileName + "_graph.txt";
    std::ofstream file;
    file.open(layout);
    if (not file) {
        std::cerr << "File " << layout << " impossible to create.\n";
        return;
    }
    for (const auto& node : nodes)
        file << node << std::endl;
    file.close();

    file.open(graph);
    if (not file) {
        std::cerr << "File " << graph << " impossible to create.\n";
        return;
    }
    file << adjacency << std::endl;
    file.close();
}

void Graph::addNode(double x, double y)
{
    nodes.push_back({x, y});
    loop.push_back(false);
    external.push_back(false);
    side.push_back(0);
    Matrix<int> adj2(nodes.size());
    for (std::size_t i = 0; i != nodes.size()-1; ++i)
        for (std::size_t j = 0; j != nodes.size()-1; ++j)
            adj2(i, j) = adjacency(i, j);
    adjacency = std::move(adj2);
}

void Graph::deleteNode(std::size_t pos)
{
    nodes.erase(nodes.begin() + pos);
    loop.erase(loop.begin() + pos);
    external.erase(external.begin() + pos);
    side.erase(side.begin() + pos);
    Matrix<int> adj2(nodes.size());
    auto t = [&](std::size_t i) { 
        return (i >= pos) ? i+1 : i;
    };
    for (std::size_t i = 0; i != nodes.size(); ++i)
        for (std::size_t j = 0; j != nodes.size(); ++j)
            adj2(i, j) = adjacency(t(i), t(j));
    adjacency = std::move(adj2);
}

std::ostream& operator<<(std::ostream& out,
                         Graph const&  graph)
{
    out << "Graph\n";
    out << graph.adjacency << '\n';
    out << '\n';
    for (int ext : graph.external)
        out << ext << '\n';
    out << '\n';
    for (int side : graph.side)
        out << side << '\n';

    return out;
}

void Graph::initEnergies() 
{
    // Uniform size for edges
    auto edgeSizeEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        for (std::size_t i = 0; i < size; ++i)
            for (std::size_t j = i+1; j < size; ++j) {
                if (g.adjacency(i, j) != 0) {
                    ++count;
                    double target = (g.external[i] or g.external[j]) ? 
                        externalLegSizeRation : 1;
                    if (g.loop[i] xor g.loop[j]) {
                        target /= 2;
                    }
                    else if (g.loop[i] and g.loop[j]) {
                        target /= 2;
                    }
                    E += std::pow(target - distance(g.nodes[i], g.nodes[j]), 2);
                }
            }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Uniformization of angles around each internal nodes
    auto angularEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        for (std::size_t i = 0; i != size; ++i)
            if (not g.external[i]) {
                std::vector<double> angles;
                angles.reserve(size);
                for (std::size_t j = 0; j < size; ++j) {
                    if (g.adjacency(i, j) != 0 and i != j) {
                        angles.push_back(angle(g.nodes[i],
                                               g.nodes[j],
                                               g.nodes[i] + Point(1, 0)));
                    }
                }
                if (angles.empty())
                    continue;
                std::sort(angles.begin(), angles.end());
                auto saveAngle = angles[0];
                for (std::size_t i = 0; i < angles.size()-1; ++i) {
                    angles[i] = remainder(angles[i+1] - angles[i]);
                }
                angles.back() = remainder(saveAngle - angles.back());
                double targetAngle = 2*M_PI / angles.size();
                for (const auto& a : angles) {
                    E += std::pow(drawer::remainder(a - targetAngle), 2);
                }
                count += angles.size();
            }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Energy for the values of angles, perfering specific values (0, pi/2 ...)
    auto absAngleEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        for (std::size_t i = 0; i < size; ++i) 
            for (std::size_t j = i+1; j < size; ++j) {
                double dy = g.nodes[j].y - g.nodes[i].y;
                double dx = g.nodes[j].x - g.nodes[i].x;
                E += (1 + std::sin(std::atan2(dy, dx))) / 2;
                ++count;
            }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Energy from layout of external (in or out) particles.
    auto layoutEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        double xmax = g.xMax();
        double xmin = g.xMin();
        for (std::size_t i = 0; i != size; ++i) {
            if (g.external[i]) {
                if (g.side[i] == 0)
                    continue;
                if (g.side[i] > 0)
                    E += g.side[i]*std::pow(g.nodes[i].x-xmax, 2);
                else
                    E -= g.side[i]*std::pow(g.nodes[i].x-xmin, 2);
                for (std::size_t j = i+1; j < size; ++j)
                    if (g.external[j] and g.side[i] == g.side[j])
                        E += std::pow(g.nodes[i].x - g.nodes[j].x, 2);
                    else if (g.adjacency(i, j) != 0)
                        E += std::pow(
                                1-g.side[i]*(g.nodes[i].x - g.nodes[j].x),
                                2);
                    else if (g.external[j] and g.side[i]*g.side[j] < 0) {
                        if (g.side[i] < 0)
                            E += 0.5*std::exp(g.nodes[i].x - g.nodes[j].x);
                        else
                            E += 0.5*std::exp(g.nodes[j].x - g.nodes[i].x);
                    }
            }
        }
        return E; // / (1 + std::abs(E)/20);
    };

    // Repulsion between nodes that are not wonnected to each other
    auto nodeRepulsionEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        for (std::size_t i = 0; i < size; ++i)
            for (std::size_t j = i+1; j < size; ++j) {
                if (g.adjacency(i, j) == 0) {
                    ++count;
                    auto d = distance2(g.nodes[i], g.nodes[j]);
                    if (decouplingDistance == -1
                            or std::sqrt(d) < decouplingDistance)
                        E += 1 / distance2(g.nodes[i], g.nodes[j]);
                }
            }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Repulsion between legs
    auto legsRepulsionEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        std::vector<Point> legs;
        legs.reserve(size + 2);
        for (std::size_t i = 0; i < size; ++i)
            for (std::size_t j = i+1; j < size; ++j)
                if (g.adjacency(i, j) != 0)
                    legs.push_back(Point((g.nodes[i] + g.nodes[j]) / 2));
        for (std::size_t i = 0; i < legs.size(); ++i)
            for (std::size_t j = i+1; j < legs.size(); ++j) {
                ++count;
                auto d = distance2(legs[i], legs[j]);
                if (decouplingDistance == -1 
                        or std::sqrt(d) < decouplingDistance)
                    E += 1 / distance2(legs[i], legs[j]);
            }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Repulsion between not connected legs and nodes 
    auto legsNodesRepulsionEnergy = [](Graph const& g)
    {
        const std::size_t size = g.size();
        double E = 0;
        std::size_t count = 0;
        std::vector<Point> legs;
        legs.reserve(size + 2);
        for (std::size_t i = 0; i < size; ++i)
            for (std::size_t j = i+1; j < size; ++j)
                if (g.adjacency(i, j) != 0)
                    for (std::size_t k = 0; k != size; ++k)
                        if (k != i and k != j) {
                            ++count;
                            auto leg = (g.nodes[i] + g.nodes[j]) / 2;
                            auto d = distance2(leg, g.nodes[k]);
                            if (decouplingDistance == -1
                                    or std::sqrt(d) < decouplingDistance)
                                E += 1 / distance2(leg, g.nodes[k]);
                        }
        return (count == 0) ? 0 : std::sqrt(E);
    };

    // Extension energy for graph, limits the size
    auto extensionEnergy = [](Graph const& g)
    {
        double xmin = g.xMin();
        double xmax = g.xMax();
        double ymin = g.yMin();
        double ymax = g.yMax();
        if (Graph::enable3D) {
            double zmin = g.zMin();
            double zmax = g.zMax();
            return std::sqrt(std::pow(xmax - xmin, 2)
                           + std::pow(ymax - ymin, 2)
                           + std::pow(zmax - zmin, 2));
        }
        return std::sqrt(std::pow(xmax - xmin, 2)
                       + std::pow(ymax - ymin, 2));
    };

    auto _3DEnergy = [](Graph const& g)
    {
        double E = 0;
        if (not Graph::enable3D)
            return E;
        for (const auto& node : g.nodes)
            E += node.z * node.z;
        return std::sqrt(E);
    };

    auto crossEnergy = [](Graph const& g)
    {
        if (g.enable3D)
            return 0.;
        const std::size_t size = g.size();
        double E = 0;
        for (std::size_t i = 0; i != size; ++i)
            for (std::size_t j = i+1; j < size; ++j)
                if (g.adjacency(i, j) != 0) {
                    line eq_ij = getEquation(g.nodes[i], g.nodes[j]);
                    if (eq_ij.a == 0)
                        continue;
                    for (std::size_t k = j+1; k < size; ++k)
                        for (std::size_t l = k+1; l < size; ++l)
                            if (g.adjacency(k, l) != 0) {
                                line eq_kl = 
                                    getEquation(g.nodes[k], g.nodes[l]);
                                if (eq_kl.a == 0 or eq_kl.a == eq_ij.a)
                                    continue;
                                double x0 = (eq_ij.b - eq_kl.b) 
                                          / (eq_ij.a - eq_kl.a);
                                if (g.nodes[i].x <= x0 and
                                        g.nodes[j].x >= x0 and
                                        g.nodes[k].x <= x0 and
                                        g.nodes[l].x >= x0)
                                    ++E;
                            }
                    }
        return E;
    };

    auto specialAnglesEnergy = [](Graph const& g)
    {
        double E = 0;
        const std::size_t size = g.size();
        double locality = M_PI/60;
        std::vector<std::pair<double, double>> scores
            = {{0, 1}, 
               {M_PI/2, 0.5},
               {M_PI/3, 0.5},
               {M_PI/4, 0.2}};  

        for (std::size_t i = 0; i != size; ++i) {
            for (std::size_t j = i+1; j < size; ++j)
                if (g.adjacency(i, j) != 0) {
                    auto angle_ij = (g.nodes[i] - g.nodes[j]).angle();
                    for (const auto& [angle, score] : scores) {
                        auto factor = score;
                        if (g.external[i] or g.external[j])
                            factor *= 1.3;
                        auto diff = std::abs(remainder(angle-angle_ij));
                        if (diff < 0 or diff > M_PI) {
                            std::cerr << "Angle out of bounds in adjustement: "
                                << diff << std::endl;
                        }
                        if (diff > M_PI / 2)
                            diff = M_PI - diff;
                        if (diff < locality)
                            E += factor*(-std::cos(M_PI*diff/locality)-1);
                    }
                }
        }

        return E;
    };

    energies.push_back(Energy(nodeSizeFactor,
                              edgeSizeEnergy,
                              "Edge size"));
    energies.push_back(Energy(angleFactor,
                              angularEnergy,
                              "Angles"));
    energies.push_back(Energy(absAngleFactor,
                              absAngleEnergy,
                              "Abs angles"));
    energies.push_back(Energy(externalLayout,
                              layoutEnergy,
                              "Layout"));
    energies.push_back(Energy(externalRepulsion,
                              nodeRepulsionEnergy,
                              "Node repulsion"));
    energies.push_back(Energy(legsRepulsion,
                              legsRepulsionEnergy,
                              "Legs repulsion"));
    energies.push_back(Energy(nodeLegsRepulsion,
                              legsNodesRepulsionEnergy,
                              "Node-legs repulsion"));
    energies.push_back(Energy(extensionFactor,
                              extensionEnergy,
                              "Extension"));
    energies.push_back(Energy(_3DFactor,
                              _3DEnergy,
                              "3D"));
    energies.push_back(Energy(defaultCrossEnergyValue,
                              crossEnergy,
                              "Cross energy"));
    energies.push_back(Energy(specialAnglesValue,
                              specialAnglesEnergy,
                              "Special angles energy"));
}

}
