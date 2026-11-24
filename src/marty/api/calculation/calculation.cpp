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
#include "marty/core/builtinOperators.h"
#include "marty/core/drawer.h"
#include "marty/core/mrtOptions.h"
#include "marty/core/wilsonOperator.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <unistd.h>
#include <utility>

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

std::vector<std::string> diagramMediators(modeling::Amplitude const &amplitude,
                                          std::size_t                index)
{
    checkIndex(amplitude, index);
    auto const &diagram = amplitude.raw().getDiagrams()[index];
    std::vector<std::string> names;
    for (auto const &particle : diagram.getParticles()) {
        auto name = std::string(particle->getName());
        if (diagram.isMediator(particle)
            && std::find(names.begin(), names.end(), name) == names.end())
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

WilsonSet::WilsonSet(mty::WilsonSet wilsons) : m_wilsons(std::move(wilsons))
{
}

csl::Expr WilsonSet::coefficient(std::vector<csl::Expr> const &operators) const
{
    return guarded(
        [&] { return mty::getWilsonCoefficient(m_wilsons, operators); });
}

WilsonSet wilsonCoefficients(modeling::Model const     &model,
                             modeling::Amplitude const &amplitude,
                             csl::Expr const           &operatorCoefficient)
{
    return guarded([&] {
        auto            &core = modeling::coreModel(model);
        Session          session(&core);
        mty::FeynOptions options;
        options.setWilsonOperatorCoefficient(operatorCoefficient);
        return WilsonSet(core.getWilsonCoefficients(amplitude.raw(), options));
    });
}

std::vector<csl::Expr> chromoMagneticOperator(modeling::Model const &model,
                                              WilsonSet const       &wilsons,
                                              std::string const     &coupling)
{
    static std::map<std::string, mty::DiracCoupling> const couplings{
        {"S", mty::DiracCoupling::S},
        {"P", mty::DiracCoupling::P},
        {"L", mty::DiracCoupling::L},
        {"R", mty::DiracCoupling::R},
        {"V", mty::DiracCoupling::V},
        {"A", mty::DiracCoupling::A},
        {"VL", mty::DiracCoupling::VL},
        {"VR", mty::DiracCoupling::VR},
        {"T", mty::DiracCoupling::T},
        {"TA", mty::DiracCoupling::TA},
        {"TL", mty::DiracCoupling::TL},
        {"TR", mty::DiracCoupling::TR}};
    auto const found = couplings.find(coupling);
    if (found == couplings.end())
        throw ValueError("Unknown Dirac coupling '" + coupling + "'");
    return guarded([&] {
        auto                  &core = modeling::coreModel(model);
        Session                session(&core);
        std::vector<csl::Expr> operators;
        for (auto const &wilson :
             mty::chromoMagneticOperator(core, wilsons.raw(), found->second))
            operators.push_back(wilson.getExpression());
        return operators;
    });
}

} // namespace mty::api::calculation
