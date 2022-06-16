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
 * @version 2.0

 * @brief Contains implementation of Goldtone bosons in MARTY.
 */
#ifndef GOLDSTONE_FIELD_H_INCLUDED
#define GOLDSTONE_FIELD_H_INCLUDED

#include "scalarField.h"

namespace mty {

class VectorBoson;

/**
 * @brief Goldstone boson class.
 *
 * @details Goldstone bosons are meant to be scalar bosons associated with a
 * vector boson through gauge fixing. A unitary gauge for example gives an
 * infinite mass the the Goldstone boson and decouples it from the theory.
 */
class GoldstoneBoson : public ScalarBoson {

  public:
    /**
     * @brief Constructor with one parameter
     *
     * @details The Goldstone name is automatically set to "G_<vector-name>".
     *
     * @param t_vectorBoson Associated Vector boson.
     */
    GoldstoneBoson(std::shared_ptr<VectorBoson> const &t_vectorBoson);

    /**
     * @brief Constructor with two parameters
     *
     * @param name          Name of the Goldstone boson.
     * @param t_vectorBoson Associated vector boson.
     */
    GoldstoneBoson(std::string const &                 name,
                   std::shared_ptr<VectorBoson> const &t_vectorBoson);

    /**
     * @brief Prints the MARTY (C++) code defining the Goldstone boson.
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
     * @return ParticleType::GoldstoneBoson
     */
    ParticleType getParticleType() const override
    {
        return ParticleType::GoldstoneBoson;
    }

    /**
     * @return Returns the vector boson associated with the Goldstone.
     */
    Particle getVectorBoson() const override;

    /**
     * @brief Generates a similar Golstone boson, giving the same associated
     * vector boson.
     *
     * @param t_name Name of the new Goldstone boson.
     *
     * @return The newly created Goldstone boson.
     */
    Particle generateSimilar(std::string const &t_name) const override;

    int getNDegreesOfFreedom() const override;

  private:
    /**
     * @brief Weak pointer to the associated vector boson
     */
    std::weak_ptr<VectorBoson> vectorBoson;
};

/**
 * @brief Creates a goldstone boson.
 *
 * @param name          Name of the Goldstone boson.
 * @param t_vectorBoson Associated vector boson.
 *
 * @return The newly created Goldstone boson.
 */
Particle goldstoneboson_s(std::string const &                 name,
                          std::shared_ptr<VectorBoson> const &t_vectorBoson);

/**
 * @brief Creates a goldstone boson.
 *
 * @details The name is determined automatically by the constructor, see
 * GoldstoneBoson::GoldstoneBoson().
 *
 * @param t_vectorBoson Associated vector boson.
 *
 * @return The newly created Goldstone boson.
 */
Particle goldstoneboson_s(std::shared_ptr<VectorBoson> const &t_vectorBoson);

/**
 * @brief Creates a goldstone boson.
 *
 * @param name          Name of the Goldstone boson.
 * @param t_vectorBoson Associated vector boson, **must be of type
 * VectorBoson**.
 *
 * @return The newly created Goldstone boson.
 */
Particle goldstoneboson_s(std::string const &name,
                          Particle const &   t_vectorBoson);

/**
 * @brief Creates a goldstone boson.
 *
 * @details The name is determined automatically by the constructor, see
 * GoldstoneBoson::GoldstoneBoson().
 *
 * @param t_vectorBoson Associated vector boson, **must be of type
 * VectorBoson**.
 *
 * @return The newly created Goldstone boson.
 */
Particle goldstoneboson_s(Particle const &t_vectorBoson);

} // namespace mty

#endif
