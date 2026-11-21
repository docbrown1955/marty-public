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

#include "feynmanRules.h"

#include "../common/error.h"
#include "../common/session.h"
#include "marty/core/mrtInterface.h"
#include "modeling.h"

#include <sstream>

namespace mty::api::modeling {

FeynmanRules::FeynmanRules(std::shared_ptr<detail::ModelState> state)
    : m_state(std::move(state))
{
}

std::size_t FeynmanRules::size() const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return m_state->core->getFeynmanRules().size();
    });
}

std::string FeynmanRules::str(std::size_t index) const
{
    return guarded([&] {
        Session     session(m_state->core.get());
        auto const &rules = m_state->core->getFeynmanRules();
        if (index >= rules.size())
            throw LookupError("no Feynman rule at index "
                              + std::to_string(index));
        std::ostringstream sout;
        sout << rules[index];
        return sout.str();
    });
}

std::string FeynmanRules::latex(std::size_t index) const
{
    return guarded([&] {
        Session     session(m_state->core.get());
        auto const &rules = m_state->core->getFeynmanRules();
        if (index >= rules.size())
            throw LookupError("no Feynman rule at index "
                              + std::to_string(index));
        return LatexFeynmanRule(rules[index]);
    });
}

std::string FeynmanRules::latexDocument() const
{
    return guarded([&] {
        Session session(m_state->core.get());
        return LatexFeynmanRules(m_state->core->getFeynmanRules());
    });
}

} // namespace mty::api::modeling
