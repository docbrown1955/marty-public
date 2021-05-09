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
#ifndef COMPARISON_H_INCLUDED
#define COMPARISON_H_INCLUDED

#include "abstract.h"
#include "literal.h"
#include "numerical.h"

namespace csl {

/*! 
 * \brief Arbitrary object that can enter mathematical expressions.
 * \details When comparing two expressions, we sometimes want to know if 
 * a certain expression respects a certain form or not, independently of
 * the exact sub-expressions.\n
 * \f$ i*X*Arbitrary == i*X*P \f$.\n 
 * Arbitrary objects are indexed by a number that distinguish them. An Arbitrary
 * object will return true when compared to any expression the first time, and 
 * then will be associated with thisa expression to the end of the comparison.
 * Ex:\n
 * \f$ x + y*z != A1 + A2*A1\f$.\n
 * \f$ x + y*x = A1 + A2*A1\f$.\n
 * Where A stands for Arbitrary objects. The first equality is not true 
 * because once \f$A1\f$ is associated to \f$x\f$ it cannot be equal to 
 * \f$ z \f$.
 */
class Arbitrary: public AbstractLiteral{

    public:

    /* \brief Constructor.
     * \param n Index of the Arbitrary object, will determine its identity
     * in comparisons.
     * \param t_type Specific type that can be set. If given, the Arbitrary will
     * be compared to only that type of expressions.
     */
    Arbitrary(int n, csl::Type t_type=csl::Type::NoType);

    Arbitrary(const Arbitrary& other) = default;

    /*! \brief Destructor*/
    ~Arbitrary(){};

    std::string const &getName() const override;

    bool isArbitrary() const override;

    csl::PrimaryType getPrimaryType() const override;

    csl::Type getType() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode=0) const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    long long int getNum() const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;


    private:

    /*! \brief Number of the Arbitrary, determines its identity in comparisons.
     */
    int number;

    std::string name;

    /*! \brief Specific type of the Arbitrary. If it is specified in the 
     * constructor, the object will be able to be equal to expressions of type
     * \b type.
     */
    csl::Type type;
};

/*! 
 * \brief Bunch of functions that allow to do comparisons with Arbitrary 
 * expressions or expressions with dummy objects (summed indices or variables).
 * \detail This is a static class, in the sense that all methods and attributes
 * are static i.e. independant of any isntance of the class. There is then no
 * Comparator object constructed, the class just allows to centralize all
 * variables and functions useful for those comparisons.
 */
class Comparator{

    friend class Arbitrary;
    // Member functions

    public:

    /*! Generate and returns an Arbitrary object.
     * \param n Number of the generated Arbitrary.
     * \param type Type of the generated Arbitrary.
     * \return The generated Arbitrary.
     */
    static Expr dummy(int n, csl::Type type=csl::Type::NoType);

    /*! \brief Compares a regular expression to an expression containing
     * Arbitrary objects.
     * \param expr Regular expression, in the form of a **const Abstract*** so
     * that it can be called by an object with itself in parameter in a const 
     * method.
     * \param dummyExpr Expression containing Arbitrary objects.
     * \return The result of the comparison.
     */
    static bool dummyComparison(Expr_info expr, Expr_info dummyExpr);

    static bool dummyComparison(Expr const& expr, Expr const& dummyExpr);

    static bool dummyVecIntComparison(Expr_info A,
                                      Expr_info B);

    /*! \return freeIndexComparisonActive.
     */
    static bool getFreeIndexComparisonActive() {
        return freeIndexComparisonActive;
    }

    static void setFreeIndexComparisonActive(bool t_activ);

    /*! \return The string that is given as a name to all Arbitrary objects in
     * the program.
     */
    static std::string dummyName();

    /*! \brief Compares two expressions considering that the index structures
     * must match no matter the names of free indices.
     * \param A First expression in the form of a **const Abstract*** so
     * that it can be called by an object with itself in parameter in a const 
     * method.
     * \param B Second expression.
     */
    static bool freeIndexComparison(const Abstract* A, Expr_info B);

    /*! \brief Compares two indices. Depends on the bare comparison and the 
     * possible maps existing for the two indices.
     * \param A First index.
     * \param B Second index.
     * \return The result of the comparison.
     */
    static bool compare(const Index& A, const Index& B);

    /*! \brief Clears all maps and correspondances.
     */
    static void clear();

    /*! \return dummyComparisonActive.
     */
    static bool getDummyComparisonActive() {
        return dummyComparisonActive;
    }

    static bool getDummyVecIntComparisonActive() {
        return dummyVecIntComparisonActive;
    }

    /*! \param state Boolean that will replace dummyComparisonActive.
     */
    static void setDummyComparisonActive(bool state);

    static void setDummyVecIntComparisonActive(bool state);

    private:

    /*! \brief Comapres \b expr and \b dummy.
     * \param expr Regular expression.
     * \param dummy constExpr because this function is called by \b itself that
     * send a shared_from_this() to the function.
     * \return The result of the comparison.
     */
    static bool compare(Expr_info expr, Expr_info dummy);
    
    // Static Attributes

    public:

    /*! \brief Tells if the index comparison is active or not.
     * \detail If true, two different indices (even free indices) can be equal.
     * This is useful for comparing structures, independently of the names of 
     * the indices.
     */
    static bool freeIndexComparisonActive;

    /*! \brief Correspondance between indices during the comparison. Allows to
     * keep track of all index comparisons we have done when the indices were 
     * not equal.
     */
    static std::map<Index,Index> indexCorrespondance;

    static bool dummyVecIntComparisonActive;

    private:

    /*! \brief Tells if the comparison with arbitries is active or not.
     */
    static bool dummyComparisonActive;

    /*! \brief Maps an integer to the corresponding Arbitrary. Allows to get 
     * quickly the Arbitrary object that has a certain integer as number.
     */
    static std::map<int,Expr> arbitrary;

    /*! \brief Map from integers (number attribute of arbitraries) to
     * expressions.
     * \details When an Arbitrary is successfully compared to an expression,
     * an element is added to this map and it will then only answer true when 
     * compared to this expression.
     */
    static std::map<int,Expr> correspondance;
};

} // End of namespace csl

#endif
