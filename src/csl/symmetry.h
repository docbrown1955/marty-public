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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 1.3

 * \brief
 */
#ifndef SYMMETRY_H_INCLUDED
#define SYMMETRY_H_INCLUDED

#include "index.h"
#include "std_vector_implementation.h"
#include <vector>

namespace csl {

/*!
 *
 * \brief Handles a std::vector of integers that represents the permutation of
 * n indices, with a possible symmetry of an IndexStructure.
 */
class Permutation {

    IMPLEMENTS_STD_VECTOR(int, permutation)

  private:
    /*!
     * \brief Order of the Permutation, i.e. the number of times one needs to
     * compose it with itself to recover the identity.
     */
    int order;

    /*!
     * \brief Sign of the permutation, i.e. its distance to the identity.
     */
    int sign;

    /*!
     * \brief Symmetry corresponding to the permutation, depending on the
     * symmetry of the underlying TensorElement.
     */
    int symmetry;

    /*!
     * \brief std::vector of integers representing the permutation. Each
     * integers between 0 and \b size - 1 are represented, vector of size \b
     * size.
     */
    std::vector<int> permutation;

  public:
    /*!
     * \brief Default constructor.
     */
    Permutation();

    /*!
     * \brief Initializes the identity permutation of \b n elements.
     *
     * \param n The number of elements of the permutation.
     */
    explicit Permutation(int n);

    /*!
     * \brief constructor that takes a vector of integers corresponding to the
     * permutation. All integers between 0 and the size of \b t_permutation-1
     * must be represented.
     *
     * \param t_permutation The complete permutation in the form of a
     * std::vector<int>.
     */
    explicit Permutation(const std::vector<int> &t_permutation);

    /*!
     * \brief Constructor that initializes the size of the Permutation to \b n,
     * and takes one cycle (that concerns possibly only few elements) as
     * initialization. For example, Permutation(5,{1,0,2}) intializes the
     * permutation to (2,0,1,3,4).
     *
     * \param n Total size of the permutation.
     * \param list Only cycle in the permutation.
     */
    Permutation(int n, const std::vector<int> &list);

    Permutation(int n, const std::vector<int> &list, int sym);

    /*!
     * \brief Constructor that initializes the size of the Permutation to \b n,
     * and takes several cycles (that concerns possibly only few elements) as
     * initialization. For example Permutation(10,{{6,7},{1,0,2}}) initializes
     * the permutation to (2,0,1,3,4,5,7,6,8,9).
     * \param n Total size of the permutation.
     * \param list Cycles in the permutation.
     */
    Permutation(int n, const std::vector<std::vector<int>> &list);

    /*!
     * \brief Copy constructor.
     *
     * \param permutation Permutation to copy.
     */
    Permutation(const Permutation &permutation) = default;

    /*!
     * \brief Destructor.
     */
    ~Permutation()
    {
    }

    /*!
     * \brief This function calculates the order of the permutation the first
     * time, or just return it if it has already been calculated.
     *
     * \return The order of the permutation.
     */
    int getOrder();

    /*!
     * \brief This function calculates the sign of the permutation the first
     * time, or just return it if it has already been calculated.
     *
     * \return The sign of the permutation.
     */
    int getSign();

    /*!
     * \return The symmetry factor the user associated with the permutation,
     * (1 if symmetric -1 if antisymmetric).
     */
    int getSymmetry() const;

    /*!
     * \return The vector of integers defining the permutation.
     */
    std::vector<int> getPermutation() const;

    /*!
     * \param t_symmetry New symmetry factor, =1 if symmetric, =-1 if
     * antisymmetric.
     */
    void setSymmetry(int t_symmetry);

    void adjustToSize(size_t newSize);

    void applyRedefinition(const std::vector<size_t> &redefinition);

    /*!
     * \brief Assignement operator. Copies \b t_permutation and returns a
     * reference to *this.
     *
     * \param t_permutation Permutation to copy.
     *
     * \return A reference to the modified object.
     */
    Permutation &operator=(const Permutation &t_permutation) = default;

    /*!
     * \brief operator*, Compose two Permutation objects and returns the result
     * in a new Permutation.
     *
     * \param t_permutation Permutation to multiply to *this.
     *
     * \return A Permutation that is the product of *this and \b t_permutation.
     */
    Permutation operator*(const Permutation &t_permutation) const;

    /*!
     * \brief operator==, tells if two Permutation objects are equal.
     *
     * \param t_permutation Permutation to compare to *this.
     *
     * \return \b True if the two Permutation objects are equal.
     * \return \b False else.
     */
    bool operator==(const Permutation &t_permutation) const;

    /*!
     * \brief operator!=, tells if two Permutation objects are not equal.
     *
     * \param t_permutation Permutation to compare to *this.
     *
     * \return \b False if the two Permutation objects are equal.
     * \return \b True else.
     */
    bool operator!=(const Permutation &t_permuation) const;

    /*!
     * \brief operator<<, displays the Permutation \b t_permutation in the
     * output \b fout.
     *
     * \param fout Output flux.
     * \param permutation Permutation to diplay.
     *
     * \return A reference to the modified flux \b fout.
     */
    friend std::ostream &operator<<(std::ostream &     fout,
                                    const Permutation &permutation);
};

/*!
 * \brief Gets all permutations (int the form of vectors of integers) of
 * n elements, n beeing the size of \b init. The vector \b init must contain
 * all integers between 0 and n-1.
 *
 * \param init Initial set of n integers.
 *
 * \return All the permutations of n elements in simple vectors of integers.
 */
std::vector<std::vector<int>> permutations(std::vector<int> init);

/*!
 * \brief Gets all permutations (int the form of Permutation objects) of
 * n elements, n beeing the size of \b init.
 *
 * \param init Initial Permutation of n elements, can be identity.
 *
 * \return All the permutations of n elements in Permutation objects.
 */
std::vector<Permutation> permutations(const Permutation &init);

/*!
 * \brief Takes a vector of Permutation objects and erase the redundant ones,
 * i.e. the permutations present several times in the vector.
 *
 * \param permutation std::vector of Permutation to reduce (modified during the
 * call of the function).
 */
void reducePermutation(std::vector<Permutation> &permutation);

/*!
 * \brief Calculates all permutations spanned by an ensemble of initial
 * Permutations \b init.
 *
 * \param init Initial permutations in a std::vector of Permutation.
 *
 * \return All permutations spanned by the initial vector \b Init in the form
 * of a another vector of Permutation objects.
 */
std::vector<Permutation> getSpan(const std::vector<Permutation> &init);

/*!
 * \brief This function adds an element in an already complete set of
 * permutations. It assumes that the std::vector \b spanned is already complete
 * by itself and modifies it by adding all the permutations spanned by itself
 * and the new permutation \b element.
 *
 * \param spanned Initial set of Permutation objects, must be already complete.
 * It is modified during the call of the function.
 * \param element Permutation to add.
 */
void getSpan(std::vector<Permutation> &spanned, const Permutation &element);

/*!
 *
 * \brief Handles the full symmetry properties of an TensorElement, i.e. a
 * vector of Permutation objects, each giving a permutation of the indices and
 * the sign of the permutation (if the permutation is symmetric or
 * anti-symmetric).
 */
class Symmetry {

    IMPLEMENTS_STD_VECTOR(Permutation, permutation)

  protected:
    /*!
     * \brief Dimension of the symmetry, i.e. number of indices of the
     * TensorElement.
     */
    int dim;

    bool mustGetSpan = true;

    /*!
     * \brief List of the permutations for which the TensorElement have a
     * symmetry or an antisymmetry property.
     */
    std::vector<Permutation> permutation;

  public:
    /*!
     * \brief Default constructor.
     */
    Symmetry();

    /*!
     * \brief Initializes the symmetry in a given dimension.
     */
    Symmetry(int t_dim);

    /*!
     * \brief Copy constructor.
     *
     * \param t_symmetry Symmetry to copy.
     */
    Symmetry(const Symmetry &t_symmetry) = default;

    /*!
     * \brief Destructor.
     */
    ~Symmetry()
    {
    }

    /*!
     * \brief The dimension of the symmetry is the number of indices on which
     * it is applied, and then also the Permutation::size of the Permutation
     * objects it encapsulates.
     *
     * \return The dimension of the Symmetry object.
     */
    int getDim() const;

    /*!
     * \return The number of Permutation objects the Symmetry handles.
     */
    size_t getNPermutation() const;

    /*!
     * \return The vector of Permutation objects managed by the Symmetry.
     */
    std::vector<Permutation> getPermutation() const;

    int getSymmetryOf(int i, int j) const;

    /*!
     * \brief Adds a new symmetry in the form of a Permutation. The user must
     * give the Permutation and the symmetry factor associated (1 if the
     * Permutation is a symmetry, -1 if it is an anti-symmetry).
     *
     * \param newPermutation New Permutation describing the symmetry.
     * \param sym Symmetry factor, +1 or -1.
     */
    void addSymmetry(const Permutation &newPermutation, int sym = 1);

    /*!
     * \brief Adds a new symmetry in the form of a Permutation. The user must
     * give the Permutation and the symmetry factor associated (1 if the
     * Permutation is a symmetry, -1 if it is an anti-symmetry).
     * \warning This function may be used only if the symmetry has already a
     * defined dimension.
     *
     * \param newPermutation New Permutation describing the symmetry.
     * \param sym Symmetry factor, +1 or -1.
     */
    void addSymmetry(const std::vector<int> &newPermutation, int sym = 1);

    /*!
     * \brief Adds a new symmetry in the form of a Permutation. The user must
     * give the Permutation and the symmetry factor associated (1 if the
     * Permutation is a symmetry, -1 if it is an anti-symmetry).
     * \warning This function may be used only if the symmetry has already a
     * defined dimension.
     *
     * \param newPermutation New Permutation describing the symmetry.
     * \param sym Symmetry factor, +1 or -1.
     */
    void addSymmetry(const std::vector<std::vector<int>> &newPermutation,
                     int                                  sym = 1);

    void adjustPermutationSize();

    void setMustGetSpan(bool t_must);

    /*!
     * \brief operator==, compares two Symmetry objects.
     *
     * \param symmetry Symmetry to compare to *this.
     *
     * \return \b True if the two symmetries are the same.
     * \return \b False else.
     */
    bool operator==(const Symmetry &symmetry) const;

    /*!
     * \brief operator!=, compares two Symmetry objects.
     *
     * \param symmetry Symmetry to compare to *this.
     *
     * \return \b False if the two symmetries are the same.
     * \return \b True else.
     */
    bool operator!=(const Symmetry &symmetry) const;

    Symmetry operator*(const Symmetry &other) const;

    /*!
     * \brief operator<<, displays the Symmetry \b symmetry in the
     * output \b fout.
     *
     * \param fout Output flux.
     * \param permutation Symmetry to diplay.
     *
     * \return A reference to the modified flux \b fout.
     */
    friend std::ostream &operator<<(std::ostream &  fout,
                                    const Symmetry &symmetry);
};

class IndexedSymmetry : public Symmetry {

  public:
    IndexedSymmetry();

    explicit IndexedSymmetry(const IndexStructure &init);

    explicit IndexedSymmetry(const Index &i1, const Index &i2);

    explicit IndexedSymmetry(const IndexStructure &init,
                             const Symmetry &      initialSym);

    void addSymmetry(const Index &i1, const Index &i2);

    void addSymmetry(const std::pair<Index, Index> &perm, int symmetry = 1);

    void addAntiSymmetry(const Index &i1, const Index &i2);

    void addAntiSymmetry(const std::pair<Index, Index> &perm);

    IndexedSymmetry operator+(const IndexedSymmetry &other) const;

    IndexedSymmetry &operator+=(const IndexedSymmetry &other);

    IndexedSymmetry operator*(const IndexedSymmetry &other) const;

    IndexedSymmetry &operator*=(const IndexedSymmetry &other);

    csl::vector_expr applySymmetry(const Expr &expr) const;

    friend std::ostream &operator<<(std::ostream &         fout,
                                    const IndexedSymmetry &sym);

  private:
    size_t
    findPos(const Index &index, size_t &maxPos, IndexStructure &newInit) const;

    std::pair<Symmetry, Symmetry>
    getCorrespondingSymmetries(IndexedSymmetry other,
                               IndexStructure &newInit) const;

  private:
    IndexStructure init;
};

} // End of namespace csl

#endif
