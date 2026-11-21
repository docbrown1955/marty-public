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

/**
 * @file modeling.h
 * @brief Facade for building models from gauge groups and particles.
 *
 * The facade hides the initialization steps of mty::Model: the gauge groups
 * are declared at construction, and the model is initialized and its gauge
 * bosons named without further calls. Errors are typed (see error.h).
 */

#ifndef MTY_API_MODELING_MODELING_H_INCLUDED
#define MTY_API_MODELING_MODELING_H_INCLUDED

#include "../../core/model.h"
#include "../../core/quantumField.h"
#include "../group_theory/group_theory.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace mty::api::modeling {

/// A gauge group of a model: the group, its name and the name of its bosons.
struct GaugeGroup {
    std::shared_ptr<const group_theory::Group> group;
    std::string                                name;
    std::string                                bosonName;
};

namespace detail {

/// Core model and gauge group kinds, shared by a Model and its Particles.
struct ModelState {
    std::shared_ptr<mty::Model>             core;
    std::map<std::string, mty::group::Type> groups;
};

} // namespace detail

/// A particle of a model, as held by the model.
class Particle {
  public:
    Particle(std::shared_ptr<detail::ModelState> state,
             mty::Particle                       particle);

    std::string name() const;

    /// Whether the gauge group is U(1), i.e. whether its charge is a rational.
    bool isU1(std::string const &group) const;

    /// Sets the highest weight (Dynkin labels) for a non-U(1) group.
    void setLabels(std::string const &group, std::vector<int> const &labels);

    /// Sets the charge num/den of a U(1) group.
    void setCharge(std::string const &group, int num, int den);

    mty::Particle const &raw() const
    {
        return m_particle;
    }

  private:
    std::shared_ptr<detail::ModelState> m_state;
    mty::Particle                       m_particle;
};

/// A model: gauge groups, particles, breaking and Feynman rules.
class Model {
  public:
    /// Declares the gauge groups, then initializes the model.
    explicit Model(std::vector<GaugeGroup> const &gauge);

    /// The built-in Standard Model.
    static Model standardModel();

    /// The built-in QED model.
    static Model qed();

    /// Creates a Weyl fermion attached to this model. Chirality is "L" or "R".
    Particle weylFermion(std::string const &name,
                         std::string const &chirality);

    /// Adds a particle to the model.
    void addParticle(Particle const &particle);

    /// Particle of the model with the given name (LookupError if absent).
    Particle particle(std::string const &name) const;

    bool contains(std::string const &name) const;

    std::vector<Particle> particles() const;

    /// Feynman rules, one printed string per rule.
    std::vector<std::string> feynmanRules() const;

    /// Breaks a gauge group, the broken particles get default names.
    void breakGaugeSymmetry(std::string const &group);

    /// Breaks a gauge group, with explicit names for some particles.
    void breakGaugeSymmetry(
        std::string const                                     &group,
        std::map<std::string, std::vector<std::string>> const &renaming);

    /// Space-separated particle names, for printing.
    std::string describe() const;

  private:
    explicit Model(std::shared_ptr<detail::ModelState> state);

    std::shared_ptr<detail::ModelState> m_state;
};

} // namespace mty::api::modeling

#endif
