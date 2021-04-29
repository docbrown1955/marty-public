#pragma once

#include "csl.h"
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

namespace sgl {

    class GExpr;
    struct TensorSet;

    class AbstractGammaSym
        :public std::enable_shared_from_this<AbstractGammaSym> 
    {

    public:

        using iterator       = std::vector<GExpr>::iterator;
        using const_iterator = std::vector<GExpr>::const_iterator;

        virtual ~AbstractGammaSym() {};

        virtual size_t size() const = 0;
        virtual GExpr const &argument(size_t i) const = 0;
        virtual GExpr &argument(size_t i) = 0;
        virtual iterator begin() = 0;
        virtual const_iterator begin() const = 0;
        virtual iterator end() = 0;
        virtual const_iterator end() const = 0;

        virtual bool isZero() const = 0;

        virtual bool hasPropertyWith(GExpr const &other) const;
        virtual GExpr propertyWith(GExpr const &other) const;

        virtual csl::Expr getFactor() const;
        virtual GExpr getTerm() const;

        GExpr self();
        virtual GExpr copy() const = 0;
        virtual GExpr refresh() const = 0;

        virtual GExpr simplify();

        virtual std::vector<csl::Index> const &indices() const;
        virtual std::vector<csl::Index>       &indices()      ;

        virtual bool contains(csl::Index const &) const = 0;
        virtual void replace(csl::Index const &, csl::Index const &) = 0;

        virtual csl::Expr const &expr() const;
        virtual csl::Expr       &expr()      ;

        virtual csl::Expr toCSL(TensorSet const &tensors) const = 0;

        virtual void print(std::ostream &out = std::cout) const = 0;
        void errorPrint() const {
            print(std::cerr);
        }
    };

    class GExpr: public std::shared_ptr<AbstractGammaSym> {

    public:

        using iterator       = AbstractGammaSym::iterator;
        using const_iterator = AbstractGammaSym::const_iterator;

        using std::shared_ptr<AbstractGammaSym>::shared_ptr;

        GExpr(csl::Expr const &expr);
        GExpr(std::shared_ptr<AbstractGammaSym> const &other)
            :std::shared_ptr<AbstractGammaSym>(other)
        {}

        size_t size() const { return (**this).size(); }
        GExpr       &operator[](size_t i)       { return (**this).argument(i); } 
        GExpr const &operator[](size_t i) const { return (**this).argument(i); }
        iterator       begin()       { return (**this).begin(); }
        const_iterator begin() const { return (**this).begin(); }
        iterator       end()       { return (**this).end(); }
        const_iterator end() const { return (**this).end(); }
    };

    GExpr operator-(GExpr const &A);
    GExpr operator+(GExpr const &A, GExpr const &B);
    GExpr operator*(GExpr const &A, GExpr const &B);
    GExpr operator-(GExpr const &A, GExpr const &B);
    GExpr operator/(GExpr const &A, GExpr const &B);

    GExpr &operator+=(GExpr &A, GExpr const &B);
    GExpr &operator*=(GExpr &A, GExpr const &B);
    GExpr &operator-=(GExpr &A, GExpr const &B);
    GExpr &operator/=(GExpr &A, GExpr const &B);

    GExpr operator+(csl::Expr const &A, GExpr const &B);
    GExpr operator*(csl::Expr const &A, GExpr const &B);
    GExpr operator-(csl::Expr const &A, GExpr const &B);
    GExpr operator/(csl::Expr const &A, GExpr const &B);
    GExpr operator+(GExpr const &A, csl::Expr const &B);
    GExpr operator*(GExpr const &A, csl::Expr const &B);
    GExpr operator-(GExpr const &A, csl::Expr const &B);
    GExpr operator/(GExpr const &A, csl::Expr const &B);

    GExpr &operator+=(GExpr &A, csl::Expr const &B);
    GExpr &operator*=(GExpr &A, csl::Expr const &B);
    GExpr &operator-=(GExpr &A, csl::Expr const &B);
    GExpr &operator/=(GExpr &A, csl::Expr const &B);


    std::ostream &operator<<(
            std::ostream &out,
            GExpr const &expr
            );


} // namespace sgl
