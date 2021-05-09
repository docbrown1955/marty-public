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
 * @file flhaReader.h
 * @brief File containing the utility class able to read a LesHouches Accord 
 * file (.lha), in particular for SM and SUSY parameters.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-08-31
 */
#ifndef FLHA_READER_INCLUDED
#define FLHA_READER_INCLUDED

#include "flha.h"
#include "flhaBlock.h"
#include <list>

/**
 * @brief Namespace containing all lha utilities.
 */
namespace mty::flha {

    /**
     * @brief This class is purely static (no instance can be created) and
     * reads a lha file, using the mty::flha::InputFlag to modify directly, 
     * as a side-effect, the values of the corresponding quantities.
     */
    class Reader {

        public:
            
        /**
         * @brief Reads a lha file from a file name.
         *
         * @param fileName Name of the lha file to read.
         */
        static 
        void readFLHA(std::string const& fileName);
            
        /**
         * @brief Reads a lha file from a file stream.
         *
         * @param file Input file stream corresponding to the lha file.
         */
        static 
        void readFLHA(std::ifstream &file);

        private:

        static
        std::list<std::string> parseLine(std::ifstream& in);

        static
        void interpretLine(std::list<std::string> line);

        static 
        void setValue(size_t                  id,
                      long double             value,
                      std::vector<int> const& indices);

        private:

        static inline size_t indexBlock = size_t(-1);

        static inline std::vector<Block> blocks = initFLHAFlags();
    };

} // namespace flha

#endif
