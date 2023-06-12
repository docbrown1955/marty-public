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

#include "commutation.h"
#include "comparison.h"
#include "interface.h"
#include "literal.h"
#include "numerical.h"
#include "operations.h"
#include "utils.h"
using namespace std;

namespace csl {

Expr Commutation(const Expr &A, const Expr &B, int sign)
{
    if ((A->size() == 0 and A->commutesWith(B.get(), sign))
        or (B->size() == 0 and B->commutesWith(A.get(), sign)))
        return CSL_0;
    if (not A->commutesWith(B.get(), sign))
        return CSL_UNDEF;
    if (sign == -1) {
        if (A->getCommutable()) {
            if (B->getCommutable())
                return CSL_0;
            else
                return prod_s(CSL_M_1, WeakCommutation(B, A));
        }
        else if (B->getCommutable())
            return WeakCommutation(A, B);

        // Here A and B are non-commutable objects
        // To implement...
        if (*A == B)
            return CSL_0;
    }
    else if (*Commutation(A, B) == CSL_0)
        return 2 * A * B;
    return CSL_UNDEF;
}

Expr Commutation(Expr_info A, Expr_info B, int)
{
    return Commutation(A->copy(), B->copy());
}

Expr WeakCommutation(const Expr &A, const Expr &B, int sign)
{
    if (sign == -1) {
        if (B->getPrimaryType() == csl::PrimaryType::Numerical
            or (B->getPrimaryType() == csl::PrimaryType::Literal
                and B->getType() != csl::Type::Variable))
            return CSL_0;
        if (A->isAnOperator()) {
            if (not A->isEmpty()) {
                if (*Commutation(A->getOperand(), B) == CSL_0)
                    return CSL_0;
                else
                    return CSL_UNDEF;
            }
            if (not A->operatorAppliesOn(B.get()))
                return CSL_0;
            else
                return CSL_UNDEF;
            // if (*Commutation(A->getArgument(), B) == CSL_0 and A->getOrder()
            // == 1)
            //    return derivative_s(A->getArgument()*B,A->getArgument(1), 1);
            return CSL_UNDEF;
        }
        switch (A->getType()) {

        // Warning here by default this function assumes [A,B] = 0
        default:
            return CSL_0;
        }
    }
    else {
        // Warning here by default this function assumes {A,B} = 2 A.B
        return 2 * A * B;
    }
}

Expr WeakCommutation(Expr_info A, Expr_info B, int)
{
    return WeakCommutation(Copy(A), Copy(B));
}

Commutator::Commutator() : AbstractDuoFunc(), sign(-1)
{
    argument[0] = CSL_0;
    argument[1] = CSL_0;
}

Commutator::Commutator(const Expr &A, const Expr &B, int t_sign)
    : AbstractDuoFunc(), sign(t_sign)
{
    argument[0] = A;
    argument[1] = B;
}

csl::Type Commutator::getType() const
{
    return csl::Type::Commutator;
}

int Commutator::getSign() const
{
    return sign;
}

void Commutator::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    if (libMode != LibraryMode::NoLib) {
        (argument[0] * argument[1] + sign * argument[1] * argument[0])
            ->print(mode, out, libMode);
        return;
    }
    if (sign == -1)
        out << "[ ";
    else
        out << "{ ";
    argument[0]->print(1, out);
    out << " , ";
    argument[1]->print(1, out);
    if (sign == -1)
        out << " ]";
    else
        out << " }";
    if (mode == 0)
        out << endl;
}

void Commutator::printLaTeX(int mode, std::ostream &out) const
{
    stringstream sout;
    if (sign == -1)
        sout << "\\left[ ";
    else
        sout << "\\left\\lbrace ";
    argument[0]->printLaTeX(1, out);
    out << ",\\ ";
    argument[1]->printLaTeX(1, out);
    if (sign == -1)
        sout << " \\right]";
    else
        sout << " \\right\\rbrace";
    if (mode == 0)
        sout << endl;
}

long double Commutator::evaluateScalar() const
{
    Expr com = Commutation(argument[0], argument[1]);
    if (operator==(com.get()))
        com = Evaluated(argument[0] * argument[1]
                        + sign * argument[1] * argument[0]);

    return com->evaluateScalar();
}

optional<Expr> Commutator::evaluate(csl::eval::mode user_mode) const
{
    Expr com = Commutation(argument[0], argument[1]);
    if (operator!=(com) and not(*com == CSL_UNDEF))
        return com->evaluate(user_mode);
    else
        return (argument[0] * argument[1] + sign * argument[1] * argument[0])
            ->evaluate(user_mode);
}

unique_Expr Commutator::copy_unique() const
{
    return make_unique<Commutator>(argument[0], argument[1], sign);
}

Expr Commutator::deepCopy() const
{
    return csl::make_shared<Commutator>(
        argument[0]->deepCopy(), argument[1]->deepCopy(), sign);
}

Expr Commutator::refresh() const
{
    return commutator_(argument[0], argument[1], sign);
}

Expr Commutator::deepRefresh() const
{
    return commutator_(
        argument[0]->deepRefresh(), argument[1]->deepRefresh(), sign);
}

optional<Expr> Commutator::derive(Expr_info expr) const
{
    return commutator_(Derived(argument[0], expr), argument[1], sign)
           + commutator_(argument[0], Derived(argument[1], expr), sign);
}

int Commutator::getParity(Expr_info variable) const
{
    int parity = argument[0]->getParity(variable);
    if (parity == argument[1]->getParity(variable))
        return parity;
    return 0;
}

bool Commutator::operator==(Expr_info expr) const
{
    if (int test = testDummy(expr); test != -1)
        return test;
    if (expr->getType() != csl::Type::Commutator) {
        if ((*argument[0] == CSL_0 or *argument[1] == CSL_0)
            and *expr == CSL_0)
            return true;
        else
            return false;
    }

    return argument[0]->    operator==(expr->getArgument(0))
           and argument[1]->operator==(expr->getArgument(1));
}

Expr commutator_(const Expr &A, const Expr &B, int sign)
{
    Expr com = Commutation(A, B, sign);
    if (*com == CSL_0)
        return com;

    // Getting all commutating Objects out of the commutator
    if (A->getType() == csl::Type::Prod) {
        Expr             left  = CSL_1;
        Expr             right = CSL_1;
        csl::vector_expr A_left(0);
        csl::vector_expr A_right = A->getVectorArgument();
        A_right.erase(A_right.begin());
        int nArgs = A->getNArgs();
        for (int i = 0; i != nArgs; ++i) {
            Expr arg = A->getArgument(i);
            if (arg->commutesWith(B.get())) {
                if (arg->commutesWith(prod_s(A_left, true).get()))
                    left = left * arg;
                else if (arg->commutesWith(prod_s(A_right, true).get()))
                    right = arg * right;
                else {
                    A_left.push_back(arg);
                }
            }
            else
                A_left.push_back(arg);
        }
        if (left != CSL_1 or right != CSL_1)
            return left * commutator_(prod_s(A_left), B, sign) * right;
    }
    if (B->getType() == csl::Type::Prod) {
        Expr             left  = CSL_1;
        Expr             right = CSL_1;
        csl::vector_expr B_left(0);
        csl::vector_expr B_right = B->getVectorArgument();
        B_right.erase(B_right.begin());
        int nArgs = B->getNArgs();
        for (int i = 0; i != nArgs; ++i) {
            Expr arg = B->getArgument(i);
            if (arg->commutesWith(A.get())) {
                if (arg->commutesWith(prod_s(B_left, true).get()))
                    left = left * arg;
                else if (arg->commutesWith(prod_s(B_right, true).get()))
                    right = arg * right;
                else
                    B_left.push_back(arg);
            }
            else
                B_left.push_back(arg);
        }
        if (left != CSL_1 or right != CSL_1)
            return left * commutator_(A, prod_s(B_left), sign) * right;
    }

    // Applying [AB,C] = A[B,C] + [A,C]B for sign == -1
    if (sign == -1) {
        if (A->getType() == csl::Type::Prod) {
            csl::vector_expr vec_A = A->getVectorArgument();
            Expr             left  = vec_A[0];
            vec_A.erase(vec_A.begin());
            Expr right = prod_s(vec_A, true);
            return left * commutator_(right, B) + commutator_(left, B) * right;
        }
        else if (B->getType() == csl::Type::Prod) {
            csl::vector_expr vec_B = B->getVectorArgument();
            Expr             left  = vec_B[0];
            vec_B.erase(vec_B.begin());
            Expr right = prod_s(vec_B, true);
            return left * commutator_(A, right) + commutator_(A, left) * right;
        }
    }

    return csl::make_shared<Commutator>(A, B, sign);
}
} // End of namespace csl
