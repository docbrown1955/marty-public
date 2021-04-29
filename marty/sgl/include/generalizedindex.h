#pragma once

#include "multifunction.h"

namespace sgl {

    class AbstractGeneralizedIndex: public AbstractLiteral {

    public:

        AbstractGeneralizedIndex() = default;
        AbstractGeneralizedIndex(csl::Index const &t_index);
        AbstractGeneralizedIndex(std::vector<csl::Index> const &t_indices);

        bool hasSelfContraction() const;
        void printIndices(std::ostream &out, char sep) const;

        std::vector<csl::Index> const &indices() const override {
            return m_indices; 
        }
        std::vector<csl::Index> &indices() override {
            return m_indices; 
        }

        bool contains(csl::Index const &index) const override;
        void replace(csl::Index const &i, csl::Index const &j) override;

    protected:

        std::vector<csl::Index> m_indices;
    };
}
