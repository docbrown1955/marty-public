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
 * @file flhaBlock.h
 * @brief Containing mty::flha::Block, allowing to define a lha block (MINPAR,
 * EXTPAR etc) with the corresponding variables modified when reading a file.
 * @author Grégoire Uhlrich
 * @version 1.0
 * @date 2020-08-31
 */

#ifndef FLHABLOCK_H_INCLUDED
#define FLHABLOCK_H_INCLUDED

#include "flha.h"
#include "SM.h"
#include "MSSM.h"

namespace mty::flha {

    /**
     * @brief Enumeration of possible lha block names in MARTY.
     */
    enum BlockName {

        None,
        SMINPUTS,
        MINPAR,
        EXTPAR,
        MSQ2,
        MSL2,
        MSu2,
        MSd2,
        MSe2,
        TUIN,
        TDIN,
        TEIN
    };

    /**
     * @brief Converts a std::string to a BlockName.
     *
     * @param block Namem of the block.
     *
     * @return The enumeration element of BlockName corresponding to \b block.
     */
    BlockName stringToBlock(std::string const& block);

    /**
     * @brief Type definition. A block is composed of a name, and a set of
     * InputFlag for all possible variables in this block.
     */
    using Block = std::pair<BlockName, std::set<InputFlag>>;

    std::vector<Block> initFLHAFlags();

} // End of namespace mty

#endif

