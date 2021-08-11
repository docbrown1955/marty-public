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

#include "operations.h"
#include "utils.h"
#include "counter.h"
#include "sort.h"
#include "space.h"
#include "interface.h"
#include "cslcomplex.h"
#include "indicial.h"
#include "mathFunctions.h"
#include "vector.h"
#include "simplification.h"
#include "replace.h"
#include "librarygenerator.h"
#include "librarydependency.h"
#include "commutation.h"
#include "comparison.h"
#include "options.h"
#include "interface.h"
using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class Sum                                    //
/*************************************************/
///////////////////////////////////////////////////

Sum::Sum(const csl::vector_expr& operands, bool explicitSum)
    :AbstractMultiFunc() 
{
    argument = operands;
    if (not option::freezeMerge and not explicitSum and size() > 0)
        mergeTerms();
}

Sum::Sum(const Expr& leftOperand,
           const Expr& rightOperand,
           bool explicitSum)
    :AbstractMultiFunc()
{
    // We check if one ooperand is already a Sum expression,
    // in which case we insert the other operand in it if the 
    // other operand is not a Sum
    if (leftOperand->getType() == csl::Type::Sum and
        rightOperand->getType() != csl::Type::Sum) 
    {
        argument = leftOperand->getVectorArgument();
        insert(rightOperand, explicitSum or option::freezeMerge);
    }
    else if (rightOperand->getType() == csl::Type::Sum and
             leftOperand->getType() != csl::Type::Sum) 
    {
        argument = rightOperand->getVectorArgument();
        insert(leftOperand, explicitSum);
    }
    else {
        argument = {leftOperand, rightOperand};
        if (not option::freezeMerge and not explicitSum)
            mergeTerms();
        else if(leftOperand->getType() == csl::Type::Sum
                and rightOperand->getType() == csl::Type::Sum) {
            argument = leftOperand->getVectorArgument();
            for (const auto& arg : *rightOperand)
                insert(arg, explicitSum or option::freezeMerge);
        }
    }
}

optional<Expr> Sum::getRealPart() const
{
    // Real part sum of real parts of arguments
    if (isReal())
        return nullopt;
    if (isPurelyImaginary())
        return CSL_0;
    csl::vector_expr newArg(argument.size());
    for (size_t i = 0; i < argument.size(); i++)
        newArg[i] = GetRealPart(argument[i]);

    return sum_s(newArg);
}

Expr Sum::getImaginaryPart() const
{
    // Imaginary part sum of imaginary parts of arguments
    if (isReal())
        return CSL_0;
    csl::vector_expr newArg(argument.size());
    for (size_t i = 0; i < argument.size(); i++)
        newArg[i] = GetImaginaryPart(argument[i]);

    return sum_s(newArg);
}

optional<Expr> Sum::getComplexModulus() const
{
    Expr real = GetRealPart(copy());
    Expr im = GetImaginaryPart(copy());
     
    return pow_s(sum_s(pow_s(real,CSL_2),
                      pow_s(im,CSL_2)),
                float_s(0.5));
}

optional<Expr> Sum::getComplexArgument() const
{
    Expr real = GetRealPart(copy());
    Expr im = GetImaginaryPart(copy());

    return csl::make_shared<Angle>(im,real);
}

void Sum::insert(const Expr& expr, bool explicitSum)
{
    if (size() == 0) {
        argument.insert(argument.begin(), expr);
        return;
    }
    if (expr->getPrimaryType() == csl::PrimaryType::Numerical) {
        // If the inserted expression is numerical, it's easy
        if (argument[0]->getPrimaryType() == csl::PrimaryType::Numerical)
            argument[0] = argument[0]->addition_own(expr);
        else {
            argument.insert(argument.begin(), expr);
        }

        if (*argument[0] == CSL_0) {
            argument.erase(argument.begin());
        }
        return;
    }

    // If not numerical, we search for a similar term
    if (not explicitSum) {
        Expr term = GetTerm(expr);
        Expr numericalFact = expr->getNumericalFactor();
        Expr term2;
        for (auto arg=argument.begin(); arg!=argument.end(); ++arg) {
            term2 = GetTerm(*arg);
            if (*term == term2) {
                *arg = prod_s((*arg)->getNumericalFactor()
                                    ->addition_own(numericalFact),
                              term);
                if (**arg == CSL_0) {
                    // x-x = 0
                    arg = argument.erase(arg);
                }
                return;
            }
        }
    }

    // No term corresponds, we order correctly the new term in the sum
    // with respect to its simplicity
    for (auto arg=argument.begin(); arg!=argument.end(); ++arg) {
        if (expr < *arg) {
            argument.insert(arg, expr);
            return;
        }
    }

    // no term is more complicated than expr, we put it at the end
    argument.push_back(expr);
}

void Sum::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (mode > 1) // Priority lesser than the previous operation: brackets
        out<<"(";
    for (size_t i = 0; i < argument.size(); i++) {
        argument[i]->print(1, out, lib);
        if (i+1 < size()) {
            if (argument[i+1] == -CSL_1)
                out << " ";
            // if (argument[i+1]->getType() == csl::Type::Prod
            //         and argument[i+1]->getNArgs() > 0
            //         and argument[i+1]->getArgument(0)->isInteger()
            //         and argument[i+1]->getArgument(0)->evaluateScalar() < 0)
            //     out << " ";
            // else
            out<<" + ";
        }
    }
    if (mode > 1)
        out<<")";
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void Sum::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::sum_s({";
    for (size_t i = 0; i != size(); ++i) {
        argument[i]->printCode(1, out);
        if (i+1 != size())
            out << " , ";
    }
    out << "})";
}

string Sum::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode > 1) // Priority lesser than the previous operation: brackets
        sout<<"\\left(";
    for (size_t i = 0; i < argument.size(); i++) {
        sout<<argument[i]->printLaTeX(1);
        if (i+1 < size())
            sout<<"+";
    }
    if (mode > 1)
        sout<<"\\right)";
    if (mode == 0)
        sout<<endl;

    return sout.str();
}

long double Sum::evaluateScalar() const
{
    double sum=0;
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getDim() > 0) 
            // if the sum contains a non-scalar term, the function returns 0.
            return 0;
        sum += argument[i]->evaluateScalar();
    }
    return sum;
}

optional<Expr> Sum::evaluate(
        csl::eval::mode user_mode
        ) const
    // Evaluates the expression replacing valued Variables and constants by
    // their values
{
    bool number = true;
    // In the loop we keep track separately of numbers and expression that do not
    // evaluate to numbers to save time.
    vector<optional<Expr>> newArg(argument.size());
    bool eval = false;
    for (size_t i = 0; i != argument.size(); ++i) {
        newArg[i] = argument[i]->evaluate(user_mode);
        if (not eval and newArg[i])
            eval = true;
    }
    if (not eval)
        return nullopt;
    Expr numericalRes = CSL_0;
    csl::vector_expr new_argument;
    for (size_t i = 0; i < argument.size(); i++) {
        Expr foo = newArg[i].value_or(argument[i]);
        if (foo->getPrimaryType() == csl::PrimaryType::Numerical)
            numericalRes = numericalRes->addition_own(foo);
        else {
            new_argument.push_back(foo);
            number = false;
        }
    }
    if (number) 
        return numericalRes;
    new_argument.push_back(numericalRes);
    return sum_s(new_argument);
}

unique_Expr Sum::copy_unique() const
{
    if (isIndexed())
        return make_unique<ISum>(argument, true);
    else
        return make_unique<Sum>(argument, true);
}

Expr Sum::deepCopy() const
{
    csl::vector_expr copyArgument(argument.size());
    for (size_t i = 0; i != argument.size(); ++i)
        copyArgument[i] = argument[i]->deepCopy();

    if (isIndexed())
        return csl::make_shared<ISum, alloc_isum>(copyArgument, true);
    else
        return csl::make_shared<Sum, alloc_sum>(copyArgument, true);
}

Expr Sum::refresh() const
{
    return sum_s(argument);
}

Expr Sum::deepRefresh() const
{
    csl::vector_expr refreshedArgument(argument.size());
    for (size_t i = 0; i != argument.size(); ++i) {
        refreshedArgument[i] = argument[i]->deepRefresh();
    }

    return sum_s(refreshedArgument);
}

bool Sum::mergeTerms()
    // Merges similar terms: numbers and identical terms to a numerical factor
    // (2*x ~ x etc). Ex: 2 + 2*x + 1/2 + 1/2*x -> 5/2 + 5/2*x.
{
    bool simplified = false;
    Expr arg;

    // Here we check if there are Sum terms in the arguments, we flatten the 
    // whole thing to have only non-plus arguments.
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getType() == csl::Type::Sum) { // Sum(Sum(.. 
            int t_nArgs = argument[i]->getNArgs();
            vector_expr t_argument = argument[i]->getVectorArgument();
            for (size_t j = 0; j < argument.size(); j++)
                if (i != j)
                    t_argument.push_back(argument[j]);

            i += t_nArgs-1;
            argument = std::move(t_argument);
        }
    }

    if (argument.empty()) {
        argument = {CSL_0};
        return true;
    }
    // Here is the merge of numbers in the expression (for example 3+x+2 = 5+x)
    bool numericalFactor = (argument[0]->getPrimaryType() 
            == csl::PrimaryType::Numerical);
    for (size_t i = numericalFactor; i < argument.size(); i++) {
        if (argument[i]->getPrimaryType() == csl::PrimaryType::Numerical) {
            if (!numericalFactor) {
                Expr foo = argument[i];
                argument.erase(argument.begin()+i);
                argument.insert(argument.begin(),foo);
                numericalFactor = true;
                i--;
            }
            else {
                argument[0] = argument[0]->addition_own(argument[i]);
                argument.erase(argument.begin()+i);
                i--;
            }
        }
    }
    if (argument.empty()) {
        argument = {CSL_0};
        return true;
    }

    // If the numerical factor is zero we delete it: 0+x = x
    if (numericalFactor and *argument[0] == CSL_0 and size() > 1) {
        numericalFactor = false;
        argument.erase(argument.begin());
    }

    // Check for identical terms in the sum, x + 2*x = 3*x
    std::vector<csl::Expr> terms(argument.size());
    for (size_t i = 0; i != terms.size(); ++i) 
        terms[i] = GetTerm(argument[i]);
    for (size_t i = numericalFactor; i+1 < argument.size(); i++) {
        Expr term = terms[i];
        Expr factor = argument[i]->getNumericalFactor();
        bool matched = false;
        for (size_t j = i+1; j < argument.size(); j++) {
            Expr term2 = terms[j];
            if (*term == term2) {
                factor = factor->addition_own(argument[j]->getNumericalFactor());
                matched = true;
                argument.erase(argument.begin()+j);
                terms.erase(terms.begin() + j);
                j--;
            }
        }
        if (matched) {
            argument[i] = prod_s(factor, term);
            if (*argument[i] == CSL_0) {
                terms.erase(terms.begin() + i);
                argument.erase(argument.begin()+i);
                i--;
            }
        }
    }
    orderTerms();

    if (argument.size() == 2) {
        // cos^2 + sin^2 = 1
        if (csl::IsPow(argument[0])
		and csl::IsPow(argument[1])
		and csl::IsCos(argument[0][0])
		and csl::IsSin(argument[1][0])
		and argument[0][1] == CSL_2
		and argument[1][1] == CSL_2
		and argument[0][0][0] == argument[1][0][0]) {
	    argument = {CSL_1};
	    return true;
	}
    }

    return simplified;
}

void testSimplificationRule(std::vector<Expr> const &arg)
{
    for (const auto &a : arg) {
        if (a < a or a > a) {
            std::cout << "SELF INCONSISTENT" << std::endl;
            std::cout << a << std::endl;
            std::cin.get();
        }
    }
    for (size_t i = 0; i != arg.size(); ++i)
        for (size_t j = 0; j < arg.size(); ++j) {
            if (i != j)
                continue;
            if (arg[i] < arg[j]) {
                if (!(arg[i] <= arg[j] and arg[i] != arg[j])) {
                    std::cout << "Rule 1 violated" << std::endl;
                    std::cout << arg[i] << std::endl;
                    std::cout << arg[j] << std::endl;
                    std::cin.get();
                }
                if (arg[j] <= arg[i]) {
                    std::cout << "Rule 2 violated" << std::endl;
                    std::cout << arg[i] << std::endl;
                    std::cout << arg[j] << std::endl;
                    std::cin.get();
                }
            }
            if (!(arg[i] < arg[j]) 
                    and !(arg[j] < arg[i]) 
                    and !(arg[i] == arg[j])) {
                std::cout << "Not trichotomus" << std::endl;
                std::cout << arg[i] << std::endl;
                std::cout << arg[j] << std::endl;
                std::cin.get();
            }
        }
    for (size_t i = 0; i != arg.size(); ++i)
        for (size_t j = 0; j < arg.size(); ++j) 
            for (size_t k = 0; k != arg.size(); ++k)
                if (i != j and i != k and k != j) {
                    bool r_ij = arg[i] < arg[j];
                    bool r_ik = arg[i] < arg[k];
                    bool r_jk = arg[j] < arg[k];
                    if (r_ij and r_jk and !r_ik) {
                        std::cout << "Not transitive " << std::endl;
                        std::cout << arg[i] << std::endl;
                        std::cout << arg[j] << std::endl;
                        std::cout << arg[k] << std::endl;
                        std::cout << csl::DeepRefreshed(arg[i]) << std::endl;
                        std::cout << csl::DeepRefreshed(arg[j]) << std::endl;
                        std::cout << csl::DeepRefreshed(arg[k]) << std::endl;
                        std::cout << r_ij << " " << r_ik << " " << r_jk << std::endl;
                        std::cin.get();
                    }
                }
}

void Sum::orderTerms()
    // Sorts the different arguments by simplicity, ex x*y+2 -> 2+x*y
{
    // The sort function of <algorithm> header uses the operator< between 
    // Expr, that is defined as the simplicity order. Therefore it sorts 
    // the arguments by order of simplicity

    // if (testOperatorComparison) {
    //     testSimplificationRule(argument);
    //     for (size_t i = 0; i != size(); ++i) {
    //         std::cout << i << " = " << argument[i] << std::endl;
    //     }
    //     std::cout << "  ";
    //     for (size_t i = 0; i != size(); ++i) {
    //         std::cout << i << " -- ";
    //         std::cout << std::endl;
    //     }
    //     for (size_t i = 0; i != size(); ++i) {
    //         std::cout << i << " ";
    //         for (size_t j = 0; j != size(); ++j)
    //             std::cout << (argument[i] < argument[j]) << " -- ";
    //         std::cout << std::endl;
    //     }
    // }
    // csl::sort(argument.begin(), argument.end());

    mergeSort(argument);

    return;
}

optional<Expr> Sum::derive(Expr_info expr) const
{
    // Derivative is the sum of the derivatives of arguments
    Expr rep = CSL_0;
    csl::vector_expr newArgs;
    for (size_t i = 0; i < argument.size(); i++) {
        if (not argument[i]->dependsOn(expr))
            continue;
        if (Expr derivative = Derived(argument[i], expr);
                derivative != CSL_0)
            newArgs.push_back(derivative);
    }

    return sum_s(newArgs);
}

Expr Sum::getNumericalFactor() const
{
    const auto isValidFactor = [](csl::Expr const &num) {
        constexpr auto lim = 10.;
        return (csl::IsInteger(num) && std::fabs(num->evaluateScalar()) < lim)
            || (csl::IsIntFraction(num) && std::fabs(num->evaluateScalar()) < lim);
    };
    const size_t sz = argument.size();
    for (size_t i = 0; i != sz; ++i) {
        const auto &arg = argument[i];
        if (arg != CSL_0) {
            auto factor = (isValidFactor(arg)) ?  arg : arg->getNumericalFactor();
            return isValidFactor(factor) ? factor : CSL_1;
        }
    }
    return CSL_0;
}

int Sum::getOrderOf(Expr_info expr) const
{
    if (argument.empty())
        return 0;
    int order = argument[0]->getOrderOf(expr);
    for (size_t i = 1; i != argument.size(); ++i)
        if (auto newOrder = argument[i]->getOrderOf(expr);
                newOrder < order)
            order = newOrder;
    return order;
}

optional<Expr> Sum::getTerm() const
{
    Expr num = getNumericalFactor();
    if (num != CSL_1 and num != CSL_0) {
        csl::vector_expr copyArgument(argument.size());
        for (size_t i = 0; i != size(); ++i) {
            copyArgument[i] = argument[i] / num;
        }
        return sum_s(copyArgument, true);
    }
    else if (num == CSL_0)
        return CSL_0;
    return nullopt;
}

csl::vector_expr Sum::getFactors() const
{
    csl::vector_expr factors;
    csl::vector_expr arg = argument;
    gatherFactors(factors, arg, false);
    auto term = getTerm();
    if (term)
        factors.push_back(*term);
    else
        factors.push_back(copy());
    clearRedundantFactors(factors);

    return factors;
}

void Sum::gatherFactors(
        csl::vector_expr& factors,
        csl::vector_expr& arg,
        bool full
        ) const
{
    arg = argument;
    // If full, we factor first the arguments independently
    if (full) {
        for (size_t i = 0; i < argument.size(); i++)
            Factor(arg[i], full);
    }

    size_t mini = 0;
    int nFactorsMini = arg[0]->getNFactor();
    for (size_t i = 1; i < argument.size(); i++) {
        if (!GetCommutable(arg[mini])
                or (GetCommutable(arg[i]) 
                and nFactorsMini > arg[i]->getNFactor())) {
            mini = i;
            nFactorsMini = arg[i]->getNFactor();
        }
    }
    if (!GetCommutable(arg[mini])) {
        return;
    }
    factors = arg[mini]->getFactors();
    if (factors.size() == 0) factors = csl::vector_expr(1,arg[mini]);
    for (size_t i = 0; i < argument.size(); i++) {
        if (i != mini) {
            for (size_t j=0; j<factors.size(); j++) {
                if (!arg[i]->askTerm(factors[j].get())) {
                    factors.erase(factors.begin()+j);
                    j--;
                }
            }
            if (factors.size() == 0) 
                break;
        }
    }
    csl::sort(factors.begin(), factors.end());
}

void Sum::clearRedundantFactors(csl::vector_expr& factors) const
{
    // Here we clear redundant factors, i.e. different powers of the same object.
    // For example if x, x^2, x^3 are possible we only keep x^3.
    for (size_t i=0; i<factors.size(); i++) {
        if (!GetCommutable(factors[i])) {
            factors.erase(factors.begin() + i);
            --i;
            continue;
        }
        if (factors[i]->getType() == csl::Type::Pow) {
            Expr exponent_i = factors[i]->getArgument(1);
            if (exponent_i->getPrimaryType() != csl::PrimaryType::Numerical
                    or exponent_i->getType() == csl::Type::Complex) {
                factors.erase(factors.begin()+i);
                i--;
                continue;
            }

            for (size_t j=0; j<factors.size(); j++) {
                if (i != j) {
                    if (factors[j]->getType() == csl::Type::Pow
                            and factors[i]->getArgument() == 
                                factors[j]->getArgument()) {
                        Expr exponent_j = factors[j]->getArgument(1);
                        if (exponent_j->getPrimaryType() != csl::PrimaryType::Numerical
                                or exponent_j->getType() == csl::Type::Complex){
                            factors.erase(factors.begin()+j);
                            if (i > j)
                                i--;
                            break;
                        }
                        if (abs(exponent_i->evaluateScalar()) <
                                abs(exponent_j->evaluateScalar()))
                        {
                            factors.erase(factors.begin()+i);
                            i--;
                            break;
                        }
                        factors.erase(factors.begin()+j);
                        if (i > j)
                            i--;
                        break;
                    }
                    else if (*factors[j] == factors[i]->getArgument(0)) {
                        factors.erase(factors.begin()+j);
                        if (i > j)
                            i--;
                        break;
                    }
                }
            }
        }
    }
}

optional<Expr> Sum::factor(bool full) const
    // We try to factor the sum without knowing a specific factor
{
    if (size() == 0)
        return CSL_0;
    csl::vector_expr arg;
    csl::vector_expr factors;
    gatherFactors(factors, arg, full);
    clearRedundantFactors(factors);
    if (factors.size() == 0)
        return nullopt;
    // Finally, if there is some factors left, we factor them out of the
    // expression
    for (size_t j=0; j<factors.size(); j++) {
        Expr expr = factors[j];
        Expr newAbstract = CSL_0;
        for (size_t i = 0; i < argument.size(); i++) {
            if (*arg[i]==expr)
                arg[i] = CSL_1;
            else {
                csl::Type type = arg[i]->getType();
                if (type == csl::Type::Prod or type == csl::Type::Pow) {
                    if (arg[i]->askTerm(expr.get())) {
                        Expr foo = arg[i]->suppressTerm(expr.get());
                        arg[i] = foo;
                    }
                    else
                        return nullopt;
                }
                else
                    return nullopt;
            }
        }
    }

    return prod_s(prod_s(factors),sum_s(arg));
}

optional<Expr> Sum::factor(Expr_info expr, bool full) const
    // This function tries to factor the sum with respect to a particular
    // expression
{

    // If (full) we first factor recursively the arguments independently
    csl::vector_expr arg = argument;
    if (full)
        for (size_t i = 0; i < argument.size(); i++)
            Factor(arg[i], true);

    // Here we test the factor for each argument (if it is a valid factor)
    Expr newAbstract = CSL_0;
    vector<int> toFactor(0);
    for (size_t i = 0; i < argument.size(); i++) {
        if (*arg[i] == expr)
            toFactor.push_back(i);
        else {
            csl::Type type = arg[i]->getType();
            if (type == csl::Type::Prod or type == csl::Type::Pow) {
                if (arg[i]->askTerm(expr))
                    toFactor.push_back(i);
                else {
                    return std::nullopt;
                }
            }
            else
                return std::nullopt;
        }
    }

    Expr copyExpr = expr->copy();
    if (toFactor.size() == argument.size()) {
        std::vector<csl::Expr> factored;
        factored.reserve(toFactor.size());
        for (size_t i=0; i<toFactor.size(); i++) {
            Expr foo = arg[toFactor[i]]->suppressTerm(expr);
            if (foo*copyExpr != arg[toFactor[i]])
                return nullopt;
            factored.push_back(arg[toFactor[i]]->suppressTerm(expr));
        }
        return csl::prod_s(sum_s(factored), copyExpr, true);
    }
    else
        return nullopt;
}

std::optional<Expr> Sum::collect(
        std::vector<Expr> const &factors,
        bool                     full
        ) const
{
    for (const auto &f : factors)
        CSL_ASSERT_SPEC(
                !f->isIndexed(),
                CSLError::TypeError,
                "Indexed factors are not yet supported for the "
                "collect function"
                )
    bool collected = false;
    Expr copy = Copy(this);
    if (full) {
        for (auto &arg : copy) {
            auto opt = arg->collect(factors, full);
            if (opt) {
                arg = *opt;
                collected = true;
            }
        }
    }
    size_t size = copy->size();
    std::vector<size_t> indicesLeft(size);
    for (size_t i = 0; i != indicesLeft.size(); ++i) 
        indicesLeft[i] = i;
    std::vector<Expr> expo1(factors.size(), CSL_0);
    std::vector<Expr> expo2(factors.size(), CSL_0);
    std::vector<size_t> sameFactors;
    sameFactors.reserve(argument.size());

    std::vector<Expr> terms;
    for (size_t arg1 = 0; arg1 != indicesLeft.size(); ++arg1)  {
        std::fill(expo1.begin(), expo1.end(), CSL_0);
        copy[indicesLeft[arg1]]->getExponents(factors, expo1);
        if (std::all_of(
                    expo1.begin(), 
                    expo1.end(), 
                    [](Expr const &e) { 
                        return e == CSL_0; 
                    }))
            continue;
        sameFactors.clear();
        sameFactors.push_back(arg1);
        for (size_t arg2 = arg1+1; arg2 < indicesLeft.size(); ++arg2) {
            std::fill(expo2.begin(), expo2.end(), CSL_0);
            copy[indicesLeft[arg2]]->getExponents(factors, expo2);
            if (expo1 == expo2) {
                sameFactors.push_back(arg2);
            }
        }
        if (sameFactors.size() > 1) {
            std::vector<Expr> frontTerm;
            frontTerm.reserve(factors.size());
            for (size_t i = 0; i != factors.size(); ++i) {
                if (expo1[i] != CSL_0)
                    frontTerm.push_back(csl::pow_s(factors[i], expo1[i]));
            }
            std::vector<Expr> sumTerms;
            sumTerms.reserve(sameFactors.size());
            for (size_t i = sameFactors.size(); i --> 0 ;) {
                Expr term = copy[indicesLeft[sameFactors[i]]];
                for (size_t j = 0; j != factors.size(); ++j) {
                    if (expo1[j] != CSL_0) {
                        auto removed = term->suppressExponent(
                                factors[j], expo1[j]);
                        CSL_ASSERT_SPEC(removed,
                                CSLError::RuntimeError,
                                "Expression " + toString(term) + " has not "
                                "removed the factor " + toString(factors[j])
                                + " correctly.")
                        term = *removed;
                    }
                }
                sumTerms.push_back(term);
                if (arg1 >= sameFactors[i])
                    --arg1;
                indicesLeft.erase(indicesLeft.begin() + sameFactors[i]);
            }
            frontTerm.push_back(csl::Factored(csl::sum_s(sumTerms)));
            terms.push_back(csl::prod_s(frontTerm));
            collected = true;
        }
    }
    if (!collected)
        return std::nullopt;
    for (size_t i : indicesLeft)
        terms.push_back(copy[i]);
    return csl::sum_s(terms);
}

bool Sum::askTerm(Expr_info term, bool exact) const
{
    if (*this == term)
        return true;
    if (getNumericalFactor() != CSL_1) {
        if (*getTerm().value() == term)
            return true;
    }
    for (const auto& arg : argument)
        if (not arg->askTerm(term, exact)) 
            return false;
    return true;
}

Expr Sum::suppressTerm(Expr_info term) const
{
    if (*this == term)
        return CSL_1;
    if (auto opt = getTerm(); opt && *opt.value() == term) {
        return getNumericalFactor();
    }
    csl::vector_expr newArg(argument.size());
    for (size_t i = 0; i != argument.size(); ++i)
        newArg[i] = argument[i]->suppressTerm(term);
    return sum_s(newArg);
}

int Sum::getParity(Expr_info t_variable) const
{
    // The parity of a sum is the parity of its arguments if they
    // all have the same
    int parity = argument[0]->getParity(t_variable);
    if (parity == 0)
        return 0;

    for (size_t i = 1; i < argument.size(); i++)
        if (parity != argument[i]->getParity(t_variable))
            return 0;

    return parity;
}

bool Sum::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    const auto sz = size();
    if (sz == 1) 
        return *argument[0]==expr;
    const auto exprType = expr->getType();
    if (exprType == csl::Type::Polynomial)
        return *this == expr->getRegularExpression().get();
    if (exprType != csl::Type::Sum) 
        return false;
    if (sz != expr->size()) 
        return false;
    vector<size_t> indicesLeft(sz);
    for (size_t i = 0; i < sz; ++i) 
        indicesLeft[i] = i;

    for (size_t i = 0; i < sz; ++i) {
        bool matched = false;
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            map<Index,Index> constraints;
            Expr const &foo = expr->getArgument(indicesLeft[j]);    
            if (*argument[i] == foo.get()) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
        }
        if (not matched)
            return false;
    }

    return true;
}

Expr sum_s(
        const Expr& leftOperand, 
        const Expr& rightOperand,
        bool explicitSum)
    // Creates a Sum object a priori but can return other types
    // in particular cases
{
    const auto lPType = leftOperand->getPrimaryType();
    const auto rPType = rightOperand->getPrimaryType();
    if (lPType== csl::PrimaryType::Arbitrary
            or rPType == csl::PrimaryType::Arbitrary) {
        if (leftOperand->isIndexed() or rightOperand->isIndexed())
            return csl::make_shared<ISum, alloc_isum>(
                    csl::vector_expr({leftOperand,rightOperand}),true);
        else
            return csl::make_shared<Sum, alloc_sum>(
                    csl::vector_expr({leftOperand,rightOperand}),true);
    }

    if (leftOperand == CSL_0)
        return rightOperand;
    if (rightOperand == CSL_0)
        return leftOperand;
    if (leftOperand->getPrimaryType() == csl::PrimaryType::Vectorial)
        return leftOperand->addition_own(rightOperand);
    if (rightOperand->getPrimaryType() == csl::PrimaryType::Vectorial)
        return rightOperand->addition_own(leftOperand);
    const auto lType = leftOperand->getType();
    const auto rType = rightOperand->getType();
    if (lType == csl::Type::Polynomial)
        return leftOperand->addition_own(rightOperand);
    if (rType == csl::Type::Polynomial)
        return rightOperand->addition_own(leftOperand);

    Expr foo;
    if (leftOperand->isIndexed() or rightOperand->isIndexed())
        foo = csl::make_shared<ISum, alloc_isum>(
                leftOperand, 
                rightOperand,
                explicitSum);
    else
        foo = csl::make_shared<Sum, alloc_sum>(
                leftOperand, 
                rightOperand,
                explicitSum);

    const auto s = foo->size();
    if (s == 1) {
        return foo[0];
    }
    else if (s == 0) {
        return CSL_0;
    }

    return foo;
}

Expr sum_s(const csl::vector_expr& operands, bool explicitSum)
{
    if (operands.size() == 0)
        return CSL_0;
    bool indexed = false;
    for (const auto& arg : operands)
        if (arg->isIndexed()) {
            indexed = true;
            break;
        }
    Expr foo;
    if (indexed)
        foo = csl::make_shared<ISum, alloc_isum>(operands, explicitSum);
    else
        foo = csl::make_shared<Sum, alloc_sum>(operands, explicitSum);

    const auto s = foo->size();
    if (s == 1) {
        return foo[0];
    }
    else if (s == 0) {
        return CSL_0;
    }

    return foo;
}

Expr minus_(const Expr& leftOperand, const Expr& rightOperand) {
    // minus(a,b) = plus(a,-b)
    // if (leftOperand == rightOperand)
    //     return CSL_0;
    return sum_s(leftOperand,prod_s(CSL_M_1,rightOperand));
}

///////////////////////////////////////////////////
/*************************************************/
// Class Prod                                   //
/*************************************************/
///////////////////////////////////////////////////

Prod::Prod(const csl::vector_expr& operands, bool explicitProd)
    :AbstractMultiFunc()
{
    argument = operands;
    if (not option::freezeMerge and not explicitProd and size() > 0) {
        mergeProducts();
        orderTerms();
        mergeTerms();
    }
    else {
        mergeProducts();
    }
}

Prod::Prod(const Expr& leftOperand,
           const Expr& rightOperand,
           bool explicitProd): AbstractMultiFunc()
{
    if (not option::freezeMerge
        and not explicitProd 
        and leftOperand->getType() == csl::Type::Prod 
        and rightOperand->getType() != csl::Type::Prod) {
        argument = leftOperand->getVectorArgument();
        insert(rightOperand, true); // rightOperand inserted to the right
                                    // of leftOperand
    }
    else if (not option::freezeMerge
             and not explicitProd 
             and rightOperand->getType() == csl::Type::Prod 
             and leftOperand->getType() != csl::Type::Prod)    {
        argument = rightOperand->getVectorArgument();
        insert(leftOperand, false); // leftOperand inserted to the left
                                    // of rightOperand
    }
    else {
        argument = csl::vector_expr(2);
        argument[0] = leftOperand;
        argument[1] = rightOperand;
        if (not option::freezeMerge and not explicitProd) {
            mergeProducts();
            orderTerms();
            mergeTerms();
        }
        else
            mergeProducts();
    }
}

void Prod::mergeProducts()
{
    size_t extraSize = 0;
    for (const auto &arg : argument)
        if (arg->getType() == csl::Type::Prod) {
            extraSize += arg->size();
            break;
        }
    if (extraSize == 0)
        return;
    csl::vector_expr argument_new;
    argument_new.reserve(argument.size() + extraSize);
    for (const auto& arg : argument) 
        if (arg->getType() == csl::Type::Prod)
            for (const auto& sub_arg : *arg)
                argument_new.push_back(sub_arg);
        else
            argument_new.push_back(arg);
    argument = std::move(argument_new);
}

Expr Prod::getNumericalFactor() const
{
    if (argument[0]->getPrimaryType() == csl::PrimaryType::Numerical)
        return argument[0];

    return CSL_1;
}

optional<Expr> Prod::getTerm() const
{
    if (argument.empty() 
            || argument[0]->getPrimaryType() == csl::PrimaryType::Numerical) {
        if (argument.size() == 2)
            return argument[1];
        csl::vector_expr foo(argument);
        foo.erase(foo.begin());
        auto res = (isIndexed()) ? 
            csl::make_shared<IProd>()
            :csl::make_shared<Prod>();
        res->setVectorArgument(foo);
        return res;
        // return prod_s(foo, true);
    }

    return nullopt;
}

int Prod::getNFactor() const
{
    int nFactor = 1;
    for (size_t i = 0; i < argument.size(); i++)
        nFactor *= argument[i]->getNFactor();

    return nFactor;
}

csl::vector_expr Prod::getFactors() const
{
    csl::vector_expr foo, bar;
    for (size_t i = 0; i != argument.size(); i++) {
        bar = argument[i]->getFactors();
        if (bar.empty())
            bar = csl::vector_expr(1, argument[i]);
        foo.insert(foo.end(), bar.begin(), bar.end());
    }
    return foo;
}

void Prod::getExponents(
        std::vector<Expr> const &factors,
        std::vector<Expr>       &exponents
        ) const
{
    for (const auto &arg : argument)
        arg->getExponents(factors, exponents);
}

bool Prod::askTerm(Expr_info expr, bool exact) const
{
    for (size_t i = 0; i < argument.size(); i++) {
        if (Comparator::freeIndexComparison(argument[i].get(),expr))
            return true;
        else if (!exact //and argument[i]->getType() == csl::Type::Pow and // Pow
                 && !argument[i]->isIndexed()
                 && argument[i]->askTerm(expr))
            return true;
    }

    return false;
}

Expr Prod::suppressTerm(Expr_info expr) const
{
    csl::vector_expr newArg;
    const size_t sz = argument.size();
    newArg.reserve(sz);
    bool matched = false;
    for (size_t i = 0; i < sz; i++) {
        auto arg = argument[i];
        if (not matched) {
            if (arg->getType() == csl::Type::Pow) { // Pow 
                if (arg->askTerm(expr)) {
                    newArg.push_back(arg->suppressTerm(expr));
                    if (newArg.back() == CSL_1)
                        newArg.erase(newArg.end() - 1);
                    matched = true;
                    continue;
                }
                else
                    newArg.push_back(arg);
            }
            else if (arg->getType() == csl::Type::Sum
                    and arg->askTerm(expr)) {
                newArg.push_back(arg->suppressTerm(expr));
                if (csl::IsNumerical(newArg.back()))
                    newArg.erase(newArg.end() - 1);
                matched = true;
                continue;
            }
            else if (*arg == expr) {
                matched = true;
                continue;
            }
            else
                newArg.push_back(arg);
        }
        else
            newArg.push_back(arg);
    }

    return prod_s(newArg, true);
}

std::optional<Expr> Prod::suppressExponent(
        Expr const &factor,
        Expr const &exponent
        ) const
{
    size_t pos;
    Expr removed = nullptr;
    for (size_t i = 0; i != argument.size(); ++i) {
        auto rm = argument[i]->suppressExponent(factor, exponent);
        if (rm) {
            removed = *rm;
            pos = i;
            break;
        }
    }
    if (removed) {
        std::vector<Expr> args = argument;
        args[pos] = removed;
        return csl::prod_s(args);
    }
    return std::nullopt;
}

int Prod::getOrderOf(Expr_info expr) const
{
    int order = 0;
    for (const auto& arg : argument)
        order += arg->getOrderOf(expr);
    return order;
}

optional<Expr> Prod::getRealPart() const
{
    Expr realPart = GetRealPart(argument[0]);
    Expr imaginaryPart = GetImaginaryPart(argument[0]);
    Expr foo;
    for (size_t i = 1; i < argument.size(); i++) {
        foo = Copy(realPart);
        realPart = sum_s(prod_s(realPart,GetRealPart(argument[i])),
                         prod_s(CSL_M_1,
                                prod_s(imaginaryPart,
                                       argument[i]->getImaginaryPart())));
        if (i+1 < size())
            imaginaryPart = sum_s(prod_s(foo,argument[i]->getImaginaryPart()),
                                  prod_s(imaginaryPart,
                                         GetRealPart(argument[i])));
    }

    return realPart;
}

Expr Prod::getImaginaryPart() const
{
    Expr realPart = GetRealPart(argument[0]);
    Expr imaginaryPart = argument[0]->getImaginaryPart();
    Expr foo;
    for (size_t i = 1; i < argument.size(); i++) {
        foo = Copy(realPart);
        if (i+1 < size())
            realPart = sum_s(prod_s(realPart,GetRealPart(argument[i])),
                             prod_s(CSL_M_1,
                                    prod_s(imaginaryPart,
                                           argument[i]->getImaginaryPart())));
        imaginaryPart = sum_s(prod_s(foo,argument[i]->getImaginaryPart()),
                              prod_s(imaginaryPart, GetRealPart(argument[i])));
    }

    return imaginaryPart;
}

optional<Expr> Prod::getComplexModulus() const
{
    Expr real = GetRealPart(copy());
    Expr im = getImaginaryPart();
    return pow_s(sum_s(pow_s(real,CSL_2),pow_s(im,CSL_2)), float_s(0.5));
}

optional<Expr> Prod::getComplexArgument() const
{
    Expr real = GetRealPart(copy());
    Expr im = getImaginaryPart();
    return csl::make_shared<Angle>(im,real);
}

optional<Expr> Prod::getHermitianConjugate(
        const Space* space) const
{
    return getHermitianConjugate(vector<const Space*>(1, space));
}

optional<Expr> Prod::getHermitianConjugate(
        const vector<const Space*>& space) const
{
    vector<optional<Expr>> newArg(argument.size());
    for (size_t i = 0; i != argument.size(); ++i) {
        newArg[i] = argument[i]->getHermitianConjugate(space);
    }
    Prod res;
    res.argument = argument;
    for (size_t i = 0; i != argument.size(); ++i)
        res.setArgument(newArg[i].value_or(argument[i]),
                         argument.size()-1-i);

    return res.refresh();
}

optional<Expr> Prod::findSubExpression(
        Expr_info   subExpression,
        const Expr& newExpression)  const
{
    if (subExpression->getType() == csl::Type::Prod) {
        if (partialComparison(subExpression)) {
            // We take care of commutation problems: we suppress
            // all terms of subExpression and place newExpression
            // at the place of the first term of subExpression.
            Expr res = copy();
            size_t t_nArgs = subExpression->getNArgs();
            const csl::vector_expr& t_arg = subExpression->getVectorArgument();
            for (size_t i = 0; i < argument.size(); ++i)
                if (*t_arg[0] == argument[i]) {
                    res->setArgument(newExpression, i);
                    break;
                }
            for (size_t i = 1; i < t_nArgs; ++i)
                res = res->suppressTerm(t_arg[i].get());

            return Refreshed(res);
        }
        else 
            return nullopt;
    }

    return AbstractMultiFunc::findSubExpression(subExpression, newExpression);
}
     
void Prod::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (mode > 2) // Priority lesser than the previous operation: brackets
        out<<"(";
    vector<int> denominatorIndices(0);
    vector<int> numeratorIndices(0);
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getType() == csl::Type::Pow and
            argument[i]->getArgument(1)->isInteger() and
            argument[i]->getArgument(1)->evaluateScalar() == -1)
            denominatorIndices.push_back(i);
        else
            numeratorIndices.push_back(i);
    }
    if (numeratorIndices.size() >= 1) {
        if (argument[numeratorIndices[0]] == -CSL_1)
            out << "-";
        else {
            argument[numeratorIndices[0]]->print(2, out, lib);
            if (numeratorIndices.size() > 1)
                out << "*";
        }
    }
    for (size_t i=1; i<numeratorIndices.size(); i++) {
        argument[numeratorIndices[i]]->print(2, out, lib);
        if (i <numeratorIndices.size() - 1)
            out<<"*";
    }
    if (denominatorIndices.size() > 0) {
        if (numeratorIndices.size() == 0 
                || (numeratorIndices.size() == 1 
                    && argument[numeratorIndices[0]] == CSL_M_1)) {
            out << "1";
        }
        out<<"/";
        if (denominatorIndices.size() == 1)
            argument[denominatorIndices[0]]->getArgument(0)->print(3, out, lib);
        else {
            out<<"(";
            for(size_t i=0; i<denominatorIndices.size(); i++) {
                argument[denominatorIndices[i]]->getArgument(0)->print(2, out, lib);
                if (i < denominatorIndices.size()-1)
                    out<<"*";
            }
            out<<")";
        }
    }
    if (mode > 2)
        out<<")";
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void Prod::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::prod_s({";
    for (size_t i = 0; i != argument.size(); ++i) {
        argument[i]->printCode(0, out);
        if (i != argument.size() - 1)
            out << ", ";
    }
    out << "})";
}

string Prod::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode > 2) // Priority lesser than the previous operation: brackets
        sout<<"\\left(";
    vector<int> denominatorIndices(0);
    vector<int> numeratorIndices(0);
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getType() == csl::Type::Pow and
            argument[i]->getArgument(1)->isInteger() and
            argument[i]->getArgument(1)->evaluateScalar() == -1)
            denominatorIndices.push_back(i);
        else
            numeratorIndices.push_back(i);
    }
    if (denominatorIndices.size() > 0)
        sout<<"\\frac{";
    for (size_t i=0; i<numeratorIndices.size(); i++) {
        sout<<argument[numeratorIndices[i]]->printLaTeX(2);
        if (i <numeratorIndices.size() - 1
                and static_cast<int>(argument[numeratorIndices[i]]->getPrimaryType()) < 10
                and static_cast<int>(argument[numeratorIndices[i+1]]->getPrimaryType()) >= 10)
            sout<<"\\cdot ";
    }
    if (denominatorIndices.size() > 0) {
        sout<<"}{";
        for(size_t i=0; i<denominatorIndices.size(); i++) {
            sout<<argument[denominatorIndices[i]]->getArgument(0)->printLaTeX(2);
            if (i < denominatorIndices.size()-1
                and static_cast<int>(argument[denominatorIndices[i]]->getPrimaryType()) < 10
                and static_cast<int>(argument[denominatorIndices[i+1]]->getPrimaryType()) >= 10)
                sout<<"\\cdot ";
        }
        sout<<"}";
    }
    if (mode > 2)
        sout<<"\\right)";

    return sout.str();
}

void getExponentStructure(const Expr& argument, Expr& term, Expr& exponent)
    // Search for argument = term^exponent (default argument = argument^1)
    // This function is used assuming that exponent is Numerical
    // Therefore exponent must always be Numerical
{
    if (argument->getType() == csl::Type::Pow) {
        exponent = argument->getArgument(1);
        if (exponent->getPrimaryType() != csl::PrimaryType::Numerical) {
            exponent = CSL_1;
            term = argument;
        }
        else term = argument->getArgument(0);
    }
    else {
        term = argument;
        exponent = CSL_1;
    }
}

// side = 0 insertion to the Left, = 1 to the right.
void Prod::insert(const Expr& expr, bool side)
{
    if (size() == 0) {
        argument.insert(argument.begin(), expr);
        return;
    }
    // If numerical, easy
    if (expr->getPrimaryType() == csl::PrimaryType::Numerical) {
        if (argument[0]->getPrimaryType() == csl::PrimaryType::Numerical)
            argument[0] = argument[0]->multiplication_own(expr);
        else {
            argument.insert(argument.begin(), expr);
        }
        if (*argument[0] == CSL_0) {
            argument.clear();
            argument.push_back(CSL_0);
        }
        else if (*argument[0] == CSL_1) {
            argument.erase(argument.begin());
        }
        return;
    }

    if (expr->isAnOperator() and expr->isEmpty() and not side) {
        Expr res = Copy(expr);
        res = res->applyOperator(Copy(this));
        if (res->getType() == csl::Type::Prod)
            argument = res->getVectorArgument();
        else
            argument = csl::vector_expr(1,res);
        return;
    }
    
    //if (expr->getType() == csl::Type::Derivative 
    //        and expr->isEmpty()
    //        and not side) {
    //    if (*expr->getOperand() == CSL_1)
    //        argument = csl::vector_expr(1,derivative_s(Copy(this),
    //                    expr->getVariable(),
    //                    expr->getOrder()));
    //    else 
    //        argument =  csl::vector_expr(1,derivative_s(
    //                    expr->getOperand()*Copy(this),
    //                    expr->getVariable(),
    //                    expr->getOrder()));
    //    nArgs = 1;
    //    return;
    //}
                
    if (side) rightInsert(expr);
    else      leftInsert (expr);
}
void Prod::leftInsert(const Expr& expr)
{
    // If not numerical, we search for a similar term
    size_t max = size();
    Expr term, exponent;
    getExponentStructure(expr, term, exponent);
    for (size_t i = 0; i < argument.size(); i++) {
        Expr term2, exponent2;
        getExponentStructure(argument[i], term2, exponent2);
        if (not option::checkCommutations 
                or *Commutation(expr, argument[i]) == CSL_0) {
            // If we found the right term, it')s done
            if (*term == term2) {
                argument[i] = pow_s(term, exponent->addition_own(exponent2));
                if (*argument[i] == CSL_1) {
                    argument.erase(argument.begin()+i);
                }
                return;
            }
        }
        else if (*term == term2) {
            argument[i] = pow_s(term, exponent->addition_own(exponent2));
            if (*argument[i] == CSL_1) {
                argument.erase(argument.begin()+i);
            }
            return;
        }
        else {
            // max is the position not reachable because of commutation not trivial
            max = i;
            break;
        }
    }

    // No term corresponds, we order correctly the new term in the sum
    for (size_t i = 0; i < max; i++) 
        if (not option::checkCommutations
                or expr < argument[i]
                or *Commutation(expr, argument[i]) != CSL_0) {
            argument.insert(argument.begin()+i, expr);
            return;
        }

    // no term is more complicated than expr, we put it at the end
    argument.insert(argument.begin()+max, expr);
}

void Prod::rightInsert(const Expr& expr)
{
    // If not numerical, we search for a similar term
    int max = -1;
    Expr term, exponent;
    getExponentStructure(expr, term, exponent);
    for (int i = getNArgs()-1; i >= 0; --i) {
        Expr term2, exponent2;
        getExponentStructure(argument[i], term2, exponent2);
        if (not option::checkCommutations
                or *Commutation(expr, argument[i]) == CSL_0) {
            // If we found the right term, it's done
            if (*term == term2) {
                argument[i] = pow_s(term, exponent->addition_own(exponent2));
                if (*argument[i] == CSL_1) {
                    argument.erase(argument.begin()+i);
                }
                return;
            }
        }
        else if (*term == term2) {
            argument[i] = pow_s(term, exponent->addition_own(exponent2));
            if (*argument[i] == CSL_1) {
                argument.erase(argument.begin()+i);
            }
            return;
        }
        else {
            // max is the position not reachable because of commutation not trivial
            max = i;
            break;
        }
    }

    // print();
    // cout << "INSERTING EFFECTIVELY ";
    // expr->print();
    // cout << "max = " << max << endl;
    // No term corresponds, we order correctly the new term in the sum
    for (int i = getNArgs()-1; i > max; --i)  {
        if (not option::checkCommutations
                or expr > argument[i]
                or *Commutation(expr, argument[i]) != CSL_0) {
            // argument[i]->print();
            // cout << "Comp1: " << (argument[i] < expr) << endl;
            // cout << "Comp: " << (expr > argument[i]) << endl;
            // cout << "[, ]: " << Commutation(expr, argument[i]) << endl;
            // cout << "INSERTING at pos " << i << endl;
            argument.insert(argument.begin()+i+1, expr);
            return;
        }
    }
    // print();
    // cout << "HERE 2\n";
    // cout << "INSERTING at max\n";

    // no term is more complicated than expr, we put it at the end
    argument.insert(argument.begin()+max+1, expr);
}


long double Prod::evaluateScalar() const
{
    double product=1;
    for (size_t i = 0; i < argument.size(); i++)
    {
        if (argument[i]->getDim() > 0) return 0;
        product *= argument[i]->evaluateScalar();
    }
    return product;
}

optional<Expr> Prod::evaluate(
        csl::eval::mode user_mode
        ) const
{
    bool number = true;
    Expr numericalRes = CSL_1;
    csl::vector_expr newArgs;
    for (size_t i = 0; i < argument.size(); i++) {
        optional<Expr> evalArg = argument[i]->evaluate(user_mode);
        Expr foo = evalArg.value_or(argument[i]);
        if (foo->getPrimaryType() == csl::PrimaryType::Numerical)
            numericalRes = numericalRes->multiplication_own(foo);
        else {
            newArgs.push_back(foo);
            number = false;
        }
    }
    if (number) 
        return numericalRes;
    else {
        newArgs.push_back(numericalRes);
        return prod_s(newArgs);
    }
}

unique_Expr Prod::copy_unique() const
{
    if (isIndexed())
        return make_unique<IProd>(argument, true);
    else
        return make_unique<Prod>(argument, true);
}

Expr Prod::deepCopy() const
{
    csl::vector_expr copyArgument(argument.size());
    for (size_t i = 0; i != argument.size(); ++i)
        copyArgument[i] = argument[i]->deepCopy();

    if (isIndexed()) {
        return csl::make_shared<IProd, alloc_iprod>(copyArgument, true);
    }
    else
        return csl::make_shared<Prod, alloc_prod>(copyArgument, true);
}

Expr Prod::refresh() const
{
    return prod_s(argument);
}

Expr Prod::deepRefresh() const
{
    csl::vector_expr refreshed(argument.size());
    for (size_t i = 0; i != argument.size(); ++i) {
        refreshed[i] = argument[i]->deepRefresh();
    }

    return prod_s(refreshed);

}

bool Prod::mergeNumericals()
{
    size_t posNonNumerical = 0;
    for (size_t i = 0; i != argument.size(); ++i) {
        if (argument[i] == CSL_0) {
            argument = {CSL_0};
            return true;
        }
        if (argument[i]->getPrimaryType() != csl::PrimaryType::Numerical) {
            posNonNumerical = i;
            break;
        }

        if (i > 0) {
            if (argument[i] != CSL_1)
                argument[0] = argument[0]->multiplication_own(argument[i]);
            argument.erase(argument.begin() + i);
            --i;
        }
    }

    Expr totalFactor = CSL_1;
    for (size_t i = posNonNumerical; i != argument.size(); ++i) {
        Expr factor = argument[i]->getNumericalFactor();
        if (factor == CSL_0) {
            argument = {CSL_0};
            return true;
        }
        else if (factor != CSL_1) {
            argument[i] = GetTerm(argument[i]);
            totalFactor = totalFactor->multiplication_own(factor);
        }
        if (argument[i] == CSL_1) {
            argument.erase(argument.begin() + i);
            --i;
        }
    }
    if (argument.empty())
        argument = {CSL_1};

    if (totalFactor != CSL_1) {
        if (argument[0]->getPrimaryType() == csl::PrimaryType::Numerical)
            argument[0] = argument[0]->multiplication_own(totalFactor);
        else
            argument.insert(argument.begin(), totalFactor);
    }
    if (not argument.empty() and argument[0] == CSL_1)
        argument.erase(argument.begin());

    return true;
}

bool Prod::mergeTerms()
{
    mergeNumericals();

    Expr factor;
    Expr term;
    bool matched = false;
    if (argument.empty()) {
        argument.push_back(CSL_1);
        return false;
    }
    bool indexed = isIndexed();
    bool remergeNumericals = false;
    for (size_t i = 0; i+1 < argument.size(); i++) {
        if (indexed
                and argument[i]->isIndexed() 
                and argument[i]->getIndexStructure().size() > 0) {
            matched = false;
            continue;
        }
        if (not matched) {
            factor = CSL_1;
            if (argument[i]->getType() == csl::Type::Pow) { //Pow 
                term = argument[i]->getArgument(1);
                if (term->getPrimaryType() == csl::PrimaryType::Numerical
                        or argument[i]->getArgument() == CSL_E) {
                    factor = term;
                    term = argument[i]->getArgument();
                }
                else 
                    term = argument[i];
            }
            else
                term = argument[i];
        }

        Expr factor2 = CSL_1;
        Expr term2;
        if (argument[i+1]->getType() == csl::Type::Pow) {  //Pow
            term2 = argument[i+1]->getArgument(1);
            if (term2->getPrimaryType() == csl::PrimaryType::Numerical
                    or argument[i+1]->getArgument() == CSL_E) {
                factor2 = term2;
                term2 = argument[i+1]->getArgument();
            }
            else 
                term2 = argument[i+1];
        }
        else
            term2 = argument[i+1];

        if (*term==term2) {
            matched = true;
            factor = factor->addition_own(factor2); //factor->addition_own(factor2);
            if (term->isInteger() 
                    and factor->getType() == csl::Type::IntFraction)
                continue;
            argument.erase(argument.begin()+i+1);
            argument[i] = pow_s(term, factor);
            if (*argument[i] == CSL_1) {
                argument.erase(argument.begin()+i);
                matched = false;
            }
            else if (argument[i]->getPrimaryType() 
                    == csl::PrimaryType::Numerical)
                remergeNumericals = true;
            --i;
        } 
        else if (matched) {
            --i;
            matched = false;
        }
    }

    if (remergeNumericals)
        mergeNumericals();
    if (argument.empty())
        argument.push_back(CSL_1);

    return false;
}

void Prod::orderTerms() 
{
    if (not option::checkCommutations) {
        csl::sort(argument.begin(), argument.end());
        return;
    }
    for (size_t i = 0; i != argument.size(); i++) {
        if (argument[i]->getType() == csl::Type::Prod) { 
            //csl::vector_expr newArg(argument.size() + argument[i]->size() - 1);
            //for (size_t j = 0; j != i; ++j)
            //    newArg[j] = argument[j];
            //for (size_t j = 0; j != argument[i]->size(); ++j)
            //    newArg[i+j] = (*argument[i])[j];
            //for (size_t j = i+1; j < argument.size(); ++j)
            //    newArg[argument[i]->size() + j - 1] = argument[j];
            //argument = std::move(newArg);
            //i += argument[i]->size()-1;
            vector_expr t_argument =
                argument[i]->getVectorArgument();

            argument.insert(argument.begin() + i+1,
                            t_argument.begin(),
                            t_argument.end());
            size_t old_i {i};
            i += argument[i]->getNArgs()-1;
            argument.erase(argument.begin() + old_i);
        }
    }
    if (argument.size() <= 1)
        return;
    if (GetCommutable(this)) {
        csl::sort(argument.begin(), argument.end());
        return;
    }
    for (size_t i = 0; i < argument.size()-1; i++) {
        size_t simpler = i;
        for (size_t j = i+1; j != argument.size(); j++) {
            if (option::checkCommutations 
                    and Commutation(argument[i], argument[j]) != CSL_0) {
                break;
            }
            if (*argument[j] < argument[simpler]) {
                simpler = j;
            }// argument[j] simpler
        }
        if (simpler != i) {
            if (not GetCommutable(argument[simpler])) {
                bool commutes = true;
                for (size_t j = i+1; j < simpler; ++j)
                    if (option::checkCommutations 
                         and Commutation(argument[simpler], argument[j]) != CSL_0) {
                        commutes = false;
                        break;
                    }
                if (not commutes)
                    continue;
            }
            if (not GetCommutable(argument[i])) {
                bool commutes = true;
                for (size_t j = i+1; j <= simpler; ++j)
                    if (option::checkCommutations 
                         and Commutation(argument[i], argument[j]) != CSL_0) {
                        commutes = false;
                        break;
                    }
                if (not commutes)
                    continue;
            }
            std::swap(argument[i], argument[simpler]);
        }
    }
}

optional<Expr> Prod::derive(Expr_info expr) const
{
    if (expr == nullptr)
        return CSL_0;
    std::vector<Expr> rep;
    rep.reserve(size());
    Expr intermediateResult = nullptr;
    for (size_t i = 0; i < size(); i++) {
        intermediateResult = CSL_0;
        if (not argument[i]->dependsOn(expr))
            continue;
        Expr derivative = Derived(argument[i], expr);
        if (derivative == CSL_0)
            continue;
        csl::vector_expr newArg = argument;
        newArg[i] = derivative;
        rep.push_back(prod_s(newArg));
    }
    return sum_s(rep);
}

optional<Expr> Prod::expand(bool full,
                            bool inPlace) const
{
    return expand_if([&](Expr const &){ return true; }, full, inPlace);
}

optional<Expr> Prod::expand_if(
        std::function<bool(Expr const&)> const& f,
        bool full,
        bool inPlace) const
{
    Expr copy = Copy(this);
    if (full)
        for (size_t i = 0; i != copy->size(); i++)
            copy->setArgument(ExpandedIf(copy[i], f, true, inPlace),i);
    std::vector<std::vector<Expr>> newAbstracts(1, {CSL_1});
    bool expanded = full;

    std::vector<Expr> inPlaceFactors;
    inPlaceFactors.reserve(argument.size());
    std::vector<Expr> args;
    std::vector<Expr> denom;
    args.reserve(copy->size());
    denom.reserve(copy->size());
    for (const auto &arg : copy) {
        if (csl::IsPow(arg) 
                and arg[1]->isInteger()
                and arg[1]->evaluateScalar() < 0) {
            denom.push_back(Copy(arg));
            if (arg[1] == CSL_M_1)
                denom.back() = arg[0];
            else
                denom.back()[1] *= -1;
        }
        else
            args.push_back(arg);
    }
    for (size_t i = 0; i != args.size(); ++i) {
        if (args[i]->getType() == csl::Type::Sum 
                and f(args[i])) {
            expanded = true;
            Expr sum = Copy(args[i]);
            std::vector<Expr> toDevelop;
            for (auto& subArg : *sum)
                if (f(subArg)) {
                    toDevelop.push_back(Copy(subArg));
                    subArg = CSL_0;
                }
            Expr remnant = Refreshed(sum);
            if (remnant != CSL_0) {
                toDevelop.push_back(remnant);
            }
            if (toDevelop.empty())
                continue;
            //nArgsBis = args.getArgument(i)->getNArgs();
            std::vector<std::vector<Expr>> foo;
            foo.reserve(toDevelop.size() * newAbstracts.size());
            for (const auto& arg : toDevelop)
                for (size_t k = 0; k < newAbstracts.size(); k++) {
                    std::vector<Expr> intermediate;
                    intermediate.reserve(size());
                    for (const auto &el : newAbstracts[k])
                        intermediate.push_back(el);
                    intermediate.push_back(arg);
                    foo.push_back(std::move(intermediate));
                }
            newAbstracts = std::move(foo);
        }
        else if (!inPlace or f(args[i])){
            for (size_t j = 0; j < newAbstracts.size(); j++)
                newAbstracts[j].push_back(args[i]);
        }
        else
            inPlaceFactors.push_back(args[i]);
    }
    std::optional<Expr> expandedDenom = (denom.empty()) ? 
        std::nullopt 
        : Prod(denom, true).expand_if(f, full, inPlace);
    if (not expanded and (!denom.empty() and not expandedDenom))
        return nullopt;
    if (!denom.empty() and !expandedDenom)
        expandedDenom = csl::prod_s(denom, true);
    std::vector<Expr> terms(newAbstracts.size());
    for (size_t i = 0; i != newAbstracts.size(); ++i) {
        if (!denom.empty())
            newAbstracts[i].push_back(1 / *expandedDenom);
        terms[i] = prod_s(newAbstracts[i]);
    }

    if (!inPlace)
        return sum_s(terms);
    else  {
        inPlaceFactors.push_back(sum_s(terms));
        return prod_s(inPlaceFactors);
    }
}

int Prod::isPolynomial(Expr_info expr) const
{
    bool polynomialTermFound = false;
    bool dependencyFound = false;
    int order = 0;
    for (size_t i = 0; i < size(); i++) {
        if (argument[i]->dependsExplicitlyOn(expr)) {
            order = argument[i]->isPolynomial(expr);
            if (order > 0) {
                for (size_t j = i+1; j < size(); ++j)
                    if (option::checkCommutations
                            and *Commutation(argument[j].get(), expr) != CSL_0) 
                        return 0;
                polynomialTermFound = true;
            }
            else 
                dependencyFound = true;
        }
    }
    if (polynomialTermFound and not dependencyFound)
        return order;
    return 0;
}

optional<Expr> Prod::getPolynomialTerm(Expr_info t_variable, int order) const
{
    int argument_order;
    Expr result = Copy(this);
    if (order == 0)
        return result;
    Expr copyVar = Copy(t_variable);
    for (size_t i = 0; i < size(); i++) {
        if (argument[i]->dependsExplicitlyOn(t_variable)) {
            argument_order = argument[i]->isPolynomial(t_variable);
            if (order == argument_order) {
                Expr foo = argument[i]->suppressTerm(
                        pow_s(copyVar, int_s(order)).get());
                result->setArgument(foo,i);
                break;
            }
        }
    }

    return Refreshed(result);
}

int Prod::getParity(Expr_info t_variable) const
{
    int parity = 1;
    for (size_t i = 0; i < size(); i++) {
        parity *= argument[i]->getParity(t_variable);
        if (parity == 0)
            return 0;
    }
    return parity;
}

bool Prod::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    const auto sz = size();
    if (sz == 1)
        return *argument[0]==expr;
    if (expr->getType() != csl::Type::Prod)
        return false;
    if (sz != expr->size())
        return false;

    vector<size_t> indicesLeft(sz);
    for (size_t i = 0; i != sz; ++i)
        indicesLeft[i] = i;

    bool dummySearch = false;
    if (Comparator::getDummyComparisonActive()) {
        dummySearch = true;
        Comparator::setDummyComparisonActive(false);
    }
    for (size_t i = 0; i < sz; ++i) {
        bool matched = false;
        Expr const &arg = argument[i];
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            Expr const &foo = expr->getArgument(indicesLeft[j]);
            if (option::checkCommutations 
                    and *Commutation(arg,foo) != CSL_0)
                    break;
            if (*arg == foo.get()) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
        }
        if (not matched) {
            if (not dummySearch)
                return false;
            if (Comparator::getDummyComparisonActive())
                return false;
            Comparator::setDummyComparisonActive(true);
            --i;
        }
        else if (dummySearch) {
            if (indicesLeft.empty()) {
                Comparator::setDummyComparisonActive(true);
                return true;
            }
        }
    }
    if (dummySearch)
        Comparator::setDummyComparisonActive(true);

    return true;
}

bool Prod::partialComparison(Expr_info expr) const
{
    if (size() == 1)
        return *argument[0]==expr;
    if (expr->getType() != csl::Type::Prod)
        return false;

    size_t t_size = expr->size();
    vector<size_t> indicesLeft(t_size);
    for (size_t i = 0; i < t_size; i++)
        indicesLeft[i] = i;

    Expr foo;
    for (size_t i = 0; i < argument.size(); i++) {
        bool matched = false;
        for (size_t j=0; j<indicesLeft.size(); j++) {
            foo = expr->getArgument(indicesLeft[j]);
            if (option::checkCommutations and *Commutation(argument[i],foo) != CSL_0)
                    break;
            if (*argument[i] == foo) {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = true;
                break;
            }
        }
        if (not matched)
            return false;
        else if (indicesLeft.size() == 0)
            break;
    }

    return true;
}

void applyOperator(Expr& product)
{
    CSL_ASSERT_SPEC(product->getType() == csl::Type::Prod,
              CSLError::TypeError,
              "should get a csl::Prod, " 
              + toString(product->getType()) + " given.");

    csl::vector_expr argument = product->getVectorArgument();
    if (argument.size() < 2)
        return;
    for (auto arg=argument.begin(); arg!=argument.end()-1; ++arg) {
        if ((*arg)->isAnOperator() and (*arg)->isEmpty()) {
            Expr right = prod_s(csl::vector_expr(arg+1,argument.end()), true);
            if ((*arg)->operatorAppliesOn(right.get())) {
                product = CSL_1;
                if (arg != argument.begin()) 
                    product = prod_s(csl::vector_expr(argument.begin(), arg),true);
                product = product * (*arg)->applyOperator(right);
                if (product->getType() == csl::Type::Prod)
                    applyOperator(product);
                return;
                
            }
            //Expr left = CSL_1;
            //Expr derivativeArgument = CSL_1;
            //Expr right = CSL_1;
            //if (arg != argument.begin())
            //    left = prod_s(csl::vector_expr(argument.begin(),arg), true); 
            //if (arg+1 != argument.end())
            //    right = prod_s(csl::vector_expr(arg+1, argument.end()), true);
            //if (left->getType() == csl::Type::Prod and
            //        left->getNArgs() > 1)
            //    applyOperator(left);
            //if (right->getType() == csl::Type::Prod and 
            //        right->getNArgs() > 1)
            //    applyOperator(right);
            //if (*right != CSL_1)
            //    product = left*((*arg)->applyOperator(right));
            //break;
        }
    }
}

Expr prod_s(const Expr& leftOperand, const Expr& rightOperand, bool explicitProd)
{
    if (leftOperand == CSL_0 or rightOperand == CSL_0)
        return CSL_0;
    if (leftOperand == CSL_1)
        return rightOperand;
    if (rightOperand == CSL_1)
        return leftOperand;
    bool indexed = (leftOperand->isIndexed() or rightOperand->isIndexed());
    //if (leftOperand->getPrimaryType() == csl::PrimaryType::Arbitrary
    //        or rightOperand->getPrimaryType() == csl::PrimaryType::Arbitrary) {
    //    if (not indexed)
    //        return csl::make_shared<Prod, alloc_prod>(csl::vector_expr({leftOperand,rightOperand}),true);
    //    else
    //        return csl::make_shared<IProd>(csl::vector_expr({leftOperand,rightOperand}),true);
    //}
    if (not explicitProd and leftOperand->isAnOperator() and leftOperand->isEmpty()
            and leftOperand->operatorAppliesOn(rightOperand.get())) {
        return leftOperand->applyOperator(rightOperand);
    }
    /*if (leftOperand->getType() == csl::Type::Derivative and
        leftOperand->isEmpty()) {
        return derivative_s(leftOperand->getOperand()*rightOperand,
                leftOperand->getVariable(), leftOperand->getOrder());
    }*/
    const auto lPType = leftOperand->getPrimaryType();
    if (lPType == csl::PrimaryType::Vectorial)
        return leftOperand->multiplication_own(rightOperand);
    const auto rPType = rightOperand->getPrimaryType();
    if (rPType == csl::PrimaryType::Vectorial)
        return rightOperand->multiplication_own(leftOperand,0);

    const auto lType = leftOperand->getType();
    const auto rType = rightOperand->getType();
    if (lType == csl::Type::Polynomial) {
        if (rType == csl::Type::Polynomial and explicitProd) {
            if (not indexed)
                return csl::make_shared<Prod, alloc_prod>(leftOperand, rightOperand);
            else
                return csl::make_shared<IProd, alloc_iprod>(leftOperand, rightOperand);
        }
        return leftOperand->multiplication_own(rightOperand);
    }
    if (rType == csl::Type::Polynomial) {
        if (lType == csl::Type::Polynomial and explicitProd) {
            if (not indexed)
                return csl::make_shared<Prod, alloc_prod>(leftOperand, rightOperand);
            else
                return csl::make_shared<IProd, alloc_iprod>(leftOperand, rightOperand);
        }
        return rightOperand->multiplication_own(leftOperand,0);
    }

    if (lPType == csl::PrimaryType::Numerical) 
        if (rPType == csl::PrimaryType::Numerical) 
            return leftOperand->multiplication_own(rightOperand);

    if (csl::option::canonicalSumNumericalFactor
            and lType == csl::Type::Sum
            and rPType == csl::PrimaryType::Numerical
            and rType != csl::Type::Float) {
        Expr copy = Copy(leftOperand);
        for (size_t i = 0; i != copy->size(); ++i)
            (*copy)[i] = (*copy)[i] * rightOperand;
        return copy;
    }
    if (csl::option::canonicalSumNumericalFactor
            and rType == csl::Type::Sum
            and lPType == csl::PrimaryType::Numerical
            and lType != csl::Type::Float) {
        Expr copy = Copy(rightOperand);
        for (size_t i = 0; i != copy->size(); ++i)
            (*copy)[i] = (*copy)[i] * leftOperand;
        return copy;
    }

    Expr foo;
    if (not indexed)
        foo = csl::make_shared<Prod, alloc_prod>(
                leftOperand, rightOperand, explicitProd);
    else
        foo = csl::make_shared<IProd, alloc_iprod>(
                leftOperand, rightOperand, explicitProd);

    if (not explicitProd)
        applyOperator(foo);
    if (foo->getPrimaryType() == csl::PrimaryType::MultiFunction) {
        const auto s = foo->size();
        if (s == 1)
            return foo->getArgument();
        else if (s == 0)
            return CSL_1;
    }

    return foo;
}

Expr prod_s(const csl::vector_expr& operands, bool explicitProd)
{
    //for (auto op=operands.begin(); op!=operands.end(); ++op) {
    //    // Checking if there is an Empty derivative in the arguments.
    //    if ((*op)->isAnOperator() and (*op)->isEmpty()) {
    //    //if ((*op)->getType() == csl::Type::Derivative 
    //    //        and (*op)->isEmpty()) {
    //        Expr left = CSL_1;
    //        if (op != operands.begin())
    //            left = prod_s(csl::vector_expr(operands.begin(),op));
    //        Expr right = CSL_1;
    //        if (op+1 != operands.end())
    //            right = prod_s(csl::vector_expr(op+1, operands.end()));
    //        return left*(*op)->applyOperator(right);
    //        //return left*derivative_s((*op)->getOperand()*right,
    //        //        (*op)->getVariable(), (*op)->getOrder());
    //    }
    //}
    for (const auto& op : operands) {
        if (op == CSL_0)
            return CSL_0;
    }
    if (operands.size() == 2)
        return prod_s(operands[0], operands[1], explicitProd);

    bool indexed = false;
    for (const auto& op : operands)
        if (op->isIndexed()) {
            indexed = true;
            break;
        }
    Expr result;
    if (not indexed)
        result = csl::make_shared<Prod, alloc_prod>(operands, explicitProd);
    else 
        result = csl::make_shared<IProd, alloc_iprod>(operands, explicitProd);
    if (not explicitProd) {
        applyOperator(result);
    }
    if (result->getPrimaryType() == csl::PrimaryType::MultiFunction) {
        const auto s = result->size();
        if (s == 1)
            return result->getArgument();
        else if (s == 0)
            return CSL_1;
    }

    return result;
}

Expr fraction_s(const Expr& leftOperand, const Expr& rightOperand)
{
    const auto rPType = rightOperand->getPrimaryType();
    if (rPType == csl::PrimaryType::Numerical and 
            rightOperand == CSL_0) {
        std::cerr << leftOperand << std::endl;
        std::cerr << rightOperand << std::endl;
        std::cerr << "Warning division by zero : " << leftOperand << " / 0.\n";
        // CSL_ASSERT_SPEC(
        //         false, 
        //         CSLError::ValueError,
        //         "Division by zero !"
        //         )
        return CSL_INF;
    }
    // if (leftOperand == rightOperand)
    //     return CSL_1;

    const auto lPType = leftOperand->getPrimaryType();
    const auto rType = rightOperand->getType();
    if (lPType == csl::PrimaryType::Numerical and 
            rPType == csl::PrimaryType::Numerical)
    {
        if (rightOperand->getType() == csl::Type::IntFraction)
            return leftOperand->multiplication_own(
                    intfraction_s(rightOperand->getDenom(), rightOperand->getNum())
                    );
        else if (rightOperand->isInteger())
            return leftOperand->multiplication_own(
                    intfraction_s(1, (int)rightOperand->evaluateScalar())
                    );
        else
            return leftOperand->division_own(rightOperand);
    }
    if (leftOperand == CSL_0 || rightOperand == CSL_1)
        return leftOperand;
    if (rightOperand->isInteger())
        return prod_s(intfraction_s(1,rightOperand->evaluateScalar()),leftOperand);
    if (rType == csl::Type::IntFraction)
        return prod_s(intfraction_s(rightOperand->getDenom(),
                                  rightOperand->getNum()),
                      leftOperand);

    return prod_s(leftOperand,pow_s(rightOperand,CSL_M_1));
}

///////////////////////////////////////////////////
/*************************************************/
// Class Pow                                     //
/*************************************************/
///////////////////////////////////////////////////

Pow::Pow(const Expr& leftOperand,
         const Expr& rightOperand,
         bool        explicitPow)
    :AbstractDuoFunc()
{
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    if (not explicitPow)
        mergeTerms();
}

Expr Pow::getNumericalFactor() const
{
    if (argument[1]->isInteger()) {
        if (Expr factor = argument[0]->getNumericalFactor();
                factor != CSL_1) {
             Expr res = pow_s(factor, argument[1]);
             return (res->getPrimaryType() == csl::PrimaryType::Numerical) ?
                 res : CSL_1;
        }
    }
    return CSL_1;
}

optional<Expr> Pow::getTerm() const
{
    if (getNumericalFactor() != CSL_1)
        return pow_s(GetTerm(argument[0]), argument[1]);
    return nullopt;
}

int Pow::getOrderOf(Expr_info expr) const
{
    if (*argument[0] == expr and argument[1]->isInteger())
        return argument[1]->evaluateScalar();
    return 0;
}

int Pow::getNFactor() const
{
    if (argument[1]->getPrimaryType() != csl::PrimaryType::Numerical
            or argument[1]->getType() == csl::Type::Complex)
        return 1;
    return 2*floor(abs(argument[1]->evaluateScalar()))+1;
}

csl::vector_expr Pow::getFactors() const
{
    //return Abstract::getFactors();
    if (argument[1]->getPrimaryType() != csl::PrimaryType::Numerical)
        return Abstract::getFactors();
    csl::vector_expr foo(0);
    if (argument[1]->getType() == csl::Type::Integer or
            argument[1]->getType() == csl::Type::Float)
    {
        int a = argument[1]->evaluateScalar();
        if (a == 0)
            return foo;
        for (int i=sgn(a); abs(i)<=abs(a); i+=sgn(a))
            foo.push_back(pow_s(argument[0],int_s(i)));
    }
    else {
        long long int num = argument[1]->getNum();
        long long int denom = argument[1]->getDenom();
        if (num < 0) {
            if (denom < 0) {
                num = -num;
                denom = -denom;
            }
        }
        else if (denom < 0) {
            num = -num;
            denom = -denom;
        }
        for (int delta_num=0;
                sgn(num+delta_num)==sgn(num);
                delta_num-=sgn(num)*denom)
            foo.push_back(pow_s(argument[0],intfraction_s(num+delta_num,denom)));

        if (PGCD(num,denom) != 1) {
            for (int i=sgn(num); abs(i)<abs(num*1.)/denom; i+=sgn(num))
                foo.push_back(pow_s(argument[0], int_s(i)));
        }
    }
    return foo;
}

void Pow::getExponents(
        std::vector<Expr> const &factors,
        std::vector<Expr>       &exponents
        ) const
{
    for (size_t i = 0; i != factors.size(); ++i) 
        if (factors[i] == argument[0]) {
            exponents[i] = argument[1];
            return;
        }
}

bool Pow::askTerm(Expr_info expr, bool exact) const
{
    if (exact) return ((*this)==expr);
    if (expr->getType() == csl::Type::Pow) {
        Expr a = expr->getArgument(1);
        Expr b = argument[1];
        if (*argument[0] != expr->getArgument(0))
            return false;
        if (*a == b)
            return true;
        if (a->getPrimaryType() != csl::PrimaryType::Numerical or 
                b->getPrimaryType() != csl::PrimaryType::Numerical)
            return false;
        if (a->getType() == csl::Type::Complex or 
                b->getType() == csl::Type::Complex)
            return false;

        if (a->getType() == csl::Type::Integer or a->getType() == csl::Type::Float){
            double a_val = a->evaluateScalar();
            double b_val = b->evaluateScalar();

            return (abs(b_val)>abs(a_val) and sgn(a_val)==sgn(b_val));
        }
        else if (a->getType() == csl::Type::IntFraction) {
            int a_denom = a->getDenom();
            int b_denom = b->getDenom();
            double a_val = a->evaluateScalar();
            double b_val = b->evaluateScalar();
            return (a_denom==b_denom and
                    abs(a_val)<abs(b_val) and
                    sgn(a_val)==sgn(b_val));
        }
        return ((*this)==expr);
    }
    else
        return (*argument[0]==expr and
                (argument[1]->isInteger() or
                 argument[1]->getType() == csl::Type::IntFraction) and
                argument[1]->evaluateScalar() > 0);
}

Expr Pow::suppressTerm(Expr_info expr) const
{
    // Caution here: we do not re-test if the term corresponds (we do not call askTerm
    // we suppose it has been done before calling this function...)
    if (expr->getType() == csl::Type::Pow)
    {
        return pow_s(argument[0],minus_(argument[1], expr->getArgument(1)));
    }
    else return pow_s(argument[0],minus_(argument[1], CSL_1));
}

std::optional<Expr> Pow::suppressExponent(
        Expr const &factor,
        Expr const &exponent
        ) const
{
    return (factor == argument[0] and exponent == argument[1]) ?
        std::optional<Expr>(CSL_1) : std::nullopt;
}

void Pow::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (lib and LibraryGenerator::isQuadruplePrecision()) {
        if (argument[0]->isReal() && argument[1]->isReal())
            out << "powq(";
        else
            out << "cpowq(";
        argument[0]->print(1, out, lib);
        out << ", ";
        argument[1]->print(1, out, lib);
        out << ")";
        if (mode == 0)
            out << endl;
        return;
    }
    else if (lib) {
        out << "std::pow(";
        argument[0]->print(1, out, lib);
        out << ", ";
        argument[1]->print(1, out, lib);
        out << ")";
        if (mode == 0)
            out << endl;
        return;
    }
    if (mode > 4) // Priority lesser than the previous operation: brackets
        out << "(";
    argument[0]->print(4, out);
    out << "^";
    argument[1]->print(4, out);
    if (mode > 4)
        out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

void Pow::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::pow_s(";
    for (size_t i = 0; i != size(); ++i) {
        argument[i]->printCode(3, out);
        if (i+1 != size())
            out << ", ";
    }
    out << ")";
}

string Pow::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode > 4) // Priority lesser than the previous operation: brackets
        sout<<"\\left(";
    sout<<argument[0]->printLaTeX(4);
    sout<<"^{";
    sout<<argument[1]->printLaTeX(1);
    sout<<"}";
    if (mode > 4)
        sout<<"\\right)";
    if (mode == 0)
        sout<<endl;

    return sout.str();
}

LibDependency Pow::getLibDependency() const
{
    return LibDependency::cmath();
}

long double Pow::evaluateScalar() const
{
    if (argument[0]->getDim() > 0 or argument[1]->getDim() > 0) return 0;
    return pow(argument[0]->evaluateScalar(), argument[1]->evaluateScalar());
}

optional<Expr> Pow::evaluate(
        csl::eval::mode user_mode
        ) const
{
    Expr foo1 = Evaluated(argument[0], user_mode);
    Expr foo2 = Evaluated(argument[1], user_mode);
    if (foo1->getPrimaryType() == csl::PrimaryType::Numerical
            and foo2->getPrimaryType() == csl::PrimaryType::Numerical) {
        return foo1->exponentiation_own(foo2);
    }

    return pow_s(foo1,foo2);
}

unique_Expr Pow::copy_unique() const
{
    return make_unique<Pow>(argument[0], argument[1], true);
}

Expr Pow::deepCopy() const
{
    return csl::make_shared<Pow, alloc_pow>(argument[0]->deepCopy(),
                            argument[1]->deepCopy(),
                            true);
}

Expr Pow::refresh() const
{
    return pow_s(argument[0], argument[1]);
}

Expr Pow::deepRefresh() const
{
    return pow_s(argument[0]->deepRefresh(),
                argument[1]->deepRefresh());
}

bool Pow::mergeTerms()
{
    bool simplified = false;
    Expr arg;
    if (argument[0]->getType() == csl::Type::Pow) {
        // (a^b)^c = a^(b*c)
        argument[1] = prod_s(argument[0]->getArgument(1),argument[1]);
        argument[0] = argument[0]->getArgument();
        mergeTerms();
        return true;
    }
    else if (argument[0] == -CSL_1 and argument[1]->isInteger())
    {
        if (static_cast<int>(argument[1]->evaluateScalar()) % 2 == 0)
            argument[0] = CSL_1;
        else
            argument[0] = CSL_M_1;
        argument[1] = CSL_1;
    }
    else if (argument[0]->isInteger()
            and argument[1]->isInteger()
            and abs(argument[1]->evaluateScalar()) < 10) {
        int res = pow(argument[0]->evaluateScalar(),
                      abs(argument[1]->evaluateScalar()));
        argument[0] = (argument[1]->evaluateScalar() < 0) ?
                      intfraction_s(1, res) : int_s(res);
        argument[1] = CSL_1;
    }
    else if (argument[0]->getType() == csl::Type::IntFraction
            and argument[1]->getPrimaryType() == csl::PrimaryType::Numerical
            and argument[1]->getType() == csl::Type::Complex
            and argument[1]->evaluateScalar() < 0) {
        argument[0] = intfraction_s(argument[0]->getDenom(),
                                    argument[0]->getNum());
        argument[1] = -argument[1];
        mergeTerms();
        return true;
    }
    else if (argument[1]->getPrimaryType() == csl::PrimaryType::Numerical
            and argument[0]->getType() == csl::Type::IntFraction
            and abs(argument[0]->getNum()) == 1) {
        argument[0] = 1 / argument[0];
        argument[1] = -argument[1];
    }
    else if (argument[0]->getType() == csl::Type::IntFraction
            and argument[1] == CSL_HALF) {
        double valueNum = sqrt(argument[0]->getNum());
        double valueDenom = sqrt(argument[0]->getDenom());
        if (argument[0]->getNum() > 0 
                and round(valueNum) == valueNum
                and round(valueDenom) == valueDenom) {
            argument[0] = intfraction_s(valueNum, valueDenom);
            argument[1] = CSL_1;
        }
        else if (valueNum == round(valueNum) 
                or valueDenom == round(valueDenom)) {
            argument[0] = sqrt_s(int_s(argument[0]->getNum()))
                        / sqrt_s(int_s(argument[0]->getDenom()));
            argument[1] = CSL_1;
        }
    }
    else if (argument[0]->getType() == csl::Type::IntFraction
            and argument[1]->isInteger()
            and std::fabs(argument[1]->evaluateScalar()) < 4) {
        const long long num = round(
                csl::pow_s(argument[0]->getNum(), argument[1])->evaluateScalar());
        const long long denom = round(
                csl::pow_s(argument[0]->getDenom(), argument[1])->evaluateScalar());
        argument[0] = csl::intfraction_s(num, denom);
        argument[1] = CSL_1;
    }
    else if (argument[0]->getPrimaryType() == csl::PrimaryType::Numerical and
            argument[1]->getType() == csl::Type::IntFraction and
            std::abs(argument[1]->getNum()) == 1 and
            argument[1]->getDenom() == 2)
    {
        if (argument[0]->isInteger()) {
            double value = sqrt(argument[0]->evaluateScalar());
            if (value == round(value)) {
                argument[0] = (argument[1]->getNum() < 0) ? 
                    intfraction_s(1, value) : int_s(value);
                argument[1] = CSL_1;
            }
        }
    }
    else if (argument[0]->getType() == csl::Type::Prod
        && csl::IsInteger(argument[1])) {
        // (a*b*...)^c = a^c*b^c*....
        csl::vector_expr newArgument;
        newArgument.reserve(argument[0]->size());
        for (const auto &arg : argument[0])
            newArgument.push_back(pow_s(arg, argument[1]));
        argument[1] = CSL_1;
        argument[0] = prod_s(newArgument);
    }
    else if (argument[0]->getType() == csl::Type::Prod) {
        // (a*b*...)^c = a^c*b^c*....
        const auto &arg = argument[0];
        const auto &exponent = argument[1];
        std::vector<csl::Expr> out;
        std::vector<csl::Expr> in;
        out.reserve(arg->size());
        in.reserve(arg->size());
        for (const auto &prodArg : arg) {
            if (csl::IsNumerical(prodArg) && prodArg->isReal()
                    && prodArg->evaluateScalar() > 0) {
                out.push_back(csl::pow_s(prodArg, exponent));
            }
            else if (csl::IsPow(prodArg)) {
                csl::Expr totalExponent = exponent * prodArg[1];
                if (csl::IsInteger(totalExponent))
                    out.push_back(csl::pow_s(prodArg, exponent));
                else
                    in.push_back(prodArg);
            }
            else 
                in.push_back(prodArg);
        }
        if (out.size() > 0) {
            out.push_back(csl::pow_s(csl::prod_s(in), exponent));
            argument[0] = csl::prod_s(out);
            argument[1] = CSL_1;
        }
    }
    else if (argument[1] == CSL_1/2 or argument[1] == -CSL_1/2) {
        if (argument[0]->isInteger()) {
            if (double value = sqrt(argument[0]->evaluateScalar());
                    value == round(value)) {
                argument[0] = (abs(argument[1]->evaluateScalar()) < 0) ? 
                              intfraction_s(1, value) : int_s(value);
                argument[1] = CSL_1;
            }
        }
    }

    return simplified;
}

optional<Expr> Pow::derive(Expr_info expr) const
{
    return log_s(argument[0])*Derived(argument[1], expr)*Copy(this)
         + argument[1]*Derived(argument[0], expr)*
            pow_s(argument[0], argument[1]-1);
}

optional<Expr> Pow::expand(bool full,
                           bool inplace) const
{
    if (full) {
        optional<Expr> opt1 = argument[0]->expand(true, inplace);
        optional<Expr> opt2 = argument[1]->expand(true, inplace);
        if (not full and not opt1 and not opt2)
            return nullopt;
        Expr foo1 = opt1.value_or(argument[0]);
        Expr foo2 = opt2.value_or(argument[1]);
        if (foo2->isInteger() 
                and foo2->evaluateScalar() > 0 
                and foo1->getType() == csl::Type::Sum) {
            double value = foo2->evaluateScalar();
            int iMax = round(abs(value));
            Expr result = CSL_1;
            for (int i=0; i<iMax; i++)
                result = prod_s(result, foo1,true);
            Expand(result, full);

            return result;
        }
        Expr result = pow_s(foo1,foo2);

        return result;
    }

    return nullopt;
}

optional<Expr> Pow::expand_if(std::function<bool(Expr const&)> const& f,
                              bool full,
                              bool inplace) const
{
    if (full) {
        optional<Expr> opt1 = argument[0]->expand_if(f, true, inplace);
        optional<Expr> opt2 = argument[1]->expand_if(f, true, inplace);
        if (not full and not opt1 and not opt2)
            return nullopt;
        Expr foo1 = opt1.value_or(argument[0]);
        Expr foo2 = opt2.value_or(argument[1]);
        if (foo2->isInteger() 
                and foo2->evaluateScalar() > 0 
                and foo1->getType() == csl::Type::Sum
                and f(foo1)) {
            double value = foo2->evaluateScalar();
            int iMax = round(abs(value));
            Expr result = CSL_1;
            for (int i=0; i<iMax; i++)
                result = prod_s(result, foo1,true);
            Expand(result, full);

            return result;
        }
        Expr result = pow_s(foo1,foo2);

        return result;
    }

    return nullopt;
}

int Pow::isPolynomial(Expr_info expr) const
{
    if (argument[1]->isInteger())
        if (*argument[0]==expr)
                return (int)argument[1]->evaluateScalar();
    return 0;
}

optional<Expr> Pow::getPolynomialTerm(Expr_info t_variable, int order) const
{
    if (*argument[0] == t_variable and
            argument[1]->isInteger() and
            argument[1]->evaluateScalar() == order)
        return CSL_1;
    else if (order == 0)
        return nullopt;
    else
        return CSL_0;
}

int Pow::getParity(Expr_info expr) const
{
    int parity = argument[0]->getParity(expr);
    int parity2 = argument[1]->getParity(expr);
    if (parity == 1 and parity2 == 1)
        return 1;
    if (parity == 0 or parity2 == 0)
        return 0;
    if (not argument[1]->isInteger())
        return 0;
    
    double value = argument[1]->evaluateScalar();
    return -2*((int)value%2)+1;
}

bool Pow::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != csl::Type::Pow)
        return false;
    return (*argument[0] == expr->getArgument(0).get() 
            && *argument[1] == expr->getArgument(1).get());
}

Expr pow_s(const Expr& leftOperand, const Expr& rightOperand)
{
    if (leftOperand->getPrimaryType() == csl::PrimaryType::Arbitrary
            or rightOperand->getPrimaryType() == csl::PrimaryType::Arbitrary)
        return csl::make_shared<Pow, alloc_pow>(leftOperand,rightOperand);
    if (*leftOperand == CSL_I and
            (rightOperand->isInteger()))
    {
        int n = rightOperand->evaluateScalar();
        Expr factor = CSL_1;
        if (n < 0) {
            n = -n;
            factor = pow_s(CSL_M_1, int_s(n));
        }
        if (n % 2 == 1) {
            return factor * pow_s(CSL_M_1 , int_s((n-1)/2)) * CSL_I;
        }
        return pow_s(CSL_M_1 , int_s(n/2));
    }
    if (leftOperand->isIndexed() 
            and rightOperand->isInteger()
            and rightOperand->evaluateScalar() > 0) {
        csl::vector_expr terms((size_t)rightOperand->evaluateScalar());
        for (size_t i = 0; i != terms.size(); ++i) {
            terms[i] = DeepCopy(leftOperand);
            RenameIndices(terms[i]);
        }
        return prod_s(terms);
    }
    if (leftOperand->getType() == csl::Type::NumericalEval
            and rightOperand->getPrimaryType() == csl::PrimaryType::Numerical)
        return leftOperand->exponentiation_own(rightOperand);
    if (rightOperand->getType() == csl::Type::NumericalEval
            and leftOperand->getPrimaryType() == csl::PrimaryType::Numerical)
        return leftOperand->exponentiation_own(rightOperand);

    if (leftOperand->isInteger()
            and rightOperand->getType() == csl::Type::IntFraction) {
        int a = leftOperand->evaluateScalar();
        int num = rightOperand->getNum();
        int denom = rightOperand->getDenom();
        if ((num < 0 or num > denom) and a != -1) {
            int ratio = std::abs(num) / denom;
            int modulo = std::abs(num) % denom;
            Expr factor = CSL_1;
            if (a < 0) {
                a = -a;
                factor = csl::pow_s(-1, rightOperand);
            }
            if (num > 0)
                return factor*std::pow(a, ratio)
                    *csl::pow_s(a, csl::intfraction_s(modulo, denom));
            return factor*csl::intfraction_s(1, std::pow(a, (1 + ratio)))
                *csl::pow_s(a, csl::intfraction_s(denom-modulo, denom));
        }
    }

    Expr foo = csl::make_shared<Pow, alloc_pow>(leftOperand, rightOperand);
    Expr arg0 = foo->getArgument(0);
    Expr arg1 = foo->getArgument(1);
    if (*arg0 == CSL_INF) {
        std::cerr << leftOperand << std::endl;
        std::cerr << rightOperand << std::endl;
        std::raise(SIGSEGV);
        return CSL_INF;
    }
    if (*arg0 == CSL_0
            and arg1->getPrimaryType() == csl::PrimaryType::Numerical
            and arg1->getType() != csl::Type::Complex
            and arg1->evaluateScalar() < 0) {
        std::cerr << leftOperand << std::endl;
        std::cerr << rightOperand << std::endl;
        std::raise(SIGSEGV);
        return CSL_INF;
    }
    if (*arg0 == CSL_0
            and arg1->getType() == csl::Type::Complex) {
        return CSL_UNDEF;
    }
    if (*arg1==CSL_0) { // x^0 = 1 
        if (*arg0 == CSL_0) // 0^0
            return CSL_UNDEF;
        return CSL_1;
    }
    if (*arg0==CSL_0) { // 0^x = 0
        if (*arg1 == CSL_0) // 0^0
            return CSL_UNDEF;
        return CSL_0;
    }
    if (*arg0==CSL_1) // 1^x = 1
        return CSL_1;
    if (*arg1==CSL_1) // x^1 = x
        return arg0;

    return foo;
}

Expr pow_s(Expr const& leftOperand,
           int         value)
{
    return pow_s(leftOperand, int_s(value));
}

Expr sqrt_s(const Expr& operand)
{
    return pow_s(operand, intfraction_s(1,2));
}

Expr sqrt_s(int number)
{
    return sqrt_s(int_s(number));
}

///////////////////////////////////////////////////
/*************************************************/
// Class Polynomial                              //
/*************************************************/
///////////////////////////////////////////////////

Polynomial::Polynomial(const Expr& expr, const Expr& t_variable)
    :AbstractMultiFunc()
{
    variable = t_variable;
    Expr exprBis;
    if (expr->getType() == csl::Type::Polynomial and
            *t_variable != expr->getVariable())
        exprBis = expr->getRegularExpression();
    else
        exprBis = expr;

    if (exprBis->getType() == csl::Type::Sum) {
        int max_order = 0;
        argument = csl::vector_expr(1);
        argument[0] = CSL_0;
        Expr foo;
        for (int i=0; i<exprBis->getNArgs(); i++) {
            foo = exprBis->getArgument(i);
            int argument_order = foo->isPolynomial(t_variable.get());
            if (argument_order > max_order)
                for (int i=0; i<argument_order-max_order; i++)
                    argument.push_back(CSL_0);
            argument[argument_order] = 
                sum_s(argument[argument_order],
                     GetPolynomialTerm(foo,
                                       t_variable.get(),
                                       argument_order));
        }
    }
    else {
        int max_order = exprBis->isPolynomial(t_variable.get());
        argument = csl::vector_expr(0);
        for (int i=0; i<max_order+1; i++)
            argument.push_back(CSL_0);
        argument[max_order] = GetPolynomialTerm(exprBis,
                                                t_variable.get(),
                                                max_order);
    }
    mergeTerms();
}

Polynomial::Polynomial(const csl::vector_expr& terms, const Expr& t_variable): AbstractMultiFunc()
{
    argument = terms;
    variable = t_variable;
    mergeTerms();
}

bool Polynomial::mergeTerms()
{
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getType() == csl::Type::Polynomial)
            argument[i] = argument[i]->getRegularExpression();
    }
    if (argument.empty())
        return false;
    for (size_t i = argument.size() - 1; i >= 1; i--) {
        if (*argument[i] == CSL_0) {
            argument.erase(argument.begin()+i);
        }
        else 
            break;
    }
    return false;
}

int Polynomial::getOrder() const
{
    return static_cast<int>(argument.size()) - 1;
}

int Polynomial::getOrderOf(Expr_info expr) const
{
    return getRegularExpression()->getOrderOf(expr);
}

Expr Polynomial::getVariable() const
{
    return variable;
}

bool Polynomial::isIndexed() const
{
    return (variable->isIndexed() or AbstractMultiFunc::isIndexed());
}

IndexStructure Polynomial::getIndexStructure() const
{
    // If the expression has a non trivial index structure, we suppose 
    // (the program should maintain that property) that all arguments have the 
    // same structure, in particular the first
    if (argument.size() > 0 
            and argument[0]->getPrimaryType() == csl::PrimaryType::Indicial)
        return argument[0]->getIndexStructure();

    return IndexStructure();
}

void Polynomial::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (mode > 1) // Priority lesser than the previous operation: brackets
        out<<"(";
    for (size_t i = 0; i < argument.size(); i++) {
        if (*argument[i] != CSL_0) {
            if (i == 0 or *argument[i] != CSL_1)
                argument[i]->print(2, out, lib);
            if (i > 0) {
                if (*argument[i] != CSL_1)
                    out<<"*";
                variable->print(4, out, lib);
                if (i > 1)
                    out<<"^"<<i;
            }
            if (i+1 < argument.size()) 
                out<<" + ";
        }
    }
    if (mode > 1)
        out<<")";
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void Polynomial::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::polynomial_s(";
    for (size_t i = 0; i != size(); ++i) {
        argument[i]->printCode(1, out);
        out << ", ";
    }
    variable->printCode(1, out);
    out << ")";
}

string Polynomial::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode > 1) // Priority lesser than the previous operation: brackets
        sout<<"\\left(";
    for (size_t i = 0; i < argument.size(); i++)
    {
        sout<<argument[i]->printLaTeX(1);
        if (i+1 < argument.size())
            sout<<"+";
    }
    if (mode > 1)
        sout<<"\\right)";
    if (mode == 0)
        sout<<endl;

    return sout.str();
}

long double Polynomial::evaluateScalar() const
{
    double sum=0;
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i]->getDim() > 0)
            return 0;
        sum += argument[i]->evaluateScalar()*pow(variable->evaluateScalar(),i);
    }
    return sum;
}

optional<Expr> Polynomial::evaluate(
        csl::eval::mode user_mode
        ) const
{
    return getRegularExpression()->evaluate(user_mode);
}

unique_Expr Polynomial::copy_unique() const
{
    return make_unique<Polynomial>(argument, variable);
}

Expr Polynomial::deepCopy() const
{
    csl::vector_expr copyArgument(argument.size());
    for (size_t i = 0; i != argument.size(); ++i)
        copyArgument[i] = argument[i]->deepCopy();

    return csl::make_shared<Polynomial>(copyArgument, variable->deepCopy());
}

Expr Polynomial::refresh() const
{
    return polynomial_s(argument, variable);
}

Expr Polynomial::deepRefresh() const
{
    csl::vector_expr refreshed(argument.size());
    for (size_t i = 0; i != argument.size(); ++i)
        refreshed[i] = argument[i]->deepRefresh();

    return polynomial_s(refreshed, variable->deepRefresh());
}


optional<Expr> Polynomial::derive(Expr_info expr) const
{
    Expr rep;
    csl::vector_expr newTerms = argument;
    if (*expr == variable) {
        Expr foo = Derived(newTerms[0], expr);
        newTerms.erase(newTerms.begin());
        if (*foo != CSL_0) {
            if (newTerms.size() > 0)
                newTerms[0] = sum_s(newTerms[0], foo);
            else
                newTerms.insert(newTerms.begin(),foo);
        }
        for(size_t i=0; i<newTerms.size(); i++)
            newTerms[i] = prod_s(int_s(i+1), newTerms[i]);
        
        return polynomial_s(newTerms, variable);
    }
    for (size_t i = 0; i < argument.size(); i++)
        newTerms[i] = Derived(newTerms[i], expr);
    for (int i=newTerms.size()-1; i>= 0; i--)
        if (*newTerms[i] == CSL_0)
            newTerms.erase(newTerms.begin()+i);

    return polynomial_s(newTerms, variable);
}

int Polynomial::getParity(Expr_info t_variable) const
{
    int parity = 0;
    int newParity;
    if (*variable == t_variable) {
        if (*argument[0] != CSL_0) parity = argument[0]->getParity(t_variable);
        for (size_t i = 1; i < argument.size(); i++) {
            if (*argument[i] != CSL_0) {
                if (parity == 0)
                    parity = -(i%2);
                else {
                    newParity = -2*(i%2)+1;
                    if (parity != newParity)
                        return 0;
                }
            }
        }
        if (parity == 0)
            return 1;

        return parity;
    }
    parity = argument[0]->getParity(t_variable);
    if (parity == 0 or variable->getParity(t_variable) == 0)
        return 0;
    for (size_t i = 1; i < argument.size(); i++) {
        newParity = argument[i]->getParity(t_variable);
        if (newParity != parity)
            return 0;
    }

    return parity*variable->getParity(t_variable);
}

Expr Polynomial::getRegularExpression() const
{
    Expr result = CSL_0;
    for (size_t i = 0; i < argument.size(); i++)
        result = sum_s(result,prod_s(argument[i],pow_s(variable,int_s(i))));

    return result;
}

Expr Polynomial::addition_own(const Expr& expr) const
{
    Expr foo;
    if (expr->getType() != csl::Type::Polynomial)
        foo = polynomial_s(expr, variable);
    else 
        foo = expr;
    csl::vector_expr foo_argument = argument;
    size_t foo_nArgs = argument.size();
    if (*variable == foo->getVariable()) {
        for (size_t i = 0; i < foo->size(); i++) {
            if (i >= foo_nArgs) {
                foo_nArgs++;
                foo_argument.push_back(CSL_0);
            }
            foo_argument[i] = sum_s(foo_argument[i], foo->getArgument(i));
        }
        return polynomial_s(foo_argument, variable);
    }

    return sum_s(getRegularExpression(),foo->getRegularExpression());
}

Expr Polynomial::multiplication_own(const Expr& expr, bool side) const
{
    Expr foo;
    if (expr->getType() != csl::Type::Polynomial)
        foo = polynomial_s(expr, variable);
    else 
        foo = expr;

    //side = true;
    if (*variable == foo->getVariable()) {
        csl::vector_expr foo_argument(argument.size() + foo->size()-1,CSL_0);
        for (size_t i = 0; i < foo->size(); i++) {
            if (option::checkCommutations
                    and *Commutation(variable,foo->getArgument(i)) != CSL_0){
                if (side)
                    return prod_s(getRegularExpression(),foo->getRegularExpression());
                else
                    return prod_s(foo->getRegularExpression(),getRegularExpression());
            }
            for (size_t j = 0; j < argument.size(); j++) {
                if (option::checkCommutations
                        and *Commutation(variable,argument[j]) != CSL_0){
                    if (side)
                        return prod_s(getRegularExpression(),foo->getRegularExpression());
                    else
                        return prod_s(foo->getRegularExpression(),getRegularExpression());
                }
                if (side)
                    foo_argument[i+j] = sum_s(foo_argument[i+j],
                                              prod_s(argument[j],
                                                     foo->getArgument(i)));
                else
                    foo_argument[i+j] = sum_s(foo_argument[i+j],
                                              prod_s(foo->getArgument(i),
                                                     argument[j]));
            }
        }

        return polynomial_s(foo_argument, variable);
    }

    if (side)
        return prod_s(getRegularExpression(),foo->getRegularExpression());
    else
        return prod_s(foo->getRegularExpression(),getRegularExpression());
}

Expr Polynomial::division_own(const Expr& expr) const
// Euclidean division of a polynomial by another. If expr is not
// a polynomial (or is a polynomial of a different variable, it is first
// converted in a polynomial of the same variable.
{
    Expr foo = Copy(expr);
    if (foo->getType() != csl::Type::Polynomial or *foo->getVariable() != variable)
        foo = polynomial_s(foo,variable);
    if (foo->size() > argument.size())
        return Copy(this);

    // If the variable does not commute with one argument, we cannot 
    // divide the polynomial
    /*
    for (int i=0; i<foo->getNArgs(); ++i)
        if (*Commutation(foo->getArgument(i),variable) != CSL_0)
            return Copy(this);
    for (int i=0; i<nArgs; ++i)
        if (*Commutation(argument[i],variable) != CSL_0)
            return Copy(this);
    */
    //cout<<"Divinding "; print();
    //cout<<"by : "; foo->print();
    
    Expr q = polynomial_s(CSL_0, variable);
    Expr r = polynomial_s(argument, variable);
    Expr t;
    int iter = 0;
    int maxiter = 10;
    while(*r!=CSL_0 and r->getNArgs()>=foo->getNArgs() and iter < maxiter) {
        //cout<<"q = "; q->print();
        //cout<<"r = "; r->print();
        t = fraction_s(r->getArgument(r->getNArgs()-1),
                      foo->getArgument(foo->getNArgs()-1));
        Expand(t, true);
        t = prod_s(t,pow_s(variable,int_s(r->getNArgs()-foo->getNArgs())));
        Expand(t, true);
        q = sum_s(q,t);
        r = sum_s(r,prod_s(prod_s(CSL_M_1,t),foo));
        //r = r+(1*foo*t);
        Expand(r, true);
        Expand(q, true);
        ++iter;
    }
        //cout<<"q = "; q->print();
        //cout<<"r = "; r->print();
    if (*r == CSL_0)
        return q;

    return Copy(this);
}

optional<Expr> Polynomial::factor(bool) const
{
    if (argument.size()== 3) {
        // Canonical factorization of order 2 polynomial
        // x = -b/2a +- sqrt(delta)/2a
       Expr delta = sum_s(pow_s(argument[1],CSL_2),
                          prod_s(prod_s(int_s(-4),argument[0]),
                                 argument[2]));
       Expr mean = prod_s(CSL_M_1, argument[1]);
       Expr diff = sqrt_s(delta);
       Expr root1 = fraction_s(sum_s(mean, diff),
                              prod_s(CSL_2,argument[2]));
       Expr root2 = fraction_s(sum_s(mean, prod_s(CSL_M_1,diff)),
                              prod_s(CSL_2,argument[2]));

       Expr pol1 = polynomial_s(minus_(variable, root1), variable);
       Expr pol2 = polynomial_s(minus_(variable, root2), variable);

       return prod_s(argument[2],prod_s(pol1, pol2, true));
    }
    return nullopt;
}

bool Polynomial::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (argument.size() == 1)
        return *argument[0]==expr;
    if (expr->getType() == csl::Type::Polynomial) {
        Expr t_variable = expr->getVariable();
        if (*variable != t_variable)
            return false;

        csl::vector_expr t_argument = expr->getVectorArgument();
        if (t_argument.size() != argument.size())
            return false;
        for (size_t i=0; i<t_argument.size(); i++)
            if (*argument[i] != t_argument[i])
                return false;

        return true;
    }
    if (expr->getType() != csl::Type::Sum)
        return false;
    if (size() != expr->size())
        return false;
    vector<int> indicesLeft(argument.size());
    for (size_t i = 0; i < argument.size();i++)
        indicesLeft[i] = i;

    for (size_t i = 0; i < argument.size(); i++) {
        bool matched = 0;
        for (size_t j = 0; j < indicesLeft.size(); j++) {
            if (*prod_s(argument[i],pow_s(variable,int_s(i))) ==
                    expr->getArgument(indicesLeft[j]))
            {
                indicesLeft.erase(indicesLeft.begin()+j);
                matched = 1;
                break;
            }
        }
        if (!matched)
            return false;
    }

    return true;
}

Expr polynomial_s(const Expr& expr, const Expr& t_variable)
{
    if (expr->getPrimaryType() == csl::PrimaryType::Vectorial) {
        Expr foo = vectorialtensor_s(expr->getShape());
        for (int i=0; i<expr->getNArgs(); i++)
            foo->setArgument(polynomial_s(expr->getArgument(i),t_variable),i);
        return foo;
    }

    Expr res = csl::make_shared<Polynomial>(expr, t_variable);
    return res;
}

Expr polynomial_s(const csl::vector_expr& operands, const Expr& t_variable)
{
    Expr res =  csl::make_shared<Polynomial>(operands, t_variable);
    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Derivative                              //
/*************************************************/
///////////////////////////////////////////////////

Expr Derivative::getVariable() const {
    return argument[1];
}

Expr Derivative::getOperand() const {
    return argument[0];
}

int Derivative::getOrder() const {
    return order;
}

void Derivative::setOperand(const Expr& t_operand) {
    argument[0] = t_operand;
}

void Derivative::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out<<"d";
    if (order > 1)
        out<<"^"<<order;
    out<<"/d";
    out<<"(";
    argument[1]->print(1, out);
    out<<")";
    if (order > 1)
        out<<"^"<<order;
    out<<"(";
    if (isEmpty()) {
        if (*argument[0] != CSL_1)
            argument[0]->print(1, out);
        if (mode == 0)
            out<<endl;
        return;
    }
    argument[0]->print(1, out);
    out<<")";
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void Derivative::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::derivative_s(";
    for (size_t i = 0; i != size(); ++i) {
        argument[i]->printCode(1, out);
        out << ", ";
    }
    out << order;
    out << ")";
}

string Derivative::printLaTeX(int) const
{
    ostringstream sout;
    sout<<"\\frac{d";
    if (order > 1)
        sout<<"^"<<order;
    sout<<"}{d";
    sout<<argument[1]->printLaTeX(1);
    if (order > 1)
        sout<<"^"<<order;
    sout<<"}\\left(";
    if (isEmpty()) {
        if (*argument[0] != CSL_1)
            sout<<argument[0]->printLaTeX(1);
        return sout.str();
    }
    sout<<argument[0]->printLaTeX(1);
    sout<<"\\right)";

    return sout.str();
}

long double Derivative::evaluateScalar() const
{
    if (argument[0]->getDim() > 0 or argument[1]->getDim() > 0)
        return 0;
    Expr df = argument[0];
    for (int i=0; i<order; i++)
        df = Derived(df, argument[1].get());

    return df->evaluateScalar();
}

optional<Expr> Derivative::evaluate(
        csl::eval::mode user_mode
        ) const
{
    if (argument[0]->getType() == csl::Type::Prod) {
        csl::vector_expr t_argument(argument[0]->getNArgs());
        for (size_t i=0; i!=t_argument.size(); ++i) {
            t_argument[i] = Copy(argument[0]);
            t_argument[i]->setArgument(
                    derivative_s(argument[0]->getArgument(i), argument[1],
                        1), i);
        }
        if (order > 1)
            return derivative_s(
                    sum_s(t_argument),argument[1],order-1)->evaluate(user_mode);
        else 
            return sum_s(t_argument)->evaluate(user_mode);
    }
    if (argument[0]->dependsExplicitlyOn(argument[1].get())) {
        Expr df = Evaluated(argument[0]);
        for (int i=0; i<order; i++)
            df = Derived(df, argument[1].get());

        return df;
    }

    return derivative_s(Evaluated(argument[0]), 
                       Evaluated(argument[1]), order);
}

unique_Expr Derivative::copy_unique() const
{
    return make_unique<Derivative>(argument[0], 
                                   argument[1],
                                   order,
                                   empty);
}

Expr Derivative::deepCopy() const
{
    return csl::make_shared<Derivative>(argument[0]->deepCopy(), 
                                   argument[1]->deepCopy(),
                                   order,
                                   empty);
}

Expr Derivative::refresh() const
{
    return derivative_s(argument[0], 
                       argument[1],
                       order,
                       empty);
}

Expr Derivative::deepRefresh() const
{
    return derivative_s(argument[0]->deepRefresh(), 
                       argument[1]->deepRefresh(),
                       order,
                       empty);
}



optional<Expr> Derivative::derive(Expr_info expr) const
{
    return derivative_s(Derived(argument[0], expr), argument[1], order);
}

int Derivative::getParity(Expr_info t_variable) const
{
    int parity = argument[0]->getParity(t_variable);
    if (parity == -1)
        parity = -2*(order%2)+1;

    if (parity == 0)
        return 0;
    else
        return parity*argument[1]->getParity(t_variable);
}

bool Derivative::operatorAppliesOn(Expr_info expr) const
{
    return expr->dependsOn(argument[1].get());
}

bool Derivative::operator==(Expr_info expr) const
{
    if (int test = testDummy(expr); test != -1)
        return test;
    if (expr->getType() != csl::Type::Derivative)
        return false;
    if (*argument[0] != expr->getOperand() or
            *argument[1] != expr->getVariable() or
            order != expr->getOrder())
        return false;

    return true;
}

Expr derivative_s(const Expr& leftOperand, const Expr& rightOperand, int order)
{
    return derivative_s(leftOperand, rightOperand, order, false);
}

Expr derivative_s(const Expr& leftOperand, const Expr& rightOperand, int order,
     bool t_empty) 
{
    if (order <= 0)
        return rightOperand;
    //We calculate the derivative and if it is 0 (many cases) we return 0.
    if (leftOperand->getPrimaryType() == csl::PrimaryType::Vectorial) {
        Expr res = vectorialtensor_s(leftOperand->getShape());
        for (int i=0; i!=res->getNArgs(); ++i)
            res->setArgument(
                    derivative_s(leftOperand->getArgument(i),rightOperand,order,
                        t_empty)
                    ,i);

        return res;
    }
    if (rightOperand->getPrimaryType() == csl::PrimaryType::Vectorial) {
        Expr res = vectorialtensor_s(rightOperand->getShape());
        for (int i=0; i!=res->getNArgs(); ++i)
            res->setArgument(
                    derivative_s(leftOperand,rightOperand->getArgument(i),
                        order, t_empty)
                    ,i);

        return res;
    }

    if (not leftOperand->dependsOn(rightOperand.get())
            and *leftOperand != CSL_1)
        return CSL_0;

    csl::vector_expr foo;
    Expr res;
    switch(leftOperand->getType()) {

        case csl::Type::Derivative:
        if (rightOperand == leftOperand->getVariable()) 
            return derivative_s(leftOperand->getOperand(), 
                    rightOperand,
                    order + leftOperand->getOrder());
        else
            return csl::make_shared<Derivative>(leftOperand, rightOperand, 
                order, t_empty);
        break;

        default:
        res = csl::make_shared<Derivative>(rightOperand,order);
        return res->applyOperator(leftOperand, t_empty);
    }
    return CSL_UNDEF;
}

Expr derivative_s(const Expr& variable, int order)
{
    if (order <= 0)
        return CSL_0;
    
    return csl::make_shared<Derivative>(variable, order);
}


///////////////////////////////////////////////////
/*************************************************/
// Class Integral                              //
/*************************************************/
///////////////////////////////////////////////////

Expr Integral::getVariable() const {
    return argument[1];
}

Expr Integral::getOperand() const {
    return argument[0];
}

Expr Integral::getInfBoundary() const 
{
    return inf;
}

Expr Integral::getSupBoundary() const
{
    return sup;
}

void Integral::setOperand(const Expr& t_operand) {
    argument[0] = t_operand;
}

void Integral::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out<<"int_s{";
    if (argument[1]->getDim() > 0)
        out << argument[1]->getName() << "-" << argument[1]->getNArgs() << "d";
    else
        argument[1]->print(1, out);
    if (inf != -CSL_INF or sup != CSL_INF) {
        out << ", ";
        inf->print(1, out);
        out << ", ";
        sup->print(1, out);
    }
    out<<"}";
    out << "(";
    if (isEmpty()) {
        if (*argument[0] != CSL_1)
            argument[0]->print(1, out);
        if (mode == 0)
            out<<endl;
        return;
    }
    argument[0]->print(1, out);
    out<<")";
    printProp(out);
    if (mode == 0)
        out<<endl;
}

void Integral::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::integral_s(";
    for (size_t i = 0; i != size(); ++i) {
        argument[i]->printCode(1, out);
        out << ", ";
    }
    inf->printCode(1, out);
    out << ", ";
    sup->printCode(1, out);
    out << ")";
}

string Integral::printLaTeX(int) const
{
    ostringstream sout;
    sout << "\\int _{" << inf->printLaTeX(1);
    sout << "}^{" << sup->printLaTeX(1);
    sout << "}d";
    if (argument[1]->getDim() > 0)
        sout << "^" << argument[1]->getNArgs();
    sout << argument[1]->getName();
    sout << "\\left(";
    if (isEmpty()) {
        if (*argument[0] != CSL_1)
            sout<<argument[0]->printLaTeX(1);
        return sout.str();
    }
    sout<<argument[0]->printLaTeX(1);
    sout<<"\\right)";

    return sout.str();
}

Expr Integral::suppressTerm(Expr_info expr) const
{
    Expr res = Copy(this);
    res->setOperand(getOperand()->suppressTerm(expr));
    return res;
}

/*bool Integral::evaluateDelta(Expr& res, const Expr& dep) const
{
    if (inf != -CSL_INF or sup != CSL_INF)
        return false;
    if (dep->getType() != csl::Type::Prod) {
        if (dep->getType() != csl::Type::DiracDelta)
            return false;
        if (dep->dependsExplicitlyOn(getVariable())) {
            res = dep->applyDiracDelta(argument[0], argument[1]);
            return true;
        }
        return false;
    }
    for (int i = 0; i != dep->getNArgs(); ++i) {
        Expr arg = dep->getArgument(i);
        if (arg->getType() != csl::Type::DiracDelta)
            continue;
        if (arg->getArgument()->dependsExplicitlyOn(getVariable())) {
            //csl::vector_expr foo = dep->getVectorArgument();
            //foo.erase(foo.begin()+i);
            //res = arg->applyDiracDelta(prod_s(foo,true),dep);
            res = arg->applyDiracDelta(argument[0],argument[1]);
            return true;
        }
    }
    return false;
}

bool Integral::evaluateExpProduct(Expr& res, const Expr& dep) const
{
    Expr foo = CSL_0;
    if (dep->getType() == csl::Type::Prod) {
        for (const auto& arg : dep->getVectorArgument()) {
            if (arg->getType() != csl::Type::Exp
                    and (arg->getType() != csl::Type::Pow
                        or arg->getArgument() != E))
                return false;
            foo = foo + arg->getArgument();
        }
    }
    else if (dep->getType() == csl::Type::Exp
                or (dep->getType() == csl::Type::Pow
                    and dep->getArgument() != E))
        foo = dep->getArgument(dep->getType() == csl::Type::Exp);
    else
        return false;
    foo = foo->expand(true)->factor();
    cout << "\n\nfor integral of ";
    print();
    argument[1]->print();
    cout << "dependecies = ";
    dep->print();

    if (Comparator::dummyComparison(foo,
                I*argument[1]*Comparator::dummy(1))
            or Comparator::dummyComparison(foo,
                -I*argument[1]*Comparator::dummy(1))) {
        //res = 2*PI*csl::make_shared<DiracDelta>(
        //        foo->suppressTerm(argument[1])->suppressTerm(I));
        res = Copy(argument[0]);
        if (dep->getType() != csl::Type::Prod)
            res = res->suppressTerm(dep);
        else {
            for (const auto& arg : dep->getVectorArgument())
                res = res->suppressTerm(arg);
        }
        res = emptyIntegrals(res)*2*PI*csl::make_shared<DiracDelta>(
                foo->suppressTerm(argument[1])->suppressTerm(I));
        cout << "RES = ";
        res = DeepRefreshed(res);
        res->print();
        return true;
    }
    cout << "NO change!\n";
    return false;
}

Expr Integral::evaluateIntegral() const
{
    Expr dep = getRecursiveIntegralDependency(getOperand(),
                                              getVariable());
    //cout << "\n\n";
    //print();
    //cout << "\nDEP : ";
    //dep->print();
    if (dep == CSL_0)
        return Copy(this);
    Expr arg0 = argument[0];
    Expr res;
    if (evaluateDelta(res,dep) or evaluateExpProduct(res,dep))
        return res;
    return Copy(this);

    //Expr arg;
    //if (arg0->getType() == csl::Type::Prod) {
    //    if (arg0->getNArgs() > 1)
    //        return evaluateExpProduct();
    //    else
    //        arg = arg0->getArgument();
    //}
    //else
    //    arg = arg0;

    //if (not (arg->getType() == csl::Type::Exp)
    //        and not (arg->getType() == csl::Type::Pow 
    //            and arg->getArgument() == E))
    //    return Copy(this);
    //if (inf != -CSL_INF or sup !=CSL_INF)
    //    return Copy(this);

    //arg = arg->getArgument(arg->getType() == csl::Type::Pow);

    //if (Comparator::dummyComparison(arg,
    //            I*argument[1]*Comparator::dummy(1))
    //        or Comparator::dummyComparison(arg,
    //            -I*argument[1]*Comparator::dummy(1))) {
    //    return 2*PI*csl::make_shared<DiracDelta>(
    //            arg->suppressTerm(argument[1])->suppressTerm(I));
    //}
    //return Copy(this);
}*/

optional<Expr> Integral::evaluate(
        csl::eval::mode user_mode
        ) const
{
    optional<Expr> eval = getOperand()->evaluate(user_mode);
    if (not eval)
        return nullopt;
    Expr foo = Copy(this);
    foo->setOperand(eval.value());
    if (foo->getOperand() == CSL_0)
        return CSL_0;
    return Refreshed(foo);
   //  Expr arg0 = argument[0]->evaluate();

   //  Expr foo = csl::make_shared<Integral>(arg0, argument[1],empty,inf,sup);
   //  foo = DeepRefreshed(foo);
   //  if (foo->getType() == csl::Type::Integral)
   //      return Integral(foo->getArgument(0), foo->getArgument(1),
   //              foo->isEmpty(), foo->getInfBoundary(), foo->getSupBoundary()).
   //                  evaluateIntegral();
   //  return foo->evaluate();
}

unique_Expr Integral::copy_unique() const
{
    return make_unique<Integral>(argument[0], 
                                 argument[1],
                                 empty,
                                 inf,
                                 sup);
}

Expr Integral::deepCopy() const
{
    return csl::make_shared<Integral>(argument[0]->deepCopy(), 
                                 argument[1]->deepCopy(),
                                 empty,
                                 inf->deepCopy(),
                                 sup->deepCopy());
}

Expr Integral::refresh() const
{
    return integral_s(argument[0], 
                     argument[1],
                     empty,
                     inf,
                     sup);
}

Expr Integral::deepRefresh() const
{
    return integral_s(argument[0]->deepRefresh(), 
                     argument[1]->deepRefresh(),
                     empty,
                     inf->deepRefresh(),
                     sup->deepRefresh());
}



optional<Expr> Integral::derive(Expr_info expr) const
{
    CSL_ASSERT_SPEC(derivative_s(Copy(expr),getVariable()) == CSL_0,
              CSLError::MathError,
              "deriving the variable of an integral.");

    return csl::make_shared<Integral>(Derived(argument[0], expr),argument[1]);
}

int Integral::getParity(Expr_info t_variable) const
{
    int parity = argument[0]->getParity(t_variable);
    if (parity == 0)
        return 0;
    else
        return parity*argument[1]->getParity(t_variable);
}

bool Integral::operatorAppliesOn(Expr_info expr) const
{
    return expr->dependsOn(argument[1].get());
}

bool Integral::dependsOn(Expr_info expr) const 
{
    return AbstractDuoFunc::dependsOn(expr)
        or inf->dependsOn(expr) or sup->dependsOn(expr);
}

bool Integral::dependsExplicitlyOn(Expr_info expr) const 
{
    return AbstractDuoFunc::dependsExplicitlyOn(expr)
        or inf->dependsExplicitlyOn(expr) or sup->dependsExplicitlyOn(expr);
}

bool Integral::operator==(Expr_info expr) const
{
    if (int test = testDummy(expr); test != -1)
        return test;
    if (expr->getType() != csl::Type::Integral)
        return false;
    if (*argument[0] != expr->getOperand() or
            *argument[1] != expr->getVariable())
        return false;

    return true;
}

Expr integral_s(const Expr& leftOperand, const Expr& rightOperand)
{
    // if (rightOperand->getDim() > 0) {
    //     csl::vector_expr argument(rightOperand->getNArgs());
    //     for (int i = 0; i != rightOperand->getNArgs(); ++i) 
    //         argument[i] = integral_s(leftOperand, rightOperand->getArgument(i));
    //     return prod_s(argument, true);
    // }
    return integral_s(leftOperand, rightOperand, false);
}

Expr integral_s(const Expr& leftOperand, const Expr& rightOperand, bool t_empty) 
{
    // Vector integrals 
    if (leftOperand->getPrimaryType() == csl::PrimaryType::Vectorial) {
        Expr res = vectorialtensor_s(leftOperand->getShape());
        for (int i=0; i!=res->getNArgs(); ++i)
            res->setArgument(
                    integral_s(leftOperand->getArgument(i),rightOperand,
                        t_empty)
                    ,i);

        return res;
    }

    // if (rightOperand->getDim() > 0) {
    //     Expr res = leftOperand;
    //     for (int i = 0; i != rightOperand->getNArgs(); ++i) {
    //         Expr foo = csl::make_shared<Integral>(rightOperand->getArgument(i));
    //         res = foo->applyOperator(res,t_empty);
    //     }
    //     return res;
    // }
    if (not leftOperand->dependsOn(rightOperand.get())
            and *leftOperand != CSL_1)
        return rightOperand;

    Expr res = csl::make_shared<Integral>(rightOperand);
    return res->applyOperator(leftOperand, t_empty);
}

Expr integral_s(const Expr& leftOperand, const Expr& rightOperand, bool t_empty,
        const Expr& t_inf, const Expr& t_sup) 
{
    // Vector integrals 
    if (leftOperand->getPrimaryType() == csl::PrimaryType::Vectorial) {
        Expr res = vectorialtensor_s(leftOperand->getShape());
        for (int i=0; i!=res->getNArgs(); ++i)
            res->setArgument(
                    integral_s(leftOperand->getArgument(i),rightOperand,
                        t_empty, t_inf, t_sup)
                    ,i);

        return res;
    }

    if (not leftOperand->dependsOn(rightOperand.get())
            and *leftOperand != CSL_1)
        return rightOperand;

    Expr res = csl::make_shared<Integral>(CSL_1,rightOperand,true,t_inf,t_sup);
    return res->applyOperator(leftOperand, t_empty);
}

Expr integral_s(const Expr& variable)
{
    return csl::make_shared<Integral>(variable);
}

} // End of namespace csl
