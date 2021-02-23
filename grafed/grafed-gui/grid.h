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
#ifndef GRID_H
#define GRID_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>

class Grid
{
public:

    inline constexpr static qint32 defaultGridSize = 25;

    Grid(QGraphicsScene *t_scene = nullptr);

    ~Grid() { clear(); }

    qint32 getSize() const { return size; }

    void clear();

    void draw(
            qint32 xmin,
            qint32 ymin,
            qint32 xmax,
            qint32 ymax,
            qint32 factor = 0);

    void show();
    void hide();

    void setScene(QGraphicsScene *t_scene);

private:

    bool                      isShown;
    QGraphicsScene           *scene;
    QList<QGraphicsLineItem*> lines;

public:

    qint32 size;
    qint32 dl0;
    QColor color;
    qint32 width;
};

#endif // GRID_H
