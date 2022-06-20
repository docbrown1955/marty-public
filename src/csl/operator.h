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

/*! \file operator.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Generic template to handle operator expression, i.e.
 * expression that applies on expressions on the right.
 */
#ifndef OPERATOR_H_INCLUDED
#define OPERATOR_H_INCLUDED

#include "abstract.h"
#include "commutation.h"
#include "interface.h"
#include "literal.h"
#include "numerical.h"
#include "utils.h"

namespace csl {

/*!
 *  \brief Linear operator O(a*X+b*Y) = a*O(X) + b*O(Y)
 */
template <typename T>
class Operator : public T {

  protected:
    bool empty;

  public:
    Operator() : T(), empty(true)
    {
    }

    template <typename... Args>
    explicit Operator(Args &&... args)
        : T(std::forward<Args>(args)...), empty(true)
    {
    }

    ~Operator(){};

    bool getCommutable() const override;

    bool commutesWith(Expr_info other, int sign = -1) const override;

    bool isAnOperator() const override;

    bool isEmpty() const override;

    void setEmpty(bool t_empty) override;

    void setOperandPrivate(const Expr &operand, bool leaveEmpty) override;

    Expr applyOperator(const Expr &expr,
                       bool        leaveEmpty = false) const override;

    std::optional<Expr> expand(bool full    = false,
                               bool inplace = false) const override;

    std::optional<Expr> expand_if(std::function<bool(Expr const &)> const &f,
                                  bool full    = false,
                                  bool inplace = false) const override;
};

#endif

#ifndef TEMPLATE_OPERATOR_DEFINED
#define TEMPLATE_OPERATOR_DEFINED

template <typename T>
bool Operator<T>::getCommutable() const
{
    return T::getCommutable()
           and (not empty and this->getOperand()->getCommutable());
}

template <typename T>
bool Operator<T>::commutesWith(Expr_info other, int sign) const
{
    return T::commutesWith(other, sign)
           and (sign == -1
                and (empty
                     or Commutation(this->getOperand().get(), other) == CSL_0)
                and (not empty or not this->operatorAppliesOn(other)));
}

template <typename T>
bool Operator<T>::isAnOperator() const
{
    return true;
}

template <typename T>
bool Operator<T>::isEmpty() const
{
    return empty;
}

template <typename T>
void Operator<T>::setEmpty(bool t_empty)
{
    empty = t_empty;
}

template <typename T>
void Operator<T>::setOperandPrivate(const Expr &expr, bool leaveEmpty)
{
    this->setOperand(expr);
    empty = empty and leaveEmpty;
}

bool pullLeft(csl::vector_expr &argument, size_t pos, size_t &begin);
bool pullRight(csl::vector_expr &argument, size_t &pos, size_t &end);
void getParts(const csl::vector_expr &argument,
              size_t                  begin,
              size_t                  end,
              Expr &                  left,
              Expr &                  mid,
              Expr &                  right);

template <typename T>
Expr Operator<T>::applyOperator(const Expr &expr, bool leaveEmpty) const
{
    if (expr->getPrimaryType() == csl::PrimaryType::Numerical and not empty)
        return CSL_0;

    csl::vector_expr foo(0);
    Expr             foo2;
    Expr             res;
    size_t           posDerivative;
    size_t           endDerivative;
    switch (expr->getType()) {
    case csl::Type::Sum:
        foo  = expr->getVectorArgument();
        foo2 = Copy(this);
        for (auto iter = foo.begin(); iter != foo.end(); ++iter) {
            *iter = foo2->applyOperator(*iter, leaveEmpty);
        }

        return sum_s(foo);
        break;

    case csl::Type::Prod:
        if (this->getOperand() != CSL_1 and this->getOperand() != CSL_UNDEF) {
            if (this->getOperand()->getType() == csl::Type::Prod)
                foo = this->getOperand()->getVectorArgument();
            else
                foo.push_back(this->getOperand());
        }
        foo.insert(foo.end(), expr->begin(), expr->end());
        posDerivative = 0;
        endDerivative = foo.size();
        for (size_t i = 0; i != endDerivative; ++i) {
            if (not this->operatorAppliesOn(foo[i].get())) {
                if (not pullLeft(foo, i, posDerivative))
                    pullRight(foo, i, endDerivative);
            }
        }
        if (posDerivative != 0 or endDerivative != foo.size()) {
            Expr left, mid, right;
            getParts(foo, posDerivative, endDerivative, left, mid, right);
            if (not leaveEmpty
                and mid->getPrimaryType() == csl::PrimaryType::Numerical)
                return CSL_0;
            res = Copy(this);
            res->setOperandPrivate(mid, leaveEmpty);
            if (*left != CSL_1 or *right != CSL_1)
                return prod_s(left, prod_s(Refreshed(res), right));
            else
                return Refreshed(res);
        }
        res = Copy(this);
        res->setOperandPrivate(prod_s(foo), leaveEmpty);
        return res;
        break;

    default:
        if (this->operatorAppliesOn(expr.get())) {
            res = Copy(this);
            res->setOperandPrivate(res->getOperand() * expr, leaveEmpty);
            return res;
        }
        else {
            if (empty and not leaveEmpty)
                return CSL_0;
            res = Copy(this);
            if (*Commutation(res->getOperand(), expr) == CSL_0)
                return prod_s(expr, res);
            else
                return prod_s(res, expr);
        }
    }
}

template <class T>
std::optional<Expr> Operator<T>::expand(bool full, bool inplace) const
{
    if (full) {
        std::optional<Expr> op = this->getOperand()->expand(full, inplace);
        if (not op)
            return std::nullopt;
        Expr copyExpr = this->copy();
        copyExpr->setOperand(CSL_1);
        copyExpr->setEmpty(true);

        return copyExpr->applyOperator(op.value());
    }

    return T::expand(full, inplace);
}

template <class T>
std::optional<Expr> Operator<T>::expand_if(
    std::function<bool(Expr const &)> const &f, bool full, bool inplace) const
{
    if (full) {
        std::optional<Expr> op
            = this->getOperand()->expand_if(f, full, inplace);
        if (not op)
            return std::nullopt;
        Expr copyExpr = this->copy();
        copyExpr->setOperand(CSL_1);
        copyExpr->setEmpty(true);

        return copyExpr->applyOperator(op.value());
    }

    return T::expand_if(f, full, inplace);
}

} // End of namespace csl

#endif
