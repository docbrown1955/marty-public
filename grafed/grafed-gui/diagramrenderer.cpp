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

#include <iostream>
#include <fstream>
#include <QScrollBar>
#include <QHeaderView>
#include "diagramrenderer.h"
#include "ui_diagramrenderer.h"
#include "latexLink.h"
#include "edgeform.h"
#include "nodeform.h"
#include "node.h"
#include <algorithm>
#include <QMessageBox>

DiagramRenderer::DiagramRenderer(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DiagramRenderer),
    nColumns(4),
    cellSize(600),
    minWidth(300),
    minHeight(300),
    firstDiag(0),
    lastDiag(0),
    pageNumber(0),
    nextDiagram(0),
    edgeForm(nullptr),
    nodeForm(nullptr),
    zoomValue(1.),
    displayGridValue(true)
{
    compiler = new latexcompiler(this);
    m_ui->setupUi(this);
    init();
}

DiagramRenderer::DiagramRenderer(
        QString const &nameFile,
        QWidget       *parent)
    :DiagramRenderer(parent)
{
    std::ifstream f(nameFile.toStdString());
    if (!f)
        newDiagram();
    else {
        f.close();
        readFile(nameFile);
    }
    update();
}

void DiagramRenderer::init()
{
    mode = SessionMode;
    indexDiagram = 0;
    clipBoardDiagram.clear();
    diag = nullptr;
    scene = new QGraphicsScene(this);
    grid.setScene(scene);
    grid.hide();
    table = new QTableWidget(this);
    table->setColumnCount(nColumns);
    table->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Fixed);
    table->verticalHeader()->setSectionResizeMode(
                QHeaderView::Fixed);
    table->horizontalHeader()->setDefaultSectionSize(minWidth);
    table->verticalHeader()->setDefaultSectionSize(minHeight);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->setStyleSheet("QTableView {border-color: red;}");
    table->setRowCount(0);

    table->setSelectionMode(QAbstractItemView::NoSelection);

    hlayout = new QHBoxLayout(this);
    hlayout->addWidget(table);
    setLayout(hlayout);

    connect(table, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(diagramDoubleClicked(qint32, qint32)));

    connect(table, SIGNAL(cellClicked(int, int)),
            this,  SLOT(cellPressed(int, int)));
}

DiagramRenderer::~DiagramRenderer()
{
    clear();
    delete m_ui;
}

void DiagramRenderer::clearScene()
{
    grid.clear();
    if (diag)
        delete diag;
    for (auto &d : diagrams)
        delete d;
    delete table;
    delete scene;
    diagrams.clear();
    resetSelection();
    delete hlayout;
    init();
}

void DiagramRenderer::clear()
{
    grid.clear();
    if (diag)
        delete diag;
    for (auto &d : diagrams)
        delete d;
    delete table;
    delete scene;
    diagrams.clear();
    links.clear();
    resetSelection();
    delete hlayout;
    init();
}

void DiagramRenderer::toggle()
{
    clipBoardDiagram.clear();
    if (mode == SessionMode)
        setDiagramMode();
    else
        setSessionMode();
}

void DiagramRenderer::updateGrid(int factor)
{
    qint32 L = cellSize;
    grid.draw(
                -L/2,
                -L/2,
                -L/2 + L * nColumns,
                -L/2 + L * table->rowCount(),
                factor
             );
    for (auto &d : diagrams)
        d->setGridSize(grid.getSize());
}

void DiagramRenderer::displayGrid(bool display)
{
    displayGridValue = display;
    if (display)
        grid.show();
    else
        grid.hide();
}

void DiagramRenderer::forceNodesOnGrid(bool force)
{
    for (auto &d : diagrams)
        d->forceNodeOnGrid(force);
}

void DiagramRenderer::centerDiagram()
{
    diagrams[indexDiagram]->center();
}

void DiagramRenderer::updateZoom(int zoom)
{
    zoomValue = zoom / 100.;
    diag->resetTransform();
    diag->zoom(zoomValue);
}

void DiagramRenderer::rotateDiagram(int rotation)
{
    diagrams[indexDiagram]->rotateTo(rotation);
}

bool DiagramRenderer::setDiagramMode()
{
    if (mode == DiagramMode)
        return true;

    //table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (!setDiagram()) {
        return false;
    }

    mode = DiagramMode;
    table->hide();
    layout()->addWidget(diag);
    diag->show();
    if (displayGridValue)
        grid.show();

    emit diagramMode();

    update();
    return true;
}

void DiagramRenderer::setSessionMode()
{
    if (mode == SessionMode)
        return;

    mode = SessionMode;
    diag->hide();
    layout()->removeWidget(diag);
    setTable();
    table->show();
    grid.hide();

    emit sessionMode();

    update();
}

void DiagramRenderer::setTable()
{
    resetSelection();
    closeNodeForm();
    closeEdgeForm();
    qint32 iRow = indexDiagram / nColumns;
    qint32 iCol = indexDiagram - nColumns * iRow;
    auto graph = new DiagramWidget(
                scene, cellSize*iCol, cellSize*iRow, this, diagrams[indexDiagram]);
    diagrams[indexDiagram]->showNodes(false);
    diagrams[indexDiagram]->resetFocus();
    diagrams[indexDiagram]->setActiveWidget(graph);
    graph->resetTransform();
    graph->centerScene();
    graph->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graph->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graph->setUpScene();
    graph->scaleView(0.6);
    graph->disableDiagram();
    graph->setEnabled(false);
    graph->update();
    table->setCellWidget(iRow, iCol, graph);
    table->cellWidget(iRow, iCol)->setToolTip(
                QString("%1").arg(iRow*nColumns + iCol));
    table->update();
}

bool DiagramRenderer::setDiagram(qint32 i)
{
    if (i == -1) {
        i = indexDiagram;
    }
    if (i < 0 or i >= diagrams.size())
        return false;

    indexDiagram = i;
    qint32 iRow = i / nColumns;
    qint32 iCol = i - nColumns * iRow;
    diag = new DiagramWidget(
                scene, cellSize*iCol, cellSize*iRow, this, diagrams[i]);
    diag->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Expanding
                );
    diagrams[i]->showNodes(true);
    diagrams[i]->setActiveWidget(diag);
    diag->enableDiagram();
    diag->setEnabled(true);
    diag->zoom(zoomValue);
    diag->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    diag->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    diag->update();
    table->removeCellWidget(iRow, iCol);

    return true;
}

void DiagramRenderer::cellPressed(int i, int j)
{
    if (i * nColumns + j >= diagrams.size())
        return;
    auto pos = std::find_if(selectedDiagrams.begin(),
                            selectedDiagrams.end(),
                            [&](std::pair<int, int> const &p)
                            {
                                 return p.first == i and p.second == j;
                            }
                            );
    if (pos == selectedDiagrams.end()) {
        diagrams[i*nColumns + j]->widget->setForegroundBrush(
                    QColor(0, 255, 0, 32));
        selectedDiagrams.push_back({i, j});
        if (selectedDiagrams.size() == 1)
            emit nonEmptySelection(true);
    }
    else {
        diagrams[i*nColumns + j]->widget->setForegroundBrush(
                    Qt::transparent);
        selectedDiagrams.erase(pos);
        if (selectedDiagrams.empty())
            emit nonEmptySelection(false);
    }
}

void DiagramRenderer::resetSelection()
{
    for (auto &diag : diagrams) {
        diag->widget->setForegroundBrush(Qt::transparent);
    }
    selectedDiagrams.clear();
    emit nonEmptySelection(false);
}

// Function not ready to be used
void DiagramRenderer::setDiagramSet(qint32 first, qint32 last)
{
    if (mode == SessionMode) {
        firstDiag = first;
        lastDiag  = last;
        diagrams = QList<Diagram*>{};
        table->clear();
        for (qint32 i = first; i != last; ++i) {
            auto res = newDiagram();
            res.first->loadLinker(links[static_cast<std::size_t>(i)]);
            res.first->showNodes(false);
            diagrams.append(res.first);
        }
        init();
    }
}

void DiagramRenderer::closeNodeForm()
{
    if (nodeForm) {
        layout()->removeWidget(nodeForm);
        nodeForm->close();
        delete nodeForm;
        nodeForm = nullptr;
    }
}

void DiagramRenderer::openNodeForm(Node *node)
{
    closeEdgeForm();
    closeNodeForm();
    nodeForm = new NodeForm(node, this);
    layout()->addWidget(nodeForm);
    for (auto &node : diagrams[indexDiagram]->nodes) {
        node->unFocusInGraph();
    }
    node->focusInGraph();
}

void DiagramRenderer::closeEdgeForm()
{
    if (edgeForm) {
        layout()->removeWidget(edgeForm);
        edgeForm->close();
        delete edgeForm;
        edgeForm = nullptr;
    }
}

void DiagramRenderer::openEdgeForm(Edge *edge)
{
    closeEdgeForm();
    closeNodeForm();
    edgeForm = new EdgeForm(edge, this);
    layout()->addWidget(edgeForm);
}

void DiagramRenderer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        resetSelection();
        closeEdgeForm();
        closeNodeForm();
    }
    else if (event->key() == Qt::Key_Delete) {
        erase();
    }
    else if (event->key() == Qt::Key_C
             and event->modifiers().testFlag(Qt::ControlModifier)) {
        copy();
    }
    else if (event->key() == Qt::Key_V
             and event->modifiers().testFlag(Qt::ControlModifier)) {
        paste();
    }
    QWidget::keyPressEvent(event);
    update();
}

void DiagramRenderer::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    update();
}

std::pair<Diagram*, DiagramWidget*> DiagramRenderer::generateDiagram(
        qint32 i,
        qint32 j)
{
    Diagram *diagram = new Diagram(scene, cellSize * j , cellSize * i, this);
    diagram->showNodes(false);
    diagram->setGridSize(grid.getSize());
    DiagramWidget *graph = new DiagramWidget(
                scene,
                cellSize * j,
                cellSize * i,
                this,
                diagram);
    diagram->setActiveWidget(graph);
    graph->resetTransform();
    graph->centerScene();
    graph->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graph->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graph->setUpScene();
    graph->scaleView(0.6);
    graph->disableDiagram();
    graph->setEnabled(false);
    graph->update();

    return {diagram, graph};
}
std::pair<Diagram*, DiagramWidget*> DiagramRenderer::newDiagram()
{
    qint32 nDiag = qint32(diagrams.size());
    qint32 i = nDiag / nColumns;
    qint32 j = nDiag - i * nColumns;
    if (j == 0) {
        table->setRowCount(table->rowCount() + 1);
        updateGrid();
    }
    auto [diagram, graph] = generateDiagram(i, j);
    diagrams.push_back(diagram);
    ++lastDiag;
    table->setCellWidget(i, j, graph);
    table->cellWidget(i, j)->setToolTip(
                QString("%1").arg(pageSize*pageNumber+i*nColumns + j));

    return {diagram, graph};
}

void DiagramRenderer::readFile(QString const &nameFile)
{
    links = drawer::LatexLinker::loadMultiple(nameFile.toStdString());
    loadDiagrams(0);
    drawer::LatexLinker::autoLabel = false;
}

std::size_t DiagramRenderer::maxDiagramNumber(std::size_t first)
{
    return std::min(links.size(), first+pageSize);
}

std::pair<std::size_t, std::size_t> DiagramRenderer::page()
{
    std::size_t first = pageNumber*pageSize;
    return {first, maxDiagramNumber(first)};
}

void DiagramRenderer::refreshLinks()
{
    std::size_t offset = pageNumber*pageSize;
    std::size_t nDiags = diagrams.size();
    if (offset + nDiags > nextDiagram) {
        links.insert(links.begin() + offset, offset + nDiags - nextDiagram, drawer::LatexLinker());
    }
    else if (offset + nDiags < nextDiagram) {
        links.erase(links.begin() + offset, links.begin() + nextDiagram - nDiags);
    }
    for (std::size_t i = 0; i != nDiags; ++i) {
        diagrams[int(i)]->refreshLinker();
        links[i+offset] = diagrams[int(i)]->link;
    }
}

std::pair<std::size_t, std::size_t> DiagramRenderer::prevPage()
{
    if (pageNumber > 0) {
        refreshLinks();
        --pageNumber;
        std::size_t first = pageNumber*pageSize;
        loadDiagrams(first);
        return {first, maxDiagramNumber(first)};
    }
    else {
        std::size_t first = pageNumber*pageSize;
        return {first, maxDiagramNumber(first)};
    }
}

std::pair<std::size_t, std::size_t> DiagramRenderer::nextPage()
{
    std::size_t first = pageNumber*pageSize;
    if (maxDiagramNumber(first) < links.size()) {
        refreshLinks();
        ++pageNumber;
        first += pageSize;
        loadDiagrams(first);
        return {first, maxDiagramNumber(first)};
    }
    else {
        return {first, maxDiagramNumber(first)};
    }
}

std::pair<std::size_t, std::size_t> DiagramRenderer::setPage(std::size_t t_page)
{
    if (t_page != pageNumber && pageNumber*pageSize < links.size()) {
        refreshLinks();
        pageNumber = t_page;
        std::size_t first = pageNumber*pageSize;
        loadDiagrams(first);
        return {first, maxDiagramNumber(first)};
    }
    else {
        std::size_t first = pageNumber*pageSize;
        return {first, maxDiagramNumber(first)};
    }
}

void DiagramRenderer::loadDiagrams(std::size_t first)
{
    clearScene();
    std::size_t last = maxDiagramNumber(first);
    nextDiagram = last;
    for (std::size_t i = first; i != last; ++i) {
        auto res = newDiagram();
        res.first->loadLinker(links[i]);
        res.first->showNodes(false);
    }
}

void DiagramRenderer::addDiagram()
{
    newDiagram();
    table->update();
    setDiagram(diagrams.size() - 1);
    if (mode == SessionMode) {
        toggle();
    }
    modificationDone();
}

void DiagramRenderer::diagramDoubleClicked(qint32 iRow, qint32 iCol)
{
    qint32 i = iRow * nColumns + iCol;
    if (setDiagram(i))
        toggle();
}

void DiagramRenderer::add()
{
    if (mode == SessionMode) {
        clipBoardDiagram.clear();
        resetSelection();
        for (const auto &d : diagrams)
            if (d->nodes.empty())
                return;
        auto res = newDiagram();
        res.first->showNodes(false);
        updateGrid();
        table->update();
        modificationDone();
    }
}

void DiagramRenderer::select()
{

}

void DiagramRenderer::erase()
{
    if (mode != SessionMode)
        return;
    if (selectedDiagrams.empty())
        return;
    clipBoardDiagram.clear();
    QString message = (selectedDiagrams.size() == 1) ?
        "Are you sure you want to delete this diagram?"
        : QString("Are you sure you want to delete these ")
                + QString::number(selectedDiagrams.size()) + " diagrams?";

    int ret = QMessageBox::warning(
          this,
          tr("Deletion confirmation"),
          message,
          QMessageBox::Cancel | QMessageBox::Yes,
          QMessageBox::Yes);

    if (ret == QMessageBox::Yes) {
        for (auto &item : selectedDiagrams) {
            int iRow = item.first;
            int iCol = item.second;
            int pos = iRow * nColumns + iCol;
            table->removeCellWidget(iRow, iCol);
            delete diagrams[pos];
            diagrams[pos] = nullptr;
        }
        int iRow = 0;
        int iCol = 0;
        int iRowNew = 0;
        int iColNew = 0;
        for (int i = 0; i != diagrams.size(); ++i) {
            if (iCol != iColNew or iRow != iRowNew) {
                if (diagrams[i]) {
                    diagrams[i]->moveTo(cellSize * iColNew, cellSize * iRowNew);
                    auto graph = new DiagramWidget(
                                scene,
                                cellSize * iColNew,
                                cellSize * iRowNew,
                                this,
                                diagrams[i]);
                    diagrams[i]->setActiveWidget(graph);
                    graph->resetTransform();
                    graph->centerScene();
                    graph->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    graph->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    graph->setUpScene();
                    graph->scaleView(0.6);
                    graph->disableDiagram();
                    graph->setEnabled(false);
                    graph->update();
                    table->setCellWidget(
                                iRowNew, iColNew, graph
                                );
                }
            }
            if (!diagrams[i]) {
                diagrams.erase(diagrams.begin() + i);
                --i;
                --iColNew;
            }
            ++iCol;
            ++iColNew;
            if (iCol == nColumns) {
                iCol = 0;
                ++iRow;
            }
            if (iColNew == nColumns) {
                iColNew = 0;
                ++iRowNew;
            }
        }
        for (int i = iColNew; i != nColumns; ++i)
            table->removeCellWidget(iRowNew, i);
        iRowNew = (iColNew == 0) ? iRowNew-1 : iRowNew;
        if (iRowNew < iRow) {
            table->setRowCount(std::max(1, iRowNew + 1));
            updateGrid();
        }
        //qint32 firstDisplaced = diagrams.size() - 1;
        //for (const auto &[row, col] : selectedDiagrams) {
        //   qint32 pos = row * nColumns + col;
        //    if (pos < firstDisplaced)
        //        firstDisplaced = pos;
        //}
        for (qint32 i = 0; i != diagrams.size(); ++i)
            refresh(i);
        if (!diagrams.empty())
            refresh(0);
        resetSelection();
        modificationDone();
    }
}

void DiagramRenderer::save(QString const &fileName)
{
    refreshLinks();
    drawer::LatexLinker::saveMultiple(fileName.toStdString(), links);
}

void DiagramRenderer::copy()
{
    if (selectedDiagrams.empty())
        return;
    clipBoardDiagram.clear();
    for (const auto &[row, col] : selectedDiagrams)
        clipBoardDiagram.push_back(row * nColumns + col);
}

void DiagramRenderer::paste()
{
    if (clipBoardDiagram.empty())
        return;

    std::sort(clipBoardDiagram.begin(), clipBoardDiagram.end());
    //qint32 firstPasted = diagrams.size();
    for (qint32 pos : clipBoardDiagram)
        paste(pos);
    for (qint32 i = 0; i != diagrams.size(); ++i)
        refresh(i);
}

void DiagramRenderer::paste(qint32 pos)
{
    diagrams[pos]->refreshLinker();
    auto res = newDiagram();
    res.first->loadLinker(diagrams[pos]->link);
    res.first->showNodes(false);
}

void DiagramRenderer::refresh(qint32 pos)
{
    if (setDiagram(pos)) {
        setDiagramMode();
        setSessionMode();
        diagrams[pos]->refresh();
        diagrams[pos]->widget->update();
    }
}

void DiagramRenderer::modificationDone()
{
    emit modif();
}

