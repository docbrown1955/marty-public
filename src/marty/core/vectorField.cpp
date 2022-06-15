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

#include "vectorField.h"
#include "gaugedGroup.h"
#include "ghostField.h"
#include "goldstoneField.h"
#include "model.h"
#include "mrtError.h"
#include "quantumFieldTheory.h"

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class BaseVectorBoson                         //
/*************************************************/
///////////////////////////////////////////////////

BaseVectorBoson::BaseVectorBoson(const std::string &t_name, Gauge *t_gauge)
    : QuantumFieldParent(t_name, 2, t_gauge)
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &t_name, mty::Model &model)
    : BaseVectorBoson(t_name, model.getGauge())
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &t_name,
                                 const GaugeIrrep & irrep)
    : QuantumFieldParent(t_name, 2, irrep)
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &t_name,
                                 Gauge *            t_gauge,
                                 bool               t_isSelfConjugate)
    : QuantumFieldParent(t_name, 2, t_gauge, t_isSelfConjugate)
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &t_name,
                                 const GaugeIrrep & irrep,
                                 bool               t_isSelfConjugate)
    : QuantumFieldParent(t_name, 2, irrep, t_isSelfConjugate)
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &t_name,
                                 const GaugeIrrep & irrep,
                                 const FlavorIrrep &flavorRep)
    : QuantumFieldParent(t_name, 2, irrep, flavorRep)
{
}

BaseVectorBoson::BaseVectorBoson(const std::string &    t_name,
                                 const BaseVectorBoson *other)
    : BaseVectorBoson(*other)
{
    relatives.clear();
    setName(t_name);
}

csl::Expr BaseVectorBoson::getEnergyDimension() const
{
    return CSL_1;
}

int BaseVectorBoson::getNDegreesOfFreedom() const
{
    int dof = (getMass() == CSL_0) ? 2 : 3;
    return dof * getNGaugeDegreesOfFreedom();
}

csl::Expr BaseVectorBoson::getKineticTerm(csl::Tensor &X)
{
    return VectorKineticTerm(*this, X);
}

///////////////////////////////////////////////////
/*************************************************/
// Class VectorBoson                             //
/*************************************************/
///////////////////////////////////////////////////

void VectorBoson::setName(std::string t_name)
{
    BaseVectorBoson::setName(t_name);
    choice.setName("xi_" + t_name);

    if (fieldStrength) {
        fieldStrength->setName("F_" + getName());
        fieldStrength->setLatexName("F_" + getLatexName());
    }
    if (ghost) {
        ghost->setName("c_" + getName());
        ghost->setLatexName("c_" + getLatexName());
    }
    if (ghost_c) {
        ghost_c->setName("c_" + getName() + "c");
        ghost_c->setLatexName("c_" + getLatexName() + "_{c}");
    }
    if (goldstone) {
        goldstone->setName("G_" + getName());
        goldstone->setLatexName("G_" + getLatexName());
    }
}

void VectorBoson::printDefinition(std::ostream &out,
                                  int           indentSize,
                                  bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    out << indent << " = mty::vectorboson_s(\""
        << csl::Abstract::regularLiteral(name) << " ; "
        << csl::Abstract::regularLiteral(latexName) << "\", _model_);\n";
    printQuantumDefinition(out, indentSize);
    if (fieldStrength)
        fieldStrength->printDefinition(out, indentSize, header);
}

Particle VectorBoson::generateSimilar(std::string const &t_name) const
{
    auto res = csl::make_shared<VectorBoson>(t_name, this);
    res->setGaugeChoice(getGaugeChoice().getChoice());
    if (ghost) {
        auto newghost = std::make_shared<GhostBoson>("c_" + t_name, res);
        res->setGhostBoson(newghost);
    }
    if (ghost_c) {
        auto newghost = std::make_shared<GhostBoson>("c_" + t_name + "c", res);
        res->setConjugatedGhostBoson(newghost);
    }
    if (goldstone) {
        auto newgoldstone
            = std::make_shared<GoldstoneBoson>("c_" + t_name, res);
        res->setGoldstoneBoson(newgoldstone);
    }
    return res;
}

Particle VectorBoson::getGhostBoson() const
{
    return ghost;
}

Particle VectorBoson::getConjugatedGhostBoson() const
{
    return ghost_c;
}

Particle VectorBoson::getGoldstone() const
{
    return goldstone;
}

void VectorBoson::initPropagator()
{
    if (not integratedOut) {
        propagator.clear();
        propagator[this] = &VectorPropagator;
        AddCustomPropagator(
            this, fieldStrength.get(), &FieldStrengthPropagator);
        AddCustomPropagator(fieldStrength.get(),
                            &FieldStrengthSquaredPropagator);
    }
    else {
        propagator.clear();
        propagator[this] = &IntegratedVectorPropagator;
        AddCustomPropagator(
            this, fieldStrength.get(), &IntegratedFieldStrengthPropagator);
        AddCustomPropagator(fieldStrength.get(),
                            &IntegratedFieldStrengthSquaredPropagator);
    }
}

bool VectorBoson::isSameSpecies(QuantumFieldParent const *other) const
{
    return this == other || fieldStrength.get() == other;
}
bool VectorBoson::hasFieldStrength() const
{
    return true;
}

Particle VectorBoson::getFieldStrength() const
{
    return fieldStrength;
}

csl::Expr VectorBoson::getXiGauge() const
{
    return choice.getXi();
}

void VectorBoson::setGoldstoneBoson(Particle const &t_goldstone)
{
    auto const &pointed = *t_goldstone;
    HEPAssert(typeid(pointed) == typeid(GoldstoneBoson),
              mty::error::TypeError,
              "Expected a goldstone, " + std::string(typeid(pointed).name())
                  + " given.");
    goldstone = std::dynamic_pointer_cast<GoldstoneBoson>(t_goldstone);
    setGaugeChoice(choice.getChoice());
}

void VectorBoson::setGhostBoson(Particle const &t_ghost)
{
    auto const &pointed = *t_ghost;
    HEPAssert(typeid(pointed) == typeid(GhostBoson),
              mty::error::TypeError,
              "Expected a ghost, " + std::string(typeid(pointed).name())
                  + " given.");
    ghost = std::dynamic_pointer_cast<GhostBoson>(t_ghost);
    setGaugeChoice(choice.getChoice());
}

void VectorBoson::setConjugatedGhostBoson(Particle const &t_ghost)
{
    auto const &pointed = *t_ghost;
    HEPAssert(typeid(pointed) == typeid(GhostBoson),
              mty::error::TypeError,
              "Expected a ghost, " + std::string(typeid(pointed).name())
                  + " given.");
    ghost_c = std::dynamic_pointer_cast<GhostBoson>(t_ghost);
    setGaugeChoice(choice.getChoice());
}

mty::gauge::Choice VectorBoson::getGaugeChoice() const
{
    return choice;
}

void VectorBoson::setGaugeChoice(gauge::Type t_choice)
{
    HEPAssert(not(mass == CSL_0 and t_choice == mty::gauge::Type::Unitary),
              mty::error::TypeError,
              "Unitary gauge for massless boson is not possible.");
    choice.setGauge(t_choice);
    if (t_choice == mty::gauge::Type::Unitary) {
        if (ghost)
            ghost->setEnabledInDiagrams(false);
        if (ghost_c)
            ghost_c->setEnabledInDiagrams(false);
        if (goldstone)
            goldstone->setEnabledInDiagrams(false);
    }
    else {
        if (ghost) {
            ghost->setEnabledInDiagrams(true);
            ghost->setMass(mass * csl::sqrt_s(getXiGauge()));
        }
        if (ghost_c) {
            ghost_c->setEnabledInDiagrams(true);
            ghost_c->setMass(mass * csl::sqrt_s(getXiGauge()));
        }
        if (goldstone) {
            goldstone->setEnabledInDiagrams(true);
            goldstone->setMass(mass * csl::sqrt_s(getXiGauge()));
        }
    }
}

void VectorBoson::setFieldStrength(Particle const &t_fieldStrength)
{
    if (fieldStrength)
        removeRelative(fieldStrength.get());
    fieldStrength = std::dynamic_pointer_cast<FieldStrength>(t_fieldStrength);
    initPropagator();
}

void VectorBoson::setBrokenParts(const csl::Space *           broken,
                                 const std::vector<Particle> &parts)
{
    std::vector<Particle> fieldStrengthParts(parts.size());
    for (size_t i = 0; i != parts.size(); ++i) {
        fieldStrengthParts[i] = csl::make_shared<FieldStrength>(
            dynamic_cast<VectorBoson *>(parts[i].get()));
        parts[i]->setFieldStrength(fieldStrengthParts[i]);
    }
    fieldStrength->setBrokenParts(broken, fieldStrengthParts);
    QuantumFieldParent::setBrokenParts(broken, parts);
}

csl::Expr VectorBoson::operator()(std::vector<csl::Index> indices,
                                  csl::Tensor const &     t_point)
{
    if (indices.size() == 1 + space.size() and fieldStrength)
        return (*fieldStrength)(indices, t_point);
    return QuantumFieldParent::operator()(indices, t_point);
}

void VectorBoson::breakParticle(mty::Group *                    brokenGroup,
                                std::vector<std::string> const &newNames)
{
    QuantumFieldParent::breakParticle(brokenGroup, newNames);
    const auto vSpace
        = brokenGroup->getVectorSpace(getGroupIrrep(brokenGroup));
    updateBrokenFieldStrength(vSpace);
    if (ghost) {
        std::vector<std::string> ghostNames(newNames.size());
        std::transform(
            newNames.begin(),
            newNames.end(),
            ghostNames.begin(),
            [&](std::string const &vectorName) { return "c_" + vectorName; });
        ghost->breakParticle(brokenGroup, ghostNames);
        updateBrokenGhost(vSpace, ghost);
    }
    if (ghost_c) {
        std::vector<std::string> ghostNames(newNames.size());
        std::transform(newNames.begin(),
                       newNames.end(),
                       ghostNames.begin(),
                       [&](std::string const &vectorName) {
                           return "c_" + vectorName + "c";
                       });
        ghost_c->breakParticle(brokenGroup, ghostNames);
        updateBrokenGhost(vSpace, ghost_c);
    }
    if (goldstone) {
        std::vector<std::string> goldstoneNames(newNames.size());
        std::transform(
            newNames.begin(),
            newNames.end(),
            goldstoneNames.begin(),
            [&](std::string const &vectorName) { return "c_" + vectorName; });
        goldstone->breakParticle(brokenGroup, goldstoneNames);
        updateBrokenGoldstone(vSpace);
    }
}
void VectorBoson::breakParticle(
    mty::FlavorGroup *                     brokenFlavor,
    std::vector<mty::FlavorGroup *> const &subGroups,
    std::vector<std::string> const &       names)
{
    QuantumFieldParent::breakParticle(brokenFlavor, subGroups, names);
    const auto vSpace = brokenFlavor->getFundamentalSpace();
    updateBrokenFieldStrength(vSpace);
    if (ghost) {
        std::vector<std::string> ghostNames(names.size());
        std::transform(
            names.begin(),
            names.end(),
            ghostNames.begin(),
            [&](std::string const &vectorName) { return "c_" + vectorName; });
        ghost->breakParticle(brokenFlavor, subGroups, ghostNames);
        updateBrokenGhost(vSpace, ghost);
    }
    if (ghost_c) {
        std::vector<std::string> ghostNames(names.size());
        std::transform(names.begin(),
                       names.end(),
                       ghostNames.begin(),
                       [&](std::string const &vectorName) {
                           return "c_" + vectorName + "c";
                       });
        ghost_c->breakParticle(brokenFlavor, subGroups, ghostNames);
        updateBrokenGhost(vSpace, ghost_c);
    }
    if (goldstone) {
        std::vector<std::string> goldstoneNames(names.size());
        std::transform(
            names.begin(),
            names.end(),
            goldstoneNames.begin(),
            [&](std::string const &vectorName) { return "c_" + vectorName; });
        goldstone->breakParticle(brokenFlavor, subGroups, goldstoneNames);
        updateBrokenGoldstone(vSpace);
    }
}

void VectorBoson::updateBrokenFieldStrength(csl::Space const *space)
{
    if (!getFieldStrength())
        return;
    const auto &               brokenParts = getBrokenParts(space);
    const size_t               sz          = brokenParts.size();
    std::vector<mty::Particle> brokenFS(sz);
    for (size_t i = 0; i != sz; ++i) {
        brokenFS[i] = ConvertToPtr<QuantumFieldParent>(brokenParts[i])
                          ->getFieldStrength();
    }
    getFieldStrength()->setBrokenParts(space, brokenFS);
}

void VectorBoson::updateBrokenGhost(
    csl::Space const *space, std::shared_ptr<GhostBoson> const &local_ghost)
{
    if (!local_ghost)
        return;
    const auto & brokenParts  = getBrokenParts(space);
    const auto & brokenGhosts = local_ghost->getBrokenParts(space);
    const size_t sz           = brokenParts.size();
    for (size_t i = 0; i != sz; ++i) {
        dynamic_cast<VectorBoson *>(brokenParts[i].get())
            ->setGhostBoson(std::dynamic_pointer_cast<QuantumFieldParent>(
                brokenGhosts[i]));
    }
}

void VectorBoson::updateBrokenGoldstone(csl::Space const *space)
{
    if (!getGoldstoneBoson())
        return;
    const auto & brokenParts      = getBrokenParts(space);
    const auto & brokenGoldstones = goldstone->getBrokenParts(space);
    const size_t sz               = brokenParts.size();
    for (size_t i = 0; i != sz; ++i) {
        dynamic_cast<VectorBoson *>(brokenParts[i].get())
            ->setGoldstoneBoson(std::dynamic_pointer_cast<QuantumFieldParent>(
                brokenGoldstones[i]));
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class GaugeBoson                              //
/*************************************************/
///////////////////////////////////////////////////

GaugeBoson::GaugeBoson(std::string const &t_name,
                       GaugeIrrep const & irrep,
                       GaugedGroup *      group)
    : VectorBoson(t_name, irrep), gaugedGroup(group)
{
    setSelfConjugate(true);
}

GaugeBoson::GaugeBoson(std::string const &t_name, GaugeBoson const *other)
    : VectorBoson(t_name, other), gaugedGroup(other->gaugedGroup)
{
    setSelfConjugate(true);
    setName(t_name);
    initPropagator();
}

void GaugeBoson::printDefinition(std::ostream &out,
                                 int           indentSize,
                                 bool          header) const
{
    if (gaugedGroup->isBroken()) {
        VectorBoson::printDefinition(out, indentSize, header);
        return;
    }
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    out << indent << " = "
        << csl::Abstract::regularName(gaugedGroup->getGroup()->getName())
        << "->buildVectorBoson();\n";
    printQuantumDefinition(out, indentSize);
    if (fieldStrength)
        fieldStrength->printDefinition(out, indentSize, header);
}

GaugedGroup const *GaugeBoson::getGaugedGroup() const
{
    return gaugedGroup;
}
GaugedGroup *GaugeBoson::getGaugedGroup()
{
    return gaugedGroup;
}

Particle GaugeBoson::generateSimilar(std::string const &t_name) const
{
    auto res = csl::make_shared<GaugeBoson>(t_name, this);
    if (ghost) {
        auto newghost = std::make_shared<GhostBoson>("c_" + t_name, res);
        res->setGhostBoson(newghost);
    }
    if (goldstone) {
        auto newgoldstone
            = std::make_shared<GoldstoneBoson>("c_" + t_name, res);
        res->setGoldstoneBoson(newgoldstone);
    }
    return res;
}

bool GaugeBoson::isGaugeBoson() const
{
    return true;
}

///////////////////////////////////////////////////
/*************************************************/
// Class FieldStrength                           //
/*************************************************/
///////////////////////////////////////////////////

FieldStrength::FieldStrength(VectorBoson *t_vectorParent)
    : BaseVectorBoson(*t_vectorParent), vectorParent(t_vectorParent)
{
    relatives.clear();
    setName("F_" + t_vectorParent->getName());
    space.push_back(Lorentz->getVectorSpace(spin));
    // const size_t n = space.size();
    // addAntiSymmetry(n-2, n-1);
}

void FieldStrength::printDefinition(std::ostream &out,
                                    int           indentSize,
                                    bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName("F_" + name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    out << indent << " = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>("
        << csl::Abstract::regularName(name) << ".get()));\n";
}

VectorBoson *FieldStrength::getVectorParent() const
{
    return vectorParent;
}

Particle FieldStrength::generateSimilar(std::string const &t_name) const
{
    Particle newField = csl::make_shared<FieldStrength>(*this);
    newField->setName(t_name);
    return newField;
}

csl::Expr FieldStrength::getEnergyDimension() const
{
    return CSL_2;
}

} // namespace mty
