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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>

MainWindow::MainWindow(
        QString const &nameFile,
        QWidget       *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    saved(true)
{
    m_ui->setupUi(this);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    mainWidget = new MainWidget(nameFile, this);
    connect(mainWidget, SIGNAL(isSaved()),
            this,       SLOT(isSaved()));
    connect(mainWidget, SIGNAL(isChanged()),
            this,       SLOT(isChanged()));
    setCentralWidget(mainWidget);
    setWindowTitle("Grafed - MARTY");
    connect(mainWidget, SIGNAL(diagramMode()),
            this,       SLOT(setDiagramMode()));
    connect(mainWidget, SIGNAL(sessionMode()),
            this,       SLOT(setSessionMode()));
    connect(mainWidget, SIGNAL(normalMode()),
            this, 		SLOT(normalMode()));
    connect(mainWidget, SIGNAL(selectMode()),
            this, 		SLOT(selectMode()));
    connect(mainWidget, SIGNAL(insertMode()),
            this, 		SLOT(insertMode()));
    m_ui->actionSource_to_clipboard->setEnabled(false);
    m_ui->actionSource_Latex->setEnabled(false);
    m_ui->actionPDF_Latex->setEnabled(false);
    m_ui->actionPNG_Latex->setEnabled(false);
    m_ui->actionPNG->setEnabled(false);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);
    font.setBold(true);
    font.setPointSize(15);
    statusBar()->setFont(font);
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    mainWidget = new MainWidget(this);
    setCentralWidget(mainWidget);
    setWindowTitle("Grafed - MARTY");
    connect(mainWidget, SIGNAL(diagramMode()),
            this,       SLOT(setDiagramMode()));
    connect(mainWidget, SIGNAL(sessionMode()),
            this,       SLOT(setSessionMode()));
    connect(mainWidget, SIGNAL(normalMode()),
            this, 		SLOT(normalMode()));
    connect(mainWidget, SIGNAL(selectMode()),
            this, 		SLOT(selectMode()));
    connect(mainWidget, SIGNAL(insertMode()),
            this, 		SLOT(insertMode()));
    m_ui->actionSource_to_clipboard->setEnabled(false);
    m_ui->actionSource_Latex->setEnabled(false);
    m_ui->actionPDF_Latex->setEnabled(false);
    m_ui->actionPNG_Latex->setEnabled(false);
    m_ui->actionPNG->setEnabled(false);
    QFont font;
    font.setCapitalization(QFont::AllUppercase);
    font.setBold(true);
    font.setPointSize(15);
    statusBar()->setFont(font);
}

void MainWindow::isChanged()
{
    if (saved) {
        saved = false;
        setWindowTitle("*Grafed - MARTY");
    }
}

void MainWindow::isSaved()
{
    if (!saved) {
        saved = true;
        setWindowTitle("Grafed - MARTY");
    }
}

MainWindow::~MainWindow()
{
    delete mainWidget;
    delete m_ui;
}

void MainWindow::closeEvent(QCloseEvent *event)  // show prompt when user wants to close app
{
    event->ignore();
    if (mainWidget->maybeSave())
    {
        event->accept();
    }

}

void MainWindow::setDiagramMode()
{
    if (m_ui->actionSource_to_clipboard->isEnabled())
        return;
    m_ui->actionSource_to_clipboard->setEnabled(true);
    m_ui->actionSource_Latex->setEnabled(true);
    m_ui->actionPDF_Latex->setEnabled(true);
    m_ui->actionPNG_Latex->setEnabled(true);
    m_ui->actionPNG->setEnabled(true);

    mainWidget->setDiagramMode();
    normalMode();
}

void MainWindow::setSessionMode()
{
    if (!m_ui->actionSource_to_clipboard->isEnabled())
        return;
    m_ui->actionSource_to_clipboard->setEnabled(false);
    m_ui->actionSource_Latex->setEnabled(false);
    m_ui->actionPDF_Latex->setEnabled(false);
    m_ui->actionPNG_Latex->setEnabled(false);
    m_ui->actionPNG->setEnabled(false);

    mainWidget->setSessionMode();
}

void MainWindow::newDiagram()
{
    mainWidget->newDiagram();
}

void MainWindow::newSession()
{
    mainWidget->newSession();
}

void MainWindow::open()
{
    mainWidget->open();
}

void MainWindow::save()
{
    mainWidget->save();
}

void MainWindow::saveAs()
{
    mainWidget->saveAs();
}

void MainWindow::exportPNG()
{
    mainWidget->exportPNG();
}

void MainWindow::exportPDF()
{
    mainWidget->exportPDF();
}

void MainWindow::latexSource()
{
    mainWidget->latexSource();
}

void MainWindow::latexSourceToClipboard()
{
    mainWidget->latexSourceToClipboard();
}

void MainWindow::latexExportPNG()
{
    mainWidget->latexExportPNG();
}

void MainWindow::latexExportPDF()
{
    mainWidget->latexExportPDF();
}

void MainWindow::normalMode()
{
    statusBar()->setStyleSheet("color: rgb(50, 50, 200)");
    statusBar()->showMessage("Normal mode");
    statusBar()->update();
}

void MainWindow::insertMode()
{
    statusBar()->setStyleSheet("color: rgb(50, 200, 50)");
    statusBar()->showMessage("Insertion mode");
    statusBar()->update();
}

void MainWindow::selectMode()
{
    statusBar()->setStyleSheet("color: rgb(200, 160, 0)");
    statusBar()->showMessage("Selection mode");
    statusBar()->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    return QMainWindow::eventFilter(target, event);
}
