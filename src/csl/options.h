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
#ifndef CSLOPTIONS_H_INCLUDED
#define CSLOPTIONS_H_INCLUDED

namespace csl::option {

inline bool errorStopsProgram           = true;
inline bool checkCommutations           = true;
inline bool freezeMerge                 = false;
inline bool canonicalSumNumericalFactor = true;

inline bool applySelfContractions  = true;
inline bool applyChainContractions = false;

inline bool fullComparison             = true;
inline bool permissiveCovariantIndices = false;
inline bool printIndexIds              = true;

/// When false, CSL errors are not printed to std::cerr before being thrown.
/// The API Session turns it off for the duration of an API call.
inline bool printErrors = true;

} // namespace csl::option

/// X-macro over every flag of csl::option. Session snapshots exactly this
/// list. The C++ API tests fail if a flag declared in this header is missing.
#define MTY_CSL_OPTION_FLAGS(X)                 \
    X(csl::option, errorStopsProgram)           \
    X(csl::option, checkCommutations)           \
    X(csl::option, freezeMerge)                 \
    X(csl::option, canonicalSumNumericalFactor) \
    X(csl::option, applySelfContractions)       \
    X(csl::option, applyChainContractions)      \
    X(csl::option, fullComparison)              \
    X(csl::option, permissiveCovariantIndices)  \
    X(csl::option, printIndexIds)               \
    X(csl::option, printErrors)

#endif
