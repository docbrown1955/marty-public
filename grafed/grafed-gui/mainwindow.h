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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString const &nameFile,
                        QWidget       *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:

    void setDiagramMode();
    void setSessionMode();
    void newDiagram();
    void newSession();
    void open();
    void save();
    void saveAs();
    void exportPNG();

    void latexSource();
    void latexSourceToClipboard();
    void latexExportPNG();
    void latexExportPDF();

    void normalMode();
    void selectMode();
    void insertMode();

    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *, QEvent *) override;
    void closeEvent(QCloseEvent *event) override;

    void isSaved();
    void isChanged();

private:
    Ui::MainWindow *m_ui;
    MainWidget     *mainWidget;
    bool            saved;
};

#endif // MAINWINDOW_H
