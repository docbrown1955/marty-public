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
 * @file visualization.h
 * @brief Export of diagrams to JSON and LaTeX, PNG rendering and the GRAFED
 * viewer.
 *
 * Nothing here computes physics: diagrams come from the core, and the
 * functions only serialize them, run external tools and report their status.
 * Each external process is started with an argument vector (no shell).
 */

#ifndef MTY_API_VISUALIZATION_VISUALIZATION_H_INCLUDED
#define MTY_API_VISUALIZATION_VISUALIZATION_H_INCLUDED

#include "../modeling/modeling.h"

#include <string>
#include <sys/types.h>

namespace mty::api::visualization {

/// Diagrams of the amplitude as a JSON document read by GRAFED.
std::string diagramsJson(modeling::Amplitude const &amplitude);

/// Writes the JSON document of the amplitude to \b path, readable by the
/// owner only.
void saveDiagrams(modeling::Amplitude const &amplitude,
                  std::string const         &path);

/// Standalone LaTeX document drawing every diagram of the amplitude
/// (tikz-feynman).
std::string diagramsLatex(modeling::Amplitude const &amplitude);

/// Standalone LaTeX document with one block per Feynman rule (tikz-feynman).
std::string rulesLatex(modeling::FeynmanRules const &rules);

/**
 * @brief Renders a standalone LaTeX document to PNG bytes.
 *
 * Runs `latex` and `dvipng` in a private temporary directory that is removed
 * afterwards. StateError when one of them is missing or LaTeX fails; the
 * message then carries the end of the LaTeX log.
 */
std::string renderPng(std::string const &latexDocument);

/**
 * @brief Path of the GRAFED executable, or an empty string when not found.
 *
 * Searched in this order: \b explicitPath when not empty (it must be
 * executable), the MARTY_GRAFED environment variable, the PATH, then the
 * bin directory of the CMake install prefix.
 */
std::string findGrafed(std::string const &explicitPath = "");

/// Starts \b executable on \b jsonPath without waiting. Returns the pid.
pid_t launchGrafed(std::string const &executable, std::string const &jsonPath);

/// Waits for the process \b pid. Returns its exit status, or -1 if it was
/// killed by a signal.
int waitProcess(pid_t pid);

} // namespace mty::api::visualization

#endif
