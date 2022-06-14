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
 * @file planargraph.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#pragma once

#include <cmath>
#include <map>
#include <csignal>
#include <functional>
#include "adjacencyMatrix.h"
#include <gsl/gsl_vector.h>

namespace drawer {

struct Point {

    double x;
    double y;
    double z;

    Point()
        :x(0),
        y(0),
        z(0)
    {}

    Point(double value)
        :x(value),
        y(value),
        z(0)
    {}

    Point(double t_x,
          double t_y,
          double t_z = 0)
        :x(t_x),
        y(t_y),
        z(t_z)
    {}

    Point(Point const&) = default;
    Point(Point&&)      = default;
    Point& operator=(Point const&) = default;
    Point& operator=(Point&&)      = default;

    Point& rotate(double theta,
                  int    axis = 2)
    {
        if (axis < 0 or axis > 2) {
            std::cerr << "Axis " << axis << " unavailable for rotation, " 
                    << " space in 3D has axis 0, 1 or 2.\n";
            std::raise(SIGTERM);
        }
        double& xref = *(&x + (axis + 1)%3);
        double& yref = *(&x + (axis + 2)%3);
        double new_x = std::cos(theta)*xref - std::sin(theta)*yref;
        yref         = std::sin(theta)*xref + std::cos(theta)*yref;
        xref = new_x;

        return *this;
    }

    double angle(int axis = 2) const
    {
        if (axis < 0 or axis > 2) {
            std::cerr << "Axis " << axis << " unavailable for rotation, " 
                    << " space in 3D has axis 0, 1 or 2.\n";
            std::raise(SIGTERM);
        }
        if (axis == 0)
            return std::atan2(z, y);
        if (axis == 1)
            return std::atan2(x, z);
        return std::atan2(y, x);
    }

    double norm() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    template<typename Value_Type>
    Point& operator+=(Value_Type   value) {
        x += value;
        y += value;
        z += value;
        return *this;
    }
    Point& operator+=(Point const& p) {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    template<typename Value_Type>
    Point& operator*=(Value_Type   value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    Point& operator*=(Point const& p) {
        x *= p.x;
        y *= p.y;
        z *= p.z;
        return *this;
    }

    template<typename Value_Type>
    Point& operator-=(Value_Type   value) {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    Point& operator-=(Point const& p) {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    template<typename Value_Type>
    Point& operator/=(Value_Type   value) {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
    Point& operator/=(Point const& p) {
        x /= p.x;
        y /= p.y;
        z /= p.z;
        return *this;
    }

    friend
    std::ostream& operator<<(std::ostream& out,
                             Point const&  point)
    {
        out << point.x << " " << point.y << " " << point.z;
        return out;
    }
};

class LatexLinker;
class Energy;

class Graph {

    friend class LatexLinker;

    public:

    inline static bool        writeResToFile  = false;
    inline static std::string fileNameBegin   = "graph/begin";
    inline static std::string fileName3DRes   = "graph/3D";
    inline static std::string fileNameProject = "graph/project";
    inline static std::string fileName2DRes   = "graph/2D";

    inline static constexpr double externalLegSizeRation     = 0.70;
    inline static constexpr double absAngleMultiple          = 4;  
    inline static constexpr double decouplingDistance        = -1;

    inline static constexpr double defaultNodeSizeFactor     = 16.50;  
    inline static constexpr double defaultAngleFactor        = 6.50; 
    inline static constexpr double defaultExternalLayout     = 0.95;
    inline static constexpr double defaultExternalRepulsion  = 2.00;
    inline static constexpr double defaultLegsRepulsion      = 4.00;
    inline static constexpr double defaultNodeLegsRepulsion  = 3.00;
    inline static constexpr double defaultExtensionFactor    = 1.00;
    inline static constexpr double defaultAbsAngleFactor     = 0.00;
    inline static constexpr double default_3DFactor          = 0.10;
    inline static constexpr double defaultCrossEnergyValue   = 1.00;
    inline static constexpr double defaultSpecialAnglesValue = 6.50; 

    inline static bool   enable3D           = true;
    inline static double nodeSizeFactor     = defaultNodeSizeFactor;
    inline static double angleFactor        = defaultAngleFactor;
    inline static double externalRepulsion  = defaultExternalRepulsion;
    inline static double externalLayout     = defaultExternalLayout;
    inline static double legsRepulsion      = defaultLegsRepulsion;
    inline static double nodeLegsRepulsion  = defaultNodeLegsRepulsion;
    inline static double extensionFactor    = defaultExtensionFactor;
    inline static double absAngleFactor     = 0;
    inline static double _3DFactor          = default_3DFactor;
    inline static double specialAnglesValue = 0;

    Graph();

    explicit
    Graph(size_t N);

    Graph(size_t N,
          std::vector<std::pair<int, int>> const& init);

    Graph(Graph const& other) = default;
    Graph(Graph&& other)      = default;
    Graph& operator=(Graph const& other) = default;
    Graph& operator=(Graph&& other)      = default;

    ~Graph() {}

    void reinit();

    void clear();

    size_t size() const {
        return nodes.size();
    }

    double& getPosition(size_t pos) {
        if (pos / 2 > nodes.size()) {
            std::cerr << nodes.size() << ", " << pos << std::endl;
            std::cerr << "IndexError.\n";
            std::raise(SIGSEGV);
        }
        return (pos % 2 == 0) ? nodes[pos / 2].x : nodes[pos / 2].y;
    }

    double getPosition(size_t pos) const {
        if (pos / 2 > nodes.size()) {
            std::cerr << nodes.size() << ", " << pos << std::endl;
            std::cerr << "IndexError.\n";
            std::raise(SIGSEGV);
        }
        return (pos % 2 == 0) ? nodes[pos / 2].x : nodes[pos / 2].y;
    }

    std::vector<Point>& getNodes();

    std::vector<Point> const& getNodes() const;

    Matrix<int>& getAdjacency();

    Matrix<int> const& getAdjacency() const;

    bool isSame(drawer::Graph const& other) const;

    bool isSame(drawer::Graph const& other,
                std::map<size_t, size_t>& mapping) const;

    void addEdge(int left, int right);

    void setSide(int pos, int t_side);

    double xMin() const;

    double xMax() const;

    double yMin() const;

    double yMax() const;

    double zMin() const;

    double zMax() const;

    double computeEnergy(bool verbose = false);

    int minimize(int mode = 0);

    int minimize2D();

    void project(bool only2DRotation = false,
                 bool keepExternal = false);

    void center();

    void centerMargin();

    void flip(int axis = -1);

    void tryFlip();
    
    void scale(double s);

    void rotate(double theta);

    void move(double x, double y);

    Point getCenter() const;

    void placeTopLeft();

    void write(std::string const& fileName) const;

    void addNode(double x, double y);

    void deleteNode(size_t i);

    friend 
    std::ostream& operator<<(std::ostream& out,
                             Graph const&  graph);

    protected:

    gsl_vector* allocGslVector();

    size_t getNVar();

    void fillGslVector(gsl_vector* x);

    void readGslVector(gsl_vector const* x);

    void initExternalDistance();

    static void initPoint(Point& p, size_t N);

    void gatherExternalNodes();

    bool tryPermutation(int order = 1, double energy = -1);

    void initEnergies();

    private:

    size_t nInsertions;

    std::vector<Point> nodes;

    std::vector<int> loop;

    std::vector<int>  external;

    std::vector<int>  side;

    Matrix<int> adjacency;

    std::vector<Energy> energies;
};

class Energy {

    private:
        
    const double* factor;

    std::function<double(Graph const& g)> func;

    std::string name;

    public:

    Energy(double const& t_factor,
           std::function<double(Graph const& g)> const& t_func,
           std::string const& t_name = "Energy")
        :factor(&t_factor),
        func(t_func),
        name(t_name)
    {}

    double compute(Graph const& g,
                   bool         verbose = false) const
    {
        if (verbose and *factor != 0) {
            auto energy = func(g);
            std::cout << name << ": ";
            std::cout << energy << std::endl;
            return *factor * energy;
        }
        return (*factor == 0) ? 0 : *factor * func(g);
    }
};

inline
Point operator+(Point const& B,
                Point const& A)
{
    Point C(A);
    C += B;
    return C;
}

template<class T>
Point operator+(T     const& B,
                Point const& A)
{
    Point C(A);
    C += B;
    return C;
}

template<class T>
Point operator+(Point const& A, 
                T     const& B)
{
    Point C(A);
    C += B;
    return C;
}

template<class T>
Point operator*(T     const& B,
                Point const& A)
{
    Point C(A);
    C *= B;
    return C;
}

template<class T>
Point operator*(Point const& A, 
                T     const& B)
{
    Point C(A);
    C *= B;
    return C;
}

inline
Point operator-(Point const& A) {
    return Point(-A.x, -A.y);
}

inline
Point operator-(Point const& A,
                Point const& B)
{
    return Point(A.x - B.x, A.y - B.y);
}

template<class T>
Point operator-(T     const& B,
                Point const& A)
{
    Point C(-A);
    C += B;
    return C;
}

template<class T>
Point operator-(Point const& A, 
                T     const& B)
{
    Point C(A);
    C -= B;
    return C;
}

template<class T>
Point operator/(T     const& B,
                Point const& A)
{
    Point C(B);
    C /= A;
    return C;
}

template<class T>
Point operator/(Point const& A, 
                T     const& B)
{
    Point C(A);
    C /= B;
    return C;
}

inline double distance2(Point const& A,
                        Point const& B)
{
    if (Graph::enable3D)
        return std::max(1e-10,
               std::pow(A.x - B.x, 2)
             + std::pow(A.y - B.y, 2)
             + std::pow(A.z - B.z, 2));
    return std::max(1e-10,
           std::pow(A.x - B.x, 2)
         + std::pow(A.y - B.y, 2));
}

inline double distance(Point const& A,
                       Point const& B)
{
    return std::sqrt(distance2(A, B));
}

inline double remainder(double angle)
{
    if (std::abs(std::abs(angle) - M_PI/2) < 1e-10)
        return ((angle < 0) ? -1 : 1) * M_PI/2;
    return std::atan2(std::sin(angle), std::cos(angle));
}

inline double angle(Point const& ref,
                    Point const& A,
                    Point const& B)
{
    Point newA = (A - ref).rotate(-(B - ref).angle());
    return newA.angle();
}

struct line {

    double a;
    double b;
};

inline line getEquation(Point const& A,
                        Point const& B)
{
    line res;
    res.a = (A.x == B.x) ? 0 : (A.y - B.y) / (A.x - B.x);
    res.b = A.y - res.a * A.x;

    return res;
}

}
