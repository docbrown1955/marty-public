/*!
 * @file lorentzcoupling.h
 *
 * @brief Contains the mty::experimental::ufo::LorentzCoupling class to store Minkowski and
 * Dirac tensors of vertices.
 */
#pragma once

#include "diracmatrix.h"
#include <marty.h>
#include <vector>

namespace mty::experimental::ufo
{

    /*!
     * @brief Contains the information about tensors in Minkowski and Dirac
     * spaces for simplified vertices.
     *
     * @details This class has two attributes. The first one is a possible chain
     * of gamma matrices (stored in DiracMatrix objects) and the second one is
     * the set of purely Minkowski tensors i.e. momenta \f$ p_i^\mu \f$ and
     * metric terms \f$ g_{\mu\nu} \f$. The Lorentz factor containing only
     * Minkowski indices in stored in a simple CSL expression that can be
     * directly converted into the final UFO format. Example of Lorentz factors
     * are
     * \f[
     *    p_1^\mu - p_2^\mu, \\
     *    g_{\mu\nu}g_{\rho\sigma}.
     * \f]
     *
     * @note The gamma-matrix chain is always a product and each term is stored
     * in one element of the chain. If there is a sum of non-trivial tensors in
     * the vertex such as \f$ P_L + P_R \f$, the expression is expanded into
     * several different vertices.
     *
     * @sa Vertex, DiracMatrix
     */
    struct LorentzCoupling
    {

        /*!
         * @brief Chain of gamma matrices. @sa DiracMatrix
         */
        std::vector<DiracMatrix> gammaMatrices;

        /*!
         * @brief Tensors in Minkowski space, momenta or metric terms.
         */
        csl::Expr lorentzFactor;
    };

} // namespace mty::experimental::ufo
