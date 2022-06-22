#pragma once

#include <vector>

namespace mty {

struct Wilson;
class Kinematics;
class Amplitude;

/**
 * @brief Tells if a set of wilson coefficients requires the penguin patch
 * implemented in this file. This function must return true for the
 * applyPenguinPatch() function to work (otherwise it raises an error).
 *
 * @param amplitude Amplitude of the process.
 *
 * @return \b True if the patch is required.
 * @return \b False otherwise.
 */
bool requiresPenguinPatch(Amplitude const &amplitude);
/**
 * @brief Applies the patch for the on-shell calculation of penguins with
 * massless vector.
 *
 * @details When the outgoing vector \f$ A \f$ of \f$ \psi\to\psi A \f$
 * transition is massless, the calculation of Wilson coefficients requires
 * the on-shell calculation with the inclusion of counter-terms for the
 * corrections of external fermions. As these counter-terms are not taken
 * care of automatically for now, this function applies gauge invariance to
 * recover the correct result, using the fact that the result is redundant
 * and that all quantities can be derived from the result without
 * counter-terms. For box diagrams or penguins with massive vector boson,
 * the calculation suffers from no issue.
 *
 * @param wilsons    Wilson Operator / Coefficients to correct.
 * @param kinematics Kinematica of the process.
 */
void applyPenguinPatch(std::vector<Wilson> &wilsons,
                       Kinematics const    &kinematics);

} // namespace mty
