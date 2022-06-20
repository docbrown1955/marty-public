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

#include "diagramwidget.h"
#include "edge.h"
#include "htmlconverter.h"
#include "node.h"
#include "planargraph.h"
#include <QCursor>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsTextItem>
#include <QGuiApplication>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QTime>
#include <QVector>
#include <cmath>
#include <sstream>

DiagramWidget::DiagramWidget(QGraphicsScene *t_scene,
                             qint32          t_X0,
                             qint32          t_Y0,
                             QWidget *       parent,
                             Diagram *       t_diagram)
    : QGraphicsView(parent),
      diagram(t_diagram),
      diagramEnabled(true),
      X0(t_X0),
      Y0(t_Y0),
      LX(1000),
      LY(1000)
{
    scene = t_scene;
    setUpScene();
    setScene(scene);
    setSceneRect(X0 - LX / 2, Y0 - LY / 2, LX, LY);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(AnchorUnderMouse);
}

DiagramWidget::~DiagramWidget()
{
}

void DiagramWidget::setUpScene()
{
    const qint32 xmin = -LX / 2;
    const qint32 xmax = LX / 2;
    const qint32 ymin = -LX / 2;
    const qint32 ymax = LY / 2;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(X0 + xmin, Y0 + ymin, xmax - xmin, ymax - ymin);
    setScene(scene);
}

void DiagramWidget::centerScene()
{
    double xmin    = diagram->graph.xMin();
    double xmax    = diagram->graph.xMax();
    double ymin    = diagram->graph.yMin();
    double ymax    = diagram->graph.yMax();
    double lx      = xmax - xmin;
    double ly      = ymax - ymin;
    double xoffset = 0.1 * lx;
    double yoffset = 0.1 * ly;
    lx += 2 * xoffset;
    ly += 2 * yoffset;
    QRectF rect(X0 + xmin - xoffset, Y0 + ymin - yoffset, lx, ly);
    scene->setSceneRect(rect);
}

void DiagramWidget::addLabel()
{
}

void DiagramWidget::keyPressEvent(QKeyEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->keyPressEvent(event);
    QGraphicsView::keyPressEvent(event);
}

void DiagramWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->keyReleaseEvent(event);
    QGraphicsView::keyReleaseEvent(event);
}

QPointF DiagramWidget::getCursorPos() const
{
    QPoint origin = mapFromGlobal(QCursor::pos());
    return mapToScene(origin);
}

void DiagramWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->mouseMoveEvent(event);
    if (diagram->mode == NormalMode)
        QGraphicsView::mouseMoveEvent(event);
}

void DiagramWidget::mousePressEvent(QMouseEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->mousePressEvent(event);
    if (diagram->mode != InsertionMode)
        QGraphicsView::mousePressEvent(event);
}

void DiagramWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->mouseReleaseEvent(event);
    if (diagram->mode != InsertionMode)
        QGraphicsView::mouseReleaseEvent(event);
}

void DiagramWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void DiagramWidget::wheelEvent(QWheelEvent *event)
{
    if (!diagramEnabled)
        return;
    diagram->wheelEvent(event);
    for (auto &e : diagram->edges)
        if (e->hasFocusInGraph())
            return;
    QGraphicsView::wheelEvent(event);
}

void DiagramWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform()
                       .scale(scaleFactor, scaleFactor)
                       .mapRect(QRectF(0, 0, 1, 1))
                       .width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void DiagramWidget::zoom(double scale)
{
    scaleView(scale);
}

void DiagramWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void DiagramWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
