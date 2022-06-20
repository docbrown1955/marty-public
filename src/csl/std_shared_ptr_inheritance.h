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
#pragma once
#define INHERIT_SHARED_PTR_CONSTRUCTOR(Class, type)                          \
    template <class... Args>                                                 \
    using _Constructible = std::shared_ptr<type>::_Constructible<Args...>;   \
    template <class Arg>                                                     \
    using _Assignable = std::shared_ptr<type>::_Assignable<Arg>;             \
                                                                             \
    constexpr Class() : std::shared_ptr<type>(){};                           \
                                                                             \
    template <class Y, class = _Constructible<Y *>>                          \
    explicit Class(Y *ptr) : std::shared_ptr<type>(ptr)                      \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y, class Deleter, class = _Constructible<Y *, Deleter>>  \
    Class(Y *ptr, Deleter d) : std::shared_ptr<type>(ptr, d)                 \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y,                                                       \
              class Deleter,                                                 \
              class Alloc,                                                   \
              class = _Constructible<Y *, Deleter, Alloc>>                   \
    Class(Y *ptr, Deleter d, Alloc alloc)                                    \
        : std::shared_ptr<type>(ptr, d, alloc)                               \
    {                                                                        \
    }                                                                        \
                                                                             \
    constexpr Class(std::nullptr_t) : std::shared_ptr<type>(nullptr)         \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Deleter>                                                 \
    Class(std::nullptr_t, Deleter d) : std::shared_ptr<type>(nullptr, d)     \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Deleter, class Alloc>                                    \
    Class(std::nullptr_t, Deleter d, Alloc alloc)                            \
        : std::shared_ptr<type>(nullptr, d, alloc)                           \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y>                                                       \
    Class(const shared_ptr<Y> &r, type *ptr) : std::shared_ptr<type>(r, ptr) \
    {                                                                        \
    }                                                                        \
                                                                             \
    Class(const shared_ptr<type> &r) : std::shared_ptr<type>(r)              \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y, class = _Constructible<const std::shared_ptr<Y> &>>   \
    Class(const shared_ptr<Y> &r) : std::shared_ptr<type>(r)                 \
    {                                                                        \
    }                                                                        \
                                                                             \
    Class(shared_ptr<type> &&r) : std::shared_ptr<type>(r)                   \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y, class = _Constructible<std::shared_ptr<Y>>>           \
    Class(shared_ptr<Y> &&r) : std::shared_ptr<type>(r)                      \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y, class = _Constructible<const std::weak_ptr<Y> &>>     \
    explicit Class(const std::weak_ptr<Y> &r) : std::shared_ptr<type>(r)     \
    {                                                                        \
    }                                                                        \
                                                                             \
    template <class Y,                                                       \
              class Deleter,                                                 \
              class = _Constructible<std::unique_ptr<Y, Deleter>>>           \
    Class(std::unique_ptr<Y, Deleter> &&r)                                   \
        : std::shared_ptr<type>(std::move(r))                                \
    {                                                                        \
    }

#define DEFINE_SHARED_PTR_OPERATOR(Class)                     \
    inline bool operator==(Class const &expr, std::nullptr_t) \
    {                                                         \
        return expr.get() == nullptr;                         \
    }                                                         \
    inline bool operator==(std::nullptr_t, Class const &expr) \
    {                                                         \
        return expr.get() == nullptr;                         \
    }                                                         \
    inline bool operator!=(Class const &expr, std::nullptr_t) \
    {                                                         \
        return expr.get() != nullptr;                         \
    }                                                         \
    inline bool operator!=(std::nullptr_t, Class const &expr) \
    {                                                         \
        return expr.get() != nullptr;                         \
    }
