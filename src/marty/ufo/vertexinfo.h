/*!
 * @file vertexinfo.h
 *
 * @brief Contains the mty::experimental::ufo::Vertex class, main data structure for MARTY-UFO
 * vertices.
 */
#pragma once

#include "charge.h"
#include "colorcoupling.h"
#include "diracmatrix.h"
#include "field.h"
#include "lorentzcoupling.h"
#include <marty.h>
#include <string>
#include <vector>

/*!
 * @brief Contains all the MARTY-UFO package.
 */
namespace mty::experimental::ufo
{

    /*!
     * @brief Main object containing all simplified information about the
     * interaction to be simply translated into UFO format.
     *
     * @details The external fields, stored in Field objects, contain all the
     * information about particle species, conjugation, spin, representation
     * etc.
     *
     * The vertex is decomposed in four parts that must be perfectly factored.
     * These four parts are:
     * - **Coupling constants:** They are stored in a simple CSL expression as
     *   they do not have any specific structure. The expression
     *   \f$ -\frac{ig}{\sqrt{2}M_W} \f$ is an example of coupling constant.
     * - **Tensors in Dirac or Minkowski spaces:**  They are both stored in the same
     *   LorentzCoupling object. These tensors can be gamma matrices
     *   \f$ \gamma^\mu \f$ and related tensors, the Minkowski metric
     *   \f$ g^{\mu\nu} \f$ or external momenta \f$ p_i^\mu \f$ (all incoming).
     * - **Color tensors:** They are stored in a ColorCoupling object and are
     *   limited to \f$ SU(3)_C \f$ generators \f$ T^A,\,f^{ABC} \f$.
     *
     * If the 4 parts cannot be exactly factored (e.g. the 4-gluon vertex
     * that mixes color and Lorentz couplings), the Feynman rule is expanded
     * and each term is stored in a different vertex.
     *
     * @sa Field, LorentzCoupling, ColorCoupling
     */
    struct Vertex
    {

        /*! Collection of external fields for the vertex. @sa Field */
        std::vector<Field> fields;

        /*! CSL symbolic expression containing the coupling constants and
         * numerical factors of the vertex. */
        csl::Expr coupling;

        /*! Lorentzcoupling, contains Minkowski indices and gamma-matrix
         * products. @sa LorentzCoupling.
         */
        LorentzCoupling lorentz;

        /*! Colorcoupling, contains the coupling in the \f$ SU(3)_C \f$ group,
         * i.e. generators. @sa ColorCoupling
         */
        ColorCoupling color;
    };

} // namespace mty::experimental::ufo
