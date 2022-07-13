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

#include "numerical.h"
#include "comparison.h"
#include "error.h"
#include "interface.h"
#include "librarygenerator.h"
#include "mathFunctions.h"
#include "operations.h"
#include "utils.h"

using namespace std;

// Expr CSL_0 = csl::make_shared<csl::Integer>(0);
// Expr CSL_1 = csl::make_shared<csl::Integer>(1);
// Expr CSL_2 = csl::make_shared<csl::Integer>(2);
// Expr CSL_M_1 = csl::make_shared<csl::Integer>(-1);
// bool smNum->initialized = true;
//
// bool smNum->init() {
//     if (initialized)
//         return true;
//     ZERO  = csl::make_shared<csl::Integer>(0);
//     ONE   = csl::make_shared<csl::Integer>(1);
//     TWO   = csl::make_shared<csl::Integer>(2);
//     M_ONE = csl::make_shared<csl::Integer>(-1);
//
//     initialized = true;
//     return true;
// }

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractNumerical                       //
/*************************************************/
///////////////////////////////////////////////////

Expr AbstractNumerical::getNumericalFactor() const
{
    return copy();
}

optional<Expr> AbstractNumerical::getTerm() const
{
    return CSL_1;
}

void AbstractNumerical::printCode(int mode, std::ostream &out) const
{
    print(mode, out);
}

std::vector<Parent> AbstractNumerical::getSubSymbols() const
{
    return {};
}

std::optional<Expr> AbstractNumerical::getComplexArgument() const
{
    return evaluateScalar() < 0 ? CSL_PI : CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Float                                  //
/*************************************************/
///////////////////////////////////////////////////

csl::Type Float::getType() const
{
    return csl::Type::Float;
}

bool Float::isInteger() const
{
    return value == round(value);
}

void Float::setValue(long double t_value)
{
    value = t_value;
}

void Float::print(int mode, std::ostream &out, LibraryMode) const
{
    if (mode == 0)
        out << value << endl;
    else if (mode > 0 and value < 0)
        out << "(" << value << ")";
    else
        out << value;
    printProp(out);
}

string Float::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0)
        sout << value << endl;
    else if (value < 0)
        sout << "(" << value << ")";
    else
        sout << value;

    return sout.str();
}

long double Float::evaluateScalar() const
{
    return value;
}

optional<Expr> Float::evaluate(csl::eval::mode) const
{
    return nullopt;
}

unique_Expr Float::copy_unique() const
{
    return make_unique<Float>(value);
}

Expr Float::refresh() const
{
    return autonumber_s(value);
}

bool Float::operator==(Expr_info expr) const
{
    if (expr->getName() == Comparator::dummyName())
        return expr->operator==(this);
    if (expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return 0;
    const auto type = expr->getType();
    if (type == csl::Type::Complex) {
        if (expr->getImaginaryPart() != CSL_0)
            return false;
        auto real = expr->getRealPart();
        if (!real && value == 0)
            return true;
        else if (real && (**real).evaluateScalar() == value)
            return true;
        return false;
    }

    return (value == expr->evaluateScalar());
}

Expr Float::multiplication_own(const Expr &expr, bool) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->multiplication_own(copy());
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
        return autonumber_s(value * expr->evaluateScalar());
        break;

    case csl::Type::Float:
        return autonumber_s(value * expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        if (value == round(value))
            return intfraction_s(value * expr->getNum(), expr->getDenom());
        return float_s(value * expr->evaluateScalar());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Float::division_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex) {
        return copy() * GetComplexConjugate(expr)
               / (pow_s(GetComplexModulus(expr), CSL_2));
    }
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
    case csl::Type::Float:
        return autonumber_s(value * 1 / expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        if (value == round(value))
            return intfraction_s(value * expr->getDenom(), expr->getNum());
        return float_s(value * 1 / expr->evaluateScalar());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Float::exponentiation_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return csl::make_shared<Pow, alloc_pow>(copy(), expr);
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    double value2 = expr->evaluateScalar();
    switch (expr->getType()) {
    case csl::Type::Integer:
    case csl::Type::Float:
        if (value2 > 0)
            return autonumber_s(pow(value, expr->evaluateScalar()));
        else {
            long double res = pow(value, -value2);
            if (res == std::round(res))
                return intfraction_s(1, res);
            return float_s(1 / res);
        }
        break;

    case csl::Type::IntFraction:
    case csl::Type::Complex:
        return csl::make_shared<Pow, alloc_pow>(float_s(value), expr);
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Float::addition_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->addition_own(copy());
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
        return autonumber_s(value + expr->evaluateScalar());
        break;

    case csl::Type::Float:
        return autonumber_s(value + expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        if (value == round(value))
            return intfraction_s(value * expr->getDenom() + expr->getNum(),
                                 expr->getDenom());
        return float_s(value + expr->evaluateScalar());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

optional<Expr> Float::derive(Expr_info) const
{
    return CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Integer                                 //
/*************************************************/
///////////////////////////////////////////////////

csl::Type Integer::getType() const
{
    return csl::Type::Integer;
}

void Integer::setValue(long double t_value)
{
    value = round(t_value);
}

void Integer::print(int mode, std::ostream &out, LibraryMode) const
{
    if (mode == 0)
        out << value << endl;
    else if (mode > 1 and value < 0)
        out << "(" << value << ")";
    else
        out << value;
    printProp(out);
}

string Integer::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0)
        sout << value << endl;
    else if (value < 0)
        sout << "(" << value << ")";
    else
        sout << value;

    return sout.str();
}

long double Integer::evaluateScalar() const
{
    return value;
}

optional<Expr> Integer::evaluate(csl::eval::mode) const
{
    return nullopt;
}

unique_Expr Integer::copy_unique() const
{
    return make_unique<Integer>(value);
}

bool Integer::operator==(Expr_info expr) const
{
    if (expr == this)
        return true;
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    const auto type = expr->getType();
    if (type == csl::Type::Integer)
        return value == static_cast<int>(expr->evaluateScalar());
    else if (type == csl::Type::Float)
        return value == expr->evaluateScalar();

    return false;
}

Expr Integer::multiplication_own(const Expr &expr, bool) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->multiplication_own(copy());
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
        return autonumber_s(value * expr->evaluateScalar());
        break;

    case csl::Type::Float:
        return autonumber_s(value * expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        return intfraction_s(value * expr->getNum(), expr->getDenom());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Integer::addition_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->addition_own(copy());
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
        return autonumber_s(value + expr->evaluateScalar());
        break;

    case csl::Type::Float:
        return autonumber_s(value + expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        return intfraction_s(value * expr->getDenom() + expr->getNum(),
                             expr->getDenom());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Integer::division_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex) {
        return copy() * GetComplexConjugate(expr)
               / (pow_s(GetComplexModulus(expr), CSL_2));
    }
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    switch (expr->getType()) {
    case csl::Type::Integer:
    case csl::Type::Float:
        return autonumber_s(value * 1. / expr->evaluateScalar());
        break;

    case csl::Type::IntFraction:
        if (value == round(value))
            return intfraction_s(value * expr->getDenom(), expr->getNum());
        return float_s(value * 1. / expr->evaluateScalar());
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

Expr Integer::exponentiation_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->exponentiation_own(copy());
    if (not expr or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    double value2 = expr->evaluateScalar();
    switch (expr->getType()) {
    case csl::Type::Integer:
        if (value2 > 0)
            return autonumber_s(pow(value, expr->evaluateScalar()));
        else
            return intfraction_s(1, pow(value, -value2));
        break;

    case csl::Type::Float:
        if (value2 > 0 or value2 != round(value2))
            return autonumber_s(pow(value, expr->evaluateScalar()));
        else
            return intfraction_s(1, pow(value, -value2));
        break;

    case csl::Type::IntFraction:
    case csl::Type::Complex:
        return csl::make_shared<Pow, alloc_pow>(copy(), expr);
        break;

    default:
        cout << "Warning: numerical type \"" << expr->getType();
        cout << "\" not recognized in number oeprations.\n";
        return CSL_0;
    }
}

optional<Expr> Integer::derive(Expr_info) const
{
    return CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class IntFraction                               //
/*************************************************/
///////////////////////////////////////////////////

IntFraction::IntFraction(long long int t_num, long long int t_denom)
    : AbstractNumerical()
{
    if (t_num < 0) {
        if (t_denom < 0) {
            t_num *= -1;
            t_denom *= -1;
        }
    }
    else if (t_denom < 0) {
        t_num *= -1;
        t_denom *= -1;
    }

    const long long int pgcd = PGCD(t_num, t_denom);
    if (pgcd != 1) {
        t_num /= pgcd;
        t_denom /= pgcd;
    }
    num   = t_num;
    denom = t_denom;
}

csl::Type IntFraction::getType() const
{
    return csl::Type::IntFraction;
}
long long int IntFraction::getNum() const
{
    return num;
}
long long int IntFraction::getDenom() const
{
    return denom;
}

void IntFraction::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    if (mode == 0) {
        if (libMode != LibraryMode::NoLib)
            out << num << "./" << denom << endl;
        else
            out << "Fraction: " << num << "/" << denom << endl;
    }
    else if (mode > 3) {
        if (libMode != LibraryMode::NoLib)
            out << "(" << num << "./" << denom << ")";
        else
            out << "(" << num << "/" << denom << ")";
    }
    else if (libMode != LibraryMode::NoLib)
        out << num << "./" << denom;
    else
        out << num << "/" << denom;

    printProp(out);
}

void IntFraction::printCode(int, std::ostream &out) const
{
    out << "csl::intfraction_s(" << num << ", " << denom << ")";
}

string IntFraction::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0)
        sout << "\\frac{" << num << "}{" << denom << "}";
    else if (mode > 3)
        sout << "\\left("
             << "\\frac{" << num << "}{" << denom << "}\\right)";
    else
        sout << "\\frac{" << num << "}{" << denom << "}";

    return sout.str();
}

long double IntFraction::evaluateScalar() const
{
    return (num * 1.) / denom;
}

optional<Expr> IntFraction::evaluate(csl::eval::mode user_mode) const
{
    if (csl::eval::isContained(user_mode, csl::eval::numerical))
        return float_s(num * 1. / denom);
    return nullopt;
}

unique_Expr IntFraction::copy_unique() const
{
    return make_unique<IntFraction>(num, denom);
}

Expr IntFraction::refresh() const
{
    return intfraction_s(num, denom);
}

Expr IntFraction::multiplication_own(const Expr &expr, bool) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->multiplication_own(copy());
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    double value = expr->evaluateScalar();
    if (expr->isInteger()) // Integer
        return intfraction_s(num * value, denom);
    else if (expr->getType() == csl::Type::Float)
        return float_s(evaluateScalar() * value);

    // Fraction
    int t_num   = expr->getNum();
    int t_denom = expr->getDenom();

    return intfraction_s(num * t_num, denom * t_denom);
}

Expr IntFraction::addition_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return expr->addition_own(copy());
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;
    double value = expr->evaluateScalar();
    if (expr->isInteger())
        return intfraction_s(num + denom * value, denom);
    else if (expr->getType() == csl::Type::Float)
        return float_s(evaluateScalar() + value);

    // Fraction
    int t_num   = expr->getNum();
    int t_denom = expr->getDenom();

    return intfraction_s(num * t_denom + t_num * denom, denom * t_denom);
}

Expr IntFraction::division_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex) {
        return copy() * GetComplexConjugate(expr)
               / (pow_s(GetComplexModulus(expr), CSL_2));
    }
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    double value = expr->evaluateScalar();
    if (expr->isInteger()) // Integer
        return intfraction_s(num, denom * value);
    else if (expr->getType() == csl::Type::Float)
        return float_s(evaluateScalar() * 1. / value);

    // Fraction
    int t_num   = expr->getNum();
    int t_denom = expr->getDenom();

    return intfraction_s(num * t_denom, denom * t_num);
}

Expr IntFraction::exponentiation_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return csl::make_shared<Pow, alloc_pow>(copy(), expr);
    if (expr == nullptr
        or expr->getPrimaryType() != csl::PrimaryType::Numerical)
        return CSL_0;

    double value = expr->evaluateScalar();
    if (expr->isInteger() and value > 0) // Integer
        return intfraction_s(pow(num, (int) value), pow(denom, int(value)));
    else if (expr->isInteger())
        return intfraction_s(pow(denom, (int) (-value)),
                             pow(num, (int) (-value)));
    else if (expr->getType() == csl::Type::Float)
        return float_s(pow(evaluateScalar(), value));

    return csl::make_shared<Pow, alloc_pow>(intfraction_s(num, denom), expr);
}

optional<Expr> IntFraction::derive(Expr_info) const
{
    return CSL_0;
}
bool IntFraction::operator==(Expr_info expr) const
{
    if (expr == this)
        return true;
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getPrimaryType() == csl::PrimaryType::Numerical) {
        if (expr->getType() == csl::Type::Complex) {
            if (expr->getImaginaryPart() != CSL_0)
                return false;
            auto real = (expr->getRealPart()).value_or(CSL_0);
            return (*this == real.get());
        }
        return (expr->evaluateScalar() == (num * 1. / denom));
    }

    return 0;
}

Expr float_s(long double value)
{
    if (abs(value) < abs(
            static_cast<long double>(std::numeric_limits<long long>::min()))
        && value == round(value))
        return int_s(value);
    return csl::make_shared<Float, alloc_float>(value);
}

Expr int_s(long long int value)
{
    if (value == 0)
        return CSL_0;
    if (value == 1)
        return CSL_1;
    if (value == -1)
        return CSL_M_1;
    if (value == 2)
        return CSL_2;
    if (value == -2)
        return CSL_M_2;
    return csl::make_shared<Integer, alloc_integer>(value);
}

Expr autonumber_s(long double value)
{
    return float_s(value);
}

Expr intfraction_s(long long int num, long long int denom)
{
    if (denom == 0)
        return CSL_INF;
    if (denom < 0) {
        num   = -num;
        denom = -denom;
    }
    if (num % denom == 0)
        return int_s(num / denom);
    if (num == 1) {
        if (denom == 2)
            return CSL_HALF;
        if (denom == 3)
            return CSL_THIRD;
    }
    if (num == -1) {
        if (denom == 2)
            return CSL_M_HALF;
        if (denom == 3)
            return CSL_M_THIRD;
    }

    return csl::make_shared<IntFraction, alloc_intfraction>(num, denom);
}

///////////////////////////////////////////////////
/*************************************************/
// Class Complex                                 //
/*************************************************/
///////////////////////////////////////////////////

Complex::Complex() : real(CSL_0), imag(CSL_0)
{
}

Complex::Complex(Expr t_real) : AbstractNumerical(), real(t_real), imag(CSL_0)
{
    checkNumerical(real);
    if (real->getType() == csl::Type::Complex)
        *this = Complex(GetRealPart(real), GetImaginaryPart(real));
}

Complex::Complex(Expr t_real, Expr t_imag)
    : AbstractNumerical(), real(t_real), imag(t_imag)
{
    checkNumerical(real);
    checkNumerical(imag);
    if (real->getType() == csl::Type::Complex
        or imag->getType() == csl::Type::Complex) {
        t_real = GetRealPart(real) - GetImaginaryPart(imag);
        t_imag = GetImaginaryPart(real) + GetRealPart(imag);
        *this  = Complex(t_real, t_imag);
    }
}

void Complex::checkNumerical(Expr const &expr) const
{
    CSL_ASSERT_SPEC(expr->getPrimaryType() == csl::PrimaryType::Numerical,
                    CSLError::TypeError,
                    "Expected numerical type in complex, "
                        + toString(expr->getType()) + " given.");
}

csl::Type Complex::getType() const
{
    return csl::Type::Complex;
}

void Complex::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    if (mode > 1 and imag != CSL_0)
        out << "(";
    if (libMode == LibraryMode::CLib) {
        real->print(1, out, libMode);
        out << " + _Complex_I*";
        imag->print(1, out, libMode);
    }
    else {
        if (imag == CSL_0) {
            real->print(1, out, libMode);
        }
        else {
            out << LibraryGenerator::complexUsing << "{";
            real->print(1, out, libMode);
            out << ", ";
            imag->print(1, out, libMode);
            out << "}";
        }
    }
    if (mode > 1 and imag != CSL_0)
        out << ")";
    if (mode == 0)
        out << endl;
}

void Complex::printCode(int, std::ostream &out) const
{
    out << "csl::complex_s(" << real << ", " << imag << ")";
}

std::string Complex::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode > 1 and imag != CSL_0)
        sout << "(";
    sout << real->printLaTeX(1);
    if (imag != CSL_0) {
        sout << " +i";
        sout << imag->printLaTeX(1);
    }
    if (mode > 1 and imag != CSL_0)
        sout << ")";
    if (mode == 0)
        sout << endl;
    return sout.str();
}

long double Complex::evaluateScalar() const
{
    CSL_ASSERT_SPEC(false,
                    CSLError::ValueError,
                    "Cannot get value for a complex number. Use getRealPart() "
                    "instead.")
    return real->evaluateScalar();
}

std::optional<Expr> Complex::evaluate(csl::eval::mode) const
{
    return std::nullopt;
}

bool Complex::isReal() const
{
    return imag == CSL_0;
}

bool Complex::isPurelyImaginary() const
{
    return imag != CSL_0 and real == CSL_0;
}

std::optional<Expr> Complex::getRealPart() const
{
    return real;
}

Expr Complex::getImaginaryPart() const
{
    return imag;
}

std::optional<Expr> Complex::getComplexArgument() const
{
    if (imag == CSL_0)
        return (real->evaluateScalar() >= 0) ? CSL_0 : CSL_PI;
    if (real == CSL_0)
        return imag->evaluateScalar() < 0 ? -CSL_PI / 2 : CSL_PI / 2;
    return csl::make_shared<Angle>(real, imag);
}

std::optional<Expr> Complex::getComplexModulus() const
{
    return sqrt_s(real * real + imag * imag);
}

std::optional<Expr> Complex::getComplexConjugate() const
{
    if (imag == CSL_0)
        return real;
    return csl::make_shared<Complex, alloc_complex>(real, -imag);
}

unique_Expr Complex::copy_unique() const
{
    return std::make_unique<Complex>(real, imag);
}

Expr Complex::refresh() const
{
    return complex_s(real, imag);
}

Expr Complex::multiplication_own(const Expr &expr, bool) const
{
    if (expr->getType() != csl::Type::Complex)
        return complex_s(real->multiplication_own(expr),
                         imag->multiplication_own(expr));

    Expr t_real = GetRealPart(expr);
    Expr t_imag = GetImaginaryPart(expr);
    return complex_s(real * t_real - imag * t_imag,
                     real * t_imag + imag * t_real);
}

Expr Complex::addition_own(const Expr &expr) const
{
    if (expr->getType() != csl::Type::Complex)
        return complex_s(real->addition_own(expr), imag);

    Expr t_real = GetRealPart(expr);
    Expr t_imag = GetImaginaryPart(expr);
    return complex_s(real + t_real, imag + t_imag);
}

Expr Complex::division_own(const Expr &expr) const
{
    if (expr->getType() != csl::Type::Complex) {
        return complex_s(real->division_own(expr), imag->division_own(expr));
    }

    return copy() * multiplication_own(GetComplexConjugate(expr))
           / pow_s(GetComplexModulus(expr), CSL_2);
}

Expr Complex::exponentiation_own(const Expr &expr) const
{
    if (expr->getType() == csl::Type::Complex)
        return csl::make_shared<Pow, alloc_pow>(copy(), expr);
    csl::Expr arg = getComplexArgument().value();
    csl::Expr mod = getComplexModulus().value();
    arg *= expr;
    csl::Expr new_real, new_imag;
    if (mod->getPrimaryType() == csl::PrimaryType::Numerical) {
        new_real = mod->exponentiation_own(expr) * csl::cos_s(arg);
        new_imag = mod->exponentiation_own(expr) * csl::sin_s(arg);
    }
    else {
        new_real = csl::pow_s(mod, expr) * csl::cos_s(arg);
        new_imag = csl::pow_s(mod, expr) * csl::sin_s(arg);
    }
    if (csl::IsNumerical(new_real) && csl::IsNumerical(new_imag)) {
        auto res = complex_s(new_real, new_imag);
        return res;
    }
    else {
        auto res = new_real + CSL_I * new_imag;
        return res;
    }
}

std::optional<Expr> Complex::derive(Expr_info) const
{
    return CSL_0;
}

bool Complex::operator==(Expr_info expr) const
{
    return expr->getType() == csl::Type::Complex
           and expr->getImaginaryPart() == imag
           and expr->getRealPart().value() == real;
}

Expr complex_s(Expr const &real, Expr const &imag)
{
    if (imag == CSL_0)
        return real;
    return csl::make_shared<Complex, alloc_complex>(real, imag);
}

} // End of namespace csl
