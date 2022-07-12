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
#ifndef CAST_H_INCLUDED
#define CAST_H_INCLUDED

#include "abstract.h"
#include "error.h"
#include "utils.h"
#include <type_traits>
#include <typeinfo>

namespace csl {

template <class T>
T pointer_to_object(const Expr &expr)
{
    auto const &pointed = *expr;
    if (typeid(T) != typeid(pointed)) {
        std::cout << typeid(T).name() << "\n"
                  << typeid(pointed).name() << "\n";
        CALL_SMERROR(CSLError::RuntimeError);
    }

    return *static_cast<T *>(expr.get());
}

template <class T>
T &pointer_to_object_ref(const Expr &expr)
{
    auto const &pointed = *expr;
    if (typeid(T) != typeid(pointed))
        CALL_SMERROR(CSLError::RuntimeError);

    return *static_cast<T *>(expr.get());
}

template <typename T>
T *shared_to_raw_ptr(const Expr_c &expr)
{
    auto const &pointed = *expr;
    if (typeid(T) != typeid(pointed))
        CALL_SMERROR(CSLError::RuntimeError);

    return static_cast<T *>(expr.get());
}

template <typename T>
Expr object_to_shared(T &csl_expr)
{
    static_assert(std::is_base_of<Abstract, T>::value);

    return csl_expr.self();
}

} // End of namespace csl

#endif
