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
 * @version 1.3

 * \brief
 */
#ifndef EDGEFORM_H
#define EDGEFORM_H

#include <QWidget>

namespace Ui {
class EdgeForm;
}

class Edge;
class DiagramRenderer;
class EdgeForm : public QWidget {
    Q_OBJECT

  public:
    explicit EdgeForm(Edge *edge, DiagramRenderer *parent = nullptr);

    ~EdgeForm();

  public slots:

    void particleTypeChange(int row, int col);
    void setFlipped(int state);
    void changeColor();

  private:
    Ui::EdgeForm *   ui;
    Edge *           edge;
    DiagramRenderer *renderer;
};

#endif // EDGEFORM_H
