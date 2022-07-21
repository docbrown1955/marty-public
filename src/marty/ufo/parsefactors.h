/*!
 * @file parsefactors.h
 *
 * @brief Contains the algorithms parsing a MARTY Feynman rule to extract
 * separate contributions.
 *
 * @details From a MARTY Feynman rules, Dirac, Minkowski, color and constant
 * factors must be identified and factored from each other to facilitate the
 * conversion to UFO format. The algorithms doing this are defined in this file,
 * and must be applied **only** on expressions that can be factored (i.e. have
 * already been expanded if necessary).
 *
 * @note A user of MARTY-UFO probably does not need to go through this file.
 */
#pragma once

#include <marty.h>

namespace mty::experimental::ufo
{

    csl::Expr getConstantFactor(
        csl::Expr const &prod);

    bool containsVectorSpace(
        csl::Expr const &expr,
        csl::Space const *space);

    bool containsLorentzFactor(csl::Expr const &expr);

    bool containsDiracFactor(csl::Expr const &expr);

    bool containsColorFactor(
        csl::Expr const &expr,
        std::vector<csl::Space const *> const &colors);

    struct FactorDecomposition
    {
        csl::Expr coupling;
        csl::Expr lorentz;
        csl::Expr dirac;
        csl::Expr color;
    };

    FactorDecomposition decomposeExpression(
        csl::Expr const &init,
        std::vector<csl::Space const *> const &colors);

} // namespace mty::experimental::ufo
