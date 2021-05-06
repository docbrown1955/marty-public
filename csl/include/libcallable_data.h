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

 #pragma once

#include <iostream>

namespace csl {
    inline void printCallableStructure(
            std::ostream &out
            )
    {
        out << "template<class ReturnType, class ParamType>\n";
        out << "struct Callable {\n";
        out << "\n";
        out << "    Callable(\n";
        out << "            std::string_view t_name,\n";
        out << "            ReturnType (*t_f)(ParamType const&)\n";
        out << "            )\n";
        out << "        :name(t_name),\n";
        out << "         f(t_f)\n";
        out << "    {}\n";
        out << "    Callable(\n";
        out << "            std::string_view t_name,\n";
        out << "            std::function<ReturnType(ParamType const&)> const &t_f\n";
        out << "            )\n";
        out << "        :name(t_name),\n";
        out << "         f(t_f)\n";
        out << "    {}\n";
        out << "\n";
        out << "    inline\n";
        out << "    ReturnType operator()(ParamType const &params) const {\n";
        out << "        return f(params);\n";
        out << "    }\n";
        out << "\n";
        out << "    std::string name;\n";
        out << "    std::function<ReturnType(ParamType)> f;\n";
        out << "};\n";
    }
}
