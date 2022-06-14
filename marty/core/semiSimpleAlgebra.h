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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Semi simple Lie algebras: calculations of highest weight
 * representations, products and sums of representations.
 * \details Handles all semi-simple Lie algebras, including the exceptionnal 
 * ones (\f$ E_6 \f$, \f$ E_7 \f$, \f$ E_8 \f$, \f$ F_4 \f$, \f$ G_2 \f$). For
 * more technical details see the manual that explains more deeply some concepts
 * and gives references.
 */
#ifndef SEMISIMPLEALGEBRA_H_INCLUDED
#define SEMISIMPLEALGEBRA_H_INCLUDED

#include <vector>
#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <string>
#include "../../csl/csl.h"
#include "algebraState.h"

//Forward declaration of Irrep and SumIrrep
namespace mty {
class Irrep;
class SumIrrep;
}

/*!
 * \brief \brief Enumeration of types of semi simple Lie algebras. The names
 * being very small, we use a namespace to avoid conflicts.
 */
namespace mty::algebra {

    /*!
     * \brief Different types of semi-simple Lie algebras.
     */
    enum class Type {
        R, /*!< Algebra of the \f$U(1)\f$ group. */
        A, /*!< Algebra \f$ A_l \f$ of \f$SU(l+1)\f$ groups. */
        B, /*!< Algebra \f$ B_l \f$ of \f$SO(2l+1)\f$ groups. */
        C, /*!< Algebra \f$ C_l \f$ of \f$Sp(2l)\f$ groups. */
        D, /*!< Algebra \f$ D_l \f$ of \f$SO(2l)\f$ groups. */
        E6, /*!< Exceptionnal semi-simple algebra \f$E_6\f$. */
        E7, /*!< Exceptionnal semi-simple algebra \f$E_7\f$. */
        E8, /*!< Exceptionnal semi-simple algebra \f$E_8\f$. */
        F4, /*!< Exceptionnal semi-simple algebra \f$F_4\f$. */
        G2, /*!< Exceptionnal semi-simple algebra \f$G_2\f$. */
    };

    /*!
     * \brief Overload of operator<< for algebra::Type. Allows to display
     * a string corresponding to the type of algebra instead of an integer.
     * \param fout Output flux.
     * \param obj  Type to display.
     * \return The modified flux.
     */
    std::ostream& operator<<(std::ostream& fout, mty::algebra::Type obj);
}

namespace mty {

int getCoxeterNumber(
        algebra::Type type,
        int           l = 0
        );

struct IndexData {
    AlgebraState state;
    size_t p;
    csl::Expr index;
};

/*!
 * 
 * \brief Abstract base class for all semi-simple Lie algebras. 
 * \details This class implements almost all calculations (getting 
 * representation from highest weight, product of representations...) except the
 * initialization of the cartan matrix and the norms of simple roots, that
 * is overriden in derived classes, specializations of semi simple Lie algebras.
 */
class SemiSimpleAlgebra{

    public:

    /*!
     * \brief Constructor with one parameter.
     * \param t_l Number of simple roots, or rank, of the algebra.
     */
    explicit
    SemiSimpleAlgebra(int t_l);

    /*!
     * \brief Destructor.
     */
    virtual
    ~SemiSimpleAlgebra(){};

    /*!
     * \brief Returns the expression (symbolic, rational number) of the
     * quadratic Casimir operator for a representation.
     * \details The AlgebraState \b irrep must be the highest-weight state of 
     * the representation from which we want to compute the Casimir. The 
     * quadratic Casimir is defined from the generators of the representation 
     * \f$ T^A(\mathcal{R}) \f$ by
     * \f[
     *     \mbox{Tr}(T(\mathcal{R})^AT(\mathcal{R})^B) \equiv C_2(\mathcal{R})
     *     \delta ^{AB}. 
     * \f]
     * Its expression in terms of the highest weight is 
     * \f[
     *     C_2 = \sum _{i, j}\mathcal{N}_i\cdot (a_i+2)G_{ij}a_j,
     * \f]
     * with \f$ a_i \f$ dynkin labels of the highest weight state,
     * \f$ G_{ij} \f$ the inverse Cartan matrix, and \f$ \mathcal{N}_i \f$ a 
     * normalization factor coming from the absolute norm of the simple root 
     * \f$ i \f$.
     * \param irrep Highest weight state of the representation from which we 
     * want the Casimir.
     * \return The symbolic expression of the Casimir, a rational number.
     */
    csl::Expr getQuadraticCasimir(const AlgebraState& irrep) const;

    virtual csl::Expr getIndex(
            const Irrep& irrep,
            size_t       n) const;

    /*!
     * \brief Calculates and returns the highest weight representation. 
     * Determines all states with their multiplicities and returns the 
     * corresponding Irrep object. Overriden in R algebra.
     * \param highestWeight AlgebraState highest weight of the representation.
     * \return The Irrep of highest weight \b highestWeight.
     */
    virtual
    Irrep highestWeightRep(const AlgebraState& highestWeight, bool mult = true) const;

    /*!
     * \brief Calculates and returns the highest weight representation. 
     * Determines all states with their multiplicities and returns the 
     * corresponding Irrep object. Overriden in R algebra.
     * \param highestWeight std::vector of integers, labels of the 
     * representation's highest weight.
     * \return The Irrep of highest weight \b highestWeight.
     */
    virtual
    Irrep highestWeightRep(const std::vector<int>& highestWeight, bool mult = true) const;

    /*!
     * \brief Returns the trivial representation of the algebra. Overriden in 
     * R algebra.
     * \return An Irrep, trivial representation (dimension 1).
     */
    virtual
    Irrep getTrivialRep() const;

    virtual 
    Irrep getDefiningRep() const;

    /*!
     * \brief Calculates and returns the product of two Irrep in the algebra.
     * The result is decomposed in a sum of irreducible representations, stored 
     * in a SumIrrep. Overriden in the R algebra.
     * \param A First representation in the product.
     * \param B Second representation in the product.
     * \return A SumIrrep, sum of irreducible representations result of the 
     * product.
     */
    virtual
    SumIrrep tensorProduct(const Irrep& A,
                           const Irrep& B,
                           bool mult = true) const; 
    virtual
    SumIrrep tensorProduct(const SumIrrep& A,
                           const Irrep& B,
                           bool mult = true) const; 
    virtual
    SumIrrep tensorProduct(const Irrep& A,
                           const SumIrrep& B,
                           bool mult = true) const; 
    SumIrrep tensorProduct(const SumIrrep& A,
                           const SumIrrep& B,
                           bool mult = true) const; 

    /*!
     * \brief Sorts by modifing input paramaters a set of AlgebraState with 
     * their multiplicities. One state is considered "bigger" than another if 
     * the sum of its dinkin labels is bigger. Overriden in the R algebra.
     * \param rep  std::vector of AlgebraState to sort, modified during the run.
     * \param mult multiplicities of the states, sorted at the same time to keep
     * the one to one correspondance between states and multiplicities.
     */
    virtual
    void sortRep(std::vector<AlgebraState>& rep,
                 std::vector<int>&          mult) const;

    /*!
     * \brief Returns the order of the algebra, i.e. the number of simple roots
     * **l**.
     * \return **l**
     */
    int getOrderL() const;

    /*!
     * \brief Pure virtual function overriden in derived classes.
     * \return The type of the algebra, see the enum algebra::Type.
     */
    virtual
    algebra::Type getType() const = 0;

    protected:

    /*!
     * \brief Initialization function called in derived classes to initialize 
     * the cartan matrix, the Weyl group and the norms of simple roots.
     */
    void init();

    /*!
     * \brief Pure virtual function overriden in derived classes. Initializes
     * the Cartan matrix specifically to the given semi-simple Lie algebra.
     */
    virtual
    void setCartanMatrix() = 0;

    /*!
     * \brief Pure virtual function overriden in derived classes. Initializes
     * norms of simple roots specifically to the given semi-simple Lie algebra.
     */
    virtual
    void setSquaredNorm() = 0;

    virtual csl::Expr getEta() const {
        return CSL_2;
    }

    private:

    /*!
     * \brief Checks if a given root is a positive root (i.e. only positive
     * dinkin labels).
     * \param root Root from which we test the positivity.
     * \return \b True  if the root is positive.
     * \return \b False else.
     */
    static bool isPositiveRoot(const std::vector<int>& root);

    /*!
     * \brief Checks if a given state is a positive state (i.e. only positive
     * dinkin labels).
     * \param state State from which we test the positivity.
     * \return \b True  if the state is positive.
     * \return \b False else.
     */
    static bool isPositiveWeight(const AlgebraState& state);

    /*!
     * \brief Adds the dinkin labels of two states and returns the resulting
     * state.
     * \param A First AlgebraState in the sum.
     * \param B Second AlgebraState in the sum.
     * \return The sum of \b A and \b B.
     */
    static AlgebraState addWeights(const AlgebraState& A,
                                   const AlgebraState& B);

    /*!
     * \brief Checks if a state is on a wall of the root lattice, i.e. if 
     * one of the coefficients if the simple roots basis (one of the dinkin
     * labels) is zero.
     * \param state AlgebraState.
     * \return \b True  if \b state is on a wall.
     * \return \b False else.
     */
    bool isOnWall(const AlgebraState& state) const;

    /*!
     * \brief Reflects a given state in the dominant Weyl chamber.
     * \details The dominant Weyl chamber is the space where all states are 
     * positive states (see isPositiveWeight()). This function returns the
     * (only) state in the dominant Weyl chamber that is equivalent to \b state
     * to a Weyl reflection, i.e. to the application of an element of the Weyl 
     * group. If the state is positive, it is itself, else the function operates
     * a series of Weyl reflections until it finds the corresponding positive
     * state. The sign of the Transformation is one if the number of reflections
     * linking the two states is even, -1 else. The sign is stored in \b sign
     * during the run of the function.
     * \param state Initial AlgebraState.
     * \param sign  Integer modified during the run, stores the sign of the 
     * Transformation.
     * \return A new AlgebraState, reflection of \b state in the dominant Weyl
     * chamber.
     */
    AlgebraState toDominantWeylChamber(const AlgebraState& state,
                                       int&                sign) const;

    /*!
     * \brief Reflects a root with respect to one simple root. 
     * \param simpleRoot, an integer specifying the number of the simple root.
     * \param root Root to reflect, a std::vector of integers labels of the 
     * root.
     * \return The reflected root, also a std::vector of integers.
     */
    std::vector<int> weylReflection(int                     simpleRoot,
                                    const std::vector<int>& root) const;

    /*!
     * \brief Reflects a state with respect to one simple root. 
     * \param simpleRoot, an integer specifying the number of the simple root.
     * \param state AlgebraState to reflect.
     * \return The reflected state, also an AlgebraState.
     */
    AlgebraState weylReflection(int                 simpleRoot,
                                const AlgebraState& state) const;

    /*!
     * \brief Reflects a simple root with respect to another. 
     * \param reflector Simple root (integer) wrt which the reflexion is done.
     * \param reflected Simple root (integer) reflected.
     * \return The reflected root, in the form of a std::vector of integers.
     */
    std::vector<int> weylReflection(int simpleRoot1,
                                    int simpleRoot2) const;

    /*!
     * \brief Inverts the Cartan matrix using csl's matrix inversion and
     * stores it in the member \\b inverseCartan.
     * \note This function actually initializes also other csl csl::Expr member
     * variables.
     */
    void invertCartanMatrix();

    /*!
     * \brief Generates the Weyl group of reflection from the simple roots.
     * \details The weyl group is stored by keeping all positive roots in memory
     * in the member \b positiveRoots. Any weyl reflection (composition of weyl
     * reflections for simple roots) is the weyl reflection wrt one single 
     * positive root. Having all positive roots is then equivalent to have all
     * weyl reflections.
     */
    void generateWeylGroup();

    /*!
     * \brief Converts a state in co-root space (AlgebraState with dinkin
     * labels) into an expression (csl csl::Expr), Vector of coefficients in
     * root space.
     * \details This function actually converts the std::vector of integers
     * into a csl::Vector of csl::Integer, and apply the inverse cartan matrix 
     * in order to go from co-root space to root space.
     * \param state AlgebraState to convert.
     * \return A csl::Vector expression of the root space-equivalent of \b state.
     */
    csl::Expr convertState(const AlgebraState& state) const;

    /*!
     * \brief Compares two AlgebraState.
     * \details One AlgebraState is considered greater than another if the sum
     * of its labels is greater. 
     * \param A First AlgebraState to compare.
     * \param B Second AlgebraState to compare.
     * \return \b True  if A is greater than or equal to B.
     * \return \b False else.
     */
    bool comparatorHighest(const AlgebraState& A,
                           const AlgebraState& B) const;

    /*!
     * \brief Computes the scalar dot of two roots in root-space.
     * \details The scalar dot depends on the Cartan matrix and norms of simple
     * roots. \b rootA and \b rootB must be csl::Vector of csl::Integers, 
     * coordinates of the roots in root space (not in co-root space i.e. dinkin
     * labels).
     * \param rootA First root in the scalar dot.
     * \param rootB Second root in the scalar dot.
     * \return The geometrical scalar dot between \b rootA and \b rootB.
     */
    csl::Expr computeScalarDot(const csl::Expr& rootA,
                          const csl::Expr& rootB) const;

    /*!
     * \brief Computes the squared norm of a root, see computeScalarDot().
     * \param root Root in the form of a csl::Vector of csl::Integer, labels in
     * root space (not co-root space i.e. dinkin labels).
     * \return The squared norm of \b root in a csl Expr.
     */
    csl::Expr computeSquaredNorm(const csl::Expr& root) const;

    /*!
     * \brief Applies the annihilation operator of a simple root on an 
     * AlgebraState and returns the resulting AlgebraState.
     * \param annihilator Integer number of the simple root.
     * \param state       State on which the annihilation operator is applied.
     * \return The resulting AlgebraState.
     */
    AlgebraState applyAnnihilationOperator(int                 annihilator,
                                           const AlgebraState& state) const;

    /*!
     * \brief Applies the annihilation operator of a root on an 
     * AlgebraState and returns the resulting AlgebraState.
     * \param root  Root from which we take the annihilation operator.
     * \param state State on which the annihilation operator is applied.
     * \return The resulting AlgebraState.
     */
    AlgebraState applyAnnihilationOperator(const std::vector<int>& root,
                                           const AlgebraState&     state) const;

    /*!
     * \brief Applies the creation operator of a simple root on an 
     * AlgebraState and returns the resulting AlgebraState.
     * \param creator Integer number of the simple root.
     * \param state   State on which the creation operator is applied.
     * \return The resulting AlgebraState.
     */
    AlgebraState applyCreationOperator(int                 creator,
                                       const AlgebraState& state) const;

    /*!
     * \brief Applies the creation operator of a root on an 
     * AlgebraState and returns the resulting AlgebraState.
     * \param root  Root from which we take the creation operator.
     * \param state State on which the creation operator is applied.
     * \return The resulting AlgebraState.
     */
    AlgebraState applyCreationOperator(const std::vector<int>& root,
                                       const AlgebraState&     state) const;

    /*!
     * \brief Calculates the root chain starting from \b states and applying
     * annihilation operator of the simple root number \n direction.
     * \details If the eigenvalue of \b state wrt the simple root number \b
     * direction is E, we apply the annihilation until we get a state of 
     * eigenvalue -E. We have then set of states of eigenvalues (for a sl(2,C)
     * module): { E, E-2, E-4, ..., -E }.
     * \param state     Initial state (highest weight) of the chain.
     * \param direction Number of the simple root applied in chain.
     * \return The set of generated AlgebraState in the chain in a std::vector.
     */
    std::vector<AlgebraState> getSingleChain(
            const AlgebraState& state,
            int                 direction) const;

    void getSingleChainExperimental(
            const AlgebraState& state,
            std::vector<AlgebraState> &states,
            std::vector<std::vector<bool>> &directionExplored,
            int                 direction) const;

    /*!
     * \brief Computes the complete root chain (with all simple roots) starting
     * from a highest weight \b highestWeightState.
     * \details Some directions (simple roots) are not computed because they 
     * already have been explored. This function is recursive and calls itself
     * again for all new states. If a state has been found along the direction 
     * d, then this direction will not be explored again getting the root chains
     * recursively for this state. The new states are added in the std::vector
     * \b states (copied at the beginning of the function), and this std::vector
     * is returned.
     * \param highestWeightState Initial state of the root chains.
     * \param states             Current set of stated found in the irrep.
     * \param directions         Set of already explored directions (along
     * simple roots) for the state \b highestWeightState, that we do not compute
     * again.
     * \return A modified copy of \b states, where new states have been added.
     */
    std::vector<AlgebraState> getRootChain(
            const AlgebraState&             highestWeightState,
            std::vector<AlgebraState>       states 
                = std::vector<AlgebraState>(0),
            std::vector<std::vector<bool> > directions
                = std::vector<std::vector<bool> >(0)) const;

    void getRootChainExperimental(
            const AlgebraState&              highestWeightState,
            std::vector<AlgebraState>       &states,
            std::vector<std::vector<bool> > &directions) const;

    /*!
     * \brief Computes the multiplicity of the state \b state.
     * \details The multiplicity of the state depends on those of highest weight
     * states. This function is then recursive and calculates on the go the 
     * multiplicities it needs if they have not been alreadt calculated. 
     * \param multiplicities std::vector of integers containing multiplicities. 
     * It is modified during the run, replacing the default -1 by the computed
     * multiplicities.
     * \param rep            Set of all AlgebraState in the representation.
     * \param state          AlgebraState of which we compute the multiplicity.
     */
    void computeMultiplicity(std::vector<int>&                multiplicities,   
                             const std::vector<AlgebraState>& rep,
                             const AlgebraState&              state) const;

    /*!
     * \brief Compute the multiplicities of the states in \b rep, where \b rep
     * is a complete set of AlgebraState.
     * \param rep Set of all the states in the representation.
     * \return The multiplicities for each states in a std::vector of integers.
     */
    std::vector<int> multiplicities(std::vector<AlgebraState>& rep) const;

    /*!
     * \brief Overload of operator<< for SemiSimpleAlgebra. Displays 
     * exhaustively members of the object.
     * \param fout    Output flux.
     * \param algebra SemiSimpleAlgebra to display.
     * \return The modified output flux.
     */
    friend
    std::ostream& operator<<(std::ostream&            fout,
                             const SemiSimpleAlgebra& algebra);

    protected:

    /*!
     * \brief Order of the semi-simple algebra, i.e. number of simple roots.
     */
    const int l;

    /*!
     * \brief std::vector of size \b l. Particular root (half-sum of simple
     * roots) useful in calculations.
     * \details In order to be able to store it in a simple std::vector of
     * integers, the 1/2 factor is discarded in this member, it corresponds then
     * strictly to 2*rho in standard conventions.
     */
    std::vector<int> rho;

    /*!
     * \brief Absolute norms of simple roots used in the computation of the 
     * quadratic Casimir operator. See SemiSimpleAlgebra::getQuadraticCasimir().
     */
    csl::vector_expr absoluteNorms;

    /*!
     * \brief std::vector of integers of size \b l. Contains squared norms of
     * all simple roots.
     */
    std::vector<int> squaredNorm;

    /*!
     * \brief Contains all positive roots in a std::vector. Each positive root
     * is a std::vector of integers of size \b l: dinkin labels in co-root 
     * space.
     */
    std::vector<std::vector<int>> positiveRoots;

    /*!
     * \brief Cartan matrix of the algebra
     */
    std::vector<std::vector<int>> cartanMatrix;

    /*!
     * \brief Metric for roots in co-root space, i.e. metrix to apply in order 
     * to compute scalar dot between roots expressed with their dinkin labels.
     */
    std::vector<std::vector<int>> rootMetric;

    /*!
     * \brief Symbolic version of the half-sum of simple roots (see \b 
     * positiveRoots). Contains the factor 1/2 that positiveRoots does not.
     */
    csl::Expr symbolicRho;

    /*!
     * \brief Symbolic version of the cartan matrix
     */
    csl::Expr symbolicCartan;

    /*!
     * \brief Inverse cartan matrix
     */
    csl::Expr inverseCartan;

    /*!
     * \brief Symbolic metric in root space. i.e. different from \b rootMetric
     * that is the metric in co-root space.
     */
    csl::Expr symbolicMetric;

    /*!
     * \brief Symbolic version of \b positiveRoots, each root is a csl::Vector
     * of csl::Integer in co-root space (dinkin labels).
     */
    csl::vector_expr symbolicRoots;

    /*!
     * \brief Symbolic version of \b squaredNorm.
     */
    csl::vector_expr symbolicSquaredNorms;

    mutable
    std::vector<std::pair<AlgebraState, Irrep>> irreps;

    mutable std::vector<IndexData> indices;
};

} // End of namespace mty

/*!
 * \brief Namespace to avoid name conflicts for semi-simple algebras that have
 * very simple names. See algebra::Type for the same reason.
 */
namespace mty::algebra{

/*!
 * 
 * \brief Algebra (R, +) of the U(1) group.
 * \details This particular algebra has a different treatment as the other 
 * semi-simple algebras and reimplements some of the main functions. R needs 
 * only one charge (possibly fractionnal) to fully describe a representation.
 * No need then for the cartan formalism. A representation is then represented
 * by a two-labels AlgebraState. The first label corresponds to the charge's
 * numerator, and the second corresponds to the denominator.
 */
class R: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor
     */
    R();

    /*!
     * \brief Destructor.
     */
    ~R(){};

    algebra::Type getType() const override;

    /*!
     * \brief Overrides the function of mty::SemiSimpleAlgebra and does nothing (no
     * Cartan matrix).
     */
    void setCartanMatrix() override;

    /*!
     * \brief Overrides the function of mty::SemiSimpleAlgebra and does nothing (no
     * root).
     */
    void setSquaredNorm() override;

    /*!
     * \brief Creates a representation from an AlgebraState containing only
     * the numerator and the denominator of the U(1) charge.
     * \return The U(1) Irrep with the right charge.
     */
    mty::Irrep highestWeightRep(
            const mty::AlgebraState& highestWeight,
            bool mult = true) const override;

    /*!
     * \brief Creates a representation from a std::vector of int containing only
     * the numerator and the denominator of the U(1) charge.
     * \return The U(1) Irrep with the right charge.
     */
    mty::Irrep highestWeightRep(
            const std::vector<int>& highestWeight,
            bool mult = true) const override;

    /*!
     * \return The trivial rep: charge 0.
     */
    mty::Irrep getTrivialRep() const override;

    /*!
     * \param A First Irrep in the product.
     * \param B Second Irrep in the procut.
     * \return The tensor procuct of the two Irrep, i.e. another U(1) rep whose
     * charge is the sum of those of \b A and \b B, embedded in a SumIrrep of 
     * one term.
     */
    mty::SumIrrep tensorProduct(const mty::Irrep& A,
                                const mty::Irrep& B,
                                bool mult = true) const override;

    /*!
     * \brief Sorts the different states of the rep.
     * \details Sorts the different states by increasing value of charge.
     * \param rep  std::vector of AlgebraState to sort (modified).
     * \param mult multiplicities of the states sorted the same way (modified).
     */
    void sortRep(std::vector<mty::AlgebraState>& rep,
                 std::vector<int>&                mult) const override;

    csl::Expr getEta() const override {
        return CSL_0;
    }
};

/*!
 * 
 * \brief Semi-simple Lie algebra A(l) == su(l+1).
 */
class A: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Constructor with one parameter.
     * \param t_l Initializes the order \b l of the algebra. Generates su(l+1).
     */
    explicit
    A(int t_l);

    /*!
     * \brief Destructor.
     */
    ~A(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    csl::Expr getEta() const override {
        return CSL_1;
    }

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Semi-simple Lie algebra B(l) == so(2l+1).
 */
class B: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Constructor with one parameter.
     * \param t_l Initializes the order \b l of the algebra. Generates so(2l+1).
     */
    explicit
    B(int t_l);

    /*!
     * \brief Destructor.
     */
    ~B(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Semi-simple Lie algebra C(l) == sp(2l).
 */
class C: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Constructor with one parameter.
     * \param t_l Initializes the order \b l of the algebra. Generates sp(2l).
     */
    explicit
    C(int t_l);

    /*!
     * \brief Destructor.
     */
    ~C(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Semi-simple Lie algebra D(l) == so(2l).
 */
class D: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Constructor with one parameter.
     * \param t_l Initializes the order \b l of the algebra. Generates so(2l).
     */
    explicit
    D(int t_l);

    /*!
     * \brief Destructor.
     */
    ~D(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Exceptional semi-simple Lie algebra E6.
 */
class E6: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor.
     */
    E6();

    /*!
     * \brief Destructor.
     */
    ~E6(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Exceptional semi-simple Lie algebra E7.
 */
class E7: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor.
     */
    E7();

    /*!
     * \brief Destructor.
     */
    ~E7(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Exceptional semi-simple Lie algebra E8.
 */
class E8: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor.
     */
    E8();

    /*!
     * \brief Destructor.
     */
    ~E8(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Exceptional semi-simple Lie algebra F4.
 */
class F4: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor.
     */
    F4();

    /*!
     * \brief Destructor.
     */
    ~F4(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};

/*!
 * 
 * \brief Exceptional semi-simple Lie algebra G2.
 */
class G2: public mty::SemiSimpleAlgebra{

    public:

    /*!
     * \brief Default constructor.
     */
    G2();

    /*!
     * \brief Destructor.
     */
    ~G2(){};

    algebra::Type getType() const override;

    void setCartanMatrix() override;

    void setSquaredNorm() override;

    mty::Irrep getDefiningRep() const override;
};
} // End of namespace mty::algebra

///////////////////////////////////////////////////
/*************************************************/
// Helper functions in calculations in .cpp      //
/*************************************************/
///////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& fout, const std::vector<T>& vec)
{
    for (const auto& el : vec)
        fout << el << "  ";
    fout << std::endl;

    return fout;
}

template<typename T>
int findInVector(const std::vector<T>& v,
                 const T             & element)
{
    const typename std::vector<T>::const_iterator iter
        = find(v.begin(), v.end(), element);
    if (iter != v.end())
        return distance(v.begin(), iter);
    return -1;
}

template<typename T>
int binaryFindInVector(const std::vector<T>& v,
                       const T             & element)
{
    size_t a = 0;
    size_t b = v.size();
    while (b != a) {
        const auto middleIndex = (a+b) / 2;
        const auto middle = v[middleIndex];
        if (element < middle) {
            b = middleIndex;
        }
        else if (middle < element) {
            a = middleIndex;
        }
        else 
            return middleIndex;
    }
    return -1;
}

template<typename T>
bool addCarefully(std::vector<T>& vec,
                 T                element)
{
    if (findInVector<T>(vec,element) == -1) {
        vec.push_back(element);
        return true;
    }
    return false;
}

template<typename T>
std::vector<T> operator+(const std::vector<T>& x,
                         const std::vector<T>& y)
{
    if (x.size() != y.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    std::vector<T> rep(x.size());
    for (size_t i=0; i!=x.size(); ++i)
        rep[i] = x[i]+y[i];

    return rep;
}

template<typename T>
std::vector<T> operator-(const std::vector<T>& x,
                         const std::vector<T>& y)
{
    if (x.size() != y.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    std::vector<T> rep(x.size());
    for (size_t i=0; i!=x.size(); ++i)
        rep[i] = x[i]-y[i];

    return rep;
}

template<typename T>
std::vector<T> operator-(const std::vector<T>& x)
{
    std::vector<T> rep(x);
    for (auto& el : rep)
        el *= -1;
    return rep;
}

template<typename T>
std::vector<T> operator*(T                     a,
                         const std::vector<T>& x)
{
    std::vector<T> rep(x);
    for (auto& el : rep)
        el *= a;
    return rep;
}

template<typename T>
std::vector<T> operator*(const std::vector<T>& x,
                         T                     a)
{
    std::vector<T> rep(x);
    for (auto& el : rep)
        el *= a;
    return rep;
}

template<typename T>
T operator*(const std::vector<T>& x,
            const std::vector<T>& y)
{
    if (x.size() != y.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    T rep = 0;
    for (size_t i=0; i!=x.size(); ++i)
        rep += x[i]*y[i];

    return rep;
}


template<typename T>
std::vector<T> operator*(const std::vector<std::vector<T>>& A,
                         const std::vector<T>&              x)
{
    if (A.size() == 0)
        return std::vector<T>(0);
    if (A[0].size() != x.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    std::vector<T> rep(A.size(),0);
    for (size_t i=0; i!=A.size(); ++i)
        for (size_t j=0; j!=x.size(); ++j)
            rep[i] += A[i][j]*x[j];

    return rep;
}

template<typename T>
std::vector<T> operator*(const std::vector<T>              &x,
                         const std::vector<std::vector<T>>& A)
{
    if (A.size() == 0)
        return std::vector<T>(0);
    if (A[0].size() != x.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    std::vector<T> rep(A.size(),0);
    for (size_t i=0; i!=A.size(); ++i)
        for (size_t j=0; j!=x.size(); ++j)
            rep[i] += A[i][j]*x[j];

    return rep;
}

template<typename T>
std::vector<std::vector<T> > operator*(const std::vector<std::vector<T>>& A,
                                       const std::vector<std::vector<T>>& B)
{
    if (A.size() == 0 or B.size() == 0) {
        return std::vector<std::vector<T> >(0);
    }
    if (A[0].size() != B.size()) {
        std::cout << "Type: two std::vectors must have "
                  << "the same dimension in product.\n";
        std::exit(1);
    }
    std::vector<std::vector<T> > rep(A.size(), std::vector<T>(B[0].size(),0));
    for (size_t i=0; i!=A.size(); ++i)
        for (size_t j=0; j!=B[0].size(); ++j)
            for (size_t k=0; k!=A[0].size(); ++k)
                rep[i][j] += A[i][k]*B[k][j];

    return rep;
}

#endif

