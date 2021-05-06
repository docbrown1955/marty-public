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

 #include "cslexpr.h"

namespace sgl {

    bool CSLExpr::isZero() const
    {
        return m_expr == CSL_0;
    }

    csl::Expr CSLExpr::getFactor() const
    {
        return expr();
    }

    GExpr CSLExpr::getTerm() const
    {
        return cslexpr_s(CSL_1);
    }

    GExpr CSLExpr::copy() const 
    {
        return std::make_shared<CSLExpr>(m_expr);
    }
    GExpr CSLExpr::refresh() const 
    {
        return cslexpr_s(csl::DeepRefreshed(m_expr));
    }

    void CSLExpr::print(std::ostream &out) const
    {
        out << m_expr;
    }
}
