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

void print_libdiagonalization_cppdata(std::ostream &out)
{
    out << "#include <cassert>\n";
    out << "#include \"libdiagonalization.h\"\n";
    out << "#include \"cdiagonalization.h\"\n";
    out << "#include <iostream>\n";
    out << "\n";
    out << "\n";
    out << "namespace {\n";
    out << "\n";
    out << "std::vector<double> flatten(\n";
    out << "        std::vector<Diagonalizer::complex> const &v\n";
    out << "        )\n";
    out << "{\n";
    out << "    std::vector<double> res;\n";
    out << "    res.reserve(2 * v.size());\n";
    out << "    for (const auto &z : v) {\n";
    out << "        res.push_back(z.real());\n";
    out << "        res.push_back(z.imag());\n";
    out << "    }\n";
    out << "    return res;\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::complex> unflatten(\n";
    out << "        std::vector<double> const &v\n";
    out << "        )\n";
    out << "{\n";
    out << "    std::vector<Diagonalizer::complex> res(v.size() / 2);\n";
    out << "    for (size_t i = 0; i != res.size(); ++i)\n";
    out << "        res[i] = {v[2 * i], v[2 * i + 1]};\n";
    out << "    return res;\n";
    out << "}\n";
    out << "\n";
    out << "} // End of anonymous namespace\n";
    out << "\n";
    out << "Diagonalizer::real Diagonalizer::precision = 1e-5;\n";
    out << "\n";
    out << "void Diagonalizer::applyDiagonalization(\n";
    out << "        std::initializer_list<complex>  massMatrix,\n";
    out << "        std::initializer_list<complex*> transfer,\n";
    out << "        std::initializer_list<real*>    masses\n";
    out << "        )\n";
    out << "{\n";
    out << "    Diagonalizer diag(massMatrix);\n";
    out << "\n";
    out << "    std::vector<complex> const &t = diag.transfer();\n";
    out << "    size_t i = 0;\n";
    out << "    for (auto iter = transfer.begin(); iter != transfer.end(); "
           "++iter)\n";
    out << "        **iter = t[i++];\n";
    out << "\n";
    out << "    i = 0;\n";
    out << "    std::vector<real> const &m = diag.mass();\n";
    out << "    for (auto iter = masses.begin(); iter != masses.end(); "
           "++iter)\n";
    out << "        **iter = m[i++];\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::applyBiDiagonalization(\n";
    out << "        std::initializer_list<complex>  massMatrix,\n";
    out << "        std::initializer_list<complex*> transfer,\n";
    out << "        std::initializer_list<complex*> transfer2,\n";
    out << "        std::initializer_list<real*>    masses\n";
    out << "        )\n";
    out << "{\n";
    out << "    Diagonalizer diag(massMatrix, true);\n";
    out << "\n";
    out << "    std::vector<complex> const &t = diag.transfer();\n";
    out << "    size_t i = 0;\n";
    out << "    for (auto iter = transfer.begin(); iter != transfer.end(); "
           "++iter)\n";
    out << "        **iter = t[i++];\n";
    out << "\n";
    out << "    i = 0;\n";
    out << "    std::vector<complex> const &t2 = diag.transfer2();\n";
    out << "    for (auto iter = transfer2.begin(); iter != transfer2.end(); "
           "++iter)\n";
    out << "        **iter = t2[i++];\n";
    out << "\n";
    out << "    i = 0;\n";
    out << "    std::vector<real> const &m = diag.mass();\n";
    out << "    for (auto iter = masses.begin(); iter != masses.end(); "
           "++iter)\n";
    out << "        **iter = m[i++];\n";
    out << "}\n";
    out << "\n";
    out << "Diagonalizer::Diagonalizer(\n";
    out << "        std::initializer_list<complex> massMatrix,\n";
    out << "        bool                           t_bidiag\n";
    out << "        )\n";
    out << "    :m_data(massMatrix),\n";
    out << "    m_N(std::sqrt(m_data.size())),\n";
    out << "    m_bidiag(t_bidiag),\n";
    out << "    m_computed(false)\n";
    out << "{\n";
    out << "    assert(m_N * m_N == massMatrix.size());\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::complex> const "
           "&Diagonalizer::transfer()\n";
    out << "{\n";
    out << "    updateDiagonalization();\n";
    out << "    return m_transfer;\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::complex> const "
           "&Diagonalizer::transfer2()\n";
    out << "{\n";
    out << "    updateDiagonalization();\n";
    out << "    assert(m_bidiag);\n";
    out << "    return m_transfer2;\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::real> const &\n";
    out << "    Diagonalizer::mass()\n";
    out << "{\n";
    out << "    updateDiagonalization();\n";
    out << "    return m_mass;\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::updateDiagonalization()\n";
    out << "{\n";
    out << "    if (!m_computed) {\n";
    out << "        (m_bidiag) ? bidiagonalize() : diagonalize();\n";
    out << "        m_computed = true;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::diagonalize()\n";
    out << "{\n";
    out << "    std::vector<double> init = flatten(m_data);\n";
    out << "    std::vector<double> transfer(2 * m_N * m_N);\n";
    out << "    std::vector<double> mass(m_N);\n";
    out << "    c_diagonalize_hermitian(\n";
    out << "            m_N, init.data(), transfer.data(), mass.data(), true\n";
    out << "            );\n";
    out << "\n";
    out << "    m_transfer = unflatten(transfer);\n";
    out << "    m_mass.assign(mass.begin(), mass.end());\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::bidiagonalize()\n";
    out << "{\n";
    out << "    std::vector<double> init = flatten(m_data);\n";
    out << "    std::vector<double> transfer(2 * m_N * m_N);\n";
    out << "    std::vector<double> transfer2(2 * m_N * m_N);\n";
    out << "    std::vector<double> mass(m_N);\n";
    out << "    c_bidiagonalize(\n";
    out << "            m_N,\n";
    out << "            init.data(),\n";
    out << "            transfer.data(),\n";
    out << "            transfer2.data(),\n";
    out << "            mass.data(),\n";
    out << "            precision\n";
    out << "            );\n";
    out << "\n";
    out << "    m_transfer  = unflatten(transfer);\n";
    out << "    m_transfer2 = unflatten(transfer2);\n";
    out << "    m_mass.assign(mass.begin(), mass.end());\n";
    out << "}\n";
}

} // End of namespace csl
