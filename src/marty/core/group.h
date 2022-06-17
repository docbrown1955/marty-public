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

/*! \file group.h
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Group objects handling different possible symmetry groups. For now
 * only semi-simple groups have representations and product of representations.
 */
#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include "mrtError.h"
#include "representation.h"
#include "semiSimpleAlgebra.h"
#include "su_n_algebra.h"
#include <memory>

// Forward declaration of QuantumFieldParent
namespace mty {
class QuantumFieldParent;
class GaugedGroup;
} // namespace mty

namespace mty {

namespace group {
/*!
 * \brief Enumeration of types of semi simple Lie groups. The names
 * being very small, we use a namespace to avoid conflicts.
 */
enum class Type {

    Trivial, /*!< Trivial group {e}. */
    O,       /*!< Orthogonal group \f$ O(N) \f$. */
    SO,      /*!< Special orthogonal group \f$ SO(N) \f$. */
    U1,      /*!< Commutative unitary group \f$ U(1) \f$. */
    SU,      /*!< Special unitary group \f$ SU(N) \f$. */
    Sp,      /*!< Symplectic group \f$ Sp(N) \f$. */
    E6,      /*!< Group exponential of exceptionnal algebra \f$E_6\f$. */
    E7,      /*!< Group exponential of exceptionnal algebra \f$E_7\f$. */
    E8,      /*!< Group exponential of exceptionnal algebra \f$E_8\f$. */
    F4,      /*!< Group exponential of exceptionnal algebra \f$F_4\f$. */
    G2,      /*!< Group exponential of exceptionnal algebra \f$G_2\f$. */
    Lorentz  /*!< Lorentz group, \f$ SO^+(1,3) \f$. */
};

/*!
 * \brief Overload of operator<< for group::Type::Type. Allows to display
 * a string corresponding to the type of group instead of an integer.
 * \param fout Output flux.
 * \param obj  Type to display.
 * \return The modified flux.
 */
std::ostream &operator<<(std::ostream &fout, mty::group::Type type);

} // End of namespace group
} // End of namespace mty

namespace mty {

/*!
 *
 * \brief Abstract base class for groups.
 * \details This class is mostly composed of empty virtual functions. It only
 * handles a std::string for the name, and an integer for the dimension of the
 * group. See SemiSimpleGroup to get to group features.
 */
typedef class AbstractGroup {

  public:
    /*!
     * \brief Constructor with one optional parameter.
     * \param t_name Name of the group.
     */
    explicit AbstractGroup(const std::string &t_name = "");

    /*!
     * \brief Constructor with two parameters.
     * \param t_dim  Dimension of the group.
     * \param t_name Name (optional) of the group.
     */
    explicit AbstractGroup(int t_dim, const std::string &t_name = "");

    /*!
     * \brief Destructor.
     */
    virtual ~AbstractGroup()
    {
    }

    /*!
     * \brief Virtual function. Raises an error if called. See
     * reimplementations.
     */
    virtual SemiSimpleAlgebra *getAlgebra() const;

    /*!
     * \brief Virtual function. Raises an error if called. See
     * reimplementations.
     */
    virtual Irrep
    highestWeightRep(const std::vector<int> &highestWeight) const;

    /*!
     * \brief Pure Virtual function. See
     * reimplementations.
     */
    virtual Irrep getTrivialRep() const = 0;

    /*!
     * \brief Virtual function. Raises an error if called. See
     * reimplementations.
     */
    virtual Irrep getAdjointRep() const;

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return The type of the group.
     */
    virtual group::Type getType() const = 0;

    mty::GaugedGroup *getGaugedGroup() const
    {
        return gauged;
    }
    void setGaugedGroup(mty::GaugedGroup *t_gauged)
    {
        gauged = t_gauged;
    }

    /*!
     * \brief Returns the name of the group.
     * \return The name of the group.
     */
    std::string getName() const;

    /*!
     * \brief Returns the dimension (integer) of the group.
     * \return The dimension of the group.
     */
    int getDim() const;

    /*!
     * \brief Pure Virtual function. See
     * reimplementations.
     */
    virtual const csl::Space *getVectorSpace(const Irrep &irrep) const = 0;

    /*!
     * \brief Virtual function. Raises an error if called. See
     * reimplementations.
     */
    virtual AlgebraState getHighestWeight(const csl::Space *t_space) const;

    /*!
     * \brief Pure Virtual function. See
     * reimplementations.
     */
    virtual std::vector<const csl::Space *> getAllVectorSpace() const = 0;

    /*!
     * \brief Pure Virtual function. See
     * reimplementations.
     */
    virtual csl::Index getCorrespondingIndex(const std::string &index_name,
                                             const QuantumFieldParent &parent)
        = 0;

  protected:
    /*!
     * \brief Dimension of the group.
     */
    int dim;

    /*!
     * \brief Name of the group.
     */
    std::string name;

    mty::GaugedGroup *gauged;

} Group;

/*!
 * \brief Base class for all Semi simple groups, i.e. groups that has a
 * SemiSimpleAlgebra.
 * \details This class handles the different vector spaces for
 * different representations. Each time a new representation is asked, a
 * csl::Space is created. Then, a same representation in the same group will
 * live in the same csl::Space. All computations of representations and
 * products use the features of SemiSimpleAlgebra.
 * \note Two identical representations of two instances of SemiSimpleGroup
 * (with the same algebra) will live in different vector spaces. For example,
 * spin and weak-isospin will not mix, the two csl::Space will be totally
 * independant.
 */
class SemiSimpleGroup : public AbstractGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param name Name of the group.
     */
    explicit SemiSimpleGroup(const std::string &name);

    /*!
     * \brief Constructor with two parameters.
     * \param t_algebra algebra corresponding to the group.
     * \param t_name    name of the group.
     */
    SemiSimpleGroup(const SemiSimpleAlgebra &t_algebra,
                    const std::string       &t_name);

    /*!
     * \brief Destructor.
     */
    ~SemiSimpleGroup() override;

    /*!
     * \brief Returns a pointer to the algebra of the group.
     * \return A raw pointer to the algebra of the group.
     */
    SemiSimpleAlgebra *getAlgebra() const override;

    /*!
     * \brief Replaces the algebra owned by the group.
     * \details \b t_algebra may be an r-value. This function simply creates
     * (allocated memory with a unique_ptr) a new algebra identical to
     * \b t_algebra.
     * \note This function does not verify that the algebra corresponds indeed
     * to the group. This function should not be called by the user as once a
     * group has been created, in particular if representations have been
     * defined, there is no reason for the algebra to change.
     * \param t_algebra New algebra of the group.
     */
    void setAlgebra(const SemiSimpleAlgebra &t_algebra);

    /*!
     * \brief Returns an Irrep of highest weight \b highestWeight.
     * \details The group uses the function
     * SemiSimpleAlgebra::highestWeightRep() of its algebra to create the
     * representation and returns it.
     * \param highestWeight Highest weight state of the representation.
     * \return The Irrep of highest weight \b highestWeight (see
     * SemiSimpleAlgebra::highestWeightRep()).
     */
    Irrep
    highestWeightRep(const std::vector<int> &highestWeight) const override;

    /*!
     * \brief Returns the expression (symbolic, rational number) of the
     * quadratic Casimir operator for a representation.
     * \details See SemiSimpleAlgebra::getQuadraticCasimir().
     * \param irrep Highest weight state of the representation from which we
     * want the Casimir.
     * \return The symbolic expression of the Casimir, a rational number.
     */

    csl::Expr getQuadraticCasimir(const AlgebraState &highest) const;

    /*!
     * \brief Returns the trivial (dimension 1) representation of the group.
     * \return The trivial Irrep of the group (see
     * SemiSimpleAlgebra::getTrivialRep()).
     */
    Irrep getTrivialRep() const override;

    /*!
     * \brief Computes the product of two representations and returns its
     * decomposition in a sum of irreducible representations (SumIrrep). See
     * SemiSimpleAlgebra::tensorProduct().
     * \param rep1 First representation in the product.
     * \param rep2 Second representation in the product.
     * \return A SumIrrep containing decomposition of the product in a direct
     * sum of irreducible representations.
     */
    SumIrrep tensorProduct(const Irrep &rep1, const Irrep &rep2) const;

    /*!
     * \brief Computes and the product of two representations and returns its
     * decomposition in a sum of irreducible representations. See
     * SemiSimpleAlgebra::tensorProduct().
     * \details Instead of taking representations, this function takes directly
     * the dynkin labels of their highest weight state. It first call
     * SemiSimpleAlgebra::highestWeightRep() to instantiate representations
     * from their highest weight, and then computes the tensor product. \param
     * rep1 First representation in the product, caracterized by its dinkin
     * labels in a std::vector of integers. \param rep2 Second representation
     * in the product, caracterized by its dinkin labels in a std::vector of
     * integers. \return A SumIrrep containing decomposition of the product in
     * a direct sum of irreducible representations.
     */
    SumIrrep tensorProduct(const std::initializer_list<int> &rep1,
                           const std::initializer_list<int> &rep2) const;

    /*!
     * \brief Returns the vector space (csl::Space) associated with a certain
     * irreducible representation of the group.
     * \details If the vector space of a representation identical to \b irrep
     * (same highest weight) is found, it is returned. Else, a new vector space
     * is created (csl::Space) and returned. The spaces created in this
     * function are specialized in color::ColorSpace in order to implement
     * trace properties in simplifications. See documentation of
     * color::ColorSpace. \note In this function only the highest weights of
     * the irreps are compared. This means that a representation of \f$ F_4 \f$
     * may be successfully compared to a representation of \f$A_4\f$ because
     * they have the same number of labels. \param irrep Representation from
     * which we want the vector space. \return A pointer to the
     * color::ColorSpace of the representation (creates the space if it had not
     * already been created).
     */
    const csl::Space *getVectorSpace(const Irrep &irrep) const override;

    /*!
     * \brief Searches and returns the highest weight of the representation
     * living in \b t_space (csl::Space).
     * \details If a representation living in \b t_space is found, its highest
     * weight is returned. Else an error is raised.
     * \param t_space Vector space in which the representation lives.
     * \return The highest weight of the representation if found.
     */
    AlgebraState getHighestWeight(const csl::Space *t_space) const override;

    /*!
     * \brief Returns all vector spaces (csl::Space) created by the group.
     * \details All vector spaces are put in a std::vector that is returned.
     * \return All vector spaces (const) created since the creation of the
     * SemiSimpleGroup.
     */
    std::vector<const csl::Space *> getAllVectorSpace() const override;

    /*!
     * \brief Returns a csl::Index of the vector space corresponding
     * to a given mty::QuantumFieldParent's representation of the group.
     * \details The QuantumFieldParent is important to know which
     * representation is wanted. For example in QCD, if 'g' is the gluon and
     * 'q' a quark that are respectively in octet and triplet representations
     * of a SemiSimpleGroup 'SU3_c', \code SU3_c.getCorrespondingIndex("A", g);
     * \endcode
     * will return an index in the adjoint representation (dimension 8),
     * whereas \code SU3_c.getCorrespondingIndex("a", q); \endcode will return
     * an index in the fundamental representation (dimension 3). \param
     * index_name Name of the csl::Index. \param parent     QuantumFieldParent
     * in which we search a representation of the group. \return An index of
     * name \b index_name living in the space of \b parent's representation of
     * the group.
     */
    csl::Index
    getCorrespondingIndex(const std::string        &index_name,
                          const QuantumFieldParent &parent) override;

  protected:
    /*!
     * \brief \b Owner std::unique_ptr to the SemiSimpleAlgebra associated with
     * the group.
     */
    std::unique_ptr<SemiSimpleAlgebra> algebra;

    /*!
     * \brief Association of AlgebraState (highest weights of irreps) and
     * vector spaces. To each irrep (i.e. each highest weight) is associated
     * a different and unique vector space (csl::Space). See getVectorSpace().
     */
    mutable std::vector<std::pair<AlgebraState, const csl::Space *>>
        vectorSpaces;

    /*!
     * \brief Association of AlgebraState (highest weights of irreps) and
     * quadratic Casimirs. To each irrep (i.e. each highest weight) is
     * associated a quadratic casimir operator. It is computed the first time
     * asked, and stored.
     */
    mutable std::map<AlgebraState, csl::Expr> quadraticCasimir;
};

} // End of namespace mty

/*!
 * \brief Namespace to avoid name conflicts for groups that can have
 * very simple names. See algebra::Type for the same reason.
 */
namespace mty::group {

/*!
 *
 * \brief Abelian unitary group. mty::algebra::R is the associated
 * SemiSimpleAlgebra.
 */
class U1 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param name Name of the group.
     */
    explicit U1(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::U1.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ U(1) \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The representation of charge \f$ 0 \f$, \f$ U(1) \f$ being
     * abelian.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Special unitary group. The algebra of \f$ SU(N) \f$ is \f$ A_{N-1}
 * \f$ , see mty::algebra::A.
 */
class SU : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with two parameters.
     * \param t_name Name of the group.
     * \param t_dim  Dimension (optional, default = 2) of the group.
     */
    explicit SU(std::string const &t_name, int t_dim = 2);

    /*!
     * \brief Constructor with two parameters.
     * \param t_dim  Dimension (optional, default = 2) of the group.
     */
    explicit SU(int t_dim = 2);

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::SU.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ SU(N) \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (1,0,...,0,1) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Special orthogonal group. The algebra of \f$ SO(N) \f$ is
 * \f$ B_{(N-1)/2} \f$ if \f$ N \f$ is odd, \f$ D_{N/2} \f$ else. See
 * mty::algebra::B and mty::algebra::D.
 */
class SO : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with two parameters.
     * \param t_name Name of the group.
     * \param t_dim  Dimension (optional, default = 3) of the group.
     */
    explicit SO(std::string const &t_name, int t_dim = 3);

    /*!
     * \brief Constructor with one parameter.
     * \param t_dim Dimension (optional, default = 3) of the group.
     */
    explicit SO(int t_dim = 3);

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::SO.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ SO(N) \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (0,1,...,0,0) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Simplectic group. The algebra of \f$ Sp(2N) \f$ is \f$ C_N \f$. See
 * mty::algebra::C.
 */
class Sp : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with two parameters.
     * \param t_name Name of the group.
     * \param t_dim  Dimension (optional, default = 2) of the group.
     */
    explicit Sp(std::string const &t_name, int t_dim = 2);

    /*!
     * \brief Constructor with one parameter.
     * \param t_dim Dimension (optional, default = 2) of the group.
     */
    explicit Sp(int t_dim = 2);

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::Sp.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ Sp(N) \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (2,0,...,0,0) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Exceptional group of algebra \f$ E_6 \f$. See mty::algebra::E6.
 */
class E6 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with two parameters.
     * \param t_name Name of the group.
     */
    explicit E6(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::E6.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ E_6 \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (0,0,0,0,0,1) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Exceptional group of algebra \f$ E_7 \f$. See mty::algebra::E7.
 */
class E7 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param t_name Name of the group.
     */
    explicit E7(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::E7.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ E_7 \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (1,0,0,0,0,0,0) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Exceptional group of algebra \f$ E_8 \f$. See mty::algebra::E8.
 */
class E8 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param t_name Name of the group.
     */
    explicit E8(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::E8.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ E_8 \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (1,0,0,0,0,0,0,0) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Exceptional group of algebra \f$ F_4 \f$. See mty::algebra::F4.
 */
class F4 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param t_name Name of the group.
     */
    explicit F4(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::F4.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ F_4 \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (1,0,0,0) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Exceptional group of algebra \f$ G_2 \f$. See mty::algebra::G2.
 */
class G2 : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Constructor with one parameter.
     * \param t_name Name of the group.
     */
    explicit G2(std::string const &t_name = "");

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::G2.
     */
    mty::group::Type getType() const override;

    /*!
     * \brief Returns the adjoint representation of \f$ G_2 \f$, i.e. the
     * representation of the gauge boson.
     * \details The adjoint representation is needed automatically in general
     * in order to define gauge bosons without the help of the user. Adjoint
     * representations expressed in terms of dynkin labels (highest weight
     * state) can be found in the literature, see the manual for references.
     * \return The Irrep of heighest weight \f$ (0,1) \f$.
     */
    mty::Irrep getAdjointRep() const override;
};

/*!
 *
 * \brief Lorentz group. Algebra of Lorentz: \f$ D_2=A_1\oplus A_1 \f$.
 */
class Lorentz : public mty::SemiSimpleGroup {

  public:
    /*!
     * \brief Default Constructor.
     */
    Lorentz();

    /*!
     * \brief Destructor.
     */
    ~Lorentz()
    {
    }

    /*!
     * \brief Returns the type of the group. See mty::group::Type.
     * \return group::Type::Lorentz.
     */
    mty::group::Type getType() const override;
};

} // End of namespace mty::group

namespace mty {

/*!
 * \brief Dynamically allocates a new group of type \b type and returns a
 * pointer to it (in a std::unique_ptr).
 * \param type Type of the group.
 * \param name Name of the group.
 * \param dim  Dimension of the group (optional for exceptional algebras).
 * \return A pointer to the dynamically allocated Group.
 */
std::unique_ptr<mty::SemiSimpleGroup>
createGroup(group::Type type, std::string const &name, int dim = -1);

/*!
 * \brief Dynamically allocates a new group of type \b type and returns a
 * pointer to it (in a std::unique_ptr).
 * \param type Type of the group.
 * \param dim  Dimension of the group (optional for exceptional algebras).
 * \return A pointer to the dynamically allocated Group.
 */
std::unique_ptr<mty::SemiSimpleGroup> createGroup(group::Type type,
                                                  int         dim = -1);

/*!
 * \brief Overload of operator<< for AbstractGroup. Displays the type of the
 * group and its dimension between brackets.
 * \param fout Output flux.
 * \param obj  Group to display.
 * \return The modified flux.
 */
std::ostream &operator<<(std::ostream &fout, const AbstractGroup &obj);

/*!
 * \brief Converts a string to a group type. Allows to read a group::Type in
 * a file.
 * \param name Name of the type.
 * \return The corresponding group::Type.
 */
group::Type stringToGroupType(std::string const &name);

/*!
 * \brief Global variable. Lorentz group in the form of a raw pointer to an
 * AbstractGroup. This variable should be used whereever the Lorentz group is
 * used in the program.
 */
inline mty::group::Lorentz *Lorentz = new mty::group::Lorentz();

} // End of namespace mty

#endif
