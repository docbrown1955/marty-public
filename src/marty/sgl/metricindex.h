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

/**
 * @file metricindex.h
 * @brief sgl::MetricIndex, symbolic Minkowski metric in SGL.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "generalizedindex.h"

namespace sgl {

class MetricIndex : public AbstractGeneralizedIndex {

  public:
    MetricIndex(csl::Index const &a, csl::Index const &b);

    bool isZero() const override;

    GExpr copy() const override;
    GExpr refresh() const override;

    csl::Expr toCSL(TensorSet const &) const override;

    csl::Expr getFactor() const override;
    GExpr     getTerm() const override;

    bool  hasPropertyWith(GExpr const &other) const override;
    GExpr propertyWith(GExpr const &other) const override;

    void print(std::ostream &out = std::cout) const override;
};

GExpr metricindex_s(csl::Index const &a, csl::Index const &b);
} // namespace sgl
