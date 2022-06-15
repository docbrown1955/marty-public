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
#ifndef BORDER_H
#define BORDER_H

#include <QRectF>

class Border {

  public:
    Border() : first(true), border(0., 0., 0., 0.)
    {
    }

    ~Border()
    {
    }

    void reset()
    {
        border = {0., 0., 0., 0.};
        first  = true;
    }

    void update(QRectF rect)
    {
        if (first) {
            border = rect;
            first  = false;
            return;
        }
        if (rect.x() < border.x())
            border.setX(rect.x());
        if (rect.y() < border.y())
            border.setY(rect.y());
        if (rect.x() + rect.width() > border.x() + border.width())
            border.setWidth(rect.x() + rect.width() - border.x());
        if (rect.y() + rect.height() > border.y() + border.height())
            border.setHeight(rect.y() + rect.height() - border.y());
    }

    QRectF getBorder() const
    {
        QRectF res(border);

        return res;
    }

  private:
    bool   first;
    QRectF border;
};

#endif // BORDER_H
