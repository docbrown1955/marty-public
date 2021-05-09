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
#ifndef PROPERTY_H_INCLUDED
#define PROPERTY_H_INCLUDED

#include "equation.h"

namespace csl {

class PropertyList{

    protected:

    std::vector<Equation*> props;

    public:

    PropertyList();
    PropertyList(const PropertyList&) = delete;
    PropertyList(const PropertyList&&) = delete;
    PropertyList& operator=(const PropertyList&) = delete;
    PropertyList& operator=(const PropertyList&&) = delete;
    ~PropertyList();

    void addProperty(const Expr& leftHandSide);
    void addProperty(const Expr& leftHandSide, const Expr& rightHandSide);
    void removeProperty(Equation* prop);
    void removeProperty(const Expr& leftHandSide);
    void removeProperty(const Expr& leftHandSide, const Expr& rightHandSide);

    csl::vector_expr apply(const Expr& expr) const;

    friend std::ostream& operator<<(std::ostream& fout, const PropertyList& prop);
};

static PropertyList PROPERTIES;
} // End of namespace csl

#endif
