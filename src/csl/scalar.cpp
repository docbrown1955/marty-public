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

#include "scalar.h"
#include "indicial.h"
#include "interface.h"
#include "utils.h"

namespace csl {

Scalar::Scalar(Expr const &left, Expr const &right)
    : AbstractDuoFunc(left, right)
{
}

Scalar::~Scalar()
{
}

csl::Type Scalar::getType() const
{
    return csl::Type::Scalar;
}

void Scalar::print(int mode, std::ostream &out, bool lib) const
{
    if (lib) {
        (argument[0] * argument[1])->print(mode, out, lib);
        return;
    }
    out << "S(";
    argument[0]->print(1, out);
    out << ", ";
    argument[1]->print(1, out);
    out << ")";
    if (mode == 0)
        out << std::endl;
}

std::string Scalar::printLaTeX(int mode) const
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

std::optional<Expr> Scalar::evaluate(csl::eval::mode) const
{
    return std::nullopt;
}

unique_Expr Scalar::copy_unique() const
{
    return std::make_unique<Scalar>(argument[0], argument[1]);
}

Expr Scalar::deepCopy() const
{
    return csl::make_shared<Scalar>(DeepCopy(argument[0]),
                                    DeepCopy(argument[1]));
}

Expr Scalar::refresh() const
{
    return scalar_(argument[0], argument[1]);
}

Expr Scalar::deepRefresh() const
{
    return scalar_(DeepRefreshed(argument[0]), DeepRefreshed(argument[1]));
}

bool Scalar::isIndexed() const
{
    return false;
}

IndexStructure Scalar::getIndexStructure() const
{
    return IndexStructure();
}

bool Scalar::compareWithDummy(Expr_info                         other,
                              std::map<csl::Index, csl::Index> &constraints,
                              bool keepAllCosntraints) const
{
    if (other->getType() != csl::Type::Scalar)
        return false;

    std::map<csl::Index, csl::Index> copyConstraints(constraints);

    if (argument[0]->compareWithDummy(
            other->getArgument(0).get(), constraints, keepAllCosntraints)
        and argument[1]->compareWithDummy(
            other->getArgument(1).get(), constraints, keepAllCosntraints)) {
        return true;
    }
    constraints = copyConstraints;
    return (argument[0]->compareWithDummy(
                other->getArgument(1).get(), constraints, keepAllCosntraints)
            and argument[1]->compareWithDummy(
                other->getArgument(0).get(), constraints, keepAllCosntraints));
}

bool Scalar::operator==(Expr_info other) const
{
    if (other->getType() != csl::Type::Scalar)
        return false;

    std::map<csl::Index, csl::Index> constraints;
    if (argument[0]->compareWithDummy(other->getArgument(0).get(), constraints)
        and argument[1]->compareWithDummy(other->getArgument(1).get(),
                                          constraints)) {
        return true;
    }
    return (
        argument[0]->compareWithDummy(other->getArgument(1).get(), constraints)
        and argument[1]->compareWithDummy(other->getArgument(0).get(),
                                          constraints));
}

Expr scalar_(Expr const &left, Expr const &right)
{
    if (left == CSL_0 or right == CSL_0)
        return CSL_0;
    if (left->getNumericalFactor() != CSL_1)
        return left->getNumericalFactor()
               * scalar_(left->getTerm().value(), right);
    if (right->getNumericalFactor() != CSL_1)
        return right->getNumericalFactor()
               * scalar_(left, right->getTerm().value());

    auto structL = left->getIndexStructure();
    auto structR = right->getIndexStructure();
    CSL_ASSERT_SPEC(structL.size() == 1 and structR.size() == 1,
                    CSLError::TypeError,
                    "Expected vectors (only 1 index) in indicial scalar.");

    return csl::make_shared<Scalar>(ContractIndex(left, structL[0]),
                                    ContractIndex(right, structR[0]));
}

} // namespace csl
