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

#include "cslcomplex.h"
#include "comparison.h"
#include "interface.h"
#include "librarydependency.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class RealPart                                //
/*************************************************/
///////////////////////////////////////////////////

RealPart::RealPart(): Operator<AbstractFunc>()
{
    argument = CSL_1;
}

RealPart::RealPart(const Expr& t_argument)
    :Operator<AbstractFunc>()
{
    argument = t_argument;
}

csl::Type RealPart::getType() const
{
    return csl::Type::RealPart;
}

bool RealPart::operatorAppliesOn(Expr_info arg) const
{
    return (not arg->isReal());
}

Expr RealPart::getOperand() const
{
    return argument;
}

void RealPart::setOperand(const Expr& arg)
{
    argument = arg;
}

void RealPart::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (lib) {
        out << "std::complex<long double>(";
        argument->print(1, out, lib);
        out << ").real()";
        if (mode == 0)
            out << endl;
        return;
    }
    out << "Re(";
    argument->print(1, out);
    out << ")";
    if (mode == 0)
        out << endl;
}

string RealPart::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "Re(";
    sout << argument->printLaTeX(1);
    sout << ")";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency RealPart::getLibDependency() const
{
    return LibDependency::complex();
}

optional<Expr> RealPart::evaluate(
        csl::eval::mode user_mode
        )  const
{
    optional<Expr> newArg = argument->evaluate(user_mode);
    if (not newArg)
        return nullopt;
    if (newArg.value()->isReal())
        return newArg.value();
    if (newArg.value()->isPurelyImaginary())
        return CSL_0;
    if (newArg.value()->getType() == csl::Type::Complex) {
        return newArg.value()->getRealPart().value();
    }

    return real_s(newArg.value());
}

unique_Expr RealPart::copy_unique() const
{
    unique_Expr res = make_unique<RealPart>(argument);
    res->setEmpty(empty);

    return res;
}

Expr RealPart::deepCopy() const
{
    Expr res = csl::make_shared<RealPart>(argument->deepCopy());
    res->setEmpty(empty);

    return res;
}

Expr RealPart::refresh() const
{
    Expr res = real_s(argument);
    res->setEmpty(empty);

    return res;
}

Expr RealPart::deepRefresh() const
{
    Expr res = real_s(argument->deepRefresh());
    res->setEmpty(empty);

    return res;
}


bool RealPart::isReal() const
{
    return true;
}

bool RealPart::isPurelyImaginary() const
{
    return false;
}

optional<Expr> RealPart::getComplexConjugate()  const
{
    return nullopt;
}

bool RealPart::operator==(Expr_info other) const
{
    if (int test = testDummy(other); test != -1)
        return test;
    if (other->getType() != csl::Type::RealPart) {
        if (other->getType() != csl::Type::ImaginaryPart)
            return false;
        return (other->getArgument() == CSL_I*argument);
    }
    return argument == other->getArgument();
}

Expr real_s(const Expr& expr)
{
    if (expr->isReal()) 
        return expr;
    if (expr->isPurelyImaginary()) 
        return CSL_0;
    return csl::make_shared<RealPart>()->applyOperator(expr);
}

///////////////////////////////////////////////////
/*************************************************/
// Class ImaginaryPart                           //
/*************************************************/
///////////////////////////////////////////////////

ImaginaryPart::ImaginaryPart(): Operator<AbstractFunc>()
{
    argument = CSL_1;
}

ImaginaryPart::ImaginaryPart(const Expr& t_argument)
    :Operator<AbstractFunc>()
{
    argument = t_argument;
}

csl::Type ImaginaryPart::getType() const
{
    return csl::Type::ImaginaryPart;
}

bool ImaginaryPart::operatorAppliesOn(Expr_info arg) const
{
    return (not arg->isReal());
}

Expr ImaginaryPart::getOperand() const
{
    return argument;
}

void ImaginaryPart::setOperand(const Expr& arg)
{
    argument = arg;
}

void ImaginaryPart::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (lib) {
        out << "std::complex<long double>(";
        argument->print(1, out, lib);
        out << ").imag()";
        if (mode == 0)
            out << endl;
        return;
    }
    out << "Im(";
    argument->print(1, out);
    out << ")";
    if (mode == 0)
        out << endl;
}

string ImaginaryPart::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "Im(";
    sout << argument->printLaTeX(1);
    sout << ")";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

LibDependency ImaginaryPart::getLibDependency() const
{
    return LibDependency::complex();
}

optional<Expr> ImaginaryPart::evaluate(
        csl::eval::mode user_mode
        )  const
{
    optional<Expr> newArg = argument->evaluate(user_mode);
    if (not newArg)
        return nullopt;
    if (newArg.value()->isPurelyImaginary())
        return newArg.value();
    if (newArg.value()->isReal())
        return CSL_0;
    if (newArg.value()->getType() == csl::Type::Complex) {
        return newArg.value()->getImaginaryPart();
    }

    return imaginary_s(newArg.value());
}

unique_Expr ImaginaryPart::copy_unique() const
{
    unique_Expr res = make_unique<ImaginaryPart>(argument);
    res->setEmpty(empty);

    return res;
}

Expr ImaginaryPart::deepCopy() const
{
    Expr res = csl::make_shared<ImaginaryPart>(argument->deepCopy());
    res->setEmpty(empty);

    return res;
}

Expr ImaginaryPart::refresh() const
{
    Expr res = imaginary_s(argument);
    res->setEmpty(empty);

    return res;
}

Expr ImaginaryPart::deepRefresh() const
{
    Expr res = imaginary_s(argument->deepRefresh());
    res->setEmpty(empty);

    return res;
}


bool ImaginaryPart::isReal() const
{
    return true;
}

bool ImaginaryPart::isPurelyImaginary() const
{
    return false;
}

optional<Expr> ImaginaryPart::getComplexConjugate()  const
{
    return nullopt;
}

bool ImaginaryPart::operator==(Expr_info other) const
{
    if (int test = testDummy(other); test != -1)
        return test;
    if (other->getType() != csl::Type::ImaginaryPart) {
        if (other->getType() != csl::Type::RealPart)
            return false;
        return (CSL_I*other->getArgument() == argument);
    }
    return argument == other->getArgument();
}

Expr imaginary_s(const Expr& expr)
{
    if (expr->isPurelyImaginary())
        return expr;
    if (expr->isReal())
        return CSL_0;
    return csl::make_shared<ImaginaryPart>()->applyOperator(expr);
}

Expr cconjugate_(const Expr& expr)
{
    return GetComplexConjugate(expr);    
}

} // End of namespace csl
