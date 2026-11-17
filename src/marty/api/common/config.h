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
 * @file config.h
 * @brief Run-time discovery of the files MARTY needs outside the library.
 *
 * Resolution order for every item: explicit argument, then environment, then
 * package data (`<packageData>/{include,lib,bin}`), then system locations.
 * The compile-time definitions MARTY_CXX and MARTY_INSTALL_PATH are not used.
 *
 * Programs are looked up by scanning `PATH` directly. No shell, `system()` or
 * `popen()` is ever involved, so names are plain data.
 *
 * Explicit arguments and environment values are taken as the user's decision:
 * a program name that cannot be found is an error, there is no silent
 * fallback to the next source. Include and library directories coming from an
 * argument or the environment are used as given.
 *
 * Environment variables: MARTY_INCLUDE_PATH, MARTY_LIBRARY_PATH, MARTY_CXX,
 * CXX (only if MARTY_CXX is unset), MARTY_GRAFED.
 */

#ifndef MTY_API_COMMON_CONFIG_H_INCLUDED
#define MTY_API_COMMON_CONFIG_H_INCLUDED

#include "error.h"

#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace mty::api {

/// Explicit values for Config::resolve(). Empty strings mean "not given".
struct ConfigArguments {
    std::string includePath;
    std::string libraryPath;
    std::string cxx;
    std::string make;
    std::string grafed;
    /// Root of the package data (`include/`, `lib/`, `bin/` below it).
    std::string packageData;
};

class Config {
  public:
    /// Resolves every item from the arguments, the environment, the package
    /// data and the system locations. Never throws: items that cannot be found
    /// are reported by the accessors.
    static Config resolve(ConfigArguments const &args = {})
    {
        namespace fs = std::filesystem;
        Config      config;
        std::string cxxVar = environment("MARTY_CXX");
        if (cxxVar.empty())
            cxxVar = environment("CXX");

        config.m_includePath = resolveDirectory(
            args.includePath,
            environment("MARTY_INCLUDE_PATH"),
            args.packageData,
            "include",
            {"/usr/local/include", "/usr/include"},
            [](fs::path const &dir) { return fs::exists(dir / "marty.h"); });
        config.m_libraryPath = resolveDirectory(
            args.libraryPath,
            environment("MARTY_LIBRARY_PATH"),
            args.packageData,
            "lib",
            {"/usr/local/lib", "/usr/lib", "/usr/lib/x86_64-linux-gnu"},
            [](fs::path const &dir) {
                return fs::exists(dir / "libmarty.so")
                       || fs::exists(dir / "libmarty-static.a")
                       || fs::exists(dir / "libmarty.a");
            });

        config.m_cxxRequested = firstNonEmpty(args.cxx, cxxVar);
        config.m_cxx = config.m_cxxRequested.empty()
                           ? findFirstInPath({"g++", "c++", "clang++"})
                           : findProgram(config.m_cxxRequested);

        config.m_makeRequested = args.make;
        config.m_make = args.make.empty() ? findFirstInPath({"make", "gmake"})
                                          : findProgram(args.make);

        std::string const grafedRequested
            = firstNonEmpty(args.grafed, environment("MARTY_GRAFED"));
        config.m_grafedRequested = grafedRequested;
        if (!grafedRequested.empty())
            config.m_grafed = findProgram(grafedRequested);
        else {
            if (!args.packageData.empty()) {
                auto const packaged
                    = fs::path(args.packageData) / "bin" / "grafed-gui";
                if (isExecutableFile(packaged))
                    config.m_grafed = packaged.string();
            }
            if (!config.m_grafed)
                config.m_grafed = findProgram("grafed-gui");
        }
        return config;
    }

    /// Directory containing `marty.h`.
    std::string const &includePath() const
    {
        return require(
            m_includePath, "MARTY include directory", "MARTY_INCLUDE_PATH");
    }

    /// Directory containing the MARTY and CSL libraries.
    std::string const &libraryPath() const
    {
        return require(
            m_libraryPath, "MARTY library directory", "MARTY_LIBRARY_PATH");
    }

    /// C++ compiler used to build generated libraries.
    std::string const &cxx() const
    {
        if (!m_cxx && !m_cxxRequested.empty())
            throw BuildError("C++ compiler '" + m_cxxRequested
                             + "' was not found");
        return require(m_cxx, "C++ compiler", "MARTY_CXX");
    }

    std::string const &make() const
    {
        if (!m_make && !m_makeRequested.empty())
            throw BuildError("make program '" + m_makeRequested
                             + "' was not found");
        return require(m_make, "make program", "PATH");
    }

    /// The GRAFED executable.
    std::string const &grafed() const
    {
        if (!m_grafed && !m_grafedRequested.empty())
            throw BuildError("GRAFED program '" + m_grafedRequested
                             + "' was not found");
        return require(m_grafed, "GRAFED program", "MARTY_GRAFED");
    }

    bool hasIncludePath() const
    {
        return m_includePath.has_value();
    }
    bool hasLibraryPath() const
    {
        return m_libraryPath.has_value();
    }
    bool hasCxx() const
    {
        return m_cxx.has_value();
    }
    bool hasMake() const
    {
        return m_make.has_value();
    }
    bool hasGrafed() const
    {
        return m_grafed.has_value();
    }

    /// Finds an executable by name or path without invoking a shell. A name
    /// without '/' is searched in the directories of `PATH`. Returns nullopt
    /// if nothing executable is found.
    static std::optional<std::string> findProgram(std::string_view name)
    {
        if (name.empty() || name.find('\0') != std::string_view::npos)
            return std::nullopt;
        if (name.find('/') != std::string_view::npos) {
            std::filesystem::path const path(name);
            return isExecutableFile(path)
                       ? std::optional<std::string>(path.string())
                       : std::nullopt;
        }
        std::string const pathVar = environment("PATH");
        std::size_t       start   = 0;
        while (start <= pathVar.size()) {
            std::size_t end = pathVar.find(':', start);
            if (end == std::string::npos)
                end = pathVar.size();
            std::string dir = pathVar.substr(start, end - start);
            if (dir.empty())
                dir = ".";
            auto const candidate
                = std::filesystem::path(dir) / std::string(name);
            if (isExecutableFile(candidate))
                return candidate.string();
            start = end + 1;
        }
        return std::nullopt;
    }

  private:
    static std::string environment(char const *name)
    {
        char const *value = std::getenv(name);
        return value ? value : "";
    }

    static std::string firstNonEmpty(std::string const &a,
                                     std::string const &b)
    {
        return a.empty() ? b : a;
    }

    static bool isExecutableFile(std::filesystem::path const &path)
    {
        std::error_code ec;
        return std::filesystem::is_regular_file(path, ec)
               && ::access(path.c_str(), X_OK) == 0;
    }

    static std::optional<std::string>
    findFirstInPath(std::vector<std::string_view> const &names)
    {
        for (auto const name : names)
            if (auto found = findProgram(name))
                return found;
        return std::nullopt;
    }

    template <class Valid>
    static std::optional<std::string>
    resolveDirectory(std::string const              &argument,
                     std::string const              &environmentValue,
                     std::string const              &packageData,
                     char const                     *packageSubdir,
                     std::vector<std::string> const &systemDirs,
                     Valid                           valid)
    {
        if (!argument.empty())
            return argument;
        if (!environmentValue.empty())
            return environmentValue;
        if (!packageData.empty()) {
            auto const dir
                = std::filesystem::path(packageData) / packageSubdir;
            if (valid(dir))
                return dir.string();
        }
        for (auto const &dir : systemDirs)
            if (valid(std::filesystem::path(dir)))
                return dir;
        return std::nullopt;
    }

    static std::string const &require(std::optional<std::string> const &value,
                                      char const                       *what,
                                      char const *variable)
    {
        if (!value)
            throw BuildError(std::string(what) + " not found; set " + variable
                             + " or pass it explicitly");
        return *value;
    }

    std::optional<std::string> m_includePath;
    std::optional<std::string> m_libraryPath;
    std::optional<std::string> m_cxx;
    std::optional<std::string> m_make;
    std::optional<std::string> m_grafed;
    std::string                m_cxxRequested;
    std::string                m_makeRequested;
    std::string                m_grafedRequested;
};

} // namespace mty::api

#endif
