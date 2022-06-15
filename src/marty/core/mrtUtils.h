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
 * @version 1.3

 * \brief
 */
#ifndef HEPUTILS_H_INCLUDED
#define HEPUTILS_H_INCLUDED

#include "../../csl/csl.h"
#include "mrtError.h"

namespace mty {

template <typename T, typename U>
bool IsOfType(std::shared_ptr<const U> const &expr)
{
    auto const &object = *expr;
    return typeid(object) == typeid(T);
}

template <typename T, typename U>
bool IsOfType(U const *expr)
{
    auto const &object = *expr;
    return typeid(object) == typeid(T);
}

template <typename T, typename U>
bool IsOfType(U const &expr)
{
    auto const &object = *expr;
    return typeid(object) == typeid(T);
}

template <typename T, typename U>
std::shared_ptr<T> ConvertToShared(std::shared_ptr<U> const &expr)
{
    auto ptr = std::dynamic_pointer_cast<T>(expr);
    HEPAssert(ptr,
              mty::error::TypeError,
              "Converting an expr to the wrong type in"
                  + std::string("ConvertToShared<T>(const csl::Expr&): type =")
                  + typeid(T).name() + ", expr = " + toString(expr));
    return ptr;
}

template <typename T, typename U>
T *ConvertToPtr(std::shared_ptr<U> const &expr)
{
    return ConvertToShared<T>(expr).get();
}

template <typename T, typename U>
const T *ConvertToPtr(const U *expr)
{
    auto ptr = dynamic_cast<const T *>(expr);
    HEPAssert(ptr,
              mty::error::TypeError,
              "Converting an expr to the wrong type in"
                  + std::string("ConvertToShared<T>(const csl::Expr&): type =")
                  + typeid(T).name() + ", expr = " + toString(expr));

    return ptr;
}

template <typename T, typename U>
T ConvertTo(std::shared_ptr<U> const &expr)
{
    return *ConvertToPtr<T>(expr);
}

template <typename T, typename U>
T &ConvertTo(std::shared_ptr<U> &expr)
{
    return *ConvertToPtr<T>(expr);
}

template <typename T, typename U>
T *ConvertToPtr(U *parent)
{
    return dynamic_cast<T *>(parent);
}

} // End of namespace mty

#endif
