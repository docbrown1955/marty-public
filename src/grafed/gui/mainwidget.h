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
 * @file mainwidget.h
 * @brief
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2020-11-05
 */
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "diagramrenderer.h"
#include "pdfoption.h"
#include "toolbar.h"
#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget {
    Q_OBJECT

    friend class MainWindow;

  public:
    explicit MainWidget(QString const &nameFile, QWidget *parent = nullptr);
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;

    void setupUi();
    void setupConnections();

    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *, QEvent *) override
    {
        return true;
    }

  public slots:

    void setSessionMode();
    void setDiagramMode();

    void newDiagram();
    void newSession();
    void open();
    void loadFile(QString const &fileName);

    void setCurrentFile(QString const &);
    bool maybeSave();
    bool save();
    bool saveAs();
    bool saveFile(QString const &fileName);

    void exportPNG();
    void exportPDF();

    void latexSource();
    void latexSourceToClipboard();
    void latexExportPNG();
    void latexExportPDF();

    void setNormalMode(bool checked = true);
    void setInsertMode(bool checked = true);
    void setSelectMode(bool checked = true);

    void resetGrid();
    void resetZoom();
    void resetRotation();

    void modif();

    void setSaved(bool);

    void prevPage();
    void nextPage();

  signals:

    void sessionMode();
    void diagramMode();

    void normalMode();
    void selectMode();
    void insertMode();

    void isChanged();
    void isSaved();

  public:
    void renderPDF(QPainter &painter, PDFOption &options);

  private:
    Ui::MainWidget * ui;
    DiagramRenderer *renderer;
    ToolBar *        toolBar;
    QString          currentFile;
    bool             saved;
};

#endif // MAINWIDGET_H
