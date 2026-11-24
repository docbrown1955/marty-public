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
 * @file calculation.h
 * @brief Read access to computed amplitudes, export and global options.
 *
 * Amplitudes are computed by mty::api::modeling::Model::computeAmplitude().
 * The functions here inspect their diagrams, export them, and set the global
 * flags of MARTY and CSL. Errors are typed (see error.h).
 */

#ifndef MTY_API_CALCULATION_CALCULATION_H_INCLUDED
#define MTY_API_CALCULATION_CALCULATION_H_INCLUDED

#include "../../../csl/api/expr.h"
#include "../modeling/modeling.h"

#include <cstddef>
#include <string>
#include <vector>

namespace mty::api::calculation {

/// Printed form of every diagram expression of the amplitude, one per line.
std::string displayAmplitude(modeling::Amplitude const &amplitude);

/// Names of the particles of diagram \b index (LookupError if out of range).
std::vector<std::string> diagramParticles(modeling::Amplitude const &amplitude,
                                          std::size_t                index);

/// Expression of diagram \b index (LookupError if out of range).
csl::Expr diagramExpression(modeling::Amplitude const &amplitude,
                            std::size_t                index);

/// Names of the boolean global options, in snake_case.
std::vector<std::string> optionNames();

/// Value of a global option (ValueError for an unknown name).
bool getOption(std::string const &name);

/// Sets a global option (ValueError for an unknown name).
void setOption(std::string const &name, bool value);

} // namespace mty::api::calculation

#endif
