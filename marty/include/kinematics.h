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
 * @file kinematics.h
 * @brief Contains the mty::Kinematics class.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include <csl.h>
#include <numeric>
#include "insertion.h"

namespace mty::wick {
    class Graph;
}

namespace mty {

    /**
     * @brief Stores insertion and momenta data and provides a simple interface
     * to manipulate it.
     */
    class Kinematics {

    public:


        /**
         * @brief Default constructor: empty process.
         */
        Kinematics() = default;

        /**
         * @brief Initializes the object only from insertions, creates the 
         * required momenta.
         *
         * @param t_insertions Insertions of the process.
         */
        explicit
        Kinematics(std::vector<mty::Insertion> const &t_insertions);

        /**
         * @brief Initializes the object from insertions and momenta.
         *
         * @details Squared momenta are determined from the momenta given as
         * parameters through the relation
         * \f$ s_{ij} = g_{\mu\nu}p_i^\mu p_j^\nu \f$.
         *
         * @param t_insertions Insertions of the process.
         * @param t_momenta    External momenta of the process. Should be of the
         * same size as \b t_insertions.
         */
        Kinematics(
                std::vector<mty::Insertion> const &t_insertions,
                std::vector<csl::Tensor>    const &t_momenta
                );

        /**
         * @brief Initializes the object from insertions and indices for 
         * momenta.
         *
         * @details Indices given as parameter determine the name of the 
         * different momenta, replacing the default \f$ p_1, p_2 ..., p_N \f$
         * that corresponds to indices \f$ {1, 2, ..., N} \f$. The range of 
         * indices can be arbitrary provided that its size corresponds to the
         * number of different insertions and that all indices are different.
         * Then squared momenta are determined from the momenta created from 
         * the indices through the relation
         * \f$ s_{ij} = g_{\mu\nu}p_i^\mu p_j^\nu \f$.
         *
         * @param t_insertions Insertions of the process.
         * @param indices      Indices to initialize momenta names.
         */
        Kinematics(
                std::vector<mty::Insertion> const &t_insertions,
                std::vector<size_t>         const &indices
                );

        /**
         * @return the size of the kinematics objects i.e. the number of 
         * insertions / momenta.
         */
        size_t size() const {
            return insertions.size();
        }

        /**
         * @brief Returns the insertion in position i.
         *
         * @param i Position of the insertion to get (starts at 0).
         *
         * @return The insertion in position \b i.
         */
        mty::Insertion const &insertion(size_t i) const {
            return insertions[i];
        }

        /**
         * @brief Returns the momentum in position i.
         *
         * @param i Position of the momentum to get (starts at 0).
         *
         * @return The momentum in position \b i.
         */
        csl::Tensor const &momentum(size_t i) const {
            return momenta[i];
        }

        csl::Expr squaredMomentum(size_t i, size_t j) const {
            return squaredMomenta[squaredMomentumIndex(i, j)];
        }

        /**
         * @return A const reference to the range of insertions.
         */
        std::vector<mty::Insertion> const &getInsertions() const {
            return insertions;
        }

        /**
         * @return A const reference to the external spin tensors.
         */
        std::vector<csl::Expr> const &getExternalSpinTensors() const {
            return externalSpinTensors;
        }

        /**
         * @return A const reference to the range of momenta.
         */
        std::vector<csl::Tensor> const &getMomenta() const {
            return momenta;
        }

        /**
         * @return Returns the momenta in order (p_1, p_2, p_3, ...)
         */
        std::vector<csl::Tensor> getOrderedMomenta() const;

        /**
         * @return A const reference to the range of squared momenta.
         */
        std::vector<csl::Expr> const &getSquaredMomenta() const {
            return squaredMomenta;
        }

        /**
         * @return #indices
         */
        std::vector<size_t> const &getIndices() const {
            return indices;
        }

        /**
         * @brief Returns the combinatorial factor corresponding to the process.
         *
         * @details This function takes into account two different factors 
         * required in the squared amplitude calculation, namely the number
         * of incoming degrees if freedom to average over them, and the 
         * combinatorial factor taking into account indistinguishable outgoing
         * particles. For \f$ N \f$ incoming particles indexed by \f$ i \f$ and
         * \f$ M \f$ groups of indistinguishable outgoing particles of 
         * respective sizes \f$ m_j \f$, this function returns a symbolic factor 
         * equal to
         * \f[
         *      F = \prod _i\dfrac{1}{d_i}\cdot \prod _j \dfrac{1}{m_j!},
         * \f]
         * with \f$ d_i \f$ the number of degrees of freedom of the incoming
         * particle $i$ taking into account spin and gauge representations 
         * dimensions.
         *
         * @note For spin 1 particles, the spin dimension is 2 if the particle 
         * is massless, 3 otherwise.
         * @note Indistinguishable particles are particles of the exact same 
         * species, with the same complex conjugation property (e.g. \f$ W^+ \f$
         * and \f$ W^- \f$ are distinguishable, \f$ Z \f$ and \f$ Z \f$ are 
         * not).
         *
         * @return The factor \f$ F \f$ calculated as the combinatorial factor
         * for the corresponding squared amplitude.
         */
        csl::Expr getDegreesOfFreedomFactor() const;

        /**
         * @brief Sets the external spin tensors from a collection of 
         * exprssions.
         *
         * @details Each term is parsed and the first mty::PolarisationField
         * object is detected and stored as external spin tensor. If no spin
         * tensor is found, CSL_UNDEF is used as default value for the tensor.
         *
         * @param terms Set of expressions, typically results of 
         * mty::QuantumField::getLSZInsertion().
         */
        void setExternalSpinTensors(std::vector<csl::Expr> const &terms);

        /**
         * @brief Returns a subset of the Kinematics object taking only some 
         * indices.
         *
         * @param pos Range of insertions indices of the subset to return.
         *
         * @return The Kinematics object with a subset of insertions and
         * (squared-)momenta containing only objects at given positions given
         * by \b pos.
         */
        Kinematics subset(std::vector<size_t> pos) const;

        /**
         * @brief Permutes elements inside the Kinematics object, keeping 
         * insertions and (squared-)momenta aligned with each other.
         *
         * @param pos Permutation. Must be of the same size as the Kinematics
         * object and contain all indices in \f$ {1, 2, ..., N} \f$.
         */
        void applyPermutation(std::vector<size_t> const &pos);

        /**
         * @brief Creates a new kinemtics object with newly created momenta for
         * the same insertions.
         *
         * @param indices Set of indices constructing the momenta for the new 
         * Kinematics object.
         *
         * @return The copy of the current Kinematics object with new momenta
         * corresponding to \b indices.
         */
        Kinematics applyIndices(
                std::vector<size_t> const &indices
                ) const;

        void sortFromIndices();

        Kinematics alignedWith(Kinematics const &other) const;

        /**
         * @brief Replaces in an expression the relevant (squared-)momenta 
         * corresponding to the replacement of one Kinematics object to another.
         * This function must receive two kinematics of the same size, and 
         * replaces in \b expr the (squared-)momenta of \b k1 by those of \b k2.
         *
         * @param expr Expression in which the replacement takes place.
         * @param k1   Left kinematics, its momenta are replaced.
         * @param k2   Right kinematics, its momenta take place in \b expr.
         */
        static void replace(
                csl::Expr        &expr,
                Kinematics const &k1,
                Kinematics const &k2
                );

        static Kinematics merge(
                Kinematics const &k1,
                Kinematics const &k2
                );

        /**
         * @brief Returns the default range of indices form 1 to N
         *
         * @param N Size of the container to return.
         *
         * @return A std::vector<size_t> equal to \f$ {1, 2, ..., N} \f$.
         */
        static std::vector<size_t> defaultIndices(size_t N) {
            std::vector<size_t> indices(N);
            std::iota(begin(indices), end(indices), 1);
            return indices;
        }

        /**
         * @brief Overload of output stream operator for Kinematics object.
         *
         * @param out Output stream.
         * @param kin Kinematics object to display.
         *
         * @return The modified stream.
         */
        friend std::ostream &operator<<(
                std::ostream     &out,
                Kinematics const &kin
                );

    private:

        /**
         * @brief Calculates and fills momenta squared from momenta contractions
         * following the expression given in the documentation of 
         * #squaredMomenta.
         *
         * @param indices Range of indices names for momenta to initialize 
         * corresponding names for \f$ s_{ij} \f$ objects.
         */
        void initMomentaSquared(std::vector<size_t> const &indices);

        /**
         * @brief Adds a CSL contraction for momenta.
         *
         * @details This function delegates to CSL the responsibility to add the
         * contraction property for \f$ p_1\cdot p_2 \f$ calling 
         * cgl::TensorParent::addSelfContraction() so that this contraction is
         * simplified when encountered in calculations and replaced by the 
         * result.
         *
         * @param p1  Left momentum in the contraction.
         * @param p2  Right momentum in the contraction.
         * @param res Result of the contraction (see #squaredMomenta).
         */
        void addContraction(
                csl::Tensor     &p1,
                csl::Tensor     &p2, 
                csl::Expr const &res
                );
        
        /**
         * @brief Sets a squared momentum from its indices and the corresponding
         * expression.
         *
         * @param i   Index of the left momentum.
         * @param j   Index of the right momentum.
         * @param res Expression result of the contraction.
         */
        void setSquaredMomentum(
                size_t           i, 
                size_t           j,
                csl::Expr const &res);

        /**
         * @brief Returns the index corresponding to a given momenta product.
         *
         * @param i Index of the left momentum.
         * @param j Index of the right momentum.
         *
         * @return The index corresponding to \f$ s_{ij} \f$ in the linear range
         * containing squared momenta.
         */
        size_t squaredMomentumIndex(size_t i, size_t j) const {
            return i * momenta.size() + j;
        }

    private:

        /**
         * @brief Set of insertions of the process.
         */
        std::vector<mty::Insertion> insertions;

        /**
         * @brief Set of external momenta of the process.
         *
         * @details There are exactly \f$N\f$ momenta for \f$N\f$ insertions.
         */
        std::vector<csl::Tensor>    momenta;

        /**
         * @brief Set of external spin tensors (mty::PolarisationField).
         *
         * @details Each element can also be CSL_UNDEF if there is no spin tensor.
         */
        std::vector<csl::Expr>      externalSpinTensors;

        /**
         * @brief Indices of momenta.
         *
         * @details Momenta are defined as \f$ p_i \f$ with indices \f$ i \f$. 
         * This member allows the Kinematics to keep this information, 
         * considering for example that a set of momenta \f$ (p_2, p_3, p_1) \f$ 
         * will have indices \f$ (2, 3, 1) \f$.
         */
        std::vector<size_t> indices;

        /**
         * @brief Set of squared external momenta for the process.
         *
         * @details For \f$N\f$ insertions, there are \f$N\f$ external momenta 
         * and \f$ N(N+1)/2 \f$ squared momenta defined by
         * \f[
         *      s_{ij} \equiv g_{\mu\nu}p_i^\mu p_j^\nu, \text{ with } i \leq j.
         * \f]
         * For on-shell particles, one has
         * \f[
         *      s_{ii} = m_i^2.
         * \f]
         * This container is linear whereas the data it represents is 
         * 2-dimensional, see squaredMomentumIndex() that provides the relation
         * between the 2D index and the physical index for this container. For 
         * now this container is redundant and contains both \f$ s_{ij} \f$ and
         * \f$ s_{ji} \f$ that are equal. This is however not an important 
         * overhead and is less error prone that storing a triangle matrix in
         * a linear container.
         */
        std::vector<csl::Expr>      squaredMomenta;
    };

} // namespace mty
