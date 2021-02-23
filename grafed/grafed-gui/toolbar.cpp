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

#include "toolbar.h"
#include "ui_toolbar.h"

ToolBar::ToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolBar)
{
    ui->setupUi(this);

    connect(ui->normalButton, SIGNAL(clicked(bool)),
            this,             SLOT(removeChecked(bool)));
    connect(ui->addButton, SIGNAL(clicked(bool)),
            this,          SLOT(addChecked(bool)));
    connect(ui->selectButton, SIGNAL(clicked(bool)),
            this,             SLOT(selectChecked(bool)));
}

ToolBar::~ToolBar()
{
    delete ui;
}

void ToolBar::addChecked(bool checked)
{
    if (checked) {
        ui->selectButton->setChecked(false);
        ui->normalButton->setChecked(false);
    }
}

void ToolBar::selectChecked(bool checked)
{
    if (checked) {
        ui->normalButton->setChecked(false);
        ui->addButton->setChecked(false);
    }
}

void ToolBar::removeChecked(bool checked)
{
    if (checked) {
        ui->selectButton->setChecked(false);
        ui->addButton->setChecked(false);
    }
}
