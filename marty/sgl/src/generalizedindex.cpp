#include "generalizedindex.h"
#include "exceptions.h"

namespace sgl {

    AbstractGeneralizedIndex::AbstractGeneralizedIndex(
            csl::Index const &t_index
            )
        :m_indices({t_index})
    {}

    AbstractGeneralizedIndex::AbstractGeneralizedIndex(
            std::vector<csl::Index> const &t_indices
            )
        :m_indices(t_indices)
    {}

    bool AbstractGeneralizedIndex::hasSelfContraction() const
    {
        for (size_t i = 0; i != m_indices.size(); ++i) 
            for (size_t j = i+1; j < m_indices.size(); ++j) 
                if (m_indices[i] == m_indices[j])
                    return true;
        return false;
    }

    void AbstractGeneralizedIndex::printIndices(
            std::ostream &out,
            char          sep
            ) const
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

    void AbstractGeneralizedIndex::replace(
            csl::Index const &i, 
            csl::Index const &j
            )
    {
        if (self().use_count() > 2) {
            throw Exception::IndexError;
        }
        if (i != j)
            std::replace(m_indices.begin(), m_indices.end(), i, j);
    }
}
