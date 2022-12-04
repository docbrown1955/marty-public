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

#include "generalizedindex.h"
#include "exceptions.h"

namespace sgl {

AbstractGeneralizedIndex::AbstractGeneralizedIndex(csl::Index const &t_index)
    : m_indices({t_index})
{
}

AbstractGeneralizedIndex::AbstractGeneralizedIndex(
    std::vector<csl::Index> const &t_indices)
    : m_indices(t_indices)
{
}

bool AbstractGeneralizedIndex::hasSelfContraction() const
{
    for (size_t i = 0; i != m_indices.size(); ++i)
        for (size_t j = i + 1; j < m_indices.size(); ++j)
            if (m_indices[i] == m_indices[j])
                return true;
    return false;
}

void AbstractGeneralizedIndex::printIndices(std::ostream &out, char sep) const
{
    for (size_t i = 0; i != m_indices.size(); ++i) {
        out << m_indices[i];
        if (i < m_indices.size() - 1)
            out << ' ' << sep << ' ';
    }
}

bool AbstractGeneralizedIndex::contains(csl::Index const &index) const
{
    return std::find(m_indices.begin(), m_indices.end(), index)
           != m_indices.end();
}

void AbstractGeneralizedIndex::replace(csl::Index const &i,
                                       csl::Index const &j)
{
    if (self().use_count() > 2) {
        throw IndexError("Invalid index replacement in a shared expression.");
    }
    if (i != j)
        std::replace(m_indices.begin(), m_indices.end(), i, j);
}
} // namespace sgl
