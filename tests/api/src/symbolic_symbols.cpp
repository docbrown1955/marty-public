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
 * @file symbolic_symbols.cpp
 * @brief Symbols, canonical forms, equality and global options of the CSL
 *        API facade.
 */

#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/options.h"

#include <gtest/gtest.h>

using csl::api::ExprHandle;

TEST(SymbolicApi, ConstantAcceptsAnyName)
{
    EXPECT_NO_THROW(ExprHandle::constant("c"));
    EXPECT_NO_THROW(ExprHandle::constant("weird name 1+2"));
    EXPECT_NO_THROW(ExprHandle::variable("v"));
    EXPECT_NO_THROW(ExprHandle::variable("weird name 1+2"));
    EXPECT_TRUE(ExprHandle::constant("c").isSymbol());
    EXPECT_TRUE(ExprHandle::variable("v").isSymbol());
    EXPECT_FALSE(ExprHandle::integer(1).isSymbol());
}

TEST(SymbolicApi, SameNameDifferentSymbol)
{
    auto a = ExprHandle::variable("x");
    auto b = ExprHandle::variable("x");
    EXPECT_TRUE(a.equals(a));
    EXPECT_FALSE(a.equals(b));
    EXPECT_NE(a.symbolIdentity(), b.symbolIdentity());
    EXPECT_EQ(a.symbolIdentity(), a.symbolIdentity());
    EXPECT_FALSE((a - b).equals(ExprHandle::integer(0)));
}

TEST(SymbolicApi, SumProductPowerCanonicalisation)
{
    auto x    = ExprHandle::variable("x");
    auto y    = ExprHandle::variable("y");
    auto zero = ExprHandle::integer(0);
    auto one  = ExprHandle::integer(1);
    auto two  = ExprHandle::integer(2);

    EXPECT_TRUE((x + x).equals(two * x));
    EXPECT_TRUE((x * x).equals(csl::api::power(x, two)));
    EXPECT_TRUE((x + y).equals(y + x));
    EXPECT_TRUE((x * y).equals(y * x));
    EXPECT_TRUE((x - x).equals(zero));
    EXPECT_TRUE((x * one).equals(x));
    EXPECT_TRUE((x + zero).equals(x));
}

TEST(SymbolicApi, EqualityDocumentedSemantics)
{
    auto x = ExprHandle::variable("x");
    EXPECT_TRUE(ExprHandle::integer(3).equals(ExprHandle::real(3.0L)));
    EXPECT_TRUE(x.equals(x));
    EXPECT_FALSE(x.equals(ExprHandle::integer(0)));
    EXPECT_FALSE(ExprHandle::imaginaryUnit().equals(ExprHandle::integer(1)));
}

TEST(SymbolicApi, OperationsLeaveGlobalOptionsUntouched)
{
#define MTY_SYMBOLIC_SAVE(ns, name) auto const saved_##name = ns::name;
    MTY_CSL_OPTION_FLAGS(MTY_SYMBOLIC_SAVE)
#undef MTY_SYMBOLIC_SAVE

    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    (void) (x + y);
    (void) (x * y);
    (void) (x / y);
    (void) csl::api::power(x, ExprHandle::integer(2));
    (void) csl::api::Sin(x);
    (void) (x.equals(y));
    (void) (ExprHandle::integer(1) / ExprHandle::integer(0));
    (void) ExprHandle::integer(3).structure();

#define MTY_SYMBOLIC_CHECK(ns, name) \
    EXPECT_EQ(ns::name, saved_##name) << #name;
    MTY_CSL_OPTION_FLAGS(MTY_SYMBOLIC_CHECK)
#undef MTY_SYMBOLIC_CHECK
}
