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
 * @file multifunction.h
 * @brief Base class for expressions with arguments in SGL.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include "abstractgammasym.h"

namespace sgl {

    class AbstractMultiFunction: public AbstractGammaSym {

    public:

        AbstractMultiFunction() = default;
        AbstractMultiFunction(GExpr const &arg);
        AbstractMultiFunction(std::vector<GExpr> const &arg);

        bool contains(csl::Index const &index) const override;

        size_t size() const override;
        GExpr const &argument(size_t i) const override;
        GExpr &argument(size_t i) override;
        iterator begin() override;
        const_iterator begin() const override;
        iterator end() override;
        const_iterator end() const override;

        void printArgs(std::ostream &out, char sep) const;

    protected:

        std::vector<GExpr> m_argument;
    };

    class AbstractLiteral: public AbstractGammaSym {

    public:

        size_t size() const override;
        GExpr const &argument(size_t i) const override;
        GExpr &argument(size_t i) override;
        iterator begin() override;
        const_iterator begin() const override;
        iterator end() override;
        const_iterator end() const override;
    };

} // namesapce sgl
