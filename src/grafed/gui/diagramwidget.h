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
#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include "diagram.h"
#include "latexLink.h"
#include "planargraph.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsView>

class MainWindow;
class QLabel;
namespace drawer {
class Graph;
}

class Node;
class Edge;

class Proxy;
class DiagramWidget : public QGraphicsView {
    Q_OBJECT

    friend class MainWindow;
    friend class MainWidget;
    friend class DiagramRenderer;

  public:
    DiagramWidget(QGraphicsScene *t_scene,
                  qint32          t_X0,
                  qint32          t_Y0,
                  QWidget *       parent,
                  Diagram *       t_diagram);

    ~DiagramWidget() override;

    void setUpScene();

    QPointF getCursorPos() const;

  public slots:
    void zoom(double scale);
    void zoomIn();
    void zoomOut();
    void centerScene();
    void addLabel();

    void enableDiagram()
    {
        diagramEnabled = true;
    }
    void disableDiagram()
    {
        diagramEnabled = false;
    }

  signals:

    void zoom(QWheelEvent *event);

  protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void scaleView(qreal scaleFactor);

  public:
    static constexpr qint32 dx = 16;

  private:
    QGraphicsScene *scene;
    Diagram *       diagram;
    bool            diagramEnabled;
    qint32          X0;
    qint32          Y0;
    qint32          LX;
    qint32          LY;
};

#endif // DIAGRAMWIDGET_H
