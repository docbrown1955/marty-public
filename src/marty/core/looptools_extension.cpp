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

#include "looptools_extension.h"
#include "feynmanIntegral.h"
#include "mrtError.h"

// Because of macro definitions, this include must be the last one
// Do not put other includes after it, especially looptools_extension.h
#include "clooptools.h"

using namespace csl;
namespace mty {

///////////////////////////////////////////////////
// Conversion functions
///////////////////////////////////////////////////

csl::Expr complexToExpr(ltComplexType const &complexNumber)
{
    return csl::complex_s(csl::autonumber_s(complexNumber.real()),
                          csl::autonumber_s(complexNumber.imag()));
}

ltComplexType exprToComplexType(csl::Expr const &init)
{
    csl::Expr eval = Evaluated(init, csl::eval::all);
    HEPAssert(eval->getPrimaryType() == csl::PrimaryType::Numerical,
              mty::error::TypeError,
              "Expression " + toString(init)
                  + " does not evaluate completely, "
                    "resulting expr: "
                  + toString(eval));

    ltComplexType z;
    z.real(GetRealPart(eval)->evaluateScalar());
    z.imag(GetImaginaryPart(eval)->evaluateScalar());

    return z;
}

std::vector<ltComplexType>
exprToComplexType(std::vector<csl::Expr> const &init)
{
    std::vector<ltComplexType> complexNumbers(init.size());
    std::transform(init.begin(),
                   init.end(),
                   complexNumbers.begin(),
                   [](csl::Expr const &el) { return exprToComplexType(el); });

    return complexNumbers;
}

double exprToRealType(csl::Expr const &init)
{
    csl::Expr eval = Evaluated(init, csl::eval::all);
    HEPAssert(eval->getPrimaryType() == csl::PrimaryType::Numerical,
              mty::error::TypeError,
              "Expression " + toString(init)
                  + " does not evaluate completely, "
                    "resulting expr: "
                  + toString(eval));

    ltComplexType z;
    z.real(GetRealPart(eval)->evaluateScalar());
    z.imag(GetImaginaryPart(eval)->evaluateScalar());

    if (z.imag() != 0)
        std::cerr << "Warning: value " << z.real() << " + i" << z.imag()
                  << " has a non zero imaginary part, ignored in "
                     "computations.\n";

    return z.real();
}

std::vector<double> exprToRealType(std::vector<csl::Expr> const &init)
{
    std::vector<double> numbers(init.size());
    std::transform(init.begin(),
                   init.end(),
                   numbers.begin(),
                   [](csl::Expr const &el) { return exprToRealType(el); });

    return numbers;
}

std::optional<std::vector<ltRealType>>
isReal(std::vector<ltComplexType> const &init)
{
    std::vector<ltRealType> realNumbers(init.size());
    for (size_t i = 0; i != init.size(); ++i)
        if (init[i].imag() != 0)
            return std::nullopt;
        else
            realNumbers[i] = init[i].real();

    return realNumbers;
}

///////////////////////////////////////////////////
// Functions taking ltRealType
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<ltRealType> const &arguments)
{
    HEPAssert(arguments.size() == 1,
              mty::error::RuntimeError,
              "Wrong number of arguments for A0i, "
                  + toString(arguments.size()) + " given, 1 expected.");
    return A0i(id, arguments[0]);
}

ltComplexType B0i_runtime(int id, std::vector<ltRealType> const &arguments)
{
    HEPAssert(arguments.size() == 3,
              mty::error::RuntimeError,
              "Wrong number of arguments for B0i, "
                  + toString(arguments.size()) + " given, 3 expected.");
    return B0i(id, arguments[0], arguments[1], arguments[2]);
}

ltComplexType C0i_runtime(int id, std::vector<ltRealType> const &arguments)
{
    HEPAssert(arguments.size() == 6,
              mty::error::RuntimeError,
              "Wrong number of arguments for C0i, "
                  + toString(arguments.size()) + " given, 6 expected.");
    return C0i(id,
               arguments[0],
               arguments[1],
               arguments[2],
               arguments[3],
               arguments[4],
               arguments[5]);
}

ltComplexType D0i_runtime(int id, std::vector<ltRealType> const &arguments)
{
    HEPAssert(arguments.size() == 10,
              mty::error::RuntimeError,
              "Wrong number of arguments for D0i, "
                  + toString(arguments.size()) + " given, 10 expected.");
    return D0i(id,
               arguments[0],
               arguments[1],
               arguments[2],
               arguments[3],
               arguments[4],
               arguments[5],
               arguments[6],
               arguments[7],
               arguments[8],
               arguments[9]);
}

ltComplexType E0i_runtime(int id, std::vector<ltRealType> const &arguments)
{
    HEPAssert(arguments.size() == 15,
              mty::error::RuntimeError,
              "Wrong number of arguments for E0i, "
                  + toString(arguments.size()) + " given, 15 expected.");
    return E0i(id,
               arguments[0],
               arguments[1],
               arguments[2],
               arguments[3],
               arguments[4],
               arguments[5],
               arguments[6],
               arguments[7],
               arguments[8],
               arguments[9],
               arguments[10],
               arguments[11],
               arguments[12],
               arguments[13],
               arguments[14]);
}

///////////////////////////////////////////////////
// Functions taking complex
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<ltComplexType> const &arguments)
{
    if (auto reals = isReal(arguments); reals)
        return A0i_runtime(id, reals.value());
    HEPAssert(arguments.size() == 1,
              mty::error::RuntimeError,
              "Wrong number of arguments for A0i, "
                  + toString(arguments.size()) + " given, 1 expected.");
    return A0iC(id, arguments[0]);
}

ltComplexType B0i_runtime(int id, std::vector<ltComplexType> const &arguments)
{
    if (auto reals = isReal(arguments); reals)
        return B0i_runtime(id, reals.value());
    HEPAssert(arguments.size() == 3,
              mty::error::RuntimeError,
              "Wrong number of arguments for B0i, "
                  + toString(arguments.size()) + " given, 3 expected.");
    return B0iC(id, arguments[0], arguments[1], arguments[2]);
}

ltComplexType C0i_runtime(int id, std::vector<ltComplexType> const &arguments)
{
    if (auto reals = isReal(arguments); reals)
        return C0i_runtime(id, reals.value());
    HEPAssert(arguments.size() == 6,
              mty::error::RuntimeError,
              "Wrong number of arguments for C0i, "
                  + toString(arguments.size()) + " given, 6 expected.");
    return C0iC(id,
                arguments[0],
                arguments[1],
                arguments[2],
                arguments[3],
                arguments[4],
                arguments[5]);
}

ltComplexType D0i_runtime(int id, std::vector<ltComplexType> const &arguments)
{
    if (auto reals = isReal(arguments); reals)
        return D0i_runtime(id, reals.value());
    HEPAssert(arguments.size() == 10,
              mty::error::RuntimeError,
              "Wrong number of arguments for D0i, "
                  + toString(arguments.size()) + " given, 10 expected.");
    return D0iC(id,
                arguments[0],
                arguments[1],
                arguments[2],
                arguments[3],
                arguments[4],
                arguments[5],
                arguments[6],
                arguments[7],
                arguments[8],
                arguments[9]);
}

ltComplexType E0i_runtime(int id, std::vector<ltComplexType> const &arguments)
{
    if (auto reals = isReal(arguments); reals)
        return E0i_runtime(id, reals.value());
    HEPAssert(arguments.size() == 15,
              mty::error::RuntimeError,
              "Wrong number of arguments for E0i, "
                  + toString(arguments.size()) + " given, 15 expected.");
    return E0iC(id,
                arguments[0],
                arguments[1],
                arguments[2],
                arguments[3],
                arguments[4],
                arguments[5],
                arguments[6],
                arguments[7],
                arguments[8],
                arguments[9],
                arguments[10],
                arguments[11],
                arguments[12],
                arguments[13],
                arguments[14]);
}

///////////////////////////////////////////////////
// Functions taking csl::Expr
///////////////////////////////////////////////////

ltComplexType A0i_runtime(int id, std::vector<csl::Expr> const &arguments)
{
    return A0i_runtime(id, exprToComplexType(arguments));
}

ltComplexType B0i_runtime(int id, std::vector<csl::Expr> const &arguments)
{
    return B0i_runtime(id, exprToComplexType(arguments));
}

ltComplexType C0i_runtime(int id, std::vector<csl::Expr> const &arguments)
{
    return C0i_runtime(id, exprToComplexType(arguments));
}

ltComplexType D0i_runtime(int id, std::vector<csl::Expr> const &arguments)
{
    return D0i_runtime(id, exprToComplexType(arguments));
}

ltComplexType E0i_runtime(int id, std::vector<csl::Expr> const &arguments)
{
    return E0i_runtime(id, exprToComplexType(arguments));
}

} // namespace mty
