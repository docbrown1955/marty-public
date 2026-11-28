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

#include "csl/error.h"
#include "marty/api/common/error.h"
#include "marty/api/common/session.h"
#include "marty/core/amplitude.h"
#include "marty/sgl/exceptions.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

void coreFailure()
{
    HEPAssert(false, mty::error::ValueError, "bad value 42");
}

// Runs f, translates whatever it throws, and returns the message of the
// typed error if it is an Expected. Other exceptions are reported as failures.
template <class Expected, class F>
std::string messageOf(F &&f)
{
    try {
        try {
            f();
        }
        catch (...) {
            mty::api::rethrowAsApiError();
        }
    }
    catch (Expected const &ex) {
        return ex.what();
    }
    catch (std::exception const &ex) {
        ADD_FAILURE() << "unexpected error type: " << ex.what();
        return {};
    }
    ADD_FAILURE() << "no exception was thrown";
    return {};
}

} // namespace

TEST(ErrorApi, HierarchyDerivesFromStdRuntimeError)
{
    static_assert(std::is_base_of_v<std::runtime_error, mty::api::Error>);
    static_assert(std::is_base_of_v<mty::api::Error, mty::api::ValueError>);
    static_assert(std::is_base_of_v<mty::api::Error, mty::api::TypeError>);
    static_assert(std::is_base_of_v<mty::api::Error, mty::api::LookupError>);
    static_assert(
        std::is_base_of_v<mty::api::Error, mty::api::NotImplementedError>);
    static_assert(std::is_base_of_v<mty::api::Error, mty::api::StateError>);
    static_assert(std::is_base_of_v<mty::api::Error, mty::api::BuildError>);
    EXPECT_THROW(throw mty::api::StateError("state"), std::runtime_error);
}

TEST(ErrorApi, CoreEnumErrorIsTranslatedWithMessage)
{
    mty::api::Session quiet;
    std::string const msg = messageOf<mty::api::ValueError>(coreFailure);
    EXPECT_NE(msg.find("bad value 42"), std::string::npos) << msg;
    EXPECT_NE(msg.find("coreFailure"), std::string::npos) << msg;
}

TEST(ErrorApi, CoreTypesAreMappedToTypedErrors)
{
    mty::api::Session quiet;
    messageOf<mty::api::LookupError>([] { throw mty::error::KeyError; });
    messageOf<mty::api::TypeError>([] { throw mty::error::TypeError; });
    messageOf<mty::api::NotImplementedError>(
        [] { throw mty::error::NotImplementedError; });
    messageOf<mty::api::BuildError>(
        [] { throw mty::error::ModelBuildingError; });
    messageOf<mty::api::TypeError>([] { throw CSLError::TypeError; });
    messageOf<mty::api::ValueError>([] { throw CSLError::ValueError; });
}

TEST(ErrorApi, GammaApiErrorIsTranslated)
{
    mty::api::Session quiet;
    std::string const msg = messageOf<mty::api::Error>(
        [] { throw GammaAPIError("gamma failure ", 3); });
    EXPECT_NE(msg.find("gamma failure 3"), std::string::npos) << msg;
}

TEST(ErrorApi, StdExceptionIsTranslated)
{
    std::string const msg = messageOf<mty::api::Error>(
        [] { throw std::out_of_range("index too large"); });
    EXPECT_EQ(msg, "index too large");
}

TEST(ErrorApi, SglExceptionsAreTyped)
{
    EXPECT_EQ(
        messageOf<mty::api::ValueError>([] { throw sgl::MathError("math "); }),
        "math ");
    EXPECT_EQ(
        messageOf<mty::api::TypeError>([] { throw sgl::TypeError("type "); }),
        "type ");
    EXPECT_EQ(messageOf<mty::api::LookupError>(
                  [] { throw sgl::IndexError("index "); }),
              "index ");
}

TEST(ErrorApi, LegacyThrowStillCatchableAsCoreException)
{
    mty::api::Session quiet;
    try {
        coreFailure();
        ADD_FAILURE() << "no exception was thrown";
    }
    catch (mty::error::Exception const &ex) {
        EXPECT_EQ(ex.type(), mty::error::ValueError);
        EXPECT_EQ(ex.message(), "bad value 42");
    }
}

TEST(ErrorApi, NothingIsPrintedToStderrInQuietMode)
{
    testing::internal::CaptureStderr();
    {
        mty::api::Session quiet;
        messageOf<mty::api::ValueError>(coreFailure);
        messageOf<mty::api::ValueError>(
            [] { CallErrorImpl(CSLError::ValueError, "f", "file", 1, "m"); });
    }
    EXPECT_EQ(testing::internal::GetCapturedStderr(), "");
}

// Acceptance criterion 3 of phase 0: an error path that existed before the API
// still prints the same text on stderr, and the failure is a catchable
// exception.
TEST(ErrorApi, ExistingErrorPathKeepsStderrTextAndIsCatchable)
{
    mty::FeynOptions options;
    mty::Amplitude   amplitude(options, mty::Kinematics{});

    testing::internal::CaptureStderr();
    bool caught = false;
    try {
        amplitude.expression(3);
    }
    catch (mty::error::Exception const &ex) {
        caught = true;
        EXPECT_EQ(ex.type(), mty::error::IndexError);
        EXPECT_EQ(ex.message(),
                  "Index 3 out of bounds of amplitude of size 0.");
    }
    std::string const err = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(caught);
    EXPECT_EQ(err.rfind("IndexError in function expression() (file ", 0), 0u)
        << err;
    EXPECT_NE(err.find("amplitude.cpp l."), std::string::npos) << err;
    EXPECT_EQ(err.substr(err.rfind("): ")),
              "): Index 3 out of bounds of amplitude of size 0.\n");
}
