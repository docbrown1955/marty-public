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

#include "quantumField.h"
#include "amplitude.h"
#include "diracology.h"
#include "fermionicField.h"
#include "mrtError.h"
#include "mrtOptions.h"
#include "mrtUtils.h"
#include "polarization.h"
#include "quantumFieldTheory.h"
#include "vectorField.h"

using namespace std;
using namespace csl;

namespace mty {

Chirality operator!(Chirality init)
{
    switch (init) {
    case Chirality::None:
        return Chirality::None;
    case Chirality::Left:
        return Chirality::Right;
    case Chirality::Right:
        return Chirality::Left;
    }
    return Chirality::None;
}

vector<const Space *> getSpaceRepresentations(Gauge *           gauge,
                                              const GaugeIrrep &irrep)
{
    HEPAssert(irrep.getGauge() == gauge,
              mty::error::ValueError,
              "Taking gauge space rep with a non matching gauge");

    vector<const Space *> res(0);
    for (size_t i = 0; i != gauge->size(); ++i) {
        const Space *space = (*gauge)[i]->getVectorSpace(irrep[i]);
        if (space and space->getDim() > 0)
            res.push_back((*gauge)[i]->getVectorSpace(irrep[i]));
    }
    return res;
}

std::ostream &operator<<(std::ostream &out, ParticleType type)
{
    switch (type) {
    case ParticleType::ScalarBoson:
        out << "ScalarBoson";
        break;
    case ParticleType::VectorBoson:
        out << "VectorBoson";
        break;
    case ParticleType::FieldStrength:
        out << "FieldStrength";
        break;
    case ParticleType::GhostBoson:
        out << "GhostBoson";
        break;
    case ParticleType::GoldstoneBoson:
        out << "GoldstoneBoson";
        break;
    case ParticleType::WeylFermion:
        out << "WeylFermion";
        break;
    case ParticleType::DiracFermion:
        out << "DiracFermion";
        break;
    default:
        out << "Unknown ParticleType (" << int(type) << ")";
    }

    return out;
}

QuantumFieldParent::QuantumFieldParent(const std::string &t_name,
                                       int                t_spin,
                                       Gauge *            t_gauge)
    : QuantumFieldParent(
        t_name, t_spin, t_gauge->getTrivialRep(), FlavorIrrep())
{
}

QuantumFieldParent::QuantumFieldParent(const std::string &t_name,
                                       int                t_spin,
                                       const GaugeIrrep & t_irrep)
    : QuantumFieldParent(t_name, t_spin, t_irrep, FlavorIrrep())
{
}

QuantumFieldParent::QuantumFieldParent(const std::string &t_name,
                                       int                t_spin,
                                       Gauge *            t_gauge,
                                       bool               t_isSelfConjugate)
    : QuantumFieldParent(
        t_name, t_spin, t_gauge->getTrivialRep(), FlavorIrrep())
{
    setSelfConjugate(t_isSelfConjugate);
}

QuantumFieldParent::QuantumFieldParent(const std::string &t_name,
                                       int                t_spin,
                                       const GaugeIrrep & t_irrep,
                                       bool               t_isSelfConjugate)
    : QuantumFieldParent(t_name, t_spin, t_irrep, FlavorIrrep())
{
    setSelfConjugate(t_isSelfConjugate);
}

QuantumFieldParent::QuantumFieldParent(const std::string &t_name,
                                       int                t_spin,
                                       const GaugeIrrep & t_irrep,
                                       const FlavorIrrep &t_flavorRep)
    : TensorFieldParent(t_name, &Minkowski, vector<const Space *>()),
      bosonic(t_spin % 2 == 0),
      mass(CSL_0),
      stGroup(Lorentz),
      gauge(t_irrep.getGauge()),
      irrep(t_irrep),
      flavor(t_flavorRep.getFlavor()),
      flavorRep(t_flavorRep),
      qNumbers(),
      propagator()
{
    valued = false;
    switch (t_spin) {
    case 0:
        spin = Irrep(stGroup->highestWeightRep({0, 0}));
        break;
    case 1:
        spin = Irrep(stGroup->highestWeightRep({1, 0}));
        break;
    case 2:
        spin = Irrep(stGroup->highestWeightRep({1, 1}));
        break;
    default:
        ostringstream sout;
        sout << "Spin " << t_spin << " not yet handled in quantum field";
        CallHEPError(mty::error::ValueError, sout.str());
    }
    setSelfConjugate(false);
    commutable = (abs(t_spin) % 2 == 0);
    initIndexSpaces();
}

QuantumFieldParent::QuantumFieldParent(const string &            t_name,
                                       const QuantumFieldParent *other)
    : QuantumFieldParent(
        t_name, other->getSpinDimension(), other->irrep, other->flavorRep)
{
    setSelfConjugate(other->isSelfConjugate());
}

drawer::ParticleType QuantumFieldParent::getDrawType() const
{
    return drawType;
}

ParticleType QuantumFieldParent::getParticleType() const
{
    CallHEPError(mty::error::NotImplementedError,
                 "Particle type is not defined for " + name);
    return ParticleType::ScalarBoson;
}

void QuantumFieldParent::setDrawType(drawer::ParticleType type)
{
    drawType = type;
    applyToRelatives([&](Particle p) { p->setDrawType(type); });
}

Index generateIndex(string name, const Space *space)
{
    if (name.empty())
        return space->generateIndex();
    return space->generateIndex(name);
}

void QuantumFieldParent::initIndexSpaces()
{
    if (flavor) {
        size_t nonTrivialReps = 0;
        for (size_t i = 0; i != flavor->size(); ++i) {
            const Space *unit_space
                = (*flavor)[i]->getVectorSpace(flavorRep[i]);
            if (unit_space and unit_space->getDim() > 0) {
                space.push_back(unit_space);
                ++nonTrivialReps;
            }
        }
        firstGaugeIndex = nonTrivialReps;
    }
    else
        firstGaugeIndex = 0;
    if (gauge) {
        size_t nonTrivialReps = 0;
        for (size_t i = 0; i != gauge->size(); ++i) {
            const Space *unit_space = (*gauge)[i]->getVectorSpace(irrep[i]);
            if (unit_space and unit_space->getDim() > 0) {
                space.push_back(unit_space);
                ++nonTrivialReps;
            }
        }
        firstSpaceIndex = firstGaugeIndex + nonTrivialReps;
    }
    else
        firstSpaceIndex = firstGaugeIndex;
    if (getSpinDimension() != 1) {
        space.push_back(stGroup->getVectorSpace(spin));
    }
}

void QuantumFieldParent::initPolarizationSum()
{
    if (polarizationSumInitialized)
        return;
    polarizationSumInitialized = true;

    vector<Index> indexA = getFullSetOfIndices();
    vector<Index> indexB = getFullSetOfIndices();

    Index polar = Euclid_R2.generateIndex();

    Tensor    impulsion = tensor_s("p", {&Minkowski});
    csl::Expr epsA      = (*this)(polar, indexA, impulsion);
    csl::Expr epsB = GetComplexConjugate((*this)(polar, indexB, impulsion));
    addSelfContraction(epsA, epsB, CSL_UNDEF);
}

bool QuantumFieldParent::isBosonic() const
{
    return bosonic;
}

bool QuantumFieldParent::isFermionic() const
{
    return not bosonic;
}

bool QuantumFieldParent::isAntiCommuting() const
{
    return isFermionic();
}

bool QuantumFieldParent::isChiral() const
{
    return false;
}

bool QuantumFieldParent::isPhysical() const
{
    return physical;
}

bool QuantumFieldParent::isEnabledInDiagrams() const
{
    return enabledInDiagrams;
}

Particle QuantumFieldParent::getChiralityCounterPart() const
{
    CallHEPError(mty::error::NotImplementedError,
                 "Particle " + std::string(getName()) + " has no chirality.");
    return nullptr;
}

Particle QuantumFieldParent::getWeylFermion(Chirality /*chirality*/) const
{
    return nullptr;
}

Particle QuantumFieldParent::getDiracParent() const
{
    return nullptr;
}

Particle QuantumFieldParent::getGoldstone() const
{
    return nullptr;
}

void QuantumFieldParent::setDiracParent(Particle const & /*diracFermion*/)
{
    CallHEPError(mty::error::NotImplementedError,
                 "Setting Dirac parent for a non weyl-fermion " + name);
}

csl::Expr applyDerivative(QuantumField const &A,
                          QuantumField const &B,
                          csl::Expr const &   prop)
{
    csl::IndexStructure structA = A.getDerivativeStructure();
    csl::IndexStructure structB = B.getDerivativeStructure();
    csl::Expr           res     = Copy(prop);
    if (structA.size() > 0) {
        csl::Tensor X = A.getPoint();
        for (const csl::Index &index : structA)
            res = res->derive(X(index.getFlipped()).get()).value_or(CSL_UNDEF);
    }
    if (structB.size() > 0) {
        csl::Tensor X = B.getPoint();
        for (const csl::Index &index : structB)
            res = res->derive(X(index.getFlipped()).get()).value_or(CSL_UNDEF);
    }

    return res;
}

csl::Expr QuantumFieldParent::getPropagator(QuantumField const &self,
                                            QuantumField const &other,
                                            csl::Tensor &       P,
                                            bool                external) const
{
    auto prop = propagator.find(other.getQuantumParent());
    HEPAssert(prop != propagator.end(),
              mty::error::RuntimeError,
              "Propagator_func not found for particle "
                  + std::string(getName()) + " and "
                  + std::string(other.getName()));

    return applyDerivative(
        self, other, prop->second(self, other, P, external));
}

csl::Expr QuantumFieldParent::getInvPropagator(QuantumField const &other,
                                               QuantumField const &self,
                                               csl::Tensor &       P,
                                               bool external) const
{
    auto prop = propagator.find(other.getQuantumParent());
    HEPAssert(prop != propagator.end(),
              mty::error::RuntimeError,
              "Propagator_func not found for particle "
                  + std::string(getName()) + " and "
                  + std::string(other.getName()));

    return applyDerivative(
        other, self, prop->second(other, self, P, external));
}

csl::Expr QuantumFieldParent::getKineticTerm(csl::Tensor & /*X*/)
{
    CallHEPError(mty::error::NotImplementedError, "");
    return CSL_UNDEF;
}

bool QuantumFieldParent::isGaugeBoson() const
{
    return false;
}

csl::Expr QuantumFieldParent::getXiGauge() const
{
    CallHEPError(mty::error::NotImplementedError,
                 "Getting Xi gauge for non gauge boson "
                     + std::string(getName()));
    return CSL_UNDEF;
}

bool QuantumFieldParent::hasFieldStrength() const
{
    return false;
}

Particle QuantumFieldParent::getFieldStrength() const
{
    return nullptr;
}

void QuantumFieldParent::setFieldStrength(Particle const & /*field*/)
{
    CallHEPError(mty::error::NotImplementedError,
                 "Field " + std::string(getName())
                     + " has no field strength.");
}

bool QuantumFieldParent::isContractibleWith(
    QuantumFieldParent const *other) const
{
    return this == other or propagator.find(other) != propagator.end();
}

Particle QuantumFieldParent::getVectorBoson() const
{
    return nullptr;
}

GaugedGroup const *QuantumFieldParent::getGaugedGroup() const
{
    CallHEPError(mty::error::NotImplementedError,
                 "Getting gauge boson not defined for "
                     + std::string(getName()));
    return nullptr;
}

GaugedGroup *QuantumFieldParent::getGaugedGroup()
{
    CallHEPError(mty::error::NotImplementedError,
                 "Getting gauge boson not defined for "
                     + std::string(getName()));
    return nullptr;
}

Particle QuantumFieldParent::getGoldstoneBoson() const
{
    return nullptr;
}

Particle QuantumFieldParent::getGhostBoson() const
{
    return nullptr;
}

Particle QuantumFieldParent::getConjugatedGhostBoson() const
{
    return nullptr;
}

void QuantumFieldParent::setGoldstoneBoson(Particle const & /*goldstone*/)
{
    CallHEPError(mty::error::NotImplementedError,
                 "Setting goldstone boson not defined for "
                     + std::string(getName()));
}

void QuantumFieldParent::setGhostBoson(Particle const & /*goldstone*/)
{
    CallHEPError(mty::error::NotImplementedError,
                 "Setting ghost fermion not defined for "
                     + std::string(getName()));
}

void QuantumFieldParent::setConjugatedGhostBoson(
    Particle const & /*goldstone*/)
{
    CallHEPError(mty::error::NotImplementedError,
                 "Setting ghost fermion not defined for "
                     + std::string(getName()));
}

void QuantumFieldParent::printQuantumDefinition(std::ostream &out,
                                                int           indentSize) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    if (isSelfConjugate())
        out << indent << regName << "->setSelfConjugate(true);\n";
    if (mass != CSL_0)
        out << indent << regName << "->setMass("
            << csl::Abstract::regularName(mass->getName()) << ");\n";
    if (width != CSL_0)
        out << indent << regName << "->setWidth("
            << csl::Abstract::regularName(width->getName()) << ");\n";
    for (size_t i = 0; i != gauge->size(); ++i) {
        mty::Irrep const &rep = irrep[i];
        if (rep == (*gauge)[i]->getTrivialRep()
            or gauge->getGaugedGroup(i)->isBroken())
            continue;
        out << indent << regName << "->setGroupRep(";
        out << "\"" << csl::Abstract::regularLiteral((*gauge)[i]->getName())
            << "\", {";
        mty::AlgebraState highestWeight = rep.getHighestWeight();
        for (size_t i = 0; i != highestWeight.size(); ++i) {
            out << highestWeight[i];
            if (i + 1 != highestWeight.size())
                out << ", ";
        }
        out << "});\n";
    }
    if (flavor) {
        for (size_t i = 0; i != flavor->size(); ++i) {
            mty::Irrep const &rep = flavorRep[i];
            if (rep == (*flavor)[i]->getTrivialRep())
                continue;
            out << indent << regName << "->setFlavorRep(";
            out << csl::Abstract::regularName((*flavor)[i]->getName()) << ", ";
            out << csl::Abstract::regularName((*flavor)[i]->getName())
                << "->getFundamentalRep());\n";
        }
    }
    out << indent << "_model_.addParticle("
        << csl::Abstract::regularName(getName()) << ", false);\n";
}

void QuantumFieldParent::initPropagator()
{
    CallHEPError(mty::error::NotImplementedError,
                 "Propagator_func not implemented for "
                     + std::string(getName()));
}

bool QuantumFieldParent::isSelfConjugate() const
{
    return selfConjugate;
}

bool QuantumFieldParent::isReal() const
{
    return isBosonic() and isSelfConjugate();
}

bool QuantumFieldParent::isIntegratedOut() const
{
    return integratedOut;
}

Chirality QuantumFieldParent::getChirality() const
{
    return Chirality::None;
}

int QuantumFieldParent::getSpinDimension() const
{
    if (isFermionic())
        return 2;
    else
        return spin.getDim() == 1 ? 1 : 3;
}

csl::Expr QuantumFieldParent::getMass() const
{
    return mass;
}

csl::Expr QuantumFieldParent::getSquaredMass() const
{
    return csl::pow_s(mass, 2);
}

csl::Expr QuantumFieldParent::getWidth() const
{
    return width;
}

mty::gauge::Choice QuantumFieldParent::getGaugeChoice() const
{
    CallHEPError(mty::error::NotImplementedError, "");
    return {};
}

csl::Expr QuantumFieldParent::getEnergyDimension() const
{
    if (isBosonic())
        return CSL_1;
    return 3 * CSL_HALF;
}

int QuantumFieldParent::getNGaugeDegreesOfFreedom() const
{
    int nDof = 1;
    for (const auto &s : space)
        if (s != &csl::Minkowski && s != &mty::dirac4) {
            nDof *= s->getDim();
        }
    return nDof;
}

bool QuantumFieldParent::hasSpecialContractionWith(
    const QuantumField &other) const
{
    return propagator.find(other.getQuantumParent()) != propagator.end();
}

Irrep QuantumFieldParent::getLorentzRep() const
{
    return spin;
}

Gauge *QuantumFieldParent::getGauge() const
{
    return gauge;
}

GaugeIrrep QuantumFieldParent::getGaugeIrrep() const
{
    return irrep;
}

Irrep QuantumFieldParent::getGroupIrrep(const Group *group) const
{
    for (size_t i = 0; i != gauge->size(); ++i) {
        if ((*gauge)[i] == group)
            return irrep[i];
    }
    for (size_t i = 0; i != flavor->size(); ++i) {
        if ((*flavor)[i]->getGroup() == group)
            return flavorRep[i];
    }
    if (group == stGroup)
        return spin;

    CallHEPError(mty::error::ValueError,
                 "Taking rep of a group that does not fit in the gauge of \
            the particle");
    return Irrep();
}

Flavor const *QuantumFieldParent::getFlavor() const
{
    return flavor;
}

void QuantumFieldParent::setFlavor(Flavor const *t_flavor)
{
    flavor = t_flavor;
    if (flavor)
        flavorRep = flavor->getTrivialRep();
    applyToRelatives([&](Particle p) { p->setFlavor(t_flavor); });
}

FlavorIrrep QuantumFieldParent::getFlavorIrrep() const
{
    return flavorRep;
}

Irrep QuantumFieldParent::getFlavorIrrep(const FlavorGroup *group) const
{
    if (!flavor)
        return Irrep();
    for (size_t i = 0; i != flavor->size(); ++i) {
        if ((*flavor)[i] == group)
            return flavorRep[i];
    }
    CallHEPError(
        mty::error::ValueError,
        "Taking rep of a flavor group that does not fit in the flavor of \
            the particle");
    return Irrep();
}

vector<Index>
QuantumFieldParent::getSpaceIndex(const string & /*nameIndex*/) const
{
    if (getSpinDimension() == 1)
        return vector<Index>();
    vector<Index> spaceIndices(space.size() - firstSpaceIndex);
    for (size_t i = 0; i != spaceIndices.size(); ++i)
        spaceIndices[i] = space[i + firstSpaceIndex]->generateIndex();

    return spaceIndices;
}

vector<Index> QuantumFieldParent::getGaugeIndex(vector<string> nameIndex) const
{
    const size_t size = firstSpaceIndex - firstGaugeIndex;
    if (nameIndex.empty()) {
        nameIndex = vector<string>(size);
        for (size_t i = 0; i != nameIndex.size(); ++i)
            nameIndex[i] = space[i + firstGaugeIndex]->getNextIndexName();
    }
    HEPAssert(nameIndex.size() == size,
              mty::error::ValueError,
              "Wrong number of indices passed to "
                  + (string) "QuantumFieldParent::getGaugeIndex()");
    vector<Index> indices;
    indices.reserve(size);

    for (size_t i = 0; i != size; ++i)
        indices.push_back(
            generateIndex(nameIndex[i], space[i + firstGaugeIndex]));

    return indices;
}

vector<Index>
QuantumFieldParent::getFlavorIndex(vector<string> nameIndex) const
{
    const size_t size = firstGaugeIndex;
    if (nameIndex.empty()) {
        nameIndex = vector<string>(size);
        for (size_t i = 0; i != size; ++i)
            nameIndex[i] = space[i]->getNextIndexName();
    }
    HEPAssert(nameIndex.size() == size,
              mty::error::ValueError,
              "Wrong number of indices passed to "
                  + (string) "QuantumFieldParent::getFlavorIndex()");
    vector<Index> indices;
    indices.reserve(size);

    for (size_t i = 0; i != size; ++i)
        indices.push_back(generateIndex(nameIndex[i], space[i]));

    return indices;
}

vector<Index> QuantumFieldParent::getFullSetOfIndices() const
{
    vector<Index> indices(space.size());
    for (size_t i = 0; i != indices.size(); ++i)
        indices[i] = space[i]->generateIndex();

    return indices;
}

QuantumNumber::Value
QuantumFieldParent::getQuantumNumber(QuantumNumber const *number) const
{
    return qNumbers[number->getID()];
}

QuantumNumber::Value
QuantumFieldParent::getQuantumNumber(QuantumField const * instance,
                                     QuantumNumber const *number) const
{
    if (instance->isComplexConjugate())
        return -qNumbers[number->getID()];
    return qNumbers[number->getID()];
}

csl::Expr QuantumFieldParent::getInstance(Tensor point)
{
    vector<Index> indices = getFullSetOfIndices();
    return quantumfield_s(point, self(), indices);
}

void QuantumFieldParent::integrateOut(bool value)
{
    integratedOut = value;
    initPropagator();
}

void QuantumFieldParent::setMass(const std::string &t_mass)
{
    mass = constant_s(t_mass);
    applyToRelatives([&](Particle p) { p->setMass(mass); });
}

void QuantumFieldParent::setMass(const std::string &t_mass, long double value)
{
    mass = constant_s(t_mass, value);
    applyToRelatives([&](Particle p) { p->setMass(mass); });
}

void QuantumFieldParent::setMass(const csl::Expr &t_mass)
{
    mass = t_mass;
    applyToRelatives([&](Particle p) { p->setMass(mass); });
}

void QuantumFieldParent::setWidth(csl::Expr const &t_width)
{
    width = t_width;
    applyToRelatives([&](Particle p) { p->setWidth(width); });
}

void QuantumFieldParent::setGaugeChoice(gauge::Type /*type*/)
{
    CallHEPError(mty::error::NotImplementedError, "");
}

void QuantumFieldParent::setSelfConjugate(bool t_selfConjugate)
{
    selfConjugate = t_selfConjugate;
    complexProp   = (selfConjugate and isBosonic())
                      ? csl::ComplexProperty::Real
                      : csl::ComplexProperty::Complex;
    applyToRelatives(
        [&](Particle p) { p->setSelfConjugate(t_selfConjugate); });
}

void QuantumFieldParent::setPhysical(bool t_physical)
{
    physical = t_physical;
    applyToRelatives([&](Particle p) { p->setPhysical(t_physical); });
}

void QuantumFieldParent::setEnabledInDiagrams(bool t_enabled)
{
    enabledInDiagrams = t_enabled;
    applyToRelatives([&](Particle p) { p->setEnabledInDiagrams(t_enabled); });
}

void QuantumFieldParent::adjustFlavorRep(Flavor *t_flavor)
{
    flavor    = t_flavor;
    flavorRep = FlavorIrrep(t_flavor, flavorRep);
    applyToRelatives([&](Particle p) { p->adjustFlavorRep(t_flavor); });
}

void QuantumFieldParent::setGaugeIrrep(GaugeIrrep const &newRep)
{
    HEPAssert(newRep.size() == gauge->size(),
              mty::error::ValueError,
              "Wrong number of group for " + toString(newRep) + " in gauge.");
    applyToRelatives([&](Particle p) { p->setGaugeIrrep(newRep); });
    for (size_t i = 0; i != newRep.size(); ++i)
        setGroupRep((*gauge)[i], newRep[i]);
}

void QuantumFieldParent::setGroupRep(const std::string &     group,
                                     std::vector<int> const &highestWeight)
{
    HEPAssert(gauge,
              mty::error::RuntimeError,
              "You must set a gauge for field " + std::string(getName())
                  + " before setting its representation.");
    applyToRelatives(
        [&](Particle p) { p->setGroupRep(group, highestWeight); });
    for (size_t i = 0; i != gauge->size(); ++i) {
        if ((*gauge)[i]->getName() == group) {
            Irrep rep = (*gauge)[i]->highestWeightRep(highestWeight);
            setGroupRep((*gauge)[i], rep);
            return;
        }
    }

    CallHEPError(mty::error::RuntimeError,
                 "Group \"" + group + "\" not found in gauge.");
}

void QuantumFieldParent::setGroupRep(std::string const &group, int charge)
{
    applyToRelatives([&](Particle p) { p->setGroupRep(group, charge); });
    setGroupRep(group, std::vector<int>(1, charge));
}

void QuantumFieldParent::setGroupRep(Group *group, const Irrep &newRep)
{
    applyToRelatives([&](Particle p) { p->setGroupRep(group, newRep); });
    symmetry.clear();
    for (size_t i = 0; i != gauge->size(); ++i) {
        Group *g = (*gauge)[i];
        if (g == group) {
            if (newRep.getDim() <= 1 and irrep[i].getDim() > 1) {
                for (size_t j = 0; j != space.size(); ++j)
                    if (space[j] == group->getVectorSpace(irrep[i])) {
                        space.erase(space.begin() + j);
                        --firstSpaceIndex;
                        break;
                    }
            }
            else if (newRep.getDim() > 1 and irrep[i].getDim() <= 1) {
                size_t nonTrivialRep = 0;
                for (size_t j = 0; j != i; ++j)
                    if (irrep[j].getDim() > 1)
                        ++nonTrivialRep;
                space.insert(space.begin() + nonTrivialRep + firstGaugeIndex,
                             group->getVectorSpace(newRep));
                ++firstSpaceIndex;
            }
            irrep[i] = newRep;
            return;
        }
    }
    CallHEPError(mty::error::RuntimeError,
                 "Flavor group " + string(group->getName()) + " not found in "
                     + "QuantumFieldParent::setFlavorRep()");
}

void QuantumFieldParent::setFundamentalFlavorRep(
    std::string const &flavorGroup)
{
    applyToRelatives(
        [&](Particle p) { p->setFundamentalFlavorRep(flavorGroup); });
    for (const auto &group : *flavor) {
        if (group->getName() == flavorGroup) {
            setFlavorRep(group, group->getFundamentalRep());
            return;
        }
    }
    CallHEPError(mty::error::KeyError,
                 "Group " + flavorGroup + " not found in model.")
}
void QuantumFieldParent::setFlavorRep(const FlavorGroup *group,
                                      const Irrep &      newRep)
{
    applyToRelatives([&](Particle p) { p->setFlavorRep(group, newRep); });
    symmetry.clear();
    for (size_t i = 0; i != flavor->size(); ++i) {
        FlavorGroup *flavGroup = (*flavor)[i];
        if (flavGroup == group) {
            if (newRep.getDim() <= 1 and flavorRep[i].getDim() > 1) {
                for (size_t j = 0; j != space.size(); ++j)
                    if (space[j] == group->getVectorSpace(flavorRep[i])) {
                        space.erase(space.begin() + j);
                        --firstGaugeIndex;
                        --firstSpaceIndex;
                        break;
                    }
            }
            else if (newRep.getDim() > 1 and flavorRep[i].getDim() <= 1) {
                size_t nonTrivialFlavor = 0;
                for (size_t j = 0; j != i; ++j)
                    if (flavorRep[j].getDim() > 1)
                        ++nonTrivialFlavor;
                space.insert(space.begin() + nonTrivialFlavor,
                             group->getFundamentalSpace());
                ++firstGaugeIndex;
                ++firstSpaceIndex;
            }
            flavorRep[i] = newRep;
            return;
        }
    }
    CallHEPError(mty::error::RuntimeError,
                 "Flavor group " + string(group->getName()) + " not found in "
                     + "QuantumFieldParent::setFlavorRep()");
}

void QuantumFieldParent::addQuantumNumber(QuantumNumber const &number,
                                          QuantumNumber::Value value)
{
    applyToRelatives([&](Particle p) { p->addQuantumNumber(number, value); });
    qNumbers[number.getID()] = value;
}

void QuantumFieldParent::setPropagator(QuantumFieldParent const *other,
                                       Propagator_func           prop)
{
    propagator[other] = prop;
}

void QuantumFieldParent::setBrokenParts(const Space *           broken,
                                        const vector<Particle> &parts)
{
    brokenParts[broken] = vector<Parent>(parts.size());
    for (size_t i = 0; i != parts.size(); ++i)
        brokenParts[broken][i] = parts[i];
}

std::vector<csl::Parent>
QuantumFieldParent::breakSpace(const csl::Space *                     broken,
                               const std::vector<const csl::Space *> &newSpace,
                               const std::vector<size_t> &pieces) const
{
    return TensorFieldParent::breakSpace(broken, newSpace, pieces);
}

ostream &operator<<(ostream &fout, const QuantumFieldParent &obj)
{
    if (obj.bosonic)
        fout << "Bosonic ";
    else
        fout << "Fermionic ";
    IntFraction foo(obj.getSpinDimension() - 1, 2);
    fout << "spin " << foo.getNum();
    if (foo.getDenom() != 1)
        fout << "/" << foo.getDenom();
    fout << endl;

    fout << "Gauge irrep: " << obj.irrep << endl;

    return fout;
}

csl::Expr QuantumFieldParent::operator()(const Tensor &t_point)
{
    checkIndicialAndFieldRequest(t_point.get());
    return quantumfield_s(t_point, self());
}

csl::Expr QuantumFieldParent::operator()(Index t_index, const Tensor &t_point)
{
    createFixedIndices(t_index);
    checkIndicialAndFieldRequest(t_index, t_point.get());
    csl::Expr res = quantumfield_s(t_point, self(), vector<Index>(1, t_index));
    return res;
}

csl::Expr QuantumFieldParent::operator()(vector<int> const &indices,
                                         const Tensor &     t_point)
{
    return (*this)(integerToIndices(indices), t_point);
}

csl::Expr QuantumFieldParent::operator()(vector<Index> indices,
                                         const Tensor &t_point)
{
    createFixedIndices(indices);
    checkIndicialAndFieldRequest(indices, t_point.get());
    csl::Expr res = quantumfield_s(t_point, self(), indices);
    return res;
}

csl::Expr QuantumFieldParent::operator()(Index              polarization,
                                         const vector<int> &indices,
                                         const Tensor &     t_point)
{
    return (*this)(polarization, integerToIndices(indices), t_point);
}

csl::Expr QuantumFieldParent::operator()()
{
    return operator()(defaultSpaceTimePoint);
}

csl::Expr QuantumFieldParent::operator()(Index t_index)
{
    return operator()(t_index, defaultSpaceTimePoint);
}

csl::Expr QuantumFieldParent::operator()(vector<Index> indices)
{
    return operator()(indices, defaultSpaceTimePoint);
}

csl::Expr QuantumFieldParent::operator()(Index         polarization,
                                         Index         index,
                                         const Tensor &impulsion)
{
    if (not mty::option::keepExternScalarLeg and getSpinDimension() == 1)
        return CSL_1;
    initPolarizationSum();
    createFixedIndices(index);
    createFixedIndices(polarization);
    checkIndicialAndFieldRequest(index, impulsion.get());
    return csl::make_shared<PolarizationField>(
        impulsion, self(), vector<Index>({polarization, index}));
}

csl::Expr QuantumFieldParent::operator()(Index         polarization,
                                         vector<Index> indices,
                                         const Tensor &impulsion)
{
    if (not mty::option::keepExternScalarLeg and getSpinDimension() == 1)
        return CSL_1;
    initPolarizationSum();
    createFixedIndices(polarization);
    createFixedIndices(indices);
    checkIndicialAndFieldRequest(indices, impulsion.get());
    indices.insert(indices.begin(), polarization);
    return csl::make_shared<PolarizationField>(impulsion, self(), indices);
}

QuantumFieldParent::operator csl::Expr()
{
    HEPAssert(dynamic_cast<ScalarBoson const *>(this),
              mty::error::TypeError,
              "You may not convert a " + toString(getParticleType()) + " ("
                  + toString(name)
                  + ") into an expression without giving "
                    "indices. Please use operator().");
    HEPAssert(space.empty(),
              mty::error::TypeError,
              "You may not convert a non trivial representation "
              " (" + toString(name)
                  + ") into an expression without giving "
                    "indices. Please use operator(). Representation is "
                  + toString(getGaugeIrrep()) + ".");
    return operator()();
}

void QuantumFieldParent::addRelative(
    std::weak_ptr<mty::QuantumFieldParent> const &rel)
{
    const auto pos
        = std::find_if(relatives.begin(),
                       relatives.end(),
                       [&](std::weak_ptr<mty::QuantumFieldParent> const &ptr) {
                           return ptr.lock() == rel.lock();
                       });
    if (pos == relatives.end() && rel.lock().get() != this)
        relatives.push_back(rel);
}

std::weak_ptr<mty::QuantumFieldParent>
QuantumFieldParent::removeRelative(QuantumFieldParent const *parent)
{
    const auto pos = std::find_if(
        relatives.begin(),
        relatives.end(),
        [&](std::weak_ptr<mty::QuantumFieldParent> const &particle) {
            return particle.lock().get() == parent;
        });
    if (pos != relatives.end()) {
        auto particle = *pos;
        relatives.erase(pos);
        return particle;
    }
    return {};
}

void QuantumFieldParent::checkRelatives()
{
    for (size_t i = 0; i != relatives.size(); ++i) {
        if (!relatives[i].lock()) {
            relatives.erase(relatives.begin() + i);
            --i;
        }
    }
}

void QuantumFieldParent::breakParticle(
    mty::Group *brokenGroup, std::vector<std::string> const &newNames)
{
    applyToRelatives(
        [&](Particle p) { p->breakParticle(brokenGroup, newNames); });
    if (!brokenParts.empty())
        return;
    std::vector<mty::Particle> newParticles;
    csl::Space const *         repSpace
        = brokenGroup->getVectorSpace(getGroupIrrep(brokenGroup));
    size_t nParts = repSpace->getDim();
    newParticles.reserve(nParts);
    for (size_t i = 0; i != nParts; ++i) {
        Particle newPart = generateSimilar(newNames[i]);
        newPart->setGroupRep(brokenGroup, brokenGroup->getTrivialRep());
        newParticles.push_back(newPart);
    }
    csl::Space const *vectorSpace
        = brokenGroup->getVectorSpace(getGroupIrrep(brokenGroup));
    setBrokenParts(vectorSpace, newParticles);
}

void QuantumFieldParent::breakParticle(
    mty::FlavorGroup *                     brokenFlavor,
    std::vector<mty::FlavorGroup *> const &subGroups,
    std::vector<std::string> const &       names)
{
    applyToRelatives(
        [&](Particle p) { p->breakParticle(brokenFlavor, subGroups, names); });
    if (!brokenParts.empty())
        return;
    std::vector<mty::Particle> newParticles;
    newParticles.reserve(subGroups.size());
    for (size_t i = 0; i != subGroups.size(); ++i) {
        Particle newPart = generateSimilar(names[i]);
        newPart->setFlavorRep(brokenFlavor,
                              brokenFlavor->getGroup()->getTrivialRep());
        if (subGroups[i]) {
            newPart->setFlavorRep(subGroups[i],
                                  subGroups[i]->getFundamentalRep());
        }
        newParticles.push_back(newPart);
    }
    setBrokenParts(brokenFlavor->getFundamentalSpace(), newParticles);
}

///////////////////////////////////////////////////
/*************************************************/
// Class QuantumField                            //
/*************************************************/
///////////////////////////////////////////////////

QuantumField::QuantumField() : QuantumField(nullptr, nullptr)
{
}

QuantumField::QuantumField(const Tensor &t_vector, const Parent &t_parent)
    : TensorFieldElement(t_vector, t_parent), partnerShip()
{
    if (parent) {
        Complexified::setComplexProperty(parent->getComplexProp());
        setIncoming(false);
        setParticle(true);
    }
}

QuantumField::QuantumField(const Tensor &            t_vector,
                           const Parent &            t_parent,
                           const std::vector<Index> &indices)
    : TensorFieldElement(t_vector, t_parent, indices), partnerShip()
{
    if (parent) {
        Complexified::setComplexProperty(parent->getComplexProp());
        setIncoming(false);
        setParticle(true);
    }
}

QuantumField::QuantumField(const Tensor &        t_vector,
                           const Parent &        t_parent,
                           const IndexStructure &indices)
    : TensorFieldElement(t_vector, t_parent, indices), partnerShip()
{
    if (parent) {
        Complexified::setComplexProperty(parent->getComplexProp());
        setIncoming(false);
        setParticle(true);
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Functions specific to QuantumField            //
/*************************************************/
///////////////////////////////////////////////////

int QuantumField::getSpinDimension() const
{
    return getQuantumParent()->getSpinDimension();
}

csl::Expr QuantumField::getEnergyDimension() const
{
    return getQuantumParent()->getEnergyDimension();
}

csl::Expr QuantumField::getMass() const
{
    return getQuantumParent()->getMass();
}

csl::Expr QuantumField::getWidth() const
{
    return getQuantumParent()->getWidth();
}

bool QuantumField::isChiral() const
{
    return getQuantumParent()->isChiral();
}

Chirality QuantumField::getChirality() const
{
    return getQuantumParent()->getChirality();
}

void QuantumField::setChirality(Chirality chirality)
{
    HEPAssert(not isBosonic(),
              mty::error::TypeError,
              "Setting chirality of a non fermionic field.");
    if (chirality == getChirality())
        return;
    auto const &pointed = *parent;
    if (typeid(pointed) == typeid(WeylFermion)) {
        if (chirality == Chirality::None)
            parent = getQuantumParent()->getDiracParent();
        else
            parent = getQuantumParent()->getChiralityCounterPart();
    }
    else if (typeid(pointed) == typeid(DiracFermion)) {
        parent = getQuantumParent()->getWeylFermion(chirality);
    }
    else {
        CallHEPError(mty::error::NotImplementedError,
                     "Change of chirality for particle " + toString(getName())
                         + " not allowed.");
    }
}

bool QuantumField::isGaugeBoson() const
{
    return getQuantumParent()->isGaugeBoson();
}

csl::Expr QuantumField::getXiGauge() const
{
    return getQuantumParent()->getXiGauge();
}

csl::Expr QuantumField::getSquaredMass() const
{
    return getQuantumParent()->getSquaredMass();
}

bool QuantumField::getConjugated() const
{
    return Complexified::conjugated;
}

bool QuantumField::isExternal() const
{
    return external;
}

bool QuantumField::isOnShell() const
{
    return onShell;
}

bool QuantumField::isBosonic() const
{
    return getQuantumParent()->isBosonic();
}

bool QuantumField::isFermionic() const
{
    return getQuantumParent()->isFermionic();
}

bool QuantumField::isAntiCommuting() const
{
    return getQuantumParent()->isAntiCommuting();
}

bool QuantumField::isSelfConjugate() const
{
    return getQuantumParent()->isSelfConjugate();
}

bool QuantumField::isReal() const
{
    return getQuantumParent()->isReal();
}

bool QuantumField::isPhysical() const
{
    return getQuantumParent()->isPhysical();
}

bool QuantumField::isContractibleWith(const QuantumField &other) const
{
    // Two fields from the same particle
    return (getQuantumParent()->isContractibleWith(other.getQuantumParent()));
}

bool QuantumField::isExactlyContractiblewith(const QuantumField &other) const
{
    return (isContractibleWith(other)
            and ((isSelfConjugate() && !IsOfType<WeylFermion>(parent))
                 or (conjugated != other.conjugated)));
}

void QuantumField::addDerivative(const Index &index)
{
    derivativeIndices += index;
}

bool QuantumField::hasDerivative() const
{
    auto const &pointed = *parent;
    return derivativeIndices.size() != 0
           or typeid(FieldStrength) == typeid(pointed);
}

csl::IndexStructure QuantumField::getDerivativeStructure() const
{
    return derivativeIndices;
}

csl::Expr QuantumField::getPropagator(const QuantumField &other,
                                      Tensor &            vertex) const
{
    if (not isExactlyContractiblewith(other))
        return CSL_0;
    if (!isSelfConjugate() and conjugated and !other.conjugated) {
        // Always contract phi.phi_star is this order (particle ->
        // antiparticle) If fermionic, factor of (-1)
        if (isBosonic() and isAntiCommuting()) {
            // Minus sign for fermions is taken care of by ConjugationInfo,
            // Here we consider only signs for ghosts.
            return -getQuantumParent()->getInvPropagator(
                other, *this, vertex, isExternal() or other.isExternal());
        }
    }
    return getQuantumParent()->getPropagator(
        *this, other, vertex, isExternal() or other.isExternal());
}

csl::Expr
QuantumField::getLSZInsertion(const Tensor &P, bool ruleMode, bool lock) const
{
    if (getQuantumParent()->isIntegratedOut())
        return CSL_1;
    return ExternalLeg(*this, P, ruleMode, lock);
}

QuantumField QuantumField::getConjugatedField() const
{
    QuantumField res(*this);
    res.conjugate();
    return res;
}

optional<csl::Expr> QuantumField::getComplexConjugate() const
{
    std::optional<csl::Expr> opt_cc = parent->getComplexProperty(this);
    if (opt_cc)
        return opt_cc;
    return quantumfield_s(getConjugatedField());
}

void QuantumField::conjugate()
{
    if (getQuantumParent()->isSelfConjugate() and isBosonic())
        return;
    particle = not particle;
    setConjugated(not conjugated);
}

void QuantumField::setExternal(bool t_external)
{
    HEPAssert(not t_external or getQuantumParent()->isPhysical(),
              mty::error::TypeError,
              "Particle " + toString(copy())
                  + " not physical. Cannot appear "
                    "in external states.");
    external = t_external;
}

void QuantumField::setOnShell(bool t_onShell)
{
    HEPAssert(not t_onShell or getQuantumParent()->isPhysical(),
              mty::error::TypeError,
              "Particle " + toString(copy())
                  + " not physical. Cannot be "
                    "on shell.");
    HEPAssert(not t_onShell or external,
              mty::error::TypeError,
              "Particle " + toString(copy())
                  + " not external leg. Cannot be "
                    "on shell.");
    onShell = t_onShell;
}

void QuantumField::setIncoming(bool t_incoming)
{
    incoming = t_incoming;
    setConjugated(isIncomingParticle() or isOutgoingAntiParticle());
}

void QuantumField::setParticle(bool t_particle)
{
    if (getQuantumParent()->isSelfConjugate() and isBosonic())
        return;
    particle = t_particle;
    setConjugated(isIncomingParticle() or isOutgoingAntiParticle());
}

void QuantumField::setDerivativeStructure(const IndexStructure &structure)
{
    derivativeIndices = structure;
}

///////////////////////////////////////////////////
/*************************************************/
// Functions derived from csl classes          //
/*************************************************/
///////////////////////////////////////////////////

bool QuantumField::getCommutable() const
{
    return not isAntiCommuting();
}

bool QuantumField::commutesWith(csl::Expr_info other, int sign) const
{
    if (other->getCommutable())
        return sign == -1;
    if (not isAntiCommuting())
        return (sign == -1);
    if (other->getType() == csl::Type::TensorFieldElement) {
        if (sign == 1)
            return IsOfType<QuantumField>(other)
                   and ConvertToPtr<QuantumField>(other)->isAntiCommuting();
        return (not IsOfType<QuantumField>(other)
                or not ConvertToPtr<QuantumField>(other)->isAntiCommuting());
    }
    if (sign != -1)
        return false;
    if (other->size() == 0)
        return true;
    for (size_t i = 0; i != other->size(); ++i)
        if (not commutesWith((*other)[i].get()))
            return false;
    return true;
}

bool QuantumField::hasContractionProperty(csl::Expr_info expr) const
{
    if (csl::IsIndicialTensor(expr)
        and expr->getParent_info() == mty::dirac4.C_matrix.get()
        and isSelfConjugate() and IsOfType<DiracFermion>(parent)
        and isFermionic())
        return true;
    return false;
}

csl::Expr QuantumField::contraction(csl::Expr_info expr) const
{
    return matrixChargeConjugation(expr);
}

csl::unique_Expr QuantumField::copy_unique() const
{
    return make_unique<QuantumField>(*this);
}

bool QuantumField::isIndexed() const
{
    return TensorFieldElement::isIndexed() or derivativeIndices.size() > 0;
}

bool QuantumField::compareWithDummy(
    csl::Expr_info                    other,
    std::map<csl::Index, csl::Index> &constraints,
    bool                              keepAllCosntraints) const
{
    QuantumField const *other_info = dynamic_cast<QuantumField const *>(other);
    if (!other_info) {
        return false;
    }
    if (derivativeIndices.size() == 0
        and other_info->derivativeIndices.size() == 0) {
        auto copyMap = constraints;
        bool res     = TensorFieldElement::compareWithDummy(
            other, copyMap, keepAllCosntraints);
        if (res) {
            constraints = copyMap;
            return true;
        }
        return false;
    }
    if (derivativeIndices.size() != other_info->derivativeIndices.size())
        return false;

    std::shared_ptr<QuantumField> fieldA
        = csl::make_shared<QuantumField>(*this);
    std::shared_ptr<QuantumField> fieldB
        = csl::make_shared<QuantumField>(*other_info);
    fieldA->setDerivativeStructure(csl::IndexStructure());
    fieldB->setDerivativeStructure(csl::IndexStructure());
    csl::Expr A = fieldA;
    for (const auto &index : derivativeIndices)
        A = prod_s(partialMinko(index, point), A, true);
    csl::Expr B = fieldB;
    for (const auto &index : other_info->derivativeIndices)
        B = prod_s(partialMinko(index, other_info->point), B, true);
    return A->compareWithDummy(B.get(), constraints, keepAllCosntraints);
}

bool QuantumField::operator==(csl::Expr_info other) const
{
    QuantumField const *other_info = dynamic_cast<const QuantumField *>(other);
    if (!other_info)
        return false;
    if (derivativeIndices.size() == 0
        and other_info->derivativeIndices.size() == 0)
        return TensorFieldElement::operator==(other);
    if (derivativeIndices.size() != other_info->derivativeIndices.size())
        return false;
    if (isFermionic() && particle != other_info->particle)
        return false;

    std::shared_ptr<QuantumField> fieldA
        = csl::make_shared<QuantumField>(*this);
    std::shared_ptr<QuantumField> fieldB
        = csl::make_shared<QuantumField>(*other_info);
    fieldA->setDerivativeStructure(csl::IndexStructure());
    fieldB->setDerivativeStructure(csl::IndexStructure());
    csl::Expr A = fieldA;
    for (const auto &index : derivativeIndices)
        A = prod_s(partialMinko(index, point), A, true);
    csl::Expr B = fieldB;
    for (const auto &index : other_info->derivativeIndices)
        B = prod_s(partialMinko(index, other_info->point), B, true);
    return *A == B.get();
}

bool QuantumField::operator<(const QuantumField &other) const
{
    return TensorFieldElement::operator<(&other);
}

bool QuantumField::operator==(const QuantumField &other) const
{
    return operator==(&other);
}

bool QuantumField::operator!=(const QuantumField &other) const
{
    return not operator==(other);
}

bool QuantumField::isParticle() const
{
    return particle;
}

bool QuantumField::isIncoming() const
{
    return incoming;
}

bool QuantumField::isIncomingParticle() const
{
    return incoming and particle;
}

bool QuantumField::isOutgoingParticle() const
{
    return (not incoming) and particle;
}

bool QuantumField::isIncomingAntiParticle() const
{
    return incoming and (not particle);
}

bool QuantumField::isOutgoingAntiParticle() const
{
    return (not incoming) and (not particle);
}

csl::Expr QuantumField::matrixChargeConjugation(csl::Expr_info other) const
{
    QuantumField newField(*this);
    int          sign = conjugated ? 1 : -1;
    newField.setParticle(!newField.particle);
    csl::Index                 spaceIndex = index.back();
    csl::IndexStructure const &C          = other->getIndexStructureView();
    if (C[0] == spaceIndex) {
        sign *= -1;
        spaceIndex = C[1];
    }
    else
        spaceIndex = C[0];
    newField.index.back() = spaceIndex;

    return int_s(sign) * newField.copy();
}

void AddCustomPropagator(QuantumFieldParent *                left,
                         QuantumFieldParent *                right,
                         QuantumFieldParent::Propagator_func propagator)
{
    left->setPropagator(right, propagator);
    right->setPropagator(left, propagator);
}

void AddCustomPropagator(QuantumFieldParent *                autoProp,
                         QuantumFieldParent::Propagator_func propagator)
{
    autoProp->setPropagator(autoProp, propagator);
}

void projectOnChirality(const QuantumField &field,
                        csl::Expr &         expression,
                        csl::Index          alpha)
{
    if (field.getChirality() == Chirality::None)
        return;
    Index beta = dirac4.generateIndex();
    expression = Replaced(expression, alpha, beta);
    if (field.getChirality() == Chirality::Left)
        expression = dirac4.P_L({alpha, beta}) * expression;
    else
        expression = dirac4.P_R({alpha, beta}) * expression;
}

csl::Expr getMomentumSum(const vector<QuantumField> &insertions,
                         const vector<Tensor> &      impulsions,
                         const Index &               index)
{
    csl::Expr sum = CSL_0;
    for (size_t i = 0; i != insertions.size(); ++i) {
        auto sign = int_s(insertions[i].isIncoming()) * 2 - 1;
        sum       = sum + sign * (*impulsions[i])(index);
    }
    return sum;
}

ostream &operator<<(ostream &fout, const mty::QuantumField &field)
{
    field.print(1, fout);

    return fout;
}

} // End of namespace mty
