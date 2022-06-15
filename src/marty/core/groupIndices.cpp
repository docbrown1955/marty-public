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

#include "groupIndices.h"
#include "gaugedGroup.h"

namespace mty {

#define ASSERT_SIZE(container, sz)                                        \
    HEPAssert(container.size() == sz,                                     \
              mty::error::ValueError,                                     \
              "Expecting a container of size " + toString(sz) + ", size " \
                  + toString(container.size()) + " given.")

bool isIndexZeroSU(std::vector<int> const &rep, size_t p)
{
    if (p > rep.size() + 1)
        return true;
    return false;
}
bool isIndexZeroEvenSO(std::vector<int> const &rep, size_t p)
{
    // No invariant if p odd and different from l
    if (p % 2 == 1 and p != rep.size())
        return true;
    // No invariant if p > l
    if (p != rep.size() and p > 2 * rep.size() - 2)
        return true;
    return false;
}
bool isIndexZeroOddSO(std::vector<int> const &rep, size_t p)
{
    // No invariant if p odd
    if (p % 2 == 1)
        return true;
    // No invariant if p > 2l
    if (p > 2 * rep.size())
        return true;
    return false;
}
bool isIndexZeroSp(std::vector<int> const &rep, size_t p)
{
    // No invariant if p odd
    if (p % 2 == 1)
        return true;
    // No invariant if p > 2l
    if (p > 2 * rep.size())
        return true;
    return false;
}
bool isIndexZeroE6(std::vector<int> const &, size_t p)
{

    if (p == 2 or p == 5 or p == 6 or p == 8 or p == 9 or p == 12)
        return false;
    return true;
}
bool isIndexZeroE7(std::vector<int> const &, size_t p)
{
    if (p == 2 or p == 6 or p == 8 or p == 10 or p == 12)
        return false;
    return true;
}
bool isIndexZeroE8(std::vector<int> const &, size_t p)
{
    if (p == 2 or p == 8 or p == 12)
        return false;
    return true;
}
bool isIndexZeroF4(std::vector<int> const &, size_t p)
{
    if (p == 2 or p == 6 or p == 8 or p == 12)
        return false;
    return true;
}
bool isIndexZeroG2(std::vector<int> const &, size_t p)
{
    return p == 2 or p == 6;
}

csl::Expr
getIndexNorm(algebra::Type algebra, std::vector<int> const &rep, size_t n)
{
    if (n == 1)
        return CSL_0;
    if (n == 2) {
        auto I2 = getIndex(algebra, rep, n);
        switch (algebra) {
        case algebra::Type::A:
            return SUGauged::a / I2;
        case algebra::Type::B:
            return SOGauged::a / I2;
        case algebra::Type::D:
            return SOGauged::a / I2;
        case algebra::Type::C:
            return SpGauged::a / I2;
        case algebra::Type::E6:
            return E6Gauged::a / I2;
        case algebra::Type::E7:
            return E7Gauged::a / I2;
        case algebra::Type::E8:
            return E8Gauged::a / I2;
        case algebra::Type::F4:
            return F4Gauged::a / I2;
        case algebra::Type::G2:
            return G2Gauged::a / I2;
        default:
            CallHEPError(mty::error::TypeError,
                         "Algebra type " + toString(algebra)
                             + " not recognized"
                               " for index norm.");
            return CSL_0;
        }
    }
    switch (algebra) {
    case algebra::Type::A:
        return getSUIndexNorm(rep, n);
    case algebra::Type::B:
        return getOddSOIndexNorm(rep, n);
    case algebra::Type::D:
        return getEvenSOIndexNorm(rep, n);
    case algebra::Type::C:
        return getSpIndexNorm(rep, n);
    case algebra::Type::E6:
        return getE6IndexNorm(rep, n);
    case algebra::Type::E7:
        return getE7IndexNorm(rep, n);
    case algebra::Type::E8:
        return getE8IndexNorm(rep, n);
    case algebra::Type::F4:
        return getF4IndexNorm(rep, n);
    case algebra::Type::G2:
        return getG2IndexNorm(rep, n);
    default:
        CallHEPError(mty::error::TypeError,
                     "Algebra type " + toString(algebra)
                         + " not recognized"
                           " for index norm.");
        return CSL_0;
    }
}
csl::Expr getSUIndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroSU(rep, n))
        return CSL_0;
    return CSL_1;
}
csl::Expr getEvenSOIndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroEvenSO(rep, n))
        return CSL_0;
    if (rep.size() == 3)
        return CSL_1;
    if (rep.size() == 4) {
        if (n == 2 or n == 4)
            return CSL_2;
        if (n == 6)
            return CSL_1;
    }
    if (rep.size() == 5) {
        if (n == 2 or n == 8)
            return CSL_2;
        if (n == 4 or n == 6)
            return CSL_1;
    }
    if (n == 2)
        return CSL_2;
    if (n == 2 * rep.size() - 2)
        return CSL_0;
    return CSL_1;
}
csl::Expr getOddSOIndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroOddSO(rep, n))
        return CSL_0;
    if (rep.size() < 3)
        return CSL_1;
    if (rep.size() == 3) {
        if (n <= 4)
            return CSL_2;
        return CSL_1;
    }
    if (n == 2)
        return CSL_2;
    if (rep.size() == 4 and n == 2 * rep.size())
        return CSL_2;
    return CSL_1;
}
csl::Expr getSpIndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroSp(rep, n))
        return CSL_0;
    return CSL_1;
}
csl::Expr getE6IndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroE6(rep, n))
        return CSL_0;
    if (n == 2)
        return csl::int_s(6);
    return CSL_1;
}
csl::Expr getE7IndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroE7(rep, n))
        return CSL_0;
    if (n == 2)
        return csl::int_s(12);
    return CSL_1;
}
csl::Expr getE8IndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroE8(rep, n))
        return CSL_0;
    if (n == 2)
        return csl::int_s(60);
    return CSL_1;
}
csl::Expr getF4IndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroF4(rep, n))
        return CSL_0;
    if (n == 2)
        return csl::int_s(6);
    return CSL_1;
}
csl::Expr getG2IndexNorm(std::vector<int> const &rep, size_t n)
{
    if (isIndexZeroG2(rep, n))
        return CSL_0;
    if (n == 2)
        return CSL_2;
    return CSL_1;
}

csl::Expr
getIndex(algebra::Type algebra, std::vector<int> const &rep, size_t n)
{
    if (n == 1)
        return CSL_0;
    switch (algebra) {
    case algebra::Type::A:
        return getSUIndex(rep, n);
    case algebra::Type::B:
        return getOddSOIndex(rep, n);
    case algebra::Type::D:
        return getEvenSOIndex(rep, n);
    case algebra::Type::C:
        return getSpIndex(rep, n);
    case algebra::Type::E6:
        return getE6Index(rep, n);
    case algebra::Type::E7:
        return getE7Index(rep, n);
    case algebra::Type::E8:
        return getE8Index(rep, n);
    case algebra::Type::F4:
        return getF4Index(rep, n);
    case algebra::Type::G2:
        return getG2Index(rep, n);
    default:
        CallHEPError(mty::error::TypeError,
                     "Algebra type " + toString(algebra)
                         + " not recognized"
                           " for indices.");
        return CSL_0;
    }
}

std::vector<csl::Expr> getFi_A(std::vector<int> const &dinkin)
{
    std::vector<csl::Expr> f(dinkin.size() + 1);
    csl::Expr              sum_f = 0;
    for (size_t i = 0; i != dinkin.size(); ++i) {
        size_t index = dinkin.size() - i - 1;
        f[index]     = dinkin[index];
        if (i != 0)
            f[index] += f[index + 1];
        sum_f += f[index];
    }
    f[dinkin.size()] = CSL_0;
    for (auto &fi : f) {
        fi -= sum_f / f.size();
    }
    // std::cout << "F = " << std::endl;
    // for (auto fi : f)
    //     std::cout << fi << " ";
    // std::cout << std::endl;

    return f;
}

std::vector<csl::Expr> getLi_A(std::vector<csl::Expr> const &f)
{
    std::vector<csl::Expr> l(f.size());
    size_t                 n = f.size() - 1;
    for (size_t i = 0; i != f.size(); ++i) {
        l[i] = f[i] + n + 1 - (i + 1);
    }
    // std::cout << "L = " << std::endl;
    // for (auto fi : l)
    //     std::cout << fi << " ";
    // std::cout << std::endl;

    return l;
}

std::vector<csl::Expr> getLi_A(std::vector<int> const &dinkin)
{
    return getLi_A(getFi_A(dinkin));
}

csl::Expr getSUIndex(std::vector<int> const &rep, size_t p)
{
    // No invariant if p > l+1
    if (isIndexZeroSU(rep, p))
        return CSL_0;
    std::vector<csl::Expr> l = getLi_A(rep);
    std::vector<csl::Expr> ITerms;
    ITerms.reserve(p + 1);
    for (size_t j = 0; j < l.size(); ++j) {
        csl::Expr prod = csl::pow_s(l[j], p);
        for (size_t k = 0; k < l.size(); ++k) {
            if (j != k) {
                prod *= l[k] + 1 - l[j];
                prod /= l[k] - l[j];
            }
        }
        ITerms.push_back(prod);
    }

    csl::Expr res = csl::Evaluated(
        csl::Expanded(csl::pow_s(-1, p) * csl::sum_s(ITerms)));
    // std::cout << "RES = " << res << std::endl;
    return res;
}

std::vector<csl::Expr> getFi_B(std::vector<int> const &dinkin)
{
    std::vector<csl::Expr> f(dinkin.size());
    for (size_t i = 0; i != f.size(); ++i) {
        f[i] = CSL_0;
        for (size_t j = i; j != f.size(); ++j) {
            if (j == f.size() - 1)
                f[i] += csl::intfraction_s(dinkin[j], 2);
            else
                f[i] += dinkin[j];
        }
    }
    std::cout << "F = ";
    for (auto const &fi : f)
        std::cout << fi << " ";
    std::cout << std::endl;

    return f;
}

std::vector<csl::Expr> getLi_B(std::vector<csl::Expr> const &f)
{
    std::vector<csl::Expr> l(f.size());
    size_t                 n = f.size();
    std::cout << "L = ";
    for (size_t i = 0; i != f.size(); ++i) {
        l[i] = f[i] + n + CSL_HALF - (i + 1);
        std::cout << l[i] << " ";
    }
    std::cout << std::endl;

    return l;
}

std::vector<csl::Expr> getLi_B(std::vector<int> const &dinkin)
{
    return getLi_B(getFi_B(dinkin));
}

csl::Expr getOddSOIndex(std::vector<int> const &rep, size_t p)
{
    if (isIndexZeroOddSO(rep, p))
        return CSL_0;
    std::vector<csl::Expr> l = getLi_B(rep);
    int                    n = static_cast<int>(rep.size());

    std::vector<csl::Expr> ITerms;
    ITerms.reserve(1 + 2 * rep.size());
    ITerms.push_back(csl::pow_s(-n, p));
    for (size_t j = 0; j != l.size(); ++j) {
        csl::Expr prod1 = CSL_1;
        csl::Expr prod2 = CSL_1;
        for (size_t k = 0; k != l.size(); ++k) {
            if (j != k) {
                prod1 *= (l[j] + 1) * (l[j] + 1) - l[k] * l[k];
                prod1 /= l[j] * l[j] - l[k] * l[k];
                prod2 *= (l[j] - 1) * (l[j] - 1) - l[k] * l[k];
                prod2 /= l[j] * l[j] - l[k] * l[k];
            }
        }
        ITerms.push_back(csl::pow_s(l[j] + CSL_HALF - n, p) * (l[j] + 1) / l[j]
                         * prod1);
        // std::cout << ITerms.back() << " ";
        ITerms.push_back(csl::pow_s(-l[j] + CSL_HALF - n, p) * (l[j] - 1)
                         / l[j] * prod2);
        // std::cout << ITerms.back() << " \n";
    }

    return csl::Evaluated(
        csl::Expanded(csl::sum_s(ITerms) / csl::pow_s(2, p)));
}

std::vector<csl::Expr> getFi_D(std::vector<int> const &dinkin)
{
    std::vector<csl::Expr> f(dinkin.size());
    for (size_t i = 0; i != f.size() - 2; ++i) {
        f[i] = CSL_0;
        for (size_t j = i; j != f.size(); ++j) {
            if (j >= f.size() - 2)
                f[i] += CSL_HALF * dinkin[j];
            else
                f[i] += dinkin[j];
        }
    }
    size_t n = dinkin.size();
    f[n - 1] = CSL_HALF * (dinkin[n - 1] - dinkin[n - 2]);
    f[n - 2] = CSL_HALF * (dinkin[n - 1] + dinkin[n - 2]);

    return f;
}

std::vector<csl::Expr> getLi_D(std::vector<csl::Expr> const &f)
{
    std::vector<csl::Expr> l(f.size());
    size_t                 n = f.size();
    for (size_t i = 0; i != f.size(); ++i) {
        l[i] = f[i] + n - (i + 1);
    }

    return l;
}

std::vector<csl::Expr> getLi_D(std::vector<int> const &dinkin)
{
    return getLi_D(getFi_D(dinkin));
}

csl::Expr getEvenSOIndex(std::vector<int> const &rep, size_t p)
{
    if (isIndexZeroEvenSO(rep, p))
        return CSL_0;
    std::vector<csl::Expr> l = getLi_D(rep);
    size_t                 n = rep.size();

    std::vector<csl::Expr> ITerms;
    ITerms.reserve(2 * rep.size());
    for (size_t j = 0; j != l.size(); ++j) {
        csl::Expr prod1 = CSL_1;
        csl::Expr prod2 = CSL_1;
        for (size_t k = 0; k != l.size(); ++k) {
            if (j != k) {
                prod1 *= (l[j] + 1) * (l[j] + 1) - l[k] * l[k];
                prod1 /= l[j] * l[j] - l[k] * l[k];
                prod2 *= (l[j] - 1) * (l[j] - 1) - l[k] * l[k];
                prod2 /= l[j] * l[j] - l[k] * l[k];
            }
        }
        ITerms.push_back(csl::pow_s(l[j] + 1 - n, p) * prod1);
        ITerms.push_back(csl::pow_s(-l[j] + 1 - n, p) * prod2);
    }

    return csl::Evaluated(
        csl::Expanded(csl::sum_s(ITerms) / csl::pow_s(2, p)));
}
csl::Expr
getLastEvenSOIndex(std::vector<int> const &rep, size_t n, bool normalize)
{
    if (rep.size() < 3)
        return CSL_0;
    if (n != rep.size())
        return CSL_0;

    std::vector<int> leftRep(rep.size(), 0);
    leftRep[leftRep.size() - 2] = 1;
    std::vector<int> rightRep(rep.size(), 0);
    rightRep[rightRep.size() - 1] = 1;
    if (normalize) {
        if (rep == leftRep or rep == rightRep)
            return CSL_1;
    }
    std::vector<csl::Expr> l     = getLi_D(rep);
    csl::Expr              index = CSL_1;
    for (const auto &label : l)
        index *= label;
    if (normalize)
        return index / getLastEvenSOIndex(leftRep, n, false);
    return index;
}

std::vector<csl::Expr> getFi_C(std::vector<int> const &dinkin)
{
    std::vector<csl::Expr> f(dinkin.size());
    for (size_t i = 0; i != f.size(); ++i) {
        f[i] = CSL_0;
        for (size_t j = i; j != f.size(); ++j)
            f[i] += dinkin[j];
    }

    return f;
}

std::vector<csl::Expr> getLi_C(std::vector<csl::Expr> const &f)
{
    std::vector<csl::Expr> l(f.size());
    size_t                 n = f.size();
    for (size_t i = 0; i != f.size(); ++i) {
        l[i] = f[i] + n + 1 - (i + 1);
    }

    return l;
}

std::vector<csl::Expr> getLi_C(std::vector<int> const &dinkin)
{
    return getLi_C(getFi_C(dinkin));
}

csl::Expr getSpIndex(std::vector<int> const &rep, size_t p)
{
    if (isIndexZeroSp(rep, p))
        return CSL_0;
    std::vector<csl::Expr> ITerms;
    ITerms.reserve(2 * rep.size());
    std::vector<csl::Expr> l = getLi_C(rep);
    size_t                 n = rep.size();
    for (size_t j = 0; j != l.size(); ++j) {
        csl::Expr prod1 = CSL_1;
        csl::Expr prod2 = CSL_1;
        for (size_t k = 0; k != l.size(); ++k) {
            if (j != k) {
                prod1 *= (l[j] + 1) * (l[j] + 1) - l[k] * l[k];
                prod1 /= l[j] * l[j] - l[k] * l[k];
                prod2 *= (l[j] - 1) * (l[j] - 1) - l[k] * l[k];
                prod2 /= l[j] * l[j] - l[k] * l[k];
            }
        }
        ITerms.push_back(csl::pow_s(l[j] - n, p) * (l[j] + 1) / l[j] * prod1);
        ITerms.push_back(csl::pow_s(-l[j] - n, p) * (l[j] - 1) / l[j] * prod2);
    }

    return csl::Evaluated(
        csl::Expanded(csl::sum_s(ITerms) / csl::pow_s(2, p)));
}

template <size_t N, size_t nReps>
csl::Expr getIntegerIndex(std::array<int, N> const &            t_rep,
                          std::array<GIndex_t<N>, nReps> const &data)
{
    for (const auto &[rep, index] : data)
        if (rep == t_rep)
            return csl::int_s(index);
    return CSL_0;
}

template <size_t N, size_t nReps>
csl::Expr getRationalIndex(std::array<int, N> const &            t_rep,
                           std::array<GIndex_t<N>, nReps> const &data,
                           int                                   denom)
{
    for (const auto [rep, index] : data)
        if (rep == t_rep)
            return csl::intfraction_s(index, denom);
    return CSL_0;
}

csl::Expr getE6Index(std::vector<int> const &rep, size_t n)
{
    ASSERT_SIZE(rep, 6)
    std::array<int, 6> arrep = toArray<int, 6>(rep);

    if (n == 2)
        return getRationalIndex(arrep, I2_E6_data, 6);
    if (n == 5)
        return getIntegerIndex(arrep, I5_E6_data);
    if (n == 6)
        return getIntegerIndex(arrep, I6_E6_data);
    if (n == 8)
        return getIntegerIndex(arrep, I8_E6_data);
    if (n == 9)
        return getIntegerIndex(arrep, I9_E6_data);
    if (n == 12)
        return getIntegerIndex(arrep, I12_E6_data);
    return CSL_0;
}

csl::Expr getE7Index(std::vector<int> const &rep, size_t n)
{
    ASSERT_SIZE(rep, 7)
    std::array<int, 7> arrep = toArray<int, 7>(rep);

    if (n == 2)
        return getRationalIndex(arrep, I2_E7_data, 12);
    if (n == 6)
        return getIntegerIndex(arrep, I6_E7_data);
    if (n == 8)
        return getIntegerIndex(arrep, I8_E7_data);
    if (n == 10)
        return getIntegerIndex(arrep, I10_E7_data);
    if (n == 12)
        return getIntegerIndex(arrep, I12_E7_data);
    return CSL_0;
}

csl::Expr getE8Index(std::vector<int> const &rep, size_t n)
{
    ASSERT_SIZE(rep, 8)
    std::array<int, 8> arrep = toArray<int, 8>(rep);

    if (n == 2)
        return getRationalIndex(arrep, I2_E8_data, 30);
    if (n == 8)
        return getIntegerIndex(arrep, I8_E8_data);
    if (n == 12)
        return getIntegerIndex(arrep, I12_E8_data);
    return CSL_0;
}

csl::Expr getF4Index(std::vector<int> const &rep, size_t n)
{
    ASSERT_SIZE(rep, 4)
    std::array<int, 4> arrep = toArray<int, 4>(rep);
    if (n == 2)
        return getRationalIndex(arrep, I2_F4_data, 6);
    if (n == 6)
        return getIntegerIndex(arrep, I6_F4_data);
    if (n == 8)
        return getIntegerIndex(arrep, I8_F4_data);
    if (n == 12)
        return getIntegerIndex(arrep, I12_F4_data);

    return CSL_0;
}

csl::Expr getG2Index(std::vector<int> const &rep, size_t n)
{
    ASSERT_SIZE(rep, 2)
    std::array<int, 2> arrep = toArray<int, 2>(rep);

    if (n == 2)
        return getRationalIndex(arrep, I2_G2_data, 2);
    if (n == 6)
        return getIntegerIndex(arrep, I6_G2_data);

    return CSL_0;
}

} // End of namespace mty
