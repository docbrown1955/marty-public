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

#include "mrtError.h"
#include "localTerms.h"
#include "feynmanIntegral.h"

// Source : Appendix A of hep-ph/0509141

namespace mty {

std::string indicesToString(std::vector<size_t> const &indices)
{
    std::string str(indices.size(), ' ');
    for (size_t i = 0; i != indices.size(); ++i) 
        str[i] = '0' + indices[i];

    return str;
}

csl::Expr getLocalTerm(
        IntegralType               type,
        std::vector<size_t> const &indices,
        std::vector<csl::Expr>   const &p,
        std::vector<csl::Expr>   const &m
        )
{
    switch(type) {
        case IntegralType::A: return getLocalATerm(
                                      indicesToString(indices),
                                      p, 
                                      m);
        case IntegralType::B: return getLocalBTerm(
                                      indicesToString(indices),
                                      p, 
                                      m);
        case IntegralType::C: return getLocalCTerm(
                                      indicesToString(indices),
                                      p, 
                                      m);
        case IntegralType::D: return getLocalDTerm(
                                      indicesToString(indices),
                                      p, 
                                      m);
        case IntegralType::E: return getLocalETerm(
                                      indicesToString(indices),
                                      p, 
                                      m);
        default: break;
    }

    return CSL_0;
}

csl::Expr getLocalATerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &,
        std::vector<csl::Expr> const &m
        )
{
    size_t n = indices.size()/2;
    if (n == 0)
        return -2 * m[0];
    if (n % 2 == 0) {
        return -csl::pow_s(m[0], n+1) 
            / (std::pow(2, n-1) * csl::factorial(n+1));
    }

    return CSL_0;
}

csl::Expr getLocalBTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momenta,
        std::vector<csl::Expr> const &masses
        )
{
    csl::Expr p = momenta[0];
    csl::Expr m0 = masses[0];
    csl::Expr m1 = masses[1];
    csl::Expr p2 = p;
    csl::Expr m0_2 = m0;
    csl::Expr m1_2 = m1;
    csl::Expr p4  = p2*p2;
    csl::Expr m0_4 = m0_2*m0_2;
    csl::Expr m1_4 = m1_2*m1_2;

    if (indices == "0")
        return -2;

    if (indices == "1")
        return 1;

    if (indices == "00")
        return CSL_1/6 * (p2 - 3*m0_2 - 3*m1_2);

    if (indices == "11")
        return -CSL_2/3;

    if (indices == "001")
        return -CSL_1/12 * (p2 - 2*m0_2 - 4*m1_2);

    if (indices == "111")
        return CSL_HALF;

    if (indices == "0000")
        return -CSL_1/120 * (
                p4 - 5*p2*(m0_2+m1_2) + 10*(m0_4 + m0_2*m1_2 + m1_4)
                );

    if (indices == "0011")
        CSL_1/60 * (3*p2 - 5*m0_2 - 15*m1_2);

    if (indices == "1111")
        return -CSL_2/5;

    if (indices == "00001")
        return CSL_1/240 * (
                p4 - 4*p2*m0_2 - 6*p2*m1_2 + 5*m0_4 + 10*m0_2*m1_2 + 15*m1_4
                );

    if (indices == "00111")
        return -CSL_1/60 * (2*p2 - 3*m0_2 - 12*m1_2);

    if (indices == "11111")
        return CSL_THIRD;

    return CSL_0;
}

csl::Expr getLocalCTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momenta,
        std::vector<csl::Expr> const &masses
        )
{
    csl::Expr p1 = momenta[0];
    csl::Expr p2 = momenta[1];
    csl::Expr m0 = masses[0];
    csl::Expr m1 = masses[1];
    csl::Expr m2 = masses[2];
    csl::Expr p1_2 = p1;
    csl::Expr p2_2 = p2;
    csl::Expr s12 = momenta[2];
    csl::Expr m0_2 = m0;
    csl::Expr m1_2 = m1;
    csl::Expr m2_2 = m2;
    csl::Expr p1_4  = p1_2*p1_2;
    csl::Expr p2_4  = p2_2*p2_2;
    csl::Expr m0_4 = m0_2*m0_2;
    csl::Expr m1_4 = m1_2*m1_2;
    csl::Expr m2_4 = m2_2*m2_2;

    if (indices == "001" or indices == "002")
        return CSL_1/6;

    if (indices == "0000")
        return CSL_1/48 * (s12 + p1_2 + p2_2)
            - CSL_1/12 * (m0_2 + m1_2 + m2_2);

    if (indices == "0011" or indices == "0022")
        return -CSL_1/12;

    if (indices == "0012")
        return -CSL_1/24;

    if (indices == "00001")
        return -CSL_1/240 * (2*s12 - 5*m0_2 + 2*p1_2-10*m1_2 + p2_2 - 5*m2_2);

    if (indices == "00002")
        return -CSL_1/240 * (2*s12 - 5*m0_2 + p1_2-5*m1_2 + 2*p2_2 - 10*m2_2);

    if (indices == "00111" or indices == "00222")
        return CSL_1/20;

    if (indices == "00112" or indices == "00122")
        return CSL_1/60;

    if (indices == "000000")
        return -CSL_1/2880 * (
                2*s12*s12 - 6*s12*m0_2 + 30*m0_4
                + 2*s12*(p1_2-6*m1_2 + p2_2-6*m2_2)
                - 6*m0_2*(2*p1_2-5*m1_2 + 2*p2_2-5*m2_2)
                + (2*p1_4 - 12*p1_2*m1_2 + 30*m1_4)
                + (2*p2_4 - 12*p2_2*m2_2 + 30*m2_4)
                + (2*p1_2*p2_2 - 6*p1_2*m2_2 - 6*p2_2*m1_2 + 30*m1_2*m2_2)
                );

    if (indices == "000011")
        return CSL_1/720 * (3*s12 - 6*m0_2 + 3*p1_2-18*m1_2 + p2_2 - 6*m2_2);

    if (indices == "000022")
        return CSL_1/720 * (3*s12 - 6*m0_2 + p1_2-6*m1_2 + 3*p2_2 - 18*m2_2);

    if (indices == "000012")
        return CSL_1/720 * (2*s12 - 3*m0_2 + p1_2-6*m1_2 + p2_2 - 6*m2_2);

    if (indices == "001111" or indices == "002222")
        return -CSL_1/30;

    if (indices == "001112" or indices == "001222")
        return -CSL_1/120;

    if (indices == "001122")
        return -CSL_1/180;

    if (indices == "0000001")
        return CSL_1/10080 * (
                3*s12*s12 - 7*s12*m0_2 + 21*m0_4
                + s12*(3*p1_2-21*m1_2 + p2_2-7*m2_2)
                - 7*m0_2*(2*p1_2-6*m1_2 + p2_2-3*m2_2)
                + (3*p1_4 - 21*p1_2*m1_2 + 63*m1_4)
                + (p1_4 - 7*p2_2*m2_2 + 21*m2_4)
                + (2*p1_2*p2_2 - 7*p1_2*m2_2 - 7*p2_2*m1_2 + 42*m1_2*m2_2)
                );

    if (indices == "0000002")
        return CSL_1/10080 * (
                3*s12*s12 - 7*s12*m0_2 + 21*m0_4
                + s12*(3*p2_2-21*m2_2 + p1_2-7*m1_2)
                - 7*m0_2*(2*p2_2-6*m2_2 + p1_2-3*m1_2)
                + (3*p2_4 - 21*p2_2*m1_2 + 63*m2_4)
                + (p1_4 - 7*p1_2*m2_2 + 21*m1_4)
                + (2*p1_2*p2_2 - 7*p1_2*m2_2 - 7*p2_2*m1_2 + 42*m1_2*m2_2)
                );

    if (indices == "0000111")
        return -CSL_1/1680 * (4*s12 - 7*m0_2 + 4*p1_2-28*m1_2 + p2_2 - 7*m2_2);

    if (indices == "0000222")
        return -CSL_1/1680 * (4*s12 - 7*m0_2 + 4*p2_2-28*m2_2 + p1_2 - 7*m1_2);

    if (indices == "0000112" or indices == "0000122")
        return -CSL_1/5040 * (6*s12 - 7*m0_2 + 3*p1_2-21*m1_2 + 2*p2_2 - 14*m2_2);

    if (indices == "0011111" or indices == "0011111")
        return CSL_1/42;

    if (indices == "0011112" or indices == "0012222")
        return CSL_1/210;

    if (indices == "0011122" or indices == "0011222")
        return CSL_1/420;


    return CSL_0;
}

csl::Expr getLocalDTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momenta,
        std::vector<csl::Expr> const &masses
        )
{
    csl::Expr p1 = momenta[0];
    csl::Expr p2 = momenta[1];
    csl::Expr p3 = momenta[2];
    csl::Expr m0 = masses[0];
    csl::Expr m1 = masses[1];
    csl::Expr m2 = masses[2];
    csl::Expr m3 = masses[3];
    csl::Expr p1_2 = psquared(p1);
    csl::Expr p2_2 = psquared(p2);
    csl::Expr p3_2 = psquared(p3);
    csl::Expr s12 = psquared(psum(p1, -p2));
    csl::Expr s13 = psquared(psum(p1, -p3));
    csl::Expr s23 = psquared(psum(p2, -p3));
    csl::Expr m0_2 = m0*m0;
    csl::Expr m1_2 = m1*m1;
    csl::Expr m2_2 = m2*m2;
    csl::Expr m3_2 = m3*m3;

    if (indices == "0000")
        return -CSL_1/12;

    if (indices == "00001" or indices == "00002" or indices == "00003")
        return CSL_1/48;

    if (indices == "000000")
        return CSL_1/480 * (s12 + s13 + s23 + p1_2 + p2_2 + p3_2)
            - CSL_1/96 * (m0_2 + m1_2 + m2_2 + m3_2);

    if (indices == "000011" or indices == "000022" or indices == "000022")
        return -CSL_1/120;

    if (indices == "000012" or indices == "000013" or indices == "000023")
        return -CSL_1/240;

    if (indices == "0000001")
        return -CSL_1/2880 * (
                2*p1_2 + p2_2 + p3_2
                + 2*s12 + 2*s13 + s23
                )
                + CSL_1/480 * (m0_2 + m1_2 + m2_2 + m3_2);

    if (indices == "0000002")
        return -CSL_1/2880 * (
                p1_2 + 2*p2_2 + p3_2
                + 2*s12 + s13 + 2*s23
                )
                + CSL_1/480 * (m0_2 + m1_2 + m2_2 + m3_2);

    if (indices == "0000003")
        return -CSL_1/2880 * (
                p1_2 + p2_2 + 2*p3_2
                + s12 + 2*s13 + 2*s23
                )
                + CSL_1/480 * (m0_2 + m1_2 + m2_2 + m3_2);

    if (indices == "0000111" or indices == "0000222" or indices == "0000333")
        return CSL_1/240;

    if (indices == "0000112" or indices == "0000113" or indices == "0000223"
            or indices == "0000122" or indices == "0000133" or indices == "0000233")
        return CSL_1/720;

    if (indices == "0000123")
        return CSL_1/1440;

    return CSL_0;
}

csl::Expr getLocalETerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &,
        std::vector<csl::Expr> const &
        )
{
    if (indices == "000000")
        return -CSL_1/96;

    return CSL_0;
}


} // End of namespace mty
