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

#include <cassert>
#include "libdiagonalization.h"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_complex_math.h>
#include <iostream>

#ifdef QUAD
    #include <quadmath.h>
    #define ABS fabsq
    #define SQRT sqrtq
    #define REAL(arg) crealq(arg)
    #define IMAG(arg) cimagq(arg)
    #define CONJ(arg) conjq(arg)
#else
    #define ABS std::abs
    #define SQRT std::sqrt
    #define REAL(arg) (arg).real()
    #define IMAG(arg) (arg).imag()
    #define CONJ(arg) std::conj(arg)
#endif

Diagonalizer::real Diagonalizer::precision = 1e-5;

void Diagonalizer::applyDiagonalization(
        std::initializer_list<complex>  massMatrix,
        std::initializer_list<complex*> transfer,
        std::initializer_list<real*>    masses
        )
{
    Diagonalizer diag(massMatrix);

    std::vector<complex> const &t = diag.transfer();
    size_t i = 0;
    for (auto iter = transfer.begin(); iter != transfer.end(); ++iter)
        **iter = t[i++];

    i = 0;
    std::vector<real> const &m = diag.mass();
    for (auto iter = masses.begin(); iter != masses.end(); ++iter)
        **iter = m[i++];
}

void Diagonalizer::applyBiDiagonalization(
        std::initializer_list<complex>  massMatrix,
        std::initializer_list<complex*> transfer,
        std::initializer_list<complex*> transfer2,
        std::initializer_list<real*>    masses
        )
{
    Diagonalizer diag(massMatrix, true);

    std::vector<complex> const &t = diag.transfer();
    size_t i = 0;
    for (auto iter = transfer.begin(); iter != transfer.end(); ++iter)
        **iter = t[i++];

    i = 0;
    std::vector<complex> const &t2 = diag.transfer2();
    for (auto iter = transfer2.begin(); iter != transfer2.end(); ++iter)
        **iter = t2[i++];

    i = 0;
    std::vector<real> const &m = diag.mass();
    for (auto iter = masses.begin(); iter != masses.end(); ++iter)
        **iter = m[i++];
}

Diagonalizer::Diagonalizer(
        std::initializer_list<complex> massMatrix,
        bool                           t_bidiag
        )
    :m_data(massMatrix),
    m_N(std::sqrt(m_data.size())),
    m_bidiag(t_bidiag),
    m_computed(false)
{
    assert(m_N * m_N == massMatrix.size());
}

std::vector<Diagonalizer::complex> const &Diagonalizer::transfer()
{
    updateDiagonalization();
    return m_transfer;
}

std::vector<Diagonalizer::complex> const &Diagonalizer::transfer2()
{
    updateDiagonalization();
    assert(m_bidiag);
    return m_transfer2;
}

std::vector<Diagonalizer::real> const &
    Diagonalizer::mass()
{
    updateDiagonalization();
    return m_mass;
}

void Diagonalizer::updateDiagonalization()
{
    if (!m_computed) {
        (m_bidiag) ? bidiagonalize() : diagonalize();
        m_computed = true;
    }
}

void Diagonalizer::diagonalize(
        std::vector<complex>  massMatrix,
        std::vector<complex>  &transfer,
        std::vector<real>     &mass,
        bool                  bidiagonalization
        )
{
    transfer = std::vector<complex>(m_N*m_N, 0);
    mass = std::vector<real>(m_N, 0);
    gsl_eigen_hermv_workspace* workspace = gsl_eigen_hermv_alloc(m_N);

    if (!bidiagonalization) {
        // Symmetrize the mass matrix (if it has not been done yet)
        const size_t N = std::sqrt(massMatrix.size());
        for (size_t i = 0; i != N; ++i)
            for (size_t j = i+1; j < N; ++j) {
                massMatrix[index(i, j)] = 
                    (massMatrix[index(i, j)] + massMatrix[index(j, i)]) / 2.;
                massMatrix[index(j, i)] = massMatrix[index(i, j)];
            }
    }
    gsl_matrix_complex* init_gsl     = getGSLMassMatrix(massMatrix);
    gsl_vector*         eigenValues  = gsl_vector_alloc(m_N);
    gsl_matrix_complex* transfer_gsl = gsl_matrix_complex_alloc(m_N, m_N);

    // Diagonalization
    gsl_eigen_hermv(init_gsl, eigenValues, transfer_gsl, workspace);

    // Gathering results
    loadGSLMatrix(transfer_gsl, transfer);
    loadGSLVector(eigenValues,  mass);

    // Freeing allocated memory by gsl
    gsl_matrix_complex_free(transfer_gsl);
    gsl_vector_free        (eigenValues);
    gsl_matrix_complex_free(init_gsl);
    gsl_eigen_hermv_free   (workspace);
}

void Diagonalizer::swap(
        const size_t i,
        const size_t j,
        std::vector<complex> &transfer,
        std::vector<real>    &mass
        )
{
    const size_t N = mass.size();
    for (size_t k = 0; k != N; ++k) {
        std::swap(transfer[index(k, i)], transfer[index(k, j)]);
    }
}

void Diagonalizer::sort(
        std::vector<complex> &transfer,
        std::vector<real>    &mass
        )
{
    for (size_t i = 0; i != mass.size(); ++i) {
        size_t i_min = i;
        for (size_t j = i+1; j < mass.size(); ++j) {
            if (ABS(mass[j]) < ABS(mass[i_min]))
                i_min = j;
        }
        if (i != i_min) {
            swap(i, i_min, transfer, mass);
            std::swap(mass[i_min], mass[i]);
        }
    }
}

void Diagonalizer::sort(
        std::vector<complex> &transfer,
        std::vector<complex> &transfer2,
        std::vector<real>    &mass
        )
{
    for (size_t i = 0; i != mass.size(); ++i) {
        size_t i_min = i;
        for (size_t j = i+1; j < mass.size(); ++j) {
            if (ABS(mass[j]) < ABS(mass[i_min]))
                i_min = j;
        }
        if (i != i_min) {
            swap(i, i_min, transfer, mass);
            swap(i, i_min, transfer2, mass);
            std::swap(mass[i_min], mass[i]);
        }
    }
}

void Diagonalizer::diagonalize()
{
    diagonalize(m_data, m_transfer, m_mass);
    sort(m_transfer, m_mass);
}

void Diagonalizer::bidiagonalize()
{
    std::vector<complex> m_data_herm = hermitian(m_data);
    std::vector<complex> m_m_dagger = dot(m_data, m_data_herm);
    std::vector<complex> m_dagger_m = dot(m_data_herm, m_data);

    std::vector<real> mass2 = m_mass;
    diagonalize(m_m_dagger, m_transfer,  m_mass, true);
    diagonalize(m_dagger_m, m_transfer2, mass2,  true);
    sort(m_transfer, m_mass);
    sort(m_transfer2, mass2);

    for (size_t i = 0; i != m_N; ++i) {
        assert(ABS(m_mass[i] - mass2[i]) / ((m_mass[i] == 0) ? 1 : m_mass[i])
                < precision);
        m_mass[i] = SQRT(m_mass[i]);
    }

    for (auto &el : m_transfer)
        el = CONJ(el);
     sort(m_transfer, m_transfer2, m_mass);
}

gsl_matrix_complex *Diagonalizer::getGSLMassMatrix(
        std::vector<complex> const &massMatrix
        ) const 
{
    gsl_matrix_complex *matrix = gsl_matrix_complex_alloc(m_N, m_N);
    for (size_t i = 0; i != m_N; ++i)
        for (size_t j = 0; j != m_N; ++j)
            gsl_matrix_complex_set(
                    matrix, 
                    i, j, 
                    gsl_complex_rect(REAL(massMatrix[index(i, j)]),
                                     IMAG(massMatrix[index(i, j)]))
                    );

    return matrix;
}

void Diagonalizer::loadGSLMatrix(
        gsl_matrix_complex const *matrix,
        std::vector<complex>     &target
        )
{
    target = std::vector<complex>(m_N * m_N);
    for (size_t i = 0; i != m_N; ++i)
        for (size_t j = 0; j != m_N; ++j) {
            auto gslcomplex = gsl_matrix_complex_get(matrix, i, j);
            target[index(i, j)] = {GSL_REAL(gslcomplex),
                                   GSL_IMAG(gslcomplex)};
        }
}

void Diagonalizer::loadGSLVector(
        gsl_vector  const *vect,
        std::vector<real> &target
        )
{
    target = std::vector<real>(m_N);
    for (size_t i = 0; i != m_N; ++i) 
        target[i] = gsl_vector_get(vect, i);
}

void Diagonalizer::positiveDiagonal(
        std::vector<complex> &transfer
        )
{
    for (size_t j = 0; j != m_N; ++j) {
        if (REAL(transfer[index(j, j)]) < 0) {
            for (size_t i = 0; i != m_N; ++i)
                transfer[index(i, j)] *= -1;
        }
    }
}

std::vector<Diagonalizer::complex> Diagonalizer::hermitian(
        std::vector<complex> const &init
        ) const
{
    std::vector<complex> res(init);
    for (size_t i = 0; i != m_N; ++i)
        for (size_t j = 0; j != m_N; ++j)
            res[index(i, j)] = CONJ(init[index(j, i)]);
    return res;
}

std::vector<Diagonalizer::complex> Diagonalizer::dot(
        std::vector<complex> const &A,
        std::vector<complex> const &B
        ) const
{
    std::vector<complex> C(A);
    for (size_t i = 0; i != m_N; ++i)
        for (size_t j = 0; j != m_N; ++j) {
            C[index(i, j)] = 0;
            for (size_t k = 0; k != m_N; ++k)
                C[index(i, j)] += A[index(i, k)] * B[index(k, j)];
        }
    return C;
}
