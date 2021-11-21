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

#include "wick.h"
#include "graph.h"
#include "mrtlog.h"
#include "mrtError.h"
#include "mrtUtils.h"
#include "amplitude.h"
#include "feynruleMomentum.h"
#include "mrtOptions.h"
#include "diracology.h"
#include "feynmanDiagram.h"

using namespace std;
using namespace csl;
using namespace mty;

namespace mty::wick {

///////////////////////////////////////////////////
/*************************************************/
// Node class                                    //
/*************************************************/
///////////////////////////////////////////////////

std::vector<std::shared_ptr<Node>> Node::copyGraph(
        std::vector<std::shared_ptr<Node>> const& init)
{
    std::vector<std::shared_ptr<Node>> newGraph(init.size());
    for (size_t i = 0; i != init.size(); ++i)
        newGraph[i] = std::make_shared<Node>(init[i]->field);
    for (size_t i = 0; i != init.size(); ++i) {
        auto pos = std::find_if(
                init.begin(), 
                init.end(),
                [&](std::shared_ptr<Node> const& el)
                {
                    return el->partner.lock() == init[i];
                });
        HEPAssert(pos != init.end(),
                mty::error::RuntimeError,
                "Node not found in copy of grpah, make sure the graph is "
                + std::string("fully connected."));
        size_t partner = pos - init.begin();
        newGraph[i]->partner = newGraph[partner];
    }

    return newGraph;
}

///////////////////////////////////////////////////
/*************************************************/
// Vertex class                                  //
/*************************************************/
///////////////////////////////////////////////////

Vertex::Vertex(): vector<shared_ptr<Node>>(),
    external(false),
    id(0)
{}

Vertex::Vertex(const vector<const QuantumField*>& fields,
               size_t                             t_id,
               bool                               t_external)
    :vector<shared_ptr<Node>>(),
    external(t_external),
    id(1+t_id)
{
    reserve(fields.size());
    for (const auto& f : fields)
        push_back(csl::make_shared<Node>(Node(f)));
}

Vertex::Vertex(const Vertex& other): vector<shared_ptr<Node>>()
{
    *this = other;
}

bool Vertex::isExternal() const
{
    return external;
}

bool Vertex::isFree() const
{
    for (const auto& node : *this)
        if (node->partner.lock()) 
            return false;
    return true;
}

bool Vertex::hasFreeNode() const
{
    for (const auto& node : *this)
        if (not node->partner.lock()) // No partner, free node
            return true;
    return false;
}

bool Vertex::hasFreeNode(const QuantumField* field) const
{
    for (const auto& node : *this)
        if (not node->partner.lock()
                and node->field->isExactlyContractiblewith(*field))
            // No partner, free node
            return true;
    return false;
}

bool Vertex::containsExplicit(const mty::QuantumField* field) const
{
    for (const auto& node : *this)
        if (*node->field == *field)
            return true;
    return false;
}

bool Vertex::isDegenerateWith(const Vertex& other) const
{
    if (size() != other.size())
        return false;
    if (id != 0 and id == other.id)
        return true;
    vector<int> indicesLeft(size());
    for (size_t i = 0; i != size(); ++i)
        indicesLeft[i] = i;

    for (auto iter = begin(); iter != end(); ++iter) {
        bool matched = false;
        const QuantumField* fA = (*iter)->field;
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            size_t index = indicesLeft[j];
            const QuantumField* fB = other[index]->field;
            if (fA->getQuantumParent() == fB->getQuantumParent()) {
                if (fA->isSelfConjugate() 
                        or (fA->getConjugated() == fB->getConjugated())) 
                    if ((not (*iter)->partner.lock()
                     and not other[index]->partner.lock())
                         or (areDegenerate(*(*iter)->partner.lock()->field,
                                           *other[index]->partner.lock()->field)
                                and (*iter)->partner.lock()->field->getPoint() == 
                                other[index]->partner.lock()->field->getPoint())) 
                        if (fA->getDerivativeStructure().size()
                                == fB->getDerivativeStructure().size())
                        {
                            matched = true;
                            indicesLeft.erase(indicesLeft.begin() + j);
                            break;
                        }
            }
        }
        if (not matched)
            return false;
    }
    return true;
}

const QuantumField* Vertex::getFirstContractibleField() const
{
    for (const auto& node : *this)
        if (node->isFree())
            return node->field;
    cout << *this << endl;
    CallHEPError(mty::error::RuntimeError,
            "No contractible node in Vertex::getFirstContractibleField()");

    return nullptr;
}

shared_ptr<Node> Vertex::getFirstContractibleNode() const
{
    for (const auto& node : *this)
        if (node->isFree())
            return node;
    
    return nullptr;
}

shared_ptr<Node> Vertex::getFirstContractibleNode(
        const QuantumField* field) const
{
    for (const auto& node : *this)
        if (node->isFree() and node->field->isExactlyContractiblewith(*field))
            return node;
    
    return nullptr;
}

vector<shared_ptr<Node>> Vertex::getContractibleNodes() const
{
    vector<shared_ptr<Node>> nodes(0);
    for (const auto& node : *this)
        if (not node->partner.lock()) // No partner, free node
            nodes.push_back(node);
    return nodes;
}

vector<shared_ptr<Node>> Vertex::getContractibleNodes(
        const mty::QuantumField* field) const
{
    vector<shared_ptr<Node>> nodes(0);
    for (const auto& node : *this)
        if (not node->partner.lock()
                and node->field->isExactlyContractiblewith(*field))
            nodes.push_back(node); // No partner, free node
    return nodes;
}


void Vertex::setExternal(bool t_external)
{
    external = t_external;
}

int Vertex::getDegeneracy(const QuantumField* t_field) const
{
    int degeneracy = 0;
    for (const auto& node : *this)
        if (not node->partner.lock())
            if (areExactlyContractible(*node->field, *t_field)) {
                ++degeneracy;
            }
    return degeneracy;
}

int Vertex::getTotalDegeneracyFactor() const
{
    int factor = 1;
    std::set<QuantumFieldParent*> treated;
    for (size_t i = 0; i != size(); ++i) {
        const QuantumField* field = (*this)[i]->field;
        if (treated.find(field->getQuantumParent()) != treated.end())
            continue;
        int n = 1;
        for (size_t j = i+1; j < size(); ++j)
            if (areDegenerate(*field, *(*this)[j]->field))
                ++n;
        treated.insert(field->getQuantumParent());
        factor *= factorial(n);
    }
    return factor;
}

Vertex& Vertex::operator=(const Vertex& other)
{
    vector<shared_ptr<Node>>::operator=(vector<shared_ptr<Node>>(other.size()));
    external = other.external;
    id = other.id;
    auto first = begin();
    for (size_t i = 0; i != size(); ++i) 
        if (other[i]->partner.lock())
            // Already contracted node: no new allocation, keep the shared_ptr
            *first++ = other[i];
        else
            // Free node: we really copy it
            *first++ = csl::make_shared<Node>(other[i]->field);

    return *this;
}

bool Vertex::operator==(const Vertex& other) const
{
    if (size() != other.size())
        return false;
    for (size_t i = 0; i != size(); ++i) 
        if (*(*this)[i]->field != *other[i]->field)
            return false;
    
    return true;
}

bool Vertex::operator!=(const Vertex& other) const
{
    return not (*this == other);
}

ostream& operator<<(ostream& fout, const Vertex& v)
{
    fout << "\nExternal = " << v.external << endl;
    for (const auto& shared_node : v)
        fout << shared_node->field->getName() << " ";
    fout << v.getContractibleNodes().size();

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// ConnectedComponent class                      //
/*************************************************/
///////////////////////////////////////////////////

ConnectedComponent::ConnectedComponent()
{

}

ConnectedComponent::ConnectedComponent(const Vertex& firstVertex)
    :vertices(1,firstVertex)
{

}

vector<Vertex> const &ConnectedComponent::getVertices() const
{
    return vertices;
}

void ConnectedComponent::addVertex(const Vertex& newVertex)
{
    push_back(newVertex);
}

bool ConnectedComponent::hasValenceLeft() const
{
    for (const auto& vertex : *this)
        if (vertex.hasFreeNode())
            return true;
    return false;
}

const QuantumField* ConnectedComponent::getFirstContractibleField() const
{
    auto node = getFirstContractibleNode();
    return node->field;
}

int ConnectedComponent::getFirstContractibleVertex() const
{
    for (size_t i = 0; i != size(); ++i) 
        if (vertices[i].hasFreeNode()) 
            return i;
    return -1;
}

vector<int> ConnectedComponent::getContractibleVertices(
        const QuantumField* field) const
{
    vector<int> pos_vertices(0);
    int firstVertex = getFirstContractibleVertex();
    for (size_t i = 0; i != size(); ++i) {
        if (mty::option::excludeTadpoles 
                and int(i) == firstVertex
                and vertices[firstVertex].size() == 3)
            continue;
        const Vertex& vertex = vertices[i];
        if (vertex.hasFreeNode(field)) {
            if (int(i) == firstVertex) {
                // Tadpole structure
                // First node already taken in contraction, we check
                // if there is a second (at least) available in the same
                // vertex, else we do not take it
                if (not field->isSelfConjugate() 
                        or vertex.getDegeneracy(field) > 1)
                    pos_vertices.push_back(i);
            }
            else 
                pos_vertices.push_back(i);
        }
    }

    return pos_vertices;
}

shared_ptr<Node> ConnectedComponent::getFirstContractibleNode() const
{
    for (const auto& vertex : *this)
        if (vertex.hasFreeNode())
            return vertex.getFirstContractibleNode();

    return nullptr;
}

vector<shared_ptr<Node>> ConnectedComponent::getNodes() const
{
    vector<shared_ptr<Node>> nodes(0);
    for (const auto& vertex : *this)
        for (const auto& node : vertex)
            nodes.push_back(node);

    return nodes;
}

int ConnectedComponent::connect(int indexVertex)
{
    const QuantumField* field = getFirstContractibleField();
    shared_ptr<Node>    node  = getFirstContractibleNode();
    HEPAssert((bool)node, mty::error::RuntimeError,
            "Invalid first contractible node in"+
            std::string("ConnectedComponent::connect(int)"));
    
    int degeneracy = vertices[indexVertex].getDegeneracy(field);
    shared_ptr<Node> node2;
    if (indexVertex == getFirstContractibleVertex()
            and field->isSelfConjugate()) {
        --degeneracy;
        node2 = vertices[indexVertex].getContractibleNodes(field)[1];
    }
    else
        node2 = vertices[indexVertex].getFirstContractibleNode(field);

    HEPAssert((bool)node2, mty::error::RuntimeError,
            "Invalid second contractible node in"+
            std::string("ConnectedComponent::connect(int)"));

    if (not node->field->isExactlyContractiblewith(*node2->field))
        return -1;
    contractNodes(node, node2);

    return degeneracy;
}

bool ConnectedComponent::connect(Vertex& other) 
{
    shared_ptr<Node>    node  = getFirstContractibleNode();
    const QuantumField* field = node->field;
    shared_ptr<Node>    node2 = other.getFirstContractibleNode(field);

    if (not node->field->isExactlyContractiblewith(*node2->field))
        return false;
    contractNodes(node, node2);
    return true;
}

ostream& operator<<(ostream& fout, const ConnectedComponent& c)
{
    fout << "Connected compo:\n";
    vector<shared_ptr<Node>> nodes;
    for (const auto& v : c)
        nodes.insert(nodes.end(), v.begin(), v.end());
    vector<size_t> indicesLeft = vector<size_t>(nodes.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;
    for (size_t indexA = 0; indexA != indicesLeft.size(); ++indexA) {
        shared_ptr<Node> nodeA = nodes[indicesLeft[indexA]];
        if (not nodeA->partner.lock()) {
            fout << "free -->  ";
            nodeA->field->print();
            indicesLeft.erase(indicesLeft.begin()+indexA);
            --indexA;
            continue;
        }
        for (size_t indexB = 1+indexA; indexB != indicesLeft.size(); ++indexB) {
            shared_ptr<Node> nodeB = nodes[indicesLeft[indexB]];
            if (not nodeB->partner.lock())
                continue;
            if (nodeA->partner.lock() == nodeB
                    and nodeB->partner.lock() == nodeA) {
                nodeA->field->print(1);
                fout << "  <-->  ";
                nodeB->field->print();
                indicesLeft.erase(indicesLeft.begin()+indexB);
                indicesLeft.erase(indicesLeft.begin()+indexA);
                --indexA;
                break;
            }
        }
    }

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Graph class                                   //
/*************************************************/
///////////////////////////////////////////////////

Graph::Graph()
    :ruleMode(false),
    init(std::make_shared<std::vector<Vertex>>()),
    initialOrder(std::make_shared<std::vector<mty::QuantumField>>()),
    extVertex(0),
    intVertex(0),
    independentVertices(0)
{

}

Graph::Graph(const vector<QuantumField>& field,
             std::map<csl::Tensor, size_t>& vertexIds,
             bool                            t_ruleMode)
    :Graph()
{
    ruleMode = t_ruleMode;
    vector<vector<const QuantumField*>> vertices(0);
    // Isolating vertices (fields at same space-time point)
    for (const auto& f : field) {
        initialOrder->push_back(f);
    }
    for (const auto& f : *initialOrder) {
        bool matched = false;
        for (auto& vertex : vertices) {
            if (f.getPoint() == vertex[0]->getPoint()) {
                vertex.push_back(&f);
                matched = true;
                break;
            }
        }
        if (not matched)
            vertices.push_back(vector<const QuantumField*>(1, &f));
    }

    // Isolating external from internal vertices
    for(size_t i = 0; i != vertices.size(); ++i) {
        vector<const QuantumField*> vertex = vertices[i];
        if (vertex[0]->isExternal()) {
            Vertex newVertex(vertex, true);
            init->push_back(newVertex);
            extVertex.push_back(newVertex);
        }
        else {
            Vertex newVertex(vertex, vertexIds[vertex[0]->getPoint()]);
            init->push_back(newVertex);
            intVertex.push_back(newVertex);
        }
    }

    // Init connectedCompo and independentVertices
    isolateIndependantVertices();
    initConnectedComponent();
}

Graph::Graph(const csl::Expr& expr,
             std::map<csl::Tensor, size_t>& vertexId,
             bool                            t_ruleMode)
    :Graph(convertExprToFields(expr), vertexId, t_ruleMode)
{}

Graph::Graph(Graph const& other,
             std::vector<std::shared_ptr<Node>> const& newNodes)
    :Graph(other)
{
    sortedNodes.clear();
    HEPAssert(getNodes().size() == newNodes.size(),
            mty::error::ValueError,
            "Wrong number of nodes for graph " + toString(*this)
            + "\n  -> " + toString(newNodes.size()) + " nodes given.");
    size_t index = 0;
    for (auto& vertex : connectedCompo)
        for (auto& node : vertex)
            node = newNodes[index++];
}

Graph Graph::copy() const
{
    return Graph(*this, Node::copyGraph(getNodes()));
}

int Graph::getFactor() const
{
    return factor;
}

int Graph::getTotalDegeneracyFactor() const
{
    int deg_factor = 1;
    for (const auto& vertex : connectedCompo)
        deg_factor *= vertex.getTotalDegeneracyFactor();
    return deg_factor;
}

vector<Vertex> const &Graph::getVertices() const
{
    return connectedCompo.getVertices();
}

void Graph::addFactor(int t_factor) 
{
    factor += t_factor;
}

void Graph::setFactor(int t_factor)
{
    factor = t_factor;
}

void Graph::multiply(int t_factor) 
{
    factor *= t_factor;
}

void Graph::multiply(csl::Expr const &t_factor) 
{
    exprFactor *= t_factor;
}

void Graph::setSymmetrizeExternalLegs(bool t_symmetrize)
{
    symmetrizeExternalLegs = t_symmetrize;
}

bool Graph::empty() const
{
    return init->empty();
}

void Graph::isolateIndependantVertices()
{
    if (intVertex.size() == 0)
        return;
    if (intVertex.size() == 1) {
        independentVertices = vector<int>(1,0);
        return;
    }

    independentVertices = vector<int>(0);
    int nFamilies = 0;
    for(size_t i = 0; i < intVertex.size(); ++i) {
        independentVertices.push_back(i);
        const size_t index = i;
        ++nFamilies;
        for (size_t j = i+1; j < intVertex.size(); ++j) {
            assert(index < intVertex.size());
            if (intVertex[j].isDegenerateWith(intVertex[index])) {
                if (i+1 != j)
                    swap(intVertex[i+1], intVertex[j]);
                ++i;
            }
        }
    }
}

void Graph::removeIntVertex(int indexVertex)
{
    // Adjusting indices of independant vertices
    for (auto& index : independentVertices)
        if (index > indexVertex)
            --index;
    // Erasing indices of equivalent classes that are not present anymore
    for (size_t i = 0; i != independentVertices.size()-1; ++i) 
        if (independentVertices[i] == independentVertices[i+1]) {
            independentVertices.erase(independentVertices.begin()+i);
            --i;
        }
    if (independentVertices[independentVertices.size() - 1] 
            == (int)intVertex.size()-1)
        independentVertices.erase(independentVertices.end()-1);
    if (independentVertices.size() == 1)
        independentVertices[0] = 0;
    
    intVertex.erase(intVertex.begin()+indexVertex);
    if (intVertex.empty())
        independentVertices.clear();
}

void Graph::initConnectedComponent()
{
    if (extVertex.size() == 0) {
        connectedCompo = ConnectedComponent(intVertex[0]);
        removeIntVertex(0);
        return;
    }
    connectedCompo = ConnectedComponent(extVertex[0]);
    extVertex.erase(extVertex.begin());
}

bool Graph::isFullyConnected() const
{
    return (connectedCompo.size() == init->size() // all vertices are in
                                                 // the connected component
            and not connectedCompo.hasValenceLeft()); // And it has no free
                                                      // free node left.
}

bool Graph::isPhysical() const
{
    // Physical if not empty and not composed of more than one connected
    // component (if it is fully connected)
    return (not empty()
            and not (not connectedCompo.hasValenceLeft()
                     and not (connectedCompo.size() == init->size())));
}

// Returns the vertex in which one node is
Vertex const *Graph::getVertexOf(
        std::shared_ptr<Node> const &node,
        std::vector<Vertex> const &vertices
        ) 
{
    auto point = node->field->getPoint().get();
    for (const auto &v : vertices)
        if (v[0]->field->getPoint().get() == point)
            return &v;
    CallHEPError(mty::error::RuntimeError, "Partner of point " 
            + toString(point->getName()) + " not found.")
    return nullptr;
}

// Returns all nodes connected to another node
std::vector<std::shared_ptr<Node>> Graph::nextNodes(
        std::shared_ptr<Node> const &node,
        std::vector<Vertex> const &vertices
        )
{
    auto partner = node->partner.lock();
    Vertex const *nextVertex = getVertexOf(partner, vertices);
    if (nextVertex->size() == 1)
        return {};
    std::vector<std::shared_ptr<Node>> next;
    next.reserve(nextVertex->size() - 1);
    for (const auto &nextNode : *nextVertex)
        if (nextNode.get() != partner.get())
            next.push_back(nextNode);
    return next;
}

int Graph::countExternalLegs(
        std::vector<csl::Tensor>::iterator first,
        std::vector<csl::Tensor>::iterator last,
        std::vector<Vertex>          const &vertices
        )
{
    int nExt = 0;
    while (first != last) {
        auto pos = std::find_if(
                vertices.begin(), 
                vertices.end(), 
                [&](Vertex const &vertex) {
                   return vertex[0]->field->getPoint().get() == first->get(); 
                });
        nExt += pos->size() - 2;
        ++first;
    }
    return nExt;
}

int Graph::walk(
        std::vector<csl::Tensor>::iterator first,
        std::vector<csl::Tensor>::iterator last,
        std::shared_ptr<Node>        const &node,
        std::vector<Vertex>          const &vertices
        )
{
    *last = node->field->getPoint();
    for (auto iter = first; iter != last; ++iter)
        if (iter->get() == last->get()) {
            return countExternalLegs(iter, last, vertices);
        }
    ++last;
    auto next = nextNodes(node, vertices);
    if (next.empty())
        return -1;
    for (const auto &nextNode : next) {
        int res = walk(first, last, nextNode, vertices);
        if (res != -1) {
            return res;
        }
    }
    return -1;
}

bool Graph::isValid() const
{
    if (!isPhysical() or !isFullyConnected())
        return false;
    if (!mty::option::excludeTadpoles
            && !mty::option::excludeMassCorrections
            && !mty::option::excludeTriangles
            && !mty::option::excludeBoxes
            && !mty::option::excludePentagons)
        return true;
    auto const &vertices = connectedCompo.getVertices();
    std::vector<csl::Tensor> points(vertices.size());
    auto first = vertices[0][0];
    auto cycleLength = walk(points.begin(), points.begin(), first, vertices);
    if (cycleLength == 1 && mty::option::excludeMassCorrections)
        return false;
    if (cycleLength == 2 && mty::option::excludeMassCorrections)
        return false;
    if (cycleLength == 3 && mty::option::excludeTriangles)
        return false;
    if (cycleLength == 4 && mty::option::excludeBoxes)
        return false;
    if (cycleLength == 5 && mty::option::excludePentagons)
        return false;
    return true;
}

int Graph::getNLoops() const 
{
    const int nV = static_cast<int>(connectedCompo.size());
    const int nE = static_cast<int>(getNodes().size());
    return nE - (nV - 1);
}

int Graph::getFieldDimension() const
{
    int dimension = 0;
    for (const auto& vertex : *init)
        dimension += vertex.size();
    return dimension;
}

Graph::Expr_type Graph::getPartialExpression(
        vector<shared_ptr<Node>>&   nodes,
        const vector<QuantumField>& initialOrder,
        mty::FeynruleMomentum& witnessMapping,
        csl::Expr const&            globalFactor
        ) const
{
    HEPAssert(nodes.size()%2 == 0,
            mty::error::RuntimeError,
            "Odd number of fields in Graph::getExpression(), should not"+
            std::string(" arrive at this point..."));

    vector<const QuantumField*> finalOrder(0);
    // Setting contracted fields next to each other to get the final order
    // of fields, then compared to the initial for the commutation sign
    for (size_t i = 0; i < nodes.size()-1; i += 2) {
        const auto nodeA = nodes[i];
        finalOrder.push_back(nodes[i]->field);
        for (size_t j = i+1; j != nodes.size(); ++j) {
            auto& nodeB = nodes[j];
            if (nodeA->partner.lock().get() == nodeB.get()) {
                if (j != i+1)
                    swap(nodes[i+1], nodes[j]);
                finalOrder.push_back(nodes[i+1]->field);
                break;
            }
        }
    }
    HEPAssert(initialOrder.size() == finalOrder.size(),
            mty::error::RuntimeError,
            "Problem in application of Wick theorem, some connections"+
            std::string("are not properly recognized, wrong number of fields in")+
            "Graph::getExpression()...");

    csl::vector_expr terms_ampl(1, globalFactor);
    for (size_t i = 0; i < finalOrder.size(); i += 2) {
        if (finalOrder[i]->isComplexConjugate()
                and !finalOrder[i+1]->isComplexConjugate())
            std::swap(finalOrder[i], finalOrder[i+1]);
    }

    auto C_info = mty::ConjugationInfo::resolveFermionLines(
            finalOrder, initialOrder, ruleMode);
    finalOrder = C_info.getFinalOrder();
    for (size_t i = 0; i < finalOrder.size(); i += 2) {
        // Generating the integral (impulsion) variable in 4D for propagator.
        string Qname = "Q_" + toString(i/2 + 1);
        csl::Expr Q = generateTensor(Qname, {&Minkowski});
        Tensor parent = tensor_s(Qname, &Minkowski, Q);
        // Integral + Propagator
        terms_ampl.push_back(
                vectorintegral_s(finalOrder[i]->getPropagator(
                *finalOrder[i+1], parent), parent));
        // std::cout << "Propagator : " << '\n';
        // std::cout << *finalOrder[i] << " " << *finalOrder[i+1] << '\n';
        // std::cout << terms_ampl.back() << '\n';
        if (not witnessMapping.isEmpty()) {
            if (finalOrder[i]->isComplexConjugate()) {
                witnessMapping.push(*finalOrder[i+1], parent);
                witnessMapping.push(*finalOrder[i], {CSL_M_1, parent});
            }
            else {
                witnessMapping.push(*finalOrder[i], parent);
                witnessMapping.push(*finalOrder[i+1], {CSL_M_1, parent});
            }
        }
    }

    terms_ampl.push_back(int_s(getCommutationSign(initialOrder, finalOrder)));
    // std::cout << "SIGN : " << terms_ampl.back() << '\n';
    csl::Expr res = prod_s(terms_ampl, true);

    return {res, C_info};
}

std::vector<Graph::Expr_type> Graph::getExpression() const
{
    mty::FeynruleMomentum emptyMap;
    return getExpression(emptyMap);
}

std::vector<Graph::Expr_type> Graph::getExpression(
        mty::FeynruleMomentum& witnessMapping) const
{
    if (fixedExpression)
        return expression;

    HEPAssert(isFullyConnected(), mty::error::RuntimeError,
            std::string("Getting expression from a graph that")
            +" is not fully connected in"+ "Graph::getExpression()");

    int nVertices = connectedCompo.size();
    int nEdges = initialOrder->size() / 2;
    int nLoops = nEdges - nVertices + 1;
    csl::Expr globalFactor 
        = int_s(factor) * pow_s(2*CSL_PI, -4*nLoops) * exprFactor;
    expression.clear();
    vector<vector<shared_ptr<Node>>> symmetrizedNodes = symmetrize();
    for (auto& nodeSet : symmetrizedNodes) {
        expression.push_back(
                    getPartialExpression(nodeSet,
                                         *initialOrder,
                                         witnessMapping,
                                         globalFactor));
    }
    fixedExpression = true;

    return expression;
}

vector<const QuantumField*> Graph::getFields() const
{
    vector<const QuantumField*> res;
    for (const auto& vertex : connectedCompo)
        for (const auto& node : vertex) {
            res.push_back(node->field);
        }

    return res;
}

std::vector<const mty::QuantumField*> Graph::getConnectedFieldsInVertex(
        const mty::QuantumField* init) const
{
    for (const auto& vertex : connectedCompo)
        for (const auto& node : vertex)
            if (node->field->getParent_info() == init->getParent_info()
                    and init->getIndexStructureView() 
                    == node->field->getIndexStructureView()) {
                std::vector<const mty::QuantumField*> res;
                res.reserve(vertex.size()-1);
                for (const auto& otherNode : vertex)
                    if (node.get() != otherNode.get())
                        res.push_back(otherNode->field);
                return res;
            }
    return std::vector<const mty::QuantumField*>();
}

const mty::QuantumField* Graph::getConnectedField(
        const mty::QuantumField* init) const
{
    // std::vector<std::shared_ptr<Node>> nodes = getNodes();
    for (const auto& vertex : connectedCompo)
        for (const auto& node : vertex)
            if (node->field->getParent_info() == init->getParent_info()
                    and init->getIndexStructureView() 
                        == node->field->getIndexStructureView())
                return node->partner.lock()->field;
    
    return nullptr;
}

vector<std::shared_ptr<Node>> const& Graph::getNodes() const
{
    if (not sortedNodes.empty())
        return sortedNodes;
    sortedNodes = connectedCompo.getNodes();
    sortNodes(sortedNodes);
    sortedNodes.erase(sortedNodes.begin()+sortedNodes.size()/2,
                      sortedNodes.end());
    return sortedNodes;
}

vector<std::shared_ptr<Node>>& Graph::getNodes()
{
    if (not sortedNodes.empty())
        return sortedNodes;
    sortedNodes = connectedCompo.getNodes();
    sortNodes(sortedNodes);
    sortedNodes.erase(sortedNodes.begin()+sortedNodes.size()/2,
                      sortedNodes.end());
    return sortedNodes;
}

ObjectSymmetry<const QuantumField*> Graph::getSymmetry(
        const Vertex&       vertex,
        const QuantumField* field) const
{
    ObjectSymmetry<const QuantumField*> sym = Id_Sym(const QuantumField*);
    for (const auto& node : vertex) {
        const QuantumField* other = node->field;
        if (not areDegenerate(*field, *other) or
                other->getIndexStructureView().
                    exactMatch(field->getIndexStructureView()))
            continue;
        sym = sym + ObjectPermutation<const QuantumField*>(field, other);
    }

    return sym;
}

vector<vector<shared_ptr<Node>>> Graph::symmetrize() const
{
    vector<vector<shared_ptr<Node>>> res;
    vector<shared_ptr<Node>> basis = connectedCompo.getNodes();
    for (const auto& permutation : symmetry) {
        vector<shared_ptr<Node>> copy = copyNodes(basis);
        applySymmetry(copy, permutation);
        res.push_back(copy);
    }
    return res;
}

vector<shared_ptr<Node>> Graph::copyNodes(
        const vector<shared_ptr<Node>>& toCopy)
{
    map<Node*, bool> copied;
    vector<shared_ptr<Node>> copy;
    for (const auto& node : toCopy)
        if (not copied[node.get()]) {
            shared_ptr<Node> node1 = csl::make_shared<Node>(
                    node->field);
            shared_ptr<Node> node2 = csl::make_shared<Node>(
                    node->partner.lock()->field);
            contractNodes(node1, node2);
            copy.push_back(node1);
            copy.push_back(node2);
            copied[node.get()]                 = true;
            copied[node->partner.lock().get()] = true;
        }
    HEPAssert(copy.size() == toCopy.size(),
            mty::error::RuntimeError,
            "Issue during copy of std::vector<std::shared<Node>> in "
            + (string)"Graph::copyNodes()");

    return copy;
}

void Graph::applySymmetry(
        vector<shared_ptr<Node>>&              nodes,
        const ObjectPermutation<const QuantumField*>& permutation)
{
    for (auto& node : nodes)
        node->field = permutation[node->field];
}

vector<int> Graph::getContractibleExtVertices(const QuantumField* field) const
{
    vector<int> vertices(0);
    for (size_t i = 0; i != extVertex.size(); ++i) 
        if (extVertex[i].getDegeneracy(field) > 0) {
            vertices.push_back(i);
            if (not symmetrizeExternalLegs)
                return vertices;
        }
    return vertices;
}

vector<int> Graph::getContractibleIntVertices(const QuantumField* field) const
{
    vector<int> vertices(0);
    for (size_t i = 0; i != independentVertices.size(); ++i)   {
        const int begin = independentVertices[i];
        const int end   = (i != independentVertices.size()-1) ? 
            independentVertices[i+1] : intVertex.size();
        for (int index = begin; index != end; ++index){
            if (intVertex[index].getDegeneracy(field) > 0) {
                vertices.push_back(index);
                break;
            }
        }
    }

    return vertices;
}

vector<int> Graph::getContractibleConnectedVertices(
        const QuantumField* field) const
{
    return connectedCompo.getContractibleVertices(field);
}

bool Graph::contractExternal(const QuantumField* field,
                             int                 indexVertex)
{
    factor *= extVertex[indexVertex].getDegeneracy(field);
    if (not connectedCompo.connect(extVertex[indexVertex]))
        return false;
    connectedCompo.addVertex(extVertex[indexVertex]);
    extVertex.erase(extVertex.begin()+indexVertex);
    return true;
}

bool Graph::contractInternal(const QuantumField* field,
                             int                 indexVertex)
{
    int deg = intVertex[indexVertex].getDegeneracy(field);
    if (not symmetrizeExternalLegs and deg > 1 and field->isIndexed()) {
        const QuantumField* chosen = intVertex[indexVertex].
            getFirstContractibleNode(field)->field;
        symmetry = symmetry * getSymmetry(intVertex[indexVertex],
                                          chosen);
    }
    else if (deg > 1)
        factor *= deg;

    if (not connectedCompo.connect(intVertex[indexVertex]))
        return false;
    connectedCompo.addVertex(intVertex[indexVertex]);
    removeIntVertex(indexVertex);

    return true;
}

bool Graph::contractConnected(const QuantumField* /*field*/,
                              int                 indexVertex)
{
    if (factor *= connectedCompo.connect(indexVertex);
            factor == -1)
        return false;
    else
        return true;
}

std::vector<std::shared_ptr<Graph>> Graph::contractionStep() const
{
    constexpr bool display = false;
    if constexpr (display)
        std::cout << "\n***\nContraction step for " << *this << endl;

    sortedNodes.clear();
    const QuantumField* field = connectedCompo.getFirstContractibleField();

    vector<int> extContractile;
    if (not field->isExternal())
        extContractile = getContractibleExtVertices(field);
    vector<int> intContractile = 
        getContractibleIntVertices(field);
    vector<int> connectedContractile = 
        getContractibleConnectedVertices(field);

    if constexpr (display) {
        std::cout << "Contracted field = " << field->printLaTeX(1) << endl;
        std::cout << "External options:\n";
        for (const auto& i : extContractile)
            std::cout << i << "  ";
        std::cout << endl;

        std::cout << "Internal options:\n";
        for (const auto& i : intContractile)
            std::cout << i << "  ";
        std::cout << endl;

        std::cout << "Options in connected component:\n";
        for (const auto& i : connectedContractile)
            std::cout << i << "  ";
        std::cout << endl;
    }

    std::vector<std::shared_ptr<Graph>> 
        newGraphs(extContractile.size()
                 +intContractile.size()
                 +connectedContractile.size());
    for (auto& g : newGraphs)
        g = std::make_shared<Graph>(*this);
    
    // vector<int> nullDiagrams;
    vector<bool> nullDiagrams(newGraphs.size(), false);
    nullDiagrams.reserve(extContractile.size()
                       + intContractile.size()
                       + connectedContractile.size());
    for (size_t i = 0; i != extContractile.size(); ++i)  
        if (not newGraphs[i]->contractExternal(field, extContractile[i]))
            nullDiagrams[i] = true;

    size_t size = extContractile.size();
    for (size_t i = 0; i != intContractile.size(); ++i) 
        if (not newGraphs[size+i]->contractInternal(field, intContractile[i]))
            nullDiagrams[size+i] = true;

    size += intContractile.size();
    for (size_t i = 0; i != connectedContractile.size(); ++i)  
        if (not newGraphs[size+i]->contractConnected(
                    field, connectedContractile[i]))
            nullDiagrams[size+i] = true;

    std::vector<std::shared_ptr<Graph>> nonZeroDiagrams;
    nonZeroDiagrams.reserve(newGraphs.size());
    for (size_t i = 0; i != newGraphs.size(); ++i)
        nonZeroDiagrams.emplace_back(std::move(newGraphs[i]));

    if constexpr (display) {
        std::cout << nonZeroDiagrams.size() << " new diagrams found:\n";
        for (const auto& g : nonZeroDiagrams)
            std::cout << *g << endl;
    }

    return nonZeroDiagrams;
}

bool Graph::compareFieldsDummyPoint(
        const QuantumField*                    fieldA,
        const QuantumField*                    fieldB,
        map<csl::Tensor, csl::Tensor>& constraints,
        bool fieldBlind)
{
    if (not fieldBlind and (fieldA->isExternal() or fieldB->isExternal()))
        return *fieldA == *fieldB;
    if (fieldBlind 
            and fieldA->isExternal() 
            and fieldB->isExternal()
            and fieldA->isIncoming() != fieldB->isIncoming()) {
        return false;
    }
    if (fieldBlind
            and fieldA->isExternal() != fieldB->isExternal()) {
        return false;
    }
    if (fieldA->isComplexConjugate() != fieldB->isComplexConjugate())
        return false;

    csl::Tensor pointA = fieldA->getPoint();
    csl::Tensor pointB = fieldB->getPoint();
    auto posA = constraints.find(pointA);
    auto posB = constraints.begin();
    while (posB != constraints.end()) {
        if (posB->second == pointB)
            break;
        ++posB;
    }

    if (posA != constraints.end()) {
        bool res = constraints[pointA] == pointB;
        return res;
    }
    else {
        if (posB != constraints.end()) {
            return false;
        }
        constraints[pointA] = pointB;
        // constraints[pointB] = pointA;
        return true;
    }
}

bool Graph::compareNodesWithConstraints(
        const shared_ptr<Node>&        nodeA,
        const shared_ptr<Node>&        nodeB,
        map<csl::Tensor, csl::Tensor>& constraints,
        bool                           fieldBlind)
{
    // if (nodeA->field->getIndexStructureView().size() > 0
    //         or nodeB->field->getIndexStructureView().size() > 0)
    //     return false;
    const QuantumField* fieldA1 = nodeA->field;
    const QuantumField* fieldA2 = nodeA->partner.lock()->field;
    const QuantumField* fieldB1 = nodeB->field;
    const QuantumField* fieldB2 = nodeB->partner.lock()->field;

    bool swapped = false;
    if (not fieldBlind) {
        // Fields must have same parent and same externality to be compared,
        // we test first and try to swap. If nothing corresponds, it's false
        if (not (fieldA1->getParent().get() == fieldB1->getParent().get()
             and fieldA2->getParent().get() == fieldB2->getParent().get())
         or not (fieldA1->isExternal() == fieldB1->isExternal()
             and fieldA2->isExternal() == fieldB2->isExternal())) {

            swap(fieldB1, fieldB2);
            swapped = true;
            if (not (fieldA1->getParent().get() == fieldB1->getParent().get()
                 and fieldA2->getParent().get() == fieldB2->getParent().get())){
                    return false;
            }
            if (not (fieldA1->isExternal() == fieldB1->isExternal()
                 and fieldA2->isExternal() == fieldB2->isExternal()))  {
                return false;
            }
        }
    }
    else {
        if ((fieldA1->isExternal() and fieldB1->isExternal()
                    and fieldA1->isIncoming() != fieldB1->isIncoming())
                or (fieldA2->isExternal() and fieldB2->isExternal()
                    and fieldA2->isIncoming() != fieldB2->isIncoming())) {
            swap(fieldB1, fieldB2);
            swapped = true;
            if ((fieldA1->isExternal() and fieldB1->isExternal()
                        and fieldA1->isIncoming() != fieldB1->isIncoming())
                    or (fieldA2->isExternal() and fieldB2->isExternal()
                        and fieldA2->isIncoming() != fieldB2->isIncoming())) {
                return false;
            }
        }
    }
    // Here parents are the same and are face to face A1<->B1 A2<->B2
    map<csl::Tensor, csl::Tensor> copyConstraints(constraints);
    if (compareFieldsDummyPoint(fieldA1, fieldB1, copyConstraints, fieldBlind)
    and compareFieldsDummyPoint(fieldA2, fieldB2, copyConstraints, fieldBlind))
    {
        constraints = copyConstraints;
        return true;
    }
    if (not swapped) {
        copyConstraints = constraints;
        if (compareFieldsDummyPoint(fieldA1, fieldB2, copyConstraints, fieldBlind)
        and compareFieldsDummyPoint(fieldA2, fieldB1, copyConstraints, fieldBlind))
        {
            constraints = copyConstraints;
            return true;
        }
    }
    return false;
}

void Graph::addFoundNode(const shared_ptr<Node>&  newNode,
                         vector<csl::Tensor>& foundNodes)
{
    const QuantumField* f1 = newNode->field;
    const QuantumField* f2 = newNode->partner.lock()->field;
    csl::Tensor point1 = f1->getPoint();
    csl::Tensor point2 = f2->getPoint();

    if (not f1->isExternal())
        if (auto pos = find(foundNodes.begin(), foundNodes.end(), point1);
                pos == foundNodes.end())
            foundNodes.push_back(point1);
    if (not f2->isExternal())
        if (auto pos = find(foundNodes.begin(), foundNodes.end(), point2);
                pos == foundNodes.end())
            foundNodes.push_back(point2);
}

void Graph::sortNodes(vector<shared_ptr<Node>>& nodes)
{
    if (nodes.empty())
        return;
    vector<Tensor> foundNodes;
    for (size_t i = 0; i != nodes.size()-1; ++i) {
        size_t iPrio = i;
        for (size_t j = i+1; j < nodes.size(); ++j)
            if (comparePriority(nodes[j], nodes[iPrio], foundNodes)) {
                iPrio = j;
            }
        if (iPrio != i) {
            addFoundNode(nodes[iPrio], foundNodes);
            swap(nodes[i], nodes[iPrio]);
        }
    }
}

bool Graph::compare(const Graph& other,
                    std::map<csl::Tensor, csl::Tensor>& constraints,
                    bool         fieldBlind) const
{
    if (not isFullyConnected() or not other.isFullyConnected())
        return false;
    if (connectedCompo.size() != other.connectedCompo.size())
        return false;

    vector<shared_ptr<Node>> selfNodes  = getNodes();
    vector<shared_ptr<Node>> otherNodes = other.getNodes();
    // sortNodes(selfNodes);
    // sortNodes(otherNodes);
    vector<size_t> indicesLeft(otherNodes.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;
    vector<shared_ptr<Node>> partnersMatched;
    for (size_t i = 0; i != selfNodes.size(); ++i) {
        shared_ptr<Node> node = selfNodes[i];
        if (find(partnersMatched.begin(), partnersMatched.end(), node)
                != partnersMatched.end())
            continue;
        bool matched = false;
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            shared_ptr<Node> nodeB = otherNodes[indicesLeft[j]];
            if (compareNodesWithConstraints(
                        node, nodeB, constraints, fieldBlind)) {
                indicesLeft.erase(indicesLeft.begin()+j);
                partnersMatched.push_back(node->partner.lock());
                matched = true;
                break;
            }
        }
        if (not matched) {
            return false;
        }
    }
    return true;
}

bool Graph::compare(const Graph& other,
                    bool         fieldBlind) const
{
    std::map<csl::Tensor, csl::Tensor> constraints;
    return compare(other, constraints, fieldBlind);
}

void Graph::contractHighMass(
        mty::QuantumFieldParent const *field
        )
{
    for (size_t i = 0; i != connectedCompo.size(); ++i) {
        Vertex &vertex = connectedCompo[i];
        for (size_t j = 0; j != vertex.size(); ++j) {
            if (vertex[j]->field->getParent_info() == field) {
                auto partner = vertex[j]->partner.lock();
                vertex.erase(vertex.begin() + j);
                for (size_t k = 0; k != connectedCompo.size(); ++k)
                    for (size_t l = 0; l != connectedCompo[k].size(); ++l)
                        if (connectedCompo[k][l].get() == partner.get()) {
                            connectedCompo[k].erase(
                                    connectedCompo[k].begin() + l);
                            vertex.insert(
                                    vertex.end(),
                                    std::make_move_iterator(
                                        connectedCompo[k].begin()),
                                    std::make_move_iterator(
                                        connectedCompo[k].end()
                                        )
                                    );
                            connectedCompo.erase(connectedCompo.begin() + k);
                            contractHighMass(field);
                            sortedNodes.clear();
                            return;
                        }
            }
        }
    }
}

bool Graph::operator==(const Graph& other) const
{
    return compare(other);
}

ostream& operator<<(ostream&     fout,
                    const wick::Graph& g)
{
    fout << "Graph: factor = " << g.factor << endl;
    fout << g.connectedCompo << endl;
    fout << "Extern vertices       = "; 
    for (const auto& vertex : g.extVertex)
        fout << vertex << endl;
    fout << endl;
    fout << "Integrated vertices   = "; 
    for (const auto& vertex : g.intVertex)
        fout << vertex << endl;

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// WickCalculator class                          //
/*************************************************/
///////////////////////////////////////////////////

csl::Expr applyConjugation(
        std::vector<Graph::Expr_type> expressions,
        bool                          //ruleMode = true
        )
{
    std::vector<csl::Expr> terms;
    terms.reserve(expressions.size());
    for (auto &expr : expressions) {
        expr.first = csl::DeepCopy(expr.first);
        // ConjugationInfo::applyConjugation(
        //         expr.second, expr.first, ruleMode
        //         );
        terms.push_back(expr.first);
    }

    return csl::sum_s(terms);
}

WickCalculator::WickCalculator()
{}

WickCalculator::WickCalculator(const Graph& freeDiagram)
    :initialDiagram(freeDiagram)
{}

void WickCalculator::setSymmetrizeExternalLegs(bool t_symmetrize)
{
    symmetrizeExternalLegs = t_symmetrize;
    initialDiagram.setSymmetrizeExternalLegs(t_symmetrize);
}

Graph WickCalculator::getInitialDiagram() const
{
    return initialDiagram;
}

std::vector<std::shared_ptr<Graph>> WickCalculator::getDiagrams()
{
    if (not diagramsCalculated) {
        calculateDiagrams();
        diagramsCalculated = true;
    }

    return feynmanDiagram;
}

void WickCalculator::eliminateNonPhysicalDiagrams(
        std::vector<std::shared_ptr<Graph>>& diagrams)
{
    for (size_t i = 0; i != diagrams.size(); ++i) 
        if (not diagrams[i]->isPhysical()) {
            diagrams.erase(diagrams.begin()+i);
            --i;
        }
}

void WickCalculator::calculateDiagrams()
{
    int dim = initialDiagram.getFieldDimension();
    if (dim % 2 != 0 or dim == 0)
        return;

    feynmanDiagram = std::vector<std::shared_ptr<Graph>>(
            1, std::make_shared<Graph>(initialDiagram));

    // Root of wick contraction algorithm, applies contraction steps on all
    // diagrams up to get only physical fully connected diagrams.
    for (int i = 0; i != dim/2; ++i) {
        std::vector<std::shared_ptr<Graph>> intermediate;
        for (const auto& graph : feynmanDiagram) {
            std::vector<std::shared_ptr<Graph>> foo 
                = graph->contractionStep();
            intermediate.insert(intermediate.end(), foo.begin(), foo.end());
        }
        eliminateNonPhysicalDiagrams(intermediate);
        if (intermediate.size() == 0) 
            break;
        feynmanDiagram = std::move(intermediate);
    }

    // Checking all resulting diagrams
    for (size_t i = 0; i != feynmanDiagram.size(); ++i) {
        if (!feynmanDiagram[i]->isValid()) {
            feynmanDiagram.erase(feynmanDiagram.begin()+i);
            --i;
        }
    }

    // Merging identical diagrams
    if (not feynmanDiagram.empty()) {
        for (size_t i = 0; i != feynmanDiagram.size(); ++i)  {
            if (feynmanDiagram[i]->getFactor() == 0) {
                continue;
            }
            for (size_t j = i+1; j < feynmanDiagram.size(); ++j)
                if (*feynmanDiagram[i] == *feynmanDiagram[j]) {
                    feynmanDiagram[i]->addFactor(feynmanDiagram[j]->getFactor());
                    feynmanDiagram.erase(feynmanDiagram.begin() + j);
                    --j;
                }
        }
        // applyExternalLegsPermutations(feynmanDiagram);
    }
}

csl::Expr WickCalculator::applyWickTheoremOnDiagram(
        const Graph& diagram,
        std::vector<mty::FeynruleMomentum>& witnessMapping,
        bool ruleMode)
{
    if (ruleMode) {
        auto res = (witnessMapping.empty()) ?
            diagram.getExpression()
            :diagram.getExpression(witnessMapping[0]);
        std::vector<csl::Expr> terms;
        terms.reserve(res.size());
        for (const auto &el : res)
            terms.push_back(el.first);
        return csl::sum_s(terms);
    }
    if (witnessMapping.empty())
        return applyConjugation(diagram.getExpression(), ruleMode);
    return applyConjugation(diagram.getExpression(witnessMapping[0]), ruleMode);
}

csl::vector_expr WickCalculator::applyWickTheoremOnDiagrams(
        const std::vector<std::shared_ptr<Graph>>& diagrams,
        std::vector<mty::FeynruleMomentum>& witnessMapping,
        bool ruleMode)
{
    return convertGraphsToCorrelators(diagrams, witnessMapping, ruleMode);
}

std::vector<mty::FeynmanDiagram> WickCalculator::getDiagrams(
        mty::Model const *model,
        const csl::Expr& initial,
        std::map<csl::Tensor, size_t>& vertexIds,
        bool symmetrizeExternalLegs,
        bool ruleMode)
{
    std::vector<mty::FeynruleMomentum> emptyMap;
    return getDiagrams(
            model, 
            initial, 
            vertexIds, 
            emptyMap, 
            symmetrizeExternalLegs, 
            ruleMode
            );
}

std::vector<mty::FeynmanDiagram> WickCalculator::getDiagrams(
        mty::Model const *model,
        const csl::Expr& initial,
        std::map<csl::Tensor, size_t>& vertexIds,
        std::vector<mty::FeynruleMomentum>& witnessMapping,
        bool symmetrizeExternalLegs,
        bool ruleMode)
{
    csl::Expr factor = CSL_1;
    vector<QuantumField> fields;
    if (initial->getType() == csl::Type::Prod) {
        for (const auto& arg : *initial)
            if (IsOfType<Wick>(arg))
                fields = convertExprToFields(arg->getOperand());
            else
                factor = prod_s(factor, arg, true);
    }
    else if (IsOfType<Wick>(initial))
        fields = convertExprToFields(initial->getOperand());
    if (fields.empty())
        return {};
    factor = Refreshed(factor);
    WickCalculator calculator = WickCalculator(
            Graph(fields, vertexIds, ruleMode));
    calculator.setSymmetrizeExternalLegs(symmetrizeExternalLegs);
    auto diagrams = calculator.getDiagrams();
    for (const auto &d : diagrams)
        d->multiply(factor);

    csl::vector_expr results = applyWickTheoremOnDiagrams(diagrams,
                                                          witnessMapping,
                                                          ruleMode);
    std::vector<mty::FeynmanDiagram> res;
    res.reserve(diagrams.size());
    for (size_t i = 0; i != diagrams.size(); ++i) {
        if (diagrams[i]->getFactor() == 0) {
            if (not witnessMapping.empty())
                witnessMapping.erase(witnessMapping.begin() + i);
            diagrams.erase(diagrams.begin() + i);
            results.erase(results.begin() + i);
            --i;
        }
        else {
            res.emplace_back(*model, results[i], diagrams[i]);
        }
    }

    return res;
}

bool WickCalculator::isContractionZero(const vector<QuantumField>& fields,
                                       size_t maxLoops,
                                       size_t nVertices)
{
    size_t nLoops = fields.size()/2 - (nVertices - 1);
    if (maxLoops != size_t(-1) and nVertices != size_t(-1)) {

        if (mty::option::discardLowerOrders and nLoops != maxLoops)
            return true;
        else if (nLoops > maxLoops)
            return true;
    }
    const vector<vector<QuantumField>> split = splitFields(fields);
    for (const auto& vec : split) {
        if (vec.size() % 2 != 0)
            return true;
        bool allExternal = true;
        size_t nConjugated = 0;
        for (const auto& field : vec) {
            if (field.isComplexConjugate())
                ++nConjugated;
            if (not field.isExternal()) {
                allExternal = false;
            }
        }
        if (allExternal)
            return true;
        if (not vec[0].isSelfConjugate() and nConjugated != vec.size() / 2)
            return true;
    }
    return false;
}

bool WickCalculator::isContractionZero(
        const vector<QuantumField const*>& fields,
        size_t maxLoops,
        size_t nVertices)
{
    size_t nLoops = fields.size()/2 - (nVertices - 1);
    if (maxLoops != size_t(-1) and nVertices != size_t(-1)) {

        if (mty::option::discardLowerOrders and nLoops != maxLoops)
            return true;
        else if (nLoops > maxLoops)
            return true;
    }
    const vector<vector<QuantumField const*>> split = splitFields(fields);
    for (const auto& vec : split) {
        if (vec.size() % 2 != 0)
            return true;
        bool allExternal = true;
        size_t nConjugated = 0;
        for (const auto& field : vec) {
            if (field->isComplexConjugate())
                ++nConjugated;
            if (not field->isExternal()) {
                allExternal = false;
            }
        }
        if (allExternal)
            return true;
        if (not vec[0]->isSelfConjugate() and nConjugated != vec.size() / 2)
            return true;
    }
    return false;
}

vector<vector<QuantumField>> WickCalculator::splitFields(
        const vector<QuantumField>& field)
{
    vector<int> fieldsLeft(field.size());
    for (size_t i = 0; i != field.size(); ++i) 
        fieldsLeft[i] = i;
    vector<vector<QuantumField>> split;
    split.reserve(fieldsLeft.size());
    for (size_t i = 0; i != fieldsLeft.size() ;) {
        const int index = fieldsLeft[0];
        vector<QuantumField> splitElement;
        splitElement.reserve(fieldsLeft.size() + 1);
        splitElement.push_back(field[index]);
        fieldsLeft.erase(fieldsLeft.begin());
        for (size_t j = 0; j != fieldsLeft.size(); ++j) {
            const int jndex = fieldsLeft[j];
            if (field[index].isContractibleWith(field[jndex])) {
                splitElement.push_back(std::move(field[jndex]));
                fieldsLeft.erase(fieldsLeft.begin()+j);
                --j;
            }
        }
        split.push_back(std::move(splitElement));
    }
    return split;
}

vector<vector<QuantumField const*>> WickCalculator::splitFields(
        const vector<QuantumField const*>& field)
{
    vector<int> fieldsLeft(field.size());
    for (size_t i = 0; i != field.size(); ++i) 
        fieldsLeft[i] = i;
    vector<vector<QuantumField const*>> split;
    split.reserve(fieldsLeft.size());
    for (size_t i = 0; i != fieldsLeft.size() ;) {
        const int index = fieldsLeft[0];
        vector<QuantumField const*> splitElement;
        splitElement.reserve(1 + field.size());
        splitElement.push_back(field[index]);
        fieldsLeft.erase(fieldsLeft.begin());
        for (size_t j = 0; j != fieldsLeft.size(); ++j) {
            const int jndex = fieldsLeft[j];
            if (field[index]->isContractibleWith(*field[jndex])) {
                splitElement.push_back(field[jndex]);
                fieldsLeft.erase(fieldsLeft.begin()+j);
                --j;
            }
        }
        split.push_back(std::move(splitElement));
    }
    return split;
}


///////////////////////////////////////////////////
/*************************************************/
// Support functions                             //
/*************************************************/
///////////////////////////////////////////////////

void contractNodes(const std::shared_ptr<Node>& node1,
                   const std::shared_ptr<Node>& node2)
{
    node1->partner = node2;
    node2->partner = node1;
}

bool areDegenerate(const QuantumField& f1,
                   const QuantumField& f2)
{
    if (f1.getQuantumParent() != f2.getQuantumParent()
            and not f1.isContractibleWith(f2))
        return false;
    if (f1.isSelfConjugate())
        return true;
    return (f1.isComplexConjugate() == f2.isComplexConjugate());
}

bool areExactlyContractible(const QuantumField& f1,
                            const QuantumField& f2)
{
    return f1.isExactlyContractiblewith(f2);
}

ostream& operator<<(ostream&                fout,
                    const shared_ptr<Node>& n)
{
    fout << n->field;
    if (not n->partner.lock()) 
        fout << " is contracted!";

    return fout;
}

bool operator==(const shared_ptr<Node>& A,
                const shared_ptr<Node>& B)
{
    return (A.get() == B.get());
}

bool comparePriority(const shared_ptr<Node>&        A,
                     const shared_ptr<Node>&        B,
                     const vector<Tensor>& foundNodes)
{
    if (not internal_comparePriority(A, A->partner.lock(), foundNodes))
        return false;
    if (not internal_comparePriority(B, B->partner.lock(), foundNodes))
        return true;
    return internal_comparePriority(A, B, foundNodes);
}

bool internal_comparePriority(const shared_ptr<Node>&        A,
                              const shared_ptr<Node>&        B,
                              const vector<Tensor>& foundNodes)
{
    bool extA = A->field->isExternal();
    bool extB = B->field->isExternal();
    // External has the priority over internal node
    if (extA and not extB)
        return true;
    if (extB and not extA)
        return false;
    // Here both external or both internal
    if (extA)  {
        if (csl::compare(A->field->getName(), B->field->getName()) == -1)
            return true;
        else if (A->field->getName() == B->field->getName())
            return (csl::compare(A->field->getPoint()->getName(),
                                 B->field->getPoint()->getName()) == -1);
    }
    // Here both internal
    Tensor pointA = A->field->getPoint();
    Tensor pointB = B->field->getPoint();
    for (const auto& point : foundNodes) 
        if (point == pointA)
            return true;
        else if (point == pointB)
            return false;
    return csl::compare(pointA->getName(), pointB->getName()) == -1;
}

vector<QuantumField> convertExprToFields(const csl::Expr& expr)
{
    if (IsOfType<QuantumField>(expr)) 
        return vector<QuantumField>(1,ConvertTo<QuantumField>(expr));
    else if (expr->getType() == csl::Type::Prod) {
        vector<QuantumField> res(0);
        for (const auto& el : expr->getVectorArgument()) {
            if (el->getType() == csl::Type::Pow) {
                HEPAssert(IsOfType<QuantumField>(el->getArgument(0)) 
                        and el->getArgument(1)->isInteger(),
                        mty::error::ValueError,
                        "Cannot create Graph with objects that are not\
                        QuantumField");
                for (int i = 0; i != el->getArgument(1)->evaluateScalar(); ++i)
                    res.push_back(ConvertTo<QuantumField>(
                                el->getArgument(0)));
                continue;
            }
            HEPAssert(IsOfType<QuantumField>(el),
                mty::error::ValueError,
                "Cannot create Graph with objects that are not QuantumField\
                ");
            res.push_back(ConvertTo<QuantumField>(el));
        }
        return res;
    }
    else if (expr->getType() == csl::Type::Pow) {
        vector<QuantumField> res(0);
        HEPAssert(IsOfType<QuantumField>(expr->getArgument(0)) 
                and expr->getArgument(1)->isInteger(),
                mty::error::ValueError,
                "Cannot create Graph with objects that are not\
                QuantumField");
        for (int i = 0; i != expr->getArgument(1)->evaluateScalar(); ++i)
            res.push_back(ConvertTo<QuantumField>(
                        expr->getArgument(0)));

        return res;
    }
    else {
        CallHEPError(mty::error::ValueError,
                "The initializator of a graph must be either a field\
                or a procduct of fields");
        return vector<QuantumField>(0);
    }
}

int countFermions(vector<const QuantumField*>::const_iterator begin,
                  vector<const QuantumField*>::const_iterator end)
{
    int nFermions = 0;
    for (auto it = begin; it != end; ++it)
        if ((**it).isAntiCommuting())
            ++nFermions;
    return nFermions;
}

int getCommutationSign(const std::vector<const mty::QuantumField*>& A,
                       std::vector<const mty::QuantumField*>       B)
{
    std::vector<mty::QuantumField> A_static(A.size());
    for (size_t i = 0; i != A.size(); ++i) 
        A_static[i] = *A[i];
    return getCommutationSign(A_static, B);
}

int getCommutationSign(const std::vector<QuantumField>& A,
                       std::vector<const QuantumField*> B)
{
    int sign = 1;
    // for (const auto& a : A)
    //     a.print(1);
    // std::cout << std::endl;
    // for (const auto& b : B)
    //     b->print(1);
    // std::cout << std::endl;
    for (size_t i = 0; i != A.size(); ++i)
        for (size_t j = i+1; j < B.size(); ++j) 
            if (A[i] == *B[j]) {
                if (B[j]->isAntiCommuting()) {
                    int nF = countFermions(B.begin()+i, B.begin()+j);
                    sign *= pow(-1, nF);
                    // std::cout << "SIGN *= " << nF << std::endl;
                }
                // std::cout << "SWAPPING" << std::endl;
                // B[i]->print();
                // B[j]->print();
                auto save = B[j];
                B.erase(B.begin() + j);
                B.insert(B.begin()+i, save);
                break;
            }
    // std::cout << "Sign = " << sign << std::endl;
    return sign;
}

csl::vector_expr convertGraphsToCorrelators(
        const std::vector<std::shared_ptr<Graph>>& diagrams,
        std::vector<mty::FeynruleMomentum>& witnessMapping,
        bool ruleMode)
{
    csl::vector_expr res(0);
    res.reserve(diagrams.size());
    std::vector<mty::FeynruleMomentum> newMapping;
    newMapping.reserve(diagrams.size());
    for (const auto& diag : diagrams) {
        if (not witnessMapping.empty()) {
            newMapping.push_back(witnessMapping[0]);
            res.push_back(
                    applyConjugation(diag->getExpression(newMapping.back()),
                                     ruleMode));
        }
        else
            res.push_back(applyConjugation(diag->getExpression(),
                                           ruleMode));
    }
    witnessMapping = std::move(newMapping);

    return res;
}

} // End of namespace mty::wick
