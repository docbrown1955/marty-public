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

#include "field.h"
#include "space.h"
#include "utils.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class FieldParent                             //
/*************************************************/
///////////////////////////////////////////////////

FieldParent::FieldParent() : AbstractParent(), spaceField(nullptr)
{
}

FieldParent::FieldParent(const string &t_name)
    : AbstractParent(t_name), spaceField(nullptr)
{
}

FieldParent::FieldParent(const string &t_name, const Space *t_spaceField)
    : AbstractParent(t_name), spaceField(t_spaceField)
{
}

void FieldParent::checkFieldRequest(TensorParent *t_point)
{
    if (t_point->getSpace() != vector<const Space *>(1, spaceField))
        callError(cslError::BadFieldInitialization,
                  "FieldParent::operator()(const TensorParent& vector)");
}

cslParent::PrimaryType FieldParent::getPrimaryType() const
{
    return cslParent::Field;
}

const Space *FieldParent::getFieldSpace() const
{
    return spaceField;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ScalarFieldParent                       //
/*************************************************/
///////////////////////////////////////////////////

ScalarFieldParent::ScalarFieldParent(const string &t_name)
    : FieldParent(t_name)
{
}

ScalarFieldParent::ScalarFieldParent(const string &t_name,
                                     const Space * t_spaceField)
    : FieldParent(t_name, t_spaceField)
{
}

cslParent::Type ScalarFieldParent::getType() const
{
    return cslParent::ScalarField;
}

Expr ScalarFieldParent::operator()(const Tensor &t_point)
{
    checkFieldRequest(t_point.get());
    return csl::make_shared<ScalarField>(t_point, self());
}

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractField                           //
/*************************************************/
///////////////////////////////////////////////////

AbstractField::AbstractField(const Tensor &t_vector, const Parent &t_parent)
    : AbstractElement(t_parent), point(t_vector)
{
}

csl::PrimaryType AbstractField::getPrimaryType() const
{
    return csl::PrimaryType::Field;
}

Tensor AbstractField::getPoint() const
{
    return point;
}

bool AbstractField::dependsOn(Expr_info expr) const
{
    return point->dependsOn(expr);
}

bool AbstractField::dependsOn(Parent_info t_parent) const
{
    return parent.get() == t_parent or point.get() == t_parent;
}

bool AbstractField::dependsExplicitlyOn(Expr_info expr) const
{
    return point->dependsExplicitlyOn(expr);
}

void AbstractField::setPoint(const Tensor &t_point)
{
    if (t_point->getSpace() != vector<const Space *>(1, getSpace()))
        callError(cslError::BadFieldInitialization,
                  "AbstractField::setPoint(const Tensor&)");
    point = t_point;
}

const Space *AbstractField::getSpace() const
{
    return parent->getFieldSpace();
}

///////////////////////////////////////////////////
/*************************************************/
// Class ScalarField                             //
/*************************************************/
///////////////////////////////////////////////////

ScalarField::ScalarField(const Tensor &t_point, const Parent &t_parent)
    : AbstractField(t_point, t_parent)
{
}

ScalarField::ScalarField(const Expr &otherField)
    : ScalarField(otherField.get())
{
}

ScalarField::ScalarField(const Abstract *otherField)
    : AbstractField(nullptr, nullptr)
{
    if (otherField->getType() != csl::Type::ScalarField)
        callError(cslError::UndefinedBehaviour,
                  "ScalarField::ScalarField(const Expr&)",
                  "Copying a non scalarField into a scalarField!");
    setParent(otherField->getParent());
    setPoint(otherField->getPoint());
}

csl::Type ScalarField::getType() const
{
    return csl::Type::ScalarField;
}

void ScalarField::print(int mode, std::ostream &, bool) const
{
    cout << getName();
    cout << "(" << point->getName() << ")";
    if (mode == 0)
        cout << endl;
}

string ScalarField::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << getName();
    sout << "(" << point->getName() << ")";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

optional<Expr> ScalarField::evaluate(csl::eval::mode) const
{
    if (auto parentEval = parent->evaluate(this); parentEval)
        return parentEval;

    return nullopt;
}

unique_Expr ScalarField::copy_unique() const
{
    unique_Expr res = make_unique<ScalarField>(point, parent);
    applyComplexPropertiesOn(res);

    return res;
}

bool ScalarField::commutesWith(Expr_info, int sign) const
{
    // To implement
    return (sign == -1);
}

bool ScalarField::operator==(Expr_info expr) const
{
    if (int test = testDummy(expr); test != -1)
        return test;
    if (expr->getType() != csl::Type::ScalarField
        or getName() != expr->getName() or point != expr->getPoint())
        return false;
    if (not Complexified::operator==(expr))
        return false;

    return true;
}
} // End of namespace csl
