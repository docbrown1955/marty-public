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
 * @version 1.0
 
 * @brief Contains implementation of the Ghost boson in MARTY.
 */
#ifndef GHOST_FIELD_H_INCLUDED
#define GHOST_FIELD_H_INCLUDED

#include "scalarField.h"

namespace mty {

class VectorBoson;

/**
 * @brief Ghost boson, created with the vector bosons in non abelian gauge 
 * groups.
 *
 * @details These particles are anti-commuting scalar bosons with the same 
 * Gauge representation as the associated vector boson. The anti-commuting 
 * property is very important calculations as it determines the sign of diagrams
 * in which ghosts arise.
 */
class GhostBoson: public ScalarBoson {

    public:

    /**
     * @brief Constructor with one parameter.
     *
     * @details By default, the name of the Ghost boson is defined as
     * "c_<vector-name>".
     *
     * @param t_vectorBoson Associated vector boson.
     */
    GhostBoson(std::shared_ptr<VectorBoson> const& t_vectorBoson);

    /**
     * @brief Constructor with two parameters.
     *
     * @param t_name        Name of the Ghost boson.
     * @param t_vectorBoson Associated vector boson.
     */
    GhostBoson(std::string                  const& t_name,
               std::shared_ptr<VectorBoson> const& t_vectorBoson);

    /**
     * @brief Prints the MARTY (C++) code defining the Ghost boson. 
     *
     * @details This function assumes that the vector boson has already been
     * defined.
     *
     * @param out        Output stream
     * @param indentSize Indent to use when printing instructions
     */
    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    /**
     * @return ParticleType::GhostBoson
     */
    ParticleType getParticleType() const override {
        return ParticleType::GhostBoson;
    }

    /**
     * @return Returns the vector boson associated with the Goldstone.
     */
    Particle getVectorBoson() const override;

    /**
     * @brief Returns true as a Ghost anti-commutes.
     *
     * @return \b True
     */
    bool isAntiCommuting() const override;

    /**
     * @brief Generates a similar Ghost boson, with the same associated vector
     * boson.
     *
     * @param t_name Name of the new ghost boson.
     *
     * @return The newly created Ghost boson.
     */
    Particle generateSimilar(std::string const& t_name) const override;

    /**
     * @brief Calculates the kinetic term of the Ghost.
     *
     * @details The Ghost kinetic term is not the same as a standard scalar 
     * boson and must be given explicitly by this class.
     *
     * @param X Space-time point for the Lagrangian term.
     *
     * @return The symbolic expression of the Ghost kinetic term.
     *
     * @sa mty::GhostKineticTerm() that contains the implementation.
     */
    csl::Expr getKineticTerm(csl::Tensor & X) override;

    int getNDegreesOfFreedom() const override;

    private:

    /**
     * @brief Weak pointer to the associated vector boson.
     */
    std::weak_ptr<VectorBoson> vectorBoson;
};

/**
 * @brief Creates a ghost boson.
 *
 * @param name          Name of the Ghost.
 * @param t_vectorBoson Associated vector boson.
 *
 * @return The newly created Ghost boson.
 */
Particle ghostboson_s(
        std::string                  const& name,
        std::shared_ptr<VectorBoson> const& t_vectorBoson);

/**
 * @brief Creates a ghost boson.
 *
 * @details The name is determined automatically by the constructor 
 * GhostBoson::GhostBoson().
 *
 * @param t_vectorBoson Associated vector boson.
 *
 * @return The newly created Ghost boson.
 */
Particle ghostboson_s(
        std::shared_ptr<VectorBoson> const& t_vectorBoson);

/**
 * @brief Creates a ghost boson.
 *
 * @param name          Name of the Ghost.
 * @param t_vectorBoson Associated vector boson, **must be of type VectorBoson**.
 *
 * @return The newly created Ghost boson.
 */
Particle ghostboson_s(
        std::string const& name,
        Particle    const& t_vectorBoson);

/**
 * @brief Creates a ghost boson.
 *
 * @details The name is determined automatically by the constructor 
 * GhostBoson::GhostBoson().
 *
 * @param t_vectorBoson Associated vector boson, **must be of type VectorBoson**.
 *
 * @return The newly created Ghost boson.
 */
Particle ghostboson_s(
        Particle const& t_vectorBoson);


}

#endif
