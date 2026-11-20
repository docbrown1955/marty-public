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

#include "marty/api/common/error.h"
#include "marty/api/group_theory/group_theory.h"

#include <gtest/gtest.h>

#include <map>
#include <vector>

namespace gt = mty::api::group_theory;
using Labels = std::vector<int>;

namespace {

std::map<Labels, int> labelsOf(gt::Decomposition const &decomp)
{
    std::map<Labels, int> out;
    for (auto const &[irrep, mult] : decomp.terms)
        out[irrep.labels()] += mult;
    return out;
}

double toDouble(gt::Rational const &r)
{
    return static_cast<double>(r.num) / static_cast<double>(r.den);
}

} // namespace

TEST(GroupTheoryApi, SU3Decompositions)
{
    auto su3      = gt::Group::su(3);
    auto fund     = su3->fundamental();
    auto antifund = fund.conjugate();
    auto adj      = su3->adjoint();

    EXPECT_EQ(labelsOf(su3->tensorProduct(fund, antifund)),
              (std::map<Labels, int>{{{0, 0}, 1}, {{1, 1}, 1}}));
    EXPECT_EQ(labelsOf(su3->tensorProduct(fund, fund)),
              (std::map<Labels, int>{{{2, 0}, 1}, {{0, 1}, 1}}));
    EXPECT_EQ(
        labelsOf(su3->tensorProduct(adj, adj)),
        (std::map<Labels, int>{
            {{0, 0}, 1}, {{1, 1}, 2}, {{3, 0}, 1}, {{0, 3}, 1}, {{2, 2}, 1}}));
    EXPECT_EQ(su3->tensorProduct(adj, adj).dim(), 64);
}

TEST(GroupTheoryApi, OtherGroupDecompositions)
{
    auto su5 = gt::Group::su(5);
    auto f5  = su5->fundamental();
    EXPECT_EQ(labelsOf(su5->tensorProduct(f5, f5.conjugate())),
              (std::map<Labels, int>{{{0, 0, 0, 0}, 1}, {{1, 0, 0, 1}, 1}}));

    auto so10   = gt::Group::so(10);
    auto spinor = so10->irrep({0, 0, 0, 0, 1});
    ASSERT_EQ(spinor.dim(), 16);
    auto prod = so10->tensorProduct(spinor, spinor);
    EXPECT_EQ(prod.dim(), 256);
    std::map<int, int> dims;
    for (auto const &[irrep, mult] : prod.terms)
        dims[irrep.dim()] += mult;
    EXPECT_EQ(dims, (std::map<int, int>{{10, 1}, {120, 1}, {126, 1}}));
}

TEST(GroupTheoryApi, CasimirAndDynkinIndexUseCoreNormalisation)
{
    auto su3  = gt::Group::su(3);
    auto fund = su3->fundamental();
    auto adj  = su3->adjoint();

    EXPECT_EQ(fund.casimir(), gt::Rational(4, 9));
    EXPECT_EQ(adj.casimir(), gt::Rational(1, 1));
    EXPECT_EQ(fund.dynkinIndex(), gt::Rational(1, 6));
    EXPECT_EQ(adj.dynkinIndex(), gt::Rational(1, 1));
}

TEST(GroupTheoryApi, DynkinIndexSumRule)
{
    // T(r1 x r2) = dim(r1) T(r2) + dim(r2) T(r1)
    auto   su3    = gt::Group::su(3);
    auto   fund   = su3->fundamental();
    auto   adj    = su3->adjoint();
    auto   decomp = su3->tensorProduct(fund, adj);
    double total  = 0.0;
    for (auto const &[irrep, mult] : decomp.terms)
        total += mult * toDouble(irrep.dynkinIndex());
    double expected = fund.dim() * toDouble(adj.dynkinIndex())
                      + adj.dim() * toDouble(fund.dynkinIndex());
    EXPECT_NEAR(total, expected, 1e-12);
}

TEST(GroupTheoryApi, GroupsAreCachedAndIrrepsCompareWithinAGroup)
{
    EXPECT_EQ(gt::Group::su(3), gt::Group::su(3));
    EXPECT_NE(gt::Group::su(3), gt::Group::su(5));

    auto fund = gt::Group::su(3)->fundamental();
    EXPECT_EQ(fund.conjugate().conjugate(), fund);
    EXPECT_TRUE(gt::Group::su(3)->adjoint().isSelfConjugate());
    EXPECT_FALSE(fund.isSelfConjugate());

    EXPECT_THROW(fund == gt::Group::su(5)->fundamental(), mty::api::TypeError);
}

TEST(GroupTheoryApi, InvalidGroupAndLabelsAreRejected)
{
    EXPECT_THROW(gt::Group::su(1), mty::api::ValueError);
    EXPECT_THROW(gt::Group::so(3), mty::api::ValueError);
    EXPECT_THROW(gt::Group::su(3)->irrep({1}), mty::api::ValueError);
    EXPECT_THROW(gt::Group::su(3)->irrep({1, -1}), mty::api::ValueError);
}

TEST(GroupTheoryApi, U1ChargesAddExactly)
{
    auto u1 = gt::Group::u1();
    auto q  = u1->u1Irrep(gt::Rational(1, 3));
    EXPECT_EQ(q.charge(), gt::Rational(1, 3));

    auto prod = u1->tensorProduct(q, q);
    ASSERT_EQ(prod.terms.size(), 1u);
    EXPECT_EQ(prod.terms[0].first.charge(), gt::Rational(2, 3));
    EXPECT_EQ(prod.terms[0].second, 1);

    EXPECT_THROW(q.casimir(), mty::api::NotImplementedError);
    EXPECT_THROW(q.dynkinIndex(), mty::api::NotImplementedError);
}

TEST(GroupTheoryApi, SymplecticAndExceptionalDecompositions)
{
    auto dimsOf = [](gt::Decomposition const &decomp) {
        std::map<int, int> dims;
        for (auto const &[irrep, mult] : decomp.terms)
            dims[irrep.dim()] += mult;
        return dims;
    };

    auto sp4 = gt::Group::sp(4);
    EXPECT_EQ(sp4->fundamental().dim(), 4);
    EXPECT_EQ(sp4->adjoint().dim(), 10);
    EXPECT_EQ(
        dimsOf(sp4->tensorProduct(sp4->fundamental(), sp4->fundamental())),
        (std::map<int, int>{{1, 1}, {5, 1}, {10, 1}}));

    auto g2 = gt::Group::g2();
    EXPECT_EQ(g2->fundamental().dim(), 7);
    EXPECT_EQ(g2->adjoint().dim(), 14);
    EXPECT_EQ(dimsOf(g2->tensorProduct(g2->fundamental(), g2->fundamental())),
              (std::map<int, int>{{1, 1}, {7, 1}, {14, 1}, {27, 1}}));

    auto f4 = gt::Group::f4();
    EXPECT_EQ(f4->fundamental().dim(), 26);
    EXPECT_EQ(f4->adjoint().dim(), 52);
    EXPECT_EQ(
        dimsOf(f4->tensorProduct(f4->fundamental(), f4->fundamental())),
        (std::map<int, int>{{1, 1}, {26, 1}, {52, 1}, {273, 1}, {324, 1}}));

    auto e7 = gt::Group::e7();
    EXPECT_EQ(e7->fundamental().dim(), 56);
    EXPECT_EQ(e7->adjoint().dim(), 133);
    EXPECT_EQ(dimsOf(e7->tensorProduct(e7->fundamental(), e7->fundamental())),
              (std::map<int, int>{{1, 1}, {133, 1}, {1463, 1}, {1539, 1}}));

    auto e8 = gt::Group::e8();
    EXPECT_EQ(e8->fundamental().dim(), 248);
    EXPECT_EQ(e8->adjoint().dim(), 248);
    EXPECT_EQ(dimsOf(e8->tensorProduct(e8->fundamental(), e8->fundamental())),
              (std::map<int, int>{
                  {1, 1}, {248, 1}, {3875, 1}, {27000, 1}, {30380, 1}}));
}

TEST(GroupTheoryApi, SymplecticDimensionIsValidated)
{
    EXPECT_THROW(gt::Group::sp(2), mty::api::ValueError);
    EXPECT_THROW(gt::Group::sp(5), mty::api::ValueError);
}

TEST(GroupTheoryApi, ConjugationFollowsDiagramAutomorphism)
{
    auto e6  = gt::Group::e6();
    auto c27 = e6->fundamental().conjugate();
    EXPECT_EQ(c27.dim(), 27);
    EXPECT_EQ(c27.labels(), (std::vector<int>{0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(e6->fundamental().isSelfConjugate());
    EXPECT_TRUE(e6->adjoint().isSelfConjugate());

    auto sp4 = gt::Group::sp(4);
    EXPECT_TRUE(sp4->fundamental().isSelfConjugate());

    auto so10   = gt::Group::so(10);
    auto spinor = so10->irrep({0, 0, 0, 0, 1});
    EXPECT_EQ(spinor.dim(), 16);
    EXPECT_EQ(spinor.conjugate().labels(), (std::vector<int>{0, 0, 0, 1, 0}));
    EXPECT_FALSE(spinor.isSelfConjugate());
    EXPECT_TRUE(so10->fundamental().isSelfConjugate());

    auto e7 = gt::Group::e7();
    EXPECT_TRUE(e7->fundamental().isSelfConjugate());
}
