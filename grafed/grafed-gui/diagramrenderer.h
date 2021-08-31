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
 * @file diagramrenderer.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#ifndef DIAGRAMRENDERER_H
#define DIAGRAMRENDERER_H

#include <QScrollArea>
#include <QList>
#include <QGridLayout>
#include <QTableWidget>
#include "diagramwidget.h"
#include "diagram.h"
#include "grid.h"

namespace Ui {
class DiagramRenderer;
}

class EdgeForm;
class NodeForm;
class DiagramRenderer : public QWidget
{
    Q_OBJECT

    friend class MainWidget;

    enum Mode {
        DiagramMode,
        SessionMode
    };

public:

    static inline qint32 maxNDiagrams = 100;

private:
    explicit DiagramRenderer(QWidget *parent = nullptr);
public:
    explicit DiagramRenderer(
            QString const &nameFile,
            QWidget       *parent = nullptr);
    ~DiagramRenderer() override;

public slots:

    void clear();
    void init();

    latexcompiler *getLatexCompiler() const {
        return compiler;
    }

    void toggle();
    void updateGrid(int factor = 0);
    void displayGrid(bool display);
    void forceNodesOnGrid(bool);

    void centerDiagram();
    void updateZoom(int zoom);
    void rotateDiagram(int rotation);
    bool setDiagramMode();
    void setSessionMode();
    void setTable();
    bool setDiagram(qint32 i = -1);
    void cellPressed(int i, int j);
    void resetSelection();

    // Function not ready to be used
    void setDiagramSet(qint32 first, qint32 last);

    bool hasEdgeForm() {
        return edgeForm;
    }
    bool hasNodeForm() {
        return nodeForm;
    }
    void closeEdgeForm();
    void openEdgeForm(Edge *edge);

    void closeNodeForm();
    void openNodeForm(Node *node);

    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void readFile(QString const &nameFile);
    void addDiagram();

    void diagramDoubleClicked(qint32, qint32);

    void add();
    void select();
    void erase();
    void save(QString const &fileName);
    void copy();
    void paste();
    void paste(qint32 pos);
    void refresh(qint32 pos);

    void modificationDone();

signals:


    void diagramMode();
    void sessionMode();

    void nonEmptySelection(bool);

    void modif();

protected:

    std::pair<Diagram*, DiagramWidget*>
        generateDiagram(qint32 iRow, qint32 iCol);
    std::pair<Diagram*, DiagramWidget*> newDiagram();

private:

    Ui::DiagramRenderer *m_ui;
    QHBoxLayout *hlayout;
    Mode mode;
    qint32          nColumns;
    qint32          minWidth;
    qint32          minHeight;
    qint32          firstDiag;
    qint32          lastDiag;
    std::vector<drawer::LatexLinker> links;
    QList<Diagram*>       allDiagrams;
    QList<Diagram*>       diagrams;
    QList<std::pair<int, int>> selectedDiagrams;
    qint32                indexDiagram;
    QList<qint32>         clipBoardDiagram;
    latexcompiler        *compiler;
    DiagramWidget        *diag;
    QTableWidget         *table;
    QGraphicsScene       *scene;
    EdgeForm             *edgeForm;
    NodeForm             *nodeForm;
    qreal                 zoomValue;
    Grid                  grid;
    bool                  displayGridValue;
};

#endif // DIAGRAMRENDERER_H
