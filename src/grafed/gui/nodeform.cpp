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

#include "nodeform.h"
#include "diagramrenderer.h"
#include "node.h"
#include "ui_nodeform.h"
#include <iostream>

NodeForm::NodeForm(Node *t_node, DiagramRenderer *t_parent)
    : QWidget(t_parent), ui(new Ui::NodeForm), renderer(t_parent), node(t_node)
{
    ui->setupUi(this);

    ui->lineName->setFocus();
    ui->colorButton->setAutoFillBackground(true);
    QString qss = QString("background-color: %1").arg(node->getColor().name());
    ui->colorButton->setStyleSheet(qss);
    ui->colorButton->update();
    ui->spinSize->setValue(node->getNodeSize());
    connect(ui->colorButton, SIGNAL(pressed()), this, SLOT(changeColor()));

    ui->lineName->setText(node->getName());
    connect(ui->lineName,
            SIGNAL(textChanged(QString const &)),
            node,
            SLOT(setName(QString const &)));

    ui->spinSize->setValue(node->getNodeSize());
    connect(
        ui->spinSize, SIGNAL(valueChanged(int)), node, SLOT(setNodeSize(int)));

    connect(
        ui->quitButton, SIGNAL(clicked()), renderer, SLOT(closeNodeForm()));

    QList<QString> files = {"",
                            ":/ressources/images/operator.png",
                            ":/ressources/images/cross.png",
                            ":/ressources/images/plain_disk.png",
                            ":/ressources/images/hatched_disk.png"};
    ui->tableType->setIconSize(QSize(150, 30));
    ui->tableType->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Fixed);
    ui->tableType->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableType->horizontalHeader()->setDefaultSectionSize(80);
    ui->tableType->verticalHeader()->setDefaultSectionSize(30);
    ui->tableType->horizontalHeader()->hide();
    ui->tableType->verticalHeader()->hide();
    ui->tableType->setSelectionBehavior(QAbstractItemView::SelectItems);
    qint32 i = 0;
    qint32 j = 0;
    ui->tableType->setColumnCount(2);
    ui->tableType->setRowCount(qint32(std::ceil(files.size() / 2.)));
    for (const auto &file : files) {
        QTableWidgetItem *item = new QTableWidgetItem(QIcon(file), nullptr);
        ui->tableType->setItem(i, j, item);
        if (++i == ui->tableType->rowCount()) {
            i = 0;
            j = 1;
        }
    }
    connect(ui->tableType,
            SIGNAL(cellClicked(int, int)),
            this,
            SLOT(nodeTypeChanged(int, int)));
}

NodeForm::~NodeForm()
{
    delete ui;
}

void NodeForm::changeColor()
{
    node->changeColor();
    ui->colorButton->setAutoFillBackground(true);
    QString qss = QString("background-color: %1").arg(node->getColor().name());
    ui->colorButton->setStyleSheet(qss);
    ui->colorButton->update();
}

void NodeForm::nodeTypeChanged(int i, int j)
{
    node->setNodeType(i + 3 * j);
}
