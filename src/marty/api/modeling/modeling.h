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

#include "../../../csl/api/tensor.h"
#include "../../core/model.h"
#include "../../core/quantumField.h"
#include "../group_theory/group_theory.h"
#include "feynmanRules.h"

#include <cstddef>
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
    std::string
        coupling; ///< Name of the gauge coupling; empty keeps g_<name>.
};

namespace detail {

/// Core model and gauge group kinds, shared by a Model and its Particles.
struct ModelState {
    std::shared_ptr<mty::Model> core;
    /// One API handle per core gauge group, so irreps of a model compare.
    std::map<mty::AbstractGroup *, std::shared_ptr<const group_theory::Group>>
        groupHandles;
    /// Set by Model::finalize(): the builders and mutators then refuse
    /// changes.
    bool finalized = false;
};

} // namespace detail

/// A flavor group of a model: its name and its number of flavors.
struct FlavorGroup {
    std::string name;
    int         dim;
};

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

    /// The particle as an expression, without indices.
    csl::Expr expr() const;

    /// The particle as an expression with the given indices.
    csl::Expr operator()(std::vector<csl::Index> const &indices) const;

    /// Field strength tensor of the particle, as a new particle.
    Particle fieldStrength() const;

    /// Particle of the same kind with another name, as a new particle.
    Particle createSimilarParticle(std::string const &name) const;

    void setSelfConjugate(bool selfConjugate);
    bool isSelfConjugate() const;

    /// Default ghost boson of a vector boson, created on first use.
    Particle ghostBoson() const;

    /// Declares the conjugated ghost boson of a vector boson.
    void setConjugatedGhostBoson(Particle const &ghost);

    /// Sets the representation of the particle in a flavor group.
    void setFundamentalFlavorRep(std::string const &flavorGroup);

    /// Mass symbol or value of the particle, as an expression.
    csl::Expr mass() const;

    /// Sets the mass symbol or value of the particle.
    void setMass(csl::Expr const &mass);

    /// Width symbol or value of the particle, used by its propagator.
    csl::Expr width() const;

    /// Sets the width symbol or value of the particle, used by its propagator.
    void setWidth(csl::Expr const &width);

    /// Sets the gauge fixing choice (xi) of a vector boson.
    void setGaugeChoice(mty::gauge::Type choice);

    /// Gauge fixing parameter (xi) symbol of a vector boson.
    csl::Expr gaugeParameter() const;

    /// Spin of the particle: (dim - 1) / 2 for its Lorentz representation.
    group_theory::Rational spin() const;

    /// Irrep of the particle under a gauge group.
    group_theory::Irrep irrep(std::string const &group) const;

    mty::Particle const &raw() const
    {
        return m_particle;
    }

  private:
    std::shared_ptr<detail::ModelState> m_state;
    mty::Particle                       m_particle;
};

/// Order of a perturbative calculation.
enum class Order { TreeLevel, OneLoop };

/// Chirality of a fermionic external leg: the Dirac field, or one Weyl
/// component.
enum class Chirality { Dirac, Left, Right };

/// An external leg of an amplitude: incoming or outgoing, particle or
/// antiparticle, with an optional chirality.
struct Leg {
    Particle  field;
    bool      incoming;
    bool      antiparticle = false;
    Chirality chirality    = Chirality::Dirac;
};

/// Options of a tree-level calculation, applied to the diagrams of an
/// amplitude.
struct AmplitudeOptions {
    /// Topology name ("Tadpole", "Mass", "Triangle", "Box", "Pentagon", or
    /// several of them separated by "|"). Empty means any topology.
    std::string topology;
    /// Diagrams without all these particles are removed.
    std::vector<Particle> forceParticles;
    /// Diagrams containing any of these particles are removed.
    std::vector<Particle> forbidParticles;
    /// Particles removed from the interaction terms before building diagrams.
    std::vector<Particle> disableParticles;
    /// Permutation of the 0-based leg positions giving the fermion order.
    /// Empty keeps the default order.
    std::vector<int> fermionOrder;
};

/// Amplitude computed by a Model.
class Amplitude {
  public:
    explicit Amplitude(mty::Amplitude amplitude);

    /// Number of Feynman diagrams of the amplitude.
    std::size_t diagramCount() const;

    /// Copy of the amplitude keeping only the diagrams at \b indices
    /// (LookupError if one is out of range).
    Amplitude select(std::vector<std::size_t> const &indices) const;

    mty::Amplitude const &raw() const
    {
        return *m_amplitude;
    }

  private:
    std::shared_ptr<mty::Amplitude const> m_amplitude;
};

/// A model: gauge groups, particles, breaking and Feynman rules.
class Model {
  public:
    /// Declares the gauge and flavor groups, then initializes the model.
    explicit Model(std::vector<GaugeGroup> const  &gauge,
                   std::vector<FlavorGroup> const &flavor = {});

    /// The built-in Standard Model.
    static Model standardModel();

    /// The built-in QED model.
    static Model qed();

    /// Creates a Weyl fermion attached to this model. Chirality is "L" or "R".
    Particle weylFermion(std::string const &name,
                         std::string const &chirality);

    /// Creates a Dirac fermion attached to this model.
    Particle diracFermion(std::string const &name);

    /// Creates a Majorana fermion attached to this model.
    Particle majoranaFermion(std::string const &name);

    /// Creates a real scalar boson attached to this model.
    Particle scalarBoson(std::string const &name);

    /// Creates a vector boson (not a gauge boson) attached to this model.
    Particle vectorBoson(std::string const &name);

    /// Renames a particle of the model (LookupError if absent, ValueError if
    /// the new name is already used).
    void renameParticle(std::string const &name, std::string const &newName);

    /// Recomputes the derived quantities of the model.
    void refresh();

    /// Completes the model and freezes it: after this call, the builders and
    /// mutators raise StateError. Calling it again does nothing.
    void finalize();

    /// Whether finalize() was called.
    bool isFinalized() const;

    /// Adds a tensorial coupling constant to the model.
    void addTensorCoupling(csl::Tensor const &tensor);

    /// Index space of a flavor group, to build tensors on (e.g. Yukawas).
    csl::api::SpaceHandle vectorSpace(std::string const &flavor) const;

    /// Generator of a gauge group in the representation of a field: a tensor
    /// with the adjoint space and the field representation space(s).
    csl::api::TensorHandle getGenerator(std::string const &group,
                                        std::string const &field) const;

    /// Diagonalizes the bosonic and fermionic mass matrices where possible.
    void diagonalizeMassMatrices();

    /// Replaces a Yukawa coupling by its diagonal form, with the given
    /// diagonal entries (global factors included). If mixing is set, the
    /// mixed particles are rotated with it.
    void diagonalizeYukawa(std::string const            &yukawa,
                           std::vector<csl::Expr> const &diagonal,
                           csl::Tensor const            &mixing,
                           std::vector<Particle> const  &mixed);

    /// Turns the mass coefficient of a particle into a named symbol
    /// (m_<name>).
    void gatherMass(std::string const &particle);

    /// Adds a term to the Lagrangian; addHermitic also adds its conjugate.
    void addTerm(csl::Expr const &term, bool addHermitic = false);

    /// Collects the Lagrangian terms with the same fields, summing them.
    void collectTerms();

    /// Covariant derivative of a field under a gauge group: D_mu field, where
    /// fieldIndices are the indices of the field.
    csl::Expr
    covariantDerivative(std::string const             &group,
                        Particle const                &field,
                        csl::Index const              &mu,
                        std::vector<csl::Index> const &fieldIndices) const;

    /// Replaces an expression everywhere in the model.
    void replace(csl::Expr const &oldExpression,
                 csl::Expr const &newExpression);

    /// Replaces a tensor everywhere in the model, by an expression. The
    /// tensor is replaced as a whole, so the expression may be zero.
    void replace(csl::Tensor const &oldTensor, csl::Expr const &newExpression);

    /// Rotates the fields into newFields with the unitary matrix rotation.
    void rotateFields(std::vector<Particle> const               &fields,
                      std::vector<Particle> const               &newFields,
                      std::vector<std::vector<csl::Expr>> const &rotation,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    /// Diagonalizes the mass matrix of the fields among themselves, without
    /// an explicit rotation matrix.
    void rotateFields(std::vector<Particle> const &fields,
                      bool                         diagonalizeMasses,
                      int                          nMassLessFields = 0);

    /// Rotates fields given by name; the new fields are created like the
    /// first field, under the names newFields.
    void rotateFields(std::vector<std::string> const            &fields,
                      std::vector<std::string> const            &newFields,
                      std::vector<std::vector<csl::Expr>> const &rotation,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    /// Rotates the left and right fields of a fermion or boson pair.
    void birotateFields(std::vector<Particle> const &fields1,
                        std::vector<Particle> const &fields2,
                        int                          nMassLessFields = 0);

    /// Adds a bosonic mass term for a field.
    void addBosonicMass(Particle const &field, csl::Expr const &mass);

    /// Adds a fermionic mass term between a left and a right fermion.
    void addFermionicMass(Particle const  &left,
                          Particle const  &right,
                          csl::Expr const &mass);

    /// Adds a Dirac fermion mass term.
    void addFermionicMass(Particle const &diracFermion, csl::Expr const &mass);

    /// Turns a self-conjugate Weyl fermion into a Majorana fermion, named
    /// newName or else as the Weyl fermion. Returns the Majorana particle,
    /// which replaces the Weyl one.
    Particle promoteToMajorana(Particle const    &weylFermion,
                               std::string const &newName = "");

    /// Scalar coupling of the model (LookupError if absent).
    csl::Expr coupling(std::string const &name) const;

    /// Creates a scalar coupling of the model and returns it.
    csl::Expr newCoupling(std::string const &name);

    /// Adds a particle to the model. Its kinetic and mass terms are added to
    /// the Lagrangian unless \p initializeTerms is false (e.g. for a component
    /// of an existing field, whose terms are already in the Lagrangian).
    void addParticle(Particle const &particle, bool initializeTerms = true);

    /// Ghost boson of a vector boson, with a given name, as a new particle.
    Particle ghostBoson(Particle const    &vectorBoson,
                        std::string const &name,
                        bool               conjugated = false);

    /// Particle of the model with the given name (LookupError if absent).
    Particle particle(std::string const &name) const;

    bool contains(std::string const &name) const;

    std::vector<Particle> particles() const;

    /// Terms of the Lagrangian of one kind: "kinetic", "mass" or
    /// "interaction" (ValueError otherwise).
    std::vector<csl::Expr> lagrangianTerms(std::string const &kind) const;

    /// Lazy view on the Feynman rules; the core computes them on first use.
    FeynmanRules feynmanRules() const;

    /// Breaks a gauge group, the broken particles get default names.
    void breakGaugeSymmetry(std::string const &group);

    /// Breaks a gauge group, with explicit names for some particles.
    void breakGaugeSymmetry(
        std::string const                                     &group,
        std::map<std::string, std::vector<std::string>> const &renaming);

    /// Breaks a flavor group, the broken particles get default names.
    void breakFlavorSymmetry(std::string const &flavor);

    /// Breaks a flavor group, with explicit names for some particles.
    void breakFlavorSymmetry(
        std::string const                                     &flavor,
        std::map<std::string, std::vector<std::string>> const &renaming);

    /// Fresh index of the fundamental representation of a gauge or flavor
    /// group.
    csl::Index generateIndex(std::string const &group) const;

    /// Fresh index of the representation of a field in a gauge group.
    csl::Index generateIndex(std::string const &group,
                             std::string const &field) const;

    /// Promotes a scalar to the Goldstone boson of a gauge boson.
    void promoteToGoldstone(std::string const &goldstone,
                            std::string const &gaugeBoson);

    /// Amplitude of the given external legs at the given order.
    Amplitude computeAmplitude(Order order, std::vector<Leg> const &legs);

    /// Amplitude with the diagram selection and interaction filters of
    /// options. Empty momentumIndices keep the default momentum names
    /// (1, ..., N); otherwise there is one distinct positive index per leg.
    Amplitude computeAmplitude(Order                           order,
                               std::vector<Leg> const         &legs,
                               std::vector<std::size_t> const &momentumIndices,
                               AmplitudeOptions const         &options);

    /// Squared amplitude, summed over the spins and polarizations.
    csl::Expr computeSquaredAmplitude(Amplitude const &amplitude);

    /// Interference term of two amplitudes, summed over the spins and
    /// polarizations.
    csl::Expr computeSquaredAmplitude(Amplitude const &amplitudeLeft,
                                      Amplitude const &amplitudeRight);

    /// Total decay width of a particle at the given order, summed over its
    /// two-body decays. Zero for a massless particle.
    csl::Expr computeWidth(Order                  order,
                           Particle const        &particle,
                           AmplitudeOptions const &options = {});

    /// Space-separated particle names, for printing.
    std::string describe() const;

    friend mty::Model &coreModel(Model const &model);

  private:
    explicit Model(std::shared_ptr<detail::ModelState> state);

    std::shared_ptr<detail::ModelState> m_state;
};

/// Core model behind a Model, for the calculation facade only.
mty::Model &coreModel(Model const &model);

/// Fresh Dirac index, named if name is not empty.
csl::Index diracIndex(std::string const &name = "");

/// Fresh Lorentz index of the Minkowski space, with the given name.
csl::Index minkowskiIndex(std::string const &name = "mu");

/// Expression with its dummy indices renamed, see csl::RenamedIndices.
csl::Expr renamedIndices(csl::Expr const &expr);

/// Dirac tensors of the standard 4-dim Dirac space. Lorentz indices (mu, nu)
/// come first, followed by the explicit Dirac indices (a, b).
csl::Expr
diracGamma(csl::Index const &mu, csl::Index const &a, csl::Index const &b);
csl::Expr diracGamma5(csl::Index const &a, csl::Index const &b);
csl::Expr diracSigma(csl::Index const &mu,
                     csl::Index const &nu,
                     csl::Index const &a,
                     csl::Index const &b);
csl::Expr diracPL(csl::Index const &a, csl::Index const &b);
csl::Expr diracPR(csl::Index const &a, csl::Index const &b);
csl::Expr diracC(csl::Index const &a, csl::Index const &b);

} // namespace mty::api::modeling

#endif
