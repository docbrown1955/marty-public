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

#include "gammaindex.h"
#include "cslexpr.h"
#include "exceptions.h"
#include "tensorset.h"

namespace sgl {

GammaIndex::GammaIndex()
{
}

GammaIndex::GammaIndex(csl::Index const &t_index)
    : AbstractGeneralizedIndex(t_index)
{
}

GammaIndex::GammaIndex(std::vector<csl::Index> const &t_indices)
    : AbstractGeneralizedIndex(t_indices)
{
}

bool GammaIndex::isZero() const
{
    return hasSelfContraction();
}

csl::Expr GammaIndex::getFactor() const
{
    if (m_indices.size() < 2)
        return CSL_1;
    if (isZero())
        return CSL_0;
    csl::Expr               sign    = CSL_1;
    std::vector<csl::Index> indices = m_indices;
    for (size_t i = 0; i != indices.size(); ++i) {
        size_t min = i;
        for (size_t j = i + 1; j != indices.size(); ++j) {
            if (indices[j] < indices[min])
                min = j;
        }
        if (min != i) {
            std::swap(indices[i], indices[min]);
            sign *= -1;
        }
    }
    return sign;
}

GExpr GammaIndex::getTerm() const
{
    if (m_indices.size() < 2)
        return copy();
    if (isZero())
        return cslexpr_s(CSL_0);
    std::vector<csl::Index> indices = m_indices;
    for (size_t i = 0; i != indices.size(); ++i) {
        size_t min = i;
        for (size_t j = i + 1; j != indices.size(); ++j) {
            if (indices[j] < indices[min])
                min = j;
        }
        if (min != i) {
            std::swap(indices[i], indices[min]);
        }
    }
    return gammaindex_s(indices);
}

bool GammaIndex::leftOf(GammaIndex const &other) const
{
    if ((!isGammaMu() && !isSigma())
        || (!other.isGammaMu() && !other.isSigma()))
        return false;
    const size_t szA = m_indices.size();
    const size_t szB = other.m_indices.size();
    if (szA > szB)
        return true;
    else if (szB > szA)
        return false;
    for (const auto &i : m_indices) {
        if (i.getType() == cslIndex::Fixed)
            return false;
    }
    for (const auto &j : other.m_indices) {
        if (j.getType() == cslIndex::Fixed)
            return true;
    }
    if (szA == 1) {
        return m_indices[0].getName() < other.m_indices[0].getName();
    }
    else {
        for (const auto i : m_indices) {
            bool simpler = false;
            for (const auto &j : other.m_indices)
                if (!(i.getName() < j.getName())) {
                    simpler = true;
                    break;
                }
            if (!simpler)
                return true;
        }
    }
    return false;
}

GExpr GammaIndex::copy() const
{
    return std::make_shared<GammaIndex>(m_indices);
}

GExpr GammaIndex::refresh() const
{
    return gammaindex_s(m_indices);
}

csl::Expr GammaIndex::buildTensor(TensorSet const & tensors,
                                  csl::Index const &a,
                                  csl::Index const &b) const
{
    if (m_indices.size() == 1 && m_indices[0].getType() == cslIndex::Fixed) {
        if (m_indices[0] == 5)
            return csl::Tensor(tensors.gamma_chir)({a, b});
        if (m_indices[0] == 6)
            return csl::Tensor(tensors.P_L)({a, b});
        if (m_indices[0] == 7)
            return csl::Tensor(tensors.P_R)({a, b});
        if (m_indices[0] == 8)
            return csl::Tensor(tensors.C)({a, b});
    }
    auto pos = tensors.gamma.find(m_indices.size());
    if (pos == tensors.gamma.end()) {
        std::cerr << "Index of size " << m_indices.size() << " not found !"
                  << std::endl;
        throw Exception::MathError;
    }
    csl::Tensor             gamma = pos->second;
    std::vector<csl::Index> indices(m_indices);
    csl::Expr               factor = (indices.size() == 2) ? -CSL_I : CSL_1;
    indices.push_back(a);
    indices.push_back(b);
    return factor * gamma(indices);
}

csl::Expr GammaIndex::toCSL(TensorSet const &) const
{
    throw Exception::MathError;
}

void GammaIndex::print(std::ostream &out) const
{
    out << "[";
    printIndices(out, ',');
    out << "]";
}

bool GammaIndex::equalTo(GammaIndex const &other) const
{
    if (m_indices.size() != other.indices().size())
        return false;
    auto iter = other.indices().begin();
    return !std::any_of(m_indices.begin(),
                        m_indices.end(),
                        [&](csl::Index const &i) { return i != *iter++; });
}

GExpr gammaindex_s()
{
    return std::make_shared<GammaIndex>();
}
GExpr gammaindex_s(csl::Index const &index)
{
    return std::make_shared<GammaIndex>(index);
}
GExpr gammaindex_s(std::vector<csl::Index> const &indices)
{
    auto gamma = std::make_shared<GammaIndex>(indices);
    return (gamma->isZero()) ? GExpr(CSL_0) : GExpr(gamma);
}
} // namespace sgl
