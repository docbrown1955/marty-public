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

#include "metricindex.h"
#include "cslexpr.h"
#include "debuglog.h"
#include "exceptions.h"
#include "gammaindex.h"
#include "sglinterface.h"

namespace sgl {

MetricIndex::MetricIndex(csl::Index const &a, csl::Index const &b)
    : AbstractGeneralizedIndex({a, b})
{
}

bool MetricIndex::isZero() const
{
    return false;
}

GExpr MetricIndex::copy() const
{
    return std::make_shared<MetricIndex>(m_indices[0], m_indices[1]);
}

GExpr MetricIndex::refresh() const
{
    return metricindex_s(m_indices[0], m_indices[1]);
}

csl::Expr MetricIndex::getFactor() const
{
    if (m_indices[0] == m_indices[1]) {
        if (m_indices[0].getType() == cslIndex::Fixed)
            return minkoSpace->getMetric()->getTensor()->getArgument(
                {m_indices[0].getValue(), m_indices[0].getValue()});
        return sgl::DMinko;
    }
    return CSL_1;
}

GExpr MetricIndex::getTerm() const
{
    return (getFactor() == CSL_1) ? copy() : cslexpr_s(1);
}

csl::Expr MetricIndex::toCSL(TensorSet const &) const
{
    return m_indices[0].getSpace()->getMetric()(m_indices);
}

bool MetricIndex::hasPropertyWith(GExpr const &other) const
{
    return other->contains(m_indices[0]) || other->contains(m_indices[1]);
}
GExpr MetricIndex::propertyWith(GExpr const &other) const
{
    SCOPELOG
    LOG("Metric contraction", copy(), "with", other)
    if (other->contains(m_indices[0])) {
        GExpr res = sgl::DeepCopy(other);
        res->replace(m_indices[0], m_indices[1]);
        LOG("Res :", res)
        return res;
    }
    else if (other->contains(m_indices[1])) {
        GExpr res = sgl::DeepCopy(other);
        res->replace(m_indices[1], m_indices[0]);
        LOG("Res :", res)
        return res;
    }
    errorPrint();
    throw MathError("Invalid property in metric index. The hasProperty()",
                    " method returned true but no property is found");
}

void MetricIndex::print(std::ostream &out) const
{
    out << "(" << m_indices[0] << ' ' << m_indices[1] << ")";
}

GExpr metricindex_s(csl::Index const &a, csl::Index const &b)
{
    if (a == b)
        return cslexpr_s(csl::DMinko);
    if (a.getType() == cslIndex::Fixed && b.getType() == cslIndex::Fixed)
        return cslexpr_s(a.getSpace()->getMetric()->getTensor()->getArgument(
            {a.getValue(), b.getValue()}));
    return std::make_shared<MetricIndex>(a, b);
}
} // namespace sgl
