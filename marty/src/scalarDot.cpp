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

#include "scalarDot.h"

namespace mty {

ScalarDot::ScalarDot(csl::Expr const& A,
                     csl::Expr const& B)
    :AbstractDuoFunc(A, B)
{
    if (argument[1] < argument[0])
        std::swap(argument[0], argument[1]);
}

void ScalarDot::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out << "S(";
    argument[0]->print(1, out);
    out << ", ";
    argument[1]->print(1, out);
    out << ")";
    if (mode == 0)
        out << std::endl;
}

std::string ScalarDot::printLaTeX(int mode) const
{
    std::ostringstream sout;
    sout << "S(";
    sout << argument[0]->printLaTeX(1);
    sout << ", ";
    sout << argument[1]->printLaTeX(1);
    sout << ")";
    if (mode == 0)
        sout << std::endl;

    return sout.str();
}

csl::unique_Expr ScalarDot::copy_unique() const
{
    return std::make_unique<ScalarDot>(argument[0], argument[1]);
}

csl::Expr ScalarDot::refresh() const
{
    return scalardot_s(argument[0], argument[1]);
}

csl::Expr ScalarDot::deepCopy() const
{
    return csl::make_shared<ScalarDot>(argument[0]->deepCopy(),
                                       argument[1]->deepCopy());
}

csl::Expr ScalarDot::deepRefresh() const
{
    return scalardot_s(argument[0]->deepRefresh(),
                       argument[1]->deepRefresh());
}

std::optional<csl::Expr> ScalarDot::evaluate(
        csl::eval::mode) const
{
    return std::nullopt;
}

bool ScalarDot::isIndexed() const
{
    return argument[0]->getIndexStructure().size() > 1
        or argument[1]->getIndexStructure().size() > 1;
}

csl::IndexStructure ScalarDot::getIndexStructure() const
{
    csl::IndexStructure res =
        argument[0]->getIndexStructure() + argument[1]->getIndexStructure();
    for (size_t i = 0; i != res.size(); ++i)
        for (size_t j = i+1; j < res.size(); ++j)
            if (res[i] == res[j] and res[i].getFree()) {
                res[i].setFree(false);
                res[j].setFree(false);
                break;
            }
    return res;
}

bool ScalarDot::operator==(csl::Expr_info other) const
{
    if (typeid(*other) != typeid(*this))
        return false;

    return (argument[0]*argument[1] == (*other)[0]*(*other)[1]);
}

csl::Expr scalardot_s(csl::Expr const& impulsion,
                 csl::Expr const& mass)
{
    return csl::make_shared<ScalarDot, alloc_scalardot>(impulsion, mass);
}

}
