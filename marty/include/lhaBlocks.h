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
 * @file lhaBlocks.h
 * @brief Contains an enumeration of LHA blocks.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-02
 */
#ifndef LHABLOCKS_H_INCLUDED
#define LHABLOCKS_H_INCLUDED

namespace mty::lha {

    /**
     * @brief Enumeration of possible LHA blocks in MARTY.
     */
    enum class BlockType {

        /*!
         * @brief Invalid Block.
         */
        None,

        /*!
         * @brief Standard Model inputs.
         */
        SMINPUTS,
        /*!
         * @brief SUSY breaking input parameters.
         */
        MINPAR,
        /*!
         * @brief Optional input parameters for non-minimal / non-universal
         * models.
         */
        EXTPAR,

        /*!
         * @brief Soft SUSY breaking left squark mass matrix.
         */
        MSQ2,
        /*!
         * @brief Soft SUSY breaking left slepton mass matrix.
         */
        MSL2,
        /*!
         * @brief Soft SUSY breaking right up-squark mass matrix.
         */
        MSu2,
        /*!
         * @brief Soft SUSY breaking right down-squark mass matrix.
         */
        MSd2,
        /*!
         * @brief Soft SUSY breaking right selectron mass matrix.
         */
        MSe2,

        /**
         * @brief Trilinear up-type coupling.
         */
        TUIN,
        /**
         * @brief Trilinear down-type coupling.
         */
        TDIN,
        /**
         * @brief Trilinear electron-type coupling.
         */
        TEIN
    };

} // End of namespace mty::lha

#endif
