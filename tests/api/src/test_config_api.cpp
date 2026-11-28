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

#include "marty/api/common/config.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

// Sets (or unsets, with nullptr) an environment variable and restores the
// previous value on destruction.
class EnvVar {
  public:
    EnvVar(std::string name, char const *value) : m_name(std::move(name))
    {
        if (char const *old = std::getenv(m_name.c_str()))
            m_old = old;
        set(value);
    }

    ~EnvVar()
    {
        set(m_old ? m_old->c_str() : nullptr);
    }

    EnvVar(EnvVar const &)            = delete;
    EnvVar &operator=(EnvVar const &) = delete;

  private:
    void set(char const *value) const
    {
        if (value)
            setenv(m_name.c_str(), value, 1);
        else
            unsetenv(m_name.c_str());
    }

    std::string                m_name;
    std::optional<std::string> m_old;
};

class TempDir {
  public:
    TempDir()
        : m_path(fs::temp_directory_path()
                 / ("marty_config_test_" + std::to_string(::getpid()) + "_"
                    + std::to_string(s_counter++)))
    {
        fs::create_directories(m_path);
    }

    ~TempDir()
    {
        std::error_code ec;
        fs::remove_all(m_path, ec);
    }

    TempDir(TempDir const &)            = delete;
    TempDir &operator=(TempDir const &) = delete;

    fs::path const &path() const
    {
        return m_path;
    }

  private:
    inline static int s_counter = 0;
    fs::path          m_path;
};

fs::path touch(fs::path const &file, bool executable = false)
{
    fs::create_directories(file.parent_path());
    std::ofstream(file) << "#!/bin/sh\n";
    if (executable)
        fs::permissions(file, fs::perms::owner_all);
    return file;
}

// Package data with include/marty.h and lib/libmarty.so below `root`.
void makePackageData(fs::path const &root)
{
    touch(root / "include" / "marty.h");
    touch(root / "lib" / "libmarty.so");
}

} // namespace

TEST(ConfigApi, EnvironmentOverridesPackageData)
{
    TempDir package;
    TempDir envInclude;
    TempDir envLib;
    makePackageData(package.path());

    mty::api::ConfigArguments args;
    args.packageData = package.path().string();

    {
        EnvVar     noInclude("MARTY_INCLUDE_PATH", nullptr);
        EnvVar     noLib("MARTY_LIBRARY_PATH", nullptr);
        auto const config = mty::api::Config::resolve(args);
        EXPECT_EQ(config.includePath(), (package.path() / "include").string());
        EXPECT_EQ(config.libraryPath(), (package.path() / "lib").string());
    }
    {
        EnvVar     include("MARTY_INCLUDE_PATH", envInclude.path().c_str());
        EnvVar     lib("MARTY_LIBRARY_PATH", envLib.path().c_str());
        auto const config = mty::api::Config::resolve(args);
        EXPECT_EQ(config.includePath(), envInclude.path().string());
        EXPECT_EQ(config.libraryPath(), envLib.path().string());

        // An explicit argument beats the environment.
        args.includePath = "/explicit/include";
        EXPECT_EQ(mty::api::Config::resolve(args).includePath(),
                  "/explicit/include");
    }
}

TEST(ConfigApi, EnvironmentCompilerOverridesDefaultsAndCxxVariable)
{
    TempDir        bin;
    fs::path const marty = touch(bin.path() / "marty-g++", true);
    fs::path const other = touch(bin.path() / "other-g++", true);

    EnvVar path("PATH", bin.path().c_str());
    EnvVar cxx("CXX", "other-g++");
    EnvVar martyCxx("MARTY_CXX", "marty-g++");
    EXPECT_EQ(mty::api::Config::resolve().cxx(), marty.string());

    EnvVar noMartyCxx("MARTY_CXX", nullptr);
    EXPECT_EQ(mty::api::Config::resolve().cxx(), other.string());

    mty::api::ConfigArguments args;
    args.cxx = marty.string();
    EXPECT_EQ(mty::api::Config::resolve(args).cxx(), marty.string());
}

TEST(ConfigApi, MissingCompilerReportsBuildError)
{
    TempDir empty;
    EnvVar  path("PATH", empty.path().c_str());
    EnvVar  martyCxx("MARTY_CXX", nullptr);
    EnvVar  cxx("CXX", nullptr);

    auto const config = mty::api::Config::resolve();
    EXPECT_FALSE(config.hasCxx());
    EXPECT_THROW(config.cxx(), mty::api::BuildError);
    EXPECT_THROW(config.make(), mty::api::BuildError);

    // A name that was asked for explicitly is an error too, without fallback.
    EnvVar missing("MARTY_CXX", "no-such-compiler");
    try {
        mty::api::Config::resolve().cxx();
        ADD_FAILURE() << "no exception was thrown";
    }
    catch (mty::api::BuildError const &ex) {
        EXPECT_NE(std::string(ex.what()).find("no-such-compiler"),
                  std::string::npos)
            << ex.what();
    }
}

TEST(ConfigApi, NoShellInvolved)
{
    TempDir bin;
    TempDir work;

    // Hostile commands would create "PWNED" in the working directory.
    struct CurrentPath {
        fs::path old = fs::current_path();
        ~CurrentPath()
        {
            fs::current_path(old);
        }
    } restoreCwd;
    fs::current_path(work.path());

    // A file whose name is full of shell metacharacters is just a name.
    std::string const hostile = "evil; touch PWNED #$(touch PWNED)`touch "
                                "PWNED`";
    fs::path const    file    = touch(bin.path() / hostile, true);

    EnvVar path("PATH", bin.path().c_str());
    EnvVar cxx("CXX", nullptr);
    {
        EnvVar martyCxx("MARTY_CXX", hostile.c_str());
        EXPECT_EQ(mty::api::Config::resolve().cxx(), file.string());
    }
    {
        // A hostile name that does not exist is reported, never executed.
        EnvVar martyCxx("MARTY_CXX", "x; touch PWNED");
        EXPECT_THROW(mty::api::Config::resolve().cxx(), mty::api::BuildError);
    }
    EXPECT_FALSE(fs::exists(work.path() / "PWNED"));
}
