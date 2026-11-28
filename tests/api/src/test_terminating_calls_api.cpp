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

// Former exit() sites (see docs/architecture/pymarty/spike-c-error-paths.md):
// each of them must now end in a catchable exception.

#include "csl/error.h"
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// LibraryGroup::printFunctionStack* are private and their invalid-enum branch
// is unreachable through the public interface. The CSL headers are included
// first, with access widened, to reach it. Only the layout-neutral keyword
// changes; nothing else in this file depends on it.
#define private public
#include "csl/librarygroup.h"
#undef private

#include "marty/api/common/error.h"
#include "marty/api/common/session.h"
#include "marty/core/semiSimpleAlgebra.h"

#include <gtest/gtest.h>

namespace {

using Vec    = std::vector<double>;
using Matrix = std::vector<Vec>;

constexpr char const *dimensionMessage = "Type: two std::vectors must have "
                                         "the same dimension in product.";

// Checks that f throws a mty::error::Exception of kind ValueError carrying the
// historical message.
template <class F>
void expectDimensionError(F &&f)
{
    try {
        f();
        ADD_FAILURE() << "no exception was thrown";
    }
    catch (mty::error::Exception const &ex) {
        EXPECT_EQ(ex.type(), mty::error::ValueError);
        EXPECT_EQ(ex.message(), dimensionMessage);
    }
}

} // namespace

TEST(TerminatingCallsApi, VectorSumWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] { return Vec{1, 2} + Vec{1}; });
}

TEST(TerminatingCallsApi, VectorDifferenceWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] { return Vec{1, 2} - Vec{1}; });
}

TEST(TerminatingCallsApi, DotProductWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] {
        Vec const a{1, 2};
        Vec const b{1};
        return a * b;
    });
}

TEST(TerminatingCallsApi, MatrixVectorWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] {
        Matrix const A{{1, 2}, {3, 4}};
        Vec const    x{1};
        return A * x;
    });
}

TEST(TerminatingCallsApi, VectorMatrixWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] {
        Matrix const A{{1, 2}, {3, 4}};
        Vec const    x{1};
        return x * A;
    });
}

TEST(TerminatingCallsApi, MatrixMatrixWithDimensionMismatchThrows)
{
    mty::api::Session quiet;
    expectDimensionError([] {
        Matrix const A{{1, 2}, {3, 4}};
        Matrix const B{{1, 2}};
        return A * B;
    });
}

TEST(TerminatingCallsApi, MatchingDimensionsStillWork)
{
    EXPECT_EQ((Vec{1, 2} + Vec{3, 4}), (Vec{4, 6}));
    EXPECT_EQ((Vec{1, 2} - Vec{3, 4}), (Vec{-2, -2}));
    Vec const a{1, 2};
    Vec const b{3, 4};
    EXPECT_DOUBLE_EQ(a * b, 11);
}

TEST(TerminatingCallsApi, DimensionErrorIsTranslatedToValueError)
{
    mty::api::Session quiet;
    EXPECT_THROW(
        {
            try {
                (void) (Vec{1, 2} + Vec{1});
            }
            catch (...) {
                mty::api::rethrowAsApiError();
            }
        },
        mty::api::ValueError);
}

TEST(TerminatingCallsApi, LibraryGroupInvalidFunctionStackTagThrows)
{
    mty::api::Session  quiet;
    csl::LibraryGroup  group("g", false);
    std::ostringstream out;
    auto const invalid = static_cast<csl::LibraryGroup::VariablesTag>(7);

    EXPECT_THROW(group.printFunctionStackDefinition(out, invalid), CSLError);
    EXPECT_THROW(group.printFunctionStackBody(out, 0, invalid), CSLError);
}

TEST(TerminatingCallsApi, LibraryGroupErrorIsTranslatedToValueError)
{
    mty::api::Session  quiet;
    csl::LibraryGroup  group("g", false);
    std::ostringstream out;
    auto const invalid = static_cast<csl::LibraryGroup::VariablesTag>(7);

    EXPECT_THROW(
        {
            try {
                group.printFunctionStackDefinition(out, invalid);
            }
            catch (...) {
                mty::api::rethrowAsApiError();
            }
        },
        mty::api::ValueError);
}

TEST(TerminatingCallsApi, LibraryGroupErrorPrintsHistoricalTextUnlessQuiet)
{
    csl::LibraryGroup  group("g", false);
    std::ostringstream out;
    auto const invalid = static_cast<csl::LibraryGroup::VariablesTag>(7);

    testing::internal::CaptureStderr();
    EXPECT_THROW(group.printFunctionStackBody(out, 0, invalid), CSLError);
    std::string const printed = testing::internal::GetCapturedStderr();
    EXPECT_NE(printed.find("LibraryGroup::printFunctionStackBody has been "
                           "called without a valid nFunction specification"),
              std::string::npos)
        << printed;
}
