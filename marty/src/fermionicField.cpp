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

#include "model.h"
#include "fermionicField.h"
#include "quantumFieldTheory.h"

using namespace std::string_literals;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class WeylFermion                             //
/*************************************************/
///////////////////////////////////////////////////

WeylFermion::WeylFermion()
{
    initPropagator();
}

WeylFermion::WeylFermion(
        std::weak_ptr<DiracFermion> t_parent,
        Chirality           t_chirality)
    :QuantumFieldParent(*t_parent.lock()),
    chirality(t_chirality),
    diracParent(t_parent)
{
    initPropagator();
}

WeylFermion::WeylFermion(
        const std::string &t_name,
        Model const&       t_model,
        Chirality t_chirality)
    :WeylFermion(t_name, t_model.getGauge(), t_chirality)
{
    if (t_model.getFlavor()) {
        flavor = t_model.getFlavor();
        flavorRep = flavor->getTrivialRep();
    }
}

WeylFermion::WeylFermion(
        const std::string &t_name,
        Gauge             *gauge,
        Chirality t_chirality)
    :QuantumFieldParent(t_name, 1, gauge),
    chirality(t_chirality)
{
    initPropagator();
}

WeylFermion::WeylFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep,
            Chirality t_chirality)
    :QuantumFieldParent(t_name, 1, irrep),
    chirality(t_chirality)
{
    initPropagator();
}

WeylFermion::WeylFermion(
            const std::string& t_name,
            Gauge*             t_gauge,
            bool               t_isSelfConjugate,
            Chirality t_chirality)
    :QuantumFieldParent(t_name, 1, t_gauge, t_isSelfConjugate),
    chirality(t_chirality)
{
    initPropagator();
}


WeylFermion::WeylFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep,
            bool               t_isSelfConjugate,
            Chirality t_chirality)
    :QuantumFieldParent(t_name, 1, irrep, t_isSelfConjugate),
    chirality(t_chirality)
{
    initPropagator();
}


WeylFermion::WeylFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep,
            const FlavorIrrep& flavorRep,
            Chirality t_chirality)
    :QuantumFieldParent(t_name, 1, irrep, flavorRep),
    chirality(t_chirality)
{
    initPropagator();
}


WeylFermion::WeylFermion(
        const std::string& t_name,
        const WeylFermion* other)
    :WeylFermion(*other)
{
    setName(t_name);
    initPropagator();
}

void WeylFermion::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    out << " = mty::weylfermion_s(\"" << csl::Abstract::regularLiteral(name) 
        << " ; " << csl::Abstract::regularLiteral(latexName)
        << "\", _model_, ";
    if (chirality == Chirality::Left)
        out << "Chirality::Left);\n";
    else
        out << "Chirality::Right);\n";
    printQuantumDefinition(out, indentSize);
}

bool WeylFermion::isSameSpecies(QuantumFieldParent const *other) const
{
    if (this == other)
        return true;
    if (auto p = diracParent.lock(); p) {
        return p.get() == other
            || getChiralityCounterPart().get() == other;
    }
    return false;
}

bool WeylFermion::isChiral() const 
{
    return true;
}

Chirality WeylFermion::getChirality() const 
{
    return chirality;
}

Particle WeylFermion::generateSimilar(std::string const& t_name) const
{
    return csl::make_shared<WeylFermion>(t_name, this);
}

csl::Expr WeylFermion::getEnergyDimension() const
{
    return 3 * CSL_HALF;
}

int WeylFermion::getNDegreesOfFreedom() const 
{
    return 2 * getNGaugeDegreesOfFreedom();
}

csl::Expr WeylFermion::getKineticTerm(csl::Tensor & X)
{
    return FermionKineticTerm(*this, X);
}

Particle WeylFermion::getDiracParent() const
{
    Particle p = diracParent.lock();
    return p;
}

void WeylFermion::setMass(csl::Expr const &t_mass)
{
    auto parent = diracParent.lock();
    if (parent and (parent->getMass() != t_mass)) {
        parent->setMass(t_mass);
    }
    else
        QuantumFieldParent::setMass(t_mass);
}

void WeylFermion::setDiracParent(Particle const &diracFermion)
{
    std::shared_ptr<DiracFermion> fermion = 
        std::dynamic_pointer_cast<DiracFermion>(diracFermion);
    HEPAssert(fermion,
            mty::error::TypeError,
            "Expected a dirac fermion, " + toString(diracFermion->getName())
            + " given.");
    diracParent = fermion;
}

Particle WeylFermion::getChiralityCounterPart() const
{
    if (auto p = diracParent.lock(); p) {
        return (p->leftPart.get() == this) ? 
            p->rightPart : p->leftPart;
    }
    CallHEPError(mty::error::ValueError,
            "Cannot get chiral counter part from weyl fermion that is not in"s
            + " a dirac fermion.");

    return nullptr;
}

void WeylFermion::initPropagator()
{
    propagator.clear();
    if (not integratedOut)
        propagator[this] = &FermionPropagator;
    else
        propagator[this] = &IntegratedFermionPropagator;
}

void WeylFermion::setDrawType(drawer::ParticleType type)
{
    if (diracParent.lock())
        diracParent.lock()->setDrawType(type);
    else
        QuantumFieldParent::setDrawType(type);
}

///////////////////////////////////////////////////
/*************************************************/
// Class DiracFermion                            //
/*************************************************/
///////////////////////////////////////////////////

auto findWeylSignature(std::string const &name)
{
    constexpr static auto signatures = {"_L", "_R", "L", "R", "_l", "_r"};
    auto iter = signatures.begin();
    while (iter != signatures.end()) {
        auto pos = name.rfind(*iter);
        if (pos != std::string::npos)
            return pos;
        ++iter;
    }
    return std::string::npos;
}

std::string DiracNameOf(std::string const &leftWeylName)
{
    auto pos = findWeylSignature(leftWeylName);
    if (pos == std::string::npos) {
        return leftWeylName + "_D";
    }
    auto diracName { leftWeylName };
    diracName.erase(pos, 2);
    return diracName;
}

DiracFermion::DiracFermion()
{
    initPropagator();
}

DiracFermion::DiracFermion(
        std::shared_ptr<WeylFermion> const& left,
        std::shared_ptr<WeylFermion> const& right)
    :QuantumFieldParent(*left),
    leftPart(left),
    rightPart(right)
{
    std::string nameLeft  = left->getName();
    std::string latexNameLeft  = left->getLatexName();
    name = DiracNameOf(nameLeft);
    latexName = DiracNameOf(latexNameLeft);
    initPropagator();
}

DiracFermion::DiracFermion(
        const std::string &t_name,
        const Model&       model)
    :DiracFermion(t_name, model.getGauge())
{
    if (model.getFlavor()) {
        flavor = model.getFlavor();
        flavorRep = flavor->getTrivialRep();
    }
}

DiracFermion::DiracFermion(
        const std::string &t_name,
        Gauge             *gauge)
    :QuantumFieldParent(t_name, 1, gauge)
{
    leftPart = csl::make_shared<WeylFermion>(getName() + "_L", gauge, Chirality::Left);
    rightPart = csl::make_shared<WeylFermion>(getName() + "_R", gauge, Chirality::Right);
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}

DiracFermion::DiracFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep)
    :QuantumFieldParent(t_name, 1, irrep)
{
    leftPart = csl::make_shared<WeylFermion>(getName() + "_L", irrep, Chirality::Left);
    rightPart = csl::make_shared<WeylFermion>(getName() + "_R", irrep, Chirality::Right);
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}

DiracFermion::DiracFermion(
            const std::string& t_name,
            Gauge*             t_gauge,
            bool               t_isSelfConjugate)
    :QuantumFieldParent(t_name, 1, t_gauge, t_isSelfConjugate)
{
    leftPart = csl::make_shared<WeylFermion>(getName() + "_L", 
                                             t_gauge,
                                             t_isSelfConjugate,
                                             Chirality::Left);
    rightPart = csl::make_shared<WeylFermion>(getName() + "_R",
                                              t_gauge,
                                              t_isSelfConjugate,
                                              Chirality::Right);
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}


DiracFermion::DiracFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep,
            bool               t_isSelfConjugate)
    :QuantumFieldParent(t_name, 1, irrep, t_isSelfConjugate)
{
    leftPart = csl::make_shared<WeylFermion>(getName() + "_L", 
                                             irrep,
                                             t_isSelfConjugate,
                                             Chirality::Left);
    rightPart = csl::make_shared<WeylFermion>(getName() + "_R",
                                              irrep,
                                              t_isSelfConjugate,
                                              Chirality::Right);
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}


DiracFermion::DiracFermion(
            const std::string& t_name,
            const GaugeIrrep&  irrep,
            const FlavorIrrep& flavorRep)
    :QuantumFieldParent(t_name, 1, irrep, flavorRep)
{
    leftPart = csl::make_shared<WeylFermion>(getName() + "_L", 
                                             irrep,
                                             flavorRep,
                                             Chirality::Left);
    rightPart = csl::make_shared<WeylFermion>(getName() + "_R",
                                              irrep,
                                              flavorRep,
                                              Chirality::Right);
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}

DiracFermion::DiracFermion(
        const std::string& t_name,
        const DiracFermion* other)
    :DiracFermion(*other)
{
    setName(t_name);
    if (other->leftPart)
        leftPart  = csl::make_shared<WeylFermion>(
                getName() + "_L", other->leftPart.get());
    if (other->rightPart)
        rightPart = csl::make_shared<WeylFermion>(
                getName() + "_R", other->rightPart.get());
    leftPart->setLatexName(getLatexName() + "_L");
    rightPart->setLatexName(getLatexName() + "_R");
    initPropagator();
}

void DiracFermion::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    if (leftPart and rightPart) {
        out << indent << " = mty::diracfermion_s(\n"
            << indent << "std::dynamic_pointer_cast<mty::WeylFermion>(" << "    " 
            << csl::Abstract::regularName(leftPart->getName()) << "),\n "
            << indent << "    " << "std::dynamic_pointer_cast<mty::WeylFermion>("
            << csl::Abstract::regularName(rightPart->getName()) << "));\n";
    }
    else {
        out << " = mty::diracfermion_s(\"" 
            << csl::Abstract::regularLiteral(name) 
            << " ; " << csl::Abstract::regularLiteral(latexName)
            << "\"_model_, ";
    }
    printQuantumDefinition(out, indentSize);
}

void DiracFermion::initWeylFermions()
{
    if (not leftPart or not rightPart)
        return;
    leftPart->diracParent = std::dynamic_pointer_cast<DiracFermion>(self());
    rightPart->diracParent = std::dynamic_pointer_cast<DiracFermion>(self());
}

Particle DiracFermion::generateSimilar(std::string const& t_name) const
{
    return csl::make_shared<DiracFermion>(t_name, this);
}

csl::Expr DiracFermion::getEnergyDimension() const
{
    return 3 * CSL_HALF;
}

int DiracFermion::getNDegreesOfFreedom() const 
{
    return 2 * getNGaugeDegreesOfFreedom();
}

csl::Expr DiracFermion::getKineticTerm(csl::Tensor & X)
{
    return FermionKineticTerm(*this, X);
}

void DiracFermion::initPropagator()
{
    propagator.clear();
    if (not integratedOut) {
        propagator[this] = &FermionPropagator;
        AddCustomPropagator(this,
                            leftPart.get(),
                            &FermionPropagator);
        AddCustomPropagator(this,
                            rightPart.get(),
                            &FermionPropagator);
        AddCustomPropagator(leftPart.get(),
                            rightPart.get(),
                            &FermionPropagator);
    }
    else {
        propagator[this] = &IntegratedFermionPropagator;
        AddCustomPropagator(this,
                            leftPart.get(),
                            &IntegratedFermionPropagator);
        AddCustomPropagator(this,
                            rightPart.get(),
                            &IntegratedFermionPropagator);
        AddCustomPropagator(leftPart.get(),
                            rightPart.get(),
                            &FermionPropagator);
    }
}

void DiracFermion::setMass(csl::Expr const &t_mass)
{
    QuantumFieldParent::setMass(t_mass);
    if (leftPart)
        leftPart->setMass(t_mass);
    if (rightPart)
        rightPart->setMass(t_mass);
}

void DiracFermion::setDrawType(drawer::ParticleType type)
{
    QuantumFieldParent::setDrawType(type);
    if (leftPart)
        leftPart->QuantumFieldParent::setDrawType(type);
    if (rightPart)
        rightPart->QuantumFieldParent::setDrawType(type);
}

Particle DiracFermion::getWeylFermion(Chirality chirality) const
{
    HEPAssert(chirality != Chirality::None,
            mty::error::ValueError,
            "A weyl fermion must have a chirality.");
    Particle res = (chirality == Chirality::Left) ? leftPart : rightPart;
    HEPAssert(res,
            mty::error::ValueError,
            "Particle " + std::string(getName()) + " has no " 
            + " weyl fermion.");
    return res;
}

void DiracFermion::setEnabledInDiagrams(bool t_enabled)
{
    QuantumFieldParent::setEnabledInDiagrams(t_enabled);
    if (leftPart)
        leftPart->setEnabledInDiagrams(t_enabled);
    if (rightPart)
        rightPart->setEnabledInDiagrams(t_enabled);
}

}
