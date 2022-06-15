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
#ifndef CSLOPTIONS_H_INCLUDED
#define CSLOPTIONS_H_INCLUDED

namespace csl::option {

inline bool errorStopsProgram           = true;
inline bool checkCommutations           = true;
inline bool freezeMerge                 = false;
inline bool canonicalSumNumericalFactor = true;

inline bool applySelfContractions  = true;
inline bool applyChainContractions = false;

inline bool fullComparison = true;
inline bool printIndexIds  = true;

} // namespace csl::option

#endif
