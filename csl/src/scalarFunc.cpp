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

#include "scalarFunc.h"
#include "indicial.h"
#include "comparison.h"
#include "interface.h"
#include "numericalEval.h"
#include "cslcomplex.h"
#include "replace.h"
#include "mathFunctions.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractFunc                            //
/*************************************************/
///////////////////////////////////////////////////

size_t AbstractFunc::size() const
{
    return 1;
}

bool AbstractFunc::getCommutable() const {
    return (argument->getCommutable());
}

std::vector<csl::Parent> AbstractFunc::getSubSymbols() const
{
    return argument->getSubSymbols();
}

bool AbstractFunc::isIndexed() const {
    return argument->isIndexed();
}

bool AbstractFunc::compareWithDummy(Expr_info          expr,
                                    map<Index, Index>& constraints,
                                    bool keepAllCosntraints) const
{
    if (expr->getType() != getType())
        return false;
    if (not isIndexed())
        return *this == expr;
    return argument->compareWithDummy(expr->getArgument().get(),
                                      constraints,
                                      keepAllCosntraints);
}


IndexStructure AbstractFunc::getIndexStructure() const
{
    return IndexStructure(); //argument->getIndexStructure();
}

Tensor AbstractFunc::getPoint() const
{
    return argument->getPoint();
}

int AbstractFunc::getNArgs(int) const {
    return 1;
}

Expr const &AbstractFunc::getArgument(int) const {
    return argument;
}

Expr& AbstractFunc::getArgument(int)
{
    return argument;
}

bool AbstractFunc::isReal() const
{
    return argument->isReal();
}

bool AbstractFunc::isPurelyImaginary() const
{
    return argument->isPurelyImaginary();
}

std::optional<Expr> AbstractFunc::getRealPart() const
{
    return real_s(copy());
}

Expr AbstractFunc::getImaginaryPart() const
{
    return imaginary_s(copy());
}

std::optional<Expr> AbstractFunc::getComplexModulus() const
{
    return sqrt_s(pow_s(GetRealPart(copy()), 2) 
                + pow_s(GetImaginaryPart(copy()), 2));
}

std::optional<Expr> AbstractFunc::getComplexArgument() const
{
    return angle_s(getImaginaryPart(), GetRealPart(copy()));
}


optional<Expr> AbstractFunc::getComplexConjugate() const
{
    optional<Expr> conjugate = argument->getComplexConjugate();
    if (conjugate) {
        Expr foo = copy();
        foo->setArgument(conjugate.value());
        return Refreshed(foo);
    }

    return nullopt;
}

optional<Expr> AbstractFunc::findSubExpression(
        Expr_info   subExpression,
        const Expr& newExpression) const
{
    if (*this == subExpression)
        return newExpression;
    optional<Expr> sub = argument->findSubExpression(subExpression,
                                                     newExpression);
    if (sub) {
        Expr res = Copy(this);
        res->setArgument(sub.value());
        return Refreshed(res);
    }

    return nullopt;
}

void AbstractFunc::setArgument(const Expr& t_argument, int) {
    argument = t_argument;
}

optional<Expr> AbstractFunc::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndex,
        bool         refresh) const
{
    optional<Expr> opt = argument->replaceIndex(
            indexToReplace,
            newIndex,
            refresh);
    if (opt) {
        Expr copyExpr = copy();
        copyExpr->setArgument(opt.value());
        if (refresh)
            return Refreshed(copyExpr);
        return copyExpr;
    }
    return nullopt;
}

std::optional<Expr> AbstractFunc::replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh,
            bool         flipped) const
{
    optional<Expr> opt = argument->replaceIndices(
            oldIndices,
            newIndices,
            refresh,
            flipped);
    if (opt) {
        Expr copyExpr = copy();
        copyExpr->setArgument(opt.value());
        if (refresh)
            return Refreshed(copyExpr);
        return copyExpr;
    }
    return nullopt;
}
optional<Expr> AbstractFunc::factor(bool full) const
{
    if (full) {
       optional<Expr> argFacto = argument->factor(full);
       if (argFacto) {
           Expr result = copy();
           result->setArgument(argFacto.value());
           return Refreshed(result);
       }
    }
    return nullopt;
}

optional<Expr> AbstractFunc::factor(Expr_info expr, bool full) const
{
    if (full) {
       optional<Expr> argFacto = argument->factor(expr, full);
       if (argFacto) {
           Expr result = copy();
           result->setArgument(argFacto.value());
           return Refreshed(result);
       }
    }
    return nullopt;
}

optional<Expr> AbstractFunc::collect(
        std::vector<Expr> const &factors,
        bool                     full
        ) const
{
    if (full) {
        optional<Expr> argCollect = argument->collect(factors, full);
        if (argCollect) {
            Expr res = copy();
            res[0] = *argCollect;
            return csl::Refreshed(res);
        }
    }
    return nullopt;
}

optional<Expr> AbstractFunc::expand(bool full,
                                    bool inPlace) const
{
    if (full) {
       optional<Expr> argExpand = argument->expand(full, inPlace);
       if (argExpand) {
           Expr result = copy();
           result->setArgument(argExpand.value());
           return Refreshed(result);
       }
    }
    return nullopt;
}

optional<Expr> AbstractFunc::expand_if(
        std::function<bool(Expr const&)> const& f,
        bool full,
        bool inPlace) const
{
    if (full) {
       optional<Expr> argExpand = argument->expand_if(f, full, inPlace);
       if (argExpand) {
           Expr result = copy();
           result->setArgument(argExpand.value());
           return Refreshed(result);
       }
    }
    return nullopt;
}

Expr AbstractFunc::evalNumerical(Expr const& arg) const
{
    Expr result = copy();
    if (arg->getType() == csl::Type::NumericalEval) {
       Expr foo_var = variable_s("foo");
       result->setArgument(foo_var);
       Expr f_prime = Derived(result, foo_var.get());
       Replace(f_prime,
               foo_var,
               autonumber_s(arg->evaluateScalar()));
       f_prime = Evaluated(f_prime);
       Expr delta_plus;
       Expr delta_minus;
       if (f_prime->evaluateScalar() < 0) {
           delta_plus = -f_prime*
               autonumber_s(arg->getDeltaMinus());
           delta_minus = f_prime*
               autonumber_s(arg->getDeltaPlus());
       }
       else {
           delta_minus = -f_prime*
               autonumber_s(arg->getDeltaMinus());
           delta_plus = f_prime*
               autonumber_s(arg->getDeltaPlus());
       }
       result->setArgument(
               autonumber_s(arg->evaluateScalar()));
       return numericaleval_s(result->evaluateScalar(),
                             delta_plus->evaluateScalar(),
                             delta_minus->evaluateScalar());
    }
    result->setArgument(arg);
    return autonumber_s(result->evaluateScalar());
}

bool AbstractFunc::dependsOn(Expr_info expr) const {
    return (*this == expr) or argument->dependsOn(expr);
}

bool AbstractFunc::dependsOn(const AbstractParent* parent) const {
    return argument->dependsOn(parent);
}

bool AbstractFunc::dependsExplicitlyOn(Expr_info expr) const {
    return (operator==(expr) or argument->dependsExplicitlyOn(expr));
}

bool AbstractFunc::dependsExplicitlyOn(Parent_info expr) const 
{
    return argument->dependsExplicitlyOn(expr);
}

bool AbstractFunc::commutesWith(Expr_info expr, int sign) const
{
    return argument->commutesWith(expr, sign);
}

int AbstractFunc::isPolynomial(Expr_info expr) const {
    return (*this==expr);
}

optional<Expr> AbstractFunc::evaluate(
        csl::eval::mode user_mode) const
{
    optional<Expr> evalArg = argument->evaluate(user_mode);
    if (evalArg) {
        if (evalArg.value()->getPrimaryType() 
                == csl::PrimaryType::Numerical
                and eval::isContained(user_mode, csl::eval::numerical)) {
            return evalNumerical(evalArg.value());
        }
        Expr result = Copy(this);
        result->setArgument(evalArg.value());
        return Refreshed(result);
    }
    else if (argument->getPrimaryType()
                == csl::PrimaryType::Numerical
            and eval::isContained(user_mode, csl::eval::numerical))
        return evalNumerical(argument);

    return nullopt;
}

Expr const &AbstractFunc::operator[](int i) const
{
    CSL_ASSERT_SPEC(i == 0,
               CSLError::IndexError,
               "Simple function has only one arg, index " 
               + toString(i) + " given.");
    return argument;
}

Expr& AbstractFunc::operator[](int i)
{
    CSL_ASSERT_SPEC(i == 0,
               CSLError::IndexError,
               "Simple function has only one arg, index " 
               + toString(i) + " given.");
    return argument;
}

bool AbstractFunc::operator==(Expr_info expr) const 
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }

    return (expr->getType() == this->getType() and 
            *argument == expr->getArgument().get());
} 

///////////////////////////////////////////////////
/*************************************************/
// AbstractDuoFunc                               //
/*************************************************/
///////////////////////////////////////////////////

int AbstractDuoFunc::getNArgs(int) const {
    return 2;
}

bool AbstractDuoFunc::getCommutable() const 
{
    return (argument[0]->getCommutable() and 
            argument[1]->getCommutable());
}

std::vector<csl::Parent> AbstractDuoFunc::getSubSymbols() const
{
    auto dep1 = argument[0]->getSubSymbols();
    auto dep2 = argument[1]->getSubSymbols();
    dep1.insert(dep1.end(),
            std::make_move_iterator(dep2.begin()),
            std::make_move_iterator(dep2.end()));
    return dep1;
}

bool AbstractDuoFunc::isIndexed() const {
    return (argument[0]->isIndexed() or argument[1]->isIndexed());
}

IndexStructure AbstractDuoFunc::getIndexStructure() const
{
    if (isIndexed())
        return (argument[0]->getIndexStructure() 
            + argument[1]->getIndexStructure());
    return IndexStructure();
}

Tensor AbstractDuoFunc::getPoint() const
{
    Tensor point = argument[0]->getPoint();
    return (point) ? point : argument[1]->getPoint();
}

Expr const &AbstractDuoFunc::getArgument(int iArg) const 
{
    return argument[iArg];
}

Expr& AbstractDuoFunc::getArgument(int iArg) 
{
    return argument[iArg];
}

optional<Expr> AbstractDuoFunc::findSubExpression(
        Expr_info   subExpression,
        const Expr& newExpression) const
{
    if (*this == subExpression)
        return newExpression;
    optional<Expr> subArg0 = argument[0]->
        findSubExpression(subExpression, newExpression);
    optional<Expr> subArg1 = argument[1]->
        findSubExpression(subExpression, newExpression);
    if (subArg0 or subArg1) {
        Expr res = Copy(this);
        res->setArgument(subArg0.value_or(argument[0]), 0);
        res->setArgument(subArg1.value_or(argument[1]), 1);
        return Refreshed(res);
    }

    return nullopt;
}

void AbstractDuoFunc::setArgument(const Expr& t_argument, int iArg) 
{
    argument[iArg] = t_argument;
}

bool AbstractDuoFunc::isReal() const
{
    return (argument[0]->isReal() and argument[1]->isReal());
}

bool AbstractDuoFunc::isPurelyImaginary() const
{
    return false;
}

std::optional<Expr> AbstractDuoFunc::getRealPart() const
{
    return real_s(copy());
}

Expr AbstractDuoFunc::getImaginaryPart() const
{
    return imaginary_s(copy());
}

std::optional<Expr> AbstractDuoFunc::getComplexModulus() const
{
    return sqrt_s(pow_s(GetRealPart(copy()), 2) 
                + pow_s(GetImaginaryPart(copy()), 2));
}

std::optional<Expr> AbstractDuoFunc::getComplexArgument() const
{
    return angle_s(getImaginaryPart(), GetRealPart(copy()));
}

optional<Expr> AbstractDuoFunc::getComplexConjugate() const
{
    optional<Expr> conjArg0 = argument[0]->getComplexConjugate();
    optional<Expr> conjArg1 = argument[1]->getComplexConjugate();
    if (conjArg0 or conjArg1) {
        Expr conjugate = Copy(this);
        conjugate->setArgument(conjArg0.value_or(argument[0]), 0);
        conjugate->setArgument(conjArg1.value_or(argument[1]), 1);
        return Refreshed(conjugate);
    }
    return nullopt;
}

optional<Expr> AbstractDuoFunc::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndex,
        bool         refresh) const
{
    optional<Expr> opt0 =
        argument[0]->replaceIndex(indexToReplace, newIndex, refresh);
    optional<Expr> opt1 =
        argument[1]->replaceIndex(indexToReplace, newIndex, refresh);
    if (opt0 or opt1) {
        Expr copyExpr = copy();
        copyExpr->setArgument(opt0.value_or(argument[0]), 0);
        copyExpr->setArgument(opt1.value_or(argument[1]), 1);
        if (refresh)
            return Refreshed(copyExpr);
        return copyExpr;
    }
    return nullopt;
}

std::optional<Expr> AbstractDuoFunc::replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh,
            bool         flipped) const
{
    optional<Expr> opt0 =
        argument[0]->replaceIndices(oldIndices, newIndices, refresh, flipped);
    optional<Expr> opt1 =
        argument[1]->replaceIndices(oldIndices, newIndices, refresh, flipped);
    if (opt0 or opt1) {
        Expr copyExpr = copy();
        copyExpr->setArgument(opt0.value_or(argument[0]), 0);
        copyExpr->setArgument(opt1.value_or(argument[1]), 1);
        if (refresh)
            return Refreshed(copyExpr);
        return copyExpr;
    }
    return nullopt;
}

optional<Expr> AbstractDuoFunc::factor(bool full)  const
{
    if (full) {
       optional<Expr> arg0Facto = argument[0]->factor(true);
       optional<Expr> arg1Facto = argument[1]->factor(true);
       if (arg0Facto or arg1Facto) {
           Expr res = Copy(this);
           res->setArgument(arg0Facto.value_or(argument[0]), 0);
           res->setArgument(arg1Facto.value_or(argument[1]), 1);
           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractDuoFunc::factor(Expr_info expr, bool full) const
{
    if (full) {
       optional<Expr> arg0Facto = argument[0]->factor(expr, true);
       optional<Expr> arg1Facto = argument[1]->factor(expr, true);
       if (arg0Facto or arg1Facto) {
           Expr res = copy();
           res->setArgument(arg0Facto.value_or(argument[0]), 0);
           res->setArgument(arg1Facto.value_or(argument[1]), 1);
           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractDuoFunc::collect(
        std::vector<Expr> const &factors,
        bool                     full
        ) const
{
    if (full) {
       optional<Expr> arg0Collect = argument[0]->collect(factors, true);
       optional<Expr> arg1Collect = argument[1]->collect(factors, true);
       if (arg0Collect or arg1Collect) {
           Expr res = copy();
           res[0] = arg0Collect.value_or(res[0]);
           res[1] = arg1Collect.value_or(res[1]);
           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractDuoFunc::expand(bool full, bool inPlace) const
{
    if (full) {
       optional<Expr> arg0Expand = argument[0]->expand(true, inPlace);
       optional<Expr> arg1Expand = argument[1]->expand(true, inPlace);
       if (arg0Expand or arg1Expand) {
           Expr res = copy();
           res->setArgument(arg0Expand.value_or(argument[0]), 0);
           res->setArgument(arg1Expand.value_or(argument[1]), 1);
           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractDuoFunc::expand_if(
        std::function<bool(Expr const&)> const& f,
        bool full,
        bool inPlace) const
{
    if (full) {
       optional<Expr> arg0Expand = argument[0]->expand_if(f, true, inPlace);
       optional<Expr> arg1Expand = argument[1]->expand_if(f, true, inPlace);
       if (arg0Expand or arg1Expand) {
           Expr res = copy();
           res->setArgument(arg0Expand.value_or(argument[0]), 0);
           res->setArgument(arg1Expand.value_or(argument[1]), 1);
           return Refreshed(res);
       }
    }

    return nullopt;
}

bool AbstractDuoFunc::dependsOn(Expr_info expr) const
{
    return (*this == expr or
            argument[0]->dependsOn(expr) or
            argument[1]->dependsOn(expr));
}

bool AbstractDuoFunc::dependsOn(const AbstractParent* parent) const {
    return argument[0]->dependsOn(parent)
        or argument[1]->dependsOn(parent);
}

bool AbstractDuoFunc::dependsExplicitlyOn(Expr_info expr) const
{
    return (operator==(expr) or
            argument[0]->dependsExplicitlyOn(expr) or
            argument[1]->dependsExplicitlyOn(expr));
}

bool AbstractDuoFunc::dependsExplicitlyOn(Parent_info expr) const
{
    return argument[0]->dependsExplicitlyOn(expr)
        or argument[1]->dependsExplicitlyOn(expr);
}

bool AbstractDuoFunc::commutesWith(Expr_info expr, int sign) const
{
    return argument[0]->commutesWith(expr, sign)
        and argument[1]->commutesWith(expr, sign);
}

int AbstractDuoFunc::isPolynomial(Expr_info expr) const {
    return (this->operator==(expr));
}

Expr const &AbstractDuoFunc::operator[](int i) const
{
    return argument[i];
}

Expr& AbstractDuoFunc::operator[](int i)
{
    return argument[i];
}

///////////////////////////////////////////////////
/*************************************************/
// AbstractMultiFunc                             //
/*************************************************/
///////////////////////////////////////////////////

int AbstractMultiFunc::getNArgs(int) const
{
    return static_cast<int>(argument.size());
}

bool AbstractMultiFunc::getCommutable() const
{
    for (const auto& arg: argument) {
        if (not arg->getCommutable())
            return false;
    }

    return true;
}

std::vector<Parent> AbstractMultiFunc::getSubSymbols() const
{
    std::vector<Parent> dep;
    for (const auto &arg : *this) {
        std::vector<Parent> interm = arg->getSubSymbols();
        dep.insert(dep.end(),
                std::make_move_iterator(interm.begin()),
                std::make_move_iterator(interm.end()));
    }
    return dep;
}

bool AbstractMultiFunc::isIndexed() const 
{
    for (const auto& arg : argument)
        if (arg->isIndexed())
            return true;

    return false;
}

const csl::vector_expr& AbstractMultiFunc::getVectorArgument() const {
    return argument;
}

Expr const &AbstractMultiFunc::getArgument(int iArg) const 
{
    return argument[iArg];
}

Expr& AbstractMultiFunc::getArgument(int iArg)
{
    return argument[iArg];
}

Tensor AbstractMultiFunc::getPoint() const
{
    for (const auto& arg : argument) {
        Tensor point = arg->getPoint();
        if (point)
            return point;
    }
    return nullptr;
}

optional<Expr> AbstractMultiFunc::findSubExpression(
        Expr_info   subExpression,
        const Expr& newExpression) const
{
    if (*this == subExpression)
        return newExpression;
    vector<optional<Expr>> argSub(argument.size());
    bool found = false;
    for (size_t i = 0; i != argument.size(); ++i) {
        argSub[i] = argument[i]->findSubExpression(
                    subExpression, newExpression);
        if (not found and argSub[i])
            found = true;
    }
    if (found) {
        Expr res = copy();
        for (size_t i = 0; i != argument.size(); ++i)
            res->setArgument(argSub[i].value_or(argument[i]), i);
        return Refreshed(res);
    }

    return nullopt;
}

size_t AbstractMultiFunc::size() const
{
    return argument.size();
}

bool AbstractMultiFunc::empty() const
{
    return argument.empty();
}

csl::vector_expr::iterator AbstractMultiFunc::begin()
{
    return argument.begin();
}

csl::vector_expr::iterator AbstractMultiFunc::end()
{
    return argument.end();
}

csl::vector_expr::const_iterator AbstractMultiFunc::begin() const
{
    return argument.begin();
}

csl::vector_expr::const_iterator AbstractMultiFunc::end() const
{
    return argument.end();
}

void AbstractMultiFunc::setArgument(const Expr& t_argument, int iArg) 
{
    argument[iArg] = t_argument;
}

void AbstractMultiFunc::setVectorArgument(const csl::vector_expr& t_argument) 
{
    argument = t_argument;
}

bool AbstractMultiFunc::isReal() const
{
    for (const auto& arg : argument)
        if (not arg->isReal())
            return false;
    return true;
}

bool AbstractMultiFunc::isPurelyImaginary() const
{
    return false;
}

std::optional<Expr> AbstractMultiFunc::getRealPart() const
{
    return real_s(copy());
}

Expr AbstractMultiFunc::getImaginaryPart() const
{
    return imaginary_s(copy());
}

std::optional<Expr> AbstractMultiFunc::getComplexModulus() const
{
    return sqrt_s(pow_s(GetRealPart(copy()), 2) 
                + pow_s(GetImaginaryPart(copy()), 2));
}

std::optional<Expr> AbstractMultiFunc::getComplexArgument() const
{
    return angle_s(getImaginaryPart(), GetRealPart(copy()));
}


optional<Expr> AbstractMultiFunc::getComplexConjugate() const
{
    vector<optional<Expr>> optConjug(argument.size());
    bool conjugFound = false;
    for (size_t i = 0; i != argument.size(); ++i) {
        optConjug[i] = argument[i]->getComplexConjugate();
        if (not conjugFound and optConjug[i])
            conjugFound = true;
    }
    if (conjugFound) {
        Expr conjugate = Copy(this);
        for (size_t i = 0; i != argument.size(); ++i)
            conjugate->setArgument(
                    optConjug[i].value_or(argument[i]), i);
        return Refreshed(conjugate);
    }

    return nullopt;
}

optional<Expr> AbstractMultiFunc::replaceIndex(
        const Index& indexToReplace,
        const Index& newIndex,
        bool         refresh) const
{

    csl::vector_expr newArg(argument.size());
    bool replaced = false;
    for (size_t i = 0; i != argument.size(); ++i)
        if (optional<Expr> opt 
                = argument[i]->replaceIndex(
                    indexToReplace,
                    newIndex,
                    refresh);
                opt) {
            newArg[i] = opt.value();
            replaced = true;
        }
        else
            newArg[i] = argument[i];

    if (replaced) {
        Expr res = copy();
        res->setVectorArgument(newArg);
        if (refresh)
            return Refreshed(res);
        return res;
    }

    return nullopt;
}

std::optional<Expr> AbstractMultiFunc::replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh,
            bool         flipped) const
{
    csl::vector_expr newArg(argument.size());
    bool replaced = false;
    for (size_t i = 0; i != argument.size(); ++i)
        if (optional<Expr> opt 
                = argument[i]->replaceIndices(
                    oldIndices,
                    newIndices,
                    refresh,
                    flipped);
                opt) {
            newArg[i] = opt.value();
            replaced = true;
        }
        else
            newArg[i] = argument[i];

    if (replaced) {
        Expr res = copy();
        res->setVectorArgument(newArg);
        if (refresh)
            return Refreshed(res);
        return res;
    }

    return nullopt;
}


optional<Expr> AbstractMultiFunc::factor(bool full) const
{
    if (full) {
       // Expr result = Copy(this);
       vector<optional<Expr>> argFacto(argument.size());
       bool factoFound = false;
       for (size_t i = 0; i != argument.size(); i++) {
           argFacto[i] = argument[i]->factor(true);
           if (not factoFound and argFacto[i])
               factoFound = true;
       }
       if (factoFound) {
           Expr res = copy();
           for (size_t i = 0; i != argument.size(); ++i)
               res->setArgument(argFacto[i].value_or(argument[i]), i);

           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractMultiFunc::factor(Expr_info expr, bool full) const
{
    if (full) {
       // Expr result = Copy(this);
       vector<optional<Expr>> argFacto(argument.size());
       bool factoFound = false;
       for (size_t i = 0; i != argument.size(); i++) {
           argFacto[i] = argument[i]->factor(expr, true);
           if (not factoFound and argFacto[i])
               factoFound = true;
       }
       if (factoFound) {
           Expr res = copy();
           for (size_t i = 0; i != argument.size(); ++i)
               res->setArgument(argFacto[i].value_or(argument[i]), i);

           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractMultiFunc::collect(
        std::vector<Expr> const &factors,
        bool                     full
        ) const
{
    if (full) {
       vector<optional<Expr>> argCollect(argument.size());
       bool factoFound = false;
       for (size_t i = 0; i != argument.size(); i++) {
           argCollect[i] = argument[i]->collect(factors, true);
           if (not factoFound and argCollect[i])
               factoFound = true;
       }
       if (factoFound) {
           Expr res = copy();
           for (size_t i = 0; i != argument.size(); ++i)
               res[i] = argCollect[i].value_or(res[i]);

           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractMultiFunc::expand(bool full,
                                         bool inPlace) const
{
    if (full) {
       // Expr result = Copy(this);
       vector<optional<Expr>> argExpand(argument.size());
       bool expandFound = false;
       for (size_t i = 0; i != argument.size(); i++) {
           argExpand[i] = argument[i]->expand(true, inPlace);
           if (not expandFound and argExpand[i])
               expandFound = true;
       }
       if (expandFound) {
           Expr res = copy();
           for (size_t i = 0; i != argument.size(); ++i)
               res->setArgument(argExpand[i].value_or(argument[i]), i);

           return Refreshed(res);
       }
    }

    return nullopt;
}

optional<Expr> AbstractMultiFunc::expand_if(
        std::function<bool(Expr const&)> const& f,
        bool full,
        bool inPlace) const
{
    if (full) {
       Expr res = copy();
       for (size_t i = 0; i != res->size(); ++i)
           res->setArgument(
                       res[i]->expand_if(f, true, inPlace).value_or(res[i]), 
                       i);

       return Refreshed(res);
    }

    return nullopt;
}

bool AbstractMultiFunc::dependsOn(Expr_info expr) const
{
    if (*this == expr)
        return true;
    for (const auto& arg : argument) {
        if (arg->dependsOn(expr))  {
            return true;
        }
    }

    return false;
}

bool AbstractMultiFunc::dependsOn(const AbstractParent* parent) const
{
    for (const auto& arg : argument) {
        if (arg->dependsOn(parent))  {
            return true;
        }
    }

    return false;
}

bool AbstractMultiFunc::dependsExplicitlyOn(Expr_info expr) const
{
    if (operator==(expr))
        return true;

    for (const auto& arg : argument)
        if (arg->dependsExplicitlyOn(expr)) 
            return true;

    return false;
}

bool AbstractMultiFunc::dependsExplicitlyOn(Parent_info expr) const
{
    for (const auto& arg : argument)
        if (arg->dependsExplicitlyOn(expr)) 
            return true;

    return false;
}

bool AbstractMultiFunc::commutesWith(Expr_info expr, int sign) const
{
    for (auto& arg : argument)
        if (not arg->commutesWith(expr, sign))
            return false;
    return true;
}

int AbstractMultiFunc::isPolynomial(Expr_info expr) const {
    return (this->operator==(expr));
}

Expr const &AbstractMultiFunc::operator[](int i) const
{
    return argument[i];
}

Expr& AbstractMultiFunc::operator[](int i)
{
    return argument[i];
}


} // End of namespace csl
