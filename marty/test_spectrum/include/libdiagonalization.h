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
            const size_t i,
            const size_t j,
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
    size_t index(
            const size_t i,
            const size_t j
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

    real det(
            std::vector<complex> const &A
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
    size_t               m_N;
    bool                 m_bidiag;
    bool                 m_computed;

    std::vector<complex> m_transfer;
    std::vector<complex> m_transfer2;
    std::vector<real>    m_mass;
};

#endif
