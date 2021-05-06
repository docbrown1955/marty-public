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
 * @file topology.h
 * @brief Contains utilities to handle topologies for Feynman diagrams.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-05
 */
#pragma once

#include <iostream>

/**
 * @brief Namespace containing topology utilities for Feynman diagram 
 * calculations.
 */
namespace mty::Topology {

    /**
     * @brief Enumeration of the different possible topologies.
     *
     * @details Enumeration values are aligned on powers of 2 to be able to 
     * compose topologies using the pipe | as the logical 'or'. For example, 
     * Triangle | Box corresponds to a topology "triangle or box". The topology
     * #All refers to all the topologies and is equivalent to
     * \code
     *      All = Tadpole | Mass | Triangle | Box | Pentagon
     * \endcode
     * The topology does not count the number of propagators in the loop but the
     * number of particles connected to it. For only 3-vertices those two 
     * situations are equivalent but when higher order vertices are connected to
     * the loop they are not. For example, a 4-vertex looping with a 3-vertex by
     * 2 propagators is a Triangle because the loop is connected to three 
     * external particles (2 from the 4-vertex + 1 form the 3-vertex).
     */
    enum Type {
        Tadpole  = (1 << 0), /*!< Tadpole 1-point function. */
        Mass     = (1 << 1), /*!< Mass correction 2-point function. */
        Triangle = (1 << 2), /*!< Triangle 3-point function. */
        Box      = (1 << 3), /*!< Box 4-point function.  */
        Pentagon = (1 << 4), /*!< Pentagon 5-point function. */
        /**
         * @brief All topologies i.e. combination of all 5 possible topologies.
         */
        All = Tadpole | Mass | Triangle | Box | Pentagon
    };

    /**
     * @brief List of the different possible topologies.
     */
    inline constexpr auto topologyList = {
        Tadpole, Mass, Triangle, Box, Pentagon
    };

    /**
     * @brief Names for topologies.
     */
    inline constexpr auto names = {
        "Tadpole", "Mass", "Triangle", "Box", "Pentagon"
    };

    /**
     * @brief Converts an integer corresponding to a number of legs to a 
     * topology.
     *
     * @param nLegs number of legs outside the loop.
     *
     * @return The topology corresponding to a loop with \b nLegs legs.
     */
    inline Type legsToTopology(int nLegs) {
        return static_cast<Type>(
                ((nLegs == 1) * Tadpole)
                | ((nLegs == 2) * Mass)
                | ((nLegs == 3) * Triangle)
                | ((nLegs == 4) * Box)
                | ((nLegs == 5) * Pentagon)
                );
    }

    /**
     * @brief Overload of output stream operator for a topology.
     *
     * @param out Output stream.
     * @param t   Type of the topology to display.
     *
     * @return The modified output stream.
     */
    inline std::ostream &operator<<(std::ostream &out, Type t) {
        if (t == All) out << "All";
        else          out << *(names.begin() + static_cast<int>(t)/2);
        return out;
    }
}
