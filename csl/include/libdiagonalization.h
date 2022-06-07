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
 * @version 1.3
 
 * \brief
 */
#ifndef DIAGONALIZATION_H_INCLUDED
#define DIAGONALIZATION_H_INCLUDED

#include <iostream>
#include <vector>
#include <complex>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#ifdef QUAD
#include <quadmath.h>
#endif

class Diagonalizer {

public:

#ifdef QUAD
    using real    = __float128;
    using complex = __complex128;
#else
    using real    = double;
    using complex = std::complex<real>;
#endif

    static real precision;

    static void applyDiagonalization(
            std::initializer_list<complex>  massMatrix,
            std::initializer_list<complex*> transfer,
            std::initializer_list<real*>    masses
            );

    static void applyBiDiagonalization(
            std::initializer_list<complex>  massMatrix,
            std::initializer_list<complex*> transfer,
            std::initializer_list<complex*> transfer2,
            std::initializer_list<real*>    masses
            );

    Diagonalizer(
            std::initializer_list<complex> massMatrix,
            bool                           biDiag = false
            );

    ~Diagonalizer() = default;

    std::vector<complex> const &transfer();

    std::vector<complex> const &transfer2();

    std::vector<real> const &mass();

private:

    void updateDiagonalization();

    void diagonalize(
            std::vector<complex>  massMatrix,
            std::vector<complex> &transfer,
            std::vector<real>     &mass,
            bool                  bidiagonalization = false
            );

    void swap(
            const std::size_t i,
            const std::size_t j,
            std::vector<complex> &transfer,
            std::vector<real>    &mass
            );

    void sort(
            std::vector<complex> &transfer,
            std::vector<real>    &mass
            );

    void sort(
            std::vector<complex> &transfer,
            std::vector<complex> &transfer2,
            std::vector<real>    &mass
            );

    void diagonalize();

    void bidiagonalize();

    inline
    std::size_t index(
            const std::size_t i,
            const std::size_t j
            ) const 
    {
        return i * m_N + j;
    }

    gsl_matrix_complex *getGSLMassMatrix(
            std::vector<complex> const &massMatrix
            ) const;

    void loadGSLMatrix(
            gsl_matrix_complex const *matrix,
            std::vector<complex>     &target
            );

    void loadGSLVector(
            gsl_vector  const *vect,
            std::vector<real> &target
            );

    void positiveDiagonal(
            std::vector<complex> &transfer
            );

    std::vector<complex> hermitian(
            std::vector<complex> const &init
            ) const;

    std::vector<complex> dot(
            std::vector<complex> const &A,
            std::vector<complex> const &B
            ) const;

    template<class T>
    void print(std::vector<T> const &v) const
    {
        std::cout << "( ";
        for (const auto &e : v)
            std::cout << (double)crealq(e) << " ";
        std::cout << " )" << std::endl;
    }

private:

    std::vector<complex> m_data;
    std::size_t               m_N;
    bool                 m_bidiag;
    bool                 m_computed;

    std::vector<complex> m_transfer;
    std::vector<complex> m_transfer2;
    std::vector<real>    m_mass;
};

#endif
