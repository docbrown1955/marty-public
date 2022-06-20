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
 * @version 2.0

 * \brief
 */
#ifndef EDGE_H
#define EDGE_H

#include "latexLink.h"
#include <QGraphicsObject>
#include <memory>

class QAction;
namespace Qt {
class QGraphicsSceneMouseEvent;
}

class Node;
class Diagram;
class DiagramWidget;
class Proxy;

class Edge : public QGraphicsObject {
    Q_OBJECT

    friend class DiagramWidget;

  public:
    Edge(Diagram *DiagramWidget);

    Edge(Diagram *DiagramWidget, Node *t_first, Node *t_second);

    virtual ~Edge() override;

    QString getName() const;
    qint32  getLineWidth() const
    {
        return lineWidth;
    }

    const Node *getFirst() const;
    const Node *getSecond() const;
    Node *      getFirst();
    Node *      getSecond();

    bool isConnectedTo(Node const *node) const;
    bool isFlipped() const
    {
        return flippedLabel;
    }
    bool hasFocusInGraph() const;
    void focusInGraph();
    void unFocusInGraph();
    void wheelEventCustom(int delta);

    bool getSign() const
    {
        return particleType > 0;
    }
    qint32 getParticleType()
    {
        return std::abs(particleType);
    }
    QColor getColor() const;
    QColor getPaintColor() const;
    qreal  length() const;
    double getCurve() const;
    void   setCurve(double t_curve);
    qreal  getCurvature() const;
    void   setCurvature(double curvature);

    void setDiagramWidget(DiagramWidget *t_widget)
    {
        widget = t_widget;
    }
    void setSides(Node *t_first, Node *t_second);
    void flipLabel()
    {
        setFlipped(!flippedLabel);
    }
    void setColor(QColor t_color)
    {
        color = t_color;
    }

    void setParticleType(qint32 t_type);

    enum { Type = static_cast<int>(UserType) + 2 };
    int type() const override
    {
        return Type;
    }

    QRectF       boundingRect() const override;
    QPainterPath shape() const override;
    void         paint(QPainter *                      painter,
                       QStyleOptionGraphicsItem const *option,
                       QWidget *                       widget) override;

    void paintLine(QPainter *painter) const;
    void paintDashedLine(QPainter *painter) const;
    void paintDotLine(QPainter *painter) const;
    void paintSinusoid(QPainter *painter) const;
    void paintCycloid(QPainter *painter) const;
    void paintArrow(QPainter *painter) const;

  public slots:

    void changeColor();
    void changeName();
    void setName(QString const &t_name);
    void setLineWidth(qint32 t_lineWidth);
    void setFlipped(bool t_flipped)
    {
        flippedLabel = t_flipped;
        setName(getName());
    }
    void release();
    void nodeMoved();
    void changeEdgeType(QAction *action);

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

  public:
    Proxy *label = nullptr;

  private:
    Diagram *      graph;
    DiagramWidget *widget;
    Node *         first;
    Node *         second;
    bool           flippedLabel;
    QColor         color;
    mutable int    signCurvature = 0;
    mutable qreal  curvature;
    qint32         lineWidth;
    qint32         amplitude;
    qint32         particleType;
    bool           selected;
};

#endif // EDGE_H
