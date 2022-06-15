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

#include "mathFunctions.h"
#include "comparison.h"
#include "index.h"
#include "indicial.h"
#include "interface.h"
#include "librarydependency.h"
#include "librarygenerator.h"
#include "operations.h"
#include "replace.h"
#include "utils.h"

using namespace std;

#define IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Type, build)        \
    unique_Expr Type::copy_unique() const                    \
    {                                                        \
        return make_unique<Type>(argument);                  \
    }                                                        \
    Expr Type::deepCopy() const                              \
    {                                                        \
        return csl::make_shared<Type>(argument->deepCopy()); \
    }                                                        \
    Expr Type::refresh() const                               \
    {                                                        \
        return build(argument);                              \
    }                                                        \
    Expr Type::deepRefresh() const                           \
    {                                                        \
        return build(argument->deepRefresh());               \
    }

#define IMPLEMENT_PRINT_CODE(Type, build)              \
    void Type::printCode(int, std::ostream &out) const \
    {                                                  \
        out << "csl::" #build "(";                     \
        argument->printCode(1, out);                   \
        out << ")";                                    \
    }

namespace csl {

IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Exp, exp_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Log, log_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Cos, cos_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Sin, sin_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Tan, tan_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Cosh, cosh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Sinh, sinh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Tanh, tanh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ACos, acos_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ASin, asin_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ATan, atan_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ACosh, acosh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ASinh, asinh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(ATanh, atanh_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(Abs, abs_s)
IMPLEMENT_COPY_FUNCTIONS_ONE_ARG(DiracDelta, diracdelta_s)

IMPLEMENT_PRINT_CODE(Exp, exp_s)
IMPLEMENT_PRINT_CODE(Log, log_s)
IMPLEMENT_PRINT_CODE(Cos, cos_s)
IMPLEMENT_PRINT_CODE(Sin, sin_s)
IMPLEMENT_PRINT_CODE(Tan, tan_s)
IMPLEMENT_PRINT_CODE(Cosh, cosh_s)
IMPLEMENT_PRINT_CODE(Sinh, sinh_s)
IMPLEMENT_PRINT_CODE(Tanh, tanh_s)
IMPLEMENT_PRINT_CODE(ACos, acos_s)
IMPLEMENT_PRINT_CODE(ASin, asin_s)
IMPLEMENT_PRINT_CODE(ATan, atan_s)
IMPLEMENT_PRINT_CODE(ACosh, acosh_s)
IMPLEMENT_PRINT_CODE(ASinh, asinh_s)
IMPLEMENT_PRINT_CODE(ATanh, atanh_s)
IMPLEMENT_PRINT_CODE(Abs, abs_s)
IMPLEMENT_PRINT_CODE(DiracDelta, diracdelta_s)
IMPLEMENT_PRINT_CODE(Factorial, factorial_s)

///////////////////////////////////////////////////
/*************************************************/
// Class Abs                                     //
/*************************************************/
///////////////////////////////////////////////////

void Abs::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "cabsq(";
    else if (lib)
        out << "std::abs(";
    else
        out << "abs(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Abs::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\text{bas}\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Abs::getLibDependency() const
{
    return LibDependency::cmath();
}

optional<Expr> Abs::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    if (opt_arg)
        return abs_s(opt_arg.value());
    return nullopt;
}

long double Abs::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::abs(argument->evaluateScalar());
}

optional<Expr> Abs::derive(Expr_info) const
{
    cout << "Warning: derivative of abs function not defined.\n";
    return CSL_UNDEF;
}

Expr abs_s(const Expr &expr)
{
    csl::Type type = expr->getType();
    if (type == csl::Type::Integer or type == csl::Type::Float)
        return autonumber_s(std::abs(expr->evaluateScalar()));
    else if (type == csl::Type::IntFraction)
        return intfraction_s(std::abs(expr->getNum()),
                             std::abs(expr->getDenom()));

    Expr res = csl::make_shared<Abs>(expr);

    return res;
}

int Abs::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return 1;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Exp                                     //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Exp::getRealPart() const
{
    return prod_s(exp_s(GetRealPart(argument)),
                  cos_s(GetImaginaryPart(argument)));
}

Expr Exp::getImaginaryPart() const
{
    return prod_s(exp_s(GetRealPart(argument)),
                  sin_s(GetImaginaryPart(argument)));
}

optional<Expr> Exp::getComplexModulus() const
{
    return exp_s(GetRealPart(argument));
}

optional<Expr> Exp::getComplexArgument() const
{
    return argument->getImaginaryPart();
}

void Exp::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "expq(";
    else if (lib)
        out << "std::exp(";
    else
        out << "exp(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Exp::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "e^{";
    sout << argument->printLaTeX(1);
    sout << "}";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Exp::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Exp::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::exp(argument->evaluateScalar());
}

optional<Expr> Exp::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), exp_s(argument));
}

optional<Expr> Exp::expand(bool full, bool inplace) const
{
    Expr arg      = argument;
    bool expanded = false;
    if (full)
        Expand(arg, true, inplace);
    if (arg->getType() == csl::Type::Sum) {
        csl::vector_expr foo = arg->getVectorArgument();
        for (auto &el : foo)
            el = exp_s(el);
        return prod_s(foo);
    }
    if (expanded)
        return exp_s(arg);

    return nullopt;
}

optional<Expr> Exp::expand_if(std::function<bool(Expr const &)> const &f,
                              bool                                     full,
                              bool inplace) const
{
    Expr arg      = argument;
    bool expanded = false;
    if (full)
        ExpandIf(arg, f, true, inplace);
    if (arg->getType() == csl::Type::Sum and f(arg)) {
        csl::vector_expr foo = arg->getVectorArgument();
        for (auto &el : foo)
            el = exp_s(el);
        return prod_s(foo);
    }
    if (expanded)
        return exp_s(arg);

    return nullopt;
}

int Exp::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return 0;
}

Expr exp_s(const Expr &expr)
{
    csl::PrimaryType type = expr->getPrimaryType();
    if (type == csl::PrimaryType::Numerical
        && expr->getType() != csl::Type::Complex) {
        double value     = expr->evaluateScalar();
        int    value_int = round(value);
        if (value == value_int) {
            if (value_int == 0)
                return CSL_1;
            if (value_int == 1)
                return CSL_E;
        }
    }
    if (expr->getType() == csl::Type::Log)
        return expr->getArgument();

    Expr res = csl::make_shared<Exp>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Log                                     //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Log::getRealPart() const
{
    return log_s(GetComplexModulus(argument));
}

Expr Log::getImaginaryPart() const
{
    Expr   foo   = GetComplexArgument(argument);
    double value = foo->evaluateScalar();
    if (std::abs(value) > M_PI) // Taking the argument mod 2 pi
        foo = sum_s(foo, float_s(-M_PI * floor(std::abs(value) / M_PI)));

    return foo;
}

optional<Expr> Log::getComplexModulus() const
{
    return sum_s(pow_s(GetRealPart(copy()), CSL_2),
                 pow_s(GetImaginaryPart(copy()), CSL_2));
}

optional<Expr> Log::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void Log::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "clogq(";
    else if (lib)
        out << "std::log(";
    else
        out << "log(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Log::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\log\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Log::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Log::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::log(argument->evaluateScalar());
}

optional<Expr> Log::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), pow_s(argument, CSL_M_1));
}

int Log::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return 0;
}

Expr log_s(const Expr &expr)
{
    if (expr == CSL_1)
        return CSL_0;
    if (expr->getType() == csl::Type::Exp)
        return expr->getArgument();
    if (expr == CSL_E)
        return CSL_1;

    Expr res = csl::make_shared<Log>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Cos                                     //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Cos::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(cosh_s(im), cos_s(re));
}

Expr Cos::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(CSL_M_1, prod_s(sinh_s(im), sin_s(re)));
}

optional<Expr> Cos::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(cos_s(re), CSL_2)),
                 prod_s(pow_s(sinh_s(im), CSL_2), pow_s(sin_s(re), CSL_2)));
}

optional<Expr> Cos::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void Cos::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "cosq(";
    else if (lib)
        out << "std::cos(";
    else
        out << "cos(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Cos::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\cos\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Cos::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Cos::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::cos(argument->evaluateScalar());
}

optional<Expr> Cos::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), prod_s(CSL_M_1, sin_s(argument)));
}

int Cos::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return 1;
}

Expr cos_s(const Expr &expr)
{
    if (expr->getPrimaryType() == csl::PrimaryType::Arbitrary)
        return csl::make_shared<Cos>(expr);
    // int type = expr->getPrimaryType();
    if (*expr == CSL_0)
        return CSL_1;
    if (*expr == fraction_s(CSL_PI, CSL_2))
        return CSL_0;
    if (*expr == fraction_s(prod_s(int_s(-3), CSL_PI), CSL_2))
        return CSL_0;
    if (*expr == CSL_PI)
        return CSL_M_1;
    if (*expr == prod_s(CSL_M_1, CSL_PI))
        return CSL_M_1;
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, CSL_2)))
        return CSL_0;
    if (*expr == fraction_s(prod_s(CSL_M_1, CSL_PI), CSL_2))
        return CSL_0;
    if (*expr == fraction_s(CSL_PI, int_s(3)))
        return intfraction_s(1, 2);
    if (*expr == prod_s(intfraction_s(2, 3), CSL_PI))
        return intfraction_s(-1, 2);
    if (*expr == fraction_s(CSL_PI, int_s(6)))
        return fraction_s(sqrt_s(int_s(3)), CSL_2);
    if (*expr == prod_s(int_s(5), fraction_s(CSL_PI, int_s(6))))
        return fraction_s(prod_s(CSL_M_1, sqrt_s(int_s(3))), CSL_2);
    if (*expr == fraction_s(CSL_PI, int_s(4)))
        return fraction_s(sqrt_s(CSL_2), CSL_2);
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, int_s(4))))
        return prod_s(CSL_M_1, fraction_s(sqrt_s(CSL_2), CSL_2));

    Expr res = csl::make_shared<Cos>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Sin                                     //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Sin::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(cosh_s(im), sin_s(re));
}

Expr Sin::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(sinh_s(im), cos_s(re));
}

optional<Expr> Sin::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(sin_s(re), CSL_2)),
                 prod_s(pow_s(sinh_s(im), CSL_2), pow_s(cos_s(re), CSL_2)));
}

optional<Expr> Sin::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void Sin::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "sinq(";
    else if (lib)
        out << "std::sin(";
    else
        out << "sin(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Sin::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\sin\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Sin::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Sin::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::sin(argument->evaluateScalar());
}

optional<Expr> Sin::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), cos_s(argument));
}

int Sin::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr sin_s(const Expr &expr)
{
    if (expr->getPrimaryType() == csl::PrimaryType::Arbitrary)
        return csl::make_shared<Sin>(expr);
    if (*expr == CSL_0)
        return CSL_0;
    if (*expr == fraction_s(CSL_PI, CSL_2))
        return CSL_1;
    if (*expr == fraction_s(prod_s(int_s(-3), CSL_PI), CSL_2))
        return CSL_1;
    if (*expr == CSL_PI)
        return CSL_0;
    if (*expr == prod_s(CSL_M_1, CSL_PI))
        return CSL_0;
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, CSL_2)))
        return CSL_M_1;
    if (*expr == fraction_s(prod_s(CSL_M_1, CSL_PI), CSL_2))
        return CSL_M_1;
    if (*expr == fraction_s(CSL_PI, int_s(3)))
        return fraction_s(sqrt_s(int_s(3)), CSL_2);
    if (*expr == prod_s(intfraction_s(2, 3), CSL_PI))
        return fraction_s(sqrt_s(int_s(3)), CSL_2);
    if (*expr == fraction_s(CSL_PI, int_s(6)))
        return intfraction_s(1, 2);
    if (*expr == prod_s(int_s(5), fraction_s(CSL_PI, int_s(6))))
        return intfraction_s(1, 2);
    if (*expr == fraction_s(CSL_PI, int_s(4)))
        return fraction_s(sqrt_s(CSL_2), CSL_2);
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, int_s(4))))
        return fraction_s(sqrt_s(CSL_2), CSL_2);
    Expr res = csl::make_shared<Sin>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Tan                                     //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Tan::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    Expr modulus
        = sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(cos_s(re), CSL_2)),
                prod_s(pow_s(sinh_s(im), CSL_2), pow_s(sin_s(re), CSL_2)));

    return fraction_s(prod_s(cos_s(re), sin_s(re)), modulus);
}

Expr Tan::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    Expr modulus
        = sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(cos_s(re), CSL_2)),
                prod_s(pow_s(sinh_s(im), CSL_2), pow_s(sin_s(re), CSL_2)));

    return fraction_s(prod_s(cosh_s(im), sinh_s(im)), modulus);
}

optional<Expr> Tan::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);

    Expr modulus
        = sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(cos_s(re), CSL_2)),
                prod_s(pow_s(sinh_s(im), CSL_2), pow_s(sin_s(re), CSL_2)));

    Expr modulus2
        = sum_s(prod_s(pow_s(cosh_s(im), CSL_2), pow_s(sinh_s(im), CSL_2)),
                prod_s(pow_s(sin_s(re), CSL_2), pow_s(cos_s(im), CSL_2)));

    return fraction_s(modulus2, modulus);
}

optional<Expr> Tan::getComplexArgument() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return csl::make_shared<Angle>(prod_s(cos_s(re), sin_s(re)),
                                   prod_s(cosh_s(im), sinh_s(im)));
}

void Tan::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "tanq(";
    else if (lib)
        out << "std::tan(";
    else
        out << "tan(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Tan::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\tan\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Tan::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Tan::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::tan(argument->evaluateScalar());
}

optional<Expr> Tan::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr),
                  sum_s(CSL_1, pow_s(tan_s(argument), CSL_2)));
}

int Tan::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr tan_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    if (*expr == fraction_s(CSL_PI, CSL_2))
        return CSL_INF;
    if (*expr == fraction_s(prod_s(int_s(-3), CSL_PI), CSL_2))
        return CSL_INF;
    if (*expr == CSL_PI)
        return CSL_0;
    if (*expr == prod_s(CSL_M_1, CSL_PI))
        return CSL_0;
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, CSL_2)))
        return CSL_INF;
    if (*expr == fraction_s(prod_s(CSL_M_1, CSL_PI), CSL_2))
        return CSL_INF;
    if (*expr == fraction_s(CSL_PI, int_s(3)))
        return sqrt_s(int_s(3));
    if (*expr == prod_s(intfraction_s(2, 3), CSL_PI))
        return prod_s(CSL_M_1, sqrt_s(int_s(3)));
    if (*expr == fraction_s(CSL_PI, int_s(6)))
        return fraction_s(CSL_1, sqrt_s(int_s(3)));
    if (*expr == prod_s(int_s(5), fraction_s(CSL_PI, int_s(6))))
        return fraction_s(CSL_M_1, sqrt_s(int_s(3)));
    if (*expr == fraction_s(CSL_PI, int_s(4)))
        return CSL_1;
    if (*expr == prod_s(int_s(3), fraction_s(CSL_PI, int_s(4))))
        return CSL_1;
    Expr res = csl::make_shared<Tan>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ACos                                    //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> ACos::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(acosh_s(im), acos_s(re));
}

Expr ACos::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(CSL_M_1, prod_s(asinh_s(im), asin_s(re)));
}

optional<Expr> ACos::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);

    return sum_s(prod_s(pow_s(acosh_s(im), CSL_2), pow_s(acos_s(re), CSL_2)),
                 prod_s(pow_s(asinh_s(im), CSL_2), pow_s(asin_s(re), CSL_2)));
}

optional<Expr> ACos::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void ACos::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "acosq(";
    else if (lib)
        out << "std::acos(";
    else
        out << "acos(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ACos::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\acos\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ACos::getLibDependency() const
{
    return LibDependency::cmath();
}

// if (csl::eval::isContained(user_mode, csl::eval::numerical))
long double ACos::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::acos(argument->evaluateScalar());
}

optional<Expr> ACos::derive(Expr_info expr) const
{
    return prod_s(
        Derived(argument, expr),
        fraction_s(CSL_M_1,
                   pow_s(sum_s(CSL_1, prod_s(CSL_M_1, pow_s(argument, CSL_2))),
                         intfraction_s(1, 2))));
}

int ACos::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;

    return 0;
}

Expr acos_s(const Expr &expr)
{
    if (expr == CSL_0)
        return fraction_s(CSL_PI, CSL_2);
    return csl::make_shared<ACos>(expr);
}

///////////////////////////////////////////////////
/*************************************************/
// Class ASin                                    //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> ASin::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(acosh_s(im), asin_s(re));
}

Expr ASin::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(asinh_s(im), acos_s(re));
}

optional<Expr> ASin::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return sum_s(prod_s(pow_s(acosh_s(im), CSL_2), pow_s(asin_s(re), CSL_2)),
                 prod_s(pow_s(asinh_s(im), CSL_2), pow_s(acos_s(re), CSL_2)));
}

optional<Expr> ASin::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void ASin::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "asinq(";
    else if (lib)
        out << "std::asin(";
    else
        out << "asin(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ASin::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\asin\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ASin::getLibDependency() const
{
    return LibDependency::cmath();
}

long double ASin::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::asin(argument->evaluateScalar());
}

optional<Expr> ASin::derive(Expr_info expr) const
{
    return prod_s(
        Derived(argument, expr),
        fraction_s(CSL_1,
                   pow_s(sum_s(CSL_1, prod_s(CSL_M_1, pow_s(argument, CSL_2))),
                         intfraction_s(1, 2))));
}

int ASin::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;

    return -1;
}

Expr asin_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    return csl::make_shared<ASin>(expr);
}

///////////////////////////////////////////////////
/*************************************************/
// Class ATan                                    //
/*************************************************/
///////////////////////////////////////////////////

void ATan::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << (argument->isReal() ? "" : "c") << "atanq(";
    else if (lib)
        out << "std::atan(";
    else
        out << "atan(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ATan::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\atan\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ATan::getLibDependency() const
{
    return LibDependency::cmath();
}

long double ATan::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::atan(argument->evaluateScalar());
}

optional<Expr> ATan::derive(Expr_info expr) const
{
    return fraction_s(Derived(argument, expr),
                      fraction_s(CSL_1, sum_s(CSL_1, pow_s(argument, CSL_2))));
}

int ATan::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr atan_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    Expr res = csl::make_shared<ATan>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Cosh                                    //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Cosh::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(cos_s(im), cosh_s(re));
}

Expr Cosh::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(sin_s(im), sinh_s(re));
}

optional<Expr> Cosh::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return sum_s(prod_s(pow_s(cos_s(im), CSL_2), pow_s(cosh_s(re), CSL_2)),
                 prod_s(pow_s(sin_s(im), CSL_2), pow_s(sinh_s(re), CSL_2)));
}

optional<Expr> Cosh::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void Cosh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "ccoshq(";
    else if (lib)
        out << "std::cosh(";
    else
        out << "cosh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Cosh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\cosh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Cosh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Cosh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::cosh(argument->evaluateScalar());
}

optional<Expr> Cosh::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), sinh_s(argument));
}

int Cosh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return 1;
}

Expr cosh_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_1;
    Expr res = csl::make_shared<Cosh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Sinh                                    //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Sinh::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(cos_s(im), sinh_s(re));
}

Expr Sinh::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return prod_s(sin_s(im), cosh_s(re));
}

optional<Expr> Sinh::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    return sum_s(prod_s(pow_s(cos_s(im), CSL_2), pow_s(sinh_s(re), CSL_2)),
                 prod_s(pow_s(sin_s(im), CSL_2), pow_s(cosh_s(re), CSL_2)));
}

optional<Expr> Sinh::getComplexArgument() const
{
    return csl::make_shared<Angle>(GetRealPart(copy()),
                                   GetImaginaryPart(copy()));
}

void Sinh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "csinhq(";
    else if (lib)
        out << "std::sinh(";
    else
        out << "sinh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Sinh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\sinh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Sinh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Sinh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::sinh(argument->evaluateScalar());
}

optional<Expr> Sinh::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr), cosh_s(argument));
}

int Sinh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr sinh_s(const Expr &expr)
{
    if (expr == CSL_0)
        return CSL_0;
    Expr res = csl::make_shared<Sinh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Tanh                                    //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Tanh::getRealPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    Expr modulus
        = sum_s(prod_s(pow_s(cos_s(im), CSL_2), pow_s(cosh_s(re), CSL_2)),
                prod_s(pow_s(sin_s(im), CSL_2), pow_s(sinh_s(re), CSL_2)));

    return fraction_s(prod_s(sinh_s(re), cosh_s(re)), modulus);
}

Expr Tanh::getImaginaryPart() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);
    Expr modulus
        = sum_s(prod_s(pow_s(cos_s(im), CSL_2), pow_s(cosh_s(re), CSL_2)),
                prod_s(pow_s(sin_s(im), CSL_2), pow_s(sinh_s(re), CSL_2)));

    return fraction_s(prod_s(sin_s(im), cos_s(im)), modulus);
}

optional<Expr> Tanh::getComplexModulus() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);

    Expr modulus
        = sum_s(prod_s(pow_s(cos_s(im), CSL_2), pow_s(cosh_s(re), CSL_2)),
                prod_s(pow_s(sin_s(im), CSL_2), pow_s(sinh_s(re), CSL_2)));

    Expr modulus2 = sum_s(pow_s(prod_s(sin_s(im), cos_s(im)), CSL_2),
                          pow_s(prod_s(sin_s(im), cos_s(im)), CSL_2));

    return fraction_s(modulus2, modulus);
}

optional<Expr> Tanh::getComplexArgument() const
{
    Expr re = GetRealPart(argument);
    Expr im = GetImaginaryPart(argument);

    return csl::make_shared<Angle>(prod_s(cosh_s(re), sinh_s(re)),
                                   prod_s(cos_s(im), sin_s(im)));
}

void Tanh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "ctanhq(";
    else if (lib)
        out << "std::tanh(";
    else
        out << "tanh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Tanh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\tanh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Tanh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Tanh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::tanh(argument->evaluateScalar());
}

optional<Expr> Tanh::derive(Expr_info expr) const
{
    return prod_s(
        Derived(argument, expr),
        sum_s(CSL_1, prod_s(CSL_M_1, pow_s(tanh_s(argument), CSL_2))));
}

int Tanh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr tanh_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    Expr res = csl::make_shared<Tanh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ACosh                                   //
/*************************************************/
///////////////////////////////////////////////////

void ACosh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "cacoshq(";
    else if (lib)
        out << "std::acosh(";
    else
        out << "acosh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ACosh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\acosh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ACosh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double ACosh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::acosh(argument->evaluateScalar());
}

optional<Expr> ACosh::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr),
                  fraction_s(CSL_1,
                             pow_s(sum_s(pow_s(argument, CSL_2), CSL_M_1),
                                   intfraction_s(1, 2))));
}

int ACosh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    return 0;
}

Expr acosh_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_1;
    Expr res = csl::make_shared<ACosh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ASinh                                   //
/*************************************************/
///////////////////////////////////////////////////

void ASinh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "casinhq(";
    else if (lib)
        out << "std::asinh(";
    else
        out << "asinh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ASinh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\asinh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ASinh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double ASinh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;

    return std::asinh(argument->evaluateScalar());
}

optional<Expr> ASinh::derive(Expr_info expr) const
{
    return prod_s(Derived(argument, expr),
                  fraction_s(CSL_1,
                             pow_s(sum_s(pow_s(argument, CSL_2), CSL_1),
                                   intfraction_s(1, 2))));
}

int ASinh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr asinh_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    Expr res = csl::make_shared<ASinh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ATanh                                   //
/*************************************************/
///////////////////////////////////////////////////

void ATanh::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "catanhq(";
    else if (lib)
        out << "std::atanh(";
    else
        out << "atanh(";
    argument->print(1, out, lib);
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string ATanh::printLaTeX(int mode) const
{
    ostringstream sout;

    sout << "\\atanh\\left(";
    sout << argument->printLaTeX(1);
    sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ATanh::getLibDependency() const
{
    return LibDependency::cmath();
}

long double ATanh::evaluateScalar() const
{
    if (argument->getDim() > 0)
        return 0;
    return std::atanh(argument->evaluateScalar());
}

optional<Expr> ATanh::derive(Expr_info expr) const
{
    return prod_s(
        Derived(argument, expr),
        fraction_s(CSL_1,
                   sum_s(CSL_1, prod_s(CSL_M_1, pow_s(argument, CSL_2)))));
}

int ATanh::getParity(Expr_info t_variable) const
{
    int parity = argument->getParity(t_variable);
    if (parity == 1)
        return 1;
    if (parity == 0)
        return 0;
    return -1;
}

Expr atanh_s(const Expr &expr)
{
    if (*expr == CSL_0)
        return CSL_0;
    Expr res = csl::make_shared<ATanh>(expr);

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Angle                                   //
/*************************************************/
///////////////////////////////////////////////////

void Angle::print(int mode, std::ostream &out, bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision())
        out << "catan2q(";
    else if (lib)
        out << "std::atan2(";
    else
        out << "angle(";
    if (lib) {
        argument[1]->print(1, out, lib);
        out << ", ";
        argument[0]->print(1, out, lib);
    }
    else {
        argument[0]->print(1, out, lib);
        out << ", ";
        argument[1]->print(1, out, lib);
    }
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

void Angle::printCode(int, std::ostream &out) const
{
    out << "csl::angle_s(";
    argument[0]->printCode(1, out);
    out << ", ";
    argument[1]->printCode(1, out);
    out << ")";
}

string Angle::printLaTeX(int) const
{
    ostringstream sout;
    sout << "\\text{angle}\\left(" << argument[0]->printLaTeX(1);
    sout << ", " << argument[1]->printLaTeX(1) << "\\right) ";

    return sout.str();
}

LibDependency Angle::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Angle::evaluateScalar() const
{
    long double a = argument[0]->evaluateScalar();
    long double b = argument[1]->evaluateScalar();

    return std::atan2(b, a);
}

optional<Expr> Angle::evaluate(csl::eval::mode user_mode) const
{
    auto opt_0 = argument[0]->evaluate(user_mode);
    auto opt_1 = argument[1]->evaluate(user_mode);
    auto arg0  = opt_0.value_or(argument[0]);
    auto arg1  = opt_1.value_or(argument[1]);

    if (csl::eval::isContained(user_mode, csl::eval::numerical)
        and arg0->getPrimaryType() == csl::PrimaryType::Numerical
        and arg1->getPrimaryType() == csl::PrimaryType::Numerical)
        return std::atan2(arg1->evaluateScalar(), arg0->evaluateScalar());

    if (opt_0 or opt_1)
        return angle_s(arg0, arg1);
    return std::nullopt;
}

unique_Expr Angle::copy_unique() const
{
    return make_unique<Angle>(argument[0], argument[1]);
}

Expr Angle::deepCopy() const
{
    return csl::make_shared<Angle>(argument[0]->deepCopy(),
                                   argument[1]->deepCopy());
}

Expr Angle::refresh() const
{
    return angle_s(argument[0]->refresh(), argument[1]->refresh());
}

Expr Angle::deepRefresh() const
{
    return angle_s(argument[0]->deepRefresh(), argument[1]->deepRefresh());
}

optional<Expr> Angle::derive(Expr_info expr) const
{
    if (!argument[0]->dependsOn(expr) and !argument[1]->dependsOn(expr))
        return CSL_0;

    return csl::make_shared<Derivative>(copy(), Copy(expr));
}

bool Angle::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != csl::Type::Angle)
        return false;
    return (*fraction_s(argument[0], argument[1])
            == fraction_s(expr->getArgument(0), expr->getArgument(1)));
}

int Angle::getParity(Expr_info t_variable) const
{
    int parity  = argument[0]->getParity(t_variable);
    int parity2 = argument[1]->getParity(t_variable);
    if (parity2 == 1)
        return parity;

    return 0;
}

Expr angle_s(Expr const &a, Expr const &b)
{
    if (b == CSL_0)
        return CSL_0;
    return csl::make_shared<Angle>(a, b);
}

///////////////////////////////////////////////////
/*************************************************/
// Class Factorial                               //
/*************************************************/
///////////////////////////////////////////////////

void Factorial::print(int mode, std::ostream &out, bool lib) const
{
    if (lib) {
        out << "csl::factorial(";
        argument->print(1, out, lib);
        out << ")";
    }
    else {
        out << argument;
        out << "!";
    }
    printProp(out);
    if (mode == 0)
        out << endl;
}

string Factorial::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << argument->printLaTeX(1);
    sout << "!";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency Factorial::getLibDependency() const
{
    return LibDependency::csl();
}

optional<Expr> Factorial::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical))
        return int_s(evaluateScalar());
    if (opt_arg)
        return factorial_s(arg);
    return std::nullopt;
}

unique_Expr Factorial::copy_unique() const
{
    return make_unique<Factorial>(argument);
}

Expr Factorial::deepCopy() const
{
    return csl::make_shared<Factorial>(argument->deepCopy());
}

Expr Factorial::refresh() const
{
    return copy();
}

Expr Factorial::deepRefresh() const
{
    return deepCopy();
}

long double Factorial::evaluateScalar() const
{
    double value = argument->evaluateScalar();
    CSL_ASSERT(value == round(value), CSLError::FloatingPointError);

    return factorial(value);
}

optional<Expr> Factorial::derive(Expr_info) const
{
    return CSL_0;
}

int Factorial::getParity(Expr_info) const
{
    return 0;
}

Expr factorial_s(const Expr &expr)
{
    if (expr->isInteger())
        return csl::make_shared<IntFactorial>(expr->evaluateScalar());
    return csl::make_shared<Factorial>(expr);
}

///////////////////////////////////////////////////
/*************************************************/
// Class DiracDelta                              //
/*************************************************/
///////////////////////////////////////////////////

DiracDelta::DiracDelta() : AbstractFunc()
{
}

DiracDelta::DiracDelta(const Expr &t_argument) : AbstractFunc(t_argument)
{
    Expand(argument, true);
    if (argument->getType() == csl::Type::Prod
        and argument->getArgument(0)->getPrimaryType()
                == csl::PrimaryType::Numerical
        and argument->getArgument(0)->evaluateScalar() < 0) {

        argument->setArgument(-argument->getArgument());
        argument = Expanded(-CSL_1 * argument, true);
    }
}

optional<Expr> DiracDelta::getRealPart() const
{
    return nullptr;
}

Expr DiracDelta::getImaginaryPart() const
{
    return CSL_0;
}

optional<Expr> DiracDelta::getComplexModulus() const
{
    return nullopt;
}

optional<Expr> DiracDelta::getComplexArgument() const
{
    return CSL_0;
}

void DiracDelta::print(int mode, std::ostream &out, bool) const
{
    out << "delta{";
    if (argument->getDim() > 0)
        out << argument->getName() << "-" << argument->getNArgs() << "d";
    else
        argument->print(1, out);
    out << "}";
    printProp(out);
    if (mode == 0)
        out << endl;
}

string DiracDelta::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "\\delta{";
    argument->print(1);
    sout << "}";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

optional<Expr> DiracDelta::derive(Expr_info expr) const
{
    CSL_ASSERT_SPEC(not argument->dependsOn(expr),
                    CSLError::NotImplementedError,
                    "the derivative of a dirac delta is not defined.");
    return nullopt;
}

int DiracDelta::getParity(Expr_info t_variable) const
{
    return argument->getParity(t_variable) != 0;
}

Expr DiracDelta::applyDiracDelta(const Expr &expr, const Expr &variable) const
{
    if (argument == CSL_0)
        return CSL_1;
    Expr polynomial = polynomial_s(Expanded(argument, true), variable);
    if (polynomial->getArgument(0)->dependsExplicitlyOn(variable.get())
        or polynomial->getNArgs() > 2) {
        polynomial->print();
        CALL_SMERROR_SPEC(CSLError::NotImplementedError,
                          "polynomial of degree higher than 2 in dirac "
                          "delta.");
    }

    ////////////
    // Ad hoc treatment of indicial objects (only order 1)
    // of type a*X(mu) + b(mu).
    ////////////
    if (IsIndicialTensor(variable)) {
        Expr rest = CSL_0;
        Expr fact = CSL_1;
        if (argument->getType() == csl::Type::Sum) {
            for (int i = 0; i != argument->getNArgs(); ++i) {
                if (argument->getArgument(i)->isPolynomial(variable.get())
                    > 0) {
                    fact = GetPolynomialTerm(
                        argument->getArgument(i), variable.get(), 1);
                    for (int j = 0; j != argument->getNArgs(); ++j)
                        if (i != j)
                            rest = rest - argument->getArgument(j);
                    break;
                }
            }
        }
        else if (argument->getType() == csl::Type::Prod) {
            fact = GetPolynomialTerm(argument, variable.get(), 1);
        }
        auto res = Replaced(expr, variable->getParent(), rest / fact);
        return res;
    }

    // Here we're sure that the argument of the DiracDelta is a polynomial
    // or order 1 max in the variable, the roots are well defined.

    Expr root       = -polynomial->getArgument(0) / polynomial->getArgument(1);
    Expr derivative = polynomial->getArgument(1);

    //(Replaced(expr,variable,root)/abs_s(derivative))->print();;
    return Replaced(expr, variable, root) / abs_s(derivative);
}

bool DiracDelta::operator==(Expr_info other) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(other); test != -1)
            return test;
    }
    if (other->getType() != csl::Type::DiracDelta)
        return false;

    Expr expr = DeepCopy(other);
    if (expr->isIndexed()
        and expr->getArgument()->getFreeIndexStructure().size() > 0) {
        IndexStructure freeStruct1
            = expr->getArgument()->getFreeIndexStructure();
        IndexStructure freeStruct2 = argument->getFreeIndexStructure();
        if (freeStruct1.size() != freeStruct2.size()) {
            return false;
        }
        Replace(expr, freeStruct1, freeStruct2);
        // for (size_t i = 0; i != freeStruct1.size(); ++i)
        //     if (freeStruct1[i] != freeStruct2[i])
        //         expr = ReplaceIndex(old2, freeStruct1[i], freeStruct2[i]);
    }

    bool res
        = (Comparator::freeIndexComparison(argument.get(),
                                           expr->getArgument().get())
           or Comparator::freeIndexComparison(
               argument.get(), Expanded((-1 * expr->getArgument())).get()));

    return res;
}

Expr diracdelta_s(const Expr &expr)
{
    // if (expr->getDim() > 0) {
    //     csl::vector_expr argument(0);
    //     for (int i = 0; i != expr->getNArgs(); ++i)
    //         argument.push_back(diracdelta_s(expr->getArgument(i)));
    //     return prod_s(argument,true);
    // }

    return csl::make_shared<DiracDelta>(expr);
}

///////////////////////////////////////////////////
//  Evaluation functions
///////////////////////////////////////////////////

std::optional<Expr> Exp::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::exp(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated(exp_s(a) * (cos_s(b) + CSL_I * sin_s(b)),
                              user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(exp_s(arg)) : std::nullopt;
}
std::optional<Expr> Log::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::log(arg->evaluateScalar());
        return csl::Evaluated(log_s(GetComplexModulus(arg))
                                  + CSL_I * GetComplexArgument(arg),
                              user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(log_s(arg)) : std::nullopt;
}
std::optional<Expr> Cos::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::cos(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated(
            cos_s(a) * cosh_s(b) - CSL_I * sin_s(a) * sinh_s(b), user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(cos_s(arg)) : std::nullopt;
}
std::optional<Expr> Sin::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::sin(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated(
            sin_s(a) * cosh_s(b) + CSL_I * cos_s(a) * sinh_s(b), user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(sin_s(arg)) : std::nullopt;
}
std::optional<Expr> Tan::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::tan(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated((sin_s(2 * a) + CSL_I * sinh_s(2 * b))
                                  / (cos_s(2 * a) + cosh_s(2 * b)),
                              user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(tan_s(arg)) : std::nullopt;
}
std::optional<Expr> ACos::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::acos(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(acos_s(arg)) : std::nullopt;
}
std::optional<Expr> ASin::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::asin(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(asin_s(arg)) : std::nullopt;
}
std::optional<Expr> ATan::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::atan(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(atan_s(arg)) : std::nullopt;
}
std::optional<Expr> Cosh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::cosh(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated(
            cosh_s(a) * cos_s(b) + CSL_I * sinh_s(a) * sin_s(b), user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(cosh_s(arg)) : std::nullopt;
}
std::optional<Expr> Sinh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::sinh(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated(
            sinh_s(a) * cos_s(b) + CSL_I * cosh_s(a) * sin_s(b), user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(sinh_s(arg)) : std::nullopt;
}
std::optional<Expr> Tanh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::tanh(arg->evaluateScalar());
        Expr a = GetRealPart(arg);
        Expr b = GetImaginaryPart(arg);
        return csl::Evaluated((sinh_s(2 * a) + CSL_I * sin_s(2 * b))
                                  / (cosh_s(2 * a) + cos_s(2 * b)),
                              user_mode);
    }
    return (opt_arg) ? std::optional<Expr>(tanh_s(arg)) : std::nullopt;
}
std::optional<Expr> ACosh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::acosh(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(acosh_s(arg)) : std::nullopt;
}
std::optional<Expr> ASinh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::asinh(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(asinh_s(arg)) : std::nullopt;
}
std::optional<Expr> ATanh::evaluate(csl::eval::mode user_mode) const
{
    auto opt_arg = argument->evaluate(user_mode);
    Expr arg     = opt_arg.value_or(argument);
    if (arg->getPrimaryType() == csl::PrimaryType::Numerical
        and csl::eval::isContained(user_mode, csl::eval::numerical)) {
        if (arg->isReal())
            return std::atanh(arg->evaluateScalar());
    }
    return (opt_arg) ? std::optional<Expr>(atanh_s(arg)) : std::nullopt;
}

} // End of namespace csl
