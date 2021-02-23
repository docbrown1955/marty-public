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

#include "node.h"
#include "edge.h"
#include "diagramwidget.h"
#include "diagramrenderer.h"
#include <iostream>
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMenu>
#include <QInputDialog>
#include <QColorDialog>

bool isPunctual(int type)
{
    return type == int(drawer::LatexLinker::NodeType::None)
            or type == int(drawer::LatexLinker::NodeType::Cross);
}
std::pair<QPointF, QPointF> Node::getEffectiveNodes(
        Node *first,
        Node *second
        )
{
    QPointF dir = second->pos() - first->pos();
    qreal length = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (length != 0.)
        dir /= length;

    std::pair<QPointF, QPointF> res;
    res.first = (isPunctual(first->getNodeType())) ?
                first->pos()
              :first->pos() + dir * first->getNodeSize() / 2;
    res.second = (isPunctual(second->getNodeType())) ?
                second->pos()
              :second->pos() - dir * second->getNodeSize() / 2;

    return res;
}

Node::Node(Diagram *diag)
     :graph(diag),
     color(Qt::black),
     nodeSize(size),
     nodeType(0),
     graphFocus(false),
     displayNode(true)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptDrops(true);
    setZValue(Node::zValue);

    connect(this, SIGNAL(xChanged()), this, SLOT(moveLabel()));
    connect(this, SIGNAL(yChanged()), this, SLOT(moveLabel()));
    oldPos = pos();
}

QString Node::getName() const {
    return (label) ? label->data : "";
}

bool Node::hasGraphFocus() const
{
    return graphFocus;
}

bool Node::isOperator() const
{
    return nodeType == int(drawer::LatexLinker::NodeType::Operator);
}

void Node::focusInGraph()
{
    graphFocus = !graphFocus;
    update();
}

void Node::unFocusInGraph()
{
    graphFocus = false;
    update();
}

void Node::setOperator(bool t_op)
{
    if (t_op)
        nodeType = int(drawer::LatexLinker::NodeType::Operator);
    else if (nodeType == int(drawer::LatexLinker::NodeType::Operator))
        nodeType = int(drawer::LatexLinker::NodeType::None);
}

void Node::setName(QString const&name)
{
    if (name == "") {
        scene()->removeItem(label);
        label = nullptr;
        graph->renderer->modificationDone();
        return;
    }
    graph->addNodeLabel(this, name);
    graph->setNodeName(this, name);
    graph->renderer->modificationDone();
    update();
}

void Node::changeNode(QAction *action)
{
    if (!action)
        return;
    if (action->text() == "Set label") {
        changeName();
    }
    else if (action->text() == "Change color") {
        changeColor();
    }
    else if (action->text() == "Edit node") {
        unFocusInGraph();
        graph->renderer->openNodeForm(this);
    }
}

void Node::changeColor()
{
    QColor c = QColorDialog::getColor(
                Qt::black,
                widget,
                "Pick a color");
    if (c.isValid()) {
        color = c;
        update();
        graph->renderer->modificationDone();
    }
}

void Node::changeName()
{
    bool accepted;
    QString name = (label) ? label->data : "";
    QString t_name = QInputDialog::getText(
                widget,
                "Change edge name",
                "Name (latex): ",
                QLineEdit::Normal,
                name,
                &accepted);
    if (accepted) {
        setName(t_name);
        graph->renderer->modificationDone();
    }
}

void Node::moveLabel()
{
    if (label and label->data != "") {
        graph->addNodeLabel(this, label->data);
        graph->renderer->modificationDone();
    }
}

void Node::updateEdge()
{
    for (auto &e : graph->edges) {
        if (e->getFirst() == this or e->getSecond() == this)
            e->nodeMoved();
    }
}

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    int s = (nodeType == int(drawer::LatexLinker::NodeType::None)) ?
                size : std::max(size, nodeSize);
    return QRectF( -s/2 - adjust,
                   -s/2 - adjust,
                    s   + adjust,
                    s   + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    int s = (nodeType == int(drawer::LatexLinker::NodeType::None)) ?
                size : std::max(size, nodeSize);
    path.addEllipse(-s/2, -s/2, s, s);
    return path;
}

void Node::paint(QPainter *painter,
                 QStyleOptionGraphicsItem const*,
                 QWidget *)
{
    switch(nodeType) {
    case int(drawer::LatexLinker::NodeType::Cross):
        paintCross(painter);
        break;
    case int(drawer::LatexLinker::NodeType::Operator):
        paintCircle(painter);
        paintCross(painter);
        break;
    case int(drawer::LatexLinker::NodeType::PlainDisk):
        paintDisk(painter);
        break;
    case int(drawer::LatexLinker::NodeType::HatchedDisk):
        paintHatch(painter);
        paintCircle(painter);
        break;
    }
    if (displayNode) {
        auto c = (!graphFocus) ? QColor("#0F3FE2") : Qt::red;
        QBrush brush(c);
        painter->setBrush(brush);
        painter->setPen(QPen(Qt::black, 0));
        painter->drawEllipse(-size/2, -size/2, size, size);
    }
}

void Node::paintCircle(QPainter *painter)
{
    painter->setPen(QPen(color, 3));
    painter->drawEllipse(-nodeSize/2, -nodeSize/2, nodeSize, nodeSize);
}

void Node::paintDisk(QPainter *painter)
{
    painter->setPen(QPen(color, 3));
    QBrush brush(color);
    painter->setBrush(brush);
    painter->drawEllipse(-nodeSize/2, -nodeSize/2, nodeSize, nodeSize);
}

void Node::paintCross(QPainter *painter)
{
    painter->setPen(QPen(color, 3));
    int d = int(nodeSize / (2 * std::sqrt(2)));
    painter->drawLine(-d, -d, d, d);
    painter->drawLine(-d, d, d, -d);
}

void Node::paintHatch(QPainter *painter)
{
    painter->setPen(QPen(color, 3));
    int R = nodeSize / 2;
    int maxi = int(std::floor(std::sqrt(2) * R));
    int x0min = -maxi;
    int x0max = maxi;
    double step = 7;
    for (double x0 = x0min; x0 <= x0max; x0 += step) {
        double offset = x0 / 2.;
        double value  = R/2 * std::sqrt(2 - (x0*x0 / (R*R)));
        int xm = int(offset - value);
        int xp = int(offset + value);
        int ym = int(-offset - value);
        int yp = int(-offset + value);
        painter->drawLine(xm, ym, xp, yp);
    }
}

void Node::show()
{
    displayNode = true;
}

void Node::hide()
{
    displayNode = false;
}

void Node::update()
{
    QGraphicsObject::update();
    updateEdge();
}

QVariant Node::itemChange(GraphicsItemChange change,
                          QVariant     const&value)
{
    switch (change) {
    case ItemPositionHasChanged:
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseMoveEvent(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    oldPos = pos();
    if (event->button() == Qt::RightButton) {
        QMenu *menu = new QMenu(widget);
        menu->addAction("Set label");
        menu->addAction("Change color");
        menu->addAction("Edit node");
        connect(menu,
                SIGNAL(triggered(QAction*)),
                this,
                SLOT(changeNode(QAction*)));
        menu->exec(QCursor::pos());
    }
    graph->setFocus(this);
    QGraphicsItem::mousePressEvent(event);
    update();
}

void Node::setGridPos(qreal x, qreal y)
{
    graph->testRoundValues(x, y);
    setPos(qint32(x), qint32(y));
}
void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (doubleClicked > 0) {
        --doubleClicked;
        setPos(oldPos);
        setAcceptDrops(true);
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }
    if (graph->renderer->hasNodeForm() or graph->renderer->hasEdgeForm()) {
        graph->renderer->openNodeForm(this);
    }
    QGraphicsItem::mouseReleaseEvent(event);
    setGridPos(pos().x(), pos().y());
    graph->renderer->modificationDone();
    update();
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent */*event*/)
{
    unFocusInGraph();
    doubleClicked = 1;
    update();
    graph->renderer->openNodeForm(this);
    setAcceptDrops(false);
}


void Node::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}
