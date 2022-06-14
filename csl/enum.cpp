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

#include "enum.h"

using namespace std;

namespace csl {

ostream& operator<<(ostream& fout, csl::Type type)
{
    // Allows to display the name of a type and not just 
    // an integer.
    switch(type) {
        case csl::Type::NoType:              fout<<"Arbitrary";           break;
        case csl::Type::Integer:             fout<<"Integer";             break;
        case csl::Type::IntFraction:           fout<<"IntFraction";           break;
        case csl::Type::Float:              fout<<"Float";              break;
        case csl::Type::IntFactorial:          fout<<"IntFactorial";          break;
        case csl::Type::Imaginary:           fout<<"Imaginary";           break;
        case csl::Type::Constant:            fout<<"Constant";            break;
        case csl::Type::Variable:            fout<<"Variable";            break;
        case csl::Type::Sum:                fout<<"Sum";                break;
        case csl::Type::Polynomial:          fout<<"Polynomial";          break;
        case csl::Type::Prod:               fout<<"Prod";               break;
        case csl::Type::Pow:                 fout<<"Pow";                 break;
        case csl::Type::Exp:                 fout<<"Exp";                 break;
        case csl::Type::Log:                 fout<<"Log";                 break;
        case csl::Type::Abs:                 fout<<"Abs";                 break;
        case csl::Type::Cos:                 fout<<"Cos";                 break;
        case csl::Type::Sin:                 fout<<"Sin";                 break;
        case csl::Type::Tan:                 fout<<"Tan";                 break;
        case csl::Type::Cosh:                fout<<"Cosh";                break;
        case csl::Type::Sinh:                fout<<"Sinh";                break;
        case csl::Type::Tanh:                fout<<"Tanh";                break;
        case csl::Type::Derivative:          fout<<"Derivative";          break;
        case csl::Type::Angle:               fout<<"Angle";               break;
        case csl::Type::Factorial:           fout<<"Factorial";           break;
        case csl::Type::ACos:                fout<<"Acos";                break;
        case csl::Type::ASin:                fout<<"Asin";                break;
        case csl::Type::ATan:                fout<<"Atan";                break;
        case csl::Type::ACosh:               fout<<"Acosh";               break;
        case csl::Type::ASinh:               fout<<"Asinh";               break;
        case csl::Type::ATanh:               fout<<"Atanh";               break;
        case csl::Type::RealPart:            fout<<"RealPart";            break;
        case csl::Type::ImaginaryPart:       fout<<"ImaginaryPart";       break;
        case csl::Type::Integral:            fout<<"Atanh";               break;
        case csl::Type::ScalarIntegral:      fout<<"ScalarIntegral";      break;
        case csl::Type::VectorIntegral:      fout<<"VectorIntegral";      break;
        case csl::Type::Vector:              fout<<"Vector";              break;
        case csl::Type::Matrix:              fout<<"Matrix";              break;
        case csl::Type::HighDTensor:         fout<<"HighDTensor";         break;
        case csl::Type::TensorElement:             fout<<"TensorElement";             break;
        case csl::Type::TensorFieldElement:         fout<<"TensorFieldElement";         break;
        case csl::Type::TDerivativeElement: fout<<"TDerivativeElement"; break;

        default: cout<<"Type "<<(int)type<<" not known in perator<<(Type)."; 
    }
    return fout;
}

ostream& operator<<(ostream& fout, csl::PrimaryType primaryType)
{
    // Allows to display the name of a primaryType and not just 
    // an integer.
    switch(primaryType) {
        case csl::PrimaryType::Arbitrary:             cout<<"Arbitrary";             break;
        case csl::PrimaryType::Numerical:             cout<<"Purely numerical";      break;
        case csl::PrimaryType::Literal:               cout<<"Variable Numerical";    break;
        case csl::PrimaryType::MultiFunction:         cout<<"Multi Scalar Function"; break;
        case csl::PrimaryType::ScalarFunction:        cout<<"Scalar Function";       break;
        case csl::PrimaryType::Vectorial:             cout<<"Vectorial";             break;
        case csl::PrimaryType::Indicial:              cout<<"Indicial";              break;
        case csl::PrimaryType::Field:                 cout<<"Field";                 break;

        default: cout<<"Primary Type "<<(int)primaryType;
                 cout<< " not known in operator<<(PrimaryType).";
    }
    return fout;
}

} // End of namespace csl
