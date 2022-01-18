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

#include "grid.h"
#include <QGraphicsLineItem>

Grid::Grid(QGraphicsScene *t_scene)
    :isShown(true),
    scene(t_scene)
{
    dl0 = defaultGridSize;
    color = Qt::darkGray;
    color.setAlpha(130);
    width = 1;
}

void Grid::clear()
{
    if (!scene)
        return;
    for (auto &l: lines)
        delete l;
    lines.clear();
}

/////////////
/// factor == 0 means no factor
/// factor = +n means grid-spacing multiplied by (n + 1)
/// factor = -n means grid-spacing divided    by (n + 1)
////
void Grid::draw(
        qint32 xmin,
        qint32 ymin,
        qint32 xmax,
        qint32 ymax,
        qint32 factor)
{
    if (!scene)
        return;
    clear();
    QPen pen(color);
    pen.setWidth(width);
    if (factor == 0)
        factor = 1;
    else if (factor > 0)
        ++factor;
    else
        --factor;
    qreal rfactor = (factor > 0) ? factor : -1./factor;
    size = qint32(dl0 * rfactor);
    qint32 dx = size;
    qint32 dy = size;

    for (int x = 0; x <= xmax; x += dx) {
        auto line = new QGraphicsLineItem(x, ymin, x, ymax);
        line->setPen(pen);
        if (!isShown)
            line->hide();
        lines.push_back(line);
        scene->addItem(line);
    }for (int x = -dx; x >= xmin; x -= dx) {
        auto line = new QGraphicsLineItem(x, ymin, x, ymax);
        line->setPen(pen);
        if (!isShown)
            line->hide();
        lines.push_back(line);
        scene->addItem(line);
    }
    for (int y = 0; y <= ymax; y += dy) {
        auto line = new QGraphicsLineItem(xmin, y, xmax, y);
        line->setPen(pen);
        if (!isShown)
            line->hide();
        lines.push_back(line);
        scene->addItem(line);
    }
    for (int y = -dy; y >= ymin; y -= dy) {
        auto line = new QGraphicsLineItem(xmin, y, xmax, y);
        line->setPen(pen);
        if (!isShown)
            line->hide();
        lines.push_back(line);
        scene->addItem(line);
    }
}

void Grid::show()
{
    isShown = true;
    for (auto &l : lines)
        l->show();
}
void Grid::hide()
{
    isShown = false;
    for (auto &l : lines)
        l->hide();
}

void Grid::setScene(QGraphicsScene *t_scene)
{
    scene = t_scene;
}
