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

#include "fermionCurrent.h"
#include "fermionChain.h"
#include "mrtError.h"
using namespace csl;
namespace mty {

std::vector<FermionCurrent> parseChains(
        csl::Expr            & product,
        DiracSpace const* diracSpace,
        bool              findAbrev)
{
    std::vector<FermionCurrent> currents;
    std::vector<FermionChain::Chain> positions
        = FermionChain::getChains(product, diracSpace, false);
    for (const auto& chain : positions) {
        std::vector<csl::Expr> tensors;
        tensors.reserve(chain.pos.size());
        for (size_t p : chain.pos) {
            tensors.push_back((*product)[p]);
            if (findAbrev)
                (*product)[p] = CSL_1;
        }
        currents.push_back(FermionCurrent(tensors, diracSpace));
        csl::Index first = currents.back().getFirst();
        csl::Index last  = currents.back().getLast();
        for (size_t i = 0; i != product->size(); ++i) {
            auto iter = std::find(chain.pos.begin(), chain.pos.end(), i);
            if (iter != chain.pos.end())
                continue;
            csl::IndexStructure structure = (*product)[i]->getIndexStructure();
            for (csl::Index index : structure)
                if (index == first)
                    currents.back().setLeft((*product)[i]);
                else if (index == last)
                    currents.back().setRight((*product)[i]);
        }
        if (findAbrev) 
            (*product)[chain.pos[0]] = csl::Abbrev::makeAbbreviation(
                    "Fc", prod_s(currents.back().getTensors(), true));
    }
    if (findAbrev) {
        product = Refreshed(product);
    }

    return currents;
}

FermionCurrent::FermionCurrent(std::vector<csl::Expr> const& t_tensors,
                               const DiracSpace *t_diracSpace)
    :diracSpace(t_diracSpace),
    left(CSL_1),
    right(CSL_1)
{
    tensors = diracSpace->simplifyChain(t_tensors);
    factor = tensors[0];
    tensors.erase(tensors.begin());
    std::cout << tensors.size() << " tensors" << std::endl;
    for (const auto& t : tensors)
        t->print();
    type = determineTypeOfChain(tensors);
    determineBorderIndices(tensors);
}

FermionCurrent::Type FermionCurrent::getType() const
{
    return type;
}

void FermionCurrent::setType(FermionCurrent::Type t_type)
{
    type = t_type;
}

csl::Expr FermionCurrent::getExpression() const
{
    if (factor == CSL_1)
        return csl::prod_s({left, csl::prod_s(tensors, true), right}, true);
    return csl::prod_s({factor, left, csl::prod_s(tensors, true), right}, true);
}

std::vector<csl::Expr>& FermionCurrent::getTensors()
{
    return tensors;
}

std::vector<csl::Expr> const& FermionCurrent::getTensors() const
{
    return tensors;
}

csl::Index FermionCurrent::getFirst() const
{
    return first;
}

csl::Index FermionCurrent::getLast() const
{
    return last;
}

csl::Expr FermionCurrent::getFactor() const
{
    return factor;
}

csl::Expr FermionCurrent::getLeft() const
{
    return left;
}

csl::Expr FermionCurrent::getRight() const
{
    return right;
}

void FermionCurrent::setFactor(csl::Expr const& t_factor)
{
    factor = t_factor;
}

void FermionCurrent::setLeft(csl::Expr const& t_left)
{
    left = t_left;
}

void FermionCurrent::setRight(csl::Expr const& t_right)
{
    right = t_right;
}

#define FERMION_CHAIN_ERROR \
    CallHEPError(mty::error::TypeError,\
            "Chain " + toString(prod_s(tensors)) + " not recognized as a"\
            " fermion current.")

FermionCurrent::Type FermionCurrent::determineTypeOfChain(
        std::vector<csl::Expr> const& tensors) const
{
    if (tensors.empty())
        return FermionCurrent::Scalar;
    if (tensors.size() == 1) {
        csl::Expr gamm = tensors[0];
        if (tensors[0]->getParent_info() == diracSpace->getDelta().get())
            return FermionCurrent::Scalar;
        if (diracSpace->isGammaMu(gamm))
            return FermionCurrent::Vector;
        if (diracSpace->isGammaChir(gamm))
            return FermionCurrent::PseudoScalar;
        if (diracSpace->isP_L(gamm))
            return FermionCurrent::LScalar;
        if (diracSpace->isP_R(gamm))
            return FermionCurrent::RScalar;
    }
    if (tensors.size() == 2) {
        csl::Expr gamm_1 = tensors[0];
        csl::Expr gamm_2 = tensors[1];
        if (not diracSpace->isGammaMu(gamm_1))
            FERMION_CHAIN_ERROR;
        if (diracSpace->isGammaMu(gamm_2))
            return FermionCurrent::Magnetic;
        if (diracSpace->isGammaChir(gamm_2))
            return FermionCurrent::Axial;
        if (diracSpace->isP_L(gamm_2))
            return FermionCurrent::LVector;
        if (diracSpace->isP_R(gamm_2))
            return FermionCurrent::RVector;
    }
    if (tensors.size() == 3) {
        csl::Expr gamm_1 = tensors[0];
        csl::Expr gamm_2 = tensors[1];
        csl::Expr gamm_3 = tensors[2];
        if (not diracSpace->isGammaMu(gamm_1)
                or not diracSpace->isGammaMu(gamm_2))
            FERMION_CHAIN_ERROR;
        if (diracSpace->isGammaChir(gamm_3))
            return FermionCurrent::AMagnetic;
        if (diracSpace->isP_L(gamm_3))
            return FermionCurrent::LMagnetic;
        if (diracSpace->isP_R(gamm_3))
            return FermionCurrent::RMagnetic;
    }
    FERMION_CHAIN_ERROR;

    return FermionCurrent::Scalar;
}
#undef FERMION_CHAIN_ERROR

void FermionCurrent::determineBorderIndices(
        std::vector<csl::Expr> const& tensors)
{
    if (tensors.empty())
        return;
    auto [t_first, t_last] = diracSpace->getBorderOfChain(
            tensors.begin(),
            tensors.end());
    first = t_first;
    last = t_last;
}

bool FermionCurrent::operator==(FermionCurrent const& other) const
{
    return diracSpace == other.diracSpace
        and type == other.type;
}

bool FermionCurrent::operator!=(FermionCurrent const& other) const
{
    return !(*this == other);
}

bool FermionCurrent::operator<(FermionCurrent const& other) const
{
    return static_cast<int>(type) < static_cast<int>(other.type);
}

bool FermionCurrent::operator>(FermionCurrent const& other) const
{
    return other < *this;
}

bool FermionCurrent::operator>=(FermionCurrent const& other) const
{
    return !(other < *this);
}

bool FermionCurrent::operator<=(FermionCurrent const& other) const
{
    return !(other > *this);
}

std::ostream& operator<<(std::ostream& out,
                         FermionCurrent::Type type)
{
    switch(type) {
        case FermionCurrent::Scalar:       out << "Scalar";       break;
        case FermionCurrent::PseudoScalar: out << "PseudoScalar"; break;
        case FermionCurrent::LScalar:      out << "LScalar";      break;
        case FermionCurrent::RScalar:      out << "RScalar";      break;
        case FermionCurrent::Vector:       out << "Vector";       break;
        case FermionCurrent::Axial:        out << "Axial";        break;
        case FermionCurrent::LVector:      out << "LVector";      break;
        case FermionCurrent::RVector:      out << "RVector";      break;
        case FermionCurrent::Magnetic:     out << "Magnetic";     break;
        case FermionCurrent::AMagnetic:    out << "AMagnetic";    break;
        case FermionCurrent::LMagnetic:    out << "LMagnetic";    break;
        case FermionCurrent::RMagnetic:    out << "RMagnetic";    break;
    }
    return out;
}


std::ostream& operator<<(std::ostream& out,
                         FermionCurrent const& current)
{
    out << "Fermion current of type " << current.type << std::endl;
    if (current.factor != CSL_1)
        out << current.factor << "*";
    out << current.left << prod_s(current.tensors, true) 
        << current.right << std::endl;

    return out;
}

}

