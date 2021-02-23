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

/*! \file algebraState.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Class AlgebraState that represents the set of dinkin labels for a 
 * state in a semi simple Lie Algebra.
 */
#ifndef ALGEBRA_STATE_H_INCLUDED
#define ALGEBRA_STATE_H_INCLUDED

#include <vector>
#include <string>

namespace mty {

/*!
 * 
 * \brief Class inherited from std::vector\<int\>, dynkin labels for a state
 * of a semi-simple algebra (see documentation od SemiSimpleAlgebra).
 */
class AlgebraState: public std::vector<int>{

    public:

    /*!
     * \brief Default constructor. Initializes an empty vector of labels.
     */
    AlgebraState()
        :std::vector<int>(){}

    /*!
     * \brief Constructor with one parameter. 
     * \param vec std::vector of int containing the dinkin labels of the state.
     */
    explicit
    AlgebraState(const std::vector<int>& vec)
        :std::vector<int>(vec){}

    /*!
     * \brief Applies shifts on labels coming from annihilation operators. 
     * Creates a new state with labels "labels - shift".
     * \param shifts std::vector of int with a size corresponding to the number
     * of labels.
     * \return The modified state with shifts applied.
     */
    AlgebraState applyAnnihilator(const std::vector<int>& shifts) const;

    /*!
     * \brief Calculates the eigenvalue of the state with respect to the root
     * \b root, considering \b l simple roots (then \b l dinkin labels).
     * \param root       Dinkin labels of the root (size \b l).
     * \param normRoots  Norm of the \b l simple roots.
     * \param normLambda Norm of the root \f$ \lambda \f$.
     * \return The eigenvalue of the state with respect to \b root.
     */
    int eigenValue(const std::vector<int>& root,
                   const std::vector<int>& normRoots,
                   int                     normLambda) const;

    /*!
     * \brief Converts the state into a simple std::vector of int corresponding
     * to the \b l dinkin labels of the state.
     * \return The dinkin labels of the state in a std::vector\<int\>.
     */
    std::vector<int> getLabels() const;

    inline bool operator<(AlgebraState const &other) const {
        return static_cast<std::vector<int>>(*this) < other;
    }

    /*!
     * \brief Overload of the operator<< for AlgebraState, displays the labels
     * in the output \b fout.
     * \param fout  Output flux.
     * \param state AlgebraState to display.
     * \return The modified flux.
     */
    friend
    std::ostream& operator<<(std::ostream&       fout,
                             const AlgebraState& state);
};

} // End of namespace mty

#endif
