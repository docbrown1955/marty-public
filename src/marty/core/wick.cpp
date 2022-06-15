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

#include "wick.h"
#include "graph.h"
#include "mrtError.h"
#include "mrtUtils.h"

using namespace std;
using namespace csl;

namespace mty {

Wick::Wick() : Operator<AbstractFunc>()
{
    argument = CSL_1;
}

Wick::Wick(const csl::Expr &expr) : Operator<AbstractFunc>()
{
    argument = expr;
    checkRightForm();
    expandToRightForm();
}

int Wick::numberOfFields(const csl::Expr &expr)
{
    if (IsOfType<QuantumField>(expr))
        return 1;
    if (expr->getType() == csl::Type::Pow
        and IsOfType<QuantumField>(expr->getArgument(0))
        and expr->getArgument(1)->isInteger())
        return (int) (expr->getArgument(1)->evaluateScalar());

    return -1;
}

void Wick::checkRightForm()
{
    if (IsOfType<QuantumField>(argument))
        rightForm = true;
    else if (argument->getType() == csl::Type::Prod) {
        rightForm = true;
        for (const auto &arg : argument->getVectorArgument())
            if (not IsOfType<QuantumField>(arg)) {
                if (not(arg->getType() == csl::Type::Pow)
                    or not arg->getArgument(1)->isInteger()
                    or not IsOfType<QuantumField>(arg->getArgument(0))) {
                    rightForm = false;
                    break;
                }
            }
    }
    else if (argument->getType() == csl::Type::Pow) {
        if (not argument->getArgument(1)->isInteger()
            or not IsOfType<QuantumField>(argument->getArgument(0))) {
            rightForm = false;
            return;
        }
        rightForm = true;
    }
    else
        rightForm = false;
}

void Wick::expandToRightForm()
{
    if (not rightForm)
        return;
    if (IsOfType<QuantumField>(argument))
        return;
    if (argument->getType() == csl::Type::Pow) {
        if (IsOfType<QuantumField>(argument->getArgument(0))
            and argument->getArgument(1)->isInteger()
            and argument->getArgument(1)->evaluateScalar() > 0) {
            csl::vector_expr foo(argument->getArgument(1)->evaluateScalar(),
                                 argument->getArgument(0));
            argument = prod_s(foo, true);
            return;
        }
    }
    if (argument->getType() == csl::Type::Prod) {
        csl::vector_expr foo(0);
        for (const auto &arg : argument->getVectorArgument()) {
            if (IsOfType<QuantumField>(arg))
                foo.push_back(arg);
            else if (arg->getType() == csl::Type::Pow) {
                if (IsOfType<QuantumField>(arg->getArgument(0))
                    and arg->getArgument(1)->isInteger()
                    and arg->getArgument(1)->evaluateScalar() > 0) {
                    csl::vector_expr bar(arg->getArgument(1)->evaluateScalar(),
                                         arg->getArgument(0));
                    foo.insert(foo.end(), bar.begin(), bar.end());
                }
            }
            else
                return;
        }
        argument = prod_s(foo, true);
    }
}

bool Wick::checkEvenNumberOfFields() const
{
    int number = 0;
    if (argument->getType() == csl::Type::Prod) {
        for (const auto &arg : argument->getVectorArgument()) {
            int n = numberOfFields(arg);
            if (n == -1)
                return false;
            number += n;
        }
    }
    else
        number = numberOfFields(argument);

    if (number % 2 == 1)
        return false;
    return true;
}

csl::Type Wick::getType() const
{
    return csl::Type::InheritanceType;
}

void Wick::print(int mode, std::ostream &out, bool) const
{
    out << "Wick< ";
    argument->print(1, out);
    out << " >";
    if (mode == 0)
        out << endl;
}

void Wick::printCode(int mode, std::ostream &out) const
{
    argument->printCode(mode, out);
}

string Wick::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "Wick< ";
    sout << argument->printLaTeX(1);
    sout << " >";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

optional<csl::Expr> Wick::evaluate(csl::eval::mode) const
{
    return nullopt;
}

csl::Expr Wick::getOperand() const
{
    return argument;
}

void Wick::setOperand(const csl::Expr &operand)
{
    argument = operand;
    checkRightForm();
}

bool Wick::operatorAppliesOn(csl::Expr_info expr) const
{
    if (IsOfType<QuantumField>(expr))
        return true;

    if (expr->getNArgs() > 0) {
        for (int i = 0; i != expr->getNArgs(); ++i)
            if (this->operatorAppliesOn(expr->getArgument(i).get()))
                return true;
    }
    if (expr->isAnOperator() and operatorAppliesOn(expr->getOperand().get()))
        return true;
    return false;
}

bool Wick::operator==(csl::Expr_info other) const
{
    if (other->getName() == Comparator::dummyName())
        return other->operator==(this);

    if (not IsOfType<Wick>(other))
        return false;
    return (argument->operator==(other->getArgument()));
}

csl::unique_Expr Wick::copy_unique() const
{
    return make_unique<Wick>(argument);
}

csl::Expr Wick::deepCopy() const
{
    if (rightForm)
        return copy();
    return csl::make_shared<Wick>(DeepCopy(argument));
}

csl::Expr Wick::refresh() const
{
    if (rightForm)
        return copy();
    return wick_s(argument);
}

csl::Expr Wick::deepRefresh() const
{
    if (rightForm)
        return copy();
    return wick_s(DeepRefreshed(argument));
}

csl::Expr wick_s(const csl::Expr &expr)
{
    Wick      w;
    csl::Expr foo = w.applyOperator(expr);
    if (not IsOfType<Wick>(foo))
        return foo;
    if (foo->getOperand() == 0)
        return CSL_0;
    if (foo->getOperand()->isAnOperator()) {
        // Wick commutes here with every operator !
        csl::Expr res = Copy(foo->getOperand());
        res->setOperand(wick_s(res->getOperand()));
        return DeepRefreshed(res);
    }

    return foo;
}

} // End of namespace mty
