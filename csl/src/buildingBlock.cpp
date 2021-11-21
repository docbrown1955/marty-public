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

#include "buildingBlock.h"
#include "cslcomplex.h"
#include "literal.h"
#include "utils.h"
#include "error.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractBuildingBlock                   //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> AbstractBuildingBlock::expand(bool, bool) const
{
    return nullopt; 
}
optional<Expr> AbstractBuildingBlock::factor(bool) const
{
    return nullopt; 
}
optional<Expr> AbstractBuildingBlock::factor(Expr_info, bool) const
{
    return nullopt; 
}
optional<Expr> AbstractBuildingBlock::getTerm() const
{
    return nullopt; 
}

optional<Expr> AbstractBuildingBlock::getRealPart() const
{
    return nullopt; 
}
optional<Expr> AbstractBuildingBlock::getComplexModulus() const
{
    return nullopt; 
}
optional<Expr> AbstractBuildingBlock::getPolynomialTerm(Expr_info expr, int order) const
{
    if (order == 1 and operator==(expr))
        return CSL_1;
    else if (order == 0)
        return nullopt;

    return CSL_0;
}
optional<Expr> AbstractBuildingBlock::findSubExpression(
        Expr_info   subExpression,
        const Expr& newExpression) const
{
    if (operator==(subExpression))
        return newExpression;
    return nullopt;
}

bool AbstractBuildingBlock::commutesWith(Expr_info, int sign) const
{
    return (sign == -1);
}

///////////////////////////////////////////////////
/*************************************************/
// Class Complexified                            //
/*************************************************/
///////////////////////////////////////////////////

Complexified::Complexified()
    :AbstractBuildingBlock(){

}

bool Complexified::isReal() const
{
    return (getComplexProperty() == csl::ComplexProperty::Real);
}

bool Complexified::isPurelyImaginary() const
{
    return (getComplexProperty() == csl::ComplexProperty::Imaginary);
}

bool Complexified::isComplexConjugate() const
{
    return conjugated;
}

optional<Expr> Complexified::getRealPart() const
{
    if (isReal())
        return std::nullopt;
    if (isPurelyImaginary())
        return CSL_0;
    return real_s(Copy(this));
}

Expr Complexified::getImaginaryPart() const
{
    if (isReal())
        return CSL_0;
    if (isPurelyImaginary())
        return -CSL_I*copy();
    return imaginary_s(Copy(this));
}

optional<Expr> Complexified::getComplexConjugate() const
{
    auto complexProp = getComplexProperty();
    if (complexProp == csl::ComplexProperty::Real)
        return copy();
    if (complexProp == csl::ComplexProperty::Imaginary)
        return -copy();

    Expr res = copy();
    res->setConjugated(not isComplexConjugate());
    return res;
}

void Complexified::printProp(std::ostream& fout) const
{
    if (isComplexConjugate())
        fout << "^(*)";
}

csl::ComplexProperty Complexified::getComplexProperty() const
{
    return csl::ComplexProperty::Real;
}

void Complexified::setConjugated(bool t_conjugated)
{
    if (getComplexProperty() != ComplexProperty::Real)
        conjugated = t_conjugated;
}

void Complexified::setComplexProperty(csl::ComplexProperty)
{

}

bool Complexified::operator==(Expr_info other) const
{
    auto complexProp = getComplexProperty();
    return (complexProp == other->getComplexProperty()
        and isComplexConjugate() == other->isComplexConjugate());
}

void Complexified::applyComplexPropertiesOn(const Expr& expr) const
{
    auto complexProp = getComplexProperty();
    expr->setComplexProperty(complexProp);
    expr->setConjugated(conjugated);
}

void Complexified::applyComplexPropertiesOn(unique_Expr& expr) const
{
    auto complexProp = getComplexProperty();
    expr->setComplexProperty(complexProp);
    expr->setConjugated(conjugated);
}

}
