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
#ifndef NODEFORM_H
#define NODEFORM_H

#include <QWidget>

namespace Ui {
class NodeForm;
}

class Node;
class DiagramRenderer;

class NodeForm : public QWidget
{
    Q_OBJECT

public:
    explicit NodeForm(
            Node *t_node,
            DiagramRenderer *t_parent = nullptr);
    ~NodeForm();

public slots:

    void changeColor();
    void nodeTypeChanged(int, int);

private:
    Ui::NodeForm *ui;
    DiagramRenderer *renderer;
    Node *node;
};

#endif // NODEFORM_H
