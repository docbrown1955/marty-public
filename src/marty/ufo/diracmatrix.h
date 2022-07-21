/*!
 * @file diracmatrix.h
 *
 * @brief Contains the mty::experimental::ufo::DiracMatrix class definition.
 */
#pragma once

#include <iostream>
#include <marty.h>

namespace mty::experimental::ufo
{

    /*!
     * @brief Represents one matrix in Dirac space \f$\Gamma _{\alpha\beta}\f$.
     *
     * @details One DiracMatrix represents a single matrix and contains its type
     * (see DiracMatrix::Type), its Minkowski indices and the two indices in
     * Dirac space. The Minkowski indices are csl::Index objects corresponding
     * to:
     *  - \f$ \mu \f$ for the \f$ \gamma_\mu \f$ tensor.
     *  - \f$ [\mu,\nu] \f$ for the \f$ \sigma_{\mu\nu} \f$ tensor.
     *  - \f$ 5 \f$ for the \f$ \gamma^5 \f$ tensor.
     *  - \f$ 6 \f$ for the \f$ P_L \f$ tensor.
     *  - \f$ 7 \f$ for the \f$ P_R \f$ tensor.
     *  - \f$ 8 \f$ for the \f$ C \f$ tensor, conjugation matrix.
     *
     * @sa Vertex, LorentzCoupling
     */
    struct DiracMatrix
    {

        /*!
         * @brief Type enumeration for the different gamma-matrices that a
         * DiracMatrix object can represent.
         */
        enum Type
        {
            Identity, /*!< The identity \f$\delta_{\alpha\beta}.\f$ */
            Gamma,    /*!< \f$\gamma^\mu\f$. */
            Gamma5,   /*!< \f$\gamma^5\f$. */
            P_L,      /*!< The left projector \f$P_L\f$. */
            P_R,      /*!< The right projector \f$P_R\f$. */
            C,        /*!< The conjugation matrix \f$C\f$. */
            Sigma     /*!< \f$\sigma^{\mu\nu}\f$. */
        };

        /*!
         * @brief Type of the Dirac matrix. @sa DiracMatrix::Type
         */
        Type type;

        /*!
         * @brief Collection of Minkowski indices for the matrix.
         *
         * @details Special tensors \f$(\gamma^5,P_L,P_R,C)\f$ have a default
         * numerical Minkowski indices, \f$(5,6,7,8)\f$ respectively.
         */
        csl::IndexStructure minkoIndices;

        /*!
         * @brief First Dirac index \f$\alpha\f$ in \f$\Gamma_{\alpha\beta}\f$.
         */
        csl::Index first;
        /*!
         * @brief Second Dirac index \f$\beta\f$ in \f$\Gamma_{\alpha\beta}\f$.
         */
        csl::Index second;
    };

    /*!
     * @brief Prints a DiracMatrix::Type.
     *
     * @param out  Output stream where the type is printed.
     * @param type Type to print.
     *
     * @return The modified stream.
     */
    inline std::ostream &operator<<(
        std::ostream &out,
        DiracMatrix::Type type)
    {
        switch (type)
        {
        case DiracMatrix::Identity:
            out << "Identity";
            break;
        case DiracMatrix::Gamma:
            out << "Gamma";
            break;
        case DiracMatrix::Gamma5:
            out << "Gamma5";
            break;
        case DiracMatrix::P_L:
            out << "P_L";
            break;
        case DiracMatrix::P_R:
            out << "P_R";
            break;
        case DiracMatrix::C:
            out << "C";
            break;
        case DiracMatrix::Sigma:
            out << "Sigma";
            break;
        default:
            out << "Unknown";
        }
        return out;
    }

} // namespace mty::experimental::ufo
