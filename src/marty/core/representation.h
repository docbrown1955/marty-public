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

/*! \file representation.h
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Classes handling irreducible representations of semi simple Lie
 * algebras, using the Cartan formalism: highest weight state representations.
 */
#ifndef REPRESENTATION_H_INCLUDED
#define REPRESENTATION_H_INCLUDED

#include "algebraState.h"
#include "semiSimpleAlgebra.h"

// Forward declaration of SumIrrep class
namespace mty {
class SumIrrep;
}

namespace mty {

/*!
 *
 * \brief Handles the irreducible representation of a given semi-simple
 * algebra. \details The irrep is defined by a highest weight state, and all
 * derived states (related by annihilation operators) with their
 * multiplicities. This class is mostly a containor for the set of states (and
 * their multiplicities) in the representation. Computations are done by
 * SemiSimpleAlgebra.
 */
class Irrep {

  public:
    /*!
     * \brief Default constructor. Initializes an empty irrep.
     */
    Irrep();

    /*!
     * \brief Constructor with 3 parameters.
     * \param t_algebra      A pointer to the SemiSimpleAlgebra of the Irrep.
     * \param t_rep          The set of AlgebraState in the representation in a
     * std::vector. The highest weight state should be in the first position.
     * \param multiplicities The multiplicities of each state in \b rep. The
     * multiplicity of the highest weight state (position 0) should be 1.
     */
    Irrep(const SemiSimpleAlgebra *        t_algebra,
          const std::vector<AlgebraState> &t_rep,
          const std::vector<int> &         multiplicities);

    /*!
     * \brief Destructor.
     */
    ~Irrep()
    {
    }

    /*!
     * \brief Returns the size of the Irrep, i.e. the number of different
     * states.
     * \details The size returned here does not count multiplicities. For
     * example, the size of the gluon representation if 7 and not 8. For the
     * total dimension of the representation see Irrep::getDim();
     * \return A size_t (long unsigned int), number of states in the irrep.
     */
    size_t size() const;

    /*!
     * \brief Tells if the Irrep is empty, i.e. contains no state.
     * \return \b True  if the Irrep is empty.
     * \return \b False else.
     */
    bool empty() const;

    /*!
     * \return An iterator to the beginning of \b rep, the std::vector of
     * AlgebraState.
     */
    std::vector<AlgebraState>::iterator begin();

    /*!
     * \return A const_iterator to the beginning of \b rep, the std::vector of
     * AlgebraState.
     */
    std::vector<AlgebraState>::const_iterator begin() const;

    /*!
     * \return An iterator to the end of \b rep, the std::vector of
     * AlgebraState.
     */
    std::vector<AlgebraState>::iterator end();

    /*!
     * \return A const_iterator to the end of \b rep, the std::vector of
     * AlgebraState.
     */
    std::vector<AlgebraState>::const_iterator end() const;

    /*!
     * \return The pointer to the (const) SemiSimpleAlgebra of the Irrep.
     */
    const SemiSimpleAlgebra *getAlgebra() const;

    /*!
     * \brief The dimension of the representation is the sum of multiplicities
     * of all states in it.
     * \details This functions returns the total dimension of the
     * representation , counting multiplicities. This is the physically
     * relevant number we use to define representations (triplet, octet,
     * doublet, ...). \return The dimension of the representation.
     */
    int getDim() const;

    /*!
     * \brief Returns the charge (csl::Expr, can be a fraction) of a \f$ U(1)
     * \f$ representation. \details This function should be called only for an
     * Irrep of the \b R algebra, i.e. a \f$ U(1) \f$ representation. The
     * function returns then a csl::Expr pointing to a csl::Integer (or a
     * csl::IntFraction if the denominator is not one). The numerator is stored
     * in highestWeight[0] and the denominator is stored in highestWeight[1].
     * \return The expression of the charge of the \f$ U(1) \f$ representation.
     */
    csl::Expr getCharge() const;

    /*!
     * \brief Returns the highest weight state of the representation.
     * \details The highest weight is the starting point to find all states in
     * the representation applying annihilation operators. Its multiplicity is
     * always 1. It is placed in the first position of the set of states in the
     * representation. In particular, if irrep is an Irrep, irrep[0] and
     * irrep.getHighestWeight() return the same state.
     * \return The highest weight state (an AlgebraState) of the Irrep.
     */
    AlgebraState getHighestWeight() const;

    /*!
     * \brief Returns all the states in the representation.
     * \details The highest weight state is always in first position, then come
     * all states found in the chain rules. See
     * SemiSimpleAlgebra::highestWeightRep() to have more informations about
     * the process. Note that taking the number of states in the Irrep does not
     * yield its dimension as some states may have a multiplicity different
     * than \f$ 1 \f$. To have the dimension of the representation see
     * Irrep::getDim().
     * \return The set of states in the Irrep, \b rep, in a std::vector.
     */
    std::vector<AlgebraState> getRep() const;

    /*!
     * \brief Returns the multiplicities in a std::vector of integers.
     * \details Each multiplicity may be equal to 1 or greater. The
     * multiplicity of the highesst weight state (in first position) is
     * always 1. There is a 1 to 1 correspondance between the multiplicities
     * returned by this function and the AlgebraStates returned by
     * Irrep::getRep(). Taking the sum of the multiplicities, one gets the
     * total dimension of the Irrep ( also given by Irrep::getDim()):\f[d =
     * \sum _{s}m_s,\f] \f$ m_s \f$ the multiplicity of the state \f$ s \f$
     * summed over all states in the representation. \return The set of
     * multiplicities of all the states in the rep, \b mult.
     */
    std::vector<int> getMult() const;

    /*!
     * \brief Creates and returns the conjugated rep, i.e. the rep with
     * inverted dinkin labels. \details For a reprentation of labels \f$ a_i
     * \f$, \f$i\f$ from 0 to \f$l-1\f$, the conjugated representation has
     * labels \f$ c_i = a_{l-i-1} \f$. \return The conjugated representation.
     */
    Irrep getConjugatedRep() const;

    /*!
     * \brief Returns the state in position i. Bound checks are done.
     * \param i Index of the AlgebraState to get.
     * \return A reference to the AlgebraState in position \b i of the rep.
     */
    AlgebraState &operator[](int i);

    /*!
     * \brief Returns the state in position i. Bound checks are done.
     * \param i Index of the AlgebraState to get.
     * \return A the AlgebraState in position \b i of the rep.
     */
    AlgebraState operator[](int i) const;

    /*!
     * \brief Comparison operator, compares the dimension of the two Irrep
     * in order to sort Irrep objects by their dimension.
     * \param other Irrep to compare.
     * \return \b True  if \b *this has a dimension less than \b other.
     * \return \b False else.
     */
    bool operator<(const Irrep &other) const;

    /*!
     * \brief Comparison operator, compares the dimension of the two Irrep
     * in order to sort Irrep objects by their dimension.
     * \param other Irrep to compare.
     * \return \b True  if \b other has a dimension less than \b *this.
     * \return \b False else.
     */
    bool operator>(const Irrep &other) const;

    /*!
     * \brief Comparison operator, compares the dimension of the two Irrep
     * in order to sort Irrep objects by their dimension.
     * \param other Irrep to compare.
     * \return \b False if \b *this has a dimension less than \b other.
     * \return \b True  else.
     */
    bool operator>=(const Irrep &other) const;

    /*!
     * \brief Comparison operator, compares the dimension of the two Irrep
     * in order to sort Irrep objects by their dimension.
     * \param other Irrep to compare.
     * \return \b False if \b other has a dimension less than \b *this.
     * \return \b True  else.
     */
    bool operator<=(const Irrep &other) const;

    /*!
     * \brief Compares two Irreps.
     * \details If the algebras of the two Irrep are identical but different
     * objects in memory, this function return \b false. For example
     * \f$ SU(2)_{s} \f$ and \f$ SU(2)_L \f$ for spin and weak isospin have
     * the same algebra but are different objects.
     * \param other Irrep to compare.
     * \return \b True  if the two Irrep have the same highest weight, in the
     * same algebra.
     * \return \b False else.
     */
    bool operator==(const Irrep &other) const;

    /*!
     * \brief Inverse of Irrep::operator==().
     * \param other Irrep to compare.
     * \return \b True  if the two irreps are different.
     * \return \b False else.
     */
    bool operator!=(const Irrep &other) const;

    /*!
     * \brief Implements the sum of the Irrep with another (\b other), stores
     * it in a SumIrrep and returns it. \details For two representations \f$
     * \mathcal{R}_1 \f$ and \f$ \mathcal{R}_2 \f$, the sum is simply \f$
     * \mathcal{R}_1\oplus \mathcal{R}_2\f$, stored (and sorted by increasing
     * dimension) in a SumIrrep. \return \b True  if \b other has a dimension
     * less than \b *this. \return \b False else. \param other Second operand
     * in the sum. \return The SumIrrep of ***this** and **other**.
     */
    SumIrrep operator+(const Irrep &other) const;

    /*!
     * \brief Implements the sum of the Irrep with a SumIrrep (\b other),
     * stores it in a SumIrrep and returns it. \details Creates a SumIrrep
     * similar to \b other and append to it the current Irrep *this. The new
     * Irrep is then automatically inserted and sorted regarding its dimension.
     * For example \f$(1\oplus 8\oplus 10)+8 = 1\oplus 8\oplus 8\oplus 10\f$.
     * \param other Second operand in the sum.
     * \return The SumIrrep of ***this** and **other**.
     */
    SumIrrep operator+(const SumIrrep &other) const;

    /*!
     * \brief Calculates and returns the product of ***this** and **other** in
     * the form of a sum of irreducible representations.
     * \details For more details on how the decomposition is determined from
     * the two initial Irrep objects, see SemiSimpleAlgebra::tensorProduct().
     * For example in \f$ SU(3) \f$ we have \f[ 8\times 8=1\oplus 8\oplus
     * 8\oplus 10 \oplus 10\oplus 27.\f] The two initial Irrep of dimension 8
     * give a total dimension of 64, decomposing into 5 irreducible
     * representations of total dimension \f$1+8+8+10+10+27=64\f$. \param other
     * Right operand in the product of representation. \return The product
     * decomposed in SumIrrep.
     */
    SumIrrep operator*(const Irrep &other) const;

    /*!
     * \brief Calculates and returns the product of ***this** and **other** in
     * the form of a sum of irreducible representations. **other** being
     * already a sum, the product is developped before being calculated.
     * \details See Irrep::operator*(Irrep const& other) for more details. Here
     * the product is simply developped to get a sum of products of two Irrep.
     * For example in \f$ SU(2) \f$ \f[ (1\oplus 3)\times 2 = (1\times 2)\oplus
     * (3\times 2) = 2\oplus 2\oplus 4.\f]
     * \param other Right operand in the product of representation.
     * \return The product decomposed in sum of Irrep.
     */
    SumIrrep operator*(const SumIrrep &other) const;

    /*!
     * \brief Overload of the operator<< for Irrep. Displays the highest weight
     * and the dimension.
     * \param fout Output flux.
     * \param irrep Irrep to display.
     * \return The modified flux.
     */
    friend std::ostream &operator<<(std::ostream &fout, const Irrep &irrep);

  protected:
    /*!
     * \brief Dimension of the Irrep, i.e. the sum of all AlgebraState's
     * mutiplicities.
     */
    int dim;

    /*!
     * \brief Pointer to the SemiSimpleAlgebra from which the Irrep is a
     * representation.
     */
    const SemiSimpleAlgebra *algebra;

    /*!
     * \brief Highest weight state of the representation.
     */
    AlgebraState highestWeight;

    /*!
     * \brief Set of AlgebraState in the representation, in a std::vector. The
     * highest weight state is in position 0.
     */
    std::vector<AlgebraState> rep;

    /*!
     * \brief Set of multiplicities in the the representation, to each state is
     * associated a multiplicity. The highest weight's multiplicity is one.
     */
    std::vector<int> mult;
};

/*!
 *
 * \brief Contains a sum of Irrep (irreducible representation) of a given
 * SemiSimpleAlgebra.
 * \details This class is a containor of a set of irreducible representations,
 * Irrep objects, result of the decomposition of a tensor product of different
 * Irrep. An example in \f$ SU(2)\f$: \f[2 \times 2 = 1 \oplus 3\f],
 * \f$1 \oplus 3\f$ is a SumIrrep of size 2, containing the trivial
 * representation of dimension 1, and the triplet. This class keeps its
 * different Irrep sorted by increasing dimension at all time in order to
 * respect usual conventions to display them.
 */
class SumIrrep : public std::vector<Irrep> {

  public:
    /*!
     * \brief Default constructor. Initializes an empty SumIrrep.
     */
    SumIrrep();

    /*!
     * \brief Constructor with one parameter.
     * \param t_irrep std::vector of Irrep in the sum.
     */
    explicit SumIrrep(const std::vector<Irrep> &t_irrep);

    /*!
     * \brief Constructor with one parameter.
     * \param other First (and unique for now) representation in the sum.
     */
    explicit SumIrrep(const Irrep &other);

    /*!
     * \brief Destructor.
     */
    ~SumIrrep(){};

    /*!
     * \brief Calculates and returns the total dimension of the SumIrrep, i.e.
     * the sum of the dimensions of all representations in the sum.
     * \details For example, the dimension of \f$ 1\oplus 8\oplus8 \oplus 10
     * \f$ is 27. \return The dimension of the SumIrrep.
     */
    int getDim() const;

    /*!
     * \return A pointer to the (const) SemiSimpleAlgebra of which the Irreps
     * are representations.
     */
    const SemiSimpleAlgebra *getAlgebra() const;

    /*!
     * \brief Inserts a new Irrep in the sum, sorting by dimension (lower
     * dimensions first).
     * \param other Irrep to insert.
     */
    void insertSort(const Irrep &other);

    /*!
     * \brief Implements the sum of the SumIrrep with an Irrep (\b other),
     * stores it in a SumIrrep and returns it.
     * \details A SumIrrep similar to \b *this is created, and \b other is
     * inserted in it, calling SumIrrep::insertSort().
     * \param other Second operand in the sum.
     * \return The SumIrrep of ***this** and **other**.
     */
    SumIrrep operator+(const Irrep &other) const;

    /*!
     * \brief Implements the sum of the SumIrrep with a SumIrrep (\b other),
     * stores it in a SumIrrep and returns it.
     * \details The two lists of Irrep objects are simply merged in a new
     * SumIrrep and sorted. The total dimension of the result is the sum of the
     * two operands' dimensions.
     * \param other Second operand in the sum.
     * \return The SumIrrep of ***this** and **other**.
     */
    SumIrrep operator+(const SumIrrep &other) const;

    /*!
     * \brief Calculates and returns the product of ***this** and **other** in
     * the form of a sum of irreducible representations.
     * \details The product is exanded the same way as
     * Irrep::operator*(SumIrrep const& other) to yield a direct sum of Irrep,
     * stored in a new SumIrrep that is returned.
     * \param other Right operand in the product of representation.
     * \return The product decomposed in sum of Irrep.
     */
    SumIrrep operator*(const Irrep &other) const;

    /*!
     * \brief Calculates and returns the product of ***this** and **other** in
     * the form of a sum of irreducible representations.
     * \details The product is exanded the same way as
     * Irrep::operator*(SumIrrep const& other) to yield a direct sum of Irrep,
     * stored in a new SumIrrep that is returned. For example, \f[ (2\oplus 2)
     * \times (1\oplus 3) = (2\times 1)\oplus (2\times 3)\oplus (2\times 1)
     * \oplus (2\times 3) = 1\oplus 1\oplus 2\oplus 2\oplus 4\oplus 4.\f]
     * \param other Right operand in the product of representation.
     * \return The product decomposed in sum of Irrep.
     */
    SumIrrep operator*(const SumIrrep &other) const;

    /*!
     * \brief Overload of the operator<< for SumIrrep. Displays the total
     * dimension and all dimensions of Irrep in the Sum.
     * \param fout Output flux.
     * \param irrep SumIrrep to display.
     * \return The modified flux.
     */
    friend std::ostream &operator<<(std::ostream &fout, const SumIrrep &s);

  protected:
    /*!
     * \brief Pointer to the algebra from which the Irreps in the sum are
     * irreducible representations.
     */
    const SemiSimpleAlgebra *algebra;
};

} // End of namespace mty

#endif
