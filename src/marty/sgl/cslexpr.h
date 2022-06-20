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
 * @file cslexpr.h
 * @brief SGL object representing a CSL Expression, sgl::CSLExpr.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "multifunction.h"

namespace sgl {

class CSLExpr : public AbstractLiteral {

  public:
    CSLExpr(csl::Expr const &expr) : m_expr(expr)
    {
    }

    bool isZero() const override;

    csl::Expr const &expr() const override
    {
        return m_expr;
    }
    csl::Expr &expr() override
    {
        return m_expr;
    }

    bool contains(csl::Index const &) const override
    {
        return false;
    }
    void replace(csl::Index const &, csl::Index const &) override
    {
    }

    csl::Expr toCSL(TensorSet const &) const override
    {
        return csl::DeepRefreshed(expr());
    }

    csl::Expr getFactor() const override;
    GExpr     getTerm() const override;

    GExpr copy() const override;
    GExpr refresh() const override;

    void print(std::ostream &out) const override;

  private:
    csl::Expr m_expr;
};

inline GExpr cslexpr_s(csl::Expr const &expr)
{
    return std::make_shared<CSLExpr>(expr);
}
} // namespace sgl
