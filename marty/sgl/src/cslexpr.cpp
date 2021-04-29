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
