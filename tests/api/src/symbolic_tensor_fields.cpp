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
 * @file symbolic_tensor_fields.cpp
 * @brief Tensor fields, tensorial derivatives and their application in the
 *        CSL API facade.
 */

#include "csl/api/error.h"
#include "csl/api/expr.h"
#include "csl/api/tensor.h"

#include <gtest/gtest.h>

#include <vector>

using csl::api::applyTensorDerivative;
using csl::api::ExprHandle;
using csl::api::freeIndices;
using csl::api::IndexHandle;
using csl::api::SpaceHandle;
using csl::api::TensorDerivativeHandle;
using csl::api::TensorFieldHandle;
using csl::api::TensorHandle;
using csl::api::TypeError;
using csl::api::ValueError;

TEST(SymbolicTensorFields, FieldElementKeepsIndexFree)
{
    auto              r3 = SpaceHandle::euclid3();
    TensorHandle      X("X", {r3});
    TensorFieldHandle F("F", r3, {r3});
    auto              mu       = r3.index("mu");
    ExprHandle        element  = F({mu}, X);
    auto              freeList = freeIndices(element);
    ASSERT_EQ(freeList.size(), 1U);
    EXPECT_EQ(freeList[0].name(), "mu");
}

TEST(SymbolicTensorFields, ScalarFieldElementHasNoFreeIndex)
{
    auto              r3 = SpaceHandle::euclid3();
    TensorHandle      X("X", {r3});
    TensorFieldHandle G("G", r3, {});
    EXPECT_TRUE(freeIndices(G({}, X)).empty());
}

TEST(SymbolicTensorFields, FieldRejectsWrongIndexCount)
{
    auto              r3 = SpaceHandle::euclid3();
    TensorHandle      X("X", {r3});
    TensorFieldHandle F("F", r3, {r3});
    EXPECT_THROW(F({}, X), ValueError);
    EXPECT_THROW(F({r3.index("a"), r3.index("b")}, X), ValueError);
}

TEST(SymbolicTensorFields, FieldRejectsForeignPoint)
{
    auto              r2 = SpaceHandle::euclid2();
    auto              r3 = SpaceHandle::euclid3();
    TensorHandle      Y("Y", {r2});
    TensorFieldHandle F("F", r3, {r3});
    EXPECT_THROW(F({r3.index("a")}, Y), ValueError);
}

TEST(SymbolicTensorFields, DerivativeOfTensorKeepsBothIndicesFree)
{
    auto                   r3 = SpaceHandle::euclid3();
    TensorHandle           X("X", {r3});
    TensorDerivativeHandle d("d", r3);
    auto                   mu       = r3.index("mu");
    auto                   nu       = r3.index("nu");
    ExprHandle             result   = applyTensorDerivative(d(mu, X), X({nu}));
    auto                   freeList = freeIndices(result);
    ASSERT_EQ(freeList.size(), 2U);
    EXPECT_EQ(freeList[0].name(), "mu");
    EXPECT_EQ(freeList[1].name(), "nu");
}

TEST(SymbolicTensorFields, DerivativeOfPointTensorIsDelta)
{
    auto                   r3 = SpaceHandle::euclid3();
    TensorHandle           X("X", {r3});
    TensorDerivativeHandle d("d", r3);
    auto                   mu = r3.index("mu");
    auto                   nu = r3.index("nu");

    EXPECT_TRUE(applyTensorDerivative(d(mu, X), X({nu}))
                    .equals(csl::api::delta(mu, nu)));
}

TEST(SymbolicTensorFields, DerivativeRejectsIndexOutsideSpace)
{
    auto                   r2 = SpaceHandle::euclid2();
    auto                   r3 = SpaceHandle::euclid3();
    TensorHandle           X("X", {r3});
    TensorDerivativeHandle d("d", r3);
    EXPECT_THROW(d(r2.index("a"), X), ValueError);
}

TEST(SymbolicTensorFields, DerivativeRejectsForeignPoint)
{
    auto                   r2 = SpaceHandle::euclid2();
    auto                   r3 = SpaceHandle::euclid3();
    TensorHandle           Y("Y", {r2});
    TensorDerivativeHandle d("d", r3);
    EXPECT_THROW(d(r3.index("a"), Y), ValueError);
}

TEST(SymbolicTensorFields, ApplyRequiresDerivativeElement)
{
    auto              r3 = SpaceHandle::euclid3();
    TensorHandle      X("X", {r3});
    TensorFieldHandle F("F", r3, {r3});
    auto              mu = r3.index("mu");
    auto              nu = r3.index("nu");
    EXPECT_THROW(applyTensorDerivative(F({mu}, X), X({nu})), TypeError);
}
