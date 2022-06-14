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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Some of csl enumerations
 */
#ifndef ENUM_H_INCLUDED
#define ENUM_H_INCLUDED

#include <iostream>

namespace csl {

/*!
 * \brief Stores enumerations of types for Abstract objects.
 */
    enum class PrimaryType {

        Arbitrary=-1,
        Numerical, /*!< = 0. Concerns Number and IntFraction. */
        Literal, /*!< = 1. Concerns Variable, IntFactorial and Imaginary. */
        MultiFunction=10, /*!< = 10. Concerns all scalar multi-variate functions
                            (Sum, Prod, Pow, etc). */
        ScalarFunction=20, /*!< = 20. Concerns all scalar uni-variate functions
                             (Exp, Log, Cos, etc). */
        Vectorial=50, /*!<  = 50. Concerns Vector, Matrix and HighDTensor. */
        Indicial=60, /*!<  = 60. Concerns all indicial exprs. */
        Field=70, /*!<  = 70. Concerns all Fields exprs. */
    };

    /*! \brief Enum of the different types of Abstract
      (i.e. list of all possible specializations).*/
    enum class Type {

        InheritanceType=-2,
        NoType=-1, /*!<  Arbitrary Abstract in comparisons */

        Integer, /*!<  Abstract specialized in Integer */
        Float, /*!<  Abstract specialized in Float */
        IntFraction, /*!<  Abstract specialized in IntFraction */
        IntFactorial, /*!<  Abstract specialized in IntFactorial */
        Complex, /*!< 4. Abstract specialized in Complex */
        NumericalEval, /*!< Abstract specialized in NumericalEval */
        Imaginary, /*!<  Abstract specialized in Imaginary */
        Constant, /*!<  Abstract specialized in Constant */
        Variable, /*!<  Abstract specialized in Variable */
        Exp, /*!< Abstract specialized in Exp */
        Log, /*!< Abstract specialized in Log */
        DiracDelta,  /*!< Abstract specialized in Dirac Delta */
        Abs, /*!< Abstract specialized in Abs */
        Cos, /*!< Abstract specialized in Cos */
        Sin, /*!< Abstract specialized in Sin */
        Tan, /*!< Abstract specialized in Tan */
        Cosh, /*!< Abstract specialized in Cosh */
        Sinh, /*!< Abstract specialized in Sinh */
        Tanh, /*!< Abstract specialized in Tanh */
        ACos, /*!< Abstract specialized in ACos */
        ASin, /*!< Abstract specialized in ASin */
        ATan, /*!< Abstract specialized in ATan */
        ACosh, /*!< Abstract specialized in ACosh */
        ASinh, /*!< Abstract specialized in ASinh */
        ATanh, /*!< Abstract specialized in ATanh */
        Scalar, /*!<Abstract specialized in Scalar. */
        RealPart, /*!<Abstract specialized in real part. */
        ImaginaryPart, /*!<Abstract specialized in imaginary part.*/
        BooleanOperator, /*!< Abstract specialized in Boolean operator. */

        TensorElement, /*!< Abstract specialized in TensorElement */
        
        ScalarField, /*!< Abstract specialized in ScalarField. */
        TensorFieldElement, /*!< Abstract specialized in TensorFieldElement. */
        TDerivativeElement, /*<!Abstract specialized in
                               TDerivativeElement */

        StandardDuo, /*!< Abstract specialized in StandardDuo */
        StandardMult,  /*!< Abstract specialized in StandardDuo */
        Pow, /*!< Abstract specialized in Pow */
        Prod, /*!< Abstract specialized in Prod */
        Sum, /*!< Abstract specialized in Sum */
        Polynomial, /*!< Abstract specialized in Polynomial */
        Derivative, /*!< Abstract specialized in Derivative */
        Commutator, /*!< Abstract specialized in Commutator */
        Angle, /*!< Abstract specialized in Angle */
        Factorial, /*!< Abstract specialized in Factorial */
        Integral, /*!< Abstract specialized in Integral */
        ScalarIntegral, /*!<Abstract specialized in ScalarIntegral.*/
        VectorIntegral, /*!<Abstract specialized in VectorIntegral.*/

        Vector, /*!< Abstract specialized in Vector */
        Matrix, /*!< Abstract specialized in Matrix */
        HighDTensor, /*!< Abstract specialized in HighDTensor */
        // Type extensions for further librairies
    };

/*! 
 * \brief Displays the name of a given csl::Type in order to be readable.
 * \param fout Out stream in which the type is send.
 * \param type Type to display.
 * \return \b fout
 */
std::ostream& operator<<(std::ostream& fout, csl::Type type);

/*! \brief Displays the name of a given csl::PrimaryType in order to be readable.
 * \param fout Out stream in which the type is send.
 * \param type PrimaryType to display.
 * \return \b fout
 */
std::ostream& operator<<(std::ostream& fout, csl::PrimaryType primaryType);

/*!
 * \brief Contains all possible complex properties of objects. Real, purely
 * imaginary, or complex.
 */
enum class ComplexProperty{

    /*!
     * \brief complex object, \f$ z^* \neq z. \f$
     */
    Complex,
    /*!
     * \brief real object, \f$ z^* = z. \f$
     */
    Real,
    /*!
     * \brief imaginary object, \f$ z^* = -z. \f$
     */
    Imaginary,
};

} // End of namespace csl

#endif
