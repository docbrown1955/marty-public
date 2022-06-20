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
 * @file diagram.h
 * @brief
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2020-11-05
 */
#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "latexLink.h"
#include "latexcompiler.h"
#include "planargraph.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QInputDialog>

class MainWindow;
class QLabel;
class DiagramWidget;
namespace drawer {
class Graph;
}

class Node;
class Edge;

enum InteractiveMode {

    NormalMode,
    InsertionMode,
    SelectionMode,
};

class Proxy;
class DiagramRenderer;
class Diagram : public QObject {

    Q_OBJECT

    friend class MainWindow;
    friend class MainWidget;
    friend class DiagramRenderer;
    friend class DiagramWidget;
    friend class Edge;
    friend class Node;

  public:
    Diagram(QGraphicsScene * t_scene,
            qint32           t_X0,
            qint32           t_Y0,
            DiagramRenderer *t_renderer);

    ~Diagram() override;

    QString getName() const
    {
        return name;
    }

    void setActiveWidget(DiagramWidget *t_widget);

    InteractiveMode getInteractiveMode() const;
    void            save(std::string const &fileName) const;
    void            exportSelfPNG(QString const &fileName);
    void            exportPNG(std::string const &fileName) const;
    void            exportPDF(std::string const &fileName) const;
    std::string     getLatexSource() const;

    void   loadGraph(drawer::Graph const &t_graph);
    void   loadLinker(drawer::LatexLinker const &t_link);
    void   itemMoved();
    void   clear();
    void   setOutputPath(std::string const &path);
    void   setFile(std::string const &t_fileName);
    void   setExportFile(std::string const &t_fileName);
    void   forceNodeOnGrid(bool value);
    void   testRoundValues(qreal &x, qreal &y);
    QRectF getBounds() const;

    void insertionMode();
    void normalMode();
    void selectionMode();

    void modificationDone();

  public slots:

    void setName(QString const &t_name)
    {
        name = t_name;
    }
    void setGridSize(qint32 t_gridSize);
    void showNodes(bool);
    void reinitGraph();
    void updateGraph();
    void nodeMoved();
    void setFocus(Node const *focused);
    void resetFocus();

    void center();
    void rotateTo(int angle);
    void moveTo(qint32 X0_new, qint32 Y0_new);

    size_t                    getPosNode(Node const *node) const;
    std::pair<size_t, size_t> getPosEdge(Edge const *edge) const;

    void addNode(qreal x = 0, qreal y = 0);
    void addNode(QPointF point);
    void addEdge(size_t i, size_t j);
    void addEdge(Node *A, Node *B);

    Proxy *generateLabel(QString const &text) const;
    void   addLabel(Proxy *label, QPointF const &pos);
    void   addNodeLabel(Node *node, QString const &text);
    void   addEdgeLabel(Edge *                           edge,
                        drawer::LatexLinker::Edge const &lEdge,
                        QString const &                  text);
    void   proxyDoubleClicked(Proxy *);

    void removeEdge(Edge *edge);
    void removeNode(size_t i);
    void removeSelected();
    void removeSelectedNodes();
    void removeSelectedEdges();

    void setEdgeName(Edge const *edge, QString const &name);
    void setNodeName(Node const *node, QString const &name);
    void
    setEdgeType(Edge const *edge, drawer::ParticleType type, bool sign = true);

    void refreshLinker();
    void refresh();

    void labelReady(std::string const &);

  protected:
    Node *buildNode(qreal x = 0, qreal y = 0);
    Edge *buildEdge(Node *first, Node *second);
    void  deleteNode(Node *&node);
    void  deleteEdge(Edge *&edge);

    QPointF getNodeLabelPos(Node const *node, Proxy const *label) const;
    QPointF getEdgeLabelPos(Edge const *                     edge,
                            drawer::LatexLinker::Edge const &lEdge,
                            Proxy const *                    label) const;

    drawer::Graph getScaledGraph() const;
    void          setFocusOn(size_t pos);
    void          move(qreal x, qreal y);
    void          keyPressEvent(QKeyEvent *event);
    void          keyReleaseEvent(QKeyEvent *event);
    QPointF       getCursorPos() const;
    void          mouseMoveEvent(QMouseEvent *event);
    void          mousePressEvent(QMouseEvent *event);
    void          mouseReleaseEvent(QMouseEvent *event);
    void          mouseDoubleClickEvent(QMouseEvent *event);
    void          wheelEvent(QWheelEvent *event);

  public:
    static constexpr qreal scaleForGraph = 100;

  private:
    inline static const std::string defaultFileName = "None";

    DiagramRenderer *           renderer;
    DiagramWidget *             widget;
    drawer::Graph               graph;
    mutable latexcompiler *     latexComp;
    mutable drawer::LatexLinker link;
    Node *                      cursor;
    Edge *                      draggedEdge;
    std::vector<Node *>         nodes;
    std::vector<Edge *>         edges;
    QGraphicsScene *            scene;
    qint32                      X0;
    qint32                      Y0;
    qint32                      LX;
    qint32                      LY;
    qint32                      rotation;

    QString     name;
    std::string outputPath = ".";
    std::string saveFile   = defaultFileName;
    std::string exportFile = defaultFileName;

    InteractiveMode mode;
    int             gridSize;
    bool            nodesOnGrid;
    bool            focusNode;
    bool            updatingGraph;
};

class Proxy : public QGraphicsProxyWidget {

    Q_OBJECT

  public:
    Proxy(DiagramWidget *t_widget) : QGraphicsProxyWidget(), widget(t_widget)
    {
    }

    ~Proxy() override
    {
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsItem::mouseMoveEvent(event);
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsItem::mousePressEvent(event);
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsItem::mouseReleaseEvent(event);
    }

    void dropEvent(QGraphicsSceneDragDropEvent *event) override
    {
        QGraphicsItem::dropEvent(event);
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

  signals:

    void doubleClicked(Proxy *);

  public:
    QString        data;
    DiagramWidget *widget;
};

#endif // DIAGRAM_H
