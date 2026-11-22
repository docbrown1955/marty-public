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

#include "modeling.h"

#include "../common/error.h"
#include "../common/session.h"
#include "csl/replace.h"
#include "marty/core/amplitude.h"
#include "marty/core/fermionicField.h"
#include "marty/core/ghostField.h"
#include "marty/core/mrtInterface.h"
#include "marty/core/scalarField.h"
#include "marty/models/qed.h"
#include "marty/models/sm.h"

#include <numeric>
#include <set>
#include <sstream>

namespace mty::api::modeling {
namespace {

int gaugePosition(detail::ModelState const &state, std::string const &group)
{
    int const position = guarded([&] {
        Session     session(state.core.get());
        auto const *gauge = state.core->getGauge();
        for (std::size_t pos = 0; pos < gauge->size(); ++pos) {
            if ((*gauge)[static_cast<int>(pos)]->getName() == group)
                return static_cast<int>(pos);
        }
        return -1;
    });
    if (position < 0)
        throw LookupError("no gauge group named \"" + group + "\"");
    return position;
}

mty::group::Type groupKind(detail::ModelState const &state,
                           std::string const        &group)
{
    int const position = gaugePosition(state, group);
    auto      kind     = guarded([&] {
        Session session(state.core.get());
        return (*state.core->getGauge())[position]->getType();
    });
    return kind == mty::group::Type::U1 ? mty::group::Type::U : kind;
}

void checkOpen(detail::ModelState const &state, std::string const &action)
{
    if (state.finalized)
        throw StateError("cannot " + action + ": the model is finalized");
}

std::shared_ptr<const group_theory::Group>
groupHandle(detail::ModelState &state, mty::AbstractGroup *core)
{
    auto &handle = state.groupHandles[core];
    if (!handle) {
        handle = guarded([&] {
            return group_theory::Group::wrap(
                std::shared_ptr<mty::AbstractGroup>(state.core, core));
        });
    }
    return handle;
}

} // namespace

Particle::Particle(std::shared_ptr<detail::ModelState> state,
                   mty::Particle                       particle)
    : m_state(std::move(state)), m_particle(std::move(particle))
{
}

std::string Particle::name() const
{
    return std::string(m_particle->getName());
}

bool Particle::isU1(std::string const &group) const
{
    return groupKind(*m_state, group) == mty::group::Type::U;
}

void Particle::setLabels(std::string const      &group,
                         std::vector<int> const &labels)
{
    checkOpen(*m_state, "set a representation");
    if (isU1(group))
        throw TypeError("group \"" + group
                        + "\" is U(1): set its charge with setCharge()");
    guarded([&] { m_particle->setGroupRep(group, labels); });
}

void Particle::setCharge(std::string const &group, int num, int den)
{
    checkOpen(*m_state, "set a charge");
    if (!isU1(group))
        throw TypeError("group \"" + group
                        + "\" is not U(1): set its representation with "
                          "setLabels()");
    if (den <= 0)
        throw ValueError("the charge denominator must be positive");
    int const              divisor = std::gcd(num, den);
    std::vector<int> const charge{num / divisor, den / divisor};
    guarded([&] { m_particle->setGroupRep(group, charge); });
}

csl::Expr Particle::expr() const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return static_cast<csl::Expr>(*m_particle);
    });
}

csl::Expr Particle::operator()(std::vector<csl::Index> const &indices) const
{
    if (indices.empty())
        return expr();
    return guarded([&] {
        Session session(m_state->core.get());
        return (*m_particle)(indices);
    });
}

Particle Particle::fieldStrength() const
{
    checkOpen(*m_state, "create a field strength");
    auto field = guarded([&] {
        Session session(m_state->core.get());
        return m_particle->getFieldStrength();
    });
    if (!field) {
        throw TypeError("particle \"" + name() + "\" has no field strength");
    }
    return Particle(m_state, field);
}

Particle Particle::createSimilarParticle(std::string const &name) const
{
    checkOpen(*m_state, "create a similar particle");
    auto field = guarded([&] {
        Session session(m_state->core.get());
        return m_particle->generateSimilar(name);
    });
    return Particle(m_state, field);
}

void Particle::setSelfConjugate(bool selfConjugate)
{
    checkOpen(*m_state, "set the self-conjugacy");
    guarded([&] {
        Session session(m_state->core.get());
        m_particle->setSelfConjugate(selfConjugate);
    });
}

bool Particle::isSelfConjugate() const
{
    return guarded([&] { return m_particle->isSelfConjugate(); });
}

Particle Particle::ghostBoson() const
{
    auto ghost = guarded([&] {
        Session session(m_state->core.get());
        return m_particle->getGhostBoson();
    });
    if (!ghost) {
        throw TypeError("particle \"" + name() + "\" has no ghost boson");
    }
    return Particle(m_state, ghost);
}

void Particle::setConjugatedGhostBoson(Particle const &ghost)
{
    checkOpen(*m_state, "set the conjugated ghost boson");
    guarded([&] {
        Session session(m_state->core.get());
        m_particle->setConjugatedGhostBoson(ghost.raw());
    });
}

group_theory::Rational Particle::spin() const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return group_theory::Rational(m_particle->getSpinDimension() - 1, 2);
    });
}

group_theory::Irrep Particle::irrep(std::string const &group) const
{
    int const position = gaugePosition(*m_state, group);
    auto      handle   = groupHandle(*m_state, guarded([&] {
        Session session(m_state->core.get());
        return static_cast<mty::AbstractGroup *>(
            (*m_state->core->getGauge())[position]);
    }));
    return guarded([&] {
        Session session(m_state->core.get());
        return group_theory::Irrep(handle,
                                   m_particle->getGaugeIrrep()[position]);
    });
}

void Particle::setFundamentalFlavorRep(std::string const &flavorGroup)
{
    checkOpen(*m_state, "set a flavor representation");
    guarded([&] {
        Session session(m_state->core.get());
        m_particle->setFundamentalFlavorRep(flavorGroup);
    });
}

csl::Expr Particle::mass() const
{
    return guarded([&] { return m_particle->getMass(); });
}

void Particle::setMass(csl::Expr const &mass)
{
    checkOpen(*m_state, "set the mass");
    guarded([&] {
        Session session(m_state->core.get());
        m_particle->setMass(mass);
    });
}

void Particle::setGaugeChoice(mty::gauge::Type choice)
{
    checkOpen(*m_state, "set the gauge choice");
    guarded([&] {
        Session session(m_state->core.get());
        m_particle->setGaugeChoice(choice);
    });
}

Amplitude::Amplitude(mty::Amplitude amplitude)
    : m_amplitude(std::make_shared<mty::Amplitude const>(std::move(amplitude)))
{
}

std::size_t Amplitude::diagramCount() const
{
    return m_amplitude->getDiagrams().size();
}

Model::Model(std::vector<GaugeGroup> const  &gauge,
             std::vector<FlavorGroup> const &flavor)
    : m_state(std::make_shared<detail::ModelState>())
{
    std::vector<mty::GaugeGroupSpec>  specs;
    std::vector<mty::FlavorGroupSpec> flavorSpecs;
    std::set<std::string>             names;
    for (auto const &item : gauge) {
        if (!item.group)
            throw ValueError("gauge group \"" + item.name + "\" is null");
        if (!names.insert(item.name).second)
            throw ValueError("duplicate gauge group \"" + item.name + "\"");
        mty::GaugeGroupSpec spec;
        spec.type      = item.group->kind();
        spec.dim       = item.group->size();
        spec.name      = item.name;
        spec.bosonName = item.bosonName;
        if (!item.coupling.empty())
            spec.coupling = csl::constant_s(item.coupling);
        specs.push_back(spec);
    }
    for (auto const &item : flavor) {
        if (item.dim < 1)
            throw ValueError("flavor group \"" + item.name
                             + "\" must have at least one flavor");
        if (!names.insert(item.name).second)
            throw ValueError("duplicate group \"" + item.name + "\"");
        flavorSpecs.push_back(
            mty::FlavorGroupSpec{csl::Expr(item.dim), item.name});
    }
    m_state->core = guarded([&] {
        Session session(nullptr);
        return std::make_shared<mty::Model>(std::as_const(specs),
                                            std::as_const(flavorSpecs));
    });
}

Model::Model(std::shared_ptr<detail::ModelState> state)
    : m_state(std::move(state))
{
}

Model Model::standardModel()
{
    auto state  = std::make_shared<detail::ModelState>();
    state->core = guarded([] {
        Session session(nullptr);
        return std::make_shared<SM_Model>();
    });
    state->finalized = true;
    return Model(std::move(state));
}

Model Model::qed()
{
    auto state  = std::make_shared<detail::ModelState>();
    state->core = guarded([] {
        Session session(nullptr);
        return std::make_shared<QED_Model>();
    });
    state->finalized = true;
    return Model(std::move(state));
}

Particle Model::weylFermion(std::string const &name,
                            std::string const &chirality)
{
    checkOpen(*m_state, "add a particle");
    mty::Chirality chir = mty::Chirality::Left;
    if (chirality == "L")
        chir = mty::Chirality::Left;
    else if (chirality == "R")
        chir = mty::Chirality::Right;
    else
        throw ValueError("chirality must be \"L\" or \"R\", got \"" + chirality
                         + "\"");
    auto field = guarded([&] {
        Session session(nullptr);
        return std::make_shared<mty::WeylFermion>(name, *m_state->core, chir);
    });
    return Particle(m_state, mty::Particle(field));
}

Particle Model::diracFermion(std::string const &name)
{
    checkOpen(*m_state, "add a particle");
    auto field = guarded([&] {
        Session session(nullptr);
        return std::make_shared<mty::DiracFermion>(name, *m_state->core);
    });
    return Particle(m_state, mty::Particle(field));
}

Particle Model::majoranaFermion(std::string const &name)
{
    checkOpen(*m_state, "add a particle");
    auto field = guarded([&] {
        Session session(nullptr);
        auto dirac = std::make_shared<mty::DiracFermion>(name, *m_state->core);
        dirac->setSelfConjugate(true);
        return dirac;
    });
    return Particle(m_state, mty::Particle(field));
}

Particle Model::scalarBoson(std::string const &name)
{
    checkOpen(*m_state, "add a particle");
    auto field = guarded([&] {
        Session session(nullptr);
        return std::make_shared<mty::ScalarBoson>(name, *m_state->core);
    });
    return Particle(m_state, mty::Particle(field));
}

void Model::renameParticle(std::string const &name, std::string const &newName)
{
    checkOpen(*m_state, "rename a particle");
    auto part = particle(name);
    if (contains(newName))
        throw ValueError("a particle named \"" + newName
                         + "\" already exists");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->renameParticle(part.raw(), newName);
    });
}

void Model::refresh()
{
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->refresh();
    });
}

void Model::finalize()
{
    if (m_state->finalized)
        return;
    refresh();
    m_state->finalized = true;
}

bool Model::isFinalized() const
{
    return m_state->finalized;
}

void Model::addTensorCoupling(csl::Tensor const &tensor)
{
    checkOpen(*m_state, "add a tensor coupling");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addTensorCoupling(tensor);
    });
}

csl::api::SpaceHandle Model::vectorSpace(std::string const &flavor) const
{
    auto space = guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->getVectorSpace(flavor);
    });
    if (!space)
        throw LookupError("no flavor group named \"" + flavor + "\"");
    return csl::api::SpaceHandle(
        std::shared_ptr<csl::Space const>(m_state, space));
}

void Model::diagonalizeMassMatrices()
{
    checkOpen(*m_state, "diagonalize the mass matrices");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->diagonalizeMassMatrices();
    });
}

namespace {

std::vector<mty::Particle> rawParticles(std::vector<Particle> const &particles)
{
    std::vector<mty::Particle> result;
    for (auto const &part : particles)
        result.push_back(part.raw());
    return result;
}

} // namespace

void Model::addTerm(csl::Expr const &term, bool addHermitic)
{
    checkOpen(*m_state, "add a term");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addLagrangianTerm(term, addHermitic);
    });
}

void Model::collectTerms()
{
    checkOpen(*m_state, "collect terms");
    guarded([&] { m_state->core->mergeTerms(); });
}

csl::Expr
Model::covariantDerivative(std::string const             &group,
                           Particle const                &field,
                           csl::Index const              &mu,
                           std::vector<csl::Index> const &fieldIndices) const
{
    return guarded([&] {
        Session session(m_state->core.get());
        mty::GaugedGroup *gaugedGroup = m_state->core->getGaugedGroup(group);
        csl::Tensor       point       = mty::defaultSpaceTimePoint;
        return gaugedGroup->covariantDerivative(
            *field.raw(), mu, fieldIndices, point);
    });
}

void Model::replace(csl::Expr const &oldExpression,
                    csl::Expr const &newExpression)
{
    checkOpen(*m_state, "replace an expression");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->replace(oldExpression, newExpression);
    });
}

void Model::replace(csl::Tensor const &oldTensor,
                    csl::Expr const   &newExpression)
{
    checkOpen(*m_state, "replace a tensor");
    guarded([&] {
        Session session(m_state->core.get());
        csl::Tensor tensor = oldTensor;
        m_state->core->replace(tensor, newExpression);
    });
}

void Model::rotateFields(std::vector<Particle> const               &fields,
                         std::vector<Particle> const               &newFields,
                         std::vector<std::vector<csl::Expr>> const &rotation,
                         bool diagonalizeMasses,
                         int  nMassLessFields)
{
    checkOpen(*m_state, "rotate fields");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->rotateFields(rawParticles(fields),
                                    rawParticles(newFields),
                                    rotation,
                                    diagonalizeMasses,
                                    nMassLessFields);
    });
}

void Model::rotateFields(std::vector<std::string> const            &fields,
                         std::vector<std::string> const            &newFields,
                         std::vector<std::vector<csl::Expr>> const &rotation,
                         bool diagonalizeMasses,
                         int  nMassLessFields)
{
    checkOpen(*m_state, "rotate fields");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->rotateFields(
            fields, newFields, rotation, diagonalizeMasses, nMassLessFields);
    });
}

void Model::birotateFields(std::vector<Particle> const &fields1,
                           std::vector<Particle> const &fields2,
                           int                          nMassLessFields)
{
    checkOpen(*m_state, "rotate fields");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->birotateFields(
            rawParticles(fields1), rawParticles(fields2), nMassLessFields);
    });
}

void Model::addBosonicMass(Particle const &field, csl::Expr const &mass)
{
    checkOpen(*m_state, "add a mass");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addBosonicMass(field.raw(), mass);
    });
}

void Model::addFermionicMass(Particle const  &left,
                             Particle const  &right,
                             csl::Expr const &mass)
{
    checkOpen(*m_state, "add a mass");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addFermionicMass(left.raw(), right.raw(), mass);
    });
}

void Model::addFermionicMass(Particle const  &diracFermion,
                             csl::Expr const &mass)
{
    checkOpen(*m_state, "add a mass");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addFermionicMass(diracFermion.raw(), mass);
    });
}

Particle Model::promoteToMajorana(Particle const    &weylFermion,
                                  std::string const &newName)
{
    checkOpen(*m_state, "promote a fermion");
    guarded([&] {
        Session       session(m_state->core.get());
        mty::Particle field = weylFermion.raw();
        m_state->core->promoteToMajorana(field, newName);
    });
    return particle(newName.empty() ? weylFermion.name() : newName);
}

csl::Expr Model::coupling(std::string const &name) const
{
    return guarded([&] {
        Session session(m_state->core.get());
        if (!m_state->core->findScalarCoupling(name))
            throw LookupError("no coupling named \"" + name + "\"");
        return m_state->core->getScalarCoupling(name);
    });
}

csl::Expr Model::newCoupling(std::string const &name)
{
    checkOpen(*m_state, "add a coupling");
    return guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addScalarCoupling(name);
        return m_state->core->getScalarCoupling(name);
    });
}

void Model::addParticle(Particle const &particle)
{
    checkOpen(*m_state, "add a particle");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->addParticle(particle.raw());
    });
}

Particle Model::particle(std::string const &name) const
{
    for (auto const &part : m_state->core->getParticles()) {
        if (std::string(part->getName()) == name)
            return Particle(m_state, part);
    }
    throw LookupError("no particle named \"" + name + "\"");
}

Particle Model::ghostBoson(Particle const &vectorBoson,
                           std::string const &name,
                           bool               conjugated)
{
    checkOpen(*m_state, "add a ghost boson");
    auto ghost = guarded([&] {
        Session session(m_state->core.get());
        return ghostboson_s(name, vectorBoson.raw(), conjugated);
    });
    return Particle(m_state, ghost);
}

bool Model::contains(std::string const &name) const
{
    for (auto const &part : m_state->core->getParticles()) {
        if (std::string(part->getName()) == name)
            return true;
    }
    return false;
}

std::vector<Particle> Model::particles() const
{
    std::vector<Particle> result;
    for (auto const &part : m_state->core->getParticles())
        result.emplace_back(m_state, part);
    return result;
}

FeynmanRules Model::feynmanRules() const
{
    return FeynmanRules(m_state);
}

void Model::breakGaugeSymmetry(std::string const &group)
{
    checkOpen(*m_state, "break a gauge group");
    groupKind(*m_state, group);
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->breakGaugeSymmetry(group);
    });
}

void Model::breakGaugeSymmetry(
    std::string const                                     &group,
    std::map<std::string, std::vector<std::string>> const &renaming)
{
    checkOpen(*m_state, "break a gauge group");
    groupKind(*m_state, group);
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->breakGaugeSymmetry(group, renaming);
    });
}

void Model::breakFlavorSymmetry(std::string const &flavor)
{
    checkOpen(*m_state, "break a flavor group");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->breakFlavorSymmetry(flavor);
    });
}

void Model::breakFlavorSymmetry(
    std::string const                                     &flavor,
    std::map<std::string, std::vector<std::string>> const &renaming)
{
    checkOpen(*m_state, "break a flavor group");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->breakFlavorSymmetry(flavor, renaming);
    });
}

csl::Index Model::generateIndex(std::string const &group) const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->generateIndex(group);
    });
}

csl::Index Model::generateIndex(std::string const &group,
                                std::string const &field) const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->generateIndex(group, field);
    });
}

void Model::promoteToGoldstone(std::string const &goldstone,
                               std::string const &gaugeBoson)
{
    checkOpen(*m_state, "promote a Goldstone boson");
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->promoteToGoldstone(goldstone, gaugeBoson);
    });
}

Amplitude Model::computeAmplitude(Order order, std::vector<Leg> const &legs)
{
    std::vector<mty::Insertion> insertions;
    for (auto const &leg : legs) {
        mty::Insertion insertion(leg.field.raw());
        insertion.setIncoming(leg.incoming);
        insertion.setParticle(!leg.antiparticle);
        insertions.push_back(insertion);
    }
    int const coreOrder
        = order == Order::TreeLevel ? mty::TreeLevel : mty::OneLoop;
    return Amplitude(guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->computeAmplitude(coreOrder, insertions);
    }));
}

csl::Expr Model::computeSquaredAmplitude(Amplitude const &amplitude)
{
    return guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->computeSquaredAmplitude(amplitude.raw());
    });
}

std::string Model::describe() const
{
    return guarded([&] {
        Session            session(m_state->core.get());
        std::ostringstream sout;
        sout << *m_state->core;
        return sout.str();
    });
}

csl::Index diracIndex(std::string const &name)
{
    return guarded([&] { return mty::DiracIndex(name); });
}

csl::Index minkowskiIndex(std::string const &name)
{
    return guarded([&] { return csl::Minkowski.generateIndex(name); });
}

csl::Expr renamedIndices(csl::Expr const &expr)
{
    return guarded([&] { return csl::RenamedIndices(expr); });
}

csl::Expr
diracGamma(csl::Index const &mu, csl::Index const &a, csl::Index const &b)
{
    return guarded([&] { return DiracGamma()({mu, a, b}); });
}

csl::Expr diracGamma5(csl::Index const &a, csl::Index const &b)
{
    return guarded([&] { return DiracGamma5()({a, b}); });
}

csl::Expr diracSigma(csl::Index const &mu,
                     csl::Index const &nu,
                     csl::Index const &a,
                     csl::Index const &b)
{
    return guarded([&] { return DiracSigma()({mu, nu, a, b}); });
}

csl::Expr diracPL(csl::Index const &a, csl::Index const &b)
{
    return guarded([&] { return DiracPL()({a, b}); });
}

csl::Expr diracPR(csl::Index const &a, csl::Index const &b)
{
    return guarded([&] { return DiracPR()({a, b}); });
}

csl::Expr diracC(csl::Index const &a, csl::Index const &b)
{
    return guarded([&] { return DiracCMatrix()({a, b}); });
}

} // namespace mty::api::modeling
