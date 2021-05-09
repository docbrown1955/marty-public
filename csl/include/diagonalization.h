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

#ifndef COMPILE_32_BITS
#include <gsl/gsl_complex.h>
#include <gsl/gsl_matrix.h>
#endif
#include "vector.h"

namespace std {
    template<class T>
    class shared_ptr;
}
namespace csl {

class Diagonalizer {

    public:

    explicit
    Diagonalizer(
            const Matrix& matrix,
            bool forceDeterminantZero = false
            );

    explicit
    Diagonalizer(
            const Expr& matrix,
            bool forceDeterminantZero = false
            );

    Diagonalizer(const Diagonalizer& other) = default;

    Diagonalizer& operator=(const Diagonalizer& other) = default;

    void diagonalize();

    csl::vector_expr getEigenValues();

    Expr getDiagonal();

    Expr getTransform();

    Expr getInvTransform();

    private:

    void checkSquareHermitian();

    void checkSpecialCases();

    void diagonalizeNonHermitic();

    void diagonalize_1x1();

    void diagonalize_2x2();

    void diagonalize_3x3();

    static
    double extractNumber(const Expr& number);

#ifndef COMPILE_32_BITS
    static
    gsl_complex csl_to_gsl(const Expr& csl_expr);

    static
    gsl_matrix_complex* csl_to_gsl(const Matrix& csl_matrix);

    static
    Expr gsl_to_csl(gsl_complex gsl_expr);

    static
    Matrix gsl_to_csl(gsl_matrix_complex* gsl_matrix);

    static
    Matrix diagFromEigenValues(gsl_vector* eigenValues);
#endif

    private:

    bool diagonalized = false;

    bool symbolic     = false;

    bool forceDeterminantZero = false;

    Matrix init_matrix;

    Matrix D;

    Matrix U;

    Matrix U_inv;
};


} // End of namespace csl


#endif

