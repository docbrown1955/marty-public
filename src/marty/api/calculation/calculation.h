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
#include "../../core/wilson.h"
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

/// Names of the mediator particles of diagram \b index (LookupError if out of
/// range).
std::vector<std::string> diagramMediators(modeling::Amplitude const &amplitude,
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

/// Wilson coefficients of an amplitude, read by operator.
class WilsonSet {
  public:
    explicit WilsonSet(mty::WilsonSet wilsons);

    /// Coefficient of the sum of the operators, given as expressions.
    csl::Expr coefficient(std::vector<csl::Expr> const &operators) const;

    mty::WilsonSet const &raw() const
    {
        return m_wilsons;
    }

  private:
    mty::WilsonSet m_wilsons;
};

/// Wilson coefficients of \b amplitude, with \b operatorCoefficient as the
/// overall factor of every operator. With \b dedicatedMatching, the model's
/// one-loop matching on the amplitude's insertions is used instead of the
/// amplitude decomposition (required for four external fermions).
WilsonSet wilsonCoefficients(modeling::Model const     &model,
                             modeling::Amplitude const &amplitude,
                             csl::Expr const           &operatorCoefficient,
                             bool                       dedicatedMatching = false);

/// Expressions of the chromomagnetic operator with the given Dirac coupling
/// (S, P, L, R, V, A, VL, VR, T, TA, TL or TR, ValueError otherwise).
std::vector<csl::Expr> chromoMagneticOperator(modeling::Model const &model,
                                              WilsonSet const       &wilsons,
                                              std::string const     &coupling);

/// Coefficient of the chromomagnetic operator with the given Dirac coupling,
/// summed over the matching Wilson coefficients of ``wilsons``.
csl::Expr chromoMagneticCoefficient(modeling::Model const &model,
                                    WilsonSet const       &wilsons,
                                    std::string const     &coupling);

/// Coefficient of the dimension-6 operator built from the two Dirac couplings
/// (left and right currents, same names as chromoMagneticOperator), summed over
/// the matching Wilson coefficients of ``wilsons``.
/// An empty ``colorGroup`` means no color structure; otherwise ``colorCoupling``
/// is one of Id, Generator, Crossed or InvCrossed.
csl::Expr dimension6Coefficient(modeling::Model const &model,
                                WilsonSet const       &wilsons,
                                std::string const     &leftCoupling,
                                std::string const     &rightCoupling,
                                std::string const     &colorGroup    = "",
                                std::string const     &colorCoupling = "Id");

} // namespace mty::api::calculation

#endif
