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
#ifndef SCALAR_DOT_H_INCLUDED
#define SCALAR_DOT_H_INCLUDED

#include <csl.h>

namespace mty {

class ScalarDot: public csl::AbstractDuoFunc {

    public:

    ScalarDot() = default;

    ScalarDot(csl::Expr const& A,
              csl::Expr const& B);

    void print(int mode = 0,
               std::ostream& out = std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode = 0) const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr refresh() const override;

    csl::Expr deepCopy() const override;

    csl::Expr deepRefresh() const override;

    std::optional<csl::Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base) const override;

    bool isIndexed() const override;

    csl::IndexStructure getIndexStructure() const override;

    bool operator==(csl::Expr_info other) const override;
};

inline csl::allocator<ScalarDot> alloc_scalardot;

csl::Expr scalardot_s(csl::Expr const& impulsion,
                 csl::Expr const& mass);

}

#endif
