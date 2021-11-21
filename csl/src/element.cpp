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

#include "element.h"
#include "utils.h"

using namespace std;


namespace csl {

AbstractElement::AbstractElement()
    :Complexified(),
    parent(nullptr)
{

}

AbstractElement::AbstractElement(const shared_ptr<AbstractParent>& t_parent)
    :Complexified(),
    parent(t_parent)
{

}

Parent AbstractElement::getParent() const
{
    return parent;
}

Parent_info AbstractElement::getParent_info() const
{
    return parent.get();
}

std::string const &AbstractElement::getName() const
{
    return parent->getName();
}

std::string const &AbstractElement::getLatexName() const
{
    return parent->getLatexName();
}

bool AbstractElement::getCommutable() const
{
    return parent->getCommutable();
}

csl::ComplexProperty AbstractElement::getComplexProperty() const
{
    return parent->getComplexProp();
}

std::vector<Parent> AbstractElement::getSubSymbols() const
{
    return {parent};
}

void AbstractElement::setParent(const Parent& t_parent)
{
    parent = t_parent;
}

void AbstractElement::setName(const string& name)
{
    parent->setName(name);
}

void AbstractElement::setCommutable(bool commutable)
{
    parent->setCommutable(commutable);
}

void AbstractElement::setComplexProperty(csl::ComplexProperty prop)
{
    parent->setComplexProperty(prop);
}

int AbstractElement::isPolynomial(Expr_info variable) const
{
    if(variable->getType() == getType()
            and parent.get() == variable->getParent().get())
        return 1;
    return 0;
}

optional<Expr> AbstractElement::getPolynomialTerm(
        Expr_info variable, int order) const
{
    if (isPolynomial(variable) == 1 and order == 1)
        return CSL_1;
    else if (order == 0)
        return nullopt;
    return CSL_0;
}

optional<Expr> AbstractElement::evaluate(
        csl::eval::mode user_mode
        ) const
{
    return parent->evaluate(this, user_mode);
}

} // End of namespace csl
