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
 * @file librarytensor_hdata.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#pragma once

#include <iostream>

namespace csl {

void print_librarytensor_hdata(std::ostream &out) {
    out << "#ifndef CSL_LIBRARYTENSOR_H_INCLUDED\n";
    out << "#define CSL_LIBRARYTENSOR_H_INCLUDED\n";
    out << "\n";
    out << "#include <vector>\n";
    out << "#include <iostream>\n";
    out << "#include \"csl/default_move_cpy.h\"\n";
    out << "\n";
    out << "namespace csl {\n";
    out << "\n";
    out << "    template<class Type>\n";
    out << "    class LibraryTensor {\n";
    out << "\n";
    out << "        public:\n";
    out << "\n";
    out << "        DEFINE_DEFAULT_CPY_MV(LibraryTensor)\n";
    out << "\n";
    out << "        using iterator       = typename std::vector<Type>::iterator;\n";
    out << "        using const_iterator = typename std::vector<Type>::const_iterator;\n";
    out << "        using reverse_iterator       \n";
    out << "            = typename std::vector<Type>::reverse_iterator;\n";
    out << "        using const_reverse_iterator \n";
    out << "            = typename std::vector<Type>::const_reverse_iterator;\n";
    out << "\n";
    out << "        static size_t getTotalDimension(std::vector<size_t> const& dimensions)\n";
    out << "        {\n";
    out << "            size_t tot = 1;\n";
    out << "            for (size_t d : dimensions)\n";
    out << "                tot *= d;\n";
    out << "            return tot;\n";
    out << "        }\n";
    out << "\n";
    out << "        LibraryTensor(std::vector<size_t> const& t_dimensions)\n";
    out << "            :dimensions(t_dimensions),\n";
    out << "            data(getTotalDimension(dimensions))\n";
    out << "        {\n";
    out << "\n";
    out << "        }\n";
    out << "\n";
    out << "        LibraryTensor(std::vector<size_t> const& t_dimensions,\n";
    out << "                      Type                const& filler)\n";
    out << "            :dimensions(t_dimensions),\n";
    out << "            data(getTotalDimension(dimensions), filler)\n";
    out << "        {\n";
    out << "\n";
    out << "        }\n";
    out << "\n";
    out << "        LibraryTensor(std::vector<size_t> const& t_dimensions,\n";
    out << "                      std::vector<Type>   const& t_data)\n";
    out << "            :dimensions(t_dimensions),\n";
    out << "            data(t_data)\n";
    out << "        {\n";
    out << "            if (getTotalDimension(dimensions) != data.size()) {\n";
    out << "                std::cerr << \"Bad initialization of LibraryTensor in file \"\n";
    out << "                    << __FILE__ << \" (l. \" << __LINE__ << \"): expected \"\n";
    out << "                    << getTotalDimension(dimensions) << \" elements, \"\n";
    out << "                    << data.size() << \" given.\\n\";\n";
    out << "                exit(123);\n";
    out << "            }\n";
    out << "        }\n";
    out << "     \n";
    out << "        std::vector<size_t> const& getDimensions() const {\n";
    out << "            return dimensions;\n";
    out << "        }\n";
    out << "        std::vector<Type> const& toStdVector() const {\n";
    out << "            return data;\n";
    out << "        }\n";
    out << "\n";
    out << "        size_t getIndex(std::vector<size_t> const& indices) const {\n";
    out << "            if (dimensions.size() == 1)\n";
    out << "                return indices[0];\n";
    out << "            size_t index = 0;\n";
    out << "            auto iter_index = indices.begin();\n";
    out << "            auto iter_dim   = dimensions.begin();\n";
    out << "            for (; iter_dim != dimensions.end(); ++iter_index, ++iter_dim) {\n";
    out << "                index *= *iter_dim;\n";
    out << "                index += *iter_index;\n";
    out << "            }\n";
    out << "\n";
    out << "            return index;\n";
    out << "        }\n";
    out << "\n";
    out << "        bool empty() const {\n";
    out << "            return data.empty();\n";
    out << "        }\n";
    out << "        size_t size() const {\n";
    out << "            return data.size();\n";
    out << "        }\n";
    out << "\n";
    out << "#ifndef CSL_LT_DISABLE_ITERATOR\n";
    out << "        iterator begin() {\n";
    out << "            return data.begin();\n";
    out << "        }\n";
    out << "        iterator end() {\n";
    out << "            return data.end();\n";
    out << "        }\n";
    out << "        const_iterator begin() const {\n";
    out << "            return data.begin();\n";
    out << "        }\n";
    out << "        const_iterator end() const {\n";
    out << "            return data.end();\n";
    out << "        }\n";
    out << "\n";
    out << "        reverse_iterator rbegin() {\n";
    out << "            return data.rbegin();\n";
    out << "        }\n";
    out << "        reverse_iterator rend() {\n";
    out << "            return data.rend();\n";
    out << "        }\n";
    out << "        const_reverse_iterator rbegin() const {\n";
    out << "            return data.rbegin();\n";
    out << "        }\n";
    out << "        const_reverse_iterator rend() const {\n";
    out << "            return data.rend();\n";
    out << "        }\n";
    out << "#endif\n";
    out << "\n";
    out << "        Type& operator[](size_t pos) {\n";
    out << "            return data[pos];\n";
    out << "        }\n";
    out << "        Type const& operator[](size_t pos) const {\n";
    out << "            return data[pos];\n";
    out << "        }\n";
    out << "\n";
    out << "        Type& operator[](std::vector<size_t> const& indices) {\n";
    out << "            return data[getIndex(indices)];\n";
    out << "        }\n";
    out << "        Type const& operator[](std::vector<size_t> const& indices) const {\n";
    out << "            return data[getIndex(indices)];\n";
    out << "        }\n";
    out << "\n";
    out << "        public:\n";
    out << "\n";
    out << "        std::vector<size_t> dimensions;\n";
    out << "\n";
    out << "        std::vector<Type>   data;\n";
    out << "    };\n";
    out << "\n";
    out << "} // End of namespace csl\n";
    out << "\n";
    out << "#endif // ifndef CSL_LIBRARYTENSOR_H_INCLUDED\n";
}

} // End of namespace csl
