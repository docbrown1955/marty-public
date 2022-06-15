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
#ifndef CSL_LIB_LOOPTOOLS_H_INCLUDED
#define CSL_LIB_LOOPTOOLS_H_INCLUDED

#include "clooptools.h"
#include <complex>
#include <quadmath.h>

namespace mty {

inline __complex128 A0iCq(int id, __complex128 const &m1)
{
    const std::complex<__float128> res
        = A0iC(id, std::complex<__float128>(crealq(m1), cimagq(m1)));
    return __complex128{res.real(), res.imag()};
}

inline __complex128 B0iCq(int                 id,
                          __complex128 const &p1,
                          __complex128 const &m1,
                          __complex128 const &m2)
{
    const std::complex<__float128> res
        = B0iC(id,
               std::complex<__float128>(crealq(p1), cimagq(p1)),
               std::complex<__float128>(crealq(m1), cimagq(m1)),
               std::complex<__float128>(crealq(m2), cimagq(m2)));
    return __complex128{res.real(), res.imag()};
}

inline __complex128 C0iCq(int                 id,
                          __complex128 const &p1,
                          __complex128 const &p2,
                          __complex128 const &p3,
                          __complex128 const &m1,
                          __complex128 const &m2,
                          __complex128 const &m3)
{
    const std::complex<__float128> res
        = ::C0iC(id,
                 std::complex<__float128>(crealq(p1), cimagq(p1)),
                 std::complex<__float128>(crealq(p2), cimagq(p2)),
                 std::complex<__float128>(crealq(p3), cimagq(p3)),
                 std::complex<__float128>(crealq(m1), cimagq(m1)),
                 std::complex<__float128>(crealq(m2), cimagq(m2)),
                 std::complex<__float128>(crealq(m3), cimagq(m3)));
    return __complex128{res.real(), res.imag()};
}

inline __complex128 D0iCq(int                 id,
                          __complex128 const &p1,
                          __complex128 const &p2,
                          __complex128 const &p3,
                          __complex128 const &p4,
                          __complex128 const &p5,
                          __complex128 const &p6,
                          __complex128 const &m1,
                          __complex128 const &m2,
                          __complex128 const &m3,
                          __complex128 const &m4)
{
    const std::complex<__float128> res
        = ::D0iC(id,
                 std::complex<__float128>(crealq(p1), cimagq(p1)),
                 std::complex<__float128>(crealq(p2), cimagq(p2)),
                 std::complex<__float128>(crealq(p3), cimagq(p3)),
                 std::complex<__float128>(crealq(p4), cimagq(p4)),
                 std::complex<__float128>(crealq(p5), cimagq(p5)),
                 std::complex<__float128>(crealq(p6), cimagq(p6)),
                 std::complex<__float128>(crealq(m1), cimagq(m1)),
                 std::complex<__float128>(crealq(m2), cimagq(m2)),
                 std::complex<__float128>(crealq(m3), cimagq(m3)),
                 std::complex<__float128>(crealq(m4), cimagq(m4)));
    return __complex128{res.real(), res.imag()};
}

inline __complex128 E0iCq(int                 id,
                          __complex128 const &p1,
                          __complex128 const &p2,
                          __complex128 const &p3,
                          __complex128 const &p4,
                          __complex128 const &p5,
                          __complex128 const &p6,
                          __complex128 const &p7,
                          __complex128 const &p8,
                          __complex128 const &p9,
                          __complex128 const &p10,
                          __complex128 const &m1,
                          __complex128 const &m2,
                          __complex128 const &m3,
                          __complex128 const &m4,
                          __complex128 const &m5)
{
    const std::complex<__float128> res
        = ::E0iC(id,
                 std::complex<__float128>(crealq(p1), cimagq(p1)),
                 std::complex<__float128>(crealq(p2), cimagq(p2)),
                 std::complex<__float128>(crealq(p3), cimagq(p3)),
                 std::complex<__float128>(crealq(p4), cimagq(p4)),
                 std::complex<__float128>(crealq(p5), cimagq(p5)),
                 std::complex<__float128>(crealq(p6), cimagq(p6)),
                 std::complex<__float128>(crealq(p7), cimagq(p7)),
                 std::complex<__float128>(crealq(p8), cimagq(p8)),
                 std::complex<__float128>(crealq(p9), cimagq(p9)),
                 std::complex<__float128>(crealq(p10), cimagq(p10)),
                 std::complex<__float128>(crealq(m1), cimagq(m1)),
                 std::complex<__float128>(crealq(m2), cimagq(m2)),
                 std::complex<__float128>(crealq(m3), cimagq(m3)),
                 std::complex<__float128>(crealq(m4), cimagq(m4)),
                 std::complex<__float128>(crealq(m5), cimagq(m5)));

    return __complex128{res.real(), res.imag()};
}

} // End of namespace mty

#endif
