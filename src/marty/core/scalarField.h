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
 * @brief Contains the implementation of the Scalar boson in MARTY.
 * @sa ghostField.h for ghosts, goldstoneField.h for Golstone bosons.
 */
#ifndef SCALARFIELD_H_INCLUDED
#define SCALARFIELD_H_INCLUDED

#include "quantumField.h"

namespace mty {

class Model;

/**
 * @brief Scalar boson, spin 0 particles in MARTY.
 */
class ScalarBoson : public QuantumFieldParent {

  public:
    /**
     * @brief Default constructor. Initializes nothing.
     */
    ScalarBoson() = default;

    /**
     * @brief Default copy constructor.
     *
     * @param other Other scalar boson to copy.
     */
    ScalarBoson(ScalarBoson const &other) = default;

    /**
     * @brief Constructor with two parameters.
     *
     * @param t_name  Name of the scalar boson.
     * @param t_gauge Gauge in which the boson lives.
     */
    ScalarBoson(const std::string &t_name, Gauge *t_gauge);

    /**
     * @brief Constructor with two parameters.
     *
     * @param t_name  Name of the scalar boson.
     * @param t_model Model in which the boson lives.
     */
    ScalarBoson(const std::string &t_name, Model const &t_model);

    /**
     * @brief Constructor with two parameters.
     *
     * @brief Contrary to some other constructors, this one initializes
     * directly the gauge representation to \b irrep.
     *
     * @param t_name Name of the boson.
     * @param irrep  Irreducible gauge representation.
     */
    ScalarBoson(const std::string &t_name, const GaugeIrrep &irrep);

    /**
     * @brief Constructor with three parameters.
     *
     * @param t_name            Name of the scalar boson.
     * @param t_gauge           Gauge in which the boson lives.
     * @param t_isSelfConjugate Tells if the particle must be real.
     */
    ScalarBoson(const std::string &t_name,
                Gauge *            t_gauge,
                bool               t_isSelfConjugate);

    /**
     * @brief Constructor with three parameters.
     *
     * @brief Contrary to some other constructors, this one initializes
     * directly the gauge representation to \b irrep.
     *
     * @param t_name Name of the boson.
     * @param irrep  Irreducible gauge representation.
     * @param t_isSelfConjugate Tells if the particle must be real.
     */
    ScalarBoson(const std::string &t_name,
                const GaugeIrrep & irrep,
                bool               t_isSelfConjugate);

    /**
     * @brief Constructor with two parameters.
     *
     * @brief Contrary to some other constructors, this one initializes
     * directly the gauge representation to \b irrep and flavor representation
     * to \b flavorRep.
     *
     * @param t_name    Name of the boson.
     * @param irrep     Irreducible gauge representation.
     * @param flavorRep Flavor representation.
     */
    ScalarBoson(const std::string &t_name,
                const GaugeIrrep & irrep,
                const FlavorIrrep &flavorRep);

    /**
     * @brief Copies another scalar boson and takes a new name.
     *
     * @param t_name Name of the scalar boson.
     * @param other  Other boson to copy.
     */
    ScalarBoson(const std::string &t_name, const ScalarBoson *other);

    /**
     * @brief Prints the MARTY (C++) code defining the scalar boson.
     *
     * @details This function assumes that the vector boson has already been
     * defined.
     *
     * @param out        Output stream
     * @param indentSize Indent to use when printing instructions
     */
    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    /**
     * @return ParticleType::ScalarBoson
     */
    ParticleType getParticleType() const override
    {
        return ParticleType::ScalarBoson;
    }

    /**
     * @brief Generates an identical particle with only a different name.
     *
     * @param t_name Name of the new particles.
     *
     * @return The newly created scalar boson.
     */
    Particle generateSimilar(std::string const &t_name) const override;

    /**
     * @return 1 as the energy dimension of a scalar boson.
     */
    csl::Expr getEnergyDimension() const override;

    /**
     * @return 1 as the number of spin degrees of freedom of a scalar boson.
     */
    int getNDegreesOfFreedom() const override;

    /**
     * @brief Calculated the kinetic term for a scalar boson.
     *
     * @param X Space-time point for the kinetic term.
     *
     * @return The symbolic result for the kinetic term.
     *
     * @sa mty::ScalarKineticTerm()
     */
    csl::Expr getKineticTerm(csl::Tensor &X) override;

  protected:
    /**
     * @brief Initializes the Scalar propagator.
     *
     * @sa mty::ScalarPropagator() and mty::IntegratedScalarPropagator().
     */
    void initPropagator() override;
};

/**
 * @brief Delegates the construction of a Scalar boson and returns the result.
 *
 * @details This function can take any argument that the constructors
 * ScalarBoson::ScalarBoson() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created scalar boson.
 */
template <class... Args>
inline Particle scalarboson_s(Args &&... args)
{
    return csl::make_shared<ScalarBoson>(std::forward<Args>(args)...);
}

} // namespace mty

#endif
