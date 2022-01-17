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

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "ui_toolbar.h"
#include "dialoglatex.h"
#include "pdfsetupdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QPrinter>
#include <QPrintDialog>
#include <QPdfWriter>
#include <QPagedPaintDevice>

MainWidget::MainWidget(
        QString const &nameFile,
        QWidget       *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    currentFile(nameFile),
    saved(true)
{
    renderer = new DiagramRenderer(nameFile, this);
    toolBar  = new ToolBar(this);
    setupUi();
    //QScrollArea *scroll = new QScrollArea(this);
    //scroll->setWidget(renderer);
    layout()->addWidget(toolBar);
    layout()->addWidget(renderer);
    setSessionMode();

    setupConnections();
    Q_INIT_RESOURCE(app);
}
MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    currentFile(""),
    saved(true)
{

    renderer = new DiagramRenderer(this);
    toolBar  = new ToolBar(this);
    setupUi();
    layout()->addWidget(toolBar);
    layout()->addWidget(renderer);
    setSessionMode();

    setupConnections();
    Q_INIT_RESOURCE(app);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setSaved(bool t_saved)
{
    if (saved == t_saved)
        return;
    saved = t_saved;
    if (saved)
        emit isSaved();
    else
        emit isChanged();
}

void MainWidget::setCurrentFile(QString const &t_currentFile)
{
    currentFile = t_currentFile;
}

void MainWidget::setSessionMode()
{
    toolBar->ui->gridLabel->setEnabled(false);
    toolBar->ui->gridSlider->setEnabled(false);
    toolBar->ui->gridButton->setEnabled(false);
    toolBar->ui->prec_page->setEnabled(true);
    toolBar->ui->next_page->setEnabled(true);
    toolBar->ui->gridValueLabel->setEnabled(false);
    toolBar->ui->zoomLabel->setEnabled(false);
    toolBar->ui->zoomSlider->setEnabled(false);
    toolBar->ui->rotationLabel->setEnabled(false);
    toolBar->ui->rotationValueLabel->setEnabled(false);
    toolBar->ui->rotationSlider->setEnabled(false);
    toolBar->ui->zoomValueLabel->setEnabled(false);
    toolBar->ui->zoomPercentLabel->setEnabled(false);
    toolBar->ui->checkGrid->setEnabled(false);
    toolBar->ui->checkNodesOnGrid->setEnabled(false);
    toolBar->ui->selectButton->setEnabled(false);
    toolBar->ui->centerButton->setEnabled(false);
    toolBar->ui->removeButton->setEnabled(false);
    toolBar->ui->normalButton->setEnabled(false);

    toolBar->ui->selectButton->setCheckable(false);
    toolBar->ui->normalButton->setCheckable(false);
    toolBar->ui->addButton->setCheckable(false);

    toolBar->ui->gridResetButton->setEnabled(false);
    toolBar->ui->zoomResetButton->setEnabled(false);
    toolBar->ui->rotationResetButton->setEnabled(false);

    toolBar->ui->labelDiagName->hide();
    toolBar->ui->lineDiagName->hide();
    toolBar->ui->labelSessionName->show();
    toolBar->ui->lineSessionName->show();

    if (renderer->diag and renderer->diag->diagram)
        disconnect(toolBar->ui->lineDiagName, SIGNAL(textChanged(QString)),
                   renderer->diag->diagram,   SLOT(setName(QString)));

    if (renderer->diag and renderer->diag->diagram)
        disconnect(toolBar->ui->removeButton, SIGNAL(pressed()),
                   renderer->diag->diagram,   SLOT(removeSelected()));

    resetGrid();
    resetZoom();
    resetRotation();

    renderer->setSessionMode();
    emit sessionMode();
}

void MainWidget::setDiagramMode()
{
    if (!renderer->setDiagramMode())
        return;

    toolBar->ui->gridLabel->setEnabled(true);
    toolBar->ui->gridSlider->setEnabled(true);
    toolBar->ui->gridButton->setEnabled(true);
    toolBar->ui->prec_page->setEnabled(false);
    toolBar->ui->next_page->setEnabled(false);
    toolBar->ui->gridValueLabel->setEnabled(true);
    toolBar->ui->zoomLabel->setEnabled(true);
    toolBar->ui->zoomSlider->setEnabled(true);
    toolBar->ui->zoomValueLabel->setEnabled(true);
    toolBar->ui->zoomPercentLabel->setEnabled(true);
    toolBar->ui->rotationLabel->setEnabled(true);
    toolBar->ui->rotationValueLabel->setEnabled(true);
    toolBar->ui->rotationSlider->setEnabled(true);
    toolBar->ui->checkGrid->setEnabled(true);
    toolBar->ui->checkNodesOnGrid->setEnabled(true);
    toolBar->ui->selectButton->setEnabled(true);
    toolBar->ui->centerButton->setEnabled(true);
    toolBar->ui->removeButton->setEnabled(true);
    toolBar->ui->normalButton->setEnabled(true);

    toolBar->ui->gridResetButton->setEnabled(true);
    toolBar->ui->zoomResetButton->setEnabled(true);
    toolBar->ui->rotationResetButton->setEnabled(true);

    toolBar->ui->selectButton->setCheckable(true);
    toolBar->ui->normalButton->setCheckable(true);
    toolBar->ui->addButton->setCheckable(true);
    toolBar->ui->normalButton->setChecked(true);

    toolBar->ui->labelDiagName->show();
    toolBar->ui->lineDiagName->show();
    toolBar->ui->labelSessionName->hide();
    toolBar->ui->lineSessionName->hide();

    toolBar->ui->lineDiagName->setText(renderer->diag->diagram->getName());
    connect(toolBar->ui->lineDiagName, SIGNAL(textChanged(QString)),
            renderer->diag->diagram,   SLOT(setName(QString)));

    connect(toolBar->ui->removeButton, SIGNAL(pressed()),
            renderer->diag->diagram,   SLOT(removeSelected()));

    emit diagramMode();
}

void MainWidget::setupUi()
{
    ui->setupUi(this);
    toolBar->ui->checkNodesOnGrid->setCheckState(Qt::Checked);
    toolBar->ui->checkGrid->setCheckState(Qt::Checked);
    toolBar->ui->removeButton->setEnabled(false);
    toolBar->ui->labelDiagName->hide();
    toolBar->ui->lineDiagName->hide();
    auto [first, last] = renderer->page();
    std::string text = std::to_string(first) + " - " + std::to_string(last);
    toolBar->ui->page->setText(QString::fromStdString(text));
}

void MainWidget::setupConnections()
{
    connect(toolBar->ui->gridButton, SIGNAL(clicked()),
            renderer,                SLOT(toggle()));
    connect(renderer, SIGNAL(diagramMode()),
            this,     SLOT(setDiagramMode()));
    connect(renderer, SIGNAL(sessionMode()),
            this,     SLOT(setSessionMode()));
    connect(renderer, SIGNAL(modif()),
            this,     SLOT(modif()));

    connect(toolBar->ui->gridSlider, SIGNAL(valueChanged(int)),
            renderer,                SLOT(updateGrid(int)));
    connect(toolBar->ui->checkGrid, SIGNAL(toggled(bool)),
            renderer,               SLOT(displayGrid(bool)));
    connect(toolBar->ui->zoomSlider, SIGNAL(valueChanged(int)),
            renderer,                SLOT(updateZoom(int)));
    connect(toolBar->ui->rotationSlider, SIGNAL(valueChanged(int)),
            renderer,                    SLOT(rotateDiagram(int)));

    connect(renderer, SIGNAL(nonEmptySelection(bool)),
            toolBar->ui->removeButton, SLOT(setEnabled(bool)));
    connect(toolBar->ui->removeButton, SIGNAL(pressed()),
            renderer,                  SLOT(erase()));
    connect(toolBar->ui->centerButton, SIGNAL(pressed()),
            renderer,                  SLOT(centerDiagram()));
    connect(toolBar->ui->selectButton, SIGNAL(pressed()),
            renderer,                  SLOT(select()));
    connect(toolBar->ui->addButton, SIGNAL(pressed()),
            this,                   SLOT(newDiagram()));

    connect(toolBar->ui->checkNodesOnGrid, SIGNAL(toggled(bool)),
            renderer,                      SLOT(forceNodesOnGrid(bool)));

    connect(toolBar->ui->addButton, SIGNAL(clicked(bool)),
            this, 					SLOT(setInsertMode(bool)));
    connect(toolBar->ui->selectButton, SIGNAL(clicked(bool)),
            this, 					SLOT(setSelectMode(bool)));
    connect(toolBar->ui->normalButton, SIGNAL(clicked(bool)),
            this, 					SLOT(setNormalMode(bool)));

    connect(toolBar->ui->gridResetButton, SIGNAL(pressed()),
            this,                         SLOT(resetGrid()));
    connect(toolBar->ui->zoomResetButton, SIGNAL(pressed()),
            this,                         SLOT(resetZoom()));
    connect(toolBar->ui->rotationResetButton, SIGNAL(pressed()),
            this,                             SLOT(resetRotation()));

    connect(toolBar->ui->prec_page, SIGNAL(pressed()),
            this, SLOT(prevPage()));
    connect(toolBar->ui->next_page, SIGNAL(pressed()),
            this, SLOT(nextPage()));
}

void MainWidget::newDiagram()
{
    if (renderer->mode == DiagramRenderer::SessionMode) {
        renderer->addDiagram();
        //renderer->setDiagram(renderer->diagrams.size() - 1);
        //setDiagramMode();
        //toolBar->ui->normalButton->setChecked(true);
        //toolBar->ui->addButton->setChecked(false);
        //toolBar->ui->selectButton->setChecked(false);
        //setNormalMode(true);
    }
}

void MainWidget::newSession()
{
    if (maybeSave()) {
        renderer->clear();
        renderer->newDiagram();
        setCurrentFile(QString());
        setSaved(true);
        //setDiagramMode();
    }
}

void MainWidget::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this,
                    QObject::tr("Open File"),
                    "",
                    QObject::tr("Diagrams (*.json)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
    setSaved(true);
    update();
}

void MainWidget::loadFile(QString const &fileName)
{
    renderer->clear();
    renderer->readFile(fileName);
    setSaved(true);
}

void MainWidget::prevPage()
{
    auto [first, last] = renderer->prevPage();
    std::string text = std::to_string(first) + " - " + std::to_string(last);
    toolBar->ui->page->setText(QString::fromStdString(text));
    toolBar->update();
}

void MainWidget::nextPage()
{
    auto [first, last] = renderer->nextPage();
    std::string text = std::to_string(first) + " - " + std::to_string(last);
    toolBar->ui->page->setText(QString::fromStdString(text));
    toolBar->update();
}

bool MainWidget::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWidget::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(
                this,
                QObject::tr("Save File"),
                ".json",
                QObject::tr("Diagrams (*.json)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWidget::maybeSave()
{
    if (saved)
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save
                               | QMessageBox::Discard
                               | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWidget::saveFile(QString const &fileName)
{
    renderer->save(fileName);
    setSaved(true);
    return true;
}

void MainWidget::exportPNG()
{
    QString name = toolBar->ui->lineSessionName->text();
    if (name.isEmpty())
        name = "diagram";
    if (renderer->mode == DiagramRenderer::DiagramMode) {
        renderer->grid.hide();
        QString path = QFileDialog::getExistingDirectory(
                    this,
                    QObject::tr("Save file in folder")
                    );
        if (path.isEmpty())
            return;

        QString fileName = path + "/" + name + "_"
                + QString::number(renderer->indexDiagram)
                + ".png";
        if (renderer->diag->diagram->getName() != "")
            fileName = path + "/"
                    + renderer->diag->diagram->getName()
                    + ".png";
        renderer->diag->diagram->exportSelfPNG(fileName);
        if(toolBar->ui->checkGrid->isChecked())
            renderer->grid.show();
    }
    else {
        QString path = QFileDialog::getExistingDirectory(
                    this,
                    QObject::tr("Save file in folder")
                    );
        if (path.isEmpty())
            return;
        size_t iDiagram = 0;
        renderer->setPage(0);
        for (qint32 i = 0; i != renderer->links.size(); ++i) {
            QString fileName = path + "/" + name + "_"
                    + QString::number(i)
                    + ".png";
            if (renderer->diagrams[iDiagram]->getName() != "")
                fileName = path + "/"
                        + renderer->diagrams[iDiagram]->getName()
                        + ".png";
            renderer->diagrams[iDiagram]->exportSelfPNG(fileName);
            renderer->diagrams[iDiagram]->showNodes(false);
            if (++iDiagram == DiagramRenderer::pageSize) {
                renderer->nextPage();
                iDiagram = 0;
            }
        }
    }
    renderer->setPage(0);
    update();
}

void MainWidget::renderPDF(
        QPainter &painter,
        PDFOption &option)
{
    auto printer = option.getPDFWriter();
    int rowSize = option.rowSize;
    float ratio = option.imageRatio;
    float hMarginRatio = option.horizontalMarginRatio;
    float hSpaceRatio  = option.horizontalSpacingRatio;
    float vMarginRatio = option.verticalMarginRatio;
    float vSpaceRatio  = option.verticalSpacingRatio;
    int pageWidth  = option.width;
    int pageHeight = option.height;

    int boxWidth = pageWidth*(1 - 2*hMarginRatio - (rowSize-1)*hSpaceRatio) *1. / rowSize;
    int boxHeight = ratio * boxWidth;
    int columnSize = pageHeight*(1 - 2*vMarginRatio + vSpaceRatio)
            * 1. / (boxHeight + pageHeight*vSpaceRatio);
    int hMargin = hMarginRatio*pageWidth;
    int vMargin = vMarginRatio*pageHeight;
    int hSpace  = hSpaceRatio*pageWidth;
    int vSpace  = vSpaceRatio*pageHeight;

    int iRow = 0;
    int iCol = 0;
    size_t iDiagram = 0;
    size_t iCell = 0;
    renderer->setPage(0);
    for (size_t i = 0; i != renderer->links.size(); ++i) {
        QRectF bounds = renderer->diagrams[iDiagram]->getBounds();
        int x = hMargin + iCol*(boxWidth + hSpace);
        int y = vMargin + iRow*(boxHeight + vSpace);
        QRectF target(x, y, boxWidth, boxHeight);
        if (option.showNumbers) {
            painter.drawText(x, y, QString::fromStdString(std::to_string(iCell++)));
        }
        renderer->scene->render(&painter, target, bounds, Qt::KeepAspectRatio);
        ++iCol;
        if (iCol == rowSize) {
            iCol = 0;
            ++iRow;
        }
        if (iRow == columnSize) {
            iRow = 0;
            if (!printer) {
                renderer->setPage(0);
                update();
                return;
            }
            printer->newPage();
        }
        if (++iDiagram == DiagramRenderer::pageSize) {
            iDiagram = 0;
            renderer->nextPage();
        }
    }
    painter.end();
    renderer->setPage(0);
    update();
}

void MainWidget::exportPDF()
{
    PDFSetupDialog pdfDialog(this);
    int code = pdfDialog.exec();
    if (code == QDialog::Accepted) {
        QPdfWriter printer(pdfDialog.getSaveFile());
        printer.setPageSize(QPrinter::A4);
        printer.setPageOrientation(QPageLayout::Portrait);
        QPainter painter(&printer);
        auto option = pdfDialog.getOptions();
        option.setPDFWriter(&printer);
        renderPDF(painter, option);
    }
}

void MainWidget::latexSource()
{
    if (renderer->mode == DiagramRenderer::DiagramMode) {
        renderer->diag->diagram->refreshLinker();
        QString latex(renderer->diag->diagram->getLatexSource().c_str());
        DialogLatex *dialog = new DialogLatex(
                    latex,
                    this);
        dialog->exec();
    }
    else {
        QString latex;
        for (const auto &diag : renderer->diagrams) {
            diag->refreshLinker();
            latex += (diag->getLatexSource().c_str());
            latex += "\n\n\n";
        }
        DialogLatex *dialog = new DialogLatex(
                    latex,
                    this);
        dialog->exec();
    }
}

void MainWidget::latexSourceToClipboard()
{
    if (renderer->mode == DiagramRenderer::DiagramMode) {
        renderer->diag->diagram->refreshLinker();
        QString latex(renderer->diag->diagram->getLatexSource().c_str());
        QApplication::clipboard()->setText(latex);
    }
    else {

        QString latex;
        for (const auto &diag : renderer->diagrams) {
            diag->refreshLinker();
            latex += (diag->getLatexSource().c_str());
            latex += "\n\n\n";
        }
        QApplication::clipboard()->setText(latex);
    }
}

void MainWidget::latexExportPNG()
{

}

void MainWidget::latexExportPDF()
{

}

void MainWidget::setNormalMode(bool checked)
{
    if (renderer->mode != DiagramRenderer::DiagramMode
            or !checked
            or !toolBar->ui->normalButton->isCheckable())
        return;
    renderer->diag->diagram->normalMode();
    emit normalMode();
}

void MainWidget::setSelectMode(bool checked)
{
    if (renderer->mode != DiagramRenderer::DiagramMode
            or !checked
            or !toolBar->ui->selectButton->isCheckable())
        return;
    renderer->diag->diagram->selectionMode();
    emit selectMode();
}

void MainWidget::setInsertMode(bool checked)
{
    if (renderer->mode != DiagramRenderer::DiagramMode
            or !checked
            or !toolBar->ui->addButton->isCheckable())
        return;
    renderer->diag->diagram->insertionMode();
    emit insertMode();
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}

void MainWidget::modif()
{
    setSaved(false);
}

void MainWidget::resetGrid()
{
    toolBar->ui->gridSlider->setValue(0);
}
void MainWidget::resetZoom()
{
    toolBar->ui->zoomSlider->setValue(100);
}
void MainWidget::resetRotation()
{
    toolBar->ui->rotationSlider->setValue(180);
}
