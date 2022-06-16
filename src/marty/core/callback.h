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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef CALLBACK_H_INCLUDED
#define CALLBACK_H_INCLUDED

#include <functional>
#include <optional>

namespace mty {

template <class ReturnType, class... Args>
struct CallBack {

    std::optional<std::function<ReturnType(Args...)>> func;

    CallBack()                      = default;
    CallBack(CallBack const &other) = default;
    CallBack &operator=(CallBack const &other) = default;
    CallBack(std::function<ReturnType(Args...)> const &t_func) : func(t_func)
    {
    }

    CallBack &operator=(std::function<ReturnType(Args...)> const &t_func)
    {
        func = t_func;
        return *this;
    }

    inline operator bool() const
    {
        return bool(func);
    }

    void reset()
    {
        func.reset();
    }

    bool apply(ReturnType &returnValue, Args &&... args) const
    {
        if (!func)
            return false;
        returnValue = (*func)(std::forward<Args>(args)...);
        return true;
    }
};

template <class... Args>
struct CallBack<void(Args...)> {

    std::optional<std::function<void(Args...)>> func;

    CallBack()                      = default;
    CallBack(CallBack const &other) = default;
    CallBack &operator=(CallBack const &other) = default;
    CallBack(std::function<void(Args...)> const &t_func) : func(t_func)
    {
    }

    CallBack &operator=(std::function<void(Args...)> const &t_func)
    {
        func = t_func;
        return *this;
    }

    inline operator bool() const
    {
        return bool(func);
    }

    void reset()
    {
        func.reset();
    }

    bool apply(Args &&... args) const
    {
        if (!func)
            return false;
        (*func)(std::forward<Args>(args)...);
        return true;
    }
};

} // namespace mty

#endif
