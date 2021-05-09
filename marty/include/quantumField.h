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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Contains QuantumField and QuantumFieldParent, basic objects handling
 * quantum fields as csl expression and physical properties.
 */
#ifndef QUANTUMFIELD_H
#define QUANTUMFIELD_H

#include <typeinfo>
#include "mrtError.h"
#include "gauge.h"
#include "flavor.h"
#include "iterable.h"
#include "quantumNumber.h"
#include "latexLink.h"
#include "doc_brown_link.h"


namespace mty {
/*!
 * \brief Chirality for fermions. Either Left or Right for WeylFermion,
 * or None for DiracFermion.
 */
enum class Chirality {
    Left = 0,  /*!< Left chirality for WeylFermion. */
    Right = 1, /*!< Rihght chirality for WeylFermion. */
    None = 2   /*!< No chirality for DiracFermion. */
};

/*!
 * \brief Returns the inverse chirality with respect to init.
 * \param init Initial chirality.
 * \return \b Chirality::Left  if init is Chirality::Right.
 * \return \b Chirality::Right if init is Chirality::Left.
 * \return \b Chirality::None  if init is Chirality::None.
 */
Chirality operator!(Chirality init);

inline
std::ostream &operator<<(std::ostream &out, Chirality chir)
{
    switch(chir) {
        case Chirality::Left: out << "Left"; break;
        case Chirality::Right: out << "Right"; break;
        default: out << "No chirality";
    }
    return out;
}

    class Particle;
    class QuantumField;
    class WeylFermion;
    class DiracFermion;
    class GaugeBoson;
    class GhostBoson;
    class GoldstoneBoson;

/*!
 * \brief Default space-time point when not given while defining a QuantumField.
 * \details See QuantumField::operator(). To define a QuantumField, one needs in
 * general indices (except for fully scalar fields), and a space-time point. 
 * This defines an object like \f$ A^{\mu A}(X) \f$. That being said, it is 
 * rarely important to specify the space-time point of the field. In particular,
 * in any interaction term of the Lagrangian all fields are at the same point. 
 * This variable is given by default to QuantumField when no point is given.
 * A code like \code A({mu, A}); \endcode is equivalent to 
 * \code A({mu, A}, defaultSpaceTimePoint) \endcode. Then, whe integrated to the 
 * Lagrangian, the point is automatically replaced by the one of the Lagrangian.
 */
inline csl::Tensor defaultSpaceTimePoint = csl::tensor_s(
        "X",
        &csl::Minkowski
        );

enum class ParticleType {

    ScalarBoson,
    VectorBoson,
    FieldStrength,
    GhostBoson,
    GoldstoneBoson,
    WeylFermion,
    DiracFermion,
};

std::ostream &operator<<(
        std::ostream &out,
        ParticleType  type
        );

/**
 * @brief Structure containing data about fermion ordering.
 *
 * @details Each external fermion in a process owns such data. self is the 
 * integer corresponding to the fermion, partner the number representing its
 * partner and isLeft a boolean telling if the fermion must be on the left of 
 * the chain or not. Take an example with fermions \f$ \psi _i \f$;
 * \f[
 *      \left(\bar{\psi}_2 \psi _3\right)\left(\bar{\psi}_4 \psi _1\right),
 * \f]
 * fermions \f$ \psi _i \f$ have respectively PartnerShips \f$ (2, 3, true) \f$,
 * \f$ (3, 2, false) \f$, \f$ (4, 1, true) \f$ and \f$ (1, 4, false) \f$.
 */
struct PartnerShip {
    int self    { -1    };
    int partner { -1    };
    bool isLeft { false };

    bool isHappyWith(PartnerShip const &other) const {
        // Sufficient condition
        return self != -1 && self == other.partner;
    }
};

/*!
 * \brief Base class for parents of QuantumField objects.
 * \details A number of features are implemented in this class. Management of 
 * the mass, the width etc. Features specific to each type of field (bosonic,
 * fermionic, ghost ...) are implemented in derived classes. QuantumFieldParent 
 * is the abstract (and unique) representation of a particle. As for 
 * csl::TensorParent or csl::TensorFieldParent, it will not appear in 
 * expressions. It contains all intrinsic features of a particle. For example
 * in an equation, \f$ A \f$ would be the abstract photon, the 
 * QuantumFieldParent whereas \f$ A^{\mu A}(X) \f$ would be the actual field,
 * with specific information (indices, conjugated or not, space-time point ...).
 * This field is then a QuantumField object. The QuantumFieldParent is there to
 * generate QuantumField when giving it indices and (optionnaly) a point
 * \f$ X \f$. See operator(). The QuantumField keeps a 
 * pointer to its parent in order to have access to all abstract properties.
 * \note QuantumFieldParent must not be created statically. The Particle object
 * must be created instead, a std::shared_ptr<QuantumFieldParent>.
 * \note QuantumFieldParent may not be built directly (pure abstract class). 
 * Instead, one must create a specialization. See ScalarBoson, VectorBoson, 
 * GaugeBoson, WeylFermion, DiracFermion, GoldstoneBoson, or GhostBoson.
 */
class QuantumFieldParent: public csl::TensorFieldParent {

    public:

    /*!
     * \brief Type definition. A Propagator_func is a function called to get the 
     * propagator result of the contraction of two fields.
     * \details The function is contained in a std::function. Such an object may
     * be instantiated with a c++ lambda function or a standard function defined
     * normally. The function or lambda must take four arguments. The two fields
     * contracted (QuantumField), the momentum integrated (csl::Tensor), and a 
     * boolean telling if one of the two fields is external. It must return an 
     * expression (csl::Expr) that is the propagator between the two fields for which
     * the factor \f$\frac{d^4p}{(2\pi)^4}\f$ has been removed. Implementations 
     * assure that if the field is complex, the conjugated field is always the 
     * second argument. If one of the fields is external, the propagator should 
     * be adapted. For example, the propagator of two scalar fields is
     * \f[
     *   \frac{ie^{-ipX}}{p^2-m^2}, 
     * \f]
     * whereas if one of the fields is external, the projection on the equations
     * of motion \f$ \partial _\mu\partial ^\mu + m^2 \f$ gives simply
     * \f[
     *   -ie^{-ipX}.
     * \f]
     * The Propagator_func must provide the entire expression of the propagator
     * but the integral \f$ \int \frac{d^4p}{(2\pi)^4} \f$ that is automatically 
     * added. In quantumFieldTheory.h / quantumFieldTheory.cpp all built-in 
     * propagators are written in the form of standard c++ functions. 
     * Propagators for any particle may be changed by the user. 
     * See mty::AddCustomPropagator().
     */
    using Propagator_func = std::function<
        csl::Expr(QuantumField const&,
             QuantumField const&,
             csl::Tensor &,
             bool)>;

    protected:

    /*!
     * \brief \b True if the field is bosonic.
     */
    bool bosonic;

    /*!
     * \brief \b True if the field is real (real scalars, gauge bosons
     * , majorana fermions).
     * \details By default, all particles are complex except VectorBoson fields
     * that are set real. This value has an impact in wick contractions.
     * \sa isSelfConjugate(), setSelfConjugate().
     */
    bool selfConjugate = false;

    /*!
     * \brief Tells if the field is integrated out in the theory. If true, the 
     * propagator is modified assuming \f$ M \gg p \f$.
     */
    bool integratedOut = false;

    /*!
     * \brief Tells if the field is physical. If true, it may appear in external
     * states. Otherwise it is forbidden.
     */
    bool physical = true;

    /*!
     * \brief Tells if the field is enabled in diagrams. If no, it is fully 
     * discarded in computations.
     */
    bool enabledInDiagrams = true;

    /*!
     * \brief Mass of the particle. Appears in the propagator.
     */
    csl::Expr mass = CSL_0;

    /*!
     * \brief Width of the particle. Appears in the propagator.
     */
    csl::Expr width = CSL_0;

    /*!
     * \brief Particle type in graphical diagrams (straight line, arrow, vector,
     * gluon ...).
     */
    drawer::ParticleType drawType = drawer::ParticleType::None;

    /*!
     * \brief Pointer to the space-time group. For know it is the Lorentz group, 
     * \f$ D_2 \f$ algebra.
     */
    Group* stGroup;
    
    /*!
     * \brief Spin representation in \b stGroup. To have a number corresponding
     * to the spin, see getSpinDimension().
     */
    Irrep spin;

    /*!
     * \brief Pointer to the gauge the particle is a representation of.
     */
    Gauge* gauge;

    /*!
     * \brief Representation of the field in the gauge \b gauge.
     */
    GaugeIrrep irrep;

    /*!
     * \brief Boolean making sure that polarization sum (see PolarizationField)
     * is initialized only onde.
     */
    bool polarizationSumInitialized = false;

    /*!
     * \brief Position of the first gauge index, after flavor indices.
     */
    size_t firstGaugeIndex;

    /*!
     * \brief Position of the first space index, after flavor and gauge indices.
     */
    size_t firstSpaceIndex;

    /*!
     * \brief Pointer to the flavor the particle is a representation of.
     */
    const Flavor* flavor;

    /*!
     * \brief Representation of the field in the flavor \b flavor.
     */
    FlavorIrrep flavorRep;

    /*!
     * \brief List of quantum numbers of the field.
     * \details Quantum numbers may be used to discard null diagrams more
     * quickly and speed up computations. See QuantumNumber and 
     * Expander::Options.
     */
    mutable
    std::map<QuantumNumber::ID, QuantumNumber::Value> qNumbers;

    /*!
     * \brief Contains all propagators for the field with other. 
     * \details Maps another field to a Propagator_func. For example, a vector 
     * boson has two propagators. One with itself, see mty::VectorPropagator in 
     * quantumFieldTheory.h, one with its field strength, see 
     * mty::FieldStrengthPropagator in quantumFieldTheory.h. Propagators may be
     * modified through the function mty::AddCustomPropagator.
     */
    std::map<QuantumFieldParent const*, Propagator_func> propagator;

    std::vector<std::weak_ptr<mty::QuantumFieldParent>> relatives;

    public:

    /*!
     * \brief Default constructor.
     */
    QuantumFieldParent() = default;

    /*!
     * \brief Copy constructor.
     */
    QuantumFieldParent(QuantumFieldParent const& other) = default;

    /*!
     * \brief Constructor with 3 parameters.
     * \details The gauge representation, not given here, is initialized to the
     * trivial representation. The flavor is null and no flavor representation
     * is created. The particle is by default complex.
     * \param t_name  Name of the field.
     * \param t_spin  Integer equal to twice the spin of the particle.
     * \param t_gauge Pointer to the gauge of the model.
     */
    QuantumFieldParent(const std::string& t_name,
                       int                t_spin,
                       Gauge*             t_gauge);

    /*!
     * \brief Constructor with 3 parameters.
     * \details The gauge reprensentation, is initialized to \b irrep. The 
     * flavor is null and no flavor representation is created. The particle is
     * by default complex.
     * \param t_name Name of the field.
     * \param t_spin Integer equal to twice the spin of the particle.
     * \param irrep  Gauge representation of the field.
     */
    QuantumFieldParent(const std::string& t_name,
                       int                t_spin,
                       const GaugeIrrep&  irrep);

    /*!
     * \brief Constructor with 4 parameters.
     * \details The gauge representation, not given here, is initialized to the
     * trivial representation. The flavor is null and no flavor representation
     * is created.
     * \param t_name            Name of the field.
     * \param t_spin            Integer equal to twice the spin of the particle.
     * \param t_gauge           Pointer to the gauge of the model.
     * \param t_isSelfConjugate Boolean that must be true for real fields, false
     * otherwise.
     */
    QuantumFieldParent(const std::string& t_name,
                       int                t_spin,
                       Gauge*             t_gauge,
                       bool               t_isSelfConjugate);

    /*!
     * \brief Constructor with 4 parameters.
     * \details The gauge reprensentation, is initialized to \b irrep. The 
     * flavor is null and no flavor representation is created.
     * \param t_name            Name of the field.
     * \param t_spin            Integer equal to twice the spin of the particle.
     * \param irrep             Gauge representation of the field.
     * \param t_isSelfConjugate Boolean that must be true for real fields, false
     * otherwise.
     */
    QuantumFieldParent(const std::string& t_name,
                       int                t_spin,
                       const GaugeIrrep&  irrep,
                       bool               t_isSelfConjugate);

    /*!
     * \brief Constructor with 4 parameters.
     * \details The gauge reprensentation, is initialized to \b irrep. The 
     * flavor representation is initialized to \b flavorRep. The particle is
     * by default complex.
     * \param t_name    Name of the field.
     * \param t_spin    Integer equal to twice the spin of the particle.
     * \param irrep     Gauge representation of the field.
     * \param flavorRep Flavor representation of the field.
     */
    QuantumFieldParent(const std::string& t_name,
                       int                t_spin,
                       const GaugeIrrep&  irrep,
                       const FlavorIrrep& flavorRep);

    /*!
     * \brief Builds a new particle identical to another, with a different name.
     * \param t_name Name of the field.
     * \param other  Ohter field to copy.
     */
    QuantumFieldParent(const std::string&        t_name,
                       const QuantumFieldParent* other);

    /*!
     * \brief Returns the drawing type of the particle.
     * \sa #drawType, setDrawType().
     * \return The drawing type of the particle.
     */
    drawer::ParticleType getDrawType() const;

    virtual bool contains(QuantumFieldParent const *other) const {
        return this == other;
    }

    virtual bool isSameSpecies(QuantumFieldParent const *other) const {
        return this == other;
    }

    virtual ParticleType getParticleType() const;

    /*!
     * \brief Sets the drawing type of the particle.
     * \details The drawing type is how the particle will be drawn on diagrams.
     * \param type Type of line for the particle in diagrams.
     * \sa #drawType, getDrawType().
     */
    void setDrawType(drawer::ParticleType type);

    /*!
     * \brief Pure virtual function. Generates a partical similar to the current
     * object, changing only the name.
     * \param t_name Name of the new particle, identical to the current object.
     * \return The new Particle.
     */
    virtual Particle generateSimilar(std::string const& t_name) const = 0;

    /*!
     * \brief Returns the chirality of the particle.
     * \details A DiracFermion or boson will return Chirality::None. WeylFermion
     * will return either Chirality::Left or Chirality::Right.
     * \return The chirality of the particle.
     */
    virtual Chirality getChirality() const;

    /*!
     * \brief Pure virtual function. Returns the energy dimension of the field.
     * \return An expression (numerical) equal to the dimension of the field.
     */
    virtual csl::Expr getEnergyDimension() const = 0;

    /*!
     * \brief Pure virtual function. Returns the number of degrees of freedom 
     * in the particle. It does not count gauge and flavor multiplicity. The 
     * result depends on the type of the particle and if it is complex.
     * \return An integer, number of degrees of freedom of the particle.
     */
    virtual int getNDegreesOfFreedom() const = 0;

    int getNGaugeDegreesOfFreedom() const;

    /*!
     * \brief Tells if the field has a special contraction property with \b
     * other.
     * \param other Field with which there may be a contraction.
     * \return \b True  if there is a contraction property.
     * \return \b False otherwise.
     */
    virtual bool hasSpecialContractionWith(QuantumField const& other) const;

    /*!
     * \brief Tells if the field is chiral.
     * \return \b True  if the field is a WeylFermion.
     * \return \b False otherwise.
     */
    virtual bool isChiral() const;

    /*!
     * \brief For a WeylFermion embedded in a DiracFermion, returns the other 
     * part of the DiracFermion.
     * \return The right WeylFermion for a left WeylFermion embedded in a 
     * DiracFermion.
     * \return The left WeylFermion for a right WeylFermion embedded in a 
     * DiracFermion.
     * \return nullptr otherwise.
     */
    virtual Particle getChiralityCounterPart() const;

    /*!
     * \brief For a DiracFermion, returns the chiral WeylFermion it contains of 
     * chirality \b chirality if it has one.
     * \param chirality Chirality of the WeylFermion we want to get.
     * \return The WeylFermion of chirality \b chirality if there is one 
     * embedded in the DiracFermion.
     * \return nullptr otherwise.
     */
    virtual Particle getWeylFermion(Chirality chirality) const;

    /*!
     * \brief For a WeylFermion embedded in a DiracFermion, returns the dirac 
     * fermion.
     * \return A the DiracFermion embedding the current WeylFermion.
     * \return nullptr otherwise.
     */
    virtual Particle getDiracParent() const;

    /**
     * @brief For a vector boson in a non abelian gauged group, returns the 
     * associated ghost boson.
     *
     * @return The ghost associated with the vector if there is one.
     * @return nullptr else.
     */
    virtual Particle getGhost() const;

    /**
     * @brief For a vector boson in a non abelian gauged group, returns the 
     * associated Goldstone boson.
     *
     * @return The Goldstone associated with the vector if there is one.
     * @return nullptr else.
     */
    virtual Particle getGoldstone() const;

    virtual void setDiracParent(Particle const &diracFermion);

    /*!
     * \brief Returns the expression of the propagator between two fields.
     * \sa Propagator_func.
     * \param self     Left field of the propagator.
     * \param other    Right field of the propagator.
     * \param P        Momentum integrated in the propagator.
     * \param external \b True if one of the two fields is an external leg.
     * \return The expression of the propagator without the integral term 
     * \f$ \int \frac{d^4p}{(2\pi)^4} \f$.
     */
    virtual csl::Expr getPropagator(QuantumField const& self,
                               QuantumField const& other,
                               csl::Tensor      & P,
                               bool                external) const;

    /*!
     * \brief Returns the expression of the propagator between two fields.
     * \sa Propagator_func.
     * \details This function is called when the left field is complex 
     * conjugated. When it is the case, in order to ensure that the conjugate is
     * on the right, the order is reverted.
     * \param other    Left field of the propagator.
     * \param self     Right field of the propagator.
     * \param P        Momentum integrated in the propagator.
     * \param external \b True if one of the two fields is an external leg.
     * \return The expression of the propagator without the integral term 
     * \f$ \int \frac{d^4p}{(2\pi)^4} \f$.
     */
    virtual csl::Expr getInvPropagator(QuantumField const& other,
                                  QuantumField const& self,
                                  csl::Tensor      & P,
                                  bool                external) const;

    /*!
     * \brief Returns the kinetic term in the Lagrangian for the field.
     * \details This function is overriden in derived classes. Each particle has
     * a specific form for its kinetic term, including covariant derivatives and
     * mass term. An example of kinetic term for a complex scalar field is
     * \f[
            \mathcal{L} \ni (D_\mu \phi)^\dagger D^\mu\phi - m_\phi^2\phi
            ^\dagger \phi,
     * \f]
     * where \f$ D_\mu \f$ stands for the covariant derivative of the field 
     * including interactions with gauge vector fields. 
     * \sa mty::gauge::covariantDerivative().
     * \param X Space-time point for the kinetic term.
     * \return The mathematical expression in the Lagrangian for the field.
     */
    virtual csl::Expr getKineticTerm(csl::Tensor & X);

    /*!
     * \brief Tells if the particle is a gauge boson.
     * \return \b True  if the particle is a gauge boson (created by a 
     * GaugedGroup).
     * \return \b False otherwise.
     */
    virtual bool isGaugeBoson() const;

    /*!
     * \brief For a GaugeBoson, returns the expression of 
     * \f$ \xi \f$ in the current gauge.
     * \sa Gauge::GaugeChoice
     * \return \f$ \xi \f$ in the current gauge of the field if it is a gauge
     * boson.
     */
    virtual csl::Expr getXiGauge() const;

    /*!
     * \brief For a ghost or goldstone boson, returns the GaugeBoson associated
     * with it.
     * \return The GaugeBoson associated with the Golstone or Ghost.
     */
    virtual Particle getVectorBoson() const;

    virtual GaugedGroup const* getGaugedGroup() const;
    virtual GaugedGroup* getGaugedGroup();

    /*!
     * \brief For a GaugeBoson, returns the associated Golstone boson of there
     * is one.
     * \return The GoldstoneBoson associated to the GaugeBoson is it exists.
     * \return nullptr otherwise.
     */
    virtual Particle getGoldstoneBoson() const;

    /*!
     * \brief For a GoldstoneBoson, returns the associated Golstone boson of 
     * there is one.
     * \return The GoldstoneBoson associated to the GoldstoneBoson is it exists.
     * \return nullptr otherwise.
     */
    virtual Particle getGhostBoson() const;

    /*!
     * \brief For a GaugeBoson, sets the associated GoldstoneBoson.
     * \details After the call of this function, the link os created and the 
     * calling getGoldstoneBoson() for the gauge boson or getVectorBoson() for 
     * the golstone will indeed yield the right particles. The golstone boson
     * must have the exact same representation as the gauge boson except the 
     * spin. This function is called automatically when creating a golstone 
     * boson from a gauge boson through mty::golstoneboson_s(), or through
     * Model::promoteToGoldstone().
     * \sa mty::golstoneboson_s, Model::promoteToGoldstone(), GoldstoneBoson.
     * \param goldstone Particle that takes the place of a golstone boson for
     * the curent gauge boson.
     */
    virtual void setGoldstoneBoson(Particle const& goldstone);

    /*!
     * \brief For a GaugeBoson, sets the associated GhostBoson.
     * \details After the call of this function, the link os created and the 
     * calling getGhostBoson() for the gauge boson or getVectorBoson() for 
     * the ghost will indeed yield the right particles. The ghost boson
     * must have the exact same representation as the gauge boson except the 
     * spin. This function is called automatically when creating a ghost 
     * boson from a gauge boson through mty::ghostboson_s().
     * \sa mty::ghostboson_s, GhostBoson.
     * \param ghost Particle that takes the place of a ghost boson for
     * the curent gauge boson.
     */
    virtual void setGhostBoson(Particle const& ghost);

    /*!
     * \brief Tells if the particle has a FieldStrength.
     * \details A FieldStrength is defined automatically for VectorBoson fields,
     * gauge bosons or not. It is defined as
     * \f[
     *     F^I_{\mu\nu} \equiv \partial _\mu A^I_\nu - \partial _\nu A^I_\mu,
     * \f]
     * with \f$ A^A_\mu \f$ a vector boson with a generic index \f$ I \f$ 
     * representing all flavor and gauge structures. If the field strength is 
     * defined, the kinetic term is expressed as \f$ -\frac{1}{4}F^I_{\mu\nu}
     * F^{I\mu\nu} \f$ and contractions like \f$ \langle A^I_\mu F^J_{\nu\rho}
     * \rangle \f$ or \f$ \langle F^I_{\mu\nu}F^J_{\rho\sigma}\rangle \f$ are 
     * defined by built-in propagators.
     * \sa VectorBoson, FieldStrength, Propagator_func, quantumFieldTheory.h.
     * \return \b True  if the particle (should be a VectorBoson) has a 
     * FieldStrength defined.
     * \return \b False else.
     */
    virtual bool hasFieldStrength() const;

    /*!
     * \brief For a VectorBoson, returns the associated FieldStrength if it 
     * exists.
     * \sa hasFieldStrength(), FieldStrength, VectorBoson.
     * \return The FieldStrength of the the VectorBoson if it exists.
     * \return nullptr otherwise.
     */
    virtual Particle getFieldStrength() const;

    /*!
     * \brief For a VectorBoson, sets the associated FieldStrength.
     * \details The fieldStrength must of course be consitent with the actual
     * field. 
     * \sa hasFieldStrength(), FieldStrength, VectorBoson.
     * \return The FieldStrength of the the VectorBoson if it exists.
     * \return nullptr otherwise.
     */
    virtual void setFieldStrength(Particle const& fieldStrength);

    /*!
     * \brief Tells if the field may be contracted with another in Wick 
     * contrations.
     * \param other Other field to test the contraction.
     * \return \b True  if a propagator exists between the two fields.
     * \return \b False else.
     * \sa wick::WickCalculator, Propagator_func, AddCustomPropagator().
     */
    bool isContractibleWith(QuantumFieldParent const* other) const;

    /*!
     * \brief Tells if the field is bosonic.
     * \return \b True  if the field is bosonic.
     * \return \b False else.
     * \sa #bosonic, isFermionic(), isAntiCommuting().
     */
    bool isBosonic() const;

    /*!
     * \brief Tells if the field is fermionic.
     * \return \b True  if the field is fermionic.
     * \return \b False else.
     * \sa #bosonic, isBosonic(), isAntiCommuting().
     */
    bool isFermionic() const;

    /*!
     * \brief Tells if the field is a grassman field.
     * \return \b True if the field if fermionic or is a ghost.
     * \return \b False else.
     * \sa #bosonic, isBosonic(), isFermionic(), GhostBoson.
     */
    virtual bool isAntiCommuting() const;

    /*!
     * \brief Tells if the field is self-conjugate (real).
     * \details By default, all fields except vector bosons are complex. If the
     * field is self-conjugate, it may contract with itself (not conjugated) in
     * wick contractions.
     * \return \b True  if the field is real.
     * \return \b False if the field is complex.
     * \sa #selfConjugate, setSelfConjugate().
     */
    bool isSelfConjugate() const;

    bool isReal() const;

    /*!
     * \brief Tells if the field is physical, i.e. may appear in external legs.
     * \details If the field is not physical, it will be forbidden in external
     * legs. In particular, the function setExternal() will yield an error. For
     * example, ghosts and goldstone bosons are not physical.
     * \sa #physical, isExternal(), setExternal(), setPhysical(), Insertion.
     * \return \b True  if the field is physical.
     * \return \b False else.
     */
    bool isPhysical() const;

    /*!
     * \brief Tells if the field is enabled in diagrams.
     * \details If it is not enables, each interaction containing the particle 
     * will be discarded in computations.
     * \return \b True if the field may appear in diagrams.
     * \return \b False else.
     * \sa #enabledInDiagrams, setEnabledInDiagrams(), Expander::Options.
     */
    bool isEnabledInDiagrams() const;

    /*!
     * \brief Tells if the field is integrated out.
     * \details If true, the only thing that changes is that the propagator is 
     * replaced by an effective interaction assuming \f$M\gg p\f$. For example 
     * in the case of a massive vector boson, the standard propagator of a 
     * VectorBoson of mass \f$ M \f$ is
     * \f[
     *     -ie^{-ipX}\frac{g_{\mu\nu} - (1-\xi)\frac{p_\mu p_\nu}{p^2-\xi M^2}}
     *     {p^2-M^2},
     * \f]
     * whereas its integrated propagator becomes 
     * \f[
     *     ie^{-ipX}\frac{g_{\mu\nu}}{M^2}.
     * \f]
     * This is possible for any field (fermionic, scalar or vector). 
     * \return \b True  if the field is integrated out.
     * \return \b False otherwise.
     * \sa #integratedOut, integrateOut(), Propagator_func, 
     * quantumFieldTheory.h, EffModel, 
     * Wilson.
     */
    bool isIntegratedOut() const;

    /*!
     * \brief Returns twice the spin of the particle in an integer.
     * \details For simplicity, this function returns a simple integer. Half 
     * spins being not represented, the spin is multiplied by 2 before being 
     * returned. Spins of scalars, fermions and vectors are then 0, 1, 2.
     * \return Twice the spin of the particle.
     * \sa #spin.
     */
    int getSpinDimension() const;

    /*!
     * \brief Returns the expression of the mass of the particle.
     * \return The mass of the particle.
     * \sa #mass, setMass().
     */
    csl::Expr getMass() const;

    csl::Expr getSquaredMass() const;

    /*!
     * \brief Returns the expression of the witdh of the particle.
     * \return The witdh of the particle.
     * \sa #width, setWidth().
     */
    csl::Expr getWidth() const;

    virtual mty::gauge::Choice getGaugeChoice() const;

    /*!
     * \brief Returns the lorentz representation of the field.
     * \return The spin representation of the field.
     * \sa #spin, mty::Lorentz, mty::algebra::D
     */
    Irrep getLorentzRep() const;

    /*!
     * \brief Returns a pointer to the gauge the field is a representation of.
     * \return The gauge of the field.
     * \sa #gauge.
     */
    Gauge* getGauge() const;
    
    /*!
     * \brief Returns the gauge representation containing all individual group
     * representations of the field in the gauge.
     * \return The gauge representation of the field.
     * \sa #irrep, getGroupIrrep(), getFlavorIrrep().
     */
    GaugeIrrep getGaugeIrrep() const;

    /*!
     * \brief Returns the representation of the field in a particle group.
     * \details If the group is not found, an error is raised. This function 
     * works also for flavor representations.
     * \param group Pointer to the group from which we want the representation
     * of the field.
     * \return The representation of the field in \b group.
     * \sa #irrep, getFlavorIrrep(), getGaugeIrrep().
     */
    Irrep getGroupIrrep(const Group* group) const;

    Flavor const* getFlavor() const;

    void setFlavor(Flavor const *flavor);
    /*!
     * \brief Returns the flavor representation containing all individual group
     * representations of the field in the flavor.
     * \return The flavor representation of the field.
     * \sa #flavorRep, getGaugeIrrep(), getGroupIrrep(), getFlavorIrrep().
     */
    FlavorIrrep getFlavorIrrep() const;

    /*!
     * \brief Returns the representation of the field in a particle flavor 
     * group.
     * \details If the flavor group is not found, an error is raised.
     * \param group Pointer to the flavor group from which we want the 
     * representation of the field.
     * \return The representation of the field in \b group.
     * \sa #flavorRep, getFlavorIrrep(), getGaugeIrrep().
     */
    Irrep getFlavorIrrep(const FlavorGroup* group) const;

    /*!
     * \brief Returns a set of free space indices corresponding to the field.
     * \details This function in general will return zero or one index. Scalar 
     * fields have no space-time index, spin 1/2 and 1 have 1. However, in order
     * to potentially extend this function to higher spins with more space-time
     * indices, this function returns a std::vector.
     * \return New indices generated corresponding to the space-time indices of
     * the field.
     * \sa getGaugeIndex(), getFlavorIndex(), 
     * getFullSetOfIndices().
     */
    std::vector<csl::Index> getSpaceIndex(
            const std::string& nameIndex = "") const;

    /*!
     * \brief Returns a set of gauge indices corresponding to the field.
     * \return A set of new indices generated in the gauge representation spaces
     * of the field.
     * \sa getFlavorIndex(), getSpaceIndex(), getFullSetOfIndices().
     */
    std::vector<csl::Index> getGaugeIndex(std::vector<std::string> names
            = std::vector<std::string>()) const;

    /*!
     * \brief Returns a set of flavor indices corresponding to the field.
     * \return A set of new indices generated in the flavor representation 
     * spaces of the field.
     * \sa getSpaceIndex(), getGaugeIndex(), getFullSetOfIndices().
     */
    std::vector<csl::Index> getFlavorIndex(std::vector<std::string> names
            = std::vector<std::string>()) const;

    /*!
     * \brief Returns a full set of indices for the field including flavor
     * , gauge, and space-time indices. 
     * \details This set is composed of independent and free indices and may be 
     * used directly to generate a QuantumField. The indices are first flavor
     * indices, then gauge indices and finally space-time indices.
     * \return A full set of new indices generated for the field.
     * \sa getSpaceIndex(), getFlavorIndex(), getGaugeIndex().
     */
    std::vector<csl::Index> getFullSetOfIndices() const;

    /*!
     * \brief Returns the quantum number of the field with respect to \b number.
     * \details Each field may have different values for quantum numbers 
     * (quarks have baryonic number +1/3, fermionic number +1, lepton number 0).
     * This function returns the value associated to the field for a given 
     * QuantumNumber.
     * \param number QuantumNumber from which we want the field's value.
     * \return The value of the QuantumNumber for the field.
     * \sa #qNumbers.
     */
    QuantumNumber::Value getQuantumNumber(
            QuantumNumber const* number) const;

    /*!
     * \brief Returns the quantum number of an instance of the field with 
     * respect to \b number.
     * \details The difference with getQuantumNumber(QuantumNumber const*) is 
     * that if the instance of the field is complex conjugated, the function 
     * returns -1 times the value of the quantum number. For example, to take
     * the same example as in getQuantumNumber(QuantumNumber const*), a 
     * conjugated quark \f$ \bar{q} \f$ has -1/3 baryon number, -1 fermion 
     * number and 0 lepton number.getQuantumNumber(QuantumNumber const*)
     * \param instance Instance of the field we want the quantum number, may be 
     * conjugated or not.
     * \param number   QuantumNumber from which we want the field's value.
     * \return The value of the QuantumNumber for the field, with a minus sign
     * if the field is complex conjugated.
     * \sa #qNumbers.
     */
    QuantumNumber::Value getQuantumNumber(
            QuantumField  const* instance,
            QuantumNumber const* number) const;

    /*!
     * \brief Returns an instance of the quantum field as an expression.
     * \details This function generates a set of indices through 
     * getFullSetOfIndices(), and uses a space-time point by default or given by
     * the user. It returns an instance of the field like \f$ A^{\mu A}(X) \f$
     * calling operator().
     * \param point Sapce-time point at which the field is evaluated (optional).
     * \return An expression of the field instance with free and independent 
     * indices.
     * \sa getFullSetOfIndices(), operator()().
     */
    csl::Expr getInstance(csl::Tensor point 
            = csl::tensor_s("X", &csl::Minkowski));

    /*!
     * \brief Sets the integrated out property of the field.
     * \details If true, the propagator of the particle is modified, assuming
     * \f$ M^2\gg p^2 \f$. If false, the propagator is reset to its default.
     * \warning (de-)Integrating out a particle, all non default propagators 
     * defined through AddCustomPropagator() will be automatically suppressed.
     * There are indeed in general ill-defined in this case. 
     * \param value Boolean, must be true to integrate out the particle, false 
     * else (default = \b true).
     * \sa #integrateOut, isIntegratedOut(), AddCustomPropagator(), 
     * Propagator_func, quantumFieldTheory.h.
     */
    void integrateOut(bool value = true);

    /*!
     * \brief Sets the mass of the particle.
     * \details The mass created for the particle is a csl::Constant name \b
     * t_mass. If the field is added to a Model through Model::addField() later 
     * on, the corresponding mass term is added automatically to the Lagrangian.
     * \param t_mass Name of the constant mass.
     * \sa #mass, getMass().
     */
    void setMass(const std::string& t_mass);

    /*!
     * \brief Sets the mass of the particle.
     * \details The mass created for the particle is a csl::Constant name \b
     * t_mass with a numerical value of \b value. If the field is added to a 
     * Model through Model::addField() later on, the corresponding mass term is 
     * added automatically to the Lagrangian.
     * \param t_mass Name of the constant mass.
     * \param value  Value of the mass (should be expressed in GeV).
     * \sa #mass, getMass().
     */
    void setMass(const std::string& t_mass, long double value);

    /*!
     * \brief Sets the mass of the particle.
     * \details The expression of the mass given by the user may be anything, 
     * the only requirements is that it must be scalar (without free index).
     * If the field is added to a Model through Model::addField() later on, the
     * corresponding mass term is added automatically to the Lagrangian.
     * \param t_mass Expression of the new mass for the particle.
     * \sa #mass, getMass().
     */
    void setMass(const csl::Expr& t_mass);

    /*!
     * \brief Sets the width of the particle.
     * \details The expression of the width given by the user may be anything, 
     * the only requirements is that it must be scalar (without free index).
     * \param t_mass Expression of the new width for the particle.
     * \sa #width, getWidth().
     */
    void setWidth(const csl::Expr& t_width);

    virtual void setGaugeChoice(gauge::Type type);

    /*!
     * \brief Sets the complex conjugation property of the particle. 
     * \details If \b t_selfConjugate is true, the particle becomes real. 
     * Otherwise it becomes complex and \f$ \Phi ^* \neq \bar{\Phi} \f$.
     * \param t_selfConjugate Boolean telling if the field must be real.
     * \sa #selfConjugate, isSelfConjugate().
     */
    void setSelfConjugate(bool t_selfConjugate);

    /*!
     * \brief Sets the physical property of the particle.
     * \details If \b t_physical is true, the field is physical and may appear
     * as external legs of diagrams (insertions). Else, the field becomes 
     * forbidden in diagrams and setting them external will raise an error.
     * \param t_physical Boolean telling if the field is physical (may appear in
     * external legs).
     * \sa #physical, isPhysical(), setExternal().
     */
    void setPhysical(bool t_physical);

    /*!
     * \brief Enables or disables the particle in diagrams.
     * \details If \b t_enabled is false, the field may not appear in any 
     * computation and will be automatically discarded. This may be useful
     * in some cases for which we know that the contribution of one or several
     * particles is negligible, and that computing diagrams containing them is
     * irrelevant.
     * \param t_enabled Boolean telling if the field may appear in diagrams.
     * \sa #enabledInDiagrams, isEnabledInDiagrams().
     */
    void setEnabledInDiagrams(bool t_enabled);

    /*!
     * \brief Ajusts the flavor representation of the field to a new flavor 
     * group.
     * \details When breaking flavor symmetry, we may sometimes be interested in
     * breaking only partially the symmetry. For example, we may break a 
     * 3 flavor-symmetry in one light particle and a 2-flavor symmetry. This 
     * function allows to keep a flavor representation but in a different group
     * (here for example \f$ SU(3) \f$ to \f$ SU(2) \f$).
     * \param t_flavor New Flavor of the particle.
     * \sa #flavorRep, getFlavorIrrep().
     */
    void adjustFlavorRep(Flavor* t_flavor);

    void setGaugeIrrep(const GaugeIrrep& newRep);
    /*!
     * \brief Sets the representation of the field for a given group.
     * \details The Irrep given as parameter is an already built representation.
     * This function is called during gauge symmetry breaking, when 
     * representation are broken. Changing the representation, the indices of 
     * the tensor change and may disappear if the new representation is trivial, 
     * or appear if the new representation is not while the old one is.
     * \param group  Group for which the field representation is changed.
     * \param newRep New representation of the field in \b group.
     * \sa #irrep, getGroupIrrep(), getGaugeIrrep().
     */
    void setGroupRep(Group*       group, 
                     const Irrep& newRep);

    /*!
     * \brief Sets the representation of the field for a given group.
     * \details The Irrep given as parameter is in the form of a std::vector of
     * integers. These integer are the dinkin labels of the representation.
     * This function is called during gauge symmetry breaking, when 
     * representation are broken. Changing the representation, the indices of 
     * the tensor change and may disappear if the new representation is trivial, 
     * or appear if the new representation is not while the old one is.
     * \param group         Group for which the field representation is changed.
     * \param highestWeight Dinkin labels of the highest weight for the wanted
     * representation.
     * \sa #irrep, getGroupIrrep(), getGaugeIrrep(), SemiSimpleAlgebra,
     * SemiSimpleAlgebra::highestWeightRep(), setFlavorRep().
     */
    void setGroupRep(std::string      const& group,
                     std::vector<int> const& highestWeight);

    void setGroupRep(std::string const& group,
                     int                charge);

    /*!
     * \brief Sets the representation of the field for a given flavor group.
     * \details The Irrep given as parameter is an already built representation.
     * This function is called during flavor symmetry breaking, when 
     * representation are broken. Changing the representation, the indices of 
     * the tensor change and may disappear if the new representation is trivial, 
     * or appear if the new representation is not while the old one is.
     * \param group  Flavor group for which the field representation is changed.
     * \param newRep New representation of the field in \b group.
     * \sa #flavor, getFlavorIrrep(), setGroupRep().
     */
    void setFlavorRep(const FlavorGroup* group, 
                      const Irrep&       newRep);
    void setFundamentalFlavorRep(
            std::string const &flavorGroup
            );

    /*!
     * \brief Adds a non trivial quantum number to the particle.
     * \param number QuantumNumber for which we set the value (for the field). 
     * \param value  Value of the quantum number of the field.
     * \sa #qNumbers, QuantumNumber, getQuantumNumber().
     */
    void addQuantumNumber(QuantumNumber const& number,
                          QuantumNumber::Value value);

    /*!
     * \brief Sets broken parts (from gauge of flavor symmetry full breaking) 
     * containing the new particles.
     * \details This function should not be called directly by the user in 
     * general. It overrides a csl function that allows to break a tensor into
     * several parts. In this case the space is fully broken (no subspace left).
     * \param broken      Space that is broken in several parts.
     * \param brokenParts New particles arising from the symmmetry breaking.
     */
    virtual void setBrokenParts(
            const csl::Space*            broken,
            const std::vector<Particle>& brokenParts);

    /*!
     * \brief Returns the set of new tensors after breaking the particle in 
     * parts.
     * \param broken   Space (gauge or flavor) that is broken.
     * \param newSpace New (non trivial) vector spaces after the symmetry 
     * breaking.
     * \param pieces   Dimensions of new spaces. The sum must be equal to the 
     * dimension of \b broken.
     * \return The set of tensors arising after the symmetry breaking.
     */
    std::vector<csl::Parent> breakSpace(
            const csl::Space*                     broken,
            const std::vector<const csl::Space*>& newSpace,
            const std::vector<size_t>&           pieces
            ) const override;

    /*!
     * \brief Assignment operator (c++ default).
     * \param other Other field to copy.
     * \return A reference to the current object.
     * \sa QuantumFieldParent(QuantumFieldParent const&).
     */
    QuantumFieldParent& operator=(const QuantumFieldParent& other) = default;

    /*!
     * \brief Overload of operator<< for QuantumFieldParent.
     * \param fout Output flux.
     * \param obj  QuantumFieldParent to display (basically).
     * \return A reference to \b fout.
     */
    friend
    std::ostream& operator<<(std::ostream& fout, const QuantumFieldParent& obj);

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This functions takes no csl::Index. The field is then for sure
     * a ScalarBoson in a trivial Gauge representation. Example: \f$\phi(X)\f$.
     * \param t_point Space_time point of the scalar field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(const csl::Tensor& t_point) override;

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This functions takes one csl::Index. This may be for example
     * a field like \f$ A^\mu(X) \f$.
     * \param index   Unique index of the field.
     * \param t_point Space-time point of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(csl::Index          index,
                    const csl::Tensor& t_point) override;

    /*!
     * \brief Generates an instance of the QuantumField as a symbolic 
     * expression.
     * \param indices Indices of the field, as integers.
     * \param t_point Space-time point of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(std::vector<int> const& indices,
                    const csl::Tensor&     t_point) override;

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This function takes multiple indices (may be only one) in a 
     * std::vector. This may correspond to a field like \f$ A^{\mu I}(X) \f$ .
     * \param indices Indices of the field.
     * \param t_point Space-time point of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(std::vector<csl::Index> indices,
                    const csl::Tensor&     t_point) override;

    /*!
     * \brief Generates an instance of the PolarizationField as a symbolic 
     * expression.
     * \param polarization Polarization index for the field.
     * \param indices      Indices of the field, as integers.
     * \param t_point      Space-time point of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(csl::Index              polarization,
                    std::vector<int> const& indices,
                    const csl::Tensor&     t_point);

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This functions takes no csl::Index. The field is then for sure
     * a ScalarBoson in a trivial Gauge representation. Example: \f$\phi(X)\f$.
     * The space_time point is taken as default: see defaultSpaceTimePoint.
     * \param t_point Space_time point of the scalar field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()();

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This functions takes one csl::Index. This may be for example
     * a field like \f$ A^\mu(X) \f$.
     * The space_time point is taken as default: see defaultSpaceTimePoint.
     * \param index   Unique index of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(csl::Index index) override;

    /*!
     * \brief Generates an instance of the field as a symbolic expression.
     * \details This function takes multiple indices (may be only one) in a 
     * std::vector. This may correspond to a field like \f$ A^{\mu I}(X) \f$ .
     * The space_time point is taken as default: see defaultSpaceTimePoint.
     * \param indices Indices of the field.
     * \return An expression, instance of the field.
     * \sa QuantumField.
     */
    csl::Expr operator()(std::vector<csl::Index> indices) override;

    /*!
     * \brief Generates an instance of the polarization field as a symbolic
     * expression.
     * \details The first index must be the spin or polarization of the field.
     * Then follow the rest of the indices, that can be unique or in a 
     * std::vector. For example, one may create the polarization tensor of a 
     * vector boson \f$ \epsilon ^\lambda_{\mu}(p) \f$ with the following code
     * \code 
     *     Index lambda = ...;
     *     Index mu = ...;
     *     Tensor p = ...;
     *     Particle A = vectorboson_s(...)
     *     csl::Expr epsilon = A(lambda, mu, p);
     * \endcode
     * \return An expression, instance of the field.
     * \sa PolarizationField.
     */
    csl::Expr operator()(csl::Index          polarization,
                    csl::Index          index,
                    const csl::Tensor& momentum);

    /*!
     * \brief Generates an instance of the polarization field as a symbolic
     * expression.
     * \details The first index must be the spin or polarization of the field.
     * Then follow the rest of the indices, that can be unique or in a 
     * std::vector. For example, one may create the polarization tensor of a 
     * vector boson \f$ \epsilon ^\lambda_{\mu I}(p) \f$ with the following code
     * \code 
     *     Index lambda = ...;
     *     Index mu = ...;
     *     Index I = ...;
     *     Tensor p = ...;
     *     Particle A = vectorboson_s(...)
     *     csl::Expr epsilon = A(lambda, {mu, I}, p);
     * \endcode
     * \return An expression, instance of the field.
     * \sa PolarizationField.
     */
    csl::Expr operator()(csl::Index              polarization,
                    std::vector<csl::Index> indices,
                    const csl::Tensor&     momentum);

    operator csl::Expr();

    void breakParticle(
            mty::Group                     *brokenGroup,
            std::vector<std::string> const &newNames
            );
    void breakParticle(
            mty::FlavorGroup                     *brokenFlavor,
            std::vector<mty::FlavorGroup*> const &subGroups,
            std::vector<std::string>       const &names
            );

    protected:

    void addRelative(std::weak_ptr<mty::QuantumFieldParent> const &particle);

    std::weak_ptr<mty::QuantumFieldParent> removeRelative(
            QuantumFieldParent const *particle
            );

    void checkRelatives();

    template<class Func>
    void applyToRelatives(Func &&func);

    void printQuantumDefinition(
            std::ostream &out,
            int           indentSize
            ) const;

    /*!
     * \brief Initializes default propagators for the particle. This function is
     * called automatically in the constructor of derived classes.
     * \sa Propagator_func, ScalarBoson, WeylFermion, DiracFermion, VectorBoson.
     */
    virtual void initPropagator();

    /*!
     * \brief Initializes firstGaugeIndex and firstSpaceIndex. Automatically
     * called by the constructor.
     * \sa #firstSpaceIndex, #firstGaugeIndex.
     */
    void initIndexSpaces();

    /*!
     * \brief Initializes the polarization sum for PolarizationField arising
     * in amplitudes.
     * \sa #polarizationSumInitialized, PolarizationField.
     */
    void initPolarizationSum();

    /*!
     * \brief Changes the propagator of a particle with another. 
     * \details This function is not callable by the user. It is called instead 
     * by AddCustomPropagator() to avoid multiple definitions of propagators. 
     * \param field      Other field in the propagator.
     * \param propagator New propagator.
     * \sa #propagator, AddCustomPropagator().
     */
    void setPropagator(QuantumFieldParent const* other,
                       Propagator_func prop);

    /*!
     * \brief Changes the propagator of a particle with another. 
     * \details The propagator should of course be consistent, but may be as 
     * exotic as possible. See Propagator_func to know the exact structure of
     * what must be given. This function may be useful for example to define the
     * propagator between a left-handed and a right-handed fermion (different
     * particles), or between a vector boson and its field strength... 
     * \param field      Left field in the propagator.
     * \param field      Right field in the propagator.
     * \param propagator New propagator.
     * \warning In the case of a non symmetric propagator, the user should take
     * into account that in its custom function, the left field will not always
     * be the first argument. The only thing that is ensured is that if there is
     * a complex conjugated field, it will be the second argument.
     * \sa QuantumFieldParent::propagator, Propagator_func, 
     * quantumFieldTheory.h.
     */
    friend
    void AddCustomPropagator(QuantumFieldParent* left,
                             QuantumFieldParent* right,
                             QuantumFieldParent::Propagator_func propagator);

    /*!
     * \brief Changes the propagator of a particle with itself. 
     * \details The propagator should of course be consistent, but may be as 
     * exotic as possible. See Propagator_func to know the exact structure of
     * what must be given.
     * \param field      Field whose propagator is modified.
     * \param propagator New propagator.
     * \sa QuantumFieldParent::propagator, Propagator_func, 
     * quantumFieldTheory.h.
     */
    friend
    void AddCustomPropagator(QuantumFieldParent* field,
                             QuantumFieldParent::Propagator_func propagator);

};

/*!
 * \brief This class inherits from std::shared_ptr<QuantumFieldParent> and 
 * should be used instead of direct QuantumFieldParent.
 * \details Particle is the object to use to create particles and their 
 * instances. As for csl::Expr, the ownership of a QuantumFieldParent is shared 
 * (in particular between the model and the user). One must then use a pointer,
 * and in this case a shared pointer. These objects are created by built-in 
 * functions and may be used using '->' instead of '.'. For example, building a
 * scalar boson, setting its mass and taking an instance gives:
 * \code 
 *     Tensor X = MinkowskiVector("X");
 *     Particle phi = scalarboson_s("\\phi");
 *     phi->setMass("m");
 *     csl::Expr instance = phi(X);
 * \endcode
 * The operator() are redefined in this class in order to use them directly on
 * a particle (otherwise we would have to write \code csl::Expr instance = (*phi)(X)
 * \endcode which is not very practical). Particle is just managing tho 
 * ownership of a QuantumFieldParent without building it (this is done by 
 * built-in functions as already said).
 * \sa scalarboson_s(), weylfermion_s(), diracfermion_s(), vectorboson_s(),
 * gaugeboson_s(), goldstoneboson_s(), ghostboson_s().
 */
class Particle: public std::shared_ptr<QuantumFieldParent> {

    public:

    INHERIT_SHARED_PTR_CONSTRUCTOR(Particle, mty::QuantumFieldParent)

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param args Variadic function arguments.
     * \return The corresponding instance of QuantumField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(Args&& ...args)
        {
            return (**this)(std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param polar Polarization index for the polarization tensor.
     * \param index Other index.
     * \param args  Variadic function arguments.
     * \return The corresponding instance of PolarizationField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const csl::Index& polar,
                        const csl::Index& index,\
                        Args&&               ...args)
        {
            return (**this)(polar, index, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param indices Indices as integer values.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of QuantumField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const std::vector<int>& indices,\
                        Args&&                     ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param indices Indices as integer values.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of QuantumField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(std::initializer_list<int> indices,\
                        Args&&                     ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param polar   Polarization index for the polarization tensor.
     * \param indices Rest of the indices, as integer values.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of PolarizationField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const csl::Index&              polar,
                        const std::vector<int>& indices,\
                        Args&&                     ...args)
        {
            return (**this)(polar, indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param polar   Polarization index for the polarization tensor.
     * \param indices Rest of the indices.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of PolarizationField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const csl::Index&              polar,
                        const std::vector<csl::Index>& indices,\
                        Args&&                     ...args)
        {
            return (**this)(polar, indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param polar   Polarization index for the polarization tensor.
     * \param indices Rest of the indices, as integer values.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of PolarizationField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const csl::Index&              polar,
                        std::initializer_list<int> indices,\
                        Args&&                     ...args)
        {
            return (**this)(polar, indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param polar   Polarization index for the polarization tensor.
     * \param indices Rest of the indices.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of PolarizationField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const csl::Index&              polar,
                        std::initializer_list<csl::Index> indices,\
                        Args&&                     ...args)
        {
            return (**this)(polar, indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param indices Rest of the indices.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of QuantumField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(const std::vector<csl::Index>& indices,\
                        Args&&                     ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    /*!
     * \brief Delegates the arguments to QuantumFieldParent::operator()() of the
     * managed object.
     * \tparam Args Variadic template parameters.
     * \param indices Rest of the indices.
     * \param args    Variadic function arguments.
     * \return The corresponding instance of QuantumField.
     * \sa QuantumFieldParent::operator()().
     */
    template<class ...Args>
        inline
        csl::Expr operator()(std::initializer_list<csl::Index> indices,\
                        Args&&                     ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    operator csl::Expr() 
    {
        return csl::Expr(**this);
    }
};

/*!
 * \brief Class deriving from csl::TensorFieldElement, may enter expressions. In 
 * contrary to QuantumFieldParent, it is not a inherited from any class.
 * \details Any quantum field (boson, fermion, gauge ...) will have QuantumField
 * instances. This simplifies the use of a generic instance, QuantumField, that 
 * will ask its parent to know its own properties (contractions, commutation, 
 * spin, mass, ...). All physical information is contained in QuantumFieldParent 
 * and its derived classes, except the few attributes of QuantumField that are 
 * instance-specific.
 * QuantumField objects will not be in general directly created by the user. 
 * Giving a QuantumFieldParent or a Particle, one should used their operator()()
 * to create a QuantumField instance, as an actual expression that may enter in
 * mathematical expressions.
 * \sa QuantumFieldParent, Particle, csl::Expr, TensorFieldElement, Particle::operator()().
 */
class QuantumField: public csl::TensorFieldElement {

    protected:

    /*!
     * \brief Tells if the QuantumField, if external, is a particle or 
     * an anti-particle.
     * \details As for the attribute \b incoming, this boolean will determine
     * if the field must be complex conjugated or not. Depending on its spin,
     * if it is an incoming particle, outgoing anti-particle...
     * \sa isIncomingParticle(), isIncomingAntiParticle(), isOutgoingParticle(),
     * isOutgoingAntiParticle(), setIncoming(), setExternal(), setParticle().
     */
    bool particle = true;

    /*!
     * \brief Tells if the field is external (initial of final state in 
     * computations) or not.
     * \sa isExternal(), setExternal(), isIncomingParticle(), 
     * isIncomingAntiParticle(), isOutgoingParticle(), isOutgoingAntiParticle(),
     * setIncoming(), setExternal(), setParticle().
     */
    bool external = false;

    bool onShell = false;

    /*!
     * \brief Tells if the QuantumField, if external, is a incoming or outgoing.
     * \details As for the attribute \b particle, this boolean will determine
     * if the field must be complex conjugated or not. Depending on its spin,
     * if it is an incoming particle, outgoing anti-particle...
     * \sa isIncomingParticle(), isIncomingAntiParticle(), isOutgoingParticle(),
     * isOutgoingAntiParticle(), setIncoming(), setExternal(), setParticle().
     */
    bool incoming = true;

    /*!
     * \brief csl::IndexStructure allowing to store derivative indices, to keep
     * track of terms in \f$ \partial _\mu \f$ in computations while having only
     * QuantumField objects.
     * \details This member allows to have an internal representation of the 
     * following definition for a field \f$ A_\mu ^I \f$:
     * \f[
     *     \partial _\mu A_\nu ^I \equiv A^I_{\nu,\mu}.
     * \f]
     * The derivative disappear and indices are transfered in the structure of 
     * the field.
     * \sa addDerivative(), hasDerivative(), getDerivativeStructure(),
     * wick::Graph, wick::WickCalculator, Expander, AmplitudeCalculator.
     */
    csl::IndexStructure derivativeIndices;

    /**
     * @brief Wanted partner for fermion ordering in amplitude results, together
     * with the number defining the fermion.
     *
     * @details Fermion ordering is determined outside (possibly by the user)
     * and the information is stored in the quantum field itself to know what
     * fermions must be paired together during simplifications. If fermions are
     * not paired accordingly to the defined order (if there is one), Fierz 
     * identities are applied to correct the amplitude, like in the following 
     * with external fermions notes as 1,2,3,4 and \f$ \Gamma^A \f$ are chains
     * of \f$ \gamma\f$-matrices:
     * \f[
     *    \left(\Gamma^A\right)_{14}\left(\Gamma^B\right)_{32} = \sum _{C,D}
     *    C^{AB}_{CD}\left(\Gamma^C\right)_{12}\left(\Gamma^D\right)_{34},
     * \f]
     * where \f$ C^{AB}_{CD} \f$ are scalar coefficients determined by the 
     * generalized Fierz identities.
     */
    PartnerShip partnerShip;

    public:

    /*!
     * \brief Constructor with two parameters.
     * \details This constructor takes no index and concerns then scalar fields.
     * \param t_vector Space_time point of the field.
     * \param t_parent Parent (must be derived from QuantumFieldParent) of the
     * field.
     */
    QuantumField(const csl::Tensor& t_vector,
                 const csl::Parent&  t_parent);

    /*!
     * \brief Constructor with three parameters.
     * \param t_vector Space_time point of the field.
     * \param t_parent Parent (must be derived from QuantumFieldParent) of the
     * field.
     * \param indices  Set of indices of the tensor.
     */
    QuantumField(const csl::Tensor& t_vector,
                 const csl::Parent&  t_parent,
                 const std::vector<csl::Index>& indices);

    /*!
     * \brief Constructor with three parameters.
     * \param t_vector Space_time point of the field.
     * \param t_parent Parent (must be derived from QuantumFieldParent) of the
     * field.
     * \param indices  Set of indices of the tensor.
     */
    QuantumField(const csl::Tensor& t_vector,
                 const csl::Parent&  t_parent,
                 const csl::IndexStructure& indices);

    public:

    template<typename ...Args>
    friend csl::Expr quantumfield_s(Args&& ...args);

    /*!
     * \brief Default constructor. Initializes no parent and no space-time 
     * point. 
     * \details May be useful in some situations but keep in mind that a non
     * initialized QuantumField is an invalid state and may cause problems.
     */
    QuantumField();

    /*!
     * \brief Default Copy constructor.
     */
    QuantumField(QuantumField const&) = default;

    /*!
     * \brief Default Move constructor.
     */
    QuantumField(QuantumField&&) = default;

    /*!
     * \brief Default copy assignement operator.
     */
    QuantumField& operator=(const QuantumField& other) = default;

    /*!
     * \brief Default move assignement operator.
     */
    QuantumField& operator=(QuantumField&&) = default;

    /*!
     * \brief Returns a pointer to the QuantumFieldParent of the field.
     * \details As QuantumField inherits from csl::TensorFieldElement, its parent is
     * in the form of a csl::Parent. This function then makes a conversion to 
     * a mty::QuantumFieldParent before returning the pointer.
     * \return A pointer to the QuantumFieldParent of the field.
     */
    inline
    QuantumFieldParent* getQuantumParent() const {
        return static_cast<QuantumFieldParent*>(parent.get());
    }

    /*!
     * \brief Returns a mty::Particle, pointer to the parent of the field.
     * \details As QuantumField inherits from csl::TensorFieldElement, its parent is
     * in the form of a csl::Parent. This function then makes a conversion to 
     * a mty::Particle before returning the pointer.
     * \return A shared pointer to the QuantumFieldParent (i.e. mty::Particle) 
     * of the field.
     */
    inline 
    Particle getParticle() const {
        return std::static_pointer_cast<QuantumFieldParent>(parent);
    }

    auto getGaugeIrrep() const {
        auto rep = getQuantumParent()->getGaugeIrrep();
        return (conjugated) ? rep.getConjugatedRep() : rep;
    }
    auto getFlavorIrrep() const {
        auto rep = getQuantumParent()->getFlavorIrrep();
        return rep;
    }
    template<class GroupType>
    auto getGroupIrrep(GroupType &&group) const {
        auto rep = getQuantumParent()->getGroupIrrep(std::forward<GroupType>(group));
        return (conjugated) ? rep.getConjugatedRep() : rep;
    }
    template<class FlavorGroupType>
    auto getFlavorIrrep(FlavorGroupType &&group) const {
        auto rep = getQuantumParent()->getFlavorIrrep(std::forward<FlavorGroupType>(group));
        return rep;
    }

    /*!
     * \brief See QuantumFieldParent::getSpinDimension().
     * \return Twice the spin of the particle.
     */
    int getSpinDimension() const;

    /*!
     * \brief See QuantumFieldParent::getEnergyDimension().
     * \return The energy dimension of the particle.
     */
    csl::Expr getEnergyDimension() const;

    /*!
     * \brief See QuantumFieldParent::getMass().
     * \return The mass of the particle.
     */
    csl::Expr getMass() const;

    /*!
     * \brief See QuantumFieldParent::getWidth().
     * \return The width of the particle.
     */
    csl::Expr getWidth() const;

    PartnerShip getPartnerShip() const {
        return partnerShip;
    }

    /*!
     * \brief See QuantumFieldParent::isChiral().
     * \return \b True  if the particle is chiral.
     * \return \b False else.
     */
    bool isChiral() const;

    /*!
     * \brief See QuantumFieldParent::getChirality().
     * \return The chirality of the particle.
     */
    Chirality getChirality() const;

    /*!
     * \brief Sets the chirality of the particle.
     * \details This function should be called wisely. If the chirality is not
     * consistent, it will raise an error. In particular, it should be used only
     * for fermions that are linked (one DiracFermion and two WeylFermion). In 
     * this case, setting the chirality of the particle is equivalent to change
     * its parent. \b chirality may be left or right (a WeylFermion takes the 
     * place of the parent) or none (a DiracFermion takes the place of the 
     * parent).
     * \param chirality New chirality of the particle.
     * \sa QuantumFieldParent::getDiracParent(), 
     * QuantumFieldParent::getWeylFermion(),
     * QuantumFieldParent::getChiralityCounterPart(),
     * setParent().
     */
    void setChirality(Chirality chirality);

    /*!
     * \brief See QuantumFieldParent::isGaugeBoson().
     * \return \b True  if the particle is a gauge boson.
     * \return \b False else.
     */
    bool isGaugeBoson() const;

    /*!
     * \brief See QuantumFieldParent::getXiGauge().
     * \return The expression of the \f$ \xi \f$ parameter of a gauge boson.
     */
    csl::Expr getXiGauge() const;

    /*!
     * \brief Returns the mass squared of the field.
     * \details Equivalent to pow_s(getMass(), 2).
     * \return The squared mass of the particle.
     */
    csl::Expr getSquaredMass() const;

    /*!
     * \brief Tells if the field is complex conjugated.
     * \return \b True  if the field is complex conjugated.
     * \return \b False else.
     * \sa Complexified::conjugated.
     */
    bool getConjugated() const;

    /*!
     * \brief Tells if the field is an external field.
     * \return \b True  if the field is external.
     * \return \b False else.
     * \sa #external, setExternal(), Insertion.
     */
    bool isExternal() const;

    bool isOnShell() const;

    /*!
     * \brief See QuantumFieldParent::isBosonic().
     * \return \b True  if the field is bosonic.
     * \return \b False else.
     */
    bool isBosonic() const;

    /*!
     * \brief See QuantumFieldParent::isFermionic().
     * \return \b True  if the field is fermionic.
     * \return \b False else.
     */
    bool isFermionic() const;

    /*!
     * \brief See QuantumFieldParent::isAntiCommuting().
     * \return \b True  if the field is anti-commuting (fermion or ghost).
     * \return \b False else.
     */
    bool isAntiCommuting() const;

    /*!
     * \brief See QuantumFieldParent::isSelfConjugate().
     * \return \b True  if the field is real.
     * \return \b False else.
     */
    bool isSelfConjugate() const;

    bool isReal() const override;

    /*!
     * \brief Tells if the field is physical, i.e. may appear in external legs.
     * \details If the field is not physical, it will be forbidden in external
     * legs. In particular, the function setExternal() will yield an error. For
     * example, ghosts and goldstone bosons are not physical.
     * \sa #physical, isExternal(), setExternal(), setPhysical(), Insertion.
     * \return \b True  if the field is physical.
     * \return \b False else.
     */
    bool isPhysical() const;

    /*!
     * \brief See QuantumFieldParent::isContractibleWith().
     * \param other Field to test if there is a contraction.
     * \return \b True  if the field is contractible with the other field.
     * \return \b False else.
     */
    bool isContractibleWith(const QuantumField& other) const;

    /*!
     * \brief Tests if the the field is exactly contractible with the other one.
     * \details The condition that does not appear in isContractibleWith() is 
     * the complex conjugation of the field. In the first function, we test only
     * that a propagator exists between the two abstract fields. Here, we test 
     * also if either the field is self-conjugate or one an only one of the two 
     * fields is complex conjugated. Other wise, even if a propagator exists, 
     * the contraction gives zero and this function returns false.
     * \param other Field with which we test the contraction.
     * \return \b True  if the actual contraction of the two fields is not zero.
     * \return \b False else.
     */
    bool isExactlyContractiblewith(const QuantumField& other) const;

    /*!
     * \brief Adds a derivative index to the the field.
     * \param index Derivative index to add.
     * \sa #derivativeIndices, hasDerivative(), getDerivativeStructure(),
     * setDerivativeStructure().
     */
    void addDerivative(const csl::Index& index);

    /*!
     * \brief Tells if the field has derivative indices.
     * \return \b True  if the derivative structure #derivativeIndices is not 
     * empty.
     * \return \b False else.
     * \sa #derivativeIndices, addDerivative(), getDerivativeStructure(),
     * setDerivativeStructure().
     */
    bool hasDerivative() const;

    /*!
     * \brief Returns the derivative structure of the field.
     * \return The derivative structure of the field.
     * \sa #derivativeIndices, addDerivative(), hasDerivative(),
     * setDerivativeStructure().
     */
    csl::IndexStructure getDerivativeStructure() const;

    /*!
     * \brief Returns the propagator with another QuantumField.
     * \details This function checks if the propagator is non null with 
     * isExactlyContractiblewith(). Then, it ensure that if the field is not 
     * self-conjugate the conjugated field is on the right of the contraction
     * (and adds a minus sign if the field is anti-commuting).
     * \param other  Other field in the contraction.
     * \param vertex Momentum integrated in the propagator.
     * \return The expression of the propagator.
     * \sa isExactlyContractiblewith(), isAntiCommuting(), isSelfConjugate(),
     * QuantumFieldParent::getPropagator(), 
     * QuantumFieldParent::getInvPropagator(), ,
     * QuantumFieldParent::propagator, Propagator_func, 
     * quantumFieldTheory.h.
     */
    csl::Expr getPropagator(const QuantumField& other,
                       csl::Tensor&       vertex) const;

    /*!
     * \brief Returns the expression of the LSZ insertion corresponding to the 
     * field.
     * \param momentum Momentum of the insertion.
     * \param ruleMode Boolean that is true if we want to determine the 
     * feynman rule (in this case the insertion is not the same).
     * \return The expressoin of the LSZ insertion.
     */
    csl::Expr getLSZInsertion(
            const csl::Tensor& momentum,
            bool               ruleMode = false,
            bool               lock = false
            ) const;

    /*!
     * \brief Copies the field in a new new one and conjugates it before 
     * returning it.
     * \return The conjugated field.
     * \sa conjugate().
     */
    QuantumField getConjugatedField() const;

    csl::Expr contraction(csl::Expr_info other) const override;

    virtual csl::Expr matrixChargeConjugation(csl::Expr_info other) const;

    /*!
     * \brief Returns the complex conjugate expression of the field.
     * \return The complex conjugate expression of the field.
     * \sa conjugate(), getConjugatedField().
     */
    std::optional<csl::Expr> getComplexConjugate() const override;

    /*!
     * \brief Conjugates the field and changes its particle number.
     * \details For example, a particle \f$ \phi (X) \f$ as insertion is an 
     * outgoing particle (otherwise it would be complex conjugated). Conjugating
     * the particle, we get \f$ \phi ^*(X) \f$ an outgoing antiparticle.
     * \sa #particle, getConjugatedField().
     */
    void conjugate();

    /*!
     * \brief Sets the external property of the particle.
     * \param t_external Must be true if the field is an external leg, false
     * otherwise.
     * \sa #external, isExternal().
     */
    void setExternal(bool t_external);

    void setOnShell(bool t_onShell);

    /*!
     * \brief Sets the incoming property of the particle.
     * \param t_incoming Must be true if the field is incoming, false
     * otherwise.
     * \sa #incoming, isIncoming().
     */
    virtual void setIncoming(bool t_incoming);

    /*!
     * \brief Sets the particle property of the particle.
     * \param t_particle Must be true if the field is a particle, false
     * otherwise.
     * \sa #particle, isParticle().
     */
    virtual void setParticle(bool t_particle);

    void setPartnerShip(PartnerShip const &t_partnerShip) {
        partnerShip = t_partnerShip;
    }

    /*!
     * \brief Sets the derivative structure of the field.
     * \param structure New derivative structure.
     * \sa #derivativeIndices, getDerivativeStructure().
     */
    void setDerivativeStructure(const csl::IndexStructure& structure);

    /*!
     * \brief Tells if the field is a commutable object in products.
     * \return \b True  if the field is not anti commuting.
     * \return \b False else.
     * \sa #bosonic, isBosonic(), isFermionic(), isAntiCommuting().
     */
    bool getCommutable() const override;

    /*!
     * \brief Tells if the field commutes with another expression.
     * \details This function assumes only that for two anti-commuting fields
     * \f$ \psi _1 \f$ and \f$ \psi _2 \f$ we have \f$\{\psi _1, \psi _2\}=0\f$.
     * This function returns in this case true iif \b sign = +1 
     * (anticommutation). For all other cases, this function returns true if 
     * \b sign = -1. In particular, beware that anti-commuting fields embedded 
     * in more complicated expressions are not taken into account for now.
     * This means that \f$ \{\psi_ 1, \psi _2+\psi _3\}\neq 0 \f$
     * \param other Expression with which we test the commutation.
     * \param sign  Sign of the commutation. +1 means anticommutation, -1 
     * (default) means commutation.
     * \return \b True  if the field commutes with \b other.
     * \return \b False else.
     */
    bool commutesWith(csl::Expr_info other, int sign=-1) const override;

    /*!
     * \brief Tells if the field has an indicial contraction property with 
     * another expression.
     * \details This functoin overrides the one of csl::TensorFieldElement and make 
     * sure to return false each time.
     * \param expr Other expression.
     * \return \b False.
     */
    bool hasContractionProperty(csl::Expr_info expr) const override;

    /*!
     * \brief Returns a copy of the field in a std::unique_ptr.
     * \return A std::unique_ptr<Abstract>, copy of the field.
     */
    csl::unique_Expr copy_unique() const override;

    /*!
     * \brief Tells if the field is indexed.
     * \return \b True  if the field has proper indices or derivative indices.
     * \return \b False else.
     */
    bool isIndexed() const override;

    /*!
     * \brief Compares the field with another expression.
     * \details Uses TensorFieldElement::operator==() and takes care of comparing 
     * the derivative structures of two fields if the other expression is 
     * another field (otherwise this function returns false).
     * \param other Expression to compare.
     * \return \b True  if the other expression is the same field, with the same
     * structure and space-time point.
     * \return \b False else.
     * \sa TensorFieldElement::operator==(), #derivativeIndices.
     */
    bool operator==(csl::Expr_info other) const override;

    /*!
     * \brief Compares the field with another expression.
     * \details Uses TensorFieldElement::compareWithDummy() and takes care of comparing 
     * the derivative structures of two fields if the other expression is 
     * another field (otherwise this function returns false). Dummy indices may
     * compare succesfully with each other with different names. Only the 
     * abstract index structure is compared. Free indices must match exactly.
     * \param other              Expression to compare.
     * \param constraints        Mapping constraints between already compared
     * dummy indices.
     * \param keepAllCosntraints Tell if constraints must be relaxed once pairs
     * of indices are matched (default = false, the user should not need that).
     * \return \b True  if the other expression is the same field, with the same
     * structure (allowing different dummy indices) and space-time point.
     * \return \b False else.
     * \sa TensorFieldElement::compareWithDummy(), #derivativeIndices.
     */
    bool compareWithDummy(csl::Expr_info other,
                          std::map<csl::Index, csl::Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    /*!
     * \brief Compares the simplicity of the field with another.
     * \param other Other field.
     * \return \b True  if the field is simpler than \b other.
     * \return \b False else.
     * \sa TensorFieldElement::operator<().
     */
    bool operator<(const QuantumField& other) const;

    /*!
     * \brief Compares the field with another.
     * \param other Other field.
     * \return \b True  if the field is equal to \b other.
     * \return \b False else.
     * \sa operator==(csl::Expr_info).
     */
    bool operator==(const QuantumField& other) const;

    /*!
     * \brief Compares the field with another.
     * \param other Other field.
     * \return \b True  if the field is equal to \b other.
     * \return \b False else.
     * \sa operator==(csl::Expr_info).
     */
    bool operator!=(const QuantumField& other) const;

    /*!
     * \brief Tells if the (external) field is a particle or anti particle.
     * \return \b True  if the field is a particle.
     * \return \b False else.
     * \sa #particle, setParticle(), isIncoming(), isIncomingParticle(),
     * isIncomingAntiParticle(), isOutgoingParticle(), isOutgoingAntiParticle().
     */
    bool isParticle() const;

    /*!
     * \brief Tells if the (external) field is incoming or outgoing.
     * \return \b True  if the field is incoming.
     * \return \b False else.
     * \sa #incoming, setIncoming(), isIncoming(), isIncomingParticle(),
     * isIncomingAntiParticle(), isOutgoingParticle(), isOutgoingAntiParticle().
     */
    bool isIncoming() const;

    /*!
     * \brief Tells if the (external) field is an incoming particle.
     * \return \b True  if the field is incoming and is a particle.
     * \return \b False else.
     * \sa isIncoming(), isParticle(), isIncomingAntiParticle(), 
     * isOutgoingParticle(), isOutgoingAntiParticle().
     */
    bool isIncomingParticle() const;

    /*!
     * \brief Tells if the (external) field is an outgoing particle.
     * \return \b True  if the field is outgoing and is a particle.
     * \return \b False else.
     * \sa isIncoming(), isParticle(), isIncomingAntiParticle(), 
     * isIncomingParticle(), isOutgoingAntiParticle().
     */
    bool isOutgoingParticle() const;

    /*!
     * \brief Tells if the (external) field is an incoming anti-particle.
     * \return \b True  if the field is incoming and is an anti-particle.
     * \return \b False else.
     * \sa isIncoming(), isParticle(), isIncomingParticle(), 
     * isOutgoingParticle(), isOutgoingAntiParticle().
     */
    bool isIncomingAntiParticle() const;

    /*!
     * \brief Tells if the (external) field is an outgoing an anti-particle.
     * \return \b True  if the field is outgoing and is an anti-particle.
     * \return \b False else.
     * \sa isIncoming(), isParticle(), isIncomingAntiParticle(), 
     * isIncomingParticle(), isOutgoingParticle().
     */
    bool isOutgoingAntiParticle() const;
};

template<class Func>
void QuantumFieldParent::applyToRelatives(Func &&func) {
    checkRelatives();
    for (auto &rel : relatives) {
        // Making sure to avoid any loop dependency here
        auto shared = rel.lock();
        auto self = shared->removeRelative(this);
        func(mty::Particle(shared));
        if (self.lock())
            shared->addRelative(self);
    }
}

/*!
 * \brief Creates and returns an expression corresponding to a QuantumField.
 * \tparam Args Variadic template parameters.
 * \param args Variadic function parameters, may be anything. Forwarded to 
 * csl::make_shared().
 * \return The expression of the field.
 */
template<typename ...Args>
csl::Expr quantumfield_s(Args&& ...args)
{
    auto field = csl::make_shared<QuantumField>(std::forward<Args>(args)...);
    field->getCanonicalPermutation();
    return field;
}

/*!
 * \brief Creates and returns the sum of momentums in a process, taking into 
 * account incoming or outgoing particles.std
 * \details This sum is the one that must be zero if external states are 
 * on-shell. In particular, any calculation must yield something like
 * \f[
 *      i\mathcal{T} = (2\pi)^4\delta^{(4)}\left(\sum _{i_{in}}p_i
 *      -\sum _{j_{out}}p_j\right)\cdot i\mathcal{M},
 * \f]
 * where \f$ i\mathcal{M} \f$ is the physical amplitude of interest, 
 * \f$ p_i \f$ represent momentums of incoming particles, \f$ p_j \f$ of 
 * outgoing ones. Each component of the momentum sum must vanish.
 * \param insertions Field insertions of the process.
 * \param momentum   Momentum in the process (must be the same size as \b 
 * insertions).
 * \param index      Index for the momentum sum.
 * \return The expression of the momentum sum.
 */
csl::Expr getMomentumSum(const std::vector<QuantumField>&  insertions,
                     const std::vector<csl::Tensor>& impulsions,
                     const csl::Index&                index);

/*!
 * \brief Overload of operator<< for QuantumField. Calls QuantumField::print().
 * \param fout  Output flux.
 * \param field QuantumField to display.
 * \return A reference to \b fout.
 * \sa QuantumField::print().
 */
std::ostream& operator<<(std::ostream& fout,
                         const mty::QuantumField& field);


} // End of namespace mty

#endif /* QUANTUMFIELD_H */
