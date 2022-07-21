/*!
 * @file charge.h
 *
 * @brief Contains the mty::experimental::ufo::Charge structure.
 */
#pragma once

namespace mty::experimental::ufo
{

    /*!
     * @brief Simple container for the electric charge of a particle.
     *
     * @details The electric charge must be a rational number, with integer
     * numerator and denominator.
     */
    struct Charge
    {
        int num;   /*!< Numerator of the charge. */
        int denom; /*!< Denominator of the charge. */
    };

} // namespace mty::experimental::ufo
