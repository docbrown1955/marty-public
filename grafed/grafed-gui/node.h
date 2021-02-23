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
 * @version 1.0
 
 * \brief
 */
#ifndef NODE_H
#define NODE_H

#include <QWidget>

#include <QObject>
#include <QGraphicsObject>
#include <QList>
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Diagram;
class DiagramWidget;
class Proxy;

class Node : public QGraphicsObject
{
    Q_OBJECT

public:

    static std::pair<QPointF, QPointF> getEffectiveNodes(
            Node *first,
            Node *second
            );

    Node(Diagram *DiagramWidget);

    enum { Type = static_cast<int>(UserType) + 1 };
    int type() const override { return Type; }

    QString getName() const;
    qint32 getNodeSize() const {
        return nodeSize;
    }
    QColor getColor() const {
        return color;
    }
    qint32 getNodeType() const {
        return nodeType;
    }
    bool hasGraphFocus() const;
    bool isOperator() const;
    void focusInGraph();
    void unFocusInGraph();
    void setOperator(bool t_op);
    void setGridPos(qreal x, qreal y);
    void setDiagramWidget(DiagramWidget *t_widget) {
        widget = t_widget;
    }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void paintCross(
            QPainter *painter);
    void paintDisk(
            QPainter *painter);
    void paintCircle(
            QPainter *painter);
    void paintHatch(
            QPainter *painter);
    void show();
    void hide();

public slots:

    void setName(QString const& t_name);
    void setNodeSize(qint32 t_size) {
        nodeSize = t_size;
        Node::update();
    }
    void setColor(QColor t_color) {
        color = t_color;
        Node::update();
    }
    void setNodeType(qint32 t_nodeType) {
        nodeType = t_nodeType;
        Node::update();
    }
    void changeNode(QAction *action = nullptr);
    void changeColor();
    void changeName();
    void moveLabel();
    void updateEdge();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void update();

public:
    static constexpr qint32 size = 16;
    static constexpr qreal zValue = -1;

    Proxy *label = nullptr;

private:

    QPointF oldPos;
    Diagram *graph;
    DiagramWidget *widget;
    QColor color;
    qint32 nodeSize;
    qint32 nodeType;
    qint32 doubleClicked = 0;
    bool graphFocus;
    bool displayNode;
};

#endif // NODE_H
