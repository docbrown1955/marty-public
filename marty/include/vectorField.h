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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * @brief Contains vector particles of MARTY: vector and gauge bosons, and the
 * field strength object.
 */
#ifndef VECTOR_FIELD_H_INCLUDED
#define VECTOR_FIELD_H_INCLUDED

#include "quantumField.h"

namespace mty {

class FieldStrength;
class Model;

/**
 * @brief Base class for vector objects.
 */
class BaseVectorBoson: public QuantumFieldParent {

    public: 

    /**
     * @brief Default constructor. Initializes nothing.
     */
    BaseVectorBoson() = default;

    /**
     * @brief Default copy constructor
     *
     * @param other Vector boson to copy
     */
    BaseVectorBoson(BaseVectorBoson const& other) = default;

    /**
     * @brief Constructor with two parameters.
     *
     * @param t_name  Name of the boson.
     * @param t_gauge Gauge in which the particle lives.
     */
    BaseVectorBoson(const std::string& t_name,
                    Gauge*             t_gauge);

    /**
     * @brief Constructor with two parameters.
     *
     * @param t_name  Name of the vector boson.
     * @param t_model Model in which the boson lives.
     */
    BaseVectorBoson(const std::string& t_name,
                    mty::Model       & model);

    /**
     * @brief Constructor with two parameters.
     *
     * @brief Contrary to some other constructors, this one initializes directly
     * the gauge representation to \b irrep.
     *
     * @param t_name Name of the boson.
     * @param irrep  Irreducible gauge representation.
     */
    BaseVectorBoson(const std::string& t_name,
                    const GaugeIrrep&  irrep);

    /**
     * @brief Constructor with three parameters.
     *
     * @param t_name            Name of the vector boson.
     * @param t_gauge           Gauge in which the boson lives.
     * @param t_isSelfConjugate Tells if the particle must be real.
     */
    BaseVectorBoson(const std::string& t_name,
                    Gauge*             t_gauge,
                    bool               t_isSelfConjugate);

    /**
     * @brief Constructor with three parameters.
     *
     * @brief Contrary to some other constructors, this one initializes directly
     * the gauge representation to \b irrep.
     *
     * @param t_name Name of the boson.
     * @param irrep  Irreducible gauge representation.
     * @param t_isSelfConjugate Tells if the particle must be real.
     */
    BaseVectorBoson(const std::string& t_name,
                    const GaugeIrrep&  irrep,
                    bool               t_isSelfConjugate);

    /**
     * @brief Constructor with two parameters.
     *
     * @brief Contrary to some other constructors, this one initializes directly
     * the gauge representation to \b irrep and flavor representation to 
     * \b flavorRep.
     *
     * @param t_name    Name of the boson.
     * @param irrep     Irreducible gauge representation.
     * @param flavorRep Flavor representation.
     */
    BaseVectorBoson(const std::string& t_name,
                    const GaugeIrrep&  irrep,
                    const FlavorIrrep& flavorRep);

    /**
     * @brief Copies another vector boson and takes a new name.
     *
     * @param t_name Name of the vector boson.
     * @param other  Other boson to copy.
     */
    BaseVectorBoson(const std::string& t_name,
                    const BaseVectorBoson* other);

    /**
     * @return 1, the energy dimension of a vector boson.
     */
    csl::Expr getEnergyDimension() const override;

    /**
     * @return 2 if the mass of the vector is 0 (2-dimensional spin).
     * @return 3 else.
     */
    int getNDegreesOfFreedom() const override;

    /**
     * @brief Calculates the kinetic term for the vector boson.
     *
     * @param X Space-time point for the Lagrangian term.
     *
     * @return Symbolic expression containing the kinetic term.
     *
     * @sa mty::VectorKineticTerm()
     */
    csl::Expr getKineticTerm(csl::Tensor & X) override;
};

/**
 * @brief Implementation of vector bosons in AMRTY.
 *
 * @details A vector boson comes with a FieldStrength each time that allows to
 * write interactions with vectors in an easier way, usin directly the Field 
 * strength for derivative couplings. Vector bosons can also be associated to
 * a GoldstoneBoson or a GhostBoson (automatic for non abelian gauged groups)
 * for gauge fixing procedures.
 */
class VectorBoson: public BaseVectorBoson {

    protected:

    /**
     * @brief Gauge choice for the vector, modifying also properties of the 
     * Ghost and Goldstone boson (if some are defined for the vector).
     */
    mty::gauge::Choice choice;

    /**
     * @brief Pointer to the GoldstoneBoson associated to the vector, if there 
     * is one.
     */
    std::shared_ptr<GoldstoneBoson> goldstone;
    /**
     * @brief Pointer to the GhostBoson associated to the vector, if there 
     * is one.
     */
    std::shared_ptr<GhostBoson>   ghost;
    /**
     * @brief Pointer to the FieldStrength associated to the vector.     
     */    
    std::shared_ptr<FieldStrength> fieldStrength;

    public:

    /**
     * @brief Delegates any arguments to the BaseVectorBoson::BaseVectorBoson()
     * constructors and creates the associated field strength.
     *
     * @tparam ...Args Variadic template argument types.
     * @param ...args  Parameter pack.
     */
    template<class ...Args>
    inline VectorBoson(Args&& ...args);

    bool isSameSpecies(QuantumFieldParent const *other) const override;

    bool contains(QuantumFieldParent const *other) const override {
        return isSameSpecies(other);
    }

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    ParticleType getParticleType() const override {
        return ParticleType::VectorBoson;
    }

    Particle generateSimilar(std::string const& t_name) const override;

    bool hasFieldStrength() const override;

    /**
     * @return The field strength of the vector. 
     *
     * @sa FieldStrength
     */
    Particle getFieldStrength() const override;

    /**
     * @return The \f$ \xi \f$ gauge fixing parameter.
     */
    csl::Expr getXiGauge() const override;

    /**
     * @return The gauge choice for the vector boson.
     */
    mty::gauge::Choice getGaugeChoice() const override;

    /**
     * @brief For a vector boson in a non abelian gauged group, returns the 
     * associated ghost boson.
     *
     * @return The ghost associated with the vector if there is one.
     * @return nullptr else.
     */
    Particle getGhost() const override;

    /**
     * @brief For a vector boson in a non abelian gauged group, returns the 
     * associated Goldstone boson.
     *
     * @return The Goldstone associated with the vector if there is one.
     * @return nullptr else.
     */
    Particle getGoldstone() const override;

    /**
     * @brief Associates the vector with a new Goldstone boson.
     *
     * @details The Goldstone should have the same representation (flavor and 
     * gauge) as the vector.
     *
     * @param goldstone Golstone boson to associate.
     */
    void setGoldstoneBoson(Particle const& goldstone) override;

    /**
     * @brief Associates the vector with a new ghost boson.
     *
     * @details The ghost should have the same representation (flavor and 
     * gauge) as the vector.
     *
     * @param goldstone Golstone boson to associate.
     */
    void setGhostBoson(Particle const& ghost) override;

    void setGaugeChoice(gauge::Type type) override;

    void setFieldStrength(Particle const& fieldStrength) override;

    void setBrokenParts(
        const csl::Space*            broken,
        const std::vector<Particle>& parts) override;

    /**
     * @brief Creates a vector or field strength expression depending on the
     * indices that are given.
     *
     * @details Generates \f$ A_\mu^{\{I\}} \f$ or \f$ F_{\mu\nu}^{\{I\}} \f$ 
     * depending on the number of Minkowski indices given. Flavor and gauge 
     * indices \f$ \{I\} \f$ are the same in both cases.
     *
     * @param indices Indices. If an additional Minkowski index is given, a 
     * field strength object is returned.
     * @param t_point Space-time point.
     *
     * @return The expression corresponding to the given indices.
     */
    csl::Expr operator()(std::vector<csl::Index> indices,
                    const csl::Tensor&     t_point) override;

    protected:

    void initPropagator() override;
};

class GoldstoneBoson;
class GhostBoson;
/**
 * @brief Gauge boson implementation in MARTY.
 *
 * @details A gauge boson is basically just a VectorBoson that also keeps a 
 * pointer to a mty::GaugedGroup from which it is the gauge boson.
 */
class GaugeBoson: public VectorBoson {

    private:

    /**
     * @brief Raw pointer to the gauged group associated with the gauge boson.
     */
    GaugedGroup* gaugedGroup;

    public:

    GaugeBoson(std::string const& t_name,
               GaugeIrrep  const& irrep,
               GaugedGroup* group);

    GaugeBoson(const std::string& t_name,
               GaugeBoson const* other);

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    GaugedGroup const* getGaugedGroup() const override;

    GaugedGroup* getGaugedGroup() override;

    Particle generateSimilar(std::string const& t_name) const override;

    bool isGaugeBoson() const override;
};

/**
 * @brief Field strength object in MARTY.
 *
 * @details This does not correspond to a new particle, simply another structure
 * for a vector boson in MARTY, with the field strength \f$ F \f$ defined from 
 * the initial vector boson \f$ A \f$ as
 * \f[
 *      F_{\mu\nu}^{\{I\}}\equiv  \partial_\mu A_\nu^{\{I\}}
 *      - \partial_\nu A_\nu^{\{I\}},
 * \f]
 * with \f$ \{I\} \f$ a generic notation including all flavor and gauge indices
 * for the initial field \f$ A \f$.
 */
class FieldStrength: public BaseVectorBoson {

    private:

    /**
     * @brief Raw pointer to the vector boson associated with the FieldStrength.
     */
    VectorBoson* vectorParent;

    public:

    /**
     * @brief Constructor with one parameter.
     *
     * @details The field strength name is automatically assigned to the same 
     * name as the vector boson.
     *
     * @param t_vectorParent The associated vector boson.
     */
    FieldStrength(VectorBoson* t_vectorParent);

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    bool isSameSpecies(QuantumFieldParent const *other) const override {
        return this == other 
            || vectorParent == other;
    }

    /**
     * @return ParticleType::FieldStrength
     */
    ParticleType getParticleType() const override {
        return ParticleType::FieldStrength;
    }

    VectorBoson* getVectorParent() const;

    Particle generateSimilar(std::string const& t_name) const override;

    csl::Expr getEnergyDimension() const override;
};

template<class ...Args>
inline VectorBoson::VectorBoson(Args&& ...args)
    :BaseVectorBoson(std::forward<Args>(args)...)
{
    fieldStrength = csl::make_shared<FieldStrength>(this);
    addRelative(fieldStrength);
    initPropagator();
}

/**
 * @brief Delegates the construction of a vector boson and returns the result.
 *
 * @details This function can take any argument that the constructors 
 * VectorBoson::VectorBoson() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created vector boson.
 */
template<class ...Args>
inline Particle vectorboson_s(Args&& ...args)
{
    return csl::make_shared<VectorBoson>(std::forward<Args>(args)...);
}

/**
 * @brief Delegates the construction of a gauge boson and returns the result.
 *
 * @details This function can take any argument that the constructors 
 * GaugeBoson::GaugeBoson() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created gauge boson.
 */
template<class ...Args>
inline Particle gaugeboson_s(Args&& ...args)
{
    return csl::make_shared<GaugeBoson>(std::forward<Args>(args)...);
}

/**
 * @brief Delegates the construction of a field strength and returns the result.
 *
 * @details This function can take any argument that the constructors 
 * FieldStrength::FieldStrength() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created field strength.
 */
template<class ...Args>
inline Particle fieldstrength_s(Args&& ...args)
{
    return csl::make_shared<FieldStrength>(std::forward<Args>(args)...);
}


}


#endif
