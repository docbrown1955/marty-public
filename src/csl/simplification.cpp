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

#include "simplification.h"
#include "abreviation.h"
#include "booleanOperators.h"
#include "commutation.h"
#include "comparison.h"
#include "field.h"
#include "indicial.h"
#include "literal.h"
#include "mathFunctions.h"
#include "numerical.h"
#include "numericalEval.h"
#include "operations.h"
#include "options.h"
#include "pseudoIntegral.h"
#include "scalar.h"
#include "tensorField.h"
#include "vector.h"

using namespace std;

namespace csl {

inline void convertScalarFuncType(const Abstract *expr, csl::Type &type)
{
    if (expr->getPrimaryType() == csl::PrimaryType::ScalarFunction
        and type != csl::Type::ScalarIntegral
        and type != csl::Type::VectorIntegral)
        type = csl::Type::Exp;
}

inline bool valueRule(const Abstract *A, const Abstract *B)
{
    const double valueA = A->evaluateScalar();
    const double valueB = B->evaluateScalar();
    if (valueA >= 0 and valueB < 0)
        return true;
    else if (not(valueA <= 0 and valueB >= 0))
        return valueA < valueB;
    return false;
}

inline bool complexRule(const Abstract *A, const Abstract *B)
{
    Expr imagA = A->getImaginaryPart();
    Expr imagB = B->getImaginaryPart();
    if (imagA < imagB)
        return true;
    else if (imagA != imagB)
        return false;

    Expr realA = A->getRealPart().value();
    Expr realB = B->getRealPart().value();
    return realA < realB;
}

inline bool evalRule(const Abstract *A, const Abstract *B)
{
    auto a = A->evaluateScalar();
    auto b = B->evaluateScalar();

    if (a < b)
        return true;
    else if (a != b)
        return false;

    a = A->getDeltaPlus();
    b = B->getDeltaPlus();

    if (a < b)
        return true;
    else if (a != b)
        return false;

    return A->getDeltaMinus() < B->getDeltaMinus();
}

inline bool alphaRule(const Abstract *A, const Abstract *B)
{
    auto const &nameA = A->getName();
    auto const &nameB = B->getName();
    if (nameA < nameB)
        return true;
    else if (!(nameB < nameA))
        return A->isComplexConjugate() < B->isComplexConjugate();
    return false;
}

inline bool mathRule(const Abstract *A, const Abstract *B)
{
    csl::Type typeA = A->getType();
    csl::Type typeB = B->getType();

    if (typeA < typeB)
        return true;
    if (typeA == typeB)
        return A->getArgument() < B->getArgument();
    return false;
}

inline bool sumRule(const Abstract *A, const Abstract *sum)
{
    auto       iter  = sum->end() - 1;
    auto const begin = sum->begin();
    while (iter != begin) {
        auto const &arg = *iter;
        if (*A < arg.get())
            return true;
        else if (*arg < A)
            return false;
        --iter;
    }
    return true; // return Sum(CSL_0, A->copy(), true) < sum;
}

inline bool prodRule(const Abstract *A, const Abstract *prod)
{
    if (A->isIndexed())
        return IProd(CSL_1, A->copy(), true) < prod;
    else
        return Prod(CSL_1, A->copy(), true) < prod;
}

inline bool powRule(const Abstract *A, const Abstract *pow)
{
    return Pow(A->copy(), CSL_1) < pow;
}

inline bool sumRule_inverted(const Abstract *A, const Abstract *sum)
{
    // auto other = Sum(CSL_0, A->copy(), true);
    // return *sum < &other;
    auto       iter  = sum->end() - 1;
    auto const begin = sum->begin();
    while (iter != begin) {
        auto const &arg = *iter;
        if (*A < arg.get())
            return false;
        else if (*arg < A)
            return true;
        --iter;
    }
    return false; // return Sum(CSL_0, A->copy(), true) < sum;
}

inline bool prodRule_inverted(const Abstract *A, const Abstract *prod)
{
    if (A->isIndexed()) {
        auto expr = IProd(CSL_1, A->copy(), true);
        return *prod < &expr;
    }
    else {
        auto expr = Prod(CSL_1, A->copy(), true);
        return *prod < &expr;
    }
}

inline bool powRule_inverted(const Abstract *A, const Abstract *pow)
{
    auto expr = Pow(A->copy(), CSL_1);
    return *pow < &expr;
}

inline bool vectorialRule(const Abstract *A, const Abstract *B)
{
    if (A->getNArgs() < B->getNArgs())
        return true;
    else if (B->getNArgs() == A->getNArgs() and B->getNArgs() > 0) {
        auto iterA = A->begin();
        auto iterB = B->begin();
        do {
            if (*iterA < *iterB)
                return true;
            else if (*iterB < *iterA)
                return false;
        } while (++iterA != A->end() and ++iterB != B->end());
    }
    return false;
}

inline bool ruleO1(const Abstract *A, const Abstract *B)
{
    if (not csl::option::fullComparison) {
        if (A->size() < B->size())
            return true;
        else if (A->size() > B->size())
            return false;
    }
    auto       iterA  = A->end() - 1;
    auto       iterB  = B->end() - 1;
    const auto beginA = A->begin();
    const auto beginB = B->begin();
    while (true) {
        if (*iterA < *iterB) {
            return true;
        }
        else if (*iterB < *iterA) {
            return false;
        }
        if (iterA-- == beginA or iterB-- == beginB)
            break;
    }

    // return true if A has been finished and not B (B has more args)
    return (A->getNArgs() < B->getNArgs());
}

inline bool compareDuoArgs(const Expr &argA0,
                           const Expr &argA1,
                           const Expr &argB0,
                           const Expr &argB1)
{
    if (argA0 < argB0)
        return true;
    if (not(argB0 < argA0))
        return argA1 < argB1;

    return false;
}

inline bool ruleO2(const Abstract *A, const Abstract *B)
{
    Expr const &argA0 = A->getArgument(0);
    Expr const &argA1 = A->getArgument(1);
    Expr const &argB0 = B->getArgument(0);
    Expr const &argB1 = B->getArgument(1);

    return compareDuoArgs(argA0, argA1, argB0, argB1);
}

inline bool ruleO2_inverted(const Abstract *A, const Abstract *B)
{
    Expr const &argA0 = A->getArgument(0);
    Expr const &argA1 = A->getArgument(1);
    Expr const &argB0 = B->getArgument(0);
    Expr const &argB1 = B->getArgument(1);

    return compareDuoArgs(argA1, argA0, argB1, argB0);
}

inline bool ruleO3(const Abstract *A, const Abstract *B)
{
    Expr const &opA  = A->getOperand();
    Expr const &opB  = B->getOperand();
    Expr const &varA = A->getVariable();
    Expr const &varB = B->getVariable();

    return compareDuoArgs(opA, varA, opB, varB);
}

inline bool ruleO4(const Abstract *A, const Abstract *B)
{
    int comp = compare(A->getName(), B->getName());
    if (comp == -1)
        return true;
    if (comp == 1)
        return false;
    if (A->getIndexStructureView() < B->getIndexStructureView())
        return true;
    else if (B->getIndexStructureView() < A->getIndexStructureView())
        return false;
    return A->isComplexConjugate() < B->isComplexConjugate();
}

inline bool ruleO5(const Abstract *A, const Abstract *B)
{
    int comp = compare(A->getName(), B->getName());
    if (comp == -1)
        return true;
    else if (comp == 0) {
        return A->getPoint() < B->getPoint();
    }
    return false;
}

inline bool ruleO6(const Abstract *A, const Abstract *B)
{
    int comp = compare(A->getName(), B->getName());
    if (comp == -1)
        return true;
    if (comp == 1)
        return false;
    if (A->getPoint() < B->getPoint())
        return true;
    if (B->getPoint() < A->getPoint())
        return false;
    auto const &structA = A->getIndexStructureView();
    auto const &structB = B->getIndexStructureView();
    if (structA < structB)
        return true;
    else if (not(structB < structA)) {
        return A->isComplexConjugate() < B->isComplexConjugate();
    }
    return false;
}

inline bool ruleO7(const Abstract *A, const Abstract *B)
{
    auto structA = A->getIndexStructure();
    auto structB = B->getIndexStructure();
    if (structA < structB)
        return true;
    else if (not(structB < structA)) {
        if (A->getPoint() < B->getPoint())
            return true;
        else if (not(B->getPoint() < A->getPoint()))
            return A->getArgument() < B->getArgument();
    }
    return false;
}

bool Integer::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
        return valueRule(this, expr);

    default:
        return true;
    }
}

bool Float::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
        return false;

    case csl::Type::Float:
        return valueRule(this, expr);

    default:
        return true;
    }
}

bool IntFraction::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
        return false;

    case csl::Type::IntFraction:
        return valueRule(this, expr);

    default:
        return true;
    }
}

bool Complex::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
        return false;

    case csl::Type::Complex:
        return complexRule(this, expr);

    default:
        return true;
    }
}

bool NumericalEval::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
        return false;

    case csl::Type::NumericalEval:
        return evalRule(this, expr);

    default:
        return true;
    }
}

bool IntFactorial::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
        return false;

    case csl::Type::IntFactorial:
        return value < expr->getValue();

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Imaginary::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
        return false;

    default:
        return true;
    }
}

bool Constant::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
        return false;

    case csl::Type::Constant:
        return alphaRule(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Arbitrary::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
        return false;

    case csl::Type::NoType:
        return alphaRule(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Variable::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {
    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
        return false;

    case csl::Type::Variable:
        return alphaRule(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Sum::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    if (type == csl::Type::TensorElement)
        return sumRule_inverted(expr, this);
    switch (type) {

    case csl::Type::Sum:
        return ruleO1(this, expr);

    case csl::Type::Prod:
        return prodRule(this, expr);

    case csl::Type::Integer:
        return false;
    case csl::Type::Pow:
        return powRule(this, expr);
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::Imaginary:
        return false;

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());

    default:
        return sumRule_inverted(expr, this);
    }
}

bool Polynomial::operator<(const Abstract *expr) const
{
    return *getRegularExpression() < expr;
}

bool Prod::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    if (type == csl::Type::TensorElement or type == csl::Type::Sum)
        return prodRule_inverted(expr, this);
    switch (type) {

    case csl::Type::Prod:
        return ruleO1(this, expr);

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::Imaginary:
        return false;

    default:
        return prodRule_inverted(expr, this);
    }
}

bool Pow::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::Imaginary:
        return false;

    case csl::Type::Prod:
        return prodRule(this, expr);

    case csl::Type::Pow:
        return ruleO2(this, expr);

    default:
        return powRule_inverted(expr, this);
    }
}

bool AbstractFunc::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::BooleanOperator:
        return false;

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::Exp:
        return mathRule(this, expr);

    default:
        return true;
    }
}

bool AbstractDuoFunc::operator<(Expr_info expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::IntFactorial:
    case csl::Type::Factorial:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
        return false;

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::StandardDuo:
        return ruleO2(this, expr);

    default:
        return true;
    }
}

bool AbstractMultiFunc::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Factorial:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
    case csl::Type::StandardDuo:
        return false;

    case csl::Type::StandardMult:
        return ruleO1(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool BooleanOperator::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
        return false;

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::BooleanOperator:
        return ruleO1(this, expr);

    default:
        return true;
    }
}

bool Angle::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
        return false;

    case csl::Type::Angle:
        return ruleO2(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Commutator::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
    case csl::Type::StandardDuo:
    case csl::Type::StandardMult:
    case csl::Type::Angle:
        return false;

    case csl::Type::Commutator:
        return ruleO2(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Scalar::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
        return false;

    case csl::Type::Scalar:
        return ruleO2(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Derivative::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
    case csl::Type::Angle:
    case csl::Type::StandardDuo:
    case csl::Type::StandardMult:
    case csl::Type::Commutator:
        return false;

    case csl::Type::Derivative:
        return ruleO2_inverted(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool Integral::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    convertScalarFuncType(expr, type);

    switch (type) {

    case csl::Type::Integer:
    case csl::Type::Float:
    case csl::Type::IntFraction:
    case csl::Type::Complex:
    case csl::Type::NumericalEval:
    case csl::Type::IntFactorial:
    case csl::Type::Imaginary:
    case csl::Type::Constant:
    case csl::Type::NoType:
    case csl::Type::Variable:
    case csl::Type::Exp:
    case csl::Type::BooleanOperator:
    case csl::Type::Scalar:
    case csl::Type::RealPart:
    case csl::Type::ImaginaryPart:
    case csl::Type::Angle:
    case csl::Type::StandardDuo:
    case csl::Type::StandardMult:
    case csl::Type::Commutator:
    case csl::Type::Derivative:
        return false;

    case csl::Type::Integral:
        return ruleO2_inverted(this, expr);

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    default:
        return true;
    }
}

bool ScalarIntegral::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
        return ruleO3(this, expr);

    case csl::Type::VectorIntegral:
        return true;

    default:
        return false;
    }
}

bool VectorIntegral::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::VectorIntegral:
        return ruleO3(this, expr);

    case csl::Type::ScalarIntegral:
        return false;

    default:
        return false;
    }
}

bool Vector::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::Matrix:
    case csl::Type::HighDTensor:
        return true;

    case csl::Type::Vector:
        return vectorialRule(this, expr);

    default:
        return false;
    }
}

bool Matrix::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::HighDTensor:
        return true;

    case csl::Type::Matrix:
        return vectorialRule(this, expr);

    default:
        return false;
    }
}

bool HighDTensor::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
        return true;

    case csl::Type::HighDTensor:
        return vectorialRule(this, expr);

    default:
        return false;
    }
}

bool TensorElement::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    if (type == csl::Type::TensorElement)
        return ruleO4(this, expr);
    switch (type) {

    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Integer:
        return false;
    case csl::Type::Pow:
        return powRule(this, expr);
    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::Vector:
    case csl::Type::Matrix:
    case csl::Type::HighDTensor:
    case csl::Type::ScalarField:
    case csl::Type::TensorFieldElement:
    case csl::Type::TDerivativeElement:
        return true;

    default:
        return false;
    }
}

bool ScalarField::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::Vector:
    case csl::Type::Matrix:
    case csl::Type::HighDTensor:
    case csl::Type::TensorFieldElement:
    case csl::Type::TDerivativeElement:
        return true;

    case csl::Type::ScalarField:
        return ruleO5(this, expr);

    default:
        return false;
    }
}

bool TensorFieldElement::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::Vector:
    case csl::Type::Matrix:
    case csl::Type::HighDTensor:
    case csl::Type::TDerivativeElement:
        return true;

    case csl::Type::TensorFieldElement:
        return ruleO6(this, expr);

    default:
        return false;
    }
}

bool TDerivativeElement::operator<(const Abstract *expr) const
{
    csl::Type type = expr->getType();
    switch (type) {

    case csl::Type::Polynomial:
        return operator<(expr->getRegularExpression().get());
    case csl::Type::Sum:
        return sumRule(this, expr);
    case csl::Type::Prod:
        return prodRule(this, expr);
    case csl::Type::Pow:
        return powRule(this, expr);

    case csl::Type::ScalarIntegral:
    case csl::Type::VectorIntegral:
    case csl::Type::Vector:
    case csl::Type::Matrix:
    case csl::Type::HighDTensor:
        return true;

    case csl::Type::TDerivativeElement:
        return ruleO7(this, expr);

    default:
        return false;
    }
}

} // End of namespace csl
