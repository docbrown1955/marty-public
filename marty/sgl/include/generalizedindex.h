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
