#pragma once

#include "multifunction.h"

namespace sgl {

    class CSLExpr: public AbstractLiteral {

    public:

        CSLExpr(csl::Expr const &expr)
            :m_expr(expr)
        {}

        bool isZero() const override;

        csl::Expr const &expr() const override { 
            return m_expr;
        }
        csl::Expr &expr() override { 
            return m_expr;
        }

        bool contains(csl::Index const &) const override {
            return false;
        }
        void replace(csl::Index const&, csl::Index const&) override {}

        csl::Expr toCSL(TensorSet const&) const override { 
            return csl::DeepRefreshed(expr());
        }

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

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
}
