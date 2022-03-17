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

/*! \file scalarFunc.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Base classes for scalar functions.
 */
#ifndef SCALARFUNC_H_INCLUDED
#define SCALARFUNC_H_INCLUDED
#include "literal.h"
#include "numerical.h"
#include "std_vector_implementation.h"

namespace csl {

class Tensor;

/*! 
 * \brief Base class for **scalar functions of one argument.**
 * \details From this class will derive functions with a **unique argument**.
 * In particular all mathematical functions will be derived classes. In the case of 
 * the call of a simplification function, if recursive, the function will be first
 * applied on the \b argument, then to the function itself.
 */
class AbstractFunc: public Abstract{

    protected:

    Expr argument; /*!< \brief Argument of the considered function. */

    public:

    /*! \brief Default constructor.
     * \details Initializes the argument to \b 0 (Number).
     */
    AbstractFunc();

    explicit AbstractFunc(const Expr& t_argument);

    /*! Gives the **primary type** of an AbstractFunc.
     * \return \b 20
     */
    csl::PrimaryType getPrimaryType() const override{
        return csl::PrimaryType::ScalarFunction;
    }

    /*! \brief Tells if the AbstractFunc is commutable.
     * \details The commutability of a function depends on the one of its \b 
     * argument.
     * \return \b True if the AbstractFunc is commutable.
     * \return \b False else.
     */
    bool getCommutable() const override;

    std::vector<csl::Parent> getSubSymbols() const override;

    bool isIndexed() const override;

    size_t size() const override;

    bool compareWithDummy(Expr_info expr,
                          std::map<Index, Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    IndexStructure getIndexStructure() const override;

    Tensor getPoint() const override;

    int getNArgs(int axis=0) const override;

    /*! \brief Returns the \b argument of the function.
     * \details The parameter \a iArg is usefull for AbstractMultiFunc. 
     * It is present ere in order to have only one access function for 
     * arguments in the Abstract class.
     * \param iArg Index of the argument, silent here.
     * \return \b argument
     */
    Expr const &getArgument(int iArg=0) const override;

    Expr& getArgument(int iArg=0) override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    std::optional<Expr> getComplexConjugate() const override;

    std::optional<Expr> findSubExpression(
            Expr_info   subExpression,
            const Expr& newExpression) const override;

    /*! \brief Replaced the \b argument of the AbstractFunc.
     * \details The parameter \a iArg is usefull for AbstractMultiFunc. 
     * It is present ere in order to have only one access function for 
     * arguments in the Abstract class.
     * \param t_argument New \b argument.
     * \param iArg       Index of the \b argument, silent here.
     */
    void setArgument(const Expr& t_argument, int iArg=0) override;

    virtual std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    virtual std::optional<Expr> replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh = true,
            bool         flipped = false) const override;

    virtual std::optional<Expr> factor(bool full=false) const override;

    virtual std::optional<Expr> factor(
            Expr_info expr,
            bool      full=false) const override;

    virtual std::optional<Expr> collect(
            std::vector<Expr> const &factors,
            bool                     full = false
            ) const override;

    std::optional<Expr> expand(bool full=false,
                               bool inPlace=false) const override;

    /*! \brief \b Develops the Abstract.
     * \details This function concerns only products (and exponents) that will be
     * flatten to give at the end a sum of independant terms.
     * \param f    Functions that returns a boolean that determines which 
     * arguments must be expanded in products.
     * \param full If true the expandment is \b recursive through all the Abstract.
     * \return The expand Abstract.
     */
    virtual std::optional<Expr> expand_if(
            std::function<bool(Expr const&)> const& f,
            bool full=false,
            bool inPlace=false) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(const AbstractParent* parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Parent_info expr) const override;
    
    bool commutesWith(Expr_info expr, int sign=-1) const override;

    int isPolynomial(Expr_info expr) const override;

    /*! \brief \b Evaluates the function after evaluating the \b argument.
     * \details Except particular cases (cos(0), exp(0) etc) this function returns
     * the same AbstractFunc with \b argument.evaluate() as argument.
     * \return The function of the evaluated \b argument.
     */
    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base) const override;

    Expr const &operator[](int i) const override;

    Expr& operator[](int i) override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    Expr evalNumerical(Expr const& argument) const;
};


/*! 
 * \brief Handle functions of \b multiple arguments. In the case of 
 * the call of a simplification function, if recursive, the function will be first
 * applied on all the \b arguments, then to the function itself.
 */
class AbstractDuoFunc: public Abstract{

    protected:

    std::array<Expr,2> argument; /*!< \brief std::array containing the arguments. */

    public:

    /*! \brief Default Constructor, initializes the std::array argument empty
     */
    AbstractDuoFunc();

    AbstractDuoFunc(const Expr& left,
                    const Expr& right);

    csl::Type getType() const override {
        return csl::Type::StandardDuo;
    }
    /*! \brief Gives the **primary type** of an AbstractDuoFunc.
     * \return 10
     */
    csl::PrimaryType getPrimaryType() const override{
        return csl::PrimaryType::MultiFunction;
    }

    /*! \brief Tells if the AbstractDuoFunc is commutable.
     * \details The commutability of a multi-function depends on the one of its 
     * \b arguments and the properties of the multi-function itself.
     * \return \b True if the AbstractFunc is commutable.
     * \return \b False else.
     */
    bool getCommutable() const override;

    std::vector<csl::Parent> getSubSymbols() const override;

    bool isIndexed() const override;

    IndexStructure getIndexStructure() const override;

    Tensor getPoint() const override;

    /*! \brief Returns the **number of arguments** of the AbstractDuoFunc.
     * \return \b nArgs
     */
    int getNArgs(int axis=0) const override;

    size_t size() const override {
        return 2;
    }

    /*! \brief Returns one particular \b argument.
     * \param iArg(default=0) Index of the argument to return in the std::array 
     * \b argument.
     * \return \b argument[iArg]
     */
    Expr const &getArgument(int iArg=0) const override;

    Expr& getArgument(int iArg=0) override;

    std::optional<Expr> findSubExpression(
            Expr_info   subExpression,
            const Expr& newExpression) const override;

    /*! \brief Replaces one particular \b argument.
     * \param t_argument      New argument.
     * \param iArg(default=0) Index of the argument to replace with t_argument.
     */
    void setArgument(const Expr& t_argument, int iArg=0) override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    std::optional<Expr> getComplexConjugate() const override;

    virtual std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    virtual std::optional<Expr> replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh = true,
            bool         flipped = false) const override;

    virtual std::optional<Expr> factor(bool full=false) const override;

    virtual std::optional<Expr> factor(
            Expr_info expr,
            bool      full=false) const override;

    virtual std::optional<Expr> collect(
            std::vector<Expr> const &factors,
            bool                     full = false
            ) const override;

    virtual std::optional<Expr> expand(bool full=false,
                                       bool inPlace=false) const override;

    /*! \brief \b Develops the Abstract.
     * \details This function concerns only products (and exponents) that will be
     * flatten to give at the end a sum of independant terms.
     * \param f    Functions that returns a boolean that determines which 
     * arguments must be expanded in products.
     * \param full If true the expandment is \b recursive through all the Abstract.
     * \return The expand Abstract.
     */
    virtual std::optional<Expr> expand_if(
            std::function<bool(Expr const&)> const& f,
            bool full=false,
            bool inPlace=false) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(Parent_info parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Parent_info expr) const override;
    
    bool commutesWith(Expr_info expr, int sign=-1) const override;

    int isPolynomial(Expr_info expr) const override;

    Expr const &operator[](int i) const override;

    Expr& operator[](int i) override;

    bool operator<(Expr_info other) const override;
};


/*! 
 * \brief Handle functions of \b multiple arguments. In the case of 
 * the call of a simplification function, if recursive, the function will be 
 * first applied on all the \b arguments, then to the function itself.
 */
class AbstractMultiFunc: public Abstract{

    protected:

    csl::vector_expr argument; /*!< \brief std::vector containing the arguments. */

    public:

    /*! \brief Default Constructor, initializes the std::vector argument empty
     */
    AbstractMultiFunc();

    AbstractMultiFunc(std::vector<csl::Expr> const &t_argument);

    csl::Type getType() const override {
        return csl::Type::StandardMult;
    }

    /*! \brief Gives the **primary type** of an AbstractMultiFunc.
     * \return 10
     */
    virtual csl::PrimaryType getPrimaryType() const override {
        return csl::PrimaryType::MultiFunction;
    }

    /*! \brief Tells if the AbstractMultiFunc is commutable.
     * \details The commutability of a multi-function depends on the one of 
     * its \b arguments and the properties of the multi-function itself.
     * \return \b True if the AbstractFunc is commutable.
     * \return \b False else.
     */
    bool getCommutable() const override;

    std::vector<csl::Parent> getSubSymbols() const override;

    virtual bool isIndexed() const override;

    /*! \brief Returns the **number of arguments** of the AbstractMultiFunc.
     * \return \b nArgs
     */
    int getNArgs(int axis=0) const override;

    /*! \brief Returns the std::vector \b argument directly.
     * \return \b argument
     */
    const csl::vector_expr& getVectorArgument() const override;

    /*! \brief Returns one particular \b argument.
     * \param iArg(default=0) Index of the argument to return in the 
     * std::vector \b argument.
     * \return \b argument[iArg]
     */
    Expr const &getArgument(int iArg=0) const override;

    Expr& getArgument(int iArg=0) override;

    std::optional<Expr> findSubExpression(
            Expr_info   subExpression,
            const Expr& newExpression) const override;

    size_t size() const override;

    bool empty() const override;

    /*! \return A csl::vector_expr::iterator at the beginning of argument
     * for multi-argument expressions.
     */    
    csl::vector_expr::iterator begin() override;

    /*! \return A csl::vector_expr::iterator at the end of argument
     * for multi-argument expressions.
     */    
    csl::vector_expr::iterator end() override;

    /*! \return A csl::vector_expr::iterator at the beginning of argument
     * for multi-argument expressions.
     */    
    csl::vector_expr::const_iterator begin() const override;

    /*! \return A csl::vector_expr::iterator at the end of argument
     * for multi-argument expressions.
     */    
    csl::vector_expr::const_iterator end() const override;

    /*! \brief Replaces one particular \b argument.
     * \param t_argument      New argument.
     * \param iArg(default=0) Index of the argument to replace with t_argument.
     */
    void setArgument(const Expr& t_argument, int iArg=0) override;

    void setVectorArgument(const csl::vector_expr& t_argument) override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    std::optional<Expr> getComplexConjugate() const override;

    virtual std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    virtual std::optional<Expr> replaceIndices(
            std::vector<csl::Index> const &oldIndices,
            std::vector<csl::Index> const &newIndices,
            bool         refresh = true,
            bool         flipped = false) const override;

    Tensor getPoint() const override;

    virtual std::optional<Expr> factor(bool full=false) const override;

    virtual std::optional<Expr> factor(Expr_info expr,
                                       bool      full=false) const override;

    virtual std::optional<Expr> collect(
            std::vector<Expr> const &factors,
            bool                     full = false
            ) const override;

    virtual std::optional<Expr> expand(bool full=false,
                                       bool inPlace=false) const override;

    /*! \brief \b Develops the Abstract.
     * \details This function concerns only products (and exponents) that will be
     * flatten to give at the end a sum of independant terms.
     * \param f    Functions that returns a boolean that determines which 
     * arguments must be expanded in products.
     * \param full If true the expandment is \b recursive through all the Abstract.
     * \return The expand Abstract.
     */
    virtual std::optional<Expr> expand_if(
            std::function<bool(Expr const&)> const& f,
            bool full=false,
            bool inPlace=false) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(const AbstractParent* parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Parent_info expr) const override;
    
    bool commutesWith(Expr_info expr, int sign=-1) const override;

    int isPolynomial(Expr_info expr) const override;

    bool operator<(Expr_info other) const override;

    Expr const &operator[](int i) const override;

    Expr& operator[](int i) override;
};

/*************************************************/
// Inline functions                              //
/*************************************************/
inline AbstractFunc::AbstractFunc(): Abstract(), argument(CSL_0){}
inline AbstractFunc::AbstractFunc(const Expr& t_argument)
    :Abstract(), argument(t_argument)
{}

inline AbstractDuoFunc::AbstractDuoFunc(): Abstract(){
    argument[0] = CSL_0;
    argument[1] = CSL_0;
}

inline AbstractDuoFunc::AbstractDuoFunc(
        const Expr& left,
        const Expr& right
        ): Abstract(){
    argument[0] = left;
    argument[1] = right;
}

inline AbstractMultiFunc::AbstractMultiFunc()
    :Abstract(), argument(csl::vector_expr(alloc_expr))
{

}

inline AbstractMultiFunc::AbstractMultiFunc(
    std::vector<csl::Expr> const &t_argument)
    :Abstract(), argument(t_argument)
{

}

} // End of namespace csl

#endif
