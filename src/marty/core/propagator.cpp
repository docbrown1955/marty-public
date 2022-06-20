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

#include "propagator.h"
#include "mrtError.h"

using namespace csl;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class Propagator                              //
/*************************************************/
///////////////////////////////////////////////////

Propagator::Propagator(csl::Expr const &impulsion,
                       csl::Expr const &mass,
                       csl::Expr const &width)
    : AbstractMultiFunc()
{
    if (impulsion == CSL_0 && mass == CSL_0) {
        std::cerr << "Warning: division by zero in propagator (p = m = 0).";
        std::cerr << "This may come from tadpole diagrams or non renormalized "
                     "two-point function contributions (in (n > 2)-point "
                     "function).\n";
        std::cerr << "Regulating with the propagator regulator \""
                  << Peps->getName() << "\"." << '\n';
    }
    argument = {impulsion, mass, width};
    if (auto m_impulsion = Expanded(-1 * impulsion); m_impulsion < impulsion)
        argument[0] = m_impulsion;
}

void Propagator::print(int mode, std::ostream &out, bool lib) const
{
    if (lib) {
        csl::Expr counter = DeepCopy(argument[0]);
        for (auto &index : counter->getIndexStructure())
            Replace(counter, index, index.getFlipped());
        (1
         / (argument[0] * counter - argument[1] * argument[1]
            + CSL_I * argument[1] * argument[2]))
            ->print(mode, out, lib);
        return;
    }
    out << "Prop(";
    argument[0]->print(1, out);
    out << ", ";
    argument[1]->print(1, out);
    if (argument[2] != CSL_0) {
        out << ", ";
        (argument[2])->print(1, out);
    }
    out << ")";
    if (mode == 0)
        out << std::endl;
}

std::string Propagator::printLaTeX(int mode) const
{
    std::ostringstream sout;
    sout << "Prop(";
    sout << argument[0]->printLaTeX(1);
    sout << ", ";
    sout << argument[1]->printLaTeX(1);
    if (argument[2] != CSL_0) {
        sout << ", ";
        sout << (argument[2])->printLaTeX(1);
    }
    sout << ")";
    if (mode == 0)
        sout << std::endl;

    return sout.str();
}

csl::unique_Expr Propagator::copy_unique() const
{
    return std::make_unique<Propagator>(argument[0], argument[1], argument[2]);
}

std::optional<csl::Expr> Propagator::getComplexConjugate() const
{
    if (argument[2] == CSL_0)
        return std::nullopt;
    Propagator p(argument[0], argument[1], -argument[2]);
    return p.copy();
}

csl::Expr Propagator::refresh() const
{
    return propagator_s(argument[0], argument[1], argument[2]);
}

csl::Expr Propagator::deepCopy() const
{
    return csl::make_shared<Propagator>(argument[0]->deepCopy(),
                                        argument[1]->deepCopy(),
                                        argument[2]->deepCopy());
}

csl::Expr Propagator::deepRefresh() const
{
    return propagator_s(argument[0]->deepRefresh(),
                        argument[1]->deepRefresh(),
                        argument[2]->deepRefresh());
}

std::optional<csl::Expr> Propagator::evaluate(csl::eval::mode user_mode) const
{
    if (csl::eval::isContained(user_mode, csl::eval::abbreviation)
        and (argument[0] != CSL_0 or argument[1] != CSL_0)) {
        csl::Expr counter = DeepCopy(argument[0]);
        for (auto &index : counter->getIndexStructure())
            Replace(counter, index, index.getFlipped());
        csl::Expr res
            = csl::Evaluated(argument[0] * counter - argument[1] * argument[1]
                                 + CSL_I * argument[1] * argument[2],
                             user_mode);
        csl::DeepExpand(res);
        res += Peps;
        return 1 / res;
    }
    std::optional<csl::Expr> arg1_opt = argument[0]->evaluate(user_mode);
    std::optional<csl::Expr> arg2_opt = argument[1]->evaluate(user_mode);
    std::optional<csl::Expr> arg3_opt = argument[2]->evaluate(user_mode);
    if (arg1_opt or arg2_opt or arg3_opt) {
        return mty::propagator_s(arg1_opt.value_or(csl::Copy(argument[0])),
                                 arg2_opt.value_or(csl::Copy(argument[1])),
                                 arg3_opt.value_or(csl::Copy(argument[2])));
    }
    return std::nullopt;
}

std::optional<csl::Expr> Propagator::derive(csl::Expr_info var) const
{
    return csl::Evaluated(copy(), csl::eval::abbreviation)->derive(var);
}

bool Propagator::isIndexed() const
{
    return false;
}

csl::IndexStructure Propagator::getIndexStructure() const
{
    return csl::IndexStructure();
}

bool Propagator::operator==(csl::Expr_info other) const
{
    if (typeid(*other) != typeid(*this))
        return false;

    if (csl::pow_s(argument[1], 2) != csl::pow_s(other->getArgument(1), 2))
        return false;
    if (argument[2] != other->getArgument(2))
        return false;

    csl::Expr counter1 = DeepCopy(argument[0]);
    for (auto &index : counter1->getIndexStructure())
        Replace(counter1, index, index.getFlipped());
    csl::Expr counter2 = DeepCopy(other->getArgument(0));
    for (auto &index : counter2->getIndexStructure())
        Replace(counter2, index, index.getFlipped());

    return (Expanded(argument[0] * counter1)
            == Expanded(other->getArgument(0) * counter2));
}

csl::Expr propagator_s(csl::Expr const &impulsion,
                       csl::Expr const &mass,
                       csl::Expr const &width)
{
    return csl::make_shared<Propagator, alloc_propagator>(
        impulsion, mass, width);
}

///////////////////////////////////////////////////
/*************************************************/
// Class FermionPropStruct                       //
/*************************************************/
///////////////////////////////////////////////////

FermionPropStruct::FermionPropStruct(csl::Expr const & impulsion,
                                     csl::Expr const & mass,
                                     csl::Index const &alpha,
                                     csl::Index const &beta)
    : AbstractDuoFunc(impulsion, mass), structure({alpha, beta})
{
}

void FermionPropStruct::print(int mode, std::ostream &out, bool) const
{
    out << "Fermi(";
    argument[0]->print(1, out);
    out << ", ";
    argument[1]->print(1, out);
    out << ")";
    out << "_" << structure;
    if (mode == 0)
        out << std::endl;
}

std::string FermionPropStruct::printLaTeX(int mode) const
{
    std::ostringstream sout;
    sout << "Fermi(";
    sout << argument[0]->printLaTeX(1);
    sout << ", ";
    sout << argument[1]->printLaTeX(1);
    sout << ")";
    sout << "_" << structure;
    if (mode == 0)
        sout << std::endl;

    return sout.str();
}

csl::unique_Expr FermionPropStruct::copy_unique() const
{
    return std::make_unique<FermionPropStruct>(
        argument[0], argument[1], structure[0], structure[1]);
}

csl::Expr FermionPropStruct::refresh() const
{
    return fermionPropStruct_s(
        argument[0], argument[1], structure[0], structure[1]);
}

csl::Expr FermionPropStruct::deepCopy() const
{
    return csl::make_shared<FermionPropStruct>(argument[0]->deepCopy(),
                                               argument[1]->deepCopy(),
                                               structure[0],
                                               structure[1]);
}

csl::Expr FermionPropStruct::deepRefresh() const
{
    return fermionPropStruct_s(argument[0]->deepRefresh(),
                               argument[1]->deepRefresh(),
                               structure[0],
                               structure[1]);
}

std::optional<csl::Expr> FermionPropStruct::evaluate(csl::eval::mode) const
{
    return std::nullopt;
}

bool FermionPropStruct::isIndexed() const
{
    return true;
}

csl::IndexStructure FermionPropStruct::getIndexStructure() const
{
    return structure;
}

bool FermionPropStruct::operator==(csl::Expr_info other) const
{
    if (typeid(*other) != typeid(*this))
        return false;

    return argument[1] == (*other)[1]
           and csl::Comparator::freeIndexComparison(argument[0].get(),
                                                    (*other)[0].get());
}

std::optional<csl::Expr>
FermionPropStruct::getHermitianConjugate(const csl::Space *space) const
{
    return getHermitianConjugate(std::vector<const csl::Space *>(1, space));
}

std::optional<csl::Expr> FermionPropStruct::getHermitianConjugate(
    const std::vector<const csl::Space *> &space) const
{
    for (const auto &s : space) {
        if (structure[0].getSpace() == s) {
            return fermionPropStruct_s(
                argument[0], argument[1], structure[1], structure[0]);
        }
    }

    return std::nullopt;
}

std::optional<csl::Expr>
FermionPropStruct::replaceIndex(const csl::Index &indexToReplace,
                                const csl::Index &newIndex,
                                bool              refresh) const
{
    for (const auto &i : structure)
        if (i.exactMatch(indexToReplace)) {
            csl::Expr other = copy_unique();
            Replace(other, indexToReplace, newIndex);
            if (refresh)
                return Refreshed(std::move(other));
            return csl::Expr(std::move(other));
        }

    return std::nullopt;
}

csl::Expr fermionPropStruct_s(csl::Expr const & impulsion,
                              csl::Expr const & mass,
                              csl::Index const &alpha,
                              csl::Index const &beta)
{
    return csl::make_shared<FermionPropStruct, alloc_fermionprop>(
        impulsion, mass, alpha, beta);
}

} // namespace mty
