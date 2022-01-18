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

#include "diagram.h"
#include "htmlconverter.h"
#include "latexcompiler.h"
#include "diagramwidget.h"
#include "diagramrenderer.h"
#include "node.h"
#include "edge.h"
#include "planargraph.h"
#include <cmath>
#include <sstream>
#include <QVector>
#include <QGuiApplication>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>
#include <QCursor>
#include <QInputDialog>
#include <QTime>
#include "grid.h"
#include "border.h"


QColor toQColor(drawer::LatexLinker::Color color)
{
    return QColor(
                static_cast<int>(color.R),
                static_cast<int>(color.G),
                static_cast<int>(color.B),
                static_cast<int>(color.A)
                );
}

drawer::LatexLinker::Color fromQColor(QColor color)
{
    return {
        static_cast<unsigned char>(color.red()),
        static_cast<unsigned char>(color.green()),
        static_cast<unsigned char>(color.blue()),
        static_cast<unsigned char>(color.alpha())
    };
}


Diagram::Diagram(
        QGraphicsScene *t_scene,
        qint32 t_X0,
        qint32 t_Y0,
        DiagramRenderer *t_renderer)
    : renderer(t_renderer),
      cursor(nullptr),
      draggedEdge(nullptr),
      X0(t_X0),
      Y0(t_Y0),
      LX(1000),
      LY(1000),
      rotation(180),
      mode(NormalMode),
      gridSize(Grid::defaultGridSize),
      nodesOnGrid(true),
      focusNode(false),
      updatingGraph(false)
{
    scene = t_scene;
    // scale(qreal(0.8), qreal(0.8));
    // setMinimumSize(400, 400);
    drawer::Graph graph;
    latexComp = t_renderer->getLatexCompiler();
    connect(latexComp, SIGNAL(labelReady(std::string const&)),
            this,      SLOT(labelReady(std::string const&)));
    auto link_to_load = drawer::LatexLinker(graph);
    loadLinker(link_to_load);
    updateGraph();
}

Diagram::~Diagram()
{
    clear();
}

void Diagram::modificationDone()
{
    renderer->modificationDone();
}

void Diagram::setGridSize(qint32 t_gridSize)
{
    gridSize = t_gridSize;
}

void Diagram::setActiveWidget(DiagramWidget *t_widget)
{
    widget = t_widget;
    for (auto &n : nodes)
        n->setDiagramWidget(t_widget);
    for (auto &e : edges)
        e->setDiagramWidget(t_widget);
}

InteractiveMode Diagram::getInteractiveMode() const
{
    return mode;
}

void Diagram::loadGraph(drawer::Graph const&t_graph)
{
    clear();
    graph = t_graph;
    graph.center();
    graph.flip(1);
    graph.scale(scaleForGraph);
    nodes.clear();
    nodes.resize(graph.size());
    for (size_t i = 0; i != graph.size(); ++i) {
        nodes[i] = buildNode(graph.getNodes()[i].x,
                             graph.getNodes()[i].y);
    }
    auto& adj = graph.getAdjacency();
    for (size_t i = 0; i != graph.size(); ++i)
        for (size_t j = i+1; j < graph.size(); ++j)
            for(int k = 0; k < adj(i, j); ++k)
                edges.push_back(buildEdge(nodes[i], nodes[j]));
    //updateGraph();
    graph.move(X0, Y0);
    nodeMoved();
}

QPointF Diagram::getNodeLabelPos(
        Node const *node,
        Proxy const *label
        ) const
{
    qreal r = 20 + ((node->getNodeType() == 0) ? 0. : node->getNodeSize()/2.);
    for (size_t j = 0; j != edges.size(); ++j) {
        auto& edge = edges[j];
        if (edge->isConnectedTo(node)) {
            auto first = edge->getFirst();
            auto second = edge->getSecond();
            QPointF dir = (first == node) ?
                        (first->pos() - second->pos())
                      : (second->pos() - first->pos());
            dir /= std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
            qreal width = label->rect().width();
            qreal heigth = label->rect().height();
            //width = std::max(width, heigth);
            qreal d = 2*r + heigth;
            qreal l = width;
            qreal alpha = std::atan2(-dir.x(), -dir.y());
            if (std::abs(alpha) > std::abs(std::atan2(l, d))
                    and std::abs(M_PI-alpha) > std::abs(std::atan2(l, d))
                    and std::abs(M_PI+alpha) > std::abs(std::atan2(l, d))) {
                int s = (alpha > 0 and alpha < M_PI) ? 1 : -1;
                qreal theta = alpha - s*std::asin(l/d * std::cos(alpha));
                dir = QPointF{
                    -l/2 * s - r * std::sin(theta) - width/2,
                             - d/2 * std::cos(theta) - heigth/2
                };
                return node->pos() + dir;
            }
            else {
                if (alpha < M_PI/2 and alpha > -M_PI/2) {
                    qreal y = -d / 2;
                    qreal x = y * std::tan(alpha);
                    return node->pos() + QPointF{x - width/2, y - heigth/2};
                }
                else {
                    qreal y = d / 2;
                    qreal x = y * std::tan(alpha);
                    return node->pos() + QPointF{x - width/2, y - heigth/2};
                }
            }
        }
    }
    return node->pos() - QPointF{-r/3-label->rect().width()/2, -label->rect().height()/2};
}

QPointF angle(QPointF X, qreal theta)
{
    const auto c = std::cos(theta);
    const auto s = std::sin(theta);
    return QPointF{c * X.x() - s * X.y(), c * X.y() + s * X.x()};
}

QPointF Diagram::getEdgeLabelPos(
        Edge                      const *edge,
        drawer::LatexLinker::Edge const &lEdge,
        Proxy                     const *label
        ) const
{
    qreal r = 20;

    size_t i = lEdge.i;
    size_t j = lEdge.j;
    QPointF dir = (nodes[j]->pos() - nodes[i]->pos());
    qreal dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    dir /= dist;
    auto posI = nodes[i]->pos();
    if (nodes[i]->getNodeType() != 0) {
        posI += dir * nodes[i]->getNodeSize()/2;
    }
    auto posJ = nodes[j]->pos();
    if (nodes[j]->getNodeType() != 0) {
        posJ -= dir * nodes[j]->getNodeSize()/2;
    }
    QPointF mean = (posI + posJ) / 2;
    QPointF perpdir{-dir.y(), +dir.x()};
    if (std::fabs(lEdge.curve) > 0.005) {
        double L = edge->length();
        double c = edge->getCurve();// * edge->length();
        double offset = -L*(1-std::sqrt(1-c*c/4))/c;
        mean += offset * perpdir;
    }
    qreal xref;
    qreal yref;
    qreal width = label->rect().width();
    qreal height = label->rect().height();
    qreal theta = std::atan2(-dir.y(), dir.x());
    if (lEdge.flipped)
        theta = M_PI + theta;
    if (theta > 2*M_PI)
        theta -= 2*M_PI;
    if (theta < 0)
        theta += 2*M_PI;
    if (theta < M_PI / 2) {
        xref = -width/2;
        yref = height / 2;
    }
    else if (theta < M_PI) {
        xref = -width/2;
        yref = -height / 2;
    }
    else if (theta < 3 * M_PI / 2) {
        xref = width/2;
        yref = -height / 2;
    }
    else {
        xref = width/2;
        yref = height / 2;
    }
    qreal R = r+height/2;
    // int s = (lEdge.flipped) ? -1 : 1;
    QPointF res = mean + angle(QPointF{
                                   QPointF{0, -R}
                                   + angle({-width/2, -height/2}, theta)
                                   + QPointF{0, height /2 - (std::cos(theta)*yref - std::sin(theta)*xref)}
                               },
                               -theta);
    return res;
}

void Diagram::loadLinker(drawer::LatexLinker const&t_linker)
{
    name = t_linker.getName().c_str();
    loadGraph(t_linker.getGraph());
    std::vector<drawer::LatexLinker::Edge> const& linkEdges
            = t_linker.getEdges();
    std::vector<drawer::LatexLinker::Node> const& linkNodes
            = t_linker.getNodes();
    std::vector<Edge*> edgesLeft(edges);
    for (size_t i = 0; i != linkNodes.size(); ++i) {
        std::string nameVertex = linkNodes[i].name;
        nodes[i]->setNodeType(int(linkNodes[i].type));
        if (linkNodes[i].size != 0)
            nodes[i]->setNodeSize(linkNodes[i].size);
        nodes[i]->setColor(toQColor(linkNodes[i].color));
        if (nameVertex != "") {
            addNodeLabel(nodes[i], nameVertex.c_str());
        }
    }
    for (const auto& lEdge : linkEdges) {
        size_t i = lEdge.i;
        size_t j = lEdge.j;
        for (size_t k = 0; k != edgesLeft.size(); ++k) {
            auto edge = edgesLeft[k];
            if (edge->isConnectedTo(nodes[i])
                    and edge->isConnectedTo(nodes[j])) {
                qint32 particleType = qint32(lEdge.type);
                qint32 sign = lEdge.sign;
                edge->setParticleType((sign) ? particleType : -particleType);
                edge->setCurve(lEdge.curve);
                edge->setFlipped(lEdge.flipped);
                edge->setColor(toQColor(lEdge.color));
                edge->setLineWidth(lEdge.lineWidth);
                edgesLeft.erase(edgesLeft.begin() + int(k));
                if (lEdge.name != "") {
                    addEdgeLabel(edge, lEdge, lEdge.name.c_str());
                }
                break;
            }
        }
    }

    link = t_linker;
}

void Diagram::clear()
{
    modificationDone();
    graph.clear();
    link.clear();
    link.setGraph(graph);
    for (size_t i = 0; i != edges.size(); ++i) {
        deleteEdge(edges[i]);
    }
    for (size_t i = 0; i != nodes.size(); ++i) {
        deleteNode(nodes[i]);
    }

    nodes.clear();
    edges.clear();
}

void Diagram::setOutputPath(const std::string &path)
{
    outputPath = path;
}

void Diagram::setFile(const std::string &t_fileName)
{
    saveFile = t_fileName;
}

void Diagram::setExportFile(const std::string &t_fileName)
{
    exportFile = t_fileName;
}

void Diagram::forceNodeOnGrid(bool value)
{
    nodesOnGrid = value;
}

void Diagram::save(std::string const&fileName) const
{
    getScaledGraph().write(outputPath + "/" + fileName);
}

QRectF Diagram::getBounds() const
{
    Border border;
    for (const auto &node : nodes) {
        border.update(node->sceneBoundingRect());
        if (node->label)
            border.update(node->label->sceneBoundingRect());
    }
    for (const auto &edge : edges) {
        border.update(edge->sceneBoundingRect());
        if (edge->label)
            border.update(edge->label->sceneBoundingRect());
    }
    QRectF bounds = border.getBorder();
    return bounds;
}

void Diagram::exportSelfPNG(const QString &fileName)
{
    resetFocus();
    showNodes(false);
    QRectF bounds = getBounds();
    QRectF oldRect = scene->sceneRect();
    scene->setSceneRect(bounds);
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    image.save(fileName);

    scene->setSceneRect(oldRect);
    showNodes(true);
}

void Diagram::exportPNG(std::string const& fileName) const
{
    link.setGraph(getScaledGraph());
    link.exportPNG(fileName, outputPath);
}

void Diagram::exportPDF(std::string const& fileName) const
{
    link.setGraph(getScaledGraph());
    link.exportPDF(fileName, outputPath);
}

std::string Diagram::getLatexSource() const
{
    std::ostringstream sout;
    sout << link << std::endl;

    return sout.str();
}

void Diagram::insertionMode()
{
    if (mode != InsertionMode) {
        focusNode = false;
        resetFocus();
        mode = InsertionMode;
    }
}

void Diagram::normalMode()
{
    if (mode != NormalMode) {
        focusNode = false;
        resetFocus();
        mode = NormalMode;
    }
}

void Diagram::selectionMode()
{
    if (mode != SelectionMode) {
        focusNode = false;
        resetFocus();
        mode = SelectionMode;
    }
}

Node* Diagram::buildNode(qreal x,
                         qreal y)
{
    Node *node = new Node(this);
    node->setDiagramWidget(widget);
    node->setPos(X0+x, Y0+y);
    connect(node, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
    connect(node, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    scene->addItem(node);
    return node;
}

Edge* Diagram::buildEdge(Node *first,
                         Node *second)
{
    Edge *edge = new Edge(this, first, second);
    edge->setDiagramWidget(widget);
    edge->nodeMoved();
    scene->addItem(edge);
    return edge;
}

void Diagram::deleteNode(Node *&node)
{
    disconnect(node, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
    disconnect(node, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    if (node->label)
        delete node->label;
    delete node;
    node = nullptr;
}

void Diagram::deleteEdge(Edge *&edge)
{
    if (edge->label)
        delete edge->label;
    delete edge;
    edge = nullptr;
}

drawer::Graph Diagram::getScaledGraph() const
{
    drawer::Graph scaled(graph);
    scaled.placeTopLeft();
    scaled.scale(1. / scaleForGraph);
    return scaled;
}

void Diagram::setFocusOn(size_t pos)
{
    if (mode != SelectionMode)
        resetFocus();
    focusNode = true;
    nodes[pos]->focusInGraph();
}

void Diagram::resetFocus()
{
    focusNode = false;
    for (auto& n : nodes)
        n->unFocusInGraph();
    for (auto& e : edges)
        e->unFocusInGraph();
}

void Diagram::testRoundValues(qreal &x,
                              qreal &y)
{
    if (not nodesOnGrid or mode == SelectionMode)
        return;
    x = std::round(x / (gridSize / 2.)) * gridSize / 2.;
    y = std::round(y / (gridSize / 2.)) * gridSize / 2.;
}

void Diagram::addNode(qreal x, qreal y)
{
    testRoundValues(x ,y);
    graph.addNode(x, y);
    link.addNode();
    link.setGraph(graph);
    nodes.push_back(buildNode());
    updateGraph();
    modificationDone();
}

void Diagram::addNode(QPointF point)
{
    addNode(point.x(), point.y());
}

void Diagram::addEdge(size_t i, size_t j)
{
    ++graph.getAdjacency()(i, j);
    ++graph.getAdjacency()(j, i);
    link.setParticlesType(i, j);
    link.setGraph(graph);
    if (i < j)
        edges.push_back(buildEdge(nodes[i], nodes[j]));
    else {
        edges.push_back(buildEdge(nodes[j], nodes[i]));
    }
    updateGraph();
    modificationDone();
}

void Diagram::addEdge(Node *A, Node *B)
{
    size_t i = size_t(-1);
    size_t j = size_t(-1);
    for (size_t k = 0; k != nodes.size(); ++k)
        if (nodes[k] == A)
            i = k;
    else if (nodes[k] == B)
            j = k;
    if (i >= nodes.size()) {
        std::cerr << "Node " << A << " not found in graph!\n";
        return;
    }
    if (j >= nodes.size()) {
        std::cerr << "Node " << B << " not found in graph!\n";
        return;
    }
    addEdge(i, j);
}
/*

 * MathJax fonts
MathJax_Main
MathJax_Math

 * Tex fonts
Tex Gyre Adventure
TeX Gyre Bonum
TeX Gyre Bonum Math
TeX Gyre Chorus
TeX Gyre Cursor
TeX Gyre DejaVu Math
TeX Gyre Heros
TeX Gyre Heros Cn
TeX Gyre Pagella
TeX Gyre Pagella Math
TeX Gyre Schola
TeX Gyre Schola Math
TeX Gyre Termes
TeX Gyre Termes Math

*/
static const QFont font("TeX Gyre Termes Math", 25);

void Diagram::labelReady(std::string const &label)
{
    std::cout << "Label \"" << label << "\" is ready !\n";
}

Proxy *Diagram::generateLabel(QString const &text) const
{
    if (1) { // !text.isEmpty()) {
#ifdef NO_LATEX_LABEL
        QString html = HTMLConverter::fromLatex(text);
        QLabel *label = new QLabel;
        label->setFont(font);
        label->setTextFormat(Qt::RichText);
        label->setText(html);
#else
        QLabel *label = new QLabel;
        QPixmap pixmap = latexComp->getLabel(text);
        label->setPixmap(pixmap);
#endif
        QPalette sample_palette;
        sample_palette.setColor(QPalette::Window, Qt::transparent);
        sample_palette.setColor(QPalette::WindowText, Qt::black);
        label->setAutoFillBackground(true);
        label->setPalette(sample_palette);
        Proxy *proxy = new Proxy(widget);
        // proxy->setFlag(QGraphicsObject::ItemIsMovable, true);
        proxy->setFlag(QGraphicsObject::ItemIsSelectable, true);
        proxy->setFlag(QGraphicsObject::ItemSendsGeometryChanges, true);
        proxy->setCacheMode(QGraphicsObject::DeviceCoordinateCache);
        proxy->setFlag(QGraphicsObject::ItemClipsToShape);
        // proxy->setAcceptDrops(true);
        proxy->setZValue(Node::zValue + 1);
        proxy->setWidget(label);
        connect(proxy, SIGNAL(doubleClicked(Proxy *)),
                this,  SLOT(proxyDoubleClicked(Proxy*)));
        proxy->data = text;
        return proxy;
    }
    //return nullptr;
}

void Diagram::addLabel(
        Proxy         *label,
        QPointF const &pos)
{
    if (label) {
        label->setPos(pos);
        scene->addItem(label);
    }
}

void Diagram::addNodeLabel(
        Node *node,
        QString const &text)
{
    Proxy *label;
    if (node->label and text != node->label->data) {
        disconnect(node->label, SIGNAL(doubleClicked(Proxy *)),
                   this     ,   SLOT(proxyDoubleClicked(Proxy*)));
        delete node->label;
        label = generateLabel(text);
        if (!label)
            return;
        QPointF pos = getNodeLabelPos(node, label);
        addLabel(label, pos);
        node->label = label;
    }
    else if (node->label) {
        label = node->label;
        QPointF pos = getNodeLabelPos(node, label);
        label->setPos(pos);
    }
    else {
        label = generateLabel(text);
        if (!label)
            return;
        QPointF pos = getNodeLabelPos(node, label);
        addLabel(label, pos);
        node->label = label;
    }
}

void Diagram::addEdgeLabel(
        Edge *edge,
        drawer::LatexLinker::Edge const &lEdge,
        QString const &text)
{
    Proxy *label;
    if (edge->label and edge->label->data != text) {
        disconnect(edge->label, SIGNAL(doubleClicked(Proxy *)),
                   this     ,   SLOT(proxyDoubleClicked(Proxy*)));
        delete edge->label;
        edge->label = nullptr;
        label = generateLabel(text);
        if (!label)
            return;
        QPointF pos = getEdgeLabelPos(edge, lEdge, label);
        addLabel(label, pos);
        edge->label = label;
    }
    else if (edge->label) {
        label = edge->label;
        QPointF pos = getEdgeLabelPos(edge, lEdge, label);
        label->setPos(pos);
    }
    else {
        label = generateLabel(text);
        if (!label)
            return;
        QPointF pos = getEdgeLabelPos(edge, lEdge, label);
        addLabel(label, pos);
        edge->label = label;
    }
}

void Diagram::proxyDoubleClicked(Proxy *proxy)
{
    for (auto &node : nodes)
        if (node->label == proxy) {
            node->changeName();
            return;
        }
    for (auto &edge : edges)
        if (edge->label == proxy) {
            edge->changeName();
            return;
        }
}

void Diagram::removeNode(size_t i)
{
    for (size_t j = 0; j != edges.size(); ++j)
        if (edges[j]->isConnectedTo(nodes[i])) {
            removeEdge(edges[j]);
            edges.erase(edges.begin() + int(j));
            --j;
        }
    deleteNode(nodes[i]);
    graph.deleteNode(i);
    delete nodes[i];
    link.removeNode(i);
    link.setGraph(graph);
}

void Diagram::removeEdge(Edge *edge)
{
    auto [i, j] = getPosEdge(edge);
    deleteEdge(edge);
    graph.getAdjacency()(i, j) = 0;
    graph.getAdjacency()(j, i) = 0;
    link.removeEdge(i, j);
    link.setGraph(graph);
}

size_t Diagram::getPosNode(Node const* node) const
{
    for (size_t i = 0; i != nodes.size(); ++i)
        if (nodes[i] == node)
            return i;
    std::cerr << "Node " << node << " not found in grpah.\n";
    return 0;
}

std::pair<size_t, size_t> Diagram::getPosEdge(Edge const*edge) const
{
    size_t i = size_t(-1);
    size_t j = size_t(-1);
    for (size_t k = 0; k != nodes.size(); ++k) {
        if (nodes[k] == edge->getFirst())
            i = k;
        else if (nodes[k] == edge->getSecond())
            j = k;
    }
    if (i >= nodes.size()) {
        std::cerr << "First node not found for edge.\n";
    }
    if (j >= nodes.size()) {
        std::cerr << "Second node not found for edge.\n";
    }

    return {i, j};
}

void Diagram::removeSelected()
{
    removeSelectedEdges();
    removeSelectedNodes();
}

void Diagram::removeSelectedNodes()
{
    for (size_t i = 0; i != nodes.size(); ++i)
        if (nodes[i]->hasGraphFocus()) {
            removeNode(i);
            nodes.erase(nodes.begin() + int(i));
            --i;
            modificationDone();
        }
}

void Diagram::removeSelectedEdges()
{
    for (size_t i = 0; i != edges.size(); ++i)
        if (edges[i]->hasFocusInGraph()) {
            removeEdge(edges[i]);
            edges.erase(edges.begin()  + int(i));
            --i;
            modificationDone();
        }
}


void Diagram::setNodeName(Node    const*node,
                              QString const&name)
{
    auto i = getPosNode(node);
    auto str = name.toStdString();
    link.setVertexName(i, str);
}

void Diagram::setEdgeName(Edge    const*edge,
                              QString const&name)
{
    auto [i, j] = getPosEdge(edge);
    link.setEdgeName(i, j, name.toStdString());
}

void Diagram::setEdgeType(Edge const          *edge,
                              drawer::ParticleType type,
                              bool                 sign)
{
    auto [i, j] = getPosEdge(edge);
    link.replaceParticlesType(i, j, type, sign);
    updateGraph();
}

void Diagram::move(qreal x, qreal y)
{
    if (not focusNode) {
        graph.move(x, y);
        modificationDone();
    }
    else {
        for (size_t i = 0; i != nodes.size(); ++i) {
            if (nodes[i]->hasGraphFocus()) {
                graph.getNodes()[i].x += x;
                graph.getNodes()[i].y += y;
                modificationDone();
            }
        }
    }
}

void Diagram::keyPressEvent(QKeyEvent *event)
{
    // const double dtheta = M_PI / 200;
    const double dl     = 1;
    bool graphUpdated = false;
    switch (event->key()) {
    case Qt::Key_Left:
        move(-dl, 0);
        graphUpdated = true;
        break;
    case Qt::Key_Right:
        move(dl, 0);
        graphUpdated = true;
        break;
    case Qt::Key_Up:
        move(0, -dl);
        graphUpdated = true;
        break;
    case Qt::Key_Down:
        move(0, dl);
        graphUpdated = true;
        break;
    case Qt::Key_Delete:
        removeSelected();
        graphUpdated = true;
        break;
    }
    if (graphUpdated)
        updateGraph();
}

void Diagram::keyReleaseEvent(QKeyEvent */*event*/)
{
}

void Diagram::mouseMoveEvent(QMouseEvent */*event*/)
{
    QPointF cursorpos = widget->getCursorPos();
    if (cursor) {
        cursor->setPos(cursorpos);
    }
}

void Diagram::mousePressEvent(QMouseEvent *event)
{
    QPointF cursorpos = widget->getCursorPos();
    if (mode == InsertionMode and event->button() == Qt::RightButton) {
        addNode(cursorpos);
    }
    else if (mode == InsertionMode and event->button() == Qt::LeftButton) {
        for (const auto& n : nodes)
            if (n->isUnderMouse()) {
                n->focusInGraph();
                cursor = buildNode();
                cursor->setVisible(false);
                cursor->setPos(cursorpos);
                draggedEdge = buildEdge(n, cursor);
                break;
            }
    }
    if (mode != SelectionMode) {
        for (auto& e : edges)
            e->release();
        for (auto& n : nodes)
            n->unFocusInGraph();
    }
}

void Diagram::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF cursorpos = widget->getCursorPos();
    Q_UNUSED(cursorpos);
    if (event->button() == Qt::LeftButton) {
        if (cursor != nullptr) {
            for (Node *n : nodes)
                if (n->isUnderMouse()) {
                    if (n != draggedEdge->getFirst()) {
                        addEdge(draggedEdge->getFirst(), n);
                    }
                    break;
                }
            for (const auto& n : nodes)
                n->unFocusInGraph();
            deleteEdge(draggedEdge);
            deleteNode(cursor);
        }
    }
}

void Diagram::mouseDoubleClickEvent(QMouseEvent */*event*/)
{

}

void Diagram::wheelEvent(QWheelEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    for (auto& e :edges)
        e->wheelEventCustom(event->delta());
#else
    for (auto& e :edges)
        e->wheelEventCustom(event->angleDelta().y());
#endif
}

void Diagram::showNodes(bool show)
{
    if (show) {
        for (const auto& n : nodes)
            n->show();
    }
    else {
        for (const auto& n : nodes)
            n->hide();
    }
}

void Diagram::reinitGraph()
{
    graph.reinit();
    graph.minimize();
    graph.center();
    graph.scale(100);
    modificationDone();
    updateGraph();
}

void Diagram::updateGraph()
{
    updatingGraph = true;
    for (size_t i = 0; i != nodes.size(); ++i)
        nodes[i]->setPos(graph.getNodes()[i].x,
                         graph.getNodes()[i].y);
    updatingGraph = false;
}

void Diagram::nodeMoved()
{
    if (updatingGraph)
        return;
    for (size_t i = 0; i != nodes.size(); ++i) {
        graph.getNodes()[i].x = nodes[i]->x();
        graph.getNodes()[i].y = nodes[i]->y();
    }
}

void Diagram::center()
{
    graph.move(-X0, -Y0);
    graph.centerMargin();
    graph.move(X0, Y0);
    updateGraph();
    modificationDone();
}

void Diagram::rotateTo(int t_rotation)
{
    graph.move(-X0, -Y0);
    graph.rotate(M_PI * (t_rotation - rotation) / 180);
    graph.move(X0, Y0);
    rotation = t_rotation;
    updateGraph();
    modificationDone();
}

void Diagram::moveTo(qint32 X0_new, qint32 Y0_new)
{
    graph.move(X0_new - X0, Y0_new - Y0);
    X0 = X0_new;
    Y0 = Y0_new;
    updateGraph();
    modificationDone();
}

void Diagram::setFocus(Node const*focused)
{
    for (size_t i = 0; i != nodes.size(); ++i)
        if (nodes[i] == focused) {
            setFocusOn(i);
            return;
        }
}

void Diagram::refreshLinker()
{
    link = drawer::LatexLinker(getScaledGraph());
    link.setName(name.toStdString());
    link.getGraph().center();
    std::vector<drawer::LatexLinker::Node> &lNodes = link.getNodes();
    for (size_t i = 0; i != nodes.size(); ++i) {
        std::string name = (nodes[i]->label) ?
                    nodes[i]->label->data.toStdString() : "";
        link.setVertexName(i, name);
        lNodes[i].type = drawer::LatexLinker::NodeType(nodes[i]->getNodeType());
        if (nodes[i]->getNodeSize() != Node::size)
            lNodes[i].size = nodes[i]->getNodeSize();
        lNodes[i].color = fromQColor(nodes[i]->getColor());
    }
    for (size_t k = 0; k != edges.size(); ++k) {
        auto [i, j] = getPosEdge(edges[k]);
        std::string name = (edges[k]->label) ?
                edges[k]->label->data.toStdString() : "";
        link.setParticlesType(
                i,
                j,
                static_cast<drawer::ParticleType>(
                    edges[k]->getParticleType()),
                name,
                edges[k]->getSign(),
                edges[k]->getCurve()/2,
                edges[k]->isFlipped(),
                fromQColor(edges[k]->getColor()),
                edges[k]->getLineWidth()
                );
    }
}

void Diagram::refresh()
{
    for (auto &n : nodes) {
        auto name = (n->label) ? n->label->data : "";
        if (name != "")
            n->setName(name);
    }
    for (auto &e : edges) {
        auto name = (e->label) ? e->label->data : "";
        if (name != "")
            e->setName(name);
    }
}

void Proxy::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* /* event */)
{
    emit doubleClicked(this);
}
