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

#include "polarization.h"
#include "quantumField.h"
#include "diracology.h"
#include "mrtUtils.h"

using namespace std;
using namespace csl;

namespace mty {

PolarizationField::PolarizationField()
    :PolarizationField(nullptr, nullptr)
{

}

PolarizationField::PolarizationField(const Tensor& impulsion,
                                     const Parent&  t_parent)
    :QuantumField(impulsion, t_parent),
    lockConjugation(false)
{
    particle = incoming = onShell = true;
}

PolarizationField::PolarizationField(const Tensor&            impulsion,
                                     const Parent&             t_parent,
                                     const std::vector<Index>& indices)
    :QuantumField(impulsion, t_parent, indices),
    lockConjugation(false)

{
    particle = incoming = onShell = true;
}

PolarizationField::PolarizationField(const Tensor&        impulsion,
                                     const Parent&         t_parent,
                                     const IndexStructure& indices)
    :QuantumField(impulsion, t_parent, indices),
    lockConjugation(false)

{
    particle = incoming = onShell = true;
}

bool PolarizationField::getCommutable() const
{
    return true;
}

optional<csl::Expr> PolarizationField::getComplexConjugate() const
{
    return csl::make_shared<PolarizationField>(complexConjugatedField());
}

PolarizationField PolarizationField::complexConjugatedField(
        bool keepFermionOrder
        ) const
{
    PolarizationField copied(*this);
    copied.incoming = !incoming;
    copied.conjugated = not conjugated;
    if (!keepFermionOrder)
        copied.partnerShip.isLeft ^= 1; // Flips the boolean
    return copied;
}

bool PolarizationField::spaceIndexContraction(csl::Expr_info other) const
{
    return index.size() > 0
        and index.back() == other->getIndexStructureView()[0];
}

bool PolarizationField::spinIndexContraction(csl::Expr_info other) const
{
    return index.size() > 0
        and index[0] == other->getIndexStructureView()[0];
}

bool PolarizationField::hasContractionProperty(csl::Expr_info other) const
{
    if (isContractibleWith(other))
        return spinIndexContraction(other);
    // if (onShell 
    //         and csl::IsIndicialTensor(other)
    //         and other->getParent_info() == point.get())
    //     return spaceIndexContraction(other);
    bool fermion = getQuantumParent()->getSpinDimension() == 2;
    if (fermion
            and csl::IsIndicialTensor(other) 
            and other->getParent_info() == dirac4.C_matrix.get()
            and other->getIndexStructureView()[1] == index.back()) 
        return true;
    if (fermion and IsOfType<PolarizationField>(other)) {
        auto polar = dynamic_cast<mty::PolarizationField const*>(other);
        return hasFieldChargeConjugation(polar);
    }
    return false;
}

bool PolarizationField::isContractibleWith(csl::Expr_info otherPolar) const
{
    if (not IsOfType<PolarizationField>(otherPolar)
            or otherPolar->getParent() != parent
            or otherPolar->getPoint()  != point)
        return false;
    //const PolarizationField* converted 
    //    = ConvertToPtr<PolarizationField>(otherPolar);
    return true;
}

csl::Expr PolarizationField::contraction(csl::Expr_info other) const
{
    HEPAssert(csl::IsIndicialTensor(other),
            mty::error::TypeError,
            "Polarization field " + toString(copy()) + " does not contract "
            "with non tensor " + toString(other->copy()) + ".");
    if (onShell 
            and csl::IsIndicialTensor(other)
            and other->getParent_info() == point.get())
        return CSL_0;
    if (isFermionic() and IsOfType<PolarizationField>(other)) {
        auto polar = dynamic_cast<mty::PolarizationField const*>(other);
        if (hasFieldChargeConjugation(polar))
            return fieldChargeConjugation(polar);
    }
    if (other->getParent_info() == parent.get())
        return sumPolarization(other);
    if (other->getParent_info() == dirac4.C_matrix.get())
        return matrixChargeConjugation(other);
    CallHEPError(mty::error::TypeError,
            "Polarization field " + toString(copy()) + " does not contract "
            "with " + toString(other->copy()) + ".");
    return nullptr;
}

csl::Expr PolarizationField::matrixChargeConjugation(csl::Expr_info other) const
{
    if (lockConjugation) {
        // If the conjugation is locked, we swap the indices of C to send it
        // on the other side of the 
        csl::IndexStructure structure = other->getIndexStructure();
        std::swap(structure[0], structure[1]);
        return -mty::dirac4.C_matrix(structure.getIndex()) * copy();
    }
    PolarizationField newPolar(*this);
    int sign = (isComplexConjugate()) ? -1 : 1;
    newPolar.setParticle(!newPolar.particle);
    csl::Index spaceIndex = index.back();
    csl::IndexStructure const &C = other->getIndexStructureView();
    if (C[0] == spaceIndex) {
        sign *= -1;
        spaceIndex = C[1];
    }
    else
        spaceIndex = C[0];
    newPolar.index.back() = spaceIndex;

    return int_s(sign) * newPolar.copy();
}

bool PolarizationField::hasFieldChargeConjugation(
        mty::PolarizationField const *other
        ) const
{
    auto partner = getPartnerShip();
    auto partner_other = other->getPartnerShip();
    if (!partner.isHappyWith(partner_other))
        return false;
    if (isComplexConjugate() == other->isComplexConjugate())
        return false;
    if (index.back() != other->index.back())
        return false;
    const bool res = partner.isLeft != isComplexConjugate();
    return res;
}

csl::Expr PolarizationField::fieldChargeConjugation(
        mty::PolarizationField const *other
        ) const
{
    auto pol_self  = complexConjugatedField(true);
    auto pol_other = other->complexConjugatedField(true);

    return -pol_self.copy() * pol_other.copy();
}

csl::Expr PolarizationField::sumPolarization(csl::Expr_info other) const
{
    if (!conjugated or other->isComplexConjugate()) {

        int spin_x_2 = getQuantumParent()->getSpinDimension() - 1;

        // Structures of the two polarization tensors
        IndexStructure structA = index;
        IndexStructure structB = other->getIndexStructure();
        // Erasing polarization index that is summed over
        structA.erase(structA.begin());
        structB.erase(structB.begin());
        csl::Expr mass = getMass();
        switch (spin_x_2) {

            case 0:
            {
            // For each Index the sum gives a kronecker delta
            csl::Expr deltaFactor = CSL_1;
            for (size_t i = 0; i != structA.size(); ++i)
                deltaFactor = deltaFactor * structA[i].getSpace()->getDelta()(
                        {structA[i],
                         structB[i]});
            // Impulsion of the particle
            return deltaFactor;
            }

            case 1:
            {
            // Getting dirac indices and getting them out of structures to
            // isolate gauge indices.
            Index alpha = structA[structA.size()-1];
            Index beta  = structB[structB.size()-1];
            csl::Expr C = CSL_1;
            bool signMassTerm = !particle;
            if (!conjugated and !other->isComplexConjugate()) {
                Index gam = beta.rename();
                C = dirac4.C_matrix({gam, beta});
                beta = gam;
            }
            else if (conjugated and other->isComplexConjugate()) {
                Index gam = alpha.rename();
                signMassTerm = !signMassTerm;
                C = dirac4.C_matrix({alpha, gam});
                alpha = gam;
            }
            structA.erase(structA.end()-1);
            structB.erase(structB.end()-1);
            // For each Index the sum gives a kronecker delta
            csl::Expr deltaFactor = CSL_1;
            for (size_t i = 0; i != structA.size(); ++i)
                deltaFactor = deltaFactor * structA[i].getSpace()->getDelta()(
                        {structA[i],
                         structB[i]});
            // Impulsion of the particle
            TensorParent& P = *point;
            Index mu = Minkowski.generateIndex();

            if (!signMassTerm) {
                auto res = deltaFactor * C * (dirac4.gamma({+mu,alpha,beta})*P(mu)
                        + mass*dirac4.getDelta()({alpha, beta}));
                return res;
            }
            else  {
                auto res = deltaFactor * C * (dirac4.gamma({+mu,alpha,beta})*P(mu)
                        - mass*dirac4.getDelta()({alpha, beta}));
                return res;
            }
            }

            case 2:
            {
            // Getting Minkowski and getting them out of the structure to 
            // isolate gauge indices
            Index mu = structA[structA.size()-1];
            Index nu = structB[structB.size()-1];
            structA.erase(structA.end()-1);
            structB.erase(structB.end()-1);
            // For each Index the sum gives a kronecker delta
            csl::Expr deltaFactor = CSL_1;
            for (size_t i = 0; i != structA.size(); ++i)
                deltaFactor = deltaFactor * structA[i].getSpace()->getDelta()(
                        {structA[i],
                         structB[i]});
            // Impulsion of the particle
            // TensorParent& P = *point;
            Index rho = Minkowski.generateIndex();
            TensorParent& g = *Minkowski.getMetric();
            Parent P = point;
            // csl::Expr XI = mty::gauge::Choice::getXi();
            if (mass == CSL_0)
                return -(g({mu, nu})) * deltaFactor;
            else
                return -(g({mu, nu}) - P(mu)*P(nu)/(mass*mass)) * deltaFactor;
            }

            default:
            CallHEPError(mty::error::NotImplementedError,
                    "Spin(x2) " + toString(spin_x_2) + " not recognized for "
                    + "polarization sum.");
            return CSL_UNDEF;
        }
    }
    else {
        return other->contraction(this);
    }
    return CSL_UNDEF;
}

csl::unique_Expr PolarizationField::copy_unique() const
{
    return std::make_unique<PolarizationField>(*this);
}

void PolarizationField::printCode(
        int           mode,
        std::ostream &out
        ) const
{
    PolarizationField other(*this);
    other.index.erase(other.index.begin());
    other.TensorFieldElement::printCode(mode, out);
}

void PolarizationField::printProp(std::ostream& out) const
{
    if (isFermionic()) {
        if (isParticle())
            out << "_u";
        else
            out << "_v";
    }
    if (conjugated)
        out << "^(*)";
}

void PolarizationField::setParticle(bool t_particle)
{
    particle = t_particle;
    updateComplexConjugation();
}

void PolarizationField::setIncoming(bool t_incoming)
{
    incoming = t_incoming;
    updateComplexConjugation();
}

void PolarizationField::updateComplexConjugation()
{
    int spinDim = getQuantumParent()->getSpinDimension();
    if (spinDim == 2)
        setConjugated(isOutgoingParticle() or isIncomingAntiParticle());
    else
        conjugated = !incoming;
        //setConjugated(not incoming);
}

ostream& operator<<(ostream& fout, const PolarizationField& pol)
{
    fout << pol.getName() << ": ";
    if (pol.incoming)
        fout << "incoming ";
    else
        fout << "outgoing ";
    if (pol.isSelfConjugate())
        fout << "(anti-)particle";
    else if (pol.particle)
        fout << "particle";
    else
        fout << "antiparticle";

    return fout;
}

} // End of namespace mty
