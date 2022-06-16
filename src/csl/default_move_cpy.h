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
#ifndef CSL_DEFAULT_MV_CPY_H_INCLUDED
#define CSL_DEFAULT_MV_CPY_H_INCLUDED

#define DEFINE_DEFAULT_CPY_CONSTRUCTOR(className) \
    className(className const &) = default;
#define DEFINE_DEFAULT_MV_CONSTRUCTOR(className) \
    className(className &&) = default;
#define DEFINE_DEFAULT_MC_CONSTRUCTOR(className) \
    DEFINE_DEFAULT_CPY_CONSTRUCTOR(className)    \
    DEFINE_DEFAULT_MV_CONSTRUCTOR(className)

#define DEFINE_DEFAULT_CPY_ASSIGNEMENT(className) \
    className &operator=(className const &) = default;
#define DEFINE_DEFAULT_MV_ASSIGNEMENT(className) \
    className &operator=(className &&) = default;
#define DEFINE_DEFAULT_MC_ASSIGNEMENT(className) \
    DEFINE_DEFAULT_CPY_ASSIGNEMENT(className)    \
    DEFINE_DEFAULT_MV_ASSIGNEMENT(className)

#define DEFINE_DEFAULT_CPY(className)         \
    DEFINE_DEFAULT_CPY_CONSTRUCTOR(className) \
    DEFINE_DEFAULT_CPY_ASSIGNEMENT(className)

#define DEFINE_DEFAULT_MV(className)         \
    DEFINE_DEFAULT_MV_CONSTRUCTOR(className) \
    DEFINE_DEFAULT_MV_ASSIGNEMENT(className)

#define DEFINE_DEFAULT_CPY_MV(className) \
    DEFINE_DEFAULT_CPY(className)        \
    DEFINE_DEFAULT_MV(className)

#endif
