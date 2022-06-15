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

#include "edgeform.h"
#include "diagramrenderer.h"
#include "edge.h"
#include "ui_edgeform.h"

EdgeForm::EdgeForm(Edge *t_edge, DiagramRenderer *parent)
    : QWidget(parent), ui(new Ui::EdgeForm), edge(t_edge), renderer(parent)
{
    ui->setupUi(this);

    ui->lineName->setFocus();
    ui->colorButton->setAutoFillBackground(true);
    QString qss = QString("background-color: %1").arg(edge->getColor().name());
    ui->colorButton->setStyleSheet(qss);
    ui->colorButton->update();
    connect(ui->colorButton, SIGNAL(pressed()), this, SLOT(changeColor()));

    ui->lineName->setText(edge->getName());
    connect(ui->lineName,
            SIGNAL(textChanged(QString const &)),
            edge,
            SLOT(setName(QString const &)));

    ui->checkFlip->setChecked(edge->isFlipped());
    connect(
        ui->checkFlip, SIGNAL(stateChanged(int)), this, SLOT(setFlipped(int)));

    ui->spinlineWidth->setValue(edge->getLineWidth());
    connect(ui->spinlineWidth,
            SIGNAL(valueChanged(int)),
            edge,
            SLOT(setLineWidth(int)));

    QList<QString> files = {":/ressources/images/Scalar.png",
                            ":/ressources/images/Charged_Scalar.png",
                            ":/ressources/images/Anti_Charged_Scalar.png",
                            ":/ressources/images/Fermion.png",
                            ":/ressources/images/Anti_Fermion.png",
                            ":/ressources/images/Majorana.png",
                            ":/ressources/images/Vector.png",
                            ":/ressources/images/Gluon.png",
                            ":/ressources/images/Ghost.png",
                            ":/ressources/images/Anti_Ghost.png",
                            ":/ressources/images/Gaugino.png",
                            ":/ressources/images/Anti_gaugino.png",
                            ":/ressources/images/Gluino.png",
                            ":/ressources/images/Anti_Gluino.png"};
    ui->listType->setIconSize(QSize(150, 30));
    ui->listType->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listType->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listType->horizontalHeader()->setDefaultSectionSize(80);
    ui->listType->verticalHeader()->setDefaultSectionSize(30);
    ui->listType->horizontalHeader()->hide();
    ui->listType->verticalHeader()->hide();
    ui->listType->setSelectionBehavior(QAbstractItemView::SelectItems);
    qint32 i = 0;
    qint32 j = 0;
    ui->listType->setColumnCount(2);
    ui->listType->setRowCount(qint32(std::ceil(files.size() / 2.)));
    for (const auto &file : files) {
        QTableWidgetItem *item = new QTableWidgetItem(QIcon(file), nullptr);
        ui->listType->setItem(i, j, item);
        if (++i == ui->listType->rowCount()) {
            i = 0;
            j = 1;
        }
    }
    connect(ui->listType,
            SIGNAL(cellClicked(int, int)),
            this,
            SLOT(particleTypeChange(int, int)));

    connect(
        ui->pushButton, SIGNAL(clicked()), renderer, SLOT(closeEdgeForm()));
}

EdgeForm::~EdgeForm()
{
    delete ui;
}

void EdgeForm::setFlipped(int state)
{
    if (state == Qt::Checked)
        edge->setFlipped(true);
    else if (state == Qt::Unchecked)
        edge->setFlipped(false);
}

void EdgeForm::changeColor()
{
    edge->changeColor();
    ui->colorButton->setAutoFillBackground(true);
    QString qss = QString("background-color: %1").arg(edge->getColor().name());
    ui->colorButton->setStyleSheet(qss);
    ui->colorButton->update();
}

void EdgeForm::particleTypeChange(int row, int col)
{
    for (qint32 i = 0; i != ui->listType->rowCount(); ++i)
        for (qint32 j = 0; j != ui->listType->columnCount(); ++j) {
            if (i != row or j != col) {
                ui->listType->item(i, j)->setSelected(false);
            }
        }
    QList<int> types = {1, 2, -2, 3, -3, 4, 5, 6, 9, -9, 7, -7, 8, -8};
    edge->setParticleType(types[col * ui->listType->rowCount() + row]);
    edge->update();
}
