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
 * @file lha.h
 * @brief File containing the utility class able to read a LesHouches Accord 
 * file (.lha), in particular for SM and SUSY parameters.
 * @details We do not guarantee an exhaustive treatment of any lha parameter.
 * Only exist in MARTY those which have already been used in built-in models, 
 * in particular in the pMSSM or more general NMFV. For more details on the LHA
 * conventions, see https://arxiv.org/pdf/hep-ph/0311123.pdf.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-08-31
 */
#ifndef LHA_H_INCLUDED
#define LHA_H_INCLUDED

#include <string>
#include <vector>

/**
 * @brief Namespace containing all lha utilities.
 */
namespace mty::lha {

    struct LHAElement;
    class LHAFileData;

    /**
     * @brief Class the reads .lha file, parses them and create a LHAFileData
     * containing all blocks with their elements.
     */
    class Reader {

    public:

        /**
         * @brief Tells if warning messages must be printed during the read
         * of .lha files.
         */
        static inline 
        bool verbose = false;

        /**
         * @brief Reads a .lha file and returns its data.
         *
         * @param fileName Name of the file.
         *
         * @return A LHAFileData object containing the file data, block by 
         * block.
         */
        static 
        LHAFileData readFile(std::string const &fileName);

        /**
         * @brief Reads a .lha file and returns its data.
         *
         * @param in Input file stream for the .lha file.
         *
         * @return A LHAFileData object containing the file data, block by 
         * block.
         */
        static
        LHAFileData readFile(std::ifstream &in);

    private:

        /**
         * @brief Reads a line in \b in and removes comments (with '#').
         *
         * @param in Input file stream.
         *
         * @return A string with the next line in the file with comments 
         * removed.
         */
        static
        std::string nextLine(std::ifstream &in);

        /**
         * @brief Splits a string into pieces, separating by space and 
         * line break.
         *
         * @param line Line to split.
         *
         * @return The splitted line, in a std::vector.
         */
        static
        std::vector<std::string> split(std::string const &line);

        /**
         * @brief Tells if a line (vector of string) is a block line.
         *
         * @details A line is considered as the beginning of a block if there 
         * is at least two words and the first is exactly (case-sensitive)
         * "BLOCK".
         *
         * @param line Line of the file.
         *
         * @return \b True  if the line is a block definition.
         * @return @b False else.
         */
        static
        bool isBlockLine(std::vector<std::string> const &line);

        /**
         * @brief Tells if a string corresponds to a positive integer.
         *
         * @param str String to test.
         *
         * @return \b True  if the string is a positive integer.
         * @return \b False else.
         */
        static 
        bool isPositiveInteger(std::string const &str);

        /**
         * @brief Tells if a string corresponds to a number, integer or 
         * floating point (standard or scientific notation).
         *
         * @param str String to test.
         *
         * @return \b True  if the string is a number.
         * @return \b False else.
         */
        static 
        bool isNumber(std::string const &str);

        /**
         * @brief Tells if a line contains a value.
         *
         * @details This function considers that the line defines a value if 
         * words befire the last are positive integers (there must be at least
         * one) and if the last is a number.
         *
         * @param line Line to test.
         *
         * @return \b True  if the line is the definition of a value.
         *
         * @sa isPositiveInteger(), isNumber()
         */
        static
        bool isValueLine(std::vector<std::string> const &line);

        /**
         * @brief Reads a line and returns the LHAElement corresponding to it.
         *
         * @details This function must only be called if isValueLine() returns 
         * true for it. Otherwise it is undefined behaviour.
         *
         * @param line Line to parse into a LHAElement.
         *
         * @return The corresponding LHAElement, including ids and the value.
         *
         * @sa LHAElement, isValueLine()
         */
        static
        LHAElement readValue(std::vector<std::string> const &line);
    };

} // End of namespace mty::lha

#endif
