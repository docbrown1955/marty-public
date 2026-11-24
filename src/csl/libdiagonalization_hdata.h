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

 * \brief
 */
#pragma once

#include <iostream>

namespace csl {

void print_libdiagonalization_hdata(std::ostream &out)
{
    out << "#ifndef DIAGONALIZATION_H_INCLUDED\n";
    out << "#define DIAGONALIZATION_H_INCLUDED\n";
    out << "\n";
    out << "#include <iostream>\n";
    out << "#include <vector>\n";
    out << "#include <complex>\n";
    out << "#ifdef QUAD\n";
    out << "#include <quadmath.h>\n";
    out << "#endif\n";
    out << "\n";
    out << "class Diagonalizer {\n";
    out << "\n";
    out << "public:\n";
    out << "\n";
    out << "#ifdef QUAD\n";
    out << "    using real    = __float128;\n";
    out << "    using complex = __complex128;\n";
    out << "#else\n";
    out << "    using real    = double;\n";
    out << "    using complex = std::complex<real>;\n";
    out << "#endif\n";
    out << "\n";
    out << "    static real precision;\n";
    out << "\n";
    out << "    static void applyDiagonalization(\n";
    out << "            std::initializer_list<complex>  massMatrix,\n";
    out << "            std::initializer_list<complex*> transfer,\n";
    out << "            std::initializer_list<real*>    masses\n";
    out << "            );\n";
    out << "\n";
    out << "    static void applyBiDiagonalization(\n";
    out << "            std::initializer_list<complex>  massMatrix,\n";
    out << "            std::initializer_list<complex*> transfer,\n";
    out << "            std::initializer_list<complex*> transfer2,\n";
    out << "            std::initializer_list<real*>    masses\n";
    out << "            );\n";
    out << "\n";
    out << "    Diagonalizer(\n";
    out << "            std::initializer_list<complex> massMatrix,\n";
    out << "            bool                           biDiag = false\n";
    out << "            );\n";
    out << "\n";
    out << "    ~Diagonalizer() = default;\n";
    out << "\n";
    out << "    std::vector<complex> const &transfer();\n";
    out << "\n";
    out << "    std::vector<complex> const &transfer2();\n";
    out << "\n";
    out << "    std::vector<real> const &mass();\n";
    out << "\n";
    out << "private:\n";
    out << "\n";
    out << "    void updateDiagonalization();\n";
    out << "\n";
    out << "    void diagonalize();\n";
    out << "\n";
    out << "    void bidiagonalize();\n";
    out << "\n";
    out << "private:\n";
    out << "\n";
    out << "    std::vector<complex> m_data;\n";
    out << "    size_t               m_N;\n";
    out << "    bool                 m_bidiag;\n";
    out << "    bool                 m_computed;\n";
    out << "\n";
    out << "    std::vector<complex> m_transfer;\n";
    out << "    std::vector<complex> m_transfer2;\n";
    out << "    std::vector<real>    m_mass;\n";
    out << "};\n";
    out << "\n";
    out << "#endif\n";
}

} // End of namespace csl
