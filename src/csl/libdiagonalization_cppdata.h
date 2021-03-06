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
    out << "#include <gsl/gsl_complex.h>\n";
    out << "#include <gsl/gsl_eigen.h>\n";
    out << "#include <gsl/gsl_errno.h>\n";
    out << "#include <gsl/gsl_complex_math.h>\n";
    out << "#include <iostream>\n";
    out << "\n";
    out << "#ifdef QUAD\n";
    out << "    #include <quadmath.h>\n";
    out << "    #define ABS fabsq\n";
    out << "    #define SQRT sqrtq\n";
    out << "    #define REAL(arg) crealq(arg)\n";
    out << "    #define IMAG(arg) cimagq(arg)\n";
    out << "    #define CONJ(arg) conjq(arg)\n";
    out << "#else\n";
    out << "    #define ABS std::abs\n";
    out << "    #define SQRT std::sqrt\n";
    out << "    #define REAL(arg) (arg).real()\n";
    out << "    #define IMAG(arg) (arg).imag()\n";
    out << "    #define CONJ(arg) std::conj(arg)\n";
    out << "#endif\n";
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
    out << "\n";
    out << "Diagonalizer::real Diagonalizer::det(\n";
    out << "        std::vector<Diagonalizer::complex> const &mat\n";
    out << "        ) const\n";
    out << "{\n";
    out << "    return REAL(\n";
    out << "            mat[index(0, 0)]*mat[index(1, 1)]\n";
    out << "          - mat[index(1, 0)]*mat[index(0, 1)]\n";
    out << "          );\n";
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
    out << "void Diagonalizer::diagonalize(\n";
    out << "        std::vector<complex>  massMatrix,\n";
    out << "        std::vector<complex>  &transfer,\n";
    out << "        std::vector<real>     &mass,\n";
    out << "        bool                  bidiagonalization\n";
    out << "        )\n";
    out << "{\n";
    out << "    transfer = std::vector<complex>(m_N*m_N, 0);\n";
    out << "    mass = std::vector<real>(m_N, 0);\n";
    out << "    gsl_eigen_hermv_workspace* workspace = "
           "gsl_eigen_hermv_alloc(m_N);\n";
    out << "\n";
    out << "    if (!bidiagonalization) {\n";
    out << "        // Symmetrize the mass matrix (if it has not been done "
           "yet)\n";
    out << "        const size_t N = std::sqrt(massMatrix.size());\n";
    out << "        for (size_t i = 0; i != N; ++i)\n";
    out << "            for (size_t j = i+1; j < N; ++j) {\n";
    out << "                massMatrix[index(i, j)] = \n";
    out << "                    (massMatrix[index(i, j)] + "
           "massMatrix[index(j, i)]) / 2.;\n";
    out << "                massMatrix[index(j, i)] = massMatrix[index(i, "
           "j)];\n";
    out << "            }\n";
    out << "    }\n";
    out << "    gsl_matrix_complex* init_gsl     = "
           "getGSLMassMatrix(massMatrix);\n";
    out << "    gsl_vector*         eigenValues  = gsl_vector_alloc(m_N);\n";
    out << "    gsl_matrix_complex* transfer_gsl = "
           "gsl_matrix_complex_alloc(m_N, m_N);\n";
    out << "\n";
    out << "    // Diagonalization\n";
    out << "    gsl_eigen_hermv(init_gsl, eigenValues, transfer_gsl, "
           "workspace);\n";
    out << "\n";
    out << "    // Gathering results\n";
    out << "    loadGSLMatrix(transfer_gsl, transfer);\n";
    out << "    loadGSLVector(eigenValues,  mass);\n";
    out << "\n";
    out << "    // Freeing allocated memory by gsl\n";
    out << "    gsl_matrix_complex_free(transfer_gsl);\n";
    out << "    gsl_vector_free        (eigenValues);\n";
    out << "    gsl_matrix_complex_free(init_gsl);\n";
    out << "    gsl_eigen_hermv_free   (workspace);\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::swap(\n";
    out << "        const size_t i,\n";
    out << "        const size_t j,\n";
    out << "        std::vector<complex> &transfer,\n";
    out << "        std::vector<real>    &mass\n";
    out << "        )\n";
    out << "{\n";
    out << "    const size_t N = mass.size();\n";
    out << "    for (size_t k = 0; k != N; ++k) {\n";
    out << "        std::swap(transfer[index(k, i)], transfer[index(k, "
           "j)]);\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::sort(\n";
    out << "        std::vector<complex> &transfer,\n";
    out << "        std::vector<real>    &mass\n";
    out << "        )\n";
    out << "{\n";
    out << "    for (size_t i = 0; i != mass.size(); ++i) {\n";
    out << "        size_t i_min = i;\n";
    out << "        for (size_t j = i+1; j < mass.size(); ++j) {\n";
    out << "            if (ABS(mass[j]) < ABS(mass[i_min]))\n";
    out << "                i_min = j;\n";
    out << "        }\n";
    out << "        if (i != i_min) {\n";
    out << "            swap(i, i_min, transfer, mass);\n";
    out << "            std::swap(mass[i_min], mass[i]);\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::sort(\n";
    out << "        std::vector<complex> &transfer,\n";
    out << "        std::vector<complex> &transfer2,\n";
    out << "        std::vector<real>    &mass\n";
    out << "        )\n";
    out << "{\n";
    out << "    for (size_t i = 0; i != mass.size(); ++i) {\n";
    out << "        size_t i_min = i;\n";
    out << "        for (size_t j = i+1; j < mass.size(); ++j) {\n";
    out << "            if (ABS(mass[j]) < ABS(mass[i_min]))\n";
    out << "                i_min = j;\n";
    out << "        }\n";
    out << "        if (i != i_min) {\n";
    out << "            swap(i, i_min, transfer, mass);\n";
    out << "            swap(i, i_min, transfer2, mass);\n";
    out << "            std::swap(mass[i_min], mass[i]);\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::diagonalize()\n";
    out << "{\n";
    out << "    diagonalize(m_data, m_transfer, m_mass);\n";
    out << "    sort(m_transfer, m_mass);\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::bidiagonalize()\n";
    out << "{\n";
    out << "    std::vector<complex> m_data_herm = hermitian(m_data);\n";
    out << "    std::vector<complex> m_m_dagger = dot(m_data, m_data_herm);\n";
    out << "    std::vector<complex> m_dagger_m = dot(m_data_herm, m_data);\n";
    out << "\n";
    out << "    std::vector<real> mass2 = m_mass;\n";
    out << "    diagonalize(m_m_dagger, m_transfer2,  m_mass, true);\n";
    out << "    diagonalize(m_dagger_m, m_transfer, mass2,  true);\n";
    out << "    sort(m_transfer2, m_mass);\n";
    out << "    sort(m_transfer, mass2);\n";
    out << "\n";
    out << "    for (size_t i = 0; i != m_N; ++i) {\n";
    out << "        assert(ABS(m_mass[i] - mass2[i]) / ((m_mass[i] == 0) ? 1 "
           ": m_mass[i])\n";
    out << "                        < precision);\n";
    out << "        m_mass[i] = SQRT(m_mass[i]);\n";
    out << "    }\n";
    out << "\n";
    out << "    auto diagMass = dot(dot(hermitian(m_transfer2), m_data), "
           "m_transfer);\n";
    out << "    for (size_t i = 0; i != m_N; ++i) \n";
    out << "        if (REAL(diagMass[index(i, i)]*m_mass[i]) < 0) \n";
    out << "            for (size_t j = 0; j != m_N; ++j)\n";
    out << "                m_transfer[index(j, i)] *= -1;\n";
    out << "\n";
    out << "    for (auto &el : m_transfer2)\n";
    out << "        el = CONJ(el);\n";
    out << "}\n";
    out << "\n";
    out << "gsl_matrix_complex *Diagonalizer::getGSLMassMatrix(\n";
    out << "        std::vector<complex> const &massMatrix\n";
    out << "        ) const \n";
    out << "{\n";
    out << "    gsl_matrix_complex *matrix = gsl_matrix_complex_alloc(m_N, "
           "m_N);\n";
    out << "    for (size_t i = 0; i != m_N; ++i)\n";
    out << "        for (size_t j = 0; j != m_N; ++j)\n";
    out << "            gsl_matrix_complex_set(\n";
    out << "                    matrix, \n";
    out << "                    i, j, \n";
    out << "                    gsl_complex_rect(REAL(massMatrix[index(i, "
           "j)]),\n";
    out << "                                     IMAG(massMatrix[index(i, "
           "j)]))\n";
    out << "                    );\n";
    out << "\n";
    out << "    return matrix;\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::loadGSLMatrix(\n";
    out << "        gsl_matrix_complex const *matrix,\n";
    out << "        std::vector<complex>     &target\n";
    out << "        )\n";
    out << "{\n";
    out << "    target = std::vector<complex>(m_N * m_N);\n";
    out << "    for (size_t i = 0; i != m_N; ++i)\n";
    out << "        for (size_t j = 0; j != m_N; ++j) {\n";
    out << "            auto gslcomplex = gsl_matrix_complex_get(matrix, i, "
           "j);\n";
    out << "            target[index(i, j)] = {GSL_REAL(gslcomplex),\n";
    out << "                                   GSL_IMAG(gslcomplex)};\n";
    out << "        }\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::loadGSLVector(\n";
    out << "        gsl_vector  const *vect,\n";
    out << "        std::vector<real> &target\n";
    out << "        )\n";
    out << "{\n";
    out << "    target = std::vector<real>(m_N);\n";
    out << "    for (size_t i = 0; i != m_N; ++i) \n";
    out << "        target[i] = gsl_vector_get(vect, i);\n";
    out << "}\n";
    out << "\n";
    out << "void Diagonalizer::positiveDiagonal(\n";
    out << "        std::vector<complex> &transfer\n";
    out << "        )\n";
    out << "{\n";
    out << "    for (size_t j = 0; j != m_N; ++j) {\n";
    out << "        if (REAL(transfer[index(j, j)]) < 0) {\n";
    out << "            for (size_t i = 0; i != m_N; ++i)\n";
    out << "                transfer[index(i, j)] *= -1;\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::complex> Diagonalizer::hermitian(\n";
    out << "        std::vector<complex> const &init\n";
    out << "        ) const\n";
    out << "{\n";
    out << "    std::vector<complex> res(init);\n";
    out << "    for (size_t i = 0; i != m_N; ++i)\n";
    out << "        for (size_t j = 0; j != m_N; ++j)\n";
    out << "            res[index(i, j)] = CONJ(init[index(j, i)]);\n";
    out << "    return res;\n";
    out << "}\n";
    out << "\n";
    out << "std::vector<Diagonalizer::complex> Diagonalizer::dot(\n";
    out << "        std::vector<complex> const &A,\n";
    out << "        std::vector<complex> const &B\n";
    out << "        ) const\n";
    out << "{\n";
    out << "    std::vector<complex> C(A);\n";
    out << "    for (size_t i = 0; i != m_N; ++i)\n";
    out << "        for (size_t j = 0; j != m_N; ++j) {\n";
    out << "            C[index(i, j)] = 0;\n";
    out << "            for (size_t k = 0; k != m_N; ++k)\n";
    out << "                C[index(i, j)] += A[index(i, k)] * B[index(k, "
           "j)];\n";
    out << "        }\n";
    out << "    return C;\n";
    out << "}\n";
}

} // End of namespace csl
