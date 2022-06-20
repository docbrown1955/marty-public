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

#include "curvature.h"
#include <QPainterPath>
#include <cmath>
#include <iostream>

namespace curvature {

double getTotalLength(double curvature, double length)
{
    if (curvature == 0.)
        return length;
    double c = std::abs(curvature);
    double L = length;

    return (M_PI - 2 * std::acos(c * L / 2)) / c;
}
void applyCurvature(QPainterPath &path, double curvature, double length)
{
    if (curvature == 0.)
        return;
    double L    = getTotalLength(curvature, length);
    double c    = std::abs(curvature);
    double h    = std::sqrt(1 - c * c * length * length / 4) / c;
    int    sign = (curvature < 0) ? -1 : 1;

    qreal theta_min = std::acos(c * length / 2);
    qreal theta_max = M_PI - theta_min;
    int   N         = path.elementCount();
    for (int i = 0; i != N; ++i) {
        QPointF point = path.elementAt(N - 1 - i);
        double  theta
            = (L - point.x()) / L * (theta_max - theta_min) + theta_min;
        double  y = point.y();
        QPointF newPoint(
            length / 2 + std::cos(theta) / c + y * std::cos(theta),
            sign * (h - std::sin(theta) / c - y * std::sin(theta)));
        path.setElementPositionAt(N - 1 - i, newPoint.x(), newPoint.y());
    }
}

} // namespace curvature
