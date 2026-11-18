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

/**
 * @file symbolic_tensors.cpp
 * @brief Tensors, metric, delta, epsilon, contractions and index queries of
 *        the CSL API facade.
 */

#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/tensor.h"

#include <gtest/gtest.h>

#include <vector>

using csl::api::ExprHandle;
using csl::api::IndexHandle;
using csl::api::SpaceHandle;
using csl::api::SymmetrySpec;
using csl::api::TensorHandle;
using csl::api::ValueError;

TEST(SymbolicTensors, RepeatedIndexContracts)
{
    auto         r3 = SpaceHandle::euclid3();
    auto         a  = r3.index("a");
    auto         b  = r3.index("b");
    TensorHandle V("V", {r3});
    TensorHandle W("W", {r3});
    ExprHandle   lhs = V({a}) * W({a});
    ExprHandle   rhs = W({b}) * V({b});
    EXPECT_TRUE(lhs.equals(rhs));
    EXPECT_FALSE(lhs.equals(V({a}) * W({b})));
}

TEST(SymbolicTensors, SignedSpaceSameSignContractionRaises)
{
    auto         mink = SpaceHandle::minkowski();
    auto         mu   = mink.index("mu");
    TensorHandle V("V", {mink});
    TensorHandle W("W", {mink});
    EXPECT_THROW(V({mu}) * W({mu}), ValueError);
    EXPECT_NO_THROW(V({mu}) * W({mu.flipped()}));
}

TEST(SymbolicTensors, MetricDeltaInvariant)
{
    auto mink = SpaceHandle::minkowski();
    auto mu   = mink.index("mu");
    auto nu   = mink.index("nu");
    EXPECT_NO_THROW(csl::api::metric(mu, nu));
    EXPECT_NO_THROW(csl::api::delta(mu, nu.flipped()));
}

TEST(SymbolicTensors, EpsilonAntisymmetry)
{
    auto       r3 = SpaceHandle::euclid3();
    auto       a  = r3.index("a");
    auto       b  = r3.index("b");
    auto       c  = r3.index("c");
    ExprHandle sum
        = csl::api::epsilon({a, b, c}) + csl::api::epsilon({b, a, c});
    EXPECT_TRUE(sum.equals(ExprHandle::integer(0)));
}

TEST(SymbolicTensors, EpsilonDimensionRange)
{
    auto r2 = SpaceHandle::euclid2();
    auto r4 = SpaceHandle::euclid4();
    EXPECT_NO_THROW(csl::api::epsilon(r2.indices(2, "a")));
    EXPECT_NO_THROW(csl::api::epsilon(r4.indices(4, "a")));
    EXPECT_THROW(csl::api::epsilon(r2.indices(3, "a")), ValueError);
    EXPECT_THROW(csl::api::epsilon(r4.indices(3, "a")), ValueError);

    SpaceHandle five("Five", 5, {"i"});
    EXPECT_THROW(csl::api::epsilon(five.indices(5, "a")), ValueError);
}

TEST(SymbolicTensors, SymmetricTensorPair)
{
    auto         r3 = SpaceHandle::euclid3();
    auto         a  = r3.index("a");
    auto         b  = r3.index("b");
    SymmetrySpec symmetric;
    symmetric.pairs.emplace_back(0, 1);
    TensorHandle S("S", {r3, r3}, symmetric);
    EXPECT_TRUE(S({a, b}).equals(S({b, a})));
}

TEST(SymbolicTensors, AntisymmetricTensorPair)
{
    auto         r3 = SpaceHandle::euclid3();
    auto         a  = r3.index("a");
    auto         b  = r3.index("b");
    SymmetrySpec antisymmetric;
    antisymmetric.pairs.emplace_back(0, 1);
    TensorHandle A("A", {r3, r3}, {}, antisymmetric);
    ExprHandle   minusAba = ExprHandle::integer(-1) * A({b, a});
    EXPECT_TRUE(A({a, b}).equals(minusAba));
}

TEST(SymbolicTensors, FullySymmetricTensor)
{
    auto         r3 = SpaceHandle::euclid3();
    auto         a  = r3.index("a");
    auto         b  = r3.index("b");
    auto         c  = r3.index("c");
    SymmetrySpec symmetric;
    symmetric.fully = true;
    TensorHandle S("S", {r3, r3, r3}, symmetric);
    EXPECT_TRUE(S({a, b, c}).equals(S({c, a, b})));
}

TEST(SymbolicTensors, ConflictingSymmetryRaises)
{
    auto         r3 = SpaceHandle::euclid3();
    SymmetrySpec full;
    full.fully = true;
    SymmetrySpec pair;
    pair.pairs.emplace_back(0, 1);
    SymmetrySpec samePairAnti;
    samePairAnti.pairs.emplace_back(1, 0);
    SymmetrySpec outOfRange;
    outOfRange.pairs.emplace_back(0, 2);
    SymmetrySpec degenerate;
    degenerate.pairs.emplace_back(1, 1);
    SymmetrySpec both;
    both.fully = true;
    both.pairs.emplace_back(0, 1);

    EXPECT_THROW(TensorHandle("T", {r3, r3}, full, pair), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, pair, full), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, pair, pair), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, pair, samePairAnti), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, outOfRange), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, degenerate), ValueError);
    EXPECT_THROW(TensorHandle("T", {r3, r3}, both), ValueError);
    EXPECT_THROW(TensorHandle("T", std::vector<SpaceHandle>{}), ValueError);
}

TEST(SymbolicTensors, FullySymmetricAndAntisymmetricRaises)
{
    auto         r3 = SpaceHandle::euclid3();
    SymmetrySpec full;
    full.fully = true;
    EXPECT_THROW(TensorHandle("T", {r3, r3, r3}, full, full), ValueError);
}

TEST(SymbolicTensors, ElementRejectsForeignIndex)
{
    auto         r3 = SpaceHandle::euclid3();
    auto         r2 = SpaceHandle::euclid2();
    TensorHandle T("T", {r3});
    EXPECT_THROW(T({r2.index("a")}), ValueError);
    EXPECT_THROW(T({r3.index("a"), r3.index("b")}), ValueError);
}

TEST(SymbolicTensors, RelabelIndicesCanonical)
{
    auto         r3 = SpaceHandle::euclid3();
    TensorHandle T("T", {r3, r3});
    TensorHandle U("U", {r3});
    auto         a      = r3.index("a");
    auto         b      = r3.index("b");
    auto         c      = r3.index("c");
    auto         d      = r3.index("d");
    ExprHandle   first  = T({a, b}) * U({b});
    ExprHandle   second = T({c, d}) * U({d});
    EXPECT_TRUE(csl::api::relabelIndices(first).equals(
        csl::api::relabelIndices(second)));
}

TEST(SymbolicTensors, FreeAndDummyIndexQueries)
{
    auto         r3 = SpaceHandle::euclid3();
    TensorHandle T("T", {r3, r3});
    TensorHandle U("U", {r3});
    auto         a = r3.index("a");
    auto         b = r3.index("b");

    ExprHandle mixed     = T({a, b}) * U({b});
    auto       freeList  = csl::api::freeIndices(mixed);
    auto       dummyList = csl::api::dummyIndices(mixed);
    ASSERT_EQ(freeList.size(), 1U);
    EXPECT_EQ(freeList[0].name(), "a");
    ASSERT_EQ(dummyList.size(), 1U);
    EXPECT_TRUE(dummyList[0].isDummy());

    ExprHandle closed = U({a}) * U({a});
    EXPECT_TRUE(csl::api::freeIndices(closed).empty());
    EXPECT_EQ(csl::api::dummyIndices(closed).size(), 1U);
}
