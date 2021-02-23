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

#include "equation.h"
#include "interface.h"
#include "utils.h"
#include "literal.h"
#include "numerical.h"
#include "mathFunctions.h"
#include "operations.h"
#include "vector.h"
#include "indicial.h"
#include "simplification.h"
#include "property.h"

using namespace std;

namespace csl {

csl::vector_expr listBuildingBlocks(const Expr& expr)
{
    if (expr->getType() == csl::Type::Variable
            or expr->getType() == csl::Type::Constant
            or expr->getType() == csl::Type::TensorElement)
        return csl::vector_expr(1,expr);
    csl::vector_expr list(0);
    int nArgs = expr->getNArgs();
    if (expr->getPrimaryType() == csl::PrimaryType::ScalarFunction
            or expr->getType() == csl::Type::Derivative)
        list = listBuildingBlocks(expr->getArgument());
    else
        for (int i=0; i!=nArgs; ++i) {
            csl::vector_expr list2 = listBuildingBlocks(expr->getArgument(i));
            if (list2.size() != 0)
                list.insert(list.end(), list2.begin(), list2.end());
        }

    if (not list.empty())
        for (auto bb1=list.begin(); bb1!=list.end()-1; ++bb1) {
            for (auto bb2=bb1+1; bb2!=list.end(); ++bb2)
                if (**bb1 == *bb2) {
                    list.erase(bb2);
                    break;
                }
            if (bb1 == list.end()-1)
                break;
        }

    return list;
}

Equation::Equation()
    :leftHandSide(CSL_0), rightHandSide(CSL_0)
{
    searchBuildingBlocks();
}

Equation::Equation(const Expr& LHS)
    :leftHandSide(LHS), rightHandSide(CSL_0)
{
    searchBuildingBlocks();
}

Equation::Equation(const Expr& LHS, const Expr& RHS)
    :leftHandSide(LHS), rightHandSide(RHS)
{
    searchBuildingBlocks();
}

Equation::Equation(const Equation& eq)
{
    leftHandSide = eq.leftHandSide;
    rightHandSide = eq.rightHandSide;
    buildingBlocks = eq.buildingBlocks;
}

Equation::~Equation()
{
    for (auto& bb : buildingBlocks)
        bb->removeProperty(this);
    PROPERTIES.removeProperty(this);
}

cslEquation::Type Equation::getType() const
{
    return cslEquation::Regular;
}

Expr Equation::getLHS() const
{
    return leftHandSide;
}

Expr Equation::getRHS() const
{
    return rightHandSide;
}

const csl::vector_expr& Equation::getBuildingBlocks() const
{
    return buildingBlocks;
}

void Equation::setBuildingBlocks(const csl::vector_expr& t_buildingBlocks)
{
    buildingBlocks = t_buildingBlocks;
}

void Equation::searchBuildingBlocks()
{
    buildingBlocks = listBuildingBlocks(leftHandSide);
    csl::vector_expr list = listBuildingBlocks(rightHandSide);
    buildingBlocks.insert(buildingBlocks.end(), list.begin(), list.end());
}

void Equation::replace(const Expr& oldE, const Expr& newE)
{
    leftHandSide = Replaced(leftHandSide, oldE, newE);
    rightHandSide = Replaced(rightHandSide, oldE, newE);
    if (oldE->isBuildingBlock())
        for (auto arg=buildingBlocks.begin(); arg!=buildingBlocks.end(); ++arg)
            if (**arg == oldE) {
                buildingBlocks.erase(arg);
                break;
            }

    if (newE->isBuildingBlock()
            and find(buildingBlocks.begin(), buildingBlocks.end(), newE) 
                == buildingBlocks.end())
        buildingBlocks.push_back(newE);
}

void Equation::simplify()
{
    leftHandSide = Simplify(leftHandSide);
    rightHandSide = Simplify(rightHandSide);
}

void Equation::makeLHSimple()
{
    if (buildingBlocks.size() == 0)
        return;
    Expr bb = buildingBlocks[0];
    if (not leftHandSide->dependsExplicitlyOn(bb.get())) 
        swap(leftHandSide, rightHandSide);
    if (leftHandSide->getType() == csl::Type::Sum) {
        bool dependencyFound = false;
        Expr sum = CSL_0;
        for (auto arg=leftHandSide->begin(); arg!=leftHandSide->end(); ++arg) {
            if (not dependencyFound and (*arg)->dependsExplicitlyOn(bb.get()))
                dependencyFound = true;
            else {
                sum = sum - (*arg);
            }
        }
        leftHandSide = leftHandSide + sum;
        rightHandSide = rightHandSide + sum;
    }
    if (leftHandSide->getType() == csl::Type::Prod) {
        bool dependencyFound = false;
        Expr product = CSL_1;
        for (auto arg=leftHandSide->begin(); arg!=leftHandSide->end(); ++arg) {
            if (not dependencyFound and (*arg)->dependsExplicitlyOn(bb.get()))
                dependencyFound = true;
            else if (not (*arg)->isIndexed()){
                product = product * (*arg);
            }
        }
        leftHandSide = Expanded(leftHandSide/product);
        rightHandSide = Expanded(rightHandSide/product);
    }
    if (leftHandSide->getType() == csl::Type::Sum
            or leftHandSide->getType() == csl::Type::Prod)
        makeLHSimple();
}

void Equation::isolate(const Expr& expr)
{
    isolationStep(expr);
}

void Equation::isolationStep(const Expr& expr)
{
    if (not expr->isBuildingBlock())
        callError(cslError::UndefinedBehaviour,
                "Equation::isolationStep(const Expr&)");
    Expr sum = CSL_0;
    // Suprressing all terms on the left that do not depend on expr
    if (leftHandSide->getType() == csl::Type::Sum) {
        for (auto arg=leftHandSide->begin();
                arg!=leftHandSide->end(); ++arg) {
            if (IsIndicialTensor(*arg)) {
                if (**arg == expr)
                    sum = sum_s(sum, *arg);
            }
            else if (not (*arg)->dependsExplicitlyOn(expr.get()))
                sum = sum_s(sum, *arg);
        }
    }
    // Suprressing all terms on the right that do depend on expr
    if (rightHandSide->getType() == csl::Type::Sum) {
        for (auto arg=rightHandSide->begin();
                arg!=rightHandSide->end(); ++arg) {
            if (IsIndicialTensor(*arg)) {
                if (**arg == expr)
                    sum = sum_s(sum, *arg);
            }
            if ((*arg)->dependsExplicitlyOn(expr.get()))
                sum = sum_s(sum, *arg);
        }
    }
    if (*sum != CSL_0) {
        leftHandSide = leftHandSide - sum;
        rightHandSide = rightHandSide - sum;
        Expand(leftHandSide, true);
        Expand(rightHandSide, true);
    }
    // Searching expr in denominators
    Expr product = CSL_1;
    if (leftHandSide->getType() == csl::Type::Sum) {

        for (auto arg=leftHandSide->begin(); arg!=leftHandSide->end(); ++arg) {
            if ((*arg)->getType() == csl::Type::Pow
                    and (*arg)->getArgument(1)->isInteger()
                    and (*arg)->getArgument(1)->evaluateScalar() < 0) {
                Expr foo = pow_s((*arg)->getArgument(0),
                        -1*(*arg)->getArgument(1));
                for (auto argModif=leftHandSide->begin();
                        argModif!=leftHandSide->end(); ++argModif)
                    (*argModif) = (*argModif)*foo;
                product = product*foo;
            }
            else if ((*arg)->getType() == csl::Type::Prod) {
                for (auto arg2=(*arg)->begin();
                        arg2!=(*arg)->end(); ++arg2) {
                    if ((*arg2)->getType() == csl::Type::Pow
                            and (*arg2)->getArgument(1)->isInteger()
                            and (*arg2)->getArgument(1)->evaluateScalar() < 0) {
                        Expr foo = pow_s((*arg2)->getArgument(0),
                                -1*(*arg2)->getArgument(1));
                        for (auto arg2Modif=leftHandSide->begin();
                                arg2Modif!=leftHandSide->end(); ++arg2Modif)
                            (*arg2Modif) = (*arg2Modif)*foo;
                        product = product*foo;
                        if ((*arg)->getType() == csl::Type::Prod)
                            --arg;
                        break;
                    }
                }
            }
        }
    }
    else if (leftHandSide->getType() == csl::Type::Prod) {
        for (auto arg=(leftHandSide)->begin();
                arg!=(leftHandSide)->end(); ++arg) {
            if ((*arg)->getType() == csl::Type::Pow
                    and (*arg)->getArgument(1)->isInteger()
                    and (*arg)->getArgument(1)->evaluateScalar() < 0) {
                Expr foo = pow_s((*arg)->getArgument(0),
                        -1*(*arg)->getArgument(1));
                for (auto argModif=leftHandSide->begin();
                        argModif!=leftHandSide->end(); ++argModif)
                    (*argModif) = (*argModif)*foo;
                product = product*foo;
            }
        }
    }
    if (*product != CSL_1) {
        leftHandSide = DeepRefreshed(leftHandSide);
        rightHandSide = Expanded(rightHandSide*product);
        isolationStep(expr);
        return;
    }

    if (leftHandSide->getPrimaryType() == csl::PrimaryType::ScalarFunction) {
        cout<<*this<<endl;
        switch(leftHandSide->getType()) {
            case csl::Type::Exp:   rightHandSide = log_s(rightHandSide);   break;
            case csl::Type::Log:   rightHandSide = exp_s(rightHandSide);   break;
            case csl::Type::Cos:   rightHandSide = acos_s(rightHandSide);  break;
            case csl::Type::Sin:   rightHandSide = asin_s(rightHandSide);  break;
            case csl::Type::Tan:   rightHandSide = atan_s(rightHandSide);  break;
            case csl::Type::ACos:  rightHandSide = cos_s(rightHandSide);   break;
            case csl::Type::ASin:  rightHandSide = sin_s(rightHandSide);   break;
            case csl::Type::ATan:  rightHandSide = tan_s(rightHandSide);   break;
            case csl::Type::Cosh:  rightHandSide = acosh_s(rightHandSide); break;
            case csl::Type::Sinh:  rightHandSide = asinh_s(rightHandSide); break;
            case csl::Type::Tanh:  rightHandSide = atanh_s(rightHandSide); break;
            case csl::Type::ACosh: rightHandSide = cosh_s(rightHandSide);  break;
            case csl::Type::ASinh: rightHandSide = sinh_s(rightHandSide);  break;
            case csl::Type::ATanh: rightHandSide = tanh_s(rightHandSide);  break;
            default: return;
        }
        leftHandSide = leftHandSide->getArgument();
        isolationStep(expr);
        return;
    }
    if (leftHandSide->getPrimaryType() == csl::PrimaryType::MultiFunction) {
        Expr foo;
        switch(leftHandSide->getType()) {
            
            case csl::Type::Pow:
            if (not leftHandSide->getArgument(1)->dependsExplicitlyOn(expr.get())) {
                rightHandSide = pow_s(rightHandSide,1/leftHandSide->getArgument(1));
                leftHandSide = leftHandSide->getArgument(0);
            }
            else
                return;
            break;

            case csl::Type::Sum:
            foo = Factored(leftHandSide, expr.get());
            if (*foo != leftHandSide) 
                leftHandSide = foo;
            else
                return;
            break;

            case csl::Type::Prod:
            foo = CSL_1;
            for (auto arg=leftHandSide->begin(); arg!=leftHandSide->end();
                    ++arg)
                if (not (*arg)->dependsExplicitlyOn(expr.get()) 
                        and not (*arg)->isIndexed())
                    foo = foo*(*arg);
            if (*foo != CSL_1) {
                leftHandSide = leftHandSide/foo;
                rightHandSide = Expanded(rightHandSide/foo);
            }
            else
                return;
            break;

            default: return;
        }
        isolationStep(expr);
        return;
    }
}

bool Equation::operator==(const Equation& eq) const
{
    return (leftHandSide->operator==(eq.getLHS())
            and rightHandSide->operator==(eq.getRHS()));
}

bool Equation::operator!=(const Equation& eq) const
{
    return not operator==(eq);
}

ostream& operator<<(ostream& fout, const Equation& eq)
{
    eq.leftHandSide->print(1);
    fout<<" = ";
    eq.rightHandSide->print(1);

    return fout;
}

ostream& operator<<(ostream& fout, cslEquation::Type type)
{
    switch(type) {

        case cslEquation::Regular:    fout<<"Regular";  break;
        case cslEquation::Indicial:   fout<<"Indicial"; break;

        default: callError(cslError::UnknownEquation,
                "operator<<(ostream&, cslEquation::Type)", (int)type);
    }

    return fout;
}

IndicialEquation::IndicialEquation(): Equation(){}
IndicialEquation::IndicialEquation(const Expr&){}
IndicialEquation::IndicialEquation(const Expr& leftHandSide,
         const Expr& rightHandSide)
    :Equation(leftHandSide, rightHandSide){}
IndicialEquation::IndicialEquation(const Equation& eq): Equation(eq){}
IndicialEquation::IndicialEquation(const IndicialEquation& eq): Equation(eq){}

Equation* equation_(const Expr& leftHandSide)
{
    Equation* eq;
    if (leftHandSide->isIndexed())
        eq = new IndicialEquation(leftHandSide);
    else
        eq = new Equation(leftHandSide);

    return eq;
}

Equation* equation_(const Expr& leftHandSide, const Expr& rightHandSide)
{
    Equation* eq;
    if (leftHandSide->isIndexed() or rightHandSide->isIndexed())
        eq = new IndicialEquation(leftHandSide, rightHandSide);
    else
        eq = new Equation(leftHandSide, rightHandSide);

    return eq;
}

Equation* equation_(const Equation& eq)
{
    if (eq.getType() == cslEquation::Regular)
        return new Equation(eq);
    else
        return new IndicialEquation(eq);
}

Equation* equation_(Equation* eq)
{
    if (eq->getType() == cslEquation::Regular)
        return new Equation(*eq);
    else
        return new IndicialEquation(*eq);
}
} // End of namespace csl
