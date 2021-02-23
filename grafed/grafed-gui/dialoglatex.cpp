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

#include "dialoglatex.h"
#include "ui_dialoglatex.h"

DialogLatex::DialogLatex(
        QString const &t_text,
        QWidget       *parent)
    :QDialog(parent),
    ui(new Ui::DialogLatex),
    text(t_text)
{
    ui->setupUi(this);
    ui->text->setPlainText(text);
}

DialogLatex::~DialogLatex()
{
    delete ui;
}
