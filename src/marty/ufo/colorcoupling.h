/*!
 * @file colorcoupling.h
 *
 * @brief Contains the mty::experimental::ufo::ColorCoupling class.
 */
#pragma once

#include <marty.h>
#include <vector>

namespace mty::experimental::ufo
{

    /*!
     * @brief Contains the color coupling for an interaction vertex.
     *
     * @details The color is limited to \f$ SU(3)_C \f$. It is expressed as a
     * simple CSL expression as the UFO format takes symbolic color couplings.
     * Examples of color couplings are:
     * \f[
     *    T^A_{ij}, \\
     *    f^{EAB}f^{ECD},
     * \f]
     * with \f$ T^A_{ij} \f$ and \f$ f^{ABC} \f$ \f$ SU(3)_C \f$ generators.
     *
     * @sa Vertex, LorentzCoupling
     */
    struct ColorCoupling
    {

        /*!
         * @brief Expression containing the color coupling of the vertex.
         */
        csl::Expr generators;
    };

} // namespace mty::experimental::ufo
