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

#include "error.h"
#include "indicial.h"
#include "interface.h"
#include "literal.h"
#include "mathFunctions.h"
#include "numerical.h"
#include "operations.h"
#include "property.h"
#include "simplification.h"
#include "utils.h"
#ifdef DEBUG
#include <chrono>
#endif
using namespace std;

namespace csl {

bool      RANDOM_SEED                 = false;
const int NUMBER_OF_MUTATIONS         = 7;
const int NATURAL_SELECTION           = 3;
const int NUMBER_OF_GENERATION        = 11;
const int GENERATION_BEFORE_SELECTION = 5;
const int SIMPLIFICATION_METHOD       = 1;
const int MAX_ALTERNATE_FORMS         = 10;
const int MAX_RECURSION_ALTERNATE     = 7;

void printVector(const csl::vector_expr &vector)
{
    for (const auto &arg : vector)
        arg->print();
}

void addAlternateForm(csl::vector_expr &alternateForms,
                      const Expr &      t_newAlternate,
                      bool              add_factor_expand)
{
    Expr newAlternate = DeepRefreshed(t_newAlternate);
    bool found        = false;
    for (const auto &al : alternateForms)
        if (*al == newAlternate) {
            found = true;
            break;
        }
    if (alternateForms.size() == 0 or not found) {
        alternateForms.push_back(newAlternate);
        if (add_factor_expand) {
            Expr foo = Expanded(DeepCopy(newAlternate));
            if (*foo != newAlternate)
                addAlternateForm(alternateForms, foo, false);
            foo = Factored(DeepCopy(newAlternate));
            if (*foo != newAlternate)
                addAlternateForm(alternateForms, foo, false);
        }
    }
}

// Reduces the number of alternate forms to MAX_ALTERNATE_FORMS
// Take only the bests (simpler) exprs
void reduceAlternate(csl::vector_expr &alternateForms)
{
    int nArgs = alternateForms.size();
    if (nArgs <= MAX_ALTERNATE_FORMS)
        return;
    // partial_sort(alternateForms.begin(),
    // alternateForms.begin()+MAX_ALTERNATE_FORMS, alternateForms.end());
    for (int i = 0; i < MAX_ALTERNATE_FORMS; ++i) {
        int iMin = i;
        for (size_t j = i + 1; j != alternateForms.size(); ++j)
            if (*alternateForms[j] < alternateForms[iMin])
                iMin = j;
        if (iMin != i)
            swap(alternateForms[i], alternateForms[iMin]);
    }
    alternateForms.erase(alternateForms.begin() + MAX_ALTERNATE_FORMS,
                         alternateForms.end());
}

csl::vector_expr applyProperties(const csl::vector_expr &alternateForms)
{
    csl::vector_expr toReturn(alternateForms);
    for (const auto &expr : alternateForms) {
        csl::vector_expr newForms = PROPERTIES.apply(expr);
        toReturn.insert(toReturn.end(), newForms.begin(), newForms.end());
    }

    reduceAlternate(toReturn);
    return toReturn;
}

void clearRedundancyAlternate(csl::vector_expr &alternateForms)
{
    for (size_t i = 0; i != alternateForms.size(); ++i)
        for (size_t j = i + 1; j != alternateForms.size(); ++j)
            if (*alternateForms[i] == alternateForms[j]) {
                alternateForms.erase(alternateForms.begin() + j);
                --j;
            }
}

csl::vector_expr getRecursiveAlternateForms(const Expr &expr, int depth)
{
    csl::vector_expr alternateForms;
    // taking alternateForms a first time
    csl::vector_expr toReturn
        = internalRecursiveAlternateForms(expr, depth - 1);
    reduceAlternate(toReturn);
    if (toReturn.size() == 0) // no alternate form for expr
        return expr->getAlternateForms();
    alternateForms = toReturn;
    // We take alternate of alternate MAX_RECURSION_ALTERNATE-1 times
    // cout<<"Alternated of "; expr->print();
    for (int i = 1; i < MAX_RECURSION_ALTERNATE; i++) {
        toReturn = csl::vector_expr(0);
        csl::vector_expr fooVec(0);
        Expr             foo;
        for (size_t j = 0; j < alternateForms.size(); j++) {
            // taking alternate of alternate
            fooVec = internalRecursiveAlternateForms(alternateForms[j],
                                                     depth - 1);
            if (fooVec.size() == 0)
                fooVec.push_back(DeepCopy(alternateForms[j]));

            size_t fooSize = toReturn.size();
            for (size_t k = 0; k < fooVec.size(); ++k) {
                if (*fooVec[k] == CSL_0)
                    return csl::vector_expr(1, CSL_0);
                addAlternateForm(toReturn, fooVec[k]);
                if (fooSize < toReturn.size())
                    fooSize = toReturn.size();
                else {
                    fooVec.erase(fooVec.begin() + k);
                    --k;
                }
            }
            // If there is no new alternate
            if (fooVec.size() == 0)
                return applyProperties(toReturn);
        }
        alternateForms = fooVec;
        reduceAlternate(alternateForms);
    }

    return applyProperties(toReturn);
}

csl::vector_expr internalRecursiveAlternateForms(const Expr &expr, int depth)
{
    int              nArgs = expr->getNArgs();
    csl::vector_expr alternateForms(0);
    // if no Argument, no alternate form (for now)
    if (nArgs == 0) {
        csl::vector_expr toReturn = expr->getAlternateForms();
        if (toReturn.size() == 0)
            return csl::vector_expr(1, expr);
        else
            return toReturn;
    }
    if (depth != 0) {
        csl::vector_expr argument(0);
        ////
        // Warning: here specific if-else conditions on types,
        // could be wrong if changes occur in structure....
        ////
        if (nArgs == 1)
            argument = csl::vector_expr(1, expr->getArgument());
        else if (expr->getPrimaryType() == csl::PrimaryType::Vectorial
                 or expr->getType() == csl::Type::Sum
                 or expr->getType() == csl::Type::Prod
                 or expr->getType() == csl::Type::Polynomial)
            argument = expr->getVectorArgument();
        else if (nArgs == 2) {
            argument    = csl::vector_expr(2);
            argument[0] = expr->getArgument(0);
            argument[1] = expr->getArgument(1);
        }
        else
            callError(cslError::UndefinedBehaviour,
                      "internalRecursiveAlternateForms(const Expr& expr, int "
                      "depth)");

        csl::vector_expr fooVec;
        Expr             foo;
        for (int i = 0; i < nArgs; i++) {
            fooVec = internalRecursiveAlternateForms(argument[i], depth - 1);
            for (size_t j = 0; j < fooVec.size(); j++)
            // We take 1+n-1 copies of actual alternates
            {
                foo = DeepCopy(expr);
                foo->setArgument(DeepRefreshed(fooVec[j]), i);
                addAlternateForm(alternateForms, DeepRefreshed(foo));
            }
            reduceAlternate(alternateForms);
        }

        // cout<<"AFTER ALL ARGS: \n";
        // printVector(alternateForms);
        csl::vector_expr toReturn(0);
        for (size_t i = 0; i < alternateForms.size(); i++) {
            alternateForms[i] = DeepRefreshed(alternateForms[i]);
            if (*alternateForms[i] == CSL_0)
                return csl::vector_expr(1, CSL_0);
            fooVec = alternateForms[i]->getAlternateForms();
            if (fooVec.size() == 0)
                fooVec.push_back(alternateForms[i]);
            for (size_t j = 0; j < fooVec.size(); j++)
                addAlternateForm(toReturn, DeepRefreshed(fooVec[j]));
        }
        reduceAlternate(toReturn);

        return toReturn;
    }
    csl::vector_expr toReturn = expr->getAlternateForms();
    if (toReturn.size() == 0)
        return csl::vector_expr(1, expr);
    else
        return toReturn;
}

Expr Simplify(const Expr &expr, int depth)
{
    int              nArgs = expr->getNArgs();
    csl::vector_expr trials;
    Expr             simplifiedAbstract;
    simplifiedAbstract = DeepCopy(expr);

    if (depth != 0 and nArgs > 0) {
        for (int i = 0; i < nArgs; i++)
            simplifiedAbstract->setArgument(
                Simplify(simplifiedAbstract->getArgument(i)), i);
        simplifiedAbstract = DeepRefreshed(simplifiedAbstract);
    }

    int  iter                  = 0;
    int  maxIter               = 50;
    bool simplified            = true;
    bool checkIndexExpressions = expr->isIndexed();
    // cout<<"Simplifying "; expr->print();
    while (simplified and iter < maxIter) {
        simplified = false;

        trials = getRecursiveAlternateForms(simplifiedAbstract, depth);
        if (checkIndexExpressions)
            for (size_t i = 0; i < trials.size(); i++) {
                if (*trials[i] == -1 * simplifiedAbstract)
                    simplifiedAbstract = CSL_0;
            }
        if (simplifiedAbstract == CSL_0)
            break;
        for (size_t i = 0; i < trials.size(); i++) {
            trials[i] = DeepRefreshed(trials[i]);
            if (*trials[i] < simplifiedAbstract) {
                simplified         = true;
                simplifiedAbstract = trials[i];
            }
        }
        iter++;
    }

    return simplifiedAbstract;
}

csl::vector_expr Sum::getAlternateForms() const
{
    Expr foo = DeepCopy(this);
    Expr foo2;
    Expr foo3, foo4;
    foo4 = DeepCopy(this);
    csl::vector_expr alternateForms(0);
    csl::vector_expr alternateArg(0);

    // Sum-specific alternate forms
    int              nArgs2;
    csl::vector_expr factors(0);
    for (size_t i = 0; i < size(); i++) {
        foo = DeepCopy(this);
        if (argument[i]->getType() == csl::Type::Prod) {
            nArgs2 = argument[i]->getNArgs();
            for (int j = 0; j < nArgs2; j++)
                factors.push_back(DeepCopy(argument[i]->getArgument(j)));
        }
        else
            factors.push_back(DeepCopy(argument[i]));

        if (argument[i]->getType()
            == csl::Type::Prod) // Searching fraction => same denominator
        {
            nArgs2 = argument[i]->getNArgs();
            for (int j = 0; j < nArgs2; j++) {
                foo2 = DeepCopy(argument[i]->getArgument(j));
                if (foo2->getType() == csl::Type::Pow) {
                    if (foo2->getArgument(1)->isInteger()
                        and foo2->getArgument(1)->evaluateScalar() < 0) {
                        int power
                            = -1 * foo2->getArgument(1)->evaluateScalar();
                        foo2->setArgument(int_s(power), 1);
                        for (size_t k = 0; k < argument.size(); k++)
                            foo->setArgument(
                                prod_s(DeepCopy(foo2), DeepCopy(argument[k])),
                                k);
                        foo2->setArgument(int_s(-power), 1);

                        foo  = DeepRefreshed(foo);
                        foo3 = prod_s(foo2, Expanded(DeepCopy(foo), 1));
                        if (*foo3 != foo4)
                            addAlternateForm(alternateForms, foo3, false);
                        foo3 = prod_s(foo2, Expanded(DeepCopy(foo)));
                        if (*foo3 != foo4)
                            addAlternateForm(alternateForms, foo3, false);
                        foo3 = prod_s(foo, foo2);
                        if (*foo3 != foo4)
                            addAlternateForm(alternateForms, foo3, false);
                    }
                }
            }
        }
        else if (argument[i]->getType()
                 == csl::Type::Pow) // Searching fraction => same denominator
        {
            foo2 = DeepCopy(argument[i]);
            if (foo2->getArgument(1)->isInteger()
                and foo2->getArgument(1)->evaluateScalar() < 0) {
                int power = -1 * foo2->getArgument(1)->evaluateScalar();
                foo2->setArgument(int_s(power), 1);
                for (size_t k = 0; k < argument.size(); k++)
                    foo->setArgument(
                        prod_s(DeepCopy(foo2), DeepCopy(argument[k])), k);
                foo2->setArgument(int_s(-power), 1);

                foo  = DeepRefreshed(foo);
                foo3 = prod_s(foo2, Expanded(DeepCopy(foo), 1));
                if (*foo3 != foo4)
                    addAlternateForm(alternateForms, foo3, false);
                foo3 = prod_s(foo2, Expanded(DeepCopy(foo)));
                if (*foo3 != foo4)
                    addAlternateForm(alternateForms, foo3, false);
                foo3 = prod_s(foo, foo2);
                if (*foo3 != foo4)
                    addAlternateForm(alternateForms, foo3, false);
            }
        }
        if (false and argument[i]->getType() == csl::Type::Prod
            and argument[i]->getNArgs() == 2) {
            Expr number = argument[i]->getArgument(0);
            if (number->isInteger()
                and abs(number->evaluateScalar()) < 10) // n*X, n integer
            {
                int  value   = number->evaluateScalar();
                Expr arg     = DeepCopy(argument[i]->getArgument(1));
                alternateArg = argument;
                alternateArg.erase(alternateArg.begin() + i);
                if (value > 0) {
                    for (int j = 0; j < value; j++)
                        alternateArg.push_back(arg);
                }
                else if (value < 0) {
                    arg = minus_(CSL_0, arg);
                    for (int j = 0; j < abs(value); j++)
                        alternateArg.push_back(arg);
                }
                if (isIndexed())
                    foo3 = csl::make_shared<ISum>();
                else
                    foo3 = csl::make_shared<Sum, alloc_sum>();
                foo3->setVectorArgument(alternateArg);
                addAlternateForm(alternateForms, foo3);
            }
        }
    }
    foo = DeepCopy(this);

    for (size_t i = 0; i != factors.size(); ++i) {
        if (factors[i]->getPrimaryType() == csl::PrimaryType::Numerical)
            continue;
        bool factorOk = true;
        for (const auto &arg : argument)
            if (not arg->askTerm(factors[i].get())) {
                factorOk = false;
                break;
            }
        if (not factorOk)
            continue;
        // cout<<"Simplifying with factor "; factors[i]->print();
        Expr factored = Factored(DeepCopy(foo), factors[i].get());
        if (*factored != foo)
            addAlternateForm(alternateForms, factored, false);
    }

    return alternateForms;
}

csl::vector_expr Prod::getAlternateForms() const
{
    Expr             foo = DeepCopy(this);
    Expr             foo2;
    csl::vector_expr alternateForms(0);
    csl::vector_expr alternateArg(0);

    alternateForms    = csl::vector_expr(0);
    bool cosProdFound = false;
    for (size_t i = 0; i < argument.size(); i++) {
        csl::Type type = argument[i]->getType();
        if (not cosProdFound and type == csl::Type::Cos) {
            for (size_t j = i + 1; j < argument.size(); j++) {
                type = argument[j]->getType();
                if (type
                    == csl::Type::Cos) // cos(a)cos(b)=cos(a+b)+sin(a)sin(b)
                {
                    cosProdFound = true;
                    Expr a       = argument[i]->getArgument();
                    Expr b       = argument[j]->getArgument();
                    foo2         = DeepCopy(this);
                    foo2->setArgument(CSL_1, i);
                    foo2->setArgument(CSL_1, j);
                    foo2 = prod_s(
                        foo2,
                        sum_s(cos_s(sum_s(a, b)), prod_s(sin_s(a), sin_s(b))));
                    alternateForms.push_back(foo2);
                }
                else if (type == csl::Type::Sin) // cos(a)sin(b) =
                                                 // sin(a+b)-sin(a)cos(b)
                {
                    cosProdFound = true;
                    Expr a       = argument[i]->getArgument();
                    Expr b       = argument[j]->getArgument();
                    foo2         = DeepCopy(this);
                    foo2->setArgument(CSL_1, i);
                    foo2->setArgument(CSL_1, j);
                    foo2 = prod_s(foo2,
                                  minus_(sin_s(sum_s(a, b)),
                                         prod_s(sin_s(a), cos_s(b))));
                    alternateForms.push_back(foo2);
                }
            }
        }
        else if (not cosProdFound and type == csl::Type::Sin) {
            for (size_t j = i + 1; j < argument.size(); j++) {
                type = argument[j]->getType();
                if (type
                    == csl::Type::Sin) // sin(a)sin(b)=cos(a)cos(b)-cos(a+b)
                {
                    cosProdFound = true;
                    Expr a       = argument[i]->getArgument();
                    Expr b       = argument[j]->getArgument();
                    foo2         = DeepCopy(this);
                    foo2->setArgument(CSL_1, i);
                    foo2->setArgument(CSL_1, j);
                    foo2 = prod_s(foo2,
                                  minus_(prod_s(cos_s(a), cos_s(b)),
                                         cos_s(sum_s(a, b))));
                    alternateForms.push_back(foo2);
                }
                else if (type == csl::Type::Cos) // cos(b)sin(a) =
                                                 // sin(a+b)-sin(b)cos(a)
                {
                    cosProdFound = true;
                    Expr a       = argument[i]->getArgument();
                    Expr b       = argument[j]->getArgument();
                    foo2         = DeepCopy(this);
                    foo2->setArgument(CSL_1, i);
                    foo2->setArgument(CSL_1, j);
                    foo2 = prod_s(foo2,
                                  minus_(sin_s(sum_s(a, b)),
                                         prod_s(sin_s(b), cos_s(a))));
                    alternateForms.push_back(foo2);
                }
            }
        }
    }
    for (size_t i = 0; i != alternateForms.size(); i++) {
        // We add automatically expandped alternate form
        foo = Expanded(alternateForms[i]);
        if (*foo != alternateForms[i])
            alternateForms.push_back(foo);

        foo = Expanded(alternateForms[i], 1);
        if (*foo != alternateForms[i])
            alternateForms.push_back(foo);
        foo = Factored(alternateForms[i]);
        if (*foo != alternateForms[i])
            alternateForms.push_back(foo);
    }
    if (alternateForms.size() > 0)
        alternateForms.insert(alternateForms.begin(), DeepCopy(this));

    return alternateForms;
}

csl::vector_expr Pow::getAlternateForms() const
{
    csl::vector_expr alternateForms(0);
    if (argument[0]->getType() == csl::Type::Cos) {
        if (*argument[1] == 2) // cos(x)^2
        {
            alternateForms.push_back(
                minus_(CSL_1,
                       pow_s(sin_s(argument[0]->getArgument()),
                             CSL_2))); // 1-sin(x)^2
            alternateForms.push_back(prod_s(
                sum_s(CSL_1, cos_s(prod_s(CSL_2, argument[0]->getArgument()))),
                intfraction_s(1, 2))); // (1+cos(2x))/2
        }
        else if (*argument[1] == -2) // 1/cos(x)^2
            alternateForms.push_back(
                sum_s(CSL_1,
                      pow_s(tan_s(argument[0]->getArgument()),
                            CSL_2))); // = 1 + tan(x)^2
    }
    else if (argument[0]->getType() == csl::Type::Sin) // sin(x)^2
    {
        if (*argument[1] == 2) {
            alternateForms.push_back(
                minus_(CSL_1,
                       pow_s(cos_s(argument[0]->getArgument()),
                             CSL_2))); // = 1-cos(x)^2
            alternateForms.push_back(prod_s(
                minus_(CSL_1,
                       cos_s(prod_s(CSL_2, argument[0]->getArgument()))),
                intfraction_s(1, 2))); // (1-cos(2x))/2
        }
    }
    if (argument[0]->getType() == csl::Type::Cosh) {
        if (*argument[1] == 2) // cosh(x)^2
        {
            alternateForms.push_back(
                minus_(CSL_1,
                       pow_s(sinh_s(argument[0]->getArgument()),
                             CSL_2))); // 1-sinh(x)^2
            alternateForms.push_back(prod_s(
                sum_s(CSL_1,
                      cosh_s(prod_s(CSL_2, argument[0]->getArgument()))),
                intfraction_s(1, 2))); // (1+cosh(2x))/2
        }
        else if (*argument[1] == -2) // 1/cosh(x)^2
            alternateForms.push_back(
                sum_s(CSL_1,
                      pow_s(tanh_s(argument[0]->getArgument()),
                            CSL_2))); // = 1 + tanh(x)^2
    }
    else if (argument[0]->getType() == csl::Type::Sinh) // sinh(x)^2
    {
        if (*argument[1] == 2) {
            alternateForms.push_back(
                minus_(CSL_1,
                       pow_s(cosh_s(argument[0]->getArgument()),
                             CSL_2))); // = 1-cosh(x)^2
            alternateForms.push_back(prod_s(
                minus_(cosh_s(prod_s(CSL_2, argument[0]->getArgument())),
                       CSL_1),
                intfraction_s(1, 2))); // (cosh(2x)-1)/2
        }
    }
    if (alternateForms.size() > 0)
        alternateForms.insert(alternateForms.begin(), DeepCopy(this));

    return alternateForms;
}

csl::vector_expr Cos::getAlternateForms() const
{
    csl::vector_expr foo(0);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // cos(a+b)
    {
        Expr a = argument->getArgument(0);
        Expr b = argument->getArgument(1);
        foo.push_back(
            minus_(prod_s(cos_s(a), cos_s(b)),
                   prod_s(sin_s(a), sin_s(b)))); // = cos(a)cos(b)-sin(a)sin(b)
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and argument->getArgument(0) == CSL_2) // cos(2a)
    {
        Expr a = argument->getArgument(1);
        foo.push_back(minus_(pow_s(cos_s(a), CSL_2),
                             pow_s(sin_s(a), CSL_2))); // = cos(a)^2-sin(a)^2
    }
    if (argument->getType() == csl::Type::Prod and argument->getNArgs() == 2
        and *argument->getArgument(1) == CSL_PI) // cos(a*pi)
    {
        Expr a = argument->getArgument(0);
        if (a->isInteger()) {
            int newValue = (int) (a->evaluateScalar()) % 2;
            if (newValue != a->evaluateScalar())
                foo.push_back(cos_s(prod_s(int_s(newValue), CSL_PI)));
        }
    }
    if (argument->getNumericalFactor()->evaluateScalar() < 0) // cos(-X)
    {
        foo.push_back(cos_s((prod_s(CSL_M_1, argument))));
    }
    if (foo.size() > 0)
        foo.insert(foo.begin(), DeepCopy(this));

    return foo;
}

csl::vector_expr Sin::getAlternateForms() const
{
    csl::vector_expr foo(0);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // sin(a+b)
    {
        Expr a = argument->getArgument(0);
        Expr b = argument->getArgument(1);
        foo.push_back(
            sum_s(prod_s(cos_s(a), sin_s(b)),
                  prod_s(sin_s(a), cos_s(b)))); // = cos(a)sin(b)+sin(a)cos(b)
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and argument->getArgument(0) == CSL_2) // sin(2a)
    {
        Expr a = argument->getArgument(1);
        foo.push_back(
            prod_s(prod_s(CSL_2, cos_s(a)), sin_s(a))); // = 2cos(a)sin(a)
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and *argument->getArgument(1) == CSL_PI) // sin(a*pi)
    {
        Expr a = argument->getArgument(0);
        if (a->isInteger()) {
            int newValue = (int) (a->evaluateScalar()) % 2;
            if (newValue != a->evaluateScalar())
                foo.push_back(sin_s(prod_s(int_s(newValue), CSL_PI)));
        }
    }
    if (argument->getNumericalFactor()->evaluateScalar() < 0) // sin(-X)
    {
        foo.push_back(prod_s(CSL_M_1, sin_s(prod_s(CSL_M_1, argument))));
    }
    if (foo.size() > 0)
        foo.insert(foo.begin(), DeepCopy(this));

    return foo;
}

csl::vector_expr Tan::getAlternateForms() const
{
    csl::vector_expr foo(2);
    foo[0] = fraction_s(sin_s(argument), cos_s(argument));
    foo[1] = DeepCopy(this);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // tan(a+b)
    {
        Expr arg1 = argument->getArgument(0);
        Expr arg2 = argument->getArgument(1);
        foo.push_back(
            prod_s(sum_s(tan_s(arg1), tan_s(arg2)),
                   pow_s(minus_(CSL_1, prod_s(tan_s(arg1), tan_s(arg2))),
                         CSL_M_1))); // = (tan(a)+tan(b))/(1-tan(a)*tan(b))
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and *argument->getArgument(0) == CSL_2) // tan(2a)
    {
        Expr arg = argument->getArgument(1);
        foo.push_back(prod_s(prod_s(CSL_2, tan_s(arg)),
                             pow_s(minus_(CSL_1, pow_s(tan_s(arg), CSL_2)),
                                   CSL_M_1))); // = 2tan(a)/(1-tan(a)^2)
    }
    if (argument->getNumericalFactor()->evaluateScalar() < 0) // tan(-X)
    {
        foo.push_back(prod_s(CSL_M_1, tan_s(prod_s(CSL_M_1, argument))));
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and *argument->getArgument(1) == CSL_PI) // tan(a*pi)
    {
        Expr a = argument->getArgument(0);
        if (a->isInteger()) {
            int newValue = (int) (a->evaluateScalar()) % 2;
            if (newValue != a->evaluateScalar())
                foo.push_back(tan_s(prod_s(int_s(newValue), CSL_PI)));
        }
    }

    return foo;
}

csl::vector_expr Cosh::getAlternateForms() const
{
    csl::vector_expr foo(0);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // cosh(a+b)
    {
        Expr a = argument->getArgument(0);
        Expr b = argument->getArgument(1);
        foo.push_back(sum_s(
            prod_s(cosh_s(a), cosh_s(b)),
            prod_s(sinh_s(a), sinh_s(b)))); // = cosh(a)cosh(b)+sinh(a)sinh(b)
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and argument->getArgument(0) == CSL_2) // cosh(2a)
    {
        Expr a = argument->getArgument(1);
        foo.push_back(sum_s(pow_s(cosh_s(a), CSL_2),
                            pow_s(sinh_s(a), CSL_2))); // = cosh(a)^2+sinh(a)^2
    }
    if (foo.size() > 0)
        foo.insert(foo.begin(), DeepCopy(this));

    return foo;
}

csl::vector_expr Sinh::getAlternateForms() const
{
    csl::vector_expr foo(0);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // sinh(a+b)
    {
        Expr a = argument->getArgument(0);
        Expr b = argument->getArgument(1);
        foo.push_back(sum_s(
            prod_s(cosh_s(a), sinh_s(b)),
            prod_s(sinh_s(a), cosh_s(b)))); // = cosh(a)sinh(b)+sinh(a)cosh(b)
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and argument->getArgument(0) == CSL_2) // sinh(2a)
    {
        Expr a = argument->getArgument(1);
        foo.push_back(
            prod_s(prod_s(CSL_2, cosh_s(a)), sinh_s(a))); // = 2cosh(a)sinh(a)
    }
    if (foo.size() > 0)
        foo.insert(foo.begin(), DeepCopy(this));

    return foo;
}

csl::vector_expr Tanh::getAlternateForms() const
{
    csl::vector_expr foo(2);
    foo[0] = fraction_s(sinh_s(argument), cosh_s(argument));
    foo[1] = DeepCopy(this);
    if (argument->getType() == csl::Type::Sum
        and argument->getNArgs() == 2) // tanh(a+b)
    {
        Expr arg1 = argument->getArgument(0);
        Expr arg2 = argument->getArgument(1);
        foo.push_back(
            prod_s(sum_s(tanh_s(arg1), tanh_s(arg2)),
                   pow_s(sum_s(CSL_1, prod_s(tanh_s(arg1), tanh_s(arg2))),
                         CSL_M_1))); // = (tanh(a)+tanh(b))/(1+tanh(a)*tanh(b))
    }
    else if (argument->getType() == csl::Type::Prod
             and argument->getNArgs() == 2
             and *argument->getArgument(0) == CSL_2) // tanh(2a)
    {
        Expr arg = argument->getArgument(1);
        foo.push_back(prod_s(prod_s(CSL_2, tanh_s(arg)),
                             pow_s(sum_s(CSL_1, pow_s(tanh_s(arg), CSL_2)),
                                   CSL_M_1))); // = 2tanh(a)/(1+tanh(a)^2)
    }

    return foo;
}

csl::vector_expr TensorElement::getAlternateForms() const
{
    return getPermutations();
}

int numberOfMutatingNodes(const Expr &expr)
{
    const int nArgs = expr->getNArgs();
    csl::Type type  = expr->getType();
    int       nodes = 0;
    if (type == csl::Type::Sum or type == csl::Type::Prod
        or type == csl::Type::Pow or type == csl::Type::Cos
        or type == csl::Type::Sin or type == csl::Type::Tan
        or type == csl::Type::Cosh or type == csl::Type::Sinh
        or type == csl::Type::Tanh or type == csl::Type::TensorElement)
        ++nodes;
    for (int i = 0; i != nArgs; ++i)
        nodes += numberOfMutatingNodes(expr->getArgument(i));
    return nodes;
}

bool insertSortMutant(csl::vector_expr &vec, const Expr &newExpr)
{
    // for (int i=0; i<vec.size()-1 and vec.size()>0; i++) {
    //    if (vec[i+1] < vec[i]) {
    //        cout<<"BAD SORT ! \n";
    //    }
    //}
    // for (int i=0; i<vec.size(); i++)
    //    for (int j=0; j<vec.size(); j++)
    //        if (i != j) {
    //            if ((i<j) != (vec[i]<vec[j]))
    //                cout<<"BAD TRANSITIVITY!\n";
    //        }
    for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
        if (**iter == newExpr)
            return false;
        if (newExpr < *iter) {
            vec.insert(iter, newExpr);
            bool simpler = distance(vec.begin(), iter) < NATURAL_SELECTION;
            return simpler;
        }
    }
    vec.insert(vec.end(), newExpr);
    return (vec.size() - 1) < NATURAL_SELECTION;
}

bool addMutants(csl::vector_expr &individuals, const csl::vector_expr &mutants)
{
    bool simpler = false;
    for (const auto &mut : mutants)
        simpler = simpler or insertSortMutant(individuals, mut);

    return simpler;
}

csl::vector_expr getRandomMutation(const Expr &expr)
{
    const int        nArgs = expr->getNArgs();
    csl::vector_expr res(1, DeepCopy(expr));
    if (nArgs > 0)
        for (int i = 0; i != nArgs; ++i) {
            Expr             foo = DeepCopy(expr);
            csl::vector_expr mutations
                = getRandomMutation(expr->getArgument(i));
            for (const auto &m : mutations) {
                foo->setArgument(m, i);
                addAlternateForm(res, foo);
            }
        }

    const size_t size = res.size();
    for (size_t i = 0; i != size; ++i) {
        csl::vector_expr mutations = res[i]->getAlternateForms();
        for (const auto &m : mutations)
            addAlternateForm(res, m);
    }

    while (res.size() > NUMBER_OF_MUTATIONS) {
        int random = rand() % res.size();
        res.erase(res.begin() + random);
    }

    return res;
}

void naturalSelection(csl::vector_expr &individuals)
{
    if (individuals.size() > NATURAL_SELECTION)
        individuals.erase(individuals.begin() + NATURAL_SELECTION,
                          individuals.end());
}

Expr evolve(const Expr &baseIndividual)
{
    if (not RANDOM_SEED) {
        srand(time(nullptr));
        RANDOM_SEED = true;
    }
    csl::vector_expr individuals(1, DeepCopy(baseIndividual));
    int              generation = 0;
    bool             degenerate = false;
    while (generation < NUMBER_OF_GENERATION and not degenerate) {

        csl::vector_expr newIndividuals(0);
        for (const auto &indiv : individuals) {
            csl::vector_expr mutants = getRandomMutation(indiv);
            addMutants(newIndividuals, mutants);
        }

        degenerate = not addMutants(individuals, newIndividuals);
        if (generation % GENERATION_BEFORE_SELECTION == 0)
            naturalSelection(individuals);

        ++generation;
    }
    return individuals[0];
}

/*
//Returns alternateForms of expr recursively (get alternateForms of
alternateForms etc up to MAX_RECURSION_ALTERNATE). csl::vector_expr
getRecursiveAlternateFormsBis(const Expr& expr, int depth)
{
    csl::vector_expr alternateForms;
    //taking alternateForms a first time
    csl::vector_expr toReturn = internalRecursiveAlternateForms(expr,depth-1);
    clearRedundancyAlternate(toReturn);
    if (toReturn.size() == 0) // no alternate form for expr
        return expr->getAlternateForms();
    csl::vector_expr fooVec = toReturn;
    //We take alternate of alternate MAX_RECURSION_ALTERNATE-1 times
    for (int i=1; i<MAX_RECURSION_ALTERNATE; i++)
    {
        reduceAlternate(toReturn);
        alternateForms = toReturn;
        toReturn = csl::vector_expr(0);
        Expr foo;
        for (size_t j=0; j<alternateForms.size(); j++)
        {
            //taking alternate of alternate
            fooVec =
internalRecursiveAlternateForms(alternateForms[j],depth-1); if (fooVec.size()
== 0) fooVec.push_back(DeepCopy(alternateForms[j]));

            size_t fooSize = toReturn.size();
            for (size_t k=0; k<fooVec.size(); k++)
                addAlternateForm(toReturn, fooVec[k]);
            // If there is no new alternate
            if (fooSize == toReturn.size()) {
                reduceAlternate(toReturn);
                return toReturn;
            }
        }
    }
    clearRedundancyAlternate(toReturn);
    reduceAlternate(toReturn);

    return toReturn;
}

csl::vector_expr internalRecursiveAlternateFormsBis(const Expr& expr, int
depth)
{
    int nArgs = expr->getNArgs();
    csl::vector_expr alternateForms(0);
    // if no Argument, no alternate form (for now)
    if (nArgs == 0) return expr->getAlternateForms();
    if (depth != 0)
    {
        csl::vector_expr argument(0);
        ////
        // Warning: here specific if-else conditions on types,
        // could be wrong if changes occur in structure....
        ////
        if (nArgs == 1) argument = csl::vector_expr(1,expr->getArgument());
        else if (expr->getPrimaryType() == csl::PrimaryType::Vectorial or
                 expr->getType() == csl::Type::Sum or
                 expr->getType() == csl::Type::Prod or
                 expr->getType() == csl::Type::Polynomial)
            argument = expr->getVectorArgument();
        else if (nArgs == 2) {
            argument = csl::vector_expr(2);
            argument[0] = expr->getArgument(0);
            argument[1] = expr->getArgument(1);
        }
        else
            callError(cslError::UndefinedBehaviour,
                    "internalRecursiveAlternateForms(const Expr& expr, int
depth)");

        csl::vector_expr fooVec;
        Expr foo;
        vector<csl::vector_expr > newArguments(0);
        if (nArgs > 1)
        {
            //fooVec = internalRecursiveAlternateForms(argument[0],depth-1);
            //if (fooVec.size() == 0) // if no alternate, we keep at least the
argument itself
            //    fooVec = csl::vector_expr(1,DeepCopy(argument[0]));
            for (int i=0; i<nArgs; i++)
            {
                fooVec = internalRecursiveAlternateForms(argument[i],depth-1);
                //cout<<"Alternates of "; expr->print();
                //printVector(fooVec);
                if (fooVec.size() == 0)
                    fooVec = csl::vector_expr(1,DeepCopy(argument[i]));
                for (size_t j=0; j<fooVec.size(); j++) // We take 1+n-1 copies
of actual alternates
                {
                    //cout<<"Adding alternate "; fooVec[j]->print();
                    foo = DeepCopy(expr);
                    foo->setArgument(DeepRefreshed(fooVec[j]),i);
                    //cout<<"DeepRefreshing";foo->print();
                    //cout<<"          ";DeepRefreshed(foo)->print();
                    addAlternateForm(alternateForms, DeepRefreshed(foo));
                }
            }
            //foo = DeepCopy(expr); // Create an Abstract of same type as expr
            //foo->setVectorArgument(csl::vector_expr(1,CSL_0)); // filling it
with a short expr
            //alternateForms = csl::vector_expr(newArguments.size(),foo);
            //for (int i=0; i<alternateForms.size(); i++)
            //{
            //    if (i % 1000 == 0)
            //    {
            //        cout<<"NEW "<<i<<"/"<<alternateForms.size()<<endl;
            //        alternateForms[i]->print();
            //    }
            //    alternateForms[i]->setVectorArgument(newArguments[i]);
            //    alternateForms[i] = DeepRefreshed(alternateForms[i]);
            //}
            clearRedundancyAlternate(alternateForms);
            //cout<<"HERE\n";
            //printVector(alternateForms);
        }
        else
        {
            fooVec =
internalRecursiveAlternateForms(expr->getArgument(),depth-1); for (size_t i=0;
i<fooVec.size(); i++)
            {
                alternateForms.push_back(DeepCopy(expr));
                alternateForms[i]->setArgument(DeepRefreshed(fooVec[i]));
            }
        }

        // Now that we have a list of all possible alternates for the arguments
        // we take the specific alternate forms of the results
        if (alternateForms.size() == 0)
            alternateForms.push_back(DeepCopy(expr));
        csl::vector_expr toReturn(0);
        for (size_t i=0; i<alternateForms.size(); i++)
        {
            alternateForms[i] = DeepRefreshed(alternateForms[i]);
            fooVec = alternateForms[i]->getAlternateForms();
            if (fooVec.size() == 0)
                fooVec.push_back(DeepCopy(alternateForms[i]));
            for (size_t j=0; j<fooVec.size(); j++)
                addAlternateForm(toReturn, DeepRefreshed(fooVec[j]));
        }
        reduceAlternate(toReturn);
        //int countTHIS = 0;
        //for (int i=0; i<toReturn.size(); i++)
        //{
        //    if (*expr == toReturn[i])
        //        countTHIS++;
        //    if (countTHIS > 1)
        //    {
        //        toReturn.erase(toReturn.begin()+i);
        //        i--;
        //    }
        //}
        //if (countTHIS == 0 and toReturn.size() > 0)
            //toReturn.push_back(DeepCopy(expr));
        //cout<<"FINAL ALTERNATES OF "; expr->print();
        //printVector(toReturn);
        return toReturn;
    }
    return expr->getAlternateForms();
}
*/
} // End of namespace csl
