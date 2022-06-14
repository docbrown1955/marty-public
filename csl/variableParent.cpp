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

#include "variableParent.h"
#include "literal.h"
#include "numerical.h"
#include "numericalEval.h"
#include "utils.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class LiteralParent                           //
/*************************************************/
///////////////////////////////////////////////////

LiteralParent::LiteralParent(string const& t_name)
    :AbstractParent(t_name),
    valued(false)
{

}

LiteralParent::LiteralParent(string const& t_name,
                             long double   t_value)
    :AbstractParent(t_name),
    valued(true),
    value(autonumber_s(t_value))
{

}

LiteralParent::LiteralParent(string const& t_name,
                             Expr   const& t_value)
    :AbstractParent(t_name),
    valued(true),
    value(t_value)
{
    CSL_ASSERT_SPEC(value->getPrimaryType() == csl::PrimaryType::Numerical,
            CSLError::TypeError,
            "Expected numerical in Literal constructor, " 
            + toString(t_value->getType()) + " given.");
}

bool LiteralParent::isValued() const
{
    return valued;
}

Expr LiteralParent::getValue() const
{
    if (valued)
        return value;
    return CSL_UNDEF;
}

void LiteralParent::setValue(long double t_value)
{
    valued = true;
    value = autonumber_s(t_value);
}

void LiteralParent::setValue(Expr const &t_value)
{
    if (t_value == CSL_UNDEF) {
        valued = false;
        value = CSL_UNDEF;
        return;
    }
    valued = true;
    CSL_ASSERT_SPEC(t_value->getPrimaryType() == csl::PrimaryType::Numerical
                or t_value == CSL_INF,
            CSLError::TypeError,
            "Expecting a numerical type, " + toString(t_value) + " given.");
    value = t_value;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ConstantParent                          //
/*************************************************/
///////////////////////////////////////////////////

ConstantParent::ConstantParent(string const& t_name)
    :LiteralParent(t_name)
{

}

ConstantParent::ConstantParent(string const& t_name,
                               long double   t_value)
    :LiteralParent(t_name, t_value)
{

}

ConstantParent::ConstantParent(string const& t_name,
                               Expr   const& t_value)
    :LiteralParent(t_name, t_value)
{

}

void ConstantParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool      header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Expr " << regName
        << " = csl::constant_s(\"" << regLite << "\"";
    if (valued) {
        out << ", ";
        value->printCode(1, out);
    }
    if (getComplexProp() != csl::ComplexProperty::Real) {
        out << ", ";
        if (getComplexProp() == csl::ComplexProperty::Imaginary)
            out << "csl::ComplexProperty::Imaginary";
        else
            out << "csl::ComplexProperty::Complex";
    }
    out << ");\n";
    printPropDefinition(out, indentSize, header);
}

Expr ConstantParent::generateInstance()
{
    return csl::make_shared<Constant, alloc_constant>(
            dynamic_pointer_cast<ConstantParent>(self()));
}

bool ConstantParent::isInstance(Expr_info expr) const
{
    if (expr->getType() != csl::Type::Constant)
        return false;
    return expr->getParent().get() == this;
}


///////////////////////////////////////////////////
/*************************************************/
// Class VariableParent                          //
/*************************************************/
///////////////////////////////////////////////////

VariableParent::VariableParent(string const& t_name)
    :LiteralParent(t_name),
    elementary(false),
    allDependencies(true)
{

}

VariableParent::VariableParent(string const& t_name,
                               long double   t_value)
    :LiteralParent(t_name, t_value),
    elementary(false),
    allDependencies(true)
{

}

VariableParent::VariableParent(string const& t_name,
                               Expr   const& t_value)
    :LiteralParent(t_name, t_value),
    elementary(false),
    allDependencies(true)
{

}

void VariableParent::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    std::string regLite = csl::Abstract::regularLiteral(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::Expr " << regName
        << " = csl::variable_s(\"" << regLite << '\"';
    if (valued) {
        out << ", ";
        value->printCode(1, out);
    }
    if (getComplexProp() != csl::ComplexProperty::Real) {
        out << ", ";
        if (getComplexProp() == csl::ComplexProperty::Imaginary)
            out << "csl::ComplexProperty::Imaginary";
        else
            out << "csl::ComplexProperty::Complex";
    }
    out << ");\n";
    if (!header) {
        out << indent << regName 
            << "->setElementary(" << elementary << ");\n";
        out << indent << regName 
            << "->setAllDependencies(" << allDependencies << ");\n";
    }
    // printPropDefinition(out, indentSize, header);
}

bool VariableParent::isElementary() const
{
    return elementary;
}

bool VariableParent::isAllDependencies() const
{
    return allDependencies;
}

void VariableParent::setElementary(bool t_elementary)
{
    elementary = t_elementary;
    if (elementary) 
        allDependencies = false;
    else
        allDependencies = true;
    dependencies.clear();
}

void VariableParent::setAllDependencies(bool t_allDependencies)
{
    dependencies.clear();
    if (allDependencies == t_allDependencies)
        return;
    allDependencies = t_allDependencies;
    //We change of dependance mode
    dependencies.clear();
    if (allDependencies and elementary)
        elementary = false;
}

void VariableParent::addDependency(Expr_info expr)
{
    if (elementary) {
        elementary = false;
        allDependencies = false;
        dependencies.clear();
    }
    auto pos = find_if(dependencies.begin(), dependencies.end(),
            [&expr] (const unique_Expr_c& el) {
                return *expr == *el.get();
            });
    if (pos != dependencies.end()) {
        if (allDependencies)
            dependencies.erase(pos);
    }
    else if (not allDependencies) {
        dependencies.push_back(expr->copy_unique()); 
    }
}

void VariableParent::removeDependency(Expr_info expr)
{
    auto pos = find_if(dependencies.begin(), dependencies.end(),
            [&expr] (const unique_Expr_c& el) {
                return *expr == el.get();
            });
    if (pos != dependencies.end()) {
        if (not allDependencies)
            dependencies.erase(pos);
    }
    else if (allDependencies) {
        dependencies.push_back(expr->copy_unique());
    }
}

bool VariableParent::dependsOn(Expr_info expr) const
{
    // Elementary variable: depends only on itself
    if (isInstance(expr))
        return true;
    if (elementary) 
        return false;

    // expr is a number: 0
    if (expr->getPrimaryType() == csl::PrimaryType::Numerical
            or expr->getType() == csl::Type::Constant)
        return false;

    auto pos = find_if(dependencies.begin(), dependencies.end(),
            [&expr] (const unique_Expr_c& el) {
                return *expr == el.get();
            });
    // All dependencies: the Variable depends on everything by default
    // except expressions in the vector dependencies
    if (allDependencies)
        return (pos == dependencies.end());
    // else the expression depends on nothing except expressions in 
    // the vector dependencies
    else 
        return (not (pos == dependencies.end()));
}

bool VariableParent::commutesWith(Expr_info expr, 
                                  int)
{
    if (commutable) {
        if (expr->getCommutable())
            return true;
        else {
            if (expr->isBuildingBlock())
                return true;
            return expr->commutesWith(generateInstance().get());
        }
    }
    else {
        if (expr->getCommutable())
            return true;
        else {
            if (expr->isBuildingBlock())
                return false;
            return expr->commutesWith(generateInstance().get());
        }
    }
}

Expr VariableParent::generateInstance()
{
    return csl::make_shared<Variable, alloc_variable>(
            dynamic_pointer_cast<VariableParent>(self())
            );
}

bool VariableParent::isInstance(Expr_info expr) const
{
    if (expr->getType() != csl::Type::Variable)
        return false;
    return expr->getParent().get() == this;
}

} // End of namespace csl
