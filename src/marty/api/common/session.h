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
 * @file session.h
 * @brief RAII guard for the global state used by the computations.
 */

#ifndef MTY_API_COMMON_SESSION_H_INCLUDED
#define MTY_API_COMMON_SESSION_H_INCLUDED

#include "../../../csl/options.h"
#include "../../core/model.h"
#include "../../core/mrtError.h"
#include "../../core/mrtOptions.h"

#include <utility>
#include <vector>

namespace mty::api {

/**
 * @brief Snapshot of the global state, restored on destruction.
 *
 * Constructing a Session records `Model::current`, every flag of the two
 * X-macro lists (mty::option, csl::option, including the gamma 4D flag) and
 * silences error printing. The destructor restores all of it, also during
 * stack unwinding. An optional model becomes `Model::current` for the
 * duration of the session.
 *
 * Single threaded: one Session at a time.
 */
class Session {
  public:
    explicit Session(mty::Model *model = nullptr)
        : m_currentModel(mty::Model::current)
    {
#define MTY_SESSION_SAVE(ns, name) m_flags.emplace_back(&ns::name, ns::name);
        MTY_MARTY_OPTION_FLAGS(MTY_SESSION_SAVE)
        MTY_CSL_OPTION_FLAGS(MTY_SESSION_SAVE)
#undef MTY_SESSION_SAVE

        mty::option::printErrors = false;
        csl::option::printErrors = false;
        if (model != nullptr)
            mty::Model::current = model;
    }

    Session(Session const &)            = delete;
    Session &operator=(Session const &) = delete;
    Session(Session &&)                 = delete;
    Session &operator=(Session &&)      = delete;

    ~Session()
    {
        for (auto const &[flag, value] : m_flags)
            *flag = value;
        mty::Model::current = m_currentModel;
    }

  private:
    mty::Model                          *m_currentModel;
    std::vector<std::pair<bool *, bool>> m_flags;
};

} // namespace mty::api

#endif
