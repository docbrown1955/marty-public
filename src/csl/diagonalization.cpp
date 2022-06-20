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

#ifndef COMPILE_32_BITS
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_errno.h>
#endif

#include "algo.h"
#include "cast.h"
#include "diagonalization.h"
#include "error.h"
#include "interface.h"
#include "simplification.h"

using namespace std;

namespace csl {

Diagonalizer::Diagonalizer(const Matrix &matrix, bool t_forceDeterminantZero)
    : forceDeterminantZero(t_forceDeterminantZero), init_matrix(matrix)
{
}

Diagonalizer::Diagonalizer(const Expr &matrix, bool t_forceDeterminantZero)
    : Diagonalizer(pointer_to_object_ref<Matrix>(matrix),
                   t_forceDeterminantZero)
{
}

void Diagonalizer::diagonalize()
{
    checkSquareHermitian();
    checkSpecialCases();
    if (diagonalized)
        return;

#ifndef COMPILE_32_BITS
    // Initialization of gsl
    const size_t               n           = init_matrix.size();
    gsl_eigen_hermv_workspace *workspace   = gsl_eigen_hermv_alloc(n);
    gsl_matrix_complex *       init_gsl    = csl_to_gsl(init_matrix);
    gsl_vector *               eigenValues = gsl_vector_alloc(n);
    gsl_matrix_complex *       transfer    = gsl_matrix_complex_alloc(n, n);

    // Diagonalization
    gsl_eigen_hermv(init_gsl, eigenValues, transfer, workspace);

    // Gathering results
    D     = diagFromEigenValues(eigenValues);
    U     = gsl_to_csl(transfer);
    U_inv = pointer_to_object<Matrix>(U.hermitian());

    // Freeing allocated memory by gsl
    gsl_eigen_hermv_free(workspace);
    gsl_matrix_complex_free(init_gsl);
    gsl_matrix_complex_free(transfer);
    gsl_vector_free(eigenValues);

    diagonalized = true;
#endif
}

void Diagonalizer::diagonalizeNonHermitic()
{
    return;
    Expr init_save = csl::make_shared<Matrix>(init_matrix);

    init_matrix.print();
    init_matrix
        = pointer_to_object<Matrix>(init_save->hermitian()->dot(init_save));
    init_matrix.print();
    diagonalize();

    Matrix U_save = U;

    init_matrix
        = pointer_to_object<Matrix>(init_save->dot(init_save->hermitian()));

    U_inv = pointer_to_object<Matrix>(U.hermitian());
    U     = U_save;

    U.print();
    U_inv.print();
    D.print();
}

csl::vector_expr Diagonalizer::getEigenValues()
{
    if (not diagonalized)
        diagonalize();
    csl::vector_expr eigen(D.size());
    for (size_t i = 0; i != eigen.size(); ++i)
        eigen[i] = (*D[i])[i];

    return eigen;
}

Expr Diagonalizer::getDiagonal()
{
    if (not diagonalized)
        diagonalize();
    return object_to_shared(D);
}

Expr Diagonalizer::getTransform()
{
    if (not diagonalized)
        diagonalize();
    return object_to_shared(U);
}

Expr Diagonalizer::getInvTransform()
{
    if (not diagonalized)
        diagonalize();
    return object_to_shared(U_inv);
}

void Diagonalizer::checkSpecialCases()
{
    if (diagonalized)
        return;
    switch (init_matrix.size()) {
    case 1:
        diagonalize_1x1();
        break;
    case 2:
        diagonalize_2x2();
        break;
    case 3:
        diagonalize_3x3();
        break;
    }
}

void Diagonalizer::diagonalize_1x1()
{
    D     = init_matrix;
    U     = Matrix({csl::vector_expr({CSL_1})});
    U_inv = U;

    diagonalized = true;
}

Expr findCommonFactor(std::vector<Expr *> const &expressions)
{
    std::vector<Expr> blocks;
    for (const auto &expr : expressions)
        csl::VisitEachLeaf(*expr, [&](Expr const &el) {
            if (el->getPrimaryType() == csl::PrimaryType::Literal) {
                auto pos = std::find(blocks.begin(), blocks.end(), el);
                if (pos == blocks.end())
                    blocks.push_back(el);
            }
        });

    for (const auto &b : blocks) {
        std::vector<Expr> modified;
        modified.reserve(expressions.size());
        for (const auto &expr : expressions) {
            Expr copy = DeepCopy(*expr);
            csl::Transform(copy, [&](Expr &el) {
                if (el->getType() == csl::Type::Sum) {
                    int index = -1;
                    for (size_t i = 0; i != el->size(); ++i)
                        if ((*el)[i]->dependsExplicitlyOn(b.get())) {
                            index = i;
                            break;
                        }
                    if (index == -1)
                        return false;
                    Expr fact = (*el)[index];
                    for (auto &arg : *el)
                        arg = arg / fact;
                    el = fact * el;
                    return true;
                }
                return false;
            });
            modified.push_back(copy);
        }

        std::vector<Expr> X(modified.size());
        for (size_t i = 0; i != modified.size(); ++i) {
            X[i]        = constant_s("x_" + toString(i));
            modified[i] = DeepRefreshed(X[i] * modified[i]);
        }

        Expr seekFactors = Factored(sum_s(modified), true);
        for (size_t i = 0; i != modified.size(); ++i)
            modified[i] = modified[i] / X[i];

        Expr factor = CSL_1;
        if (seekFactors->getType() == csl::Type::Prod) {
            for (const auto &arg : *seekFactors)
                if (arg->getPrimaryType() != csl::PrimaryType::Numerical) {
                    if (arg->dependsExplicitlyOn(X[0].get()))
                        continue;
                    for (auto &m : modified) {
                        m = m / arg;
                    }
                    factor = factor * arg;
                }
        }
        if (factor != CSL_1) {
            for (size_t i = 0; i != expressions.size(); ++i)
                *expressions[i] = modified[i];
            return factor;
        }
    }
    return CSL_1;
}

void Diagonalizer::diagonalize_2x2()
{
    // Diagonalizing
    // ( a  , c )
    // ( c* , b )
    // with a, b real numbers and c complex

    auto simpli = [](Expr const &init) {
        Expr other = init->deepCopy();
        Expand(other);
        Factor(other, true);
        return other;
    };

    Expr a           = init_matrix[0][0];
    Expr b           = init_matrix[1][1];
    Expr c           = init_matrix[0][1];
    Expr factor      = findCommonFactor({&a, &b, &c});
    Expr c_star      = GetComplexConjugate(c);
    Expr determinant = a * b - c * c_star;
    Expand(determinant, true);
    if (forceDeterminantZero or determinant == CSL_0) {
        Expr eigenValue = simpli(factor * (a + b));
        D     = pointer_to_object<Matrix>(diagonal_s({CSL_0, eigenValue}));
        U     = Matrix(2);
        U_inv = Matrix(2);

        Expr norm1  = sqrt_s(simpli(b * b + c * c));
        Expr norm2  = sqrt_s(simpli(b * b + c_star * c_star));
        Expr value1 = b / norm1;
        Expr value2 = c / norm1;
        Expr value3 = -c_star / norm2;
        Expr value4 = b / norm2;
        U[0][0]     = value1;
        U[0][1]     = value2;
        U[1][0]     = value3;
        U[1][1]     = value4;

        U_inv[0][0] = value4;
        U_inv[0][1] = value2;
        U_inv[1][0] = value3;
        U_inv[1][1] = value1;

        diagonalized = true;
        symbolic     = true;
        return;
    }
    else if (init_matrix[0][0] == init_matrix[1][1]
             and init_matrix[0][1] == init_matrix[1][0]) {
        D     = pointer_to_object<Matrix>(diagonal_s({a - c, a + c}));
        U     = Matrix(2);
        U_inv = Matrix(2);

        U[0][0] = 1 / csl::sqrt_s(2);
        U[0][1] = -1 / csl::sqrt_s(2);
        U[1][0] = 1 / csl::sqrt_s(2);
        U[1][1] = 1 / csl::sqrt_s(2);

        U_inv[0][0] = 1 / csl::sqrt_s(2);
        U_inv[0][1] = -1 / csl::sqrt_s(2);
        U_inv[1][0] = 1 / csl::sqrt_s(2);
        U_inv[1][1] = 1 / csl::sqrt_s(2);

        diagonalized = true;
        symbolic     = true;
        return;
    }

    Expr delta = (a + b) * (a + b) + 4 * (c * c_star - a * b);
    delta      = DeepRefreshed(delta);

    Expr lambda_plus  = CSL_1 / 2 * (a + b + sqrt_s(delta));
    Expr lambda_minus = CSL_1 / 2 * (a + b - sqrt_s(delta));
    Expand(lambda_plus, true);
    Factor(lambda_plus);
    Expand(lambda_minus, true);
    Factor(lambda_minus);

    Expr factor_plus = (lambda_plus - a) / c;
    if (Expr trial = c_star / (lambda_plus - b); trial < factor_plus)
        factor_plus = trial;

    Expr factor_minus = (lambda_minus - a) / c;
    if (Expr trial = c_star / (lambda_minus - b); trial < factor_minus)
        factor_minus = trial;

    Expr norm_plus
        = 1 / sqrt_s(1 + factor_plus * GetComplexConjugate(factor_plus));
    Expr norm_minus
        = 1 / sqrt_s(1 + factor_minus * GetComplexConjugate(factor_minus));

    norm_plus  = DeepRefreshed(norm_plus);
    norm_minus = DeepRefreshed(norm_minus);

    D = pointer_to_object<Matrix>(
        diagonal_s({factor * lambda_plus, factor * lambda_minus}));
    U     = Matrix(2);
    U_inv = Matrix(2);

    (*U[0])[0] = 1 / norm_plus;
    (*U[0])[1] = 1 / norm_minus;
    (*U[1])[0] = factor_plus / norm_plus;
    (*U[1])[1] = factor_minus / norm_minus;

    Expr det = U.determinant();

    (*U_inv[1])[1] = 1 / norm_plus / det;
    (*U_inv[1])[0] = -1 / norm_minus / det;
    (*U_inv[0])[1] = -factor_plus / norm_plus / det;
    (*U_inv[0])[0] = factor_minus / norm_minus / det;

    diagonalized = true;
    symbolic     = true;
}

void Diagonalizer::diagonalize_3x3()
{
}

void Diagonalizer::checkSquareHermitian()
{
    vector<int> shape = init_matrix.getShape();
    if (shape[0] != shape[1]) {
        callError(cslError::UndefinedBehaviour,
                  "Diagonalizer::checkSquareHermitian().");
    }
    for (size_t i = 0; i != init_matrix.size(); ++i)
        for (size_t j = i; j != init_matrix.size(); ++j)
            if (i == j) {
                if (not(*init_matrix[i])[j]->isReal()) {
                    diagonalizeNonHermitic();
                    return;
                }
            }
            else if ((*init_matrix[i])[j]
                     != (*init_matrix[j])[i]->getComplexConjugate()) {
                diagonalizeNonHermitic();
                return;
            }
}

double Diagonalizer::extractNumber(const Expr &number)
{

    if (number->getType() == csl::Type::Prod and number->size() == 2
        and (*number)[1] == CSL_I)
        return extractNumber((*number)[0]);

    if (number == CSL_I)
        return 1;

    if (number->getPrimaryType() != csl::PrimaryType::Numerical) {
        number->print();
        callError(cslError::UndefinedBehaviour,
                  "Diagonalizer::extractNumber(const Expr&).");
    }
    return number->evaluateScalar();
}

#ifndef COMPILE_32_BITS
gsl_complex Diagonalizer::csl_to_gsl(const Expr &expr)
{
    double realPart      = 0;
    double imaginaryPart = 0;

    if (expr->getType() == csl::Type::Sum) {
        if (expr->getNArgs() > 2) {
            expr->print();
            callError(cslError::UndefinedBehaviour,
                      "Diagonalizer::csl_to_gsl(const Expr&).");
        }
        realPart = extractNumber((*expr)[0]);
        if (expr->size() > 1) {
            imaginaryPart = extractNumber((*expr)[1]);
        }
    }
    else if (expr->getType() == csl::Type::Prod or expr == CSL_I)
        imaginaryPart = extractNumber(expr);
    else
        realPart = extractNumber(expr);

    return gsl_complex_rect(realPart, imaginaryPart);
}

gsl_matrix_complex *Diagonalizer::csl_to_gsl(const Matrix &csl_matrix)
{
    vector<int>         shape = csl_matrix.getShape();
    gsl_matrix_complex *gsl_matrix
        = gsl_matrix_complex_alloc(shape[0], shape[1]);
    size_t i = 0;
    for (auto row = csl_matrix.begin(); row != csl_matrix.end(); ++row) {
        size_t j = 0;
        for (auto col = (*row)->begin(); col != (*row)->end(); ++col) {
            gsl_matrix_complex_set(gsl_matrix, i, j, csl_to_gsl(*col));
            ++j;
        }
        ++i;
    }

    return gsl_matrix;
}

Expr Diagonalizer::gsl_to_csl(gsl_complex gsl_expr)
{
    return float_s(GSL_REAL(gsl_expr)) + float_s(GSL_IMAG(gsl_expr)) * CSL_I;
}

Matrix Diagonalizer::gsl_to_csl(gsl_matrix_complex *gsl_matrix)
{
    vector<csl::vector_expr> elements(gsl_matrix->size1,
                                      csl::vector_expr(gsl_matrix->size2));
    for (size_t i = 0; i != gsl_matrix->size1; ++i)
        for (size_t j = 0; j != gsl_matrix->size2; ++j)
            elements[i][j]
                = gsl_to_csl(gsl_matrix_complex_get(gsl_matrix, i, j));

    return Matrix(elements);
}

Matrix Diagonalizer::diagFromEigenValues(gsl_vector *eigenValues)
{
    csl::vector_expr csl_values(eigenValues->size);
    for (size_t i = 0; i != eigenValues->size; ++i)
        csl_values[i] = float_s(gsl_vector_get(eigenValues, i));

    return pointer_to_object<Matrix>(diagonal_s(csl_values));
}
#endif

} // End of namespace csl
