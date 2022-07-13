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

/*! \file abstract.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Base classes for all exprs in the program.
 */
#ifndef ABSTRACT_H_INCLUDED
#define ABSTRACT_H_INCLUDED

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include <functional>
#include <memory>
#include <optional>

#include <initializer_list>
#include <map>
#include <set>
#include <vector>

#include <mutex>
#include <thread>

#include "allocator.h"
#include "enum.h"
#include "error.h"
#include "evaluation.h"
#include "std_shared_ptr_inheritance.h"
#include "support.h"
#include "typedef.h"
#ifdef DEBUG
#include "alloc_monitor.h"
#endif

// Forward declaration of Index TensorParent
// and IndexStructure classes (indicial.h)
namespace csl {

class Index;
class IndexStructure;
class Space;
class AbstractParent;
class TensorParent;
class ContractionChain;
class LibDependency;

#ifdef DEBUG
class Abstract : public __alloc_monitor {
#else

/*! \brief \b Root class of the inheritance tree of abstracts.
 * \details Contains all functions that derived class needs. In particular all
 * documented functions of the class Abstract are \b specialized in all the \b
 * derived \b classes.
 * \note Some functions are not documented here because not every derived class
 *  use them.
 */
class Abstract {
#endif

  public:
    /*************************************************/
    // Constructors and destructors                  //
    /*************************************************/

    /*!
     * \brief Default Constructor.
     * \details Initializes name empty and commutable to \b True.
     */
#ifdef DEBUG
    Abstract() : __alloc_monitor(){};
#else
    Abstract(){};
#endif

    /*! \brief Destructor.*/
    virtual ~Abstract()
    {
    }

    Expr self();

    /*************************************************/
    // Utils, common to all Abstract                 //
    /*************************************************/

    /*! \brief Displays the abstract in standard output.
     * \param mode Tells if the Abstract is printed alone (default) or in
     * another expr.
     */
    virtual void print(int           mode    = 0,
                       std::ostream &out     = std::cout,
                       LibraryMode   libMode = LibraryMode::NoLib) const = 0;

    virtual void printCode(int mode = 0, std::ostream &out = std::cout) const;

    static std::string regularName(std::string const &name);
    static std::string regularName(std::string_view name);
    static std::string regularLiteral(std::string const &name);
    static std::string regularLiteral(std::string_view name);

    virtual void printProp(std::ostream &fout = std::cout) const;

    /*! \brief Displays explicitely the expression, with types of each
     * component. This function is only used for debug. \param mode Mode of
     * printing.
     */
    void printExplicit(int mode = 0) const;

    /*! \brief Creates a LaTeX output for the Abstract.
     * \param mode Tells if the Abstract is printed alone (default) or in
     * another expr.
     * \return The string corresponding to the LaTeX output.
     */
    virtual std::string printLaTeX(int mode = 0) const;

    virtual std::vector<Parent> getSubSymbols() const = 0;

    virtual LibDependency getLibDependency() const;

    virtual size_t memoryOverhead() const;

    /*************************************************/
    // Common access functions for all Abstracts     //
    /*************************************************/

    /*! \brief Returns the Abstract's name.
     * \return \a name
     */
    virtual std::string const &getName() const;

    virtual std::string const &getLatexName() const;

    /*! \brief Allows to know if the object commutes with all the others.
     * \return \a commutable
     */
    virtual bool getCommutable() const;

    virtual bool getElementary() const;

    virtual bool getAllDependencies() const;

    /*! \brief Gives the \b primary \b type of Abstract.
     * \details In the program this function is very often called. It allows
     * different functions to know what type of expr they are
     * manipulating (single number, scalar function with one argument, with
     * multiple argumments, a Vector, etc) in order to do special treatments
     * or simplifications.
     * \return type (a non memorized integer corresponding to the type of
     * abstract)
     */
    virtual csl::PrimaryType getPrimaryType() const = 0;

    /*! \brief Gives the \b type of Abstract.
     * \details In the program this function is very often called. It allows
     * different functions to know what type of expr they are
     * manipulating (cos, product, number, etc) in order to do special
     * treatments or simplifications.
     * \return type (a non memorized integer corresponding to the type of
     * abstract)
     */
    virtual csl::Type getType() const = 0;

    /*! \brief Gives the \b dimension of the object.
     * \details Allows to know if we are manipulating a pure scalar (i.e. that
     * can have a real value) or something else. There is the particular case
     * of the Imaginary \b i that is considered as a scalar for simplicity
     * but in reality cannot be evaluated with a real. Example: 1 + \b i
     * cannot be reduced.
     * \return dim (a non memorized integer corresponding to the dimension of
     * the abstract)
     */
    virtual int getDim() const;

    /*! \brief Tells if the expression is a Building Block or not.
     * \details Building blocks are derived classes that cannot contain further
     * expressions, i.e. expressions that are the leafs of the recursive
     * tree reprensent mathematical expressions: numerical or pure literal
     * objects (variable, constant etc).
     * \return \b True if \a *this is a Building Block.
     * \return \b False else.
     */
    virtual bool isBuildingBlock() const;

    virtual int getOrderOf(Expr_info expr) const;

    virtual bool isArbitrary() const;

    /*! \return \b True if the expression is indexed.
     * \return \b False else.
     */
    virtual bool isIndexed() const;

    virtual bool isReal() const;

    virtual bool isPurelyImaginary() const;

    virtual bool isComplexConjugate() const;

    virtual bool isHermitianConjugate() const;

    virtual csl::ComplexProperty getComplexProperty() const;

    virtual void setComplexProperty(csl::ComplexProperty prop);

    virtual void setConjugated(bool t_conjugated);

    /*************************************************/
    // Access functions for specializations          //
    /*************************************************/

    ///////////////////////////////////////////////////
    // Numerical- and Literal types
    ///////////////////////////////////////////////////

    /*! \brief Tells if the expression is an integer. Either an Integer object
     * directly, or a Float that has an integer value.
     * \return \b True if \a *this is an Integer or a Float with integer value.
     * \return \b False else.
     */
    virtual bool isInteger() const;
    /*! \brief Tells if the expression is valued, i.e. is a function of numbers
     * and valued literals (a Variable or Constant is not valued by default).
     * \return \b True if the expression is valued.
     * \return \b False else.
     */
    virtual bool getValued() const;
    /*! \brief Returns the value of the expression, if it has one \b
     * explicitely. In particular, it will work only on Numbers and valued
     * Literals, not on functions. \return The value of the expression.
     */
    virtual long double getValue() const;

    virtual long double getDeltaPlus() const;

    virtual long double getDeltaMinus() const;

    ///////////////////////////////////////////////////
    // Class IntFraction
    ///////////////////////////////////////////////////

    /*! \return The numerator for a IntFraction.
     */
    virtual long long int getNum() const;
    /*! \return The denominator for a IntFraction.
     */
    virtual long long int getDenom() const;

    ///////////////////////////////////////////////////
    // (Multiple) argument(s) functions
    ///////////////////////////////////////////////////

    /*! \brief Returns the number of arguments of the expression. If the
     * expression is a building block  (AbstractBuildingBlock), this function
     * returns 0.
     * \return The number of arguments of the expression.
     */
    virtual int getNArgs(int axis = 0) const;

    virtual size_t size() const;

    virtual bool empty() const;

    /*! \return A csl::vector_expr::iterator at the beginning of argument
     * for multi-argument expressions.
     */
    virtual csl::vector_expr::iterator begin();

    /*! \return A csl::vector_expr::iterator at the end of argument
     * for multi-argument expressions.
     */
    virtual csl::vector_expr::iterator end();

    /*! \return A csl::vector_expr::iterator at the beginning of argument
     * for multi-argument expressions.
     */
    virtual csl::vector_expr::const_iterator begin() const;

    /*! \return A csl::vector_expr::iterator at the end of argument
     * for multi-argument expressions.
     */
    virtual csl::vector_expr::const_iterator end() const;

    /*! \warning This function must not be called for building blocks, one must
     * check first that the expression has arguments.
     * \return The i^{th} argument of the expression.
     */
    virtual Expr const &getArgument(int iArg = 0) const;

    virtual Expr &getArgument(int iArg = 0);

    /*! \brief Allows to get specific arguments of expressions in multiple
     * dimensions, by giving the indices in each dimension.
     * \warning This function must not be called for building blocks, one must
     * check first that the expression has arguments.
     * \return The argument {i,j,...} of the expression.
     */
    virtual Expr const &getArgument(const std::vector<int> &indices) const;

    virtual Expr &getArgument(const std::vector<int> &indices);

    /*! \brief Allows to get the entire std::vector of arguments of the
     * expression. \warning This function must not be called for building
     * blocks, one must check first that the expression has arguments. \return
     * The std::vector of argument.
     */
    virtual const csl::vector_expr &getVectorArgument() const;

    ///////////////////////////////////////////////////
    // Derivative, Integral, Polynomial, Commutator
    ///////////////////////////////////////////////////

    /*! \brief Accessor to the variable that defines certain types of
     * expressions. \return \b variable.
     */
    virtual Expr getVariable() const;

    /*! \brief Accessor to the order (integer) that defines certain types of
     * expressions.
     * \return \b order.
     */
    virtual int getOrder() const;

    /*! \return The \b sign for a Commutator type expression.
     */
    virtual int getSign() const;

    /*! \brief Tells if the expression is an operator (like a derivetive
     * operator). \return \b True if the expression is an Operator. \return \b
     * False else.
     */
    virtual bool isAnOperator() const;

    /*! \brief Tells for a Derivative or an Integral if the argument is empty
     * i.e. if the object must apply on the next argument encountered on the
     * right. \return \b True if the Derivative or Integral awaits an argument.
     * \return \b False else.
     */
    virtual bool isEmpty() const;

    virtual bool operatorAppliesOn(Expr_info expr) const;

    /*! \brief Returns the operand of an Operator.
     * \return \b operand of an Operator.
     */
    virtual Expr getOperand() const;

    ///////////////////////////////////////////////////
    // Vectorial-type expressions
    ///////////////////////////////////////////////////

    /*! \brief Accessor to the shape of the tensor in the form of a std::vector
     * of integers.
     * \return \b shape.
     */
    virtual std::vector<int> getShape() const;

    ///////////////////////////////////////////////////
    // Integral
    ///////////////////////////////////////////////////

    virtual Expr getInfBoundary() const;

    virtual Expr getSupBoundary() const;

    virtual void setSupBoundary(Expr const &t_inf);

    virtual void setInfBoundary(Expr const &t_inf);

    ///////////////////////////////////////////////////
    // Indicial-type expressions
    ///////////////////////////////////////////////////
    //

    /*! \return The number of indices of an \b Indicial expression.
     */
    virtual int getNIndices() const;

    /*! \param i Spot of the index to get.
     * \return the i^{th} index of an \b Indicial expression.
     */
    virtual Index getIndex(int i = 0) const;

    virtual void resetIndexStructure();

    /*! \return The index structure of the \b Indicial expression
     */
    virtual IndexStructure getIndexStructure() const;

    IndexStructure getIndexStructure(csl::Space const *space) const;

    virtual const IndexStructure &getIndexStructureView() const;

    virtual IndexStructure &getIndexStructureView();

    /*! \return The free index structure of the \b Indicial expression
     */
    virtual IndexStructure getFreeIndexStructure() const;

    /*! \brief For indicial expressions this function returns a pointer
     * to the parent object of type TensorParent (not an expression).
     * \return \b parent for TensorElement-type expression.
     */
    virtual Parent getParent() const;

    virtual Parent_info getParent_info() const;

    virtual Tensor getPoint() const;

    /*! \brief Returns the number of contracted pairs of indices **in an
     * Indicial expression**. \return The number of contracted pairs of
     * indices.
     */
    virtual int getNContractedPairs() const;

    /*! \brief Returns a std::vector of all possible permutations of an \b
     * Indicial expression. The possible permutations are determined from the
     * posible symmetries and anti-symmetries of the object. \return A
     * std::vector containing all possible permutations of the tensor.
     */
    virtual csl::vector_expr getPermutations(bool optimize = true) const;

    /*! \return All contracted pairs of indices of an \b Indicial
     * expression.
     * \warning This function is not yet well implemented and may not be useful
     * in the future.
     */
    virtual std::set<std::pair<int, int>> getContractedPair() const;

    ///////////////////////////////////////////////////
    // Copy functions for derivated classes
    // (to reimplement)
    ///////////////////////////////////////////////////

    Expr copy() const;

    virtual unique_Expr copy_unique() const = 0;

    virtual Expr deepCopy() const;

    virtual Expr refresh() const;

    virtual Expr deepRefresh() const;

    /*************************************************/
    // Common modifiers for all Abstracts            //
    /*************************************************/

    /*! \brief Change the name of the abstract.
     * \param t_name Replaces name.
     */
    virtual void setName(const std::string &t_name);

    /*! \brief Allows the abstract to commute or not.
     * \param t_commutable Must be \b true if the abstract can commute.
     */
    virtual void setCommutable(bool t_commutable);

    /*************************************************/
    // Modifiers for specializations                 //
    /*************************************************/

    ///////////////////////////////////////////////////
    // Numerical- and Literal- types
    ///////////////////////////////////////////////////

    /*! \brief Sets the value if there is one (for Numerical and Literal
     * valued).
     */
    virtual void setValue(long double t_value);

    virtual void setValue(Expr const &t_value);

    ///////////////////////////////////////////////////
    // Variable
    ///////////////////////////////////////////////////

    virtual void setElementary(bool t_elementary);

    virtual void setAllDependencies(bool t_allDependencies);

    virtual void addDependency(Expr const &expr);

    virtual void removeDependency(Expr const &expr);

    ///////////////////////////////////////////////////
    // (Multi) argument(s) functions
    ///////////////////////////////////////////////////

    /*! \brief Sets the argument at position \b iArg (default=0).
     * \param expr Expression that replaces the argument.
     * \param iArg the position of the argument to change.
     */
    virtual void setArgument(const Expr &expr, int iArg = 0);

    /*! \brief Sets the argument at position \b {i,j,...} for multi-dimensions
     * expressions.
     * \param expr Expression that replaces the argument.
     * \param indices An std::vector containing the series of indices
     * corresponding to the argument to replace.
     */
    virtual void setArgument(const Expr             &expr,
                             const std::vector<int> &indices);

    virtual void setEmpty(bool t_empty);

    /*! \brief Sets the operand of an operator.
     * \param operand New operand.
     */
    virtual void setOperand(const Expr &operand);

    /*! \brief Sets the operand of an operator.
     * \param operand New operand.
     * \param leaveEmpty Boolean that tells if the operator must stay empty (if
     * true, then it will again apply later) or not (if false).
     */
    virtual void setOperandPrivate(const Expr &operand, bool leaveEmpty);

    /*! \brief Replaced the entire std::vector of argument.
     * \param t_argument std::vector of expressions to copy.
     */
    virtual void setVectorArgument(const csl::vector_expr &t_argument);

    virtual void setVariable(Expr const &t_variable);

    ///////////////////////////////////////////////////
    // Sum and Prod
    ///////////////////////////////////////////////////

    /*! \brief Inserts an expression in a sum or a product.
     * \details Allows to insert an element in a sum or product without
     * comparing all existing terms. This saves time when inserting element by
     * element. The \b side parameter allows to insert to the left (side = 0)
     * or to the right (side = 1) in products (useful when considering non
     * commutating expressions.
     * \param expr Expression to insert.
     * \param side Side of insertion for Prod expressions.
     */
    virtual void insert(const Expr &expr, bool side = 1);

    ///////////////////////////////////////////////////
    // Indicial types
    ///////////////////////////////////////////////////

    virtual void setParent(const Parent &t_parent);

    /*! \brief For indicial expressions, this function searches \b
     * indexToContract and replaces it with newIndex.
     * \param indexToContract Index that is newly contracted.
     * \param newIndex Dummy new index that replaces \b indexToContract
     * in the expression.
     * \return \b True if the index has been found.
     * \return \b False else.
     */
    virtual std::optional<Expr> replaceIndex(const Index &indexToReplace,
                                             const Index &newIndex,
                                             bool refresh = true) const;

    virtual std::optional<Expr>
    replaceIndices(std::vector<Index> const &indexToReplace,
                   std::vector<Index> const &newIndex,
                   bool                      refresh = true,
                   bool                      flipped = false) const;

    virtual void replaceIndexInPlace(Index const &oldIndex,
                                     Index const &newIndex);

    std::optional<Expr> contractIndex(const Index &index) const;

    csl::vector_expr
    breakSpace(const Space                      *brokenSpace,
               const std::vector<const Space *> &newSpace) const;

    virtual csl::vector_expr
    breakSpace(const Space                      *brokenSpace,
               const std::vector<const Space *> &newSpace,
               const std::vector<std::string>   &indexNames) const;

    /*! \brief Replaces the index structure of the object, that must be an
     * \b Indicial expression.
     * \param t_index A std::vector of Index which takes the place of the
     * structure index.
     */
    virtual void setIndexStructure(const IndexStructure &t_index);

    virtual void setPoint(const Tensor &t_point);

    /*! \brief Sets an \b Indicial object fully symmetric. Allows to set
     * quickly a frequent property of tensors. This function then erases all
     * properties of symmetry / antisymmetry and sets \b fullySymmetric to \b
     * True.
     */
    virtual void setFullySymmetric();

    /*! \brief Sets an \b Indicial object fully anti-symmetric. Allows to set
     * quickly a frequent property of tensors. This function then erases all
     * properties * of symmetry / antisymmetry and sets \b fullyAntiSymmetric
     * to \b True.
     */
    virtual void setFullyAntiSymmetric();

    /*! \brief Add a symmetry between the i1^{th} and the i2^{th} indices. If
     * those indices are anti-symmetric, an error is thrown.
     * \param i1 Position of the first index.
     * \param i2 Position of the second index.
     */
    virtual void addSymmetry(int i1, int i2);

    /*! \brief Add an anti-symmetry between the i1^{th} and the i2^{th}
     * indices. If those indices are symmetric, an error is thrown. \param i1
     * Position of the first index. \param i2 Position of the second index.
     */
    virtual void addAntiSymmetry(int i1, int i2);

    /*! \brief Tries to permut indices at place \b i1 and \b i2. If those two
     * indices have a symmetry property, indices are swaped and the symmetry is
     * returned. Else the fnuction does nothing and returns 0. \param i1
     * Position of the first index. \param i2 Position of the second index.
     * \return 1 if the permutation **(i1,i2) is symmetric**.
     * \return -1 if the permutation **(i1,i2) is anti-symmetric**.
     * \return 0 else (and do not permut the two indices).
     */
    virtual int permut(int i1, int i2);

    /*************************************************/
    // Getting properties that need calculation      //
    /*************************************************/

    ///////////////////////////////////////////////////
    // Useful functions in factorization and
    // for making canonical expressions (basic
    // simplifications).
    ///////////////////////////////////////////////////

    /*! \brief Returns the numerical factor of the expression, i.e. returns
     * \b C if the expression if of the form **C*x** (\b x having a numerical
     * factor equal to 1), and return 1 else.
     * \note This function returns the factor in an Expression (then of
     * Numerical type). \return The numerical factor in front of the
     * expression.
     */
    virtual Expr getNumericalFactor() const;

    /*! \return The number of possible factors for the expression
     */
    virtual int getNFactor() const;

    /*! \brief Allows to get a std::vector of all terms than could factor the
     * expression.
     * \return A std::vector containing the possible factors of \b *this.
     */
    virtual csl::vector_expr getFactors() const;

    /*! \brief This function returns the same expression as *this but amputated
     * of its numerical factor. Example: (4*cos(x) -> cos(x)).
     * \return The term without numerical factor corresponding to \b *this.
     */
    virtual std::optional<Expr> getTerm() const;

    /**
     * @brief Fills in a vector the exponents corresponding to some factors for
     * the expression.
     *
     * @details For example, an expression like \f$ 2ax^2\cos y \f$ will have
     * exponents \f$ (1, 2, 0) \f$ for the set of factors \f$ (a, x, y). \f$
     * \b factors and \b exponents must of course be of the same size.
     * Otherwise the behaviour is undefined.
     *
     * @note This function assumes that the expression is canonical, in
     * particular that no terms like \f$ x\cdot x^2 \f$ can appear.
     *
     * @note This function does not take into account factors in sums like
     * \f$ (1 + x) \f$ (this will return a factor 0 for \f$ x \f$).
     *
     * @param factors   Factors.
     * @param exponents Exponents (out variable, modified during the run).
     */
    virtual void getExponents(std::vector<Expr> const &factors,
                              std::vector<Expr>       &exponents) const;

    ///////////////////////////////////////////////////
    // Checking matching of different indicial
    // expressions in calculations.
    ///////////////////////////////////////////////////

    /*! \brief Checks the compatibility of the index structure of an \b
     * Indicial expression with another. In a sum, two terms must have exaclty
     * the same index structure. \param t_index A std::vector of Index to
     * compare. \return \b True if the two structures match. \return \b False
     * else.
     */
    virtual bool checkIndexStructure(const std::vector<Index> &t_index) const;

    /*! \brief Checks the compatibility of the index structure of an \b
     * Indicial expression with another. In a sum, two terms must have exaclty
     * the same index structure. \param t_index A std::initializer_list of
     * Index to compare. \return \b True if the two structures match. \return
     * \b False else.
     */
    virtual bool
    checkIndexStructure(const std::initializer_list<Index> &index) const;

    /*! \brief Comparison disregarding name of dummy indices, i.e. the two
     * expressions * are equals even if dummy indices have not the same names
     * in *this and * expr. \param expr Expression to compare. \param
     * constraints List of existing constraints between indices (it is modified
     * in the function).
     *
     * \return \b True if expr == *this taking constraints into account.
     * \return \b False else.
     */
    virtual bool compareWithDummy(Expr_info               expr,
                                  std::map<Index, Index> &constraints,
                                  bool keepAllCosntraints = false) const;

    bool compareWithDummy(Expr_info expr,
                          bool      keepAllCosntraints = false) const;

    /*************************************************/
    // Getting properties depending on an expr //
    /*************************************************/

    /*! \brief Returns the parity property of the expression with respect to
     * \b t_variable.
     * \param t_variable.
     * \return 1  if the expression is \b even in \b t_variable.
     * \return -1 if the expression is \b odd in \b t_variable.
     * \return 0  else.
     */
    virtual int getParity(Expr_info t_variable) const;

    /*! \brief Check if \b expr can factor \b *this.
     * \details In almost every case this corresponds just to the comparison
     * ** *this == expr**. For Prod, Pow, Fraction types (all that are
     * multiplicative) we must check if the factor hides in a product.
     * \param expr Factor to search in the expression.
     * \return \b True if \b expr is a possible factor.
     * \return \b False else.
     */
    virtual bool askTerm(Expr_info expr, bool exact = false) const;

    /*! \brief Check recursively if the expression depends on \b expr.
     * \param expr Expression to search.
     * \return \b True if a dependency in \b expr is found.
     * \return \b False else.
     */
    virtual bool dependsOn(Expr_info expr) const;

    virtual bool dependsOn(Parent_info parent) const;

    /*! \brief Check recursively if \b expr is present in the expression.
     * \param expr Expression to search.
     * \return \b True if \b expr is found.
     * \return \b False else.
     */
    virtual bool dependsExplicitlyOn(Expr_info expr) const;

    virtual bool dependsExplicitlyOn(Parent_info parent) const;

    /*! \brief Tells if the object commutes with expr.
     * \param expr
     * \return \b True if \b *this commutes with \b expr.
     * \return \b False else.
     */
    virtual bool commutesWith(Expr_info expr, int sign = -1) const;

    /*! \brief Searches a sub-expression and replaces it.
     * \param subExpression Expression to search.
     * \param newExpression Expression that replaces \b subExpression if it is
     * found. \return The expression with the replacement done.
     */
    virtual std::optional<Expr>
    findSubExpression(Expr_info   subExpression,
                      const Expr &newExpression) const;

    /*! \brief Determines if the expression is a mononomial term in \b expr,
     * i.e. a term of the form C*expr^n with C independent of expr, n integer.
     * \param expr Variable of the supposed mononomial.
     * \return The order of the exponent if there is one (n in the example).
     * \return -1 else.
     */
    virtual int isPolynomial(Expr_info expr) const;

    /*! \brief In the case of a vectorial-type expression, this function
     * checks if the shape of expr matches itself.
     * \details If \b exact is true, the function search an exact match i.e.
     * either the two shapes are exactly equal or one of the two objects is a
     * scalar. If \b exact is false, this function only search for a possible
     * dot product between the two expressions, and see if the last axis of \b
     * *this matches the first of \b expr (or if one of the two objects is
     * scalar also). Then, a product \sum _k (*this)[i,j,...,k]*expr[k,l,m,...]
     * is possible. \param expr Expression of which we compare the shape.
     * \param exact Boolean than specifies if we need an exact match or not.
     * \return \b True if the two shapes correspond.
     * \return \b False else.
     */
    virtual bool matchShape(Expr_info expr, bool exact = false) const;

    /*! \brief Tells (for an Indicial type) if there is a special contraction
     * property with \b B.
     * \param B Expression with which we test if there is a special
     * contraction. \return \b True if there is a contraction. \return \b False
     * else.
     */
    virtual bool hasContractionProperty(Expr_info B) const;

    virtual bool hasChainContractionProperty() const;

    virtual std::vector<ContractionChain> getContractionProperties() const;

    /*! \brief Applies a special contraction of indices. Before calling this
     * function we must check that there is indeed a contraction by calling the
     * function hasContractionProperty().
     * \param B Expression with which \b *this contracts.
     * \return The result of the contraction.
     */
    virtual Expr contraction(Expr_info B) const;

    virtual Expr contraction(const csl::vector_expr &chain) const;

    /*************************************************/
    // Members that return new exprs                 //
    // results of specific calculations              //
    // COMMON TO ALL ABSTRACTS PART                  //
    /*************************************************/

    /*! \brief \b Evaluates the value of the Abstract.
     * \details Tries to replace all variables by a real value.
     * If it is not possible (for example treating a Vector or an Imaginary)
     * the considered object is replaced by 0. A warning message is displayed
     * in the case of i.
     * \return The value of the Abstract (double).
     */
    virtual long double evaluateScalar() const;

    /*! \brief \b Evaluates the Abstract.
     * \details Replaces all variables by their value and evaluating.
     * This function must be used instead of evaluateScalar() when treating
     * not only real scalars.* In particular (x=2, y=3) x+iy evaluates to 3+
     * 3i whereas evaluateScalar() would return 3.
     * \return The abstract result of the evaluation.
     */
    virtual std::optional<Expr> evaluate(csl::eval::mode user_mode
                                         = csl::eval::base) const = 0;

    /*! \brief Calculates the derivative of the Abstract wrt another.
     * \details It is possible to derive wrt any complicated expr.
     * In this case however, the calculation is \b not \b always \b
     * mathematically \b correct. The program just searches for equal
     * Abstract or Abstract with the same name. In particular dx/d(exp(x))=0.
     * \param expr Expression wrt which we derive.
     * \return The derivative.
     */
    virtual std::optional<Expr> derive(Expr_info expr) const;

    /*! \brief \b Factors the Abstract.
     * \details This function tries to factor the Abstract wrt any \b factor.
     * This will be more involved in calculation than the other factorizing
     * function that takes the factor as a parameter. So this function **must
     * be used only** if we don't know the factors we want at the end.
     * \param full If true the factorization is \b recursive through all the
     * Abstract.
     */
    virtual std::optional<Expr> factor(bool full = false) const;

    /*! \brief \b Factors the Abstract wrt a **particular** Abstract.
     * \param factor Abstract wrt which we try to factor.
     * \param full   If true the factorization is \b recursive through all the
     * Abstract.
     */
    virtual std::optional<Expr> factor(Expr_info factor,
                                       bool      full = false) const;

    /**
     * @brief Collects terms in sum according to some factors given by the
     * user.
     *
     * @details This function allows the factor with some specific scalar
     * variables. For example, \f$ ax + ay + by \f$ can be factored in two
     * ways. Either with \f$ (a, b) \f$ which gives \f$ a(x+y) + by \f$ ; or
     * with \f$ (x, y) \f$ which gives \f$ ax + (a+b)y \f$. With the collect
     * function it is possible to choose precisely the set of variables that
     * will be factored to express results in a standard way.
     *
     * @param factors Factors to search in the expression.
     * @param full    Boolean (default = false) that tells if the collection
     * must be recursive (full expression depth).
     *
     * @return The collected expression if modifications have been done.
     * @return std::nullopt else.
     *
     * @note This function will probably replace the factor() function in the
     * future.
     *
     * @note For now, the collect function does not support factorization by
     * indicial tensors, whereas factor() does.
     */
    virtual std::optional<Expr> collect(std::vector<Expr> const &factors,
                                        bool full = false) const;

    /*! \brief Remove a factor from an expr, that must have been
     * determined before.
     * \param factor Expression to remove
     * \return The expr in which \b factor has been removed
     */
    virtual Expr suppressTerm(Expr_info expr) const;

    /**
     * @brief Returns the expression where the factor **factor^exponent** has
     * been suppressed.
     *
     * @details This function works the same manner as getExponents().
     *
     * @param factor   Factor to suppress.
     * @param exponent Exponent of the factor to suppress.
     *
     * @return The modified expression if the factor has been found.
     * @return std::nullopt else.
     *
     * @note This function may replace in the future the functions askTerm()
     * and suppressTerm().
     */
    virtual std::optional<Expr> suppressExponent(Expr const &factor,
                                                 Expr const &exponent) const;

    /*! \brief \b Develops the Abstract.
     * \details This function concerns only products (and exponents) that will
     * be flatten to give at the end a sum of independant terms. \param full If
     * true the expandment is \b recursive through all the Abstract. \return
     * The expand Abstract.
     */
    virtual std::optional<Expr> expand(bool full    = false,
                                       bool inPlace = false) const;

    /*! \brief \b Develops the Abstract.
     * \details This function concerns only products (and exponents) that will
     * be flatten to give at the end a sum of independant terms. \param f
     * Functions that returns a boolean that determines which arguments must be
     * expanded in products. \param full If true the expandment is \b recursive
     * through all the Abstract. \return The expand Abstract.
     */
    virtual std::optional<Expr>
    expand_if(std::function<bool(Expr const &)> const &f,
              bool                                     full    = false,
              bool                                     inPlace = false) const;

    /*! \brief \b Evaluates the **real part** of the Abstract and returns it.
     * \return The real part of the Abstract.
     */
    virtual std::optional<Expr> getRealPart() const;

    /*! \brief \b Evaluates the **imaginary part** of the Abstract and returns
     * it. \return The imaginary part of the Abstract.
     */
    virtual Expr getImaginaryPart() const;

    /*! \brief \b Evaluates the **modulus in the complex plane** of the
     * Abstract and returns it. \return The modulus part of the Abstract.
     */
    virtual std::optional<Expr> getComplexModulus() const;

    /*! \brief \b Evaluates the **argument in the complex plane** of the
     * Abstract and returns it.
     * \return The argument part of the Abstract.
     */
    virtual std::optional<Expr> getComplexArgument() const;

    /*! \brief Calculates and returns the complex conjugate of the expression.
     * \return \bar{*this}.
     */
    virtual std::optional<Expr> getComplexConjugate() const;

    virtual Expr &applySelfStructureOn(Expr &expr) const;

    virtual std::optional<Expr> getTransposed(const Space *space,
                                              bool applyProp = true) const;

    virtual std::optional<Expr>
    getTransposed(const std::vector<const Space *> &spaces,
                  bool                              applyProp = true) const;

    virtual std::optional<Expr>
    getHermitianConjugate(const Space *space) const;

    virtual std::optional<Expr>
    getHermitianConjugate(const std::vector<const Space *> &spaces) const;

    /*! \brief Calculates and returns the polynomial term corresponding to
     * \b *this with the variable \b t_variable at order \b order. In
     * particular, this function assumes that the checks have already been made
     * with the function isPolynomial(). \param t_variable Variable of the
     * polynomial. \order Order of \b *this in \b t_variable. \return The same
     * expression as (*this) with the term **t_variable^order** removed.
     */
    virtual std::optional<Expr> getPolynomialTerm(Expr_info t_variable,
                                                  int       order) const;

    /*! \brief Apply the operator on an \b operand, iif the expression is an
     * operator.
     * \param operand Operand of the operator.
     * \return The operator filled with \b operand.
     */
    virtual Expr applyOperator(const Expr &operand,
                               bool        leaveEmpty = false) const;

    /*************************************************/
    // Members that return new exprs                 //
    // results of specific calculations              //
    // SPECIFIC TO SOME ABSTRACTS PART               //
    /*************************************************/

    ///////////////////////////////////////////////////
    // Numerical- Polynomial- and Vectorial-types
    ///////////////////////////////////////////////////

    /*! \brief Contains implementation of special addition for Numerical- and
     * Vectorial-types.
     * \param expr Right operrand of the addition.
     * \return The sum of the two operands.
     */
    virtual Expr addition_own(const Expr &expr) const;

    /*! \brief Contains implementation of special multiplication for Numerical-
     * and Vectorial-types. \param expr Right operrand of the product. \return
     * The product of the two operands.
     */
    virtual Expr multiplication_own(const Expr &expr, bool side = 1) const;

    /*! \brief Contains implementation of special division for Numerical- and
     * Polynomial-types. For polynomial, the euclidean division of two
     * polynomials is implemented. \param expr Right operrand of the division.
     * \return The division of the two operands.
     */
    virtual Expr division_own(const Expr &expr) const;

    /*! \brief Contains implementation of special exponentiation for Numerical-
     * and Vectorial-types. \param expr exponent. \return The exponentiation of
     * the two operands.
     */
    virtual Expr exponentiation_own(const Expr &expr) const;

    ///////////////////////////////////////////////////
    // Polynomial type
    ///////////////////////////////////////////////////

    /*! \brief Returns a regular expression from the polynomial, that is a sum
     * where the different powers of the variable appear explicitely.
     * \return a Sum expression equal to the polynomial.
     */
    virtual Expr getRegularExpression() const;

    ///////////////////////////////////////////////////
    // Vectorial type
    ///////////////////////////////////////////////////

    /*! \brief Returns the tensordot of two \b Vectorial expressions.
     * \param expr The right operand of the tensordot
     * \return The tensor dot of \b *this and \b expr.
     */
    virtual Expr tensordot(const Expr &expr) const;

    /*! \brief Returns the dot product of two \b Vectorial expressions.
     * \param expr The right operand of the dot product.
     * \return sum _k (*this)[i,j,...,k]*expr[k,l,m,...].
     */
    virtual Expr dot(const Expr &expr) const;

    /*! \brief Calculates and returns the sum of all elements in the \b
     * Vectorial object. \return The sum of all elements.
     */
    virtual Expr getSum() const;

    /*! \brief Calculates and returns the product of all elements in the \b
     * Vectorial object. \return The product of all elements.
     */
    virtual Expr getProduct() const;

    /*! \brief Returns the Vectorial modulus of the \b Vectorial object, that
     * is defined here as the squared root of the sum of element squared.
     * Example: \sqrt{A_{11}^2+A_{12}^2+\ldot}. \return The vectorial modulus
     * of the expression.
     */
    virtual Expr getVectorialModulus() const;

    /*! \brief Allows to pick a part of a Vectorial expression, excluding
     * the iExcept^{th} element.
     * \param iExcept Element to ignore.
     * \return The part of *this excluding iExcept.
     */
    virtual Expr getSubVectorial(int iExcept) const;

    /*! \brief Allows to pick a part of a Vectorial expression, excluding
     * the [iExcept^{th},jExcept^{th}] element (useful for matrices).
     * \param iExcept Element of the first axis to ignore.
     * \param jExcept Element of the second axis to ignore.
     * \return The part of *this excluding iExcept.
     */
    virtual Expr getSubVectorial(int iExcept, int jExcept) const;

    /*! \brief Allows to pick a part of a Vectorial expression, excluding
     * the iExcept^{th} element.
     * \param iExcept Element to ignore.
     * \return The part of *this excluding iExcept.
     */
    virtual Expr getSubVectorial(const std::vector<int> &exceptions) const;

    virtual Expr
    getSubVectorial(const std::vector<std::vector<int>> &keepIndices) const;

    virtual Expr
    getSubVectorial(std::vector<std::vector<int>>::const_iterator begin,
                    std::vector<std::vector<int>>::const_iterator end) const;

    /*! \brief Returns the determinant of the object if it corresponds to a
     * square matrix (or a scalar), 0 else.
     * \return det(\b *this) if \b *this is a square matrix or scalar.
     * \return 0 else.
     */
    virtual Expr determinant() const;

    /*! \return \sum _i A_{ii} for a square matrix A.
     * \return A for a scalar A.
     * \return 0 else.
     */
    virtual Expr trace() const;

    /*! \brief Calculates the trace over the axis \b axis1 and \b axis2 of a
     * tensor. \b axis1 and \b axis2 can be the same, in which case the trace
     * just corresponds to the sum over this particular axis.
     * \param axis1 First axis to contract.
     * \param axis2 Second axis to contract.
     * \return the trace over axis \b axis1 and \b axis2.
     */
    virtual Expr trace(int axis1, int axis2) const;

    /*! \brief Calculates and returns the transpose of a 2D matrix.
     * \return A^T for a matrix (2D) A.
     */
    virtual Expr transpose() const;

    virtual Expr hermitian() const;

    /*! \brief Calculates and returns the symmetrization of a 2D matrix.
     * \return 1/2*(A + A^T) for a matrix (2D) A.
     */
    virtual Expr symmetrise() const;

    /*! \brief Calculates and returns the anti-symmetrization of a 2D matrix.
     * \return 1/2*(A - A^T) for a matrix (2D) A.
     */
    virtual Expr antisymmetrise() const;

    /*! \brief Calculates and returns the inverse of a 2D square matrix. The
     * applied method is: A^{-1} = 1/det(A)*Com(A)^T.
     * \return A^{-1} for a matrix (2D) A if det(A) != 0.
     * \return 0 else
     */
    virtual Expr inverseMatrix() const;

    ///////////////////////////////////////////////////
    // Indicial
    ///////////////////////////////////////////////////

    virtual Expr getCanonicalPermutation() const;

    ///////////////////////////////////////////////////
    //// DiracDelta
    /////////////////////////////////////////////////////

    virtual Expr applyDiracDelta(const Expr &, const Expr &) const;

    /*************************************************/
    // Operators                                     //
    /*************************************************/

    /*! \brief Equivalent to the setValue() function.
     * \param t_value The new value of the expression.
     */
    virtual void operator=(double t_value);

    /*! \return \b True if the expression is valued and is equal to t_value.
     * \return \b False else.
     */
    virtual bool operator==(int t_value) const;
    /*! \return \b True if the expression is valued and is equal to t_value.
     * \return \b False else.
     */
    virtual bool operator==(double t_value) const;
    /*! \return \b False if the expression is valued and is equal to t_value.
     * \return \b True else.
     */
    virtual bool operator!=(int t_value) const;
    /*! \return \b False if the expression is valued and is equal to t_value.
     * \return \b True else.
     */
    virtual bool operator!=(double t_value) const;

    /*! \brief \b Compares the Abstract with another.
     * \details Here if two Abstracts have the same name, the function will
     * return \b true even if they are \b not \b mathematically \b equal.
     * So beware not to name different things the same way.
     * \param expr Abstract to compare.
     * \return \b True if the two Abstracts are the same (or have the same
     * name). \return \b False else.
     */
    virtual bool operator==(Expr_info expr) const = 0;

    int testDummy(Expr_info expr) const;

    inline bool operator==(const Expr_c &expr) const;
    inline bool operator==(const Expr &expr) const;
    inline bool operator==(const Abstract &other) const;
    inline bool operator!=(Expr_info expr) const;
    /*! \brief \b Compares the Abstract with another.
     * \param expr Abstract to compare.
     * \return False if the two Abstracts are the same (or have the same name).
     * \return True else.
     */
    inline bool operator!=(const Expr_c &expr) const;
    inline bool operator!=(const Expr &expr) const;
    inline bool operator!=(const Abstract &other) const;
    /*! \brief Access operator for multi-argument expressions, equivalent to
     * the function getArgument().
     * \param iArg Index of the argument to get.
     * \return \b argument[iArg].
     */
    virtual Expr const &operator[](int iArg) const;

    /*! \brief Access operator for multi-argument expressions, returns
     * a reference so this function is not const.
     * \param iArg Index of the argument to get.
     * \return A reference to \b argument[iArg].
     */
    virtual Expr &operator[](int iArg);

    ///////////////////////////////////////////////////
    // Comparison operators in terms of simplicity
    // For example x < x^2, (y*x+1) > (1+x*y) etc
    ///////////////////////////////////////////////////

    /*! \brief Compares the simplicity of the expression to another.
     * \param expr \b Expression to compare.
     * \return \b False if \b expr is simpler or equivalent.
     * \return \b True else.
     */
    virtual bool operator<(Expr_info expr) const = 0;

    /*! \brief Compares the simplicity of the expression to another.
     * \param expr \b Expression to compare.
     * \return \b False if \b expr is simpler or equivalent.
     * \return \b True else.
     */
    inline bool operator<(const Expr_c &expr) const
    {
        return *this < expr.get();
    }

    /*! \brief Compares the simplicity of the expression to another.
     * \param expr \b Expression to compare.
     * \return \b True if \b expr is simpler.
     * \return \b False else.
     */
    inline bool operator>(Expr_info expr) const
    {
        return *expr < this;
    }

    inline bool operator>(const Expr_c &expr) const
    {
        return *expr < this;
    }

    inline bool operator<=(Expr_info expr) const
    {
        return (not this->operator>(expr));
    }

    /*! \brief Compares the simplicity of the expression to another.
     * \param expr \b Expression to compare.
     * \return \b True if \b expr is simpler or equivalent.
     * \return \b False else.
     */
    inline bool operator>=(Expr_info expr) const
    {
        return (not this->operator<(expr));
    }

    /*! \brief Compares the simplicity of the expression to another.
     * \param expr \b Expression to compare.
     * \return \b False if \b expr is simpler.
     * \return \b True else.
     */
    inline bool operator<=(const Expr_c &expr) const
    {
        return operator<=(expr.get());
    }

    inline bool operator>=(const Expr_c &expr) const
    {
        return operator>=(expr.get());
    }
};

/*************************************************/
// User-functions to create objects defined      //
// in this file                                  //
/*************************************************/

/*! \fn Expr sum_s(const Expr& leftOperand, const Expr& rightOperand)
 * \brief Returns the \b sum of the two operands, applying basic
 * simplifications. \details This function possibly returns an object different
 * from a Sum object if the simplification requires so. For example 0+x -> x
 * that is a Variable. \param leftOperand  Left operand. \param rightOperand
 * Right operand. \param explicitSum  Tells if merging of terms must be
 * disabled (default = false). \return The sum of \b leftOperand and \b
 * rightOperand.
 */
Expr sum_s(const Expr &leftOperand,
           const Expr &rightOperand,
           bool        explicitSum = false);

/*! \fn Expr sum_s(const csl::vector_expr& operands)
 * \brief Returns the \b sum of the operands, applying basic simplifications.
 * \details This function possibly returns an object different from a Sum
 * object if the simplification requires so. For example 0+x+2x -> 3x
 *  that is a Prod object.
 * \param operands vector containing the operands.
 * \return The sum of the operands.
 */
Expr sum_s(const csl::vector_expr &operands, bool explicitSum = false);

/*! \fn Expr minus_(const Expr& leftOperand, const Expr& rightOperand)
 * \brief Returns the \b sum of the two operands (with a minus sign),
 *  applying basic simplifications.
 * \details This function possibly returns an object different from a Sum
 * object if the simplification requires so. For example 0+x -> x that is
 * a Variable. The Minus object does not exists, this function
 *  applies leftOperand + (-1*rightOperand).
 * \param leftOperand  Left operand.
 * \param rightOperand Right operand.
 * \return The sum of \b leftOperand and \b rightOperand.
 */
Expr minus_(const Expr &leftOperand, const Expr &rightOperand);
/*! \fn Expr prod_s(const Expr& leftOperand,
                    const Expr& rightOperand,
                    bool explicitProd=0)
  * \brief Returns the \b product of the two operands, applying basic
  simplifications.
  * \details This function possibly returns an object different from a Prod
  * object if the simplification requires so. For example 1*x -> x that is a
  Variable.
  * \param leftOperand  Left operand.
  * \param rightOperand Right operand.
  * \param explicitProd If \b true Prod::mergeTerms() is not called and
  * no simplification is done concerning the reordering/merging of terms.
  * \return The product of \b leftOperand and \b rightOperand.
  */
Expr prod_s(const Expr &leftOperand,
            const Expr &rightOperand,
            bool        explicitProd = 0);

/*! \fn Expr prod_s(const csl::vector_expr& operands)
 * \brief Returns the \b product of the operands, applying basic
 * simplifications. \details This function possibly returns an object different
 * from a Prod object if the simplification requires so. For example 1*x*x^2 ->
 * x^3 that is a Pow. \param operands vector containing the operands. \param
 * explicitProd If \b true Prod::mergeTerms() is not called and no
 * simplification \return The product of \b leftOperand and \b rightOperand.
 */
Expr prod_s(const csl::vector_expr &operands, bool explicitProd = false);

/*! \fn Expr fraction_s(const Expr& leftOperand, const Expr& rightOperand)
 * \brief Returns the \b fraction of the two operands, applying basic
 * simplifications. \details This function possibly returns an object different
 * from a Fraction object if the simplification requires so. For example
 * 1/(x^(-1)) -> x that is a Variable. \param leftOperand  Left operand. \param
 * rightOperand Right operand. \return The fraction of \b leftOperand and \b
 * rightOperand.
 */
Expr fraction_s(const Expr &leftOperand, const Expr &rightOperand);

/*! \fn Expr pow_s(const Expr& leftOperand, const Expr& rightOperand)
 * \brief Returns the \b exponentiation of the two operands, applying basic
 * simplifications.
 * \details This function possibly returns an object different from a Pow
 * object if the simplification requires so. For example x^1 -> x that is a
 * Variable. \param leftOperand  Left operand. \param rightOperand Right
 * operand. \return The exponentiation of \b leftOperand and \b rightOperand.
 */
Expr pow_s(const Expr &leftOperand, const Expr &rightOperand);

Expr sqrt_s(const Expr &operand);

Expr sqrt_s(int number);

/*! \fn Expr derivative_s(const Expr& leftOperand,
                         const Expr& rightOperand,
                         int order)
  * \brief Returns the \b derivative of \b leftOperand wrt \a rightOperand,
  * applying basic simplifications.  \details This function possibly
  * returns an object different from a Derivative object if
  * the simplification requires so.
  * \param leftOperand  Left operand.
  * \param rightOperand Right operand.
  * \param order Order of derivation.
  * \return The derivative of \b leftOperand wrt \b rightOperand at the order
  \b order.
  */
Expr derivative_s(const Expr &leftOperand,
                  const Expr &rightOperand,
                  int         order = 1);
Expr derivative_s(const Expr &leftOperand,
                  const Expr &rightOperand,
                  int         order,
                  bool        empty);

Expr derivative_s(const Expr &variable, int order = 1);

Expr integral_s(const Expr &leftOperand, const Expr &rightOperand);
Expr integral_s(const Expr &leftOperand, const Expr &rightOperand, bool empty);
Expr integral_s(const Expr &leftOperand,
                const Expr &rightOperand,
                bool        empty,
                const Expr &t_inf,
                const Expr &t_sup);
Expr integral_s(const Expr &variable);

Expr polynomial_s(const Expr &expr, const Expr &t_variable);

Expr polynomial_s(const csl::vector_expr &operands, const Expr &t_variable);

///////////////////////////////////////////////////
/*************************************************/
// Arithmetic operators                          //
/*************************************************/
///////////////////////////////////////////////////

/*! \fn Expr operator+(const Expr& a, const Expr& b)
 * \brief Shortcut function that allows to use arithmetic
 * operator + with Expr (== shared_ptr<Abstract>).
 * \param a const Expr& .
 * \param b const Expr&.
 * \return The expression result of a+b.
 */
Expr operator+(const Expr &a, const Expr &b);

Expr operator-(const Expr &a);

/*! \fn Expr operator-(const Expr& a, const Expr& b)
 * \brief Shortcut function that allows to use arithmetic
 * operator - with Expr (== shared_ptr<Abstract>).
 * \param a const Expr&.
 * \param b const Expr&.
 * \return The expression result of a-b.
 */
Expr operator-(const Expr &a, const Expr &b);

/*! \fn Expr operator*(const Expr& a, const Expr& b)
 * \brief Shortcut function that allows to use arithmetic
 * operator * with Expr (== shared_ptr<Abstract>).
 * \param a const Expr&.
 * \param b const Expr&.
 * \return The expression result of a*b.
 */
Expr operator*(const Expr &a, const Expr &b);

/*! \fn Expr operator/(const Expr& a, const Expr& b)
 * \brief Shortcut function that allows to use arithmetic
 * operator / with Expr (== shared_ptr<Abstract>).
 * \param a const Expr&.
 * \param b const Expr&.
 * \return The expression result of a/b.
 */
Expr operator/(const Expr &a, const Expr &b);

/*! \fn Expr operator^(const Expr& a, const Expr& b)
 * \brief Shortcut function that allows to use arithmetic
 * operator ^ with Expr (== shared_ptr<Abstract>).
 * \param a const Expr&.
 * \param b const Expr&.
 * \return The expression result of a^b.
 */
// Expr operator^(const Expr& a, const Expr& b);

/*!
 * \brief Expression type/
 */
class Expr : public std::shared_ptr<csl::Abstract> {

  public:
    INHERIT_SHARED_PTR_CONSTRUCTOR(Expr, csl::Abstract)

    Expr(int value);
    Expr(long int value);
    Expr(long long int value);
    Expr(unsigned int value);
    Expr(unsigned long int value);
    Expr(unsigned long long int value);

    Expr(float value);
    Expr(double value);
    Expr(long double value);

    inline explicit operator Expr_info() const
    {
        return get();
    }

    inline csl::vector_expr::iterator begin()
    {
        return (**this).begin();
    }
    inline csl::vector_expr::iterator end()
    {
        return (**this).end();
    }
    inline csl::vector_expr::const_iterator begin() const
    {
        return (**this).begin();
    }
    inline csl::vector_expr::const_iterator end() const
    {
        return (**this).end();
    }

    template <class Index>
    inline Expr &operator[](Index &&index)
    {
        return (**this)[std::forward<Index>(index)];
    }

    template <class Index>
    inline Expr const &operator[](Index &&index) const
    {
        return (**this)[std::forward<Index>(index)];
    }

    Expr &operator+=(Expr const &other)
    {
        return (*this = *this + other);
    }

    Expr &operator-=(Expr const &other)
    {
        return (*this = *this - other);
    }

    Expr &operator*=(Expr const &other)
    {
        return (*this = *this * other);
    }

    Expr &operator/=(Expr const &other)
    {
        return (*this = *this / other);
    }
};

DEFINE_SHARED_PTR_OPERATOR(Expr)

std::ostream &operator<<(std::ostream &fout, const Expr &obj);

///////////////////////////////////////////////////
/*************************************************/
// Redefinition of operator for Expr alias       //
// std::shared_ptr<Abstract>. Allows then to use //
// operator with both syntaxes:                  //
// (*a op b) AND (a op b) with a and b Expr.     //
/*************************************************/
///////////////////////////////////////////////////

/*! \brief see Abstract::operator==()
 */
bool operator==(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator!=()
 */
bool operator!=(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator>=()
 */
bool operator>=(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator<=()
 */
bool operator<=(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator>()
 */
bool operator>(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator<()
 */
bool operator<(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator|=()
 */
bool operator|=(const Expr &a, const Expr &b);

/*! \brief see Abstract::operator&=()
 */
bool operator&=(const Expr &a, const Expr &b);

inline Expr Abstract::copy() const
{
    return copy_unique();
}

inline bool Abstract::operator==(const Expr_c &expr) const
{
    return *this == expr.get();
}

inline bool Abstract::operator==(const Expr &expr) const
{
    return *this == expr.get();
}

inline bool Abstract::operator==(const Abstract &other) const
{
    return *this == other.copy();
}

inline bool Abstract::operator!=(Expr_info expr) const
{
    return not(*this == expr);
}

/*! \brief \b Compares the Abstract with another.
 * \param expr Abstract to compare.
 * \return False if the two Abstracts are the same (or have the same name).
 * \return True else.
 */
inline bool Abstract::operator!=(const Expr_c &expr) const
{
    return *this != expr.get();
}

inline bool Abstract::operator!=(const Expr &expr) const
{
    return *this != expr.get();
}

inline bool Abstract::operator!=(const Abstract &other) const
{
    return *this != other.copy();
}
} // End of namespace csl

#endif
