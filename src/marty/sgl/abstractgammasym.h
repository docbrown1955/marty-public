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
 * @file abstractgammasym.h
 * @brief Contains the base class for the Symbolic Gamma Library (SGL) and the
 * GExpr definition.
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2021-05-06
 */
#pragma once

#include "csl.h"
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace sgl {

class GExpr;
struct TensorSet;

class AbstractGammaSym
    : public std::enable_shared_from_this<AbstractGammaSym> {

  public:
    using iterator       = std::vector<GExpr>::iterator;
    using const_iterator = std::vector<GExpr>::const_iterator;

    virtual ~AbstractGammaSym(){};

    virtual size_t         size() const             = 0;
    virtual GExpr const &  argument(size_t i) const = 0;
    virtual GExpr &        argument(size_t i)       = 0;
    virtual iterator       begin()                  = 0;
    virtual const_iterator begin() const            = 0;
    virtual iterator       end()                    = 0;
    virtual const_iterator end() const              = 0;

    virtual bool isZero() const = 0;

    virtual bool  hasPropertyWith(GExpr const &other) const;
    virtual GExpr propertyWith(GExpr const &other) const;

    virtual csl::Expr getFactor() const;
    virtual GExpr     getTerm() const;

    GExpr         self();
    virtual GExpr copy() const    = 0;
    virtual GExpr refresh() const = 0;

    virtual GExpr simplify();

    virtual std::vector<csl::Index> const &indices() const;
    virtual std::vector<csl::Index> &      indices();

    virtual bool contains(csl::Index const &) const              = 0;
    virtual void replace(csl::Index const &, csl::Index const &) = 0;

    virtual csl::Expr const &expr() const;
    virtual csl::Expr &      expr();

    virtual csl::Expr toCSL(TensorSet const &tensors) const = 0;

    virtual void print(std::ostream &out = std::cout) const = 0;
    void         errorPrint() const
    {
        print(std::cerr);
    }
};

class GExpr : public std::shared_ptr<AbstractGammaSym> {

  public:
    using iterator       = AbstractGammaSym::iterator;
    using const_iterator = AbstractGammaSym::const_iterator;

    using std::shared_ptr<AbstractGammaSym>::shared_ptr;

    GExpr(csl::Expr const &expr);
    GExpr(std::shared_ptr<AbstractGammaSym> const &other)
        : std::shared_ptr<AbstractGammaSym>(other)
    {
    }

    size_t size() const
    {
        return (**this).size();
    }
    GExpr &operator[](size_t i)
    {
        return (**this).argument(i);
    }
    GExpr const &operator[](size_t i) const
    {
        return (**this).argument(i);
    }
    iterator begin()
    {
        return (**this).begin();
    }
    const_iterator begin() const
    {
        return (**this).begin();
    }
    iterator end()
    {
        return (**this).end();
    }
    const_iterator end() const
    {
        return (**this).end();
    }
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

std::ostream &operator<<(std::ostream &out, GExpr const &expr);

} // namespace sgl
