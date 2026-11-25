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

 * \brief Printers for the C implementation of the diagonalization core.
 */
#pragma once

#include <iostream>

namespace csl {

void print_cdiagonalization_hdata(std::ostream &out)
{
    out << R"(#ifndef CSL_LIB_CDIAGONALIZATION_H_INCLUDED
#define CSL_LIB_CDIAGONALIZATION_H_INCLUDED

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Complex matrices are stored row-major as interleaved (real, imag) pairs.
// Eigenvectors are the columns of the transfer matrices.
void c_diagonalize_hermitian(
        size_t        N,
        const double *massMatrix,
        double       *transfer,
        double       *mass,
        bool          symmetrize
        );

void c_bidiagonalize(
        size_t        N,
        const double *massMatrix,
        double       *transfer,
        double       *transfer2,
        double       *mass,
        double        precision
        );

#ifdef __cplusplus
} // extern "C" block
#endif

#endif
)";
}

void print_cdiagonalization_cdata(std::ostream &out)
{
    out << R"(#include "cdiagonalization.h"

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

typedef double _Complex cplx;

static cplx *load(size_t n, const double *src)
{
    cplx *res = malloc(n * sizeof *res);
    memcpy(res, src, n * sizeof *res);
    return res;
}

static void store(size_t n, const cplx *src, double *dst)
{
    memcpy(dst, src, n * sizeof *src);
}

static void eigen_hermitian(size_t N, const cplx *matrix, cplx *transfer,
                            double *mass)
{
    gsl_matrix_complex *init = gsl_matrix_complex_alloc(N, N);
    gsl_matrix_complex *vectors = gsl_matrix_complex_alloc(N, N);
    gsl_vector *values = gsl_vector_alloc(N);
    gsl_eigen_hermv_workspace *workspace = gsl_eigen_hermv_alloc(N);

    for (size_t i = 0; i != N; ++i)
        for (size_t j = 0; j != N; ++j) {
            const cplx z = matrix[i * N + j];
            gsl_matrix_complex_set(init, i, j,
                                   gsl_complex_rect(creal(z), cimag(z)));
        }

    gsl_eigen_hermv(init, values, vectors, workspace);

    for (size_t i = 0; i != N; ++i)
        for (size_t j = 0; j != N; ++j) {
            const gsl_complex z = gsl_matrix_complex_get(vectors, i, j);
            transfer[i * N + j] = CMPLX(GSL_REAL(z), GSL_IMAG(z));
        }
    for (size_t i = 0; i != N; ++i)
        mass[i] = gsl_vector_get(values, i);

    gsl_matrix_complex_free(init);
    gsl_matrix_complex_free(vectors);
    gsl_vector_free(values);
    gsl_eigen_hermv_free(workspace);
}

static void swap_columns(size_t N, cplx *matrix, size_t i, size_t j)
{
    for (size_t k = 0; k != N; ++k) {
        const cplx tmp = matrix[k * N + i];
        matrix[k * N + i] = matrix[k * N + j];
        matrix[k * N + j] = tmp;
    }
}

static void sort_by_mass(size_t N, cplx *transfer, cplx *transfer2,
                         double *mass)
{
    for (size_t i = 0; i != N; ++i) {
        size_t i_min = i;
        for (size_t j = i + 1; j < N; ++j)
            if (fabs(mass[j]) < fabs(mass[i_min]))
                i_min = j;
        if (i != i_min) {
            swap_columns(N, transfer, i, i_min);
            if (transfer2)
                swap_columns(N, transfer2, i, i_min);
            const double tmp = mass[i];
            mass[i] = mass[i_min];
            mass[i_min] = tmp;
        }
    }
}

static void hermitian(size_t N, const cplx *A, cplx *res)
{
    for (size_t i = 0; i != N; ++i)
        for (size_t j = 0; j != N; ++j)
            res[i * N + j] = conj(A[j * N + i]);
}

static void product(size_t N, const cplx *A, const cplx *B, cplx *res)
{
    for (size_t i = 0; i != N; ++i)
        for (size_t j = 0; j != N; ++j) {
            cplx sum = 0;
            for (size_t k = 0; k != N; ++k)
                sum += A[i * N + k] * B[k * N + j];
            res[i * N + j] = sum;
        }
}

void c_diagonalize_hermitian(size_t N, const double *massMatrix,
                             double *transfer, double *mass, bool symmetrize)
{
    cplx *matrix = load(N * N, massMatrix);
    cplx *vectors = malloc(N * N * sizeof *vectors);

    if (symmetrize) {
        for (size_t i = 0; i != N; ++i)
            for (size_t j = i + 1; j < N; ++j) {
                matrix[i * N + j] = (matrix[i * N + j] + matrix[j * N + i]) / 2.;
                matrix[j * N + i] = matrix[i * N + j];
            }
    }
    eigen_hermitian(N, matrix, vectors, mass);
    sort_by_mass(N, vectors, NULL, mass);
    store(N * N, vectors, transfer);

    free(vectors);
    free(matrix);
}

void c_bidiagonalize(size_t N, const double *massMatrix, double *transfer,
                     double *transfer2, double *mass, double precision)
{
    const size_t size = N * N;
    cplx *data = load(size, massMatrix);
    cplx *data_herm = malloc(size * sizeof *data_herm);
    cplx *m_m_dagger = calloc(size, sizeof *m_m_dagger);
    cplx *m_dagger_m = calloc(size, sizeof *m_dagger_m);
    cplx *tmp = malloc(size * sizeof *tmp);
    cplx *diag = malloc(size * sizeof *diag);
    cplx *left = malloc(size * sizeof *left);
    cplx *right = malloc(size * sizeof *right);
    double *mass2 = malloc(N * sizeof *mass2);

    hermitian(N, data, data_herm);
    product(N, data, data_herm, m_m_dagger);
    product(N, data_herm, data, m_dagger_m);

    eigen_hermitian(N, m_m_dagger, left, mass);
    eigen_hermitian(N, m_dagger_m, right, mass2);
    sort_by_mass(N, left, NULL, mass);
    sort_by_mass(N, right, NULL, mass2);

    for (size_t i = 0; i != N; ++i) {
        assert(fabs(mass[i] - mass2[i])
                   / ((mass[i] == 0) ? 1 : mass[i])
               < precision);
        mass[i] = sqrt(mass[i]);
    }

    hermitian(N, left, tmp);
    product(N, tmp, data, data_herm);
    product(N, data_herm, right, diag);
    for (size_t i = 0; i != N; ++i)
        if (creal(diag[i * N + i] * mass[i]) < 0)
            for (size_t j = 0; j != N; ++j)
                right[j * N + i] *= -1;

    for (size_t k = 0; k != size; ++k)
        left[k] = conj(left[k]);

    store(size, right, transfer);
    store(size, left, transfer2);

    free(mass2);
    free(right);
    free(left);
    free(diag);
    free(tmp);
    free(m_dagger_m);
    free(m_m_dagger);
    free(data_herm);
    free(data);
}
)";
}

} // End of namespace csl
