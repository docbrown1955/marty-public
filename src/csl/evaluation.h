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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Contains evaluation flags for csl.
 */
#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED

#include <bitset>

namespace csl {

namespace eval {

using mode = std::bitset<6>;

constexpr inline mode base(0);
const inline mode     literal(mode(1) | base);
const inline mode     numerical(mode(2) | base);
const inline mode     indicial(mode(4) | base);
const inline mode     abbreviation(mode(8) | base);
const inline mode all(base | literal | numerical | indicial | abbreviation);

/*!
 * \brief Tells if the defined mode is contained in the user_mode.
 */
bool isContained(mode const &user_mode, mode const &defined_mode);
} // namespace eval

} // End of namespace csl

#endif
