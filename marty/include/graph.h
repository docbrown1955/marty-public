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

/*! \file graph.h
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Handles graphs for applying wick theorem. In this file lies the
 * algorithm performing Wick contractions, i.e. that finds all different 
 * Feynman diagrams (with their numerical factor) that intervenes in a given 
 * process (i.e. correlation function).
 */
#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "quantumField.h"
#include "fermionFlow.h"

namespace mty {
class FeynruleMomentum;
class FeynmanDiagram;
class ConjugationList;
class Model;
}

namespace mty::wick {

/*!
 * 
 * \brief Represents a contractible mty::QuantumField in the context of Wick 
 * contraction.
 * \details A node is by definition a single power of mty::QuantumField. It 
 * means that a vertex with non trivial powers of a field will be divided in 
 * several nodes, one for each power. For example, 
 * \f$ \frac{\lambda}{3}\phi (X)^3 \f$ is divided in 3 nodes, each Node being 
 * for the field \f$ \phi (X) \f$. A node can be contracted with another and 
 * posess a pointer to its partner in this case. Only std::shared_ptr to Node 
 * are used in the program (Node objects that are already contracted are shared 
 * by several graphs) and the pointer to the partner is then a std::weak::ptr to 
 * avoid a circular owning pattern.
 */
class Node: public std::enable_shared_from_this<Node>{

    public:

    /*!
     * \brief Constructor with one parameter. Initializes a NULL partner.
     * \param t_field mty::QuantumField represented by the node.
     */
    explicit
    Node(const mty::QuantumField* t_field): field(t_field){};

    /*!
     * \brief Copy constructor = C++ default.
     */
    Node(const Node& node) = default;

    /*!
     * \brief Copy constructor = C++ default.
     */
    Node(Node&& node) = default;

    /*!
     * \brief Tells if the Node is free or not.
     * \return \b True  if the Node has no partner.
     * \return \b False if the Node has a non NULL partner, i.e. is contracted.
     */
    bool isFree() const {
        return (not partner.lock());
    };

    static std::vector<std::shared_ptr<Node>> copyGraph(
            std::vector<std::shared_ptr<Node>> const& init);

    public:

    /*!
     * \brief mty::QuantumField represented by the node in the graph.
     */
    const mty::QuantumField *field;

    /*!
     * \brief std::weak_ptr to the Node with which it is contracted, if it is.
     * Else the value is empty.
     */
    std::weak_ptr<Node> partner;
};

// Forward declaration of ConnectedComponent class
class ConnectedComponent;

/*!
 * 
 * \brief Represents a Vertex in feynman diagrams, i.e. a set of Nodes at the 
 * same space-time point.
 */
class Vertex: public std::vector<std::shared_ptr<Node>> {

    public:

    /*!
     * \brief Default constructor, initializes an empty Vertex.
     */
    Vertex();

    /*!
     * \brief Constructor with 2 parameters.
     * \param fields     std::vector of mty::QuantumField in the Vertex.
     * \param t_external Tells if the vertex is external (field insertion in
     * correlator).
     */
    explicit
    Vertex(const std::vector<const mty::QuantumField*>& fields,
           size_t                                       id,
           bool                                         t_external=false);

    /*!
     * \brief Copy constructor. See Vertex::operator=().
     */
    Vertex(const Vertex& other);

    /*!
     * \return external (i.e. if the Vertex comes from field insertions).
     */
    bool isExternal() const;

    /*!
     * \return \b True  if the Vertex is free, i.e. has no contraction.
     * \return \b False else.
     */
    bool isFree() const;

    /*!
     * \return \b True  if the Vertex has at least one free Node.
     * \return \b False else.
     */
    bool hasFreeNode() const;

    /*!
     * \brief Tells if a Vertex can be contracted with (has free nodes for) a 
     * particular mty::QuantumField.
     * \param field mty::QuantumField we want to contract.
     * \return \b True  if the Vertex has at least one contractible Node with
     * \b field (see isExactlyContractiblewith()).
     */
    bool hasFreeNode(const mty::QuantumField* field) const;

    bool containsExplicit(const mty::QuantumField* field) const;

    /*!
     * \brief Tells if the Vertex is degenerate with another.
     * \return \b True if the two vertices have the same field content.
     * \return \b False else.
     */
    bool isDegenerateWith(const Vertex& other) const;

    /*!
     * \brief Returns the field associated to the first contractible Node of the
     * Vertex. This function calls an error if the Vertex is fully contracted.
     * \return The mty::QuantumField of the first contractible Node.
     */
    const mty::QuantumField* getFirstContractibleField() const;

    /*!
     * \return A shared_ptr to the the first constractible Node if ther is one.
     * \return \b nullptr else.
     */
    std::shared_ptr<Node> getFirstContractibleNode() const; 

    /*!
     * \brief Returns the first Node contractible with \b field if there is one.
     * \param field mty::QuantumField we want to contract.
     * \return A shared_ptr to the the first Node contractible with \b field
     * if ther is one.
     * \return \b nullptr else.
     */
    std::shared_ptr<Node> getFirstContractibleNode(
            const mty::QuantumField* field) const;

    /*!
     * \return A std::vector filled with all contractible Node objects
     * in the Vertex.
     */
    std::vector<std::shared_ptr<Node>> getContractibleNodes() const;

    std::vector<std::shared_ptr<Node>> getContractibleNodes(
            const mty::QuantumField* field) const;

    /*!
     * \brief Sets the external property of the Vertex. \b True means that the
     * Vertex is composed of field insertions.
     */
    void setExternal(bool t_external);
    
    /*!
     * \brief Computes and returns the degeneracy of the Vertex with respect to
     * a given mty::QuantumField. The degeneracy is the number of free Nodes 
     * contractible with \b field. See isExactlyContractiblewith().
     * \param t_field mty::QuantumField we want to contract.
     * \return The number of free Node contractible with \b field.
     */
    int getDegeneracy(const mty::QuantumField* t_field) const;

    int getTotalDegeneracyFactor() const;

    /*!
     * \brief Overload of operator= for Vertex.
     * \details A little subtlety lies there. Nodes that are already contracted 
     * are not really copied, i.e. only the shared_ptr are copied. In that case
     * the two final vertices have a pointer to the same Node in memory. This is
     * because in the contraction algorithm, a contracted Node do not change at 
     * all after that. However, free nodes are really copied (the two Vertices
     * have two different Nodes) because in two different graphs, the two Nodes
     * can be contracted with different possible partners, and then will be 
     * different.
     * \param other Vertex to copy.
     * \return A reference to the modified Vertex.
     */
    Vertex& operator=(const Vertex& other);

    /*!
     * \brief Overload of operator== for Vertex.
     * \details Tells if the field content is the same and in the same order.
     * \param other Vertex to compare.
     * \return \b True  if the field contents are identical.
     * \return \b False else.
     */
    bool operator==(const Vertex& other) const;

    /*!
     * \brief Overload of operator!= for Vertex. See Vertex::operator==().
     * \return \b False if the field contents are identical.
     * \return \b True  else.
     */
    bool operator!=(const Vertex& other) const;

    /*!
     * \brief Overload of operator<< for Vertex.
     * \param fout Output stream.
     * \param v    Vertex to display.
     * \return The modified stream.
     */
    friend
    std::ostream& operator<<(std::ostream& fout,
                             const Vertex& v);

    protected:

    /*!
     * \brief Tells if the Vertex is external, i.e. composed of field
     * insertions.
     */
    bool external;

    size_t id;
};

/*!
 * 
 * \brief std::vector of Vertex that represents the connected component of a 
 * Graph. A Graph has typically one ConnectedComponent and free Vertices.
 */
class ConnectedComponent {

    IMPLEMENTS_STD_VECTOR(Vertex, vertices)

    public:

    /*!
     * \brief Default constructor. Empty ConnectedComponent.
     */
    ConnectedComponent();

    /*!
     * \brief Constructor with one parameter.
     * \param firstVertex First (and unique for now) Vertex in the component.
     */
    explicit
    ConnectedComponent(const Vertex& firstVertex);

    /*!
     * \brief Copy constructor = C++ default.
     */
    ConnectedComponent(const ConnectedComponent& other) = default;

    std::vector<Vertex> const &getVertices() const;

    /*!
     * \brief Adds a Vertex in the connected component.
     */
    void addVertex(const Vertex& newVertex);

    /*!
     * \brief Tells if the ConnectedComponent has contractible Node left.
     * \details If no, it means that the component can no longer be contracted. 
     * And if there if free Node left in the Graph, the contraction gives 0 in
     * the calculation: only connected Graph are considered in Wick contraction.
     * \return \b True  if the component has free Node left.
     * \return \b False else.
     */
    bool hasValenceLeft() const;

    /*!
     * \brief This function returns the mty::QuantumField of the first contractible
     * Node if there is one. If no, this function calls an error.
     * \return The mty::QuantumField of the first contractible Node in the component.
     */
    const mty::QuantumField* getFirstContractibleField() const;

    /*!
     * \return The position of the first contractible Vertex if there is one.
     * \return -1 else.
     */
    int getFirstContractibleVertex() const;

    /*!
     * \brief Returns the list (in a std::vector) of the Vertices in the 
     * ConnectedComponent contractible with \b field.
     * \details The first constractible vertex is not put in the returned vector 
     * except is its degeneracy (see Vertex::getDegeneracy()) is bigger than
     * one, i.e. has more than one Node contractible with \b field. This is 
     * because at each contraction step we contract the first Node of the 
     * ConnectedComponent of the Graph with possible other Nodes. The first 
     * contractible Node of the ConnectedComponent is then not included in the
     * "other Nodes".
     * \param field mty::QuantumField we want to contract.
     * \return A std::vector filled with Vertices of the ConnectedComponent
     * contractible with \b field.
     */
    std::vector<int> getContractibleVertices(
            const mty::QuantumField* field) const;

    /*!
     * \return The first contractible Node of the ConnectedComponent if 
     * there is one.
     * \return nullptr else.
     */
    std::shared_ptr<Node> getFirstContractibleNode() const;

    /*!
     * \brief Allows to get all nodes of the ConnectedComponent independently
     * of their contractions.
     * \return A std::vector of std::shared_ptr to the Nodes of the component.
     */
    std::vector<std::shared_ptr<Node>> getNodes() const;

    /*!
     * \brief Connects the first contractible Node of the ConnectedComponent 
     * with the first Node (contractible with the previous one) of the Vertex
     * in position \b indexVertex of the component.
     * \param indexVertex Index of the Vertex to contract.
     * \return The degeneracy encountered in the contraction.
     */
    int connect(int indexVertex);

    /*!
     * \brief Tries to connect the first contractible Node of the 
     * ConnectedComponent with the Vertex \b other. 
     * \param other Vertex to contract with the first contractible Node.
     * \return \b True  if there is a contraction.
     * \return \b False else.
     */
    bool connect(Vertex& other);

    /*!
     * \brief Overload of operator<< for ConnectedComponent.
     * \param fout Output stream.
     * \param c    ConnectedComponent to display.
     * \return The modified stream.
     */
    friend
    std::ostream& operator<<(std::ostream&             fout,
                             const ConnectedComponent& c);

    private:

    std::vector<Vertex> vertices;
};

/*!
 * 
 * \brief Handles wick contractions. A graph contains Vertices, and is able to
 * connect them in all possible ways returning the set of all possible Graph.
 */
class Graph {

    public:

    using Expr_type = std::pair<csl::Expr, ConjugationList>;

    /*!
     * \brief Default constructor. Empty Graph.
     */
    Graph();

    /*!
     * \brief Constructor with one parameter.
     * \param field std::vector of mty::QuantumField on which apply contractions.
     */
    explicit
    Graph(const std::vector<mty::QuantumField>& field,
          std::map<csl::Tensor, size_t>& vertexIds,
          bool                           ruleMode);

    /*!
     * \brief Constructor with one parameter.
     * \param expr csl csl::Expr converted into fields (convertExprToFields()).
     */
    explicit
    Graph(const csl::Expr& expr,
          std::map<csl::Tensor, size_t>& vertexIds,
          bool                           ruleMode);

    Graph(Graph const& other,
          std::vector<std::shared_ptr<Node>> const& init);

    /*!
     * \brief Destructor.
     */
    ~Graph(){};

    /*!
     * \brief Copy constructor = C++ default.
     */
    Graph(const Graph& other) = default;

    /*!
     * \brief Overload of operator= : C++ default.
     */
    Graph& operator=(const Graph& other) = default;
    // Graph(Graph const& other)
    //     :sortedNodes(other.sortedNodes),
    //     factor(other.factor),
    //     symmetrizeExternalLegs(other.symmetrizeExternalLegs),
    //     init(other.init),
    //     initialOrder(other.initialOrder),
    //     connectedCompo(other.connectedCompo),
    //     extVertex(other.extVertex),
    //     intVertex(other.intVertex),
    //     independentVertices(other.independentVertices),
    //     symmetry(other.symmetry),
    //     fixedExpression(other.fixedExpression),
    //     expression(other.expression)
    // {
    //     ++countCopy;
    // }

    // Graph& operator=(Graph const& other) {
    //     sortedNodes = other.sortedNodes;
    //     factor = other.factor;
    //     symmetrizeExternalLegs = other.symmetrizeExternalLegs;
    //     init = other.init;
    //     initialOrder = other.initialOrder;
    //     connectedCompo = other.connectedCompo;
    //     extVertex = other.extVertex;
    //     intVertex = other.intVertex;
    //     independentVertices = other.independentVertices;
    //     symmetry = other.symmetry;
    //     fixedExpression = other.fixedExpression;
    //     expression = other.expression;
    //     ++countCopy;
    //     return *this;
    // }

    public: 

    Graph copy() const;

    /*!
     * \return \b factor, the combiinatorial factor of the graph.
     */
    int getFactor() const;

    int getTotalDegeneracyFactor() const;

    std::vector<Vertex> const &getVertices() const;

    void addFactor(int t_factor);

    void setFactor(int t_factor);

    void setSymmetrizeExternalLegs(bool t_symmetrize);

    void multiply(int t_factor);

    void multiply(csl::Expr const &t_factor);

    /*!
     * \return \b True  if the Graph is fully connected (no free Node left).
     * \return \b False else.
     */
    bool isFullyConnected() const;

    /*!
     * \brief A Graph is considered physical if it is not (yet) fully connected
     * or if it is composed of one unique connected component.
     * \return \b True  if the Graph is physical.
     * \return \b False else.
     */
    bool isPhysical() const;

    bool isValid() const;

    int getNLoops() const;

    /*!
     * \return The total number of fields in the Graph (counting powers of
     * fields).
     */
    int getFieldDimension() const;

    /*!
     * \brief For a fully connected Graph, return the associated csl csl::Expr,
     * applying combinatorial factors and propagators of the contracted pairs
     * of fields (see mty::QuantumField::getPropagator()).
     * \return The symbolic expression where contractions are replaced by 
     * propagators.
     */
    std::vector<Expr_type> getExpression() const;

    std::vector<Expr_type> getExpression(
            mty::FeynruleMomentum& witness
            ) const;

    std::vector<const mty::QuantumField*> getFields() const;

    std::vector<const mty::QuantumField*> getConnectedFieldsInVertex(
            const mty::QuantumField* init) const;

    const mty::QuantumField* getConnectedField(
            const mty::QuantumField* init) const;

    std::vector<std::shared_ptr<Node>>& getNodes();

    std::vector<std::shared_ptr<Node>>const& getNodes() const;

    /*!
     * \return \b True  if the Graph is empty.
     * \return \b False else.
     */
    bool empty() const;

    /*!
     * \brief Performs one contraction step (the Graph must not be fully 
     * connected) and returns all possible new Graphs.
     * \details A contraction step consists in contracting the first 
     * contractible Node of the ConnectedComponent with every possible partner.
     * Each partner gives a new Graph, and all these possible graphs are
     * returned in a std::vector.
     * \return All possible graphs after one contraction step in a std::vector.
     */
    std::vector<std::shared_ptr<Graph>> contractionStep() const;

    bool compare(const Graph& other,
                 std::map<csl::Tensor, csl::Tensor>& mapping,
                 bool         fieldBlind = false) const;

    bool compare(const Graph& other,
                 bool         fieldBlind = false) const;

    void contractHighMass(
            mty::QuantumFieldParent const *field
            );

    bool operator==(const Graph& other) const;

    /*!
     * \brief Overload of operator<< for Graph.
     * \param fout Output stream.
     * \param g    Graph to display.
     * \return The output flux.
     */
    friend std::ostream& operator<<(std::ostream& fout,
                                    const Graph&  g);

    static 
    Vertex const *getVertexOf(
            std::shared_ptr<Node> const &node,
            std::vector<Vertex> const &vertices
            );

    static
    std::vector<std::shared_ptr<Node>> nextNodes(
            std::shared_ptr<Node> const &node,
            std::vector<Vertex> const &vertices
            );

    static
    int countExternalLegs(
            std::vector<csl::Tensor>::iterator first,
            std::vector<csl::Tensor>::iterator last,
            std::vector<Vertex>          const &vertices
            );

    static
    int walk(
            std::vector<csl::Tensor>::iterator first,
            std::vector<csl::Tensor>::iterator last,
            std::shared_ptr<Node>        const &node,
            std::vector<Vertex>          const &vertices
            );

    private:
    
    /*!
     * \brief Isolates at the Graph's creation independant Internal vertices.
     * \details It is important to avoid unnecessary calculations and keep the
     * good combinatorial factors to recognize identical internal vertices.
     * For example two vertices \f$ \phi(X)^3\quad \phi(Y)^3 \f$ are degenerate
     * (see Vertex::isDegenerateWith()). Contractions will be performed 
     * considering the two vertices discernable (eg treating X before Y) and a 
     * factor 2 will be added in front the Graph's expression.
     */
    void isolateIndependantVertices();

    void removeIntVertex(int indexVertex);

    /*!
     * \brief Puts the first external vertex in the connected component and 
     * removes it from the set of external vertices for further contractions.
     */
    void initConnectedComponent();

    Expr_type getPartialExpression(
            std::vector<std::shared_ptr<Node>>&   nodes,
            const std::vector<mty::QuantumField>& initialOrder,
            mty::FeynruleMomentum& witnessMapping,
            csl::Expr const&            globalFactor) const;

    csl::ObjectSymmetry<const mty::QuantumField*>
        getSymmetry(const Vertex&            vertex,
                    const mty::QuantumField* field) const;

    std::vector<std::vector<std::shared_ptr<Node>>> symmetrize() const;

    static std::vector<std::shared_ptr<Node>> copyNodes(
            const std::vector<std::shared_ptr<Node>>& toCopy);

    static void applySymmetry(
            std::vector<std::shared_ptr<Node>>&             nodes,
            const csl::ObjectPermutation<const mty::QuantumField*>& permutation);

    /*!
     * \param field mty::QuantumField to contract.
     * \return A std::vector of integers corresponding to the positions of
     * external vertices contractible with \b field.
     */
    std::vector<int> getContractibleExtVertices(
            const mty::QuantumField* field) const;

    /*!
     * \param field mty::QuantumField to contract.
     * \return A std::vector of integers corresponding to the positions of
     * internal vertices contractible with \b field.
     */
    std::vector<int> getContractibleIntVertices(
            const mty::QuantumField* field) const;

    /*!
     * \param field mty::QuantumField to contract.
     * \return A std::vector of integers corresponding to the positions of
     * vertices contractible with \b field in \b connectedCompo.
     */
    std::vector<int> getContractibleConnectedVertices(
            const mty::QuantumField* field) const;

    /*!
     * \brief Tries to contract the mty::QuantumField \b field between the 
     * ConnectedComponent and the external vertex at position \b indexVertex.
     * \param field       mty::QuantumField being contracted.
     * \param indexVertex Position of the external Vertex to contract with 
     * \b connectedCompo.
     * \return \b True  if the contraction is done.
     * \return \b False if it was not possible.
     */
    bool contractExternal(const mty::QuantumField* field,
                          int                 indexVertex);

    /*!
     * \brief Tries to contract the mty::QuantumField \b field between the 
     * ConnectedComponent and the internal vertex at position \b indexVertex.
     * \param field       mty::QuantumField being contracted.
     * \param indexVertex Position of the internal Vertex to contract with 
     * \b connectedCompo.
     * \return \b True  if the contraction is done.
     * \return \b False if it was not possible.
     */
    bool contractInternal(const mty::QuantumField* field,
                          int                 indexVertex);

    /*!
     * \brief Tries to contract the mty::QuantumField \b field between the 
     * ConnectedComponent and the vertex at position \b indexVertex in 
     * \b connectedCompo.
     * \param field       mty::QuantumField being contracted.
     * \param indexVertex Position of the second Vertex being contracted in 
     * \b connectedCompo.
     * \return \b True  if the contraction is done.
     * \return \b False if it was not possible.
     */
    bool contractConnected(const mty::QuantumField* field,
                           int                      indexVertex);

    static void addFoundNode(const std::shared_ptr<Node>&      newNode,
                             std::vector<csl::Tensor>& foundNodes);

    static void sortNodes(std::vector<std::shared_ptr<Node>>& nodes);

    static bool compareFieldsDummyPoint(
            const mty::QuantumField*                            fieldA,
            const mty::QuantumField*                            fieldB,
            std::map<csl::Tensor, csl::Tensor>& constraints,
            bool fieldBlind = false);

    static bool compareNodesWithConstraints(
            const std::shared_ptr<Node>&                        nodeA,
            const std::shared_ptr<Node>&                        nodeB,
            std::map<csl::Tensor, csl::Tensor>& constraints,
            bool fieldBlind = false);

    private:

    mutable
    std::vector<std::shared_ptr<Node>> sortedNodes;
    /*!
     * \brief Combinatorial factor of the Graph in its current state of
     * contraction
     */
    long long int factor = 1;

    csl::Expr          exprFactor = CSL_1;

    bool symmetrizeExternalLegs = false;

    bool ruleMode;

    /*!
     * \brief Initial order of vertices, in order to keep track of a possible
     * sign of commutation between fields.
     */
    std::shared_ptr<std::vector<Vertex>> init;

    std::shared_ptr<std::vector<mty::QuantumField>> initialOrder;

    /*!
     * \brief Connected component of the Graph (contains all its vertices).
     */
    ConnectedComponent  connectedCompo;

    /*!
     * \brief List of external vertices not already contracted.
     */
    std::vector<Vertex> extVertex;

    /*!
     * \brief List of internal vertices not already contracted.
     */
    std::vector<Vertex> intVertex;

    /*!
     * \brief Sets of integers spotting the independant internal vertices
     * in \b intVertex.
     */
    std::vector<int>    independentVertices;

    csl::ObjectSymmetry<const mty::QuantumField*> symmetry 
        = Id_Sym(const mty::QuantumField*);

    mutable bool fixedExpression = false;

    mutable std::vector<Expr_type> expression;
};

/*!
 * 
 * \brief Class handling the full application of Wick theorem. Starts from an
 * initial Graph fully disconnected, delegates contraction steps to the Graph
 * class and handles the series of resulting Graphs (eliminates non physical
 * ones, iterates the Graph::contractoinStep()...).
 */
class WickCalculator {

    public:

    static csl::Expr applyWickTheoremOnDiagram(
            const Graph& diagram,
            std::vector<mty::FeynruleMomentum>& witnessMapping,
            bool ruleMode = true);

    static csl::vector_expr applyWickTheoremOnDiagrams(
            const std::vector<std::shared_ptr<Graph>>& diagrams,
            std::vector<mty::FeynruleMomentum>& witnessMapping,
            bool ruleMode = true);

    static std::vector<mty::FeynmanDiagram> getDiagrams(
            mty::Model const *model,
            const csl::Expr& initialExpr,
            std::map<csl::Tensor, size_t>& vertexIds,
            std::vector<mty::FeynruleMomentum>& witnessMapping,
            bool symmetriseExternalLegs = false,
            bool ruleMode = true);

    static std::vector<mty::FeynmanDiagram> getDiagrams(
            mty::Model const *model,
            const csl::Expr& initialExpr,
            std::map<csl::Tensor, size_t>& vertexIds,
            bool symmetriseExternalLegs = false,
            bool ruleMode = true);

    /*!
     * \brief Checks quickly if a set of fields gives a zero contraction.
     * \param fields std::vector of mty::QuantumField to check.
     * \details This function will not detect all cases at all. The purpose is
     * to do a quick check to discard the most obvious cases, in order to save
     * time in computations.
     * \return \b True  if the contraction of \b fields is zero in some cases.
     * \return \b False else.
     */
    static bool isContractionZero(
            const std::vector<mty::QuantumField>& fields,
            size_t maxLoops  = size_t(-1),
            size_t nVertices = size_t(-1));

    static bool isContractionZero(
            const std::vector<mty::QuantumField const*>& fields,
            size_t maxLoops  = size_t(-1),
            size_t nVertices = size_t(-1));

    private:

    /*!
     * \brief Static helper function that splits a std::vector of fields into
     * several, each part being associated with a unique QuantumFieldParent.
     * Allows to discard quickly diagrams that have one (at least) species 
     * that presents an odd number of fields.
     * \param field Initial std::vector of mty::QuantumField to split in species.
     * \return The splitted std::vector into groups of mty::QuantumField from
     * the same species.
     */
    static std::vector<std::vector<mty::QuantumField>> splitFields(
            const std::vector<mty::QuantumField>& field);

    static std::vector<std::vector<mty::QuantumField const*>> splitFields(
            const std::vector<mty::QuantumField const*>& field);

    private:

    /*!
     * \brief Default constructor
     */
    WickCalculator();

    /*!
     * \brief Constructor with one parameter.
     * \param freeDiagram Graph fully disconnected initializing the algorithm.
     */
    explicit
    WickCalculator(const Graph& freeDiagram);

    /*!
     * \brief Copy constructor = c++ default.
     */
    WickCalculator(const WickCalculator& other) = default;

    /*!
     * \brief Destructor.
     */
    ~WickCalculator(){};

    void setSymmetrizeExternalLegs(bool t_symmetrize);

    /*!
     * \return The initial Graph \b initialDiagram.
     */
    Graph getInitialDiagram() const;

    /*!
     * \brief Calculates and returns all possible contractions of the initial
     * Graph. If the different diagrams have already been found, does not 
     * recompute them.
     * \return The set of possible Graph once contractions have been done.
     */
    std::vector<std::shared_ptr<Graph>> getDiagrams();

    /*!
     * \brief See Graph::isPhysical(). Eliminate Graph that do not fill this 
     * condition.
     * \param diagrams std::vector of Graph which non physical elements are 
     * removed (modified during the run).
     */
    void eliminateNonPhysicalDiagrams(
            std::vector<std::shared_ptr<Graph>>& diagrams);

    /*!
     * \brief Contraction algorithm. Generates all possible Graph using the
     * method Graph::contractionStep(), and stores it in the member 
     * \b feynmanDiagram.
     */
    void calculateDiagrams();

    private:

    /*!
     * \brief Boolean that tells if contractions have already been done.
     */
    bool diagramsCalculated = false;

    bool symmetrizeExternalLegs = false;

    /*!
     * \brief Initial diagram for contractions.
     */
    Graph initialDiagram;

    /*!
     * \brief All possible diagrams once contractions have been done. Result
     * of the function calculateDiagrams()).
     */
    std::vector<std::shared_ptr<Graph>> feynmanDiagram;
};

///////////////////////////////////////////////////
/*************************************************/
// Functions                                     //
/*************************************************/
///////////////////////////////////////////////////

/*!
 * \brief Contracts two Node together, i.e. sets the partner of each node equal
 * to a std::weak_ptr to the other.
 * \param node1 std::shared_ptr to the first Node.
 * \param node2 std::shared_ptr to the second Node.
 */
void contractNodes(const std::shared_ptr<Node>& node1,
                   const std::shared_ptr<Node>& node2);

/*!
 * \brief Tells if two mty::QuantumField are degenerate, i.e. have the **same value**
 * in the contraction sens. They must be the same mty::QuantumField and have the same
 * conjugation property.
 * \details Example: \f$ \psi^*(X)_{\alpha} \f$ and \f$ \psi(Y)_{\beta} \f$
 * are contractible together but not degenerate. 
 *  \f$ \psi^*(X)_{\alpha} \f$ and \f$ \psi^*(Y)_{\beta} \f$ are not 
 *  contractible together but are degenerate.
 * \param f1 First mty::QuantumField to compare.
 * \param f2 Second mty::QuantumField to compare.
 * \return \b True  if \b f1 and \b f2 are degenerate.
 * \return \b False else.
 */
bool areDegenerate(const mty::QuantumField& f1, 
                   const mty::QuantumField& f2);

/*!
 * \brief Tells if the contraction of two mty::QuantumField is non zero.
 * \param f1 First mty::QuantumField in the contraction.
 * \param f2 Second mty::QuantumField in the contraction.
 * \return \b True  if the contraction of \b f1 and \b f2 is non zero.
 * \return \b False else.
 */
bool areExactlyContractible(const mty::QuantumField& f1, 
                            const mty::QuantumField& f2);

/*!
 * \brief Overload of the operator<< for Node.
 * \param fout Output flux.
 * \param node Node to display in a std::shared_ptr.
 * \return The modified flux.
 */
std::ostream& operator<<(std::ostream&                fout,
                         const std::shared_ptr<Node>& node);

/*!
 * \brief Overload of operator== for sstd::shared_ptr\<Node\>. Compares the 
 * values of the two pointers.
 * \param A First std::shared_ptr to Node to compare.
 * \param B Second std::shared_ptr to Node to compare.
 * \return \b True  if the two pointers point to the same memory location.
 * \return \b False else.
 */
bool operator==(const std::shared_ptr<Node>& A,
                const std::shared_ptr<Node>& B);

bool comparePriority(const std::shared_ptr<Node>&            A,
                     const std::shared_ptr<Node>&            B,
                     const std::vector<csl::Tensor>& foundNodes);

bool internal_comparePriority(const std::shared_ptr<Node>&            A,
                              const std::shared_ptr<Node>&            B,
                              const std::vector<csl::Tensor>& foundNodes);

/*!
 * \brief Converts a csl csl::Expr that is **just** a product of field into a 
 * std::vector of mty::QuantumField objects.
 * \details This function raises an error if it finds an object in the product
 * that is neither a mty::QuantumField nor a positive Integer power of 
 * mty::QuantumField. If a field appears in a power, it is put as many times
 * in the vector as its integer power.
 * \param expr csl::Expr product of quantum fields to convert.
 * \return A std::vector of mty::QuantumField containing all fields in \b expr.
 */
std::vector<mty::QuantumField> convertExprToFields(const csl::Expr& expr);

/*!
 * \brief Counts the number of fermions in a std::vector of mty::QuantumField.
 * \param begin Begin iterator of the std::vector of mty::QuantumField.
 * \param end   End iterator of the std::vector of mty::QuantumField.
 * \return The number of fermions in the vector.
 */
int countFermions(std::vector<const mty::QuantumField*>::const_iterator begin,
                  std::vector<const mty::QuantumField*>::const_iterator end);

/*!
 * \brief Computes and returns the sign cost of the commutation of two
 * sets of mty::QuantumField. \f$ sgn = (-1)^{N_c}\f$, with \f$ N_c \f$ the number
 * of fermion permutations necessary to Transform \b A to \b B.
 * \param A Initial order of fields.
 * \param B Same field content as \b A but in a different order.
 * \return The sign cost of the permutation of \b B in order to Transform it 
 * into \b A.
 */
int getCommutationSign(const std::vector<mty::QuantumField>& A,
                       std::vector<const mty::QuantumField*>       B);

int getCommutationSign(const std::vector<const mty::QuantumField*>& A,
                       std::vector<const mty::QuantumField*>       B);

/*!
 * \brief Converts a std::vector of fully connected graphs into a std::vector
 * of expressions (one expression per graph) via Graph::getExpression().
 * \param diagrams Fully connected graphs to convert.
 * \return The converted graphs into csl csl::Expr.
 */
csl::vector_expr convertGraphsToCorrelators(
        const std::vector<std::shared_ptr<Graph>>& diagrams,
        std::vector<mty::FeynruleMomentum>& witnessMapping,
        bool ruleMode = true); 

} // End of namespace mty::wick

#endif
