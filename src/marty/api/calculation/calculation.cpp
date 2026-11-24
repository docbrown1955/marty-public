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

#include "calculation.h"

#include "../common/error.h"
#include "../common/session.h"
#include "csl/options.h"
#include "marty/core/amplitude.h"
#include "marty/core/drawer.h"
#include "marty/core/mrtOptions.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace mty::api::calculation {
namespace {

struct FlagRef {
    std::string name;
    bool       *flag;
};

std::string snakeCase(std::string_view camel)
{
    std::string out;
    for (char c : camel) {
        if (std::isupper(static_cast<unsigned char>(c))) {
            out += '_';
            out += static_cast<char>(
                std::tolower(static_cast<unsigned char>(c)));
        }
        else {
            out += c;
        }
    }
    return out;
}

// Every global boolean flag, keyed by its snake_case name. printErrors exists
// in both mty::option and csl::option; the first one wins.
std::vector<FlagRef> const &flags()
{
    static std::vector<FlagRef> const table = [] {
        std::vector<FlagRef> result;
        auto                 add = [&](std::string_view name, bool *flag) {
            auto key = snakeCase(name);
            bool known
                = std::any_of(result.begin(),
                              result.end(),
                              [&](FlagRef const &f) { return f.name == key; });
            if (!known)
                result.push_back({key, flag});
        };
#define MTY_CALC_ADD_FLAG(ns, name) add(#name, &ns::name);
        MTY_MARTY_OPTION_FLAGS(MTY_CALC_ADD_FLAG)
        MTY_CSL_OPTION_FLAGS(MTY_CALC_ADD_FLAG)
#undef MTY_CALC_ADD_FLAG
        return result;
    }();
    return table;
}

FlagRef const &findFlag(std::string const &name)
{
    auto const &table = flags();
    auto        found
        = std::find_if(table.begin(), table.end(), [&](FlagRef const &f) {
              return f.name == name;
          });
    if (found == table.end()) {
        std::string valid;
        for (auto const &f : table)
            valid += (valid.empty() ? "" : ", ") + f.name;
        throw ValueError("Unknown option '" + name
                         + "'. Valid options: " + valid + ".");
    }
    return *found;
}

void checkIndex(modeling::Amplitude const &amplitude, std::size_t index)
{
    if (index >= amplitude.raw().size())
        throw LookupError("Diagram index " + std::to_string(index)
                          + " is out of range.");
}

} // namespace

std::string displayAmplitude(modeling::Amplitude const &amplitude)
{
    std::ostringstream out;
    auto const        &raw = amplitude.raw();
    for (std::size_t i = 0; i < raw.size(); ++i)
        out << raw.expression(i) << '\n';
    return out.str();
}

std::vector<std::string> diagramParticles(modeling::Amplitude const &amplitude,
                                          std::size_t                index)
{
    checkIndex(amplitude, index);
    std::vector<std::string> names;
    for (auto const &particle :
         amplitude.raw().getDiagrams()[index].getParticles()) {
        auto name = std::string(particle->getName());
        if (std::find(names.begin(), names.end(), name) == names.end())
            names.push_back(name);
    }
    return names;
}

csl::Expr diagramExpression(modeling::Amplitude const &amplitude,
                            std::size_t                index)
{
    checkIndex(amplitude, index);
    return amplitude.raw().expression(index);
}

std::vector<std::string> optionNames()
{
    std::vector<std::string> names;
    for (auto const &flag : flags())
        names.push_back(flag.name);
    return names;
}

bool getOption(std::string const &name)
{
    return *findFlag(name).flag;
}

void setOption(std::string const &name, bool value)
{
    *findFlag(name).flag = value;
}

} // namespace mty::api::calculation
