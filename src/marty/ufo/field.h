/*!
 * @file field.h
 *
 * @brief Contains the mty::experimental::ufo::Field class.
 */
#pragma once

#include "charge.h"
#include <marty.h>
#include <string>

namespace mty::experimental::ufo
{

    /*!
     * @brief Representes an external field in an interaction vertex.
     *
     * @details This object contains all information about the particle species
     * and field insertion required for the generation of UFO vertices. In
     * particular the particule name, its momentum (that can be found in the
     * vertex expression), its representation (spin, electric charge and color
     * dimension), its complex conjugation and indices.
     *
     * In the electromagnetic interaction of the tau \f$\tau\f$ for example,
     * the external particle (tau, anti-tau and photon) could be defined as:
     * \f[
     *     \tau_\alpha^*[p_1; 1/2, -1, 1], \\
     *     \tau_\beta[p_2; 1/2, -1, 1],  \\
     *     A^\mu[p_3; 1, 0, 1],
     * \f]
     * for an interaction vertex that has the expression
     * \f[
     *    \gamma^\mu_{\alpha\beta}.
     * \f]
     *
     * @note The particle meta-data such as spin and gauge representations are
     * not necessary to define the vertex but used in the UFO output.
     *
     * @sa Vertex, Charge
     */
    struct Field
    {

        /*!
         * @brief Name of the particle.
         */
        std::string name;

        /*!
         * @brief Name of the particle-s momentum that can be identified in
         * vertex expressions.
         */
        std::string momentum;

        /*!
         * @brief Spin dimension of the particle.
         *
         * @details For a particle of spin \f$ j \f$, the spin dimension
         * \f$ d \f$ simply reads
         * \f[
         *      d = 2j + 1.
         * \f]
         */
        int spinDimension;

        /*!
         * @brief Electrc charge.
         *
         * @details The electric charge in models derived fron symmetry breaking
         * of a \f$ SU(2)_L\times U(1)_Y \f$ group is not provided directly by
         * marty. Consequently, it is identified in the Lagrangian by specific
         * algorithms that can be found in electriccharge.h. The charge is a
         * Charge object that can contain a rational number for fractional
         * charges.
         *
         * @sa electriccharge.h, Charge
         */
        Charge electricCharge;

        /*!
         * @brief Color dimension of the particle.
         *
         * @details The exact representation is not specified here, only the
         * dimension. The leptons, quarks and gluons for example have charge
         * dimensions 1, 3 and 8 respectively.
         */
        int colorDimension;

        /*!
         * @brief True if the particle is conjugated (anti-particle).
         */
        bool antiParticle;

        /*!
         * @brief Indices of the field that can be identified in the vertex
         * expressions.
         */
        csl::IndexStructure indices;
    };

} // namespace mty::experimental::ufo
