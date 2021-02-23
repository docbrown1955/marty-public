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
 * @version 1.0
 
 * \brief
 */
#ifndef SCALAR_H
#define SCALAR_H

#include "scalarFunc.h"

namespace csl {

class Scalar: public AbstractDuoFunc
{
    public:

    Scalar(Expr  const&  left,
           Expr  const&  right);

    ~Scalar();

    csl::Type getType() const override;

    void print(int mode = 0,
               std::ostream& out = std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode = 0) const override;
    
    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool isIndexed() const override;

    IndexStructure getIndexStructure() const override;

    bool compareWithDummy(
            Expr_info               expr,
            std::map<Index, Index>& constraints,
            bool                    keepAllCosntraints = false) const override;

    bool operator==(Expr_info other) const override;

    bool operator<(Expr_info other) const override;
};

Expr scalar_(Expr  const& left,
             Expr  const& right);

}

#endif /* SCALAR_H */
