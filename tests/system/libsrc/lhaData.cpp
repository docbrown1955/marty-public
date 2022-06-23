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

#include "lhaData.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace mty::lha {

///////////////////////////////////////////////////
/*************************************************/
// Utilities                                     //
/*************************************************/
///////////////////////////////////////////////////

BlockType blockType(std::string_view name)
{
    if (name == "SMINPUTS")
        return BlockType::SMINPUTS;
    if (name == "MINPAR")
        return BlockType::MINPAR;
    if (name == "EXTPAR")
        return BlockType::EXTPAR;
    if (name == "MSQ2")
        return BlockType::MSQ2;
    if (name == "MSL2")
        return BlockType::MSL2;
    if (name == "MSu2")
        return BlockType::MSu2;
    if (name == "MSd2")
        return BlockType::MSd2;
    if (name == "MSe2")
        return BlockType::MSe2;
    if (name == "TUIN")
        return BlockType::TUIN;
    if (name == "TDIN")
        return BlockType::TDIN;
    if (name == "TEIN")
        return BlockType::TEIN;

    return BlockType::None;
}

std::ostream &operator<<(
        std::ostream &out,
        BlockType     type
        )
{
    switch(type) {
        case BlockType::SMINPUTS: out << "SMINPUTS"; break;
        case BlockType::MINPAR:   out << "MINPAR";   break;
        case BlockType::EXTPAR:   out << "EXTPAR";   break;
        case BlockType::MSQ2:     out << "MSQ2";     break;
        case BlockType::MSL2:     out << "MSL2";     break;
        case BlockType::MSu2:     out << "MSu2";     break;
        case BlockType::MSd2:     out << "MSd2";     break;
        case BlockType::MSe2:     out << "MSe2";     break;
        case BlockType::TUIN:     out << "TUIN";     break;
        case BlockType::TDIN:     out << "TDIN";     break;
        case BlockType::TEIN:     out << "TEIN";     break;
        default:
            out << "None"; 
            break;
    }

    return out;
}

///////////////////////////////////////////////////
/*************************************************/
// LHABlock Class                                //
/*************************************************/
///////////////////////////////////////////////////

LHABlock::LHABlock(std::string const &t_name)
{
    setName(t_name);
}

std::optional<LHAElement> LHABlock::getElement(size_t id) const
{
    for (const auto &e : elements) {
        if (e.id == id)
            return e;
    }
    return std::nullopt;
}

std::optional<LHAElement> LHABlock::getElement(
        size_t id,
        size_t id_sup
        ) const
{
    for (const auto &e : elements) {
        if (e.id == id and (id_sup == size_t(-1) or e.id_sup == id_sup))
            return e;
    }
    return std::nullopt;
}

std::vector<LHAElement> LHABlock::getMultipleElements(
        size_t id
        ) const
{
    std::vector<LHAElement> res;
    for (const auto &e : elements)
        if (e.id == id)
            res.push_back(e);
    return res;
}

void LHABlock::sortElements()
{
    using compareElementFunc = bool(*)(
            LHAElement const&,
            LHAElement const&
            );
    std::sort(
            elements.begin(),
            elements.end(),
            static_cast<compareElementFunc>(mty::lha::Comparator::compare)
            );
}

void LHABlock::addElement(
        size_t    id,
        FloatType value
        )
{
    elements.push_back({id, size_t(-1), value});
}

void LHABlock::addElement(
        size_t    id,
        size_t    id_sup,
        FloatType value
        )
{
    elements.push_back({id, id_sup, value});
}

void LHABlock::addElement(LHAElement const &t_element)
{
    elements.emplace_back(t_element);
}

void LHABlock::addElement(LHAElement &&t_element)
{
    elements.emplace_back(t_element);
}

///////////////////////////////////////////////////
/*************************************************/
// LHAFileData Class                             //
/*************************************************/
///////////////////////////////////////////////////

size_t LHAFileData::findBlock(
        std::string_view nameBlock
        ) const
{
    for (size_t i = 0; i != blocks.size(); ++i) 
        if (blocks[i].getName() == nameBlock)
            return i;
    return npos;
}

void LHAFileData::addBlock(std::string_view name)
{
    if (findBlock(name) == npos) {
        blocks.emplace_back(std::string(name));
    }
}

std::vector<FloatType> LHAFileData::getValues(
        std::string_view nameBlock
        ) const 
{
    for (const auto &block : blocks)
        if (block.getName() == nameBlock) {
            std::vector<FloatType> res;
            res.reserve(block.size());
            for (const auto &el : block) {
                res.push_back(el.value);
            }
            return res;
        }

    return {};
}

std::optional<FloatType> LHAFileData::getValue(
        std::string_view nameBlock,
        size_t           id
        ) const
{
    for (const auto &block : blocks)
        if (block.getName() == nameBlock) 
            for (const auto &el : block)
                if (el.id == id)
                    return el.value;

    return std::nullopt;
}

std::optional<FloatType> LHAFileData::getValue(
        std::string_view nameBlock,
        size_t           i,
        size_t           j
        ) const
{
    for (const auto &block : blocks)
        if (block.getName() == nameBlock) 
            for (const auto &el : block)
                if (el.id == i and el.id_sup == j)
                    return el.value;

    return std::nullopt;
}

///////////////////////////////////////////////////
/*************************************************/
// Output stream operators                       //
/*************************************************/
///////////////////////////////////////////////////

/**
 * @brief Returns the number of spaces needed to display correctly an integer.
 *
 * @param n Integer to display.
 *
 * @return std::ceil(std::log10(n)) >= 4 ? 10 : 5
 */
int nSpace(size_t n) {
    return (std::ceil(std::log10(n)) >= 4) ? 10 : 5;
}

std::ostream &operator<<(
        std::ostream     &out,
        LHAElement const &element
        )
{
    out << std::left;
    out << std::setw(nSpace(element.id)) << element.id;
    if (element.id_sup > 0)
        out << std::setw(nSpace(element.id_sup)) << element.id_sup;
    size_t n_digits = std::numeric_limits<FloatType>::max_digits10;
    out.precision(n_digits);
    out << std::setw(n_digits + 8) << element.value;

    return out;
}

std::ostream &operator<<(
        std::ostream   &out,
        LHABlock const &block
        )
{
    out << "BLOCK " << block.getName() << '\n';
    for (const auto &element : block)
        out << element << '\n';

    return out;
}

std::ostream &operator<<(
        std::ostream      &out,
        LHAFileData const &data
        )
{
    out << "# LHA file generated by MARTY\n";
    for (const auto &block : data) {
        out << block << '\n';
    }

    return out;
}

} // End of namespace lha
