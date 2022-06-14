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

#include "property.h"
#include "indicial.h"
#include "comparison.h"
#include "replace.h"

using namespace std;

namespace csl {

PropertyList::PropertyList(): props(vector<Equation*>(0)){}

PropertyList::~PropertyList()
{
    props.clear();
}

void PropertyList::addProperty(const Expr& leftHandSide)
{
    addProperty(leftHandSide, CSL_0);
}

void PropertyList::addProperty(const Expr& leftHandSide, const Expr& rightHandSide)
{
    Equation* property = equation_(leftHandSide, rightHandSide);
    csl::vector_expr BB = property->getBuildingBlocks();
    const size_t size = props.size();
    for (const auto& bb : BB) {
        Equation* newProp = equation_(property);
        newProp->isolate(bb);
        if (*newProp != *property) {
            newProp->setBuildingBlocks(csl::vector_expr(1,bb));
            props.push_back(newProp);
            bb->addProperty(newProp);
        }
        else
            delete newProp;
    }
    if (size == props.size()) {
        props.push_back(property);
        for (const auto& bb : property->getBuildingBlocks()) {
            bb->addProperty(property);
        }
    }
    else
        delete property;
}

void PropertyList::removeProperty(Equation* prop)
{
    for (auto iter=props.begin(); iter!=props.end(); ++iter)
        if (*iter == prop) {
            props.erase(iter);
            return;
        }
}

void PropertyList::removeProperty(const Expr& leftHandSide)
{
    removeProperty(leftHandSide, CSL_0);
}

void PropertyList::removeProperty(const Expr& leftHandSide,
                                  const Expr& rightHandSide)
{ 
    Equation* property = equation_(leftHandSide, rightHandSide);
    csl::vector_expr BB = property->getBuildingBlocks();
    for (const auto& bb : BB) {
        Equation* newProp = equation_(property);
        newProp->isolate(bb);
        for (auto iter=props.begin(); iter!=props.end(); ++iter)
            if (**iter == *newProp) {
                delete *iter;
                props.erase(iter);
            }
        delete newProp;
    }
}

csl::vector_expr PropertyList::apply(const Expr& expr) const
{
    const csl::vector_expr& buildingBlocks = listBuildingBlocks(expr);
    if (buildingBlocks.size() == 0)
        return csl::vector_expr(0);

    csl::vector_expr res(0);
    bool indexed = expr->isIndexed();
    IndexStructure structure = expr->getFreeIndexStructure();
    for (const auto& bb : buildingBlocks) {
        vector<Equation*> propsToApply = bb->getProperties();
        for (const auto& p : propsToApply) {
            if (indexed) 
                Comparator::freeIndexComparisonActive = true;
            optional<Expr> foo = expr->findSubExpression(p->getLHS().get(),
                                                         p->getRHS());
            if (not foo)
                return csl::vector_expr(1,CSL_0);
            if (indexed) {
                IndexStructure structure2 
                    = foo.value()->getFreeIndexStructure();
                for (size_t i=0; i!=structure.size(); ++i)
                    Replace(foo.value(), structure2[i],structure[i]);
            }
            if (foo.value() != expr)
                res.push_back(foo.value());
            if (indexed) 
                Comparator::clear();
        }
    }

    return res;
}

ostream& operator<<(ostream& fout, const PropertyList& props)
{
    for (const auto& p : props.props)
        cout<<*p<<endl;

    return fout;
}
} // End of namespace csl
