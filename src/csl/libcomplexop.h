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
#ifndef LIBCOMPLEXOP_H_INCLUDED
#define LIBCOMPLEXOP_H_INCLUDED

#include <complex>
#include <iostream>

#ifdef QUAD

///////////////////////////////////////////////////
// quadruple - double / float operators
///////////////////////////////////////////////////

#include <quadmath.h>

inline std::complex<__float128> operator+(std::complex<__float128> const &c,
                                          double                          a)
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(double                          a,
                                          std::complex<__float128> const &c)
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(std::complex<__float128> const &c,
                                          double                          a)
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(double                          a,
                                          std::complex<__float128> const &c)
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(std::complex<__float128> const &c,
                                          double                          a)
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(double                          a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(std::complex<__float128> const &c,
                                          double                          a)
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(double                          a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) / c;
}

// Complex - complex operators

inline std::complex<__float128> operator+(std::complex<__float128> const &c,
                                          std::complex<double> const     &a)
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(std::complex<double> const     &a,
                                          std::complex<__float128> const &c)
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(std::complex<__float128> const &c,
                                          std::complex<double> const     &a)
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(std::complex<double> const     &a,
                                          std::complex<__float128> const &c)
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(std::complex<__float128> const &c,
                                          std::complex<double> const     &a)
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(std::complex<double> const     &a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(std::complex<__float128> const &c,
                                          std::complex<double> const     &a)
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(std::complex<double> const     &a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) / c;
}

///////////////////////////////////////////////////
// quadruple / int operators
///////////////////////////////////////////////////

inline std::complex<__float128> operator+(std::complex<__float128> const &c,
                                          int                             a)
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(int                             a,
                                          std::complex<__float128> const &c)
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(std::complex<__float128> const &c,
                                          int                             a)
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(int                             a,
                                          std::complex<__float128> const &c)
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(std::complex<__float128> const &c,
                                          int                             a)
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(int                             a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(std::complex<__float128> const &c,
                                          int                             a)
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(int                             a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) / c;
}

// Complex - complex operators

inline std::complex<__float128> operator+(std::complex<__float128> const &c,
                                          std::complex<int> const        &a)
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(std::complex<int> const        &a,
                                          std::complex<__float128> const &c)
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(std::complex<__float128> const &c,
                                          std::complex<int> const        &a)
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(std::complex<int> const        &a,
                                          std::complex<__float128> const &c)
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(std::complex<__float128> const &c,
                                          std::complex<int> const        &a)
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(std::complex<int> const        &a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(std::complex<__float128> const &c,
                                          std::complex<int> const        &a)
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(std::complex<int> const        &a,
                                          std::complex<__float128> const &c)
{
    return std::complex<__float128>(a) / c;
}

inline std::ostream &operator<<(std::ostream &out, __float128 x)
{
    out << (double) x;
    return out;
}

inline std::ostream &operator<<(std::ostream &out, __complex128 z)
{
    out << "(" << crealq(z) << " + i*" << cimagq(z) << ")";
    return out;
}

#define MTY_REAL(x) crealq(x)
#define MTY_IMAG(x) cimagq(x)
#define MTY_POW(x, y) cpowq(x, y)
#define MTY_COS(x) ccosq(x)
#define MTY_SIN(x) csinq(x)
#define MTY_ACOS(x) cacosq(x)
#define MTY_ASIN(x) casinq(x)
#define MTY_TAN(x) ctanq(x)
#define MTY_ATAN(x) catanq(x)
#define MTY_EXP(x) cexpq(x)
#define MTY_LOG(x) clogq(x)
#define MTY_COSH(x) ccoshq(x)
#define MTY_ACOSH(x) cacoshq(x)
#define MTY_SINH(x) csinhq(x)
#define MTY_ASINH(x) casinhq(x)
#define MTY_TANH(x) ctanhq(x)
#define MTY_ATANH(x) catanhq(x)
#else
#define MTY_REAL(x) x.real()
#define MTY_IMAG(x) x.imag()
#define MTY_POW(x, y) std::pow(x, y)
#define MTY_COS(x) std::cos(x)
#define MTY_SIN(x) std::sin(x)
#define MTY_ACOS(x) std::acos(x)
#define MTY_ASIN(x) std::asin(x)
#define MTY_TAN(x) std::tan(x)
#define MTY_ATAN(x) std::atan(x)
#define MTY_EXP(x) std::exp(x)
#define MTY_LOG(x) std::log(x)
#define MTY_COSH(x) std::cosh(x)
#define MTY_ACOSH(x) std::acosh(x)
#define MTY_SINH(x) std::sinh(x)
#define MTY_ASINH(x) std::asinh(x)
#define MTY_TANH(x) std::tanh(x)
#define MTY_ATANH(x) std::atanh(x)
#endif

///////////////////////////////////////////////////
// double / int operators
///////////////////////////////////////////////////

inline std::complex<double> operator+(std::complex<double> const &c, int a)
{
    return c + std::complex<double>(a);
}
inline std::complex<double> operator+(int a, std::complex<double> const &c)
{
    return c + std::complex<double>(a);
}

inline std::complex<double> operator*(std::complex<double> const &c, int a)
{
    return c * std::complex<double>(a);
}
inline std::complex<double> operator*(int a, std::complex<double> const &c)
{
    return c * std::complex<double>(a);
}

inline std::complex<double> operator-(std::complex<double> const &c, int a)
{
    return c - std::complex<double>(a);
}
inline std::complex<double> operator-(int a, std::complex<double> const &c)
{
    return std::complex<double>(a) - c;
}

inline std::complex<double> operator/(std::complex<double> const &c, int a)
{
    return c / std::complex<double>(a);
}
inline std::complex<double> operator/(int a, std::complex<double> const &c)
{
    return std::complex<double>(a) / c;
}

// Complex - complex operators

inline std::complex<double> operator+(std::complex<double> const &c,
                                      std::complex<int> const    &a)
{
    return c + std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator+(std::complex<int> const    &a,
                                      std::complex<double> const &c)
{
    return c + std::complex<double>(a.real(), a.imag());
}

inline std::complex<double> operator*(std::complex<double> const &c,
                                      std::complex<int> const    &a)
{
    return c * std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator*(std::complex<int> const    &a,
                                      std::complex<double> const &c)
{
    return c * std::complex<double>(a.real(), a.imag());
}

inline std::complex<double> operator-(std::complex<double> const &c,
                                      std::complex<int> const    &a)
{
    return c - std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator-(std::complex<int> const    &a,
                                      std::complex<double> const &c)
{
    return std::complex<double>(a.real(), a.imag()) - c;
}

inline std::complex<double> operator/(std::complex<double> const &c,
                                      std::complex<int> const    &a)
{
    return c / std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator/(std::complex<int> const    &a,
                                      std::complex<double> const &c)
{
    return std::complex<double>(a.real(), a.imag()) / c;
}

#endif
