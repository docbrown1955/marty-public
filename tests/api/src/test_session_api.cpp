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

#include "csl/options.h"
#include "marty/api/common/session.h"
#include "marty/api/gamma.h"
#include "marty/core/model.h"
#include "marty/core/mrtError.h"
#include "marty/core/mrtOptions.h"
#include "marty/sgl/sgloptions.h"

#include <gtest/gtest.h>

#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string readHeader(std::string const &relative)
{
    std::ifstream      in(std::string(MARTY_INCLUDE_PATH) + "/" + relative);
    std::ostringstream sout;
    sout << in.rdbuf();
    return sout.str();
}

// Names of the declarations "inline bool name = ..." (or "inline bool &name").
std::set<std::string> declaredFlags(std::string const &text)
{
    static std::regex const re(R"(inline\s+bool\s*&?\s*(\w+)\s*=)");
    std::set<std::string>   names;
    for (std::sregex_iterator it(text.begin(), text.end(), re), end; it != end;
         ++it)
        names.insert((*it)[1].str());
    return names;
}

// Names of the entries "X(namespace, name)" of an X-macro list.
std::set<std::string> listedFlags(std::string const &text)
{
    static std::regex const re(R"(X\(\s*[\w:]+\s*,\s*(\w+)\s*\))");
    std::set<std::string>   names;
    for (std::sregex_iterator it(text.begin(), text.end(), re), end; it != end;
         ++it)
        names.insert((*it)[1].str());
    return names;
}

} // namespace

TEST(SessionApi, OptionListCoversAllFlagsInHeaders)
{
    std::string const mrtOptions = readHeader("marty/core/mrtOptions.h");
    std::string const mrtError   = readHeader("marty/core/mrtError.h");
    std::string const cslOptions = readHeader("csl/options.h");

    std::set<std::string> martyDeclared = declaredFlags(mrtOptions);
    std::set<std::string> errorDeclared = declaredFlags(mrtError);
    martyDeclared.insert(errorDeclared.begin(), errorDeclared.end());

    EXPECT_FALSE(martyDeclared.empty());
    EXPECT_EQ(martyDeclared, listedFlags(mrtOptions));
    EXPECT_EQ(declaredFlags(cslOptions), listedFlags(cslOptions));
}

TEST(SessionApi, RestoresEveryOptionFlag)
{
    std::vector<bool> before;
#define MTY_TEST_SAVE(ns, name) before.push_back(ns::name);
    MTY_MARTY_OPTION_FLAGS(MTY_TEST_SAVE)
    MTY_CSL_OPTION_FLAGS(MTY_TEST_SAVE)
#undef MTY_TEST_SAVE

    {
        mty::api::Session session;
#define MTY_TEST_FLIP(ns, name) ns::name = !ns::name;
        MTY_MARTY_OPTION_FLAGS(MTY_TEST_FLIP)
        MTY_CSL_OPTION_FLAGS(MTY_TEST_FLIP)
#undef MTY_TEST_FLIP
    }

    std::size_t i = 0;
#define MTY_TEST_CHECK(ns, name) \
    EXPECT_EQ(ns::name, before[i++]) << #ns "::" #name;
    MTY_MARTY_OPTION_FLAGS(MTY_TEST_CHECK)
    MTY_CSL_OPTION_FLAGS(MTY_TEST_CHECK)
#undef MTY_TEST_CHECK
}

TEST(SessionApi, RestoresGammaSymbolicFlag)
{
    bool const before = sgl::option::keepEvanescentOperators;
    {
        mty::api::Session session;
        mty::gamma_api::keepSymbolic4D(!before);
        EXPECT_EQ(sgl::option::keepEvanescentOperators, !before);
    }
    EXPECT_EQ(sgl::option::keepEvanescentOperators, before);
}

TEST(SessionApi, RestoresCurrentModelOnExit)
{
    mty::Model outer;
    mty::Model inner;
    {
        mty::api::Session session(&outer);
        EXPECT_EQ(mty::Model::current, &outer);
    }
    EXPECT_EQ(mty::Model::current, &inner);
}

TEST(SessionApi, RestoresCurrentModelOnException)
{
    mty::Model outer;
    mty::Model inner;
    try {
        mty::api::Session session(&outer);
        throw std::runtime_error("failure inside session");
    }
    catch (std::runtime_error const &) {
    }
    EXPECT_EQ(mty::Model::current, &inner);
}

TEST(SessionApi, TwoModelsSequentialAndInterleaved)
{
    mty::Model        first;
    mty::Model        second;
    mty::Model *const before = mty::Model::current;
    {
        mty::api::Session outerSession(&first);
        EXPECT_EQ(mty::Model::current, &first);
        {
            mty::api::Session innerSession(&second);
            EXPECT_EQ(mty::Model::current, &second);
        }
        EXPECT_EQ(mty::Model::current, &first);
    }
    EXPECT_EQ(mty::Model::current, before);

    {
        mty::api::Session sessionFirst(&first);
    }
    {
        mty::api::Session sessionSecond(&second);
        EXPECT_EQ(mty::Model::current, &second);
    }
    EXPECT_EQ(mty::Model::current, before);
}

TEST(SessionApi, ModelDestructorClearsCurrent)
{
    mty::Model outer;
    {
        mty::Model inner;
        EXPECT_EQ(mty::Model::current, &inner);
    }
    EXPECT_EQ(mty::Model::current, nullptr);
}
