#ifndef LIBCOMPLEXOP_H_INCLUDED
#define LIBCOMPLEXOP_H_INCLUDED

#include <complex>
#include <iostream>


#ifdef QUAD

///////////////////////////////////////////////////
// quadruple - double / float operators
///////////////////////////////////////////////////

#include <quadmath.h>

inline std::complex<__float128> operator+(
        std::complex<__float128> const&c, 
        double                         a
        )
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(
        double                         a,
        std::complex<__float128> const&c
        )
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(
        std::complex<__float128> const&c, 
        double                         a
        )
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(
        double                         a,
        std::complex<__float128> const&c
        )
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(
        std::complex<__float128> const&c, 
        double                         a
        )
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(
        double                         a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(
        std::complex<__float128> const&c, 
        double                         a
        )
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(
        double                         a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) / c;
}

// Complex - complex operators

inline std::complex<__float128> operator+(
        std::complex<__float128> const&c, 
        std::complex<double>     const&a
        )
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(
        std::complex<double>     const&a,
        std::complex<__float128> const&c
        )
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(
        std::complex<__float128> const&c, 
        std::complex<double>     const&a
        )
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(
        std::complex<double>     const&a,
        std::complex<__float128> const&c
        )
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(
        std::complex<__float128> const&c, 
        std::complex<double>     const&a
        )
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(
        std::complex<double>     const&a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(
        std::complex<__float128> const&c, 
        std::complex<double>     const&a
        )
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(
        std::complex<double>     const&a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) / c;
}

///////////////////////////////////////////////////
// quadruple / int operators
///////////////////////////////////////////////////

inline std::complex<__float128> operator+(
        std::complex<__float128> const&c, 
        int                            a
        )
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(
        int                            a,
        std::complex<__float128> const&c
        )
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(
        std::complex<__float128> const&c, 
        int                            a
        )
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(
        int                            a,
        std::complex<__float128> const&c
        )
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(
        std::complex<__float128> const&c, 
        int                            a
        )
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(
        int                            a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(
        std::complex<__float128> const&c, 
        int                            a
        )
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(
        int                            a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) / c;
}

// Complex - complex operators

inline std::complex<__float128> operator+(
        std::complex<__float128> const&c, 
        std::complex<int>        const&a
        )
{
    return c + std::complex<__float128>(a);
}
inline std::complex<__float128> operator+(
        std::complex<int>        const&a,
        std::complex<__float128> const&c
        )
{
    return c + std::complex<__float128>(a);
}

inline std::complex<__float128> operator*(
        std::complex<__float128> const&c, 
        std::complex<int>        const&a
        )
{
    return c * std::complex<__float128>(a);
}
inline std::complex<__float128> operator*(
        std::complex<int>        const&a,
        std::complex<__float128> const&c
        )
{
    return c * std::complex<__float128>(a);
}

inline std::complex<__float128> operator-(
        std::complex<__float128> const&c, 
        std::complex<int>        const&a
        )
{
    return c - std::complex<__float128>(a);
}
inline std::complex<__float128> operator-(
        std::complex<int>        const&a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) - c;
}

inline std::complex<__float128> operator/(
        std::complex<__float128> const&c, 
        std::complex<int>        const&a
        )
{
    return c / std::complex<__float128>(a);
}
inline std::complex<__float128> operator/(
        std::complex<int>        const&a,
        std::complex<__float128> const&c
        )
{
    return std::complex<__float128>(a) / c;
}

inline std::ostream &operator<<(
        std::ostream &out,
        __float128    x
        )
{
    out << (double)x;
    return out;
}

inline std::ostream &operator<<(
        std::ostream &out,
        __complex128  z
        )
{
    out << "(" << crealq(z) << " + i*" << cimagq(z) << ")";
    return out;
}

#endif

///////////////////////////////////////////////////
// double / int operators
///////////////////////////////////////////////////

inline std::complex<double> operator+(
        std::complex<double> const&c, 
        int                            a
        )
{
    return c + std::complex<double>(a);
}
inline std::complex<double> operator+(
        int                            a,
        std::complex<double> const&c
        )
{
    return c + std::complex<double>(a);
}

inline std::complex<double> operator*(
        std::complex<double> const&c, 
        int                            a
        )
{
    return c * std::complex<double>(a);
}
inline std::complex<double> operator*(
        int                            a,
        std::complex<double> const&c
        )
{
    return c * std::complex<double>(a);
}

inline std::complex<double> operator-(
        std::complex<double> const&c, 
        int                            a
        )
{
    return c - std::complex<double>(a);
}
inline std::complex<double> operator-(
        int                            a,
        std::complex<double> const&c
        )
{
    return std::complex<double>(a) - c;
}

inline std::complex<double> operator/(
        std::complex<double> const&c, 
        int                            a
        )
{
    return c / std::complex<double>(a);
}
inline std::complex<double> operator/(
        int                            a,
        std::complex<double> const&c
        )
{
    return std::complex<double>(a) / c;
}

// Complex - complex operators

inline std::complex<double> operator+(
        std::complex<double> const&c, 
        std::complex<int>        const&a
        )
{
    return c + std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator+(
        std::complex<int>        const&a,
        std::complex<double> const&c
        )
{
    return c + std::complex<double>(a.real(), a.imag());
}

inline std::complex<double> operator*(
        std::complex<double> const&c, 
        std::complex<int>        const&a
        )
{
    return c * std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator*(
        std::complex<int>        const&a,
        std::complex<double> const&c
        )
{
    return c * std::complex<double>(a.real(), a.imag());
}

inline std::complex<double> operator-(
        std::complex<double> const&c, 
        std::complex<int>        const&a
        )
{
    return c - std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator-(
        std::complex<int>        const&a,
        std::complex<double> const&c
        )
{
    return std::complex<double>(a.real(), a.imag()) - c;
}

inline std::complex<double> operator/(
        std::complex<double> const&c, 
        std::complex<int>        const&a
        )
{
    return c / std::complex<double>(a.real(), a.imag());
}
inline std::complex<double> operator/(
        std::complex<int>        const&a,
        std::complex<double> const&c
        )
{
    return std::complex<double>(a.real(), a.imag()) / c;
}

#endif
