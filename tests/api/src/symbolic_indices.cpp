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
 * @file symbolic_indices.cpp
 * @brief Spaces and indices of the CSL API facade.
 */

#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/tensor.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using csl::api::ExprHandle;
using csl::api::IndexHandle;
using csl::api::IndexType;
using csl::api::SpaceHandle;
using csl::api::TensorHandle;
using csl::api::TypeError;
using csl::api::ValueError;

TEST(SymbolicIndices, IndexOutlivesScope)
{
    IndexHandle index = [] {
        SpaceHandle space("Tmp", 3, {"i", "j", "k"});
        return space.index("a");
    }();
    EXPECT_EQ(index.name(), "a");
    EXPECT_TRUE(index.equals(index));
}

TEST(SymbolicIndices, IndicesFromListAreDistinct)
{
    auto space = SpaceHandle::euclid3();
    auto list  = space.indices(3, "a");
    ASSERT_EQ(list.size(), 3U);
    EXPECT_FALSE(list[0].equals(list[1]));
    EXPECT_FALSE(list[1].equals(list[2]));
    EXPECT_FALSE(list[0].equals(list[2]));
    EXPECT_NE(list[0].hashKey(), list[1].hashKey());
}

TEST(SymbolicIndices, UnnamedIndexCyclesDefaultNames)
{
    SpaceHandle space("Cycle", 3, {"i", "j"});
    auto        first  = space.index();
    auto        second = space.index();
    auto        third  = space.index();
    EXPECT_EQ(first.name(), "i");
    EXPECT_EQ(second.name(), "j");
    EXPECT_EQ(third.name(), "i");
    EXPECT_FALSE(first.equals(third));
    EXPECT_NE(first.hashKey(), third.hashKey());
}

TEST(SymbolicIndices, UnnamedIndicesAreDistinct)
{
    SpaceHandle space("Pool", 3, {"i", "j", "k"});
    auto        list = space.indices(5);
    ASSERT_EQ(list.size(), 5U);
    EXPECT_EQ(list[0].name(), "i");
    EXPECT_EQ(list[1].name(), "j");
    EXPECT_EQ(list[2].name(), "k");
    EXPECT_EQ(list[3].name(), "i");
    EXPECT_EQ(list[4].name(), "j");
    for (std::size_t a = 0; a != list.size(); ++a)
        for (std::size_t b = a + 1; b != list.size(); ++b)
            EXPECT_FALSE(list[a].equals(list[b]));
}

TEST(SymbolicIndices, MinkowskiIsOneSharedSpace)
{
    EXPECT_EQ(&SpaceHandle::minkowski().raw(), &csl::Minkowski);
    EXPECT_EQ(SpaceHandle::minkowski().owner().get(), &csl::Minkowski);
    auto mu = SpaceHandle::minkowski().index("mu");
    EXPECT_EQ(mu.raw().getSpace(), &csl::Minkowski);
}

TEST(SymbolicIndices, FixedIndexRange)
{
    auto space = SpaceHandle::euclid3();
    auto two   = space.fixedIndex(2);
    EXPECT_TRUE(two.isFixed());
    EXPECT_EQ(two.value(), 2);
    EXPECT_THROW(space.fixedIndex(3), ValueError);
    EXPECT_THROW(space.fixedIndex(-1), ValueError);

    SpaceHandle big("Big", 200, {"a"});
    EXPECT_THROW(big.fixedIndex(128), ValueError);
    EXPECT_NO_THROW(big.fixedIndex(127));
}

TEST(SymbolicIndices, NameAndValueAreTypeChecked)
{
    auto space = SpaceHandle::euclid3();
    EXPECT_THROW(static_cast<void>(space.fixedIndex(1).name()), TypeError);
    EXPECT_THROW(static_cast<void>(space.index("a").value()), TypeError);
}

TEST(SymbolicIndices, IndexHashIgnoresSign)
{
    auto mu = SpaceHandle::minkowski().index("mu");
    auto nu = mu.flipped();
    EXPECT_TRUE(mu.equals(nu));
    EXPECT_EQ(mu.hashKey(), nu.hashKey());
}

TEST(SymbolicIndices, IndexSpaceMismatchRaises)
{
    auto a = SpaceHandle::euclid3().index("a");
    auto b = SpaceHandle::euclid2().index("b");
    EXPECT_THROW(csl::api::delta(a, b), ValueError);
    EXPECT_THROW(csl::api::metric(a, b), ValueError);
}

TEST(NullSpaceIndices, MetricWithoutSpaceRaises)
{
    IndexHandle first(nullptr, csl::Index());
    IndexHandle second(nullptr, csl::Index());
    EXPECT_THROW(static_cast<void>(csl::api::metric(first, second)),
                 ValueError);
}

TEST(NullSpaceIndices, DeltaWithoutSpaceRaises)
{
    IndexHandle first(nullptr, csl::Index());
    IndexHandle second(nullptr, csl::Index());
    EXPECT_THROW(static_cast<void>(csl::api::delta(first, second)),
                 ValueError);
}

TEST(NullSpaceIndices, EpsilonWithoutSpaceRaises)
{
    std::vector<IndexHandle> indices;
    for (int i = 0; i != 3; ++i)
        indices.emplace_back(nullptr, csl::Index());
    EXPECT_THROW(static_cast<void>(csl::api::epsilon(indices)), ValueError);
}

TEST(NullSpaceIndices, IdAndTypeWithoutSpaceRaise)
{
    IndexHandle index(nullptr, csl::Index());
    EXPECT_THROW(static_cast<void>(index.id()), ValueError);
    EXPECT_THROW(static_cast<void>(index.type()), ValueError);
}

TEST(SymbolicIndices, IdAndTypeOfFreeAndFixedIndices)
{
    SpaceHandle space("Probe", 3, {"i", "j"});
    auto        first  = space.index("a");
    auto        second = space.index("a");
    EXPECT_EQ(first.type(), IndexType::Free);
    EXPECT_EQ(first.id(), first.raw().getID());
    EXPECT_NE(first.id(), second.id());
    EXPECT_EQ(space.fixedIndex(2).type(), IndexType::Fixed);
}

TEST(SymbolicIndices, DummyIndexType)
{
    auto         space = SpaceHandle::euclid3();
    auto         a     = space.index("a");
    TensorHandle U("U", {space});
    ExprHandle   closed  = U({a}) * U({a});
    auto         dummies = csl::api::dummyIndices(closed);
    ASSERT_EQ(dummies.size(), 1U);
    EXPECT_EQ(dummies[0].type(), IndexType::Dummy);
    EXPECT_TRUE(csl::api::freeIndices(closed).empty());
}

TEST(SymbolicIndices, RenamingPairsGiveOldAndNewIndices)
{
    SpaceHandle  space("Renamed", 3, {"alpha", "beta"});
    TensorHandle T("T", {space});
    auto         a    = space.index("beta");
    ExprHandle   expr = T({a});

    csl::api::IndexRenamingHandle renaming;
    ExprHandle relabelled = csl::api::relabelIndices(expr, true, renaming);

    auto pairs = renaming.pairs();
    ASSERT_EQ(pairs.size(), 1U);
    EXPECT_TRUE(pairs[0].first.equals(a));
    EXPECT_EQ(pairs[0].first.type(), IndexType::Free);
    EXPECT_EQ(pairs[0].second.name(), "alpha");
    EXPECT_TRUE(relabelled.equals(csl::api::relabelIndices(expr)));
}

TEST(SymbolicIndices, RenamingPairsExcludeDummyIndices)
{
    SpaceHandle  space("Dummies", 3, {"alpha", "beta"});
    TensorHandle T("T", {space, space});
    auto         i    = space.index("i");
    ExprHandle   expr = T({i, i});

    csl::api::IndexRenamingHandle renaming;
    ExprHandle relabelled = csl::api::relabelIndices(expr, true, renaming);
    EXPECT_TRUE(csl::api::freeIndices(relabelled).empty());
    EXPECT_TRUE(renaming.pairs().empty());
}

TEST(SymbolicIndices, RenamingRejectsDummyIndices)
{
    SpaceHandle  space("Rejected", 3, {"alpha", "beta"});
    auto         i = space.index("i");
    TensorHandle x("x", {space});
    auto         dummies = csl::api::dummyIndices(x({i}) * x({i}));
    ASSERT_EQ(dummies.size(), 1U);
    csl::api::IndexRenamingHandle renaming;
    EXPECT_THROW(renaming.add(dummies[0], space.index("j")),
                 csl::api::ValueError);
    EXPECT_THROW(renaming.add(space.index("k"), dummies[0]),
                 csl::api::ValueError);
}
