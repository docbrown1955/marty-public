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
 * @file lhaData.h
 * @brief Contains all data containers for .lha files.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-02
 */
#ifndef LHADATA_H_INCLUDED
#define LHADATA_H_INCLUDED

#include "lhaBlocks.h"
#include "csl/std_vector_implementation.h"
#include <vector>
#include <optional>
#include <string>
#include <string_view>

namespace mty::lha {

/**
 * @brief Returns the BlockType corresponding to its name. The name must 
 * correspond to the enumeration name (see BlockType).
 *
 * @param name Name of the type.
 *
 * @return The BlockType corresponding to \b name.
 */
BlockType blockType(std::string_view name);
/**
 * @brief Output stream overload for BlockType.
 *
 * @param out  Output stream.
 * @param type BlockType to display.
 *
 * @return A reference to \b out.
 */
std::ostream &operator<<(
        std::ostream &out,
        BlockType     type
        );

/**
 * @brief Floating point type for LHA support. Long double is 64 bits usually.
 */
using FloatType = long double;

/**
 * @brief Corresponds to a line of a .lha file.
 *
 * @details Contains an id, a value, and eventually some additional integers 
 * corresponding typically to matrix positions.
 *
 * @sa LHABlock
 */
struct LHAElement {

    /**
     * @brief Id of the element in the block, or row position for a matrix
     * element.
     */
    size_t    id;
    /**
     * @brief Column position for a matrix element.
     */
    size_t    id_sup;
    /**
     * @brief Value of the element.
     */
    FloatType value;
};

/**
 * @brief Helper class to define a comparison function for positions.
 */
struct Comparator {

    /**
     * @brief Compares two vectors of positions.
     *
     * @details If \b pos1 and \b pos2 have different sizes, this function 
     * returns 
     * \code
     *    pos1.size() < pos2.size();
     * \endcode
     * Otherwise, the alphabetical order is used. This allows to sort matrix 
     * elements row by row, column by column (00 - 01 - 10 - 11 for example).
     *
     * @param pos1 First position.
     * @param pos2 Second position.
     *
     * @return \b True  if \b pos1 < \b pos2.
     * @return \b False else.
     */
    static bool compare(
            std::vector<size_t> const &pos1,
            std::vector<size_t> const &pos2
            )
    {
        if (pos1.size() != pos2.size())
            return pos1.size() < pos2.size();
        auto iter1 = pos1.begin();
        auto iter2 = pos2.begin();
        while (iter1 != pos1.end()) {
            if (*iter1 < *iter2)
                return true;
            else if (*iter2 < *iter1)
                return false;
            ++iter1;
            ++iter2;
        }
        return false;
    }

    /**
     * @brief Compares two LHAElement.
     *
     * @details First compares the id. Returns the simpler if they are different
     * and compares the positions else.
     *
     * @param A First LHAElement.
     * @param B Second LHAElement.
     *
     * @return \b True  if \b A < \b B.
     * @return \b False else.
     */
    static bool compare(
            LHAElement const &A,
            LHAElement const &B
            )
    {
        if (A.id != B.id)
            return A.id < B.id;
        return A.id_sup < B.id_sup;
    }
};

/**
 * @brief LHABlock, containing a vector of LHAElement. 
 *
 * @details When reading a LHA File, all elements of a given block are stored
 * in a LHABlock.
 *
 * @sa LHAElement
 */
class LHABlock {

public:

    IMPLEMENTS_STD_VECTOR(LHAElement, elements)

    /**
     * @brief Default constructor.
     */
    LHABlock() = default;
    /**
     * @brief Default destructor.
     */
    ~LHABlock() = default;

    /**
     * @brief Constructor with one (optional) parameter.
     *
     * @param t_name Name of the block.
     */
    LHABlock(std::string const &);

    /**
     * @return The name of the block.
     */
    std::string const &getName() const { 
        return name; 
    }
    /**
     * @brief Sets the name of the block.
     *
     * @param t_name New name of the block.
     */
    void setName(std::string const &t_name) {
        name = t_name;
    }

    /**
     * @brief Returns the first element (if found) with id \b id.
     *
     * @param id Id of the element to search.
     *
     * @return A std::optional containing the first element with id \b id if 
     * found.
     * @return std::nullopt else.
     */
    std::optional<LHAElement> getElement(size_t id) const;

    /**
     * @brief Returns the first element (if found) with id \b id and position
     * \b pos.
     *
     * @param id  Id of the element to search.
     * @param pos Matrix coordinates of the elements.
     *
     * @return A std::optional containing the first element corresponding to 
     * \b id and \b pos if found.
     * @return std::nullopt else.
     */
    std::optional<LHAElement> getElement(
            size_t id,
            size_t id_sup = -1
            ) const;

    /**
     * @brief Returns all the elements for a given id. If there is multiple 
     * ones, for example a matrix, they are stored linearly in the container.
     *
     * @param id Id of the elements to return.
     *
     * @return The LHAElement with id \b id.
     *
     * @note If the sortElements() has been called, the elements in the vector
     * are sorted by position (if more than one). In that case, for a matrix,
     * elements are sorted by row and by column ((0, 0) (0, 1) (1, 0) (1, 1)
     * etc).
     */
    std::vector<LHAElement> getMultipleElements(size_t id) const;

    /**
     * @brief Sorts the elements in the block using Comparator::compare().
     */
    void sortElements();

    /**
     * @brief Adds an elements to the block.
     *
     * @param id     Id of the element.
     * @param id_sup Supplementary id for the element.
     * @param value  Value of the element.
     */
    void addElement(
            size_t    id,
            size_t    id_sup,
            FloatType value
            );
    /**
     * @brief Adds an elements to the block.
     *
     * @param id    Id of the element.
     * @param value Value of the element.
     */
    void addElement(
            size_t    id,
            FloatType value
            );

    /**
     * @brief Adds an elements to the block.
     *
     * @param t_element LHAElement to add in the block.
     */
    void addElement(LHAElement const &t_element);

    /**
     * @brief Adds an elements to the block.
     *
     * @param t_element LHAElement to add in the block.
     */
    void addElement(LHAElement &&t_element);

private:


    /**
     * @brief Name of the block.
     */
    std::string name;
    /**
     * @brief Vector of LHAElement in the block.
     */
    std::vector<LHAElement> elements;
};

/**
 * @brief Final class containing all the data coming from a LHA file.
 *
 * @details This object is composed of a set of blocks (see LHABlock), each one
 * containing a set of elements (see LHAElement).
 */
class LHAFileData {

public:

    static constexpr size_t npos = -1;

    IMPLEMENTS_STD_VECTOR(LHABlock, blocks)

    /**
     * @brief Default constructor.
     */
    LHAFileData() = default;
    /**
     * @brief Default destructor.
     */
    ~LHAFileData() = default;

    /**
     * @brief Searches a block of a given type and returns it if found.
     *
     * @param nameBlock Name of the block type.
     *
     * @return The position of the block of type named \b nameBlock if found.
     * @return LHAFileData::npos else.
     *
     * @sa BlockName, blockType()
     */
    size_t findBlock(std::string_view nameBlock) const;
    /**
     * @brief Adds a block to the data if it does not already exist.
     *
     * @param nameBlock Name of the block to create.
     */
    void addBlock(std::string_view nameBlock);

    /**
     * @brief Adds an element (LHAElement) to a block (LHABlock).
     *
     * @tparam ...Args Variadic template for any arguments for the overloads of
     * LHABlock::addElement().
     * @param type Type of the block in which the element is added.
     * @param args Arguments for LHABlock::addElement().
     *
     * @note If the block does not exists, it is created and the element is 
     * added directly after.
     */
    template<class ...Args>
        void addElement(
                std::string_view nameBlock,
                Args           &&...args
                )
        {
            for (auto &b : blocks)
                if (b.getName() == nameBlock) {
                    b.addElement(std::forward<Args>(args)...);
                    return;
                }
        }

    /**
     * @brief Returns all the values of a given block, in order.
     *
     * @param nameBlock Name of the block to get.
     *
     * @return All the values of the block if it exists.
     * @return An empty vector else.
     */
    std::vector<FloatType> getValues(
            std::string_view nameBlock
            ) const;

    /**
     * @brief Returns the value in a given block for a given id.
     *
     * @param nameBlock Name of the block.
     * @param id        Id of the element to get.
     *
     * @return The element in the block \b nameBlock with id \b id if it exists.
     * @return std::nullopt else.
     */
    std::optional<FloatType> getValue(
            std::string_view nameBlock,
            size_t           id
            ) const;

    /**
     * @brief Returns the value in a given block for a given id.
     *
     * @param nameBlock Name of the block.
     * @param i         Row number for matrix element.
     * @param j         Column number for matrix element.
     *
     * @return The element in the block \b nameBlock with id \b id if it exists.
     * @return std::nullopt else.
     */
    std::optional<FloatType> getValue(
            std::string_view nameBlock,
            size_t           i,
            size_t           j
            ) const;

private:

    /**
     * @brief Vector of blocks representing the .lha file.
     */
    std::vector<LHABlock> blocks;
};

/**
 * @brief Displays a LHAElement in an output stream.
 *
 * @param out     Output stream.
 * @param element LHAElement to display.
 *
 * @return A reference to \b out.
 */
std::ostream &operator<<(
        std::ostream     &out,
        LHAElement const &element
        );

/**
 * @brief Displays a LHABlock in an output stream.
 *
 * @param out     Output stream.
 * @param element LHABlock to display.
 *
 * @return A reference to \b out.
 */
std::ostream &operator<<(
        std::ostream   &out,
        LHABlock const &block
        );

/**
 * @brief Displays a LHAFileData in an output stream.
 *
 * @param out     Output stream.
 * @param element LHAFileData to display.
 *
 * @return A reference to \b out.
 */
std::ostream &operator<<(
        std::ostream      &out,
        LHAFileData const &data
        );

} // End of namespace mty::lha

#endif 
