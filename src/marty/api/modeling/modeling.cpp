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
#include "marty/core/fermionicField.h"
#include "marty/models/qed.h"
#include "marty/models/sm.h"

#include <numeric>
#include <sstream>

namespace mty::api::modeling {
namespace {

template <class F>
auto guarded(F &&f)
{
    try {
        return f();
    }
    catch (...) {
        rethrowAsApiError();
    }
}

mty::group::Type groupKind(detail::ModelState const &state,
                           std::string const        &group)
{
    auto found = state.groups.find(group);
    if (found == state.groups.end())
        throw LookupError("no gauge group named \"" + group + "\"");
    return found->second;
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
    if (isU1(group))
        throw TypeError("group \"" + group
                        + "\" is U(1): set its charge with setCharge()");
    guarded([&] { m_particle->setGroupRep(group, labels); });
}

void Particle::setCharge(std::string const &group, int num, int den)
{
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

Model::Model(std::vector<GaugeGroup> const &gauge)
    : m_state(std::make_shared<detail::ModelState>())
{
    std::vector<mty::GaugeGroupSpec> specs;
    for (auto const &item : gauge) {
        if (!item.group)
            throw ValueError("gauge group \"" + item.name + "\" is null");
        if (m_state->groups.contains(item.name))
            throw ValueError("duplicate gauge group \"" + item.name + "\"");
        mty::GaugeGroupSpec spec;
        spec.type                  = item.group->kind();
        spec.dim                   = item.group->size();
        spec.name                  = item.name;
        spec.bosonName             = item.bosonName;
        m_state->groups[item.name] = spec.type;
        specs.push_back(spec);
    }
    m_state->core = guarded([&] {
        Session session(nullptr);
        return std::make_shared<mty::Model>(std::as_const(specs));
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
    return Model(std::move(state));
}

Model Model::qed()
{
    auto state  = std::make_shared<detail::ModelState>();
    state->core = guarded([] {
        Session session(nullptr);
        return std::make_shared<QED_Model>();
    });
    return Model(std::move(state));
}

Particle Model::weylFermion(std::string const &name,
                            std::string const &chirality)
{
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

void Model::addParticle(Particle const &particle)
{
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

std::vector<std::string> Model::feynmanRules() const
{
    return guarded([&] {
        Session                  session(m_state->core.get());
        std::vector<std::string> result;
        for (auto const &rule : m_state->core->getFeynmanRules()) {
            std::ostringstream sout;
            sout << rule;
            result.push_back(sout.str());
        }
        return result;
    });
}

void Model::breakGaugeSymmetry(std::string const &group)
{
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
    groupKind(*m_state, group);
    guarded([&] {
        Session session(m_state->core.get());
        m_state->core->breakGaugeSymmetry(group, renaming);
    });
}

std::string Model::describe() const
{
    std::ostringstream sout;
    bool               first = true;
    for (auto const &part : m_state->core->getParticles()) {
        sout << (first ? "" : " ") << part->getName();
        first = false;
    }
    return sout.str();
}

} // namespace mty::api::modeling
