#include "abstractgammasym.h"
#include "operations.h"
#include "exceptions.h"
#include "typecast.h"
#include "cslexpr.h"

namespace sgl {

    csl::Expr AbstractGammaSym::getFactor() const
    {
        return CSL_1;
    }

    GExpr AbstractGammaSym::getTerm() const
    {
        return copy();
    }

    GExpr AbstractGammaSym::self() 
    {
        return shared_from_this();
    }

    bool AbstractGammaSym::hasPropertyWith(GExpr const &) const
    {
        return false;
    }

    GExpr AbstractGammaSym::propertyWith(GExpr const &) const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }

    GExpr AbstractGammaSym::simplify() 
    {
        return self();
    }

    std::vector<csl::Index> const &AbstractGammaSym::indices() const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    std::vector<csl::Index>       &AbstractGammaSym::indices()
    {
        errorPrint();
        throw Exception::AbstractCall;
    }

    csl::Expr const &AbstractGammaSym::expr() const
    {
        errorPrint();
        throw Exception::AbstractCall;
    }
    csl::Expr       &AbstractGammaSym::expr()
    {
        errorPrint();
        throw Exception::AbstractCall;
    }

    GExpr::GExpr(csl::Expr const &expr)
        :GExpr(cslexpr_s(expr))
    {

    }

    GExpr operator-(GExpr const &A)
    {
        return sgl::prod_s({cslexpr_s(CSL_M_1), A});
    }
    GExpr operator+(GExpr const &A, GExpr const &B)
    {
        GExpr C = A;
        return C += B;
    }
    GExpr operator*(GExpr const &A, GExpr const &B)
    {
        GExpr C = A;
        return C *= B;
    }
    GExpr operator-(GExpr const &A, GExpr const &B)
    {
        GExpr C = A;
        return C -= B;
    }
    GExpr operator/(GExpr const &A, GExpr const &B)
    {
        GExpr C = A;
        return C /= B;
    }

    GExpr &operator+=(GExpr &A, GExpr const &B)
    {
        A = sgl::sum_s({A, B});
        return A;
    }

    GExpr &operator*=(GExpr &A, GExpr const &B)
    {
        A = sgl::prod_s({A, B});
        return A;
    }

    GExpr &operator-=(GExpr &A, GExpr const &B)
    {
        A = sgl::sum_s({A, sgl::prod_s({cslexpr_s(CSL_M_1), B})});
        return A;
    }

    GExpr &operator/=(GExpr &A, GExpr const &B)
    {
        if (!IsType<CSLExpr>(B)) {
            A->errorPrint();
            B->errorPrint();
            throw Exception::MathError;
        }
        A = sgl::prod_s({A, cslexpr_s(1 / B->expr())});
        return A;
    }

    GExpr operator+(csl::Expr const &A, GExpr const &B)
    {
        return cslexpr_s(A)+B;
    }
    GExpr operator*(csl::Expr const &A, GExpr const &B)
    {
        return cslexpr_s(A)*B;
    }
    GExpr operator-(csl::Expr const &A, GExpr const &B)
    {
        return cslexpr_s(A)-B;
    }
    GExpr operator/(csl::Expr const &A, GExpr const &B)
    {
        return cslexpr_s(A)/B;
    }
    GExpr operator+(GExpr const &A, csl::Expr const &B)
    {
        return A+cslexpr_s(B);
    }
    GExpr operator*(GExpr const &A, csl::Expr const &B)
    {
        return A*cslexpr_s(B);
    }
    GExpr operator-(GExpr const &A, csl::Expr const &B)
    {
        return A-cslexpr_s(B);
    }
    GExpr operator/(GExpr const &A, csl::Expr const &B)
    {
        return A/cslexpr_s(B);
    }

    GExpr &operator+=(GExpr &A, csl::Expr const &B)
    {
        return A+=cslexpr_s(B);
    }
    GExpr &operator*=(GExpr &A, csl::Expr const &B)
    {
        return A*=cslexpr_s(B);
    }
    GExpr &operator-=(GExpr &A, csl::Expr const &B)
    {
        return A-=cslexpr_s(B);
    }
    GExpr &operator/=(GExpr &A, csl::Expr const &B)
    {
        return A/=cslexpr_s(B);
    }

    std::ostream &operator<<(
            std::ostream &out,
            GExpr const &expr
            )
    {
        expr->print(out);
        return out;
    }
}
