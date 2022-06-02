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
 * @version 2.0

 * \brief
 */
#ifndef LITERAL_H_INCLUDED
#define LITERAL_H_INCLUDED

#include "buildingBlock.h"
#include "variableParent.h"

namespace csl {

/*!
 * \brief Abstract class from which derive literal building blocks:
 * Constant, Variable, Imaginary and IntFactorial.
 */
class AbstractLiteral : public Complexified {

  protected:
    /*! Pointers to the specific properties of the object. */
    std::vector<Equation *> props;

  public:
    AbstractLiteral();

    csl::PrimaryType getPrimaryType() const override
    {
        return csl::PrimaryType::Literal;
    }

    size_t memoryOverhead() const override;

    std::vector<Parent> getSubSymbols() const override;

    const std::vector<Equation *> &getProperties() const override;

    void addProperty(Equation *property) override;

    void removeProperty(Equation *property) override;
};

inline AbstractLiteral::AbstractLiteral()
    : Complexified(), props(std::vector<Equation *>(0))
{
}

/*!
 * \brief Handle an object with a \b name and a \b value.
 */
class Constant : public AbstractLiteral {

  protected:
    std::shared_ptr<ConstantParent> parent;

  public:
    /*! \brief Default constructor.
     * \details Initializes the Constant with no \b name and value=0.
     */
    Constant(const std::shared_ptr<ConstantParent> &t_parent);

#ifdef DEBUG
    ~Constant()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    std::string const &getName() const override;

    std::string const &getLatexName() const override;

    bool getCommutable() const override;

    /*! \brief Gives the \b primary \b type of a Constant.
     * \details The primaryType of Constant is 1, it concerns a scalar object
     * not function of others and that is real-valued.
     * \return \b 1
     */
    csl::PrimaryType getPrimaryType() const override;

    /*! \brief Gives the \b type of a Constant.
     * \return \b 1
     */
    csl::Type getType() const override;

    Parent getParent() const override;

    Parent_info getParent_info() const override;

    csl::ComplexProperty getComplexProperty() const override;

    void setComplexProperty(csl::ComplexProperty prop) override;

    void setName(const std::string &name) override;

    void setCommutable(bool commutable) override;

    bool getValued() const override;

    long double getValue() const override;

    bool dependsOn(Expr_info expr) const override;

    /*! \brief Sets the \b value.
     * \details Allows to associate a number to each Constant
     * before evaluating an expr.
     * \param t_value New \b value for the Constant.
     */
    void setValue(long double t_value) override;

    void setValue(Expr const &t_value) override;

    /*! \brief Displays the Constant on standard output.
     * \details If mode==0 prints the Constant alone with its \b value, else
     * prints the Constant considering it in a larger expr.
     * \param mode Type of printing.
     */
    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    std::vector<Parent> getSubSymbols() const override;

    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    /*! \brief Derives the Constant wrt \b expr.
     * \param expr Abstract
     * \return \b 1 if \b expr is a Constant with the **same name**.
     * \return \b 0 else.
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    unique_Expr copy_unique() const override;

    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(double t_value) override;

    /*! \brief Compares \b value with another Abstract.
     * \param expr Abstract to compare.
     * \return \b 1 if expr is a Constant with the **same name**.
     * \return \b 0 else.
     */
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;
};

/*!
 * \brief Handle an object with a \b name and a \b value.
 */
class Variable : public AbstractLiteral {

  protected:
    std::shared_ptr<VariableParent> parent;

  public:
    /*! \brief Default constructor.
     * \details Initializes the Variable with no \b name and value=0.
     */
    Variable(const std::shared_ptr<VariableParent> &t_parent);

#ifdef DEBUG
    ~Variable()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the \b primary \b type of a Variable.
     * \details The primaryType of Variable is 1, it concerns a scalar object
     * not function of others and that is real-valued.
     * \return \b 1
     */
    csl::PrimaryType getPrimaryType() const override;

    /*! \brief Gives the \b type of a Variable.
     * \return \b 1
     */
    csl::Type getType() const override;

    std::string const &getName() const override;

    std::string const &getLatexName() const override;

    bool getCommutable() const override;

    csl::ComplexProperty getComplexProperty() const override;

    void setComplexProperty(csl::ComplexProperty prop) override;

    void setName(const std::string &name) override;

    void setCommutable(bool commutable) override;

    Parent getParent() const override;

    Parent_info getParent_info() const override;

    bool getElementary() const override;

    bool getAllDependencies() const override;

    bool getValued() const override;

    long double getValue() const override;

    bool dependsOn(Expr_info expr) const override;

    bool commutesWith(Expr_info expr, int sign = -1) const override;

    /*! \brief Sets the \b value.
     * \details Allows to associate a number to each Variable
     * before evaluating an expr.
     * \param t_value New \b value for the Variable.
     */
    void setValue(long double t_value) override;

    void setValue(Expr const &t_value) override;

    void setElementary(bool t_elementary) override;

    void setAllDependencies(bool t_allDependencies) override;

    void addDependency(Expr const &expr) override;

    void removeDependency(Expr const &expr) override;

    /*! \brief Displays the Variable on standard output.
     * \details If mode==0 prints the Variable alone with its \b value, else
     * prints the Variable considering it in a larger expr.
     * \param mode Type of printing.
     */
    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    std::vector<Parent> getSubSymbols() const override;

    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    /*! \brief Derives the Variable wrt \b expr.
     * \param expr Abstract
     * \return \b 1 if \b expr is a Variable with the **same name**.
     * \return \b 0 else.
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    unique_Expr copy_unique() const override;

    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(double t_value) override;

    /*! \brief Compares \b value with another Abstract.
     * \param expr Abstract to compare.
     * \return \b 1 if expr is a Variable with the **same name**.
     * \return \b 0 else.
     */
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;
};

/*!
 * \brief Handles factorial of a Number.
 * \details This class allows to manipulate very big numbers without
 * evaluating them first, for example in **Taylor expansions**.
 */
class IntFactorial : public AbstractLiteral {

  protected:
    int value; /*!< Integer value from which we take the factorial. */

  public:
    /*! \brief Default Constructor.
     * \details Initializes \b value to 0, then the expr evaluates to 0!=1.
     */
    IntFactorial();

    /*!
     * \brief Initializes \b value to \a t_value.
     * \param t_value integer.
     */
    explicit IntFactorial(int t_value);

#ifdef DEBUG
    ~IntFactorial()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Returns the \b value in the factorial.
     * \details Caution with this function: it returns \b value and (except
     * for the special value 1) it is **different from the return \b value of
     * evaluateScalar()**.
     * \return \b value
     */
    long double getValue() const override
    {
        return value;
    }

    /*! \brief Gives the **primary type** of a IntFraction.
     * \details The **primary type** is the same as the one of Variable.
     * \return \b 1
     */
    csl::PrimaryType getPrimaryType() const override
    {
        return csl::PrimaryType::Literal;
    };

    /*! \brief Gives the \b type of a IntFraction.
     * \return \b 3
     */
    csl::Type getType() const override
    {
        return csl::Type::IntFactorial;
    };

    /*! \brief Sets the \b value from which we want the \b factorial.
     * \param t_value
     */
    void setValue(long double t_value) override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    /*! \brief Returns the **evaluation of the factorial**.
     * \return **value!**
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    /*! \brief Derives the IntFactorial wrt expr.
     * \param expr Argument of the derivative.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(int t_value);
    /*! Compares value with another Abstract
     * \param expr Abstract
     * \return 1 if expr is a IntFactorial with the same value
     * \return 0 else
     */
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;
};

/*!
 * \brief Numerical representation of \b i.
 * \details This class should not be used by the user. An instance **I** is
 * statically defined below and is used in all the program.
 */
class Imaginary : public AbstractLiteral {

  public:
    /*! \brief Default constructor.
     * \details Initializes the \b name to "Imaginary" and \b value
     * to 0, even if it should not be used: a warning message is printed if so.
     */
    Imaginary();

#ifdef DEBUG
    ~Imaginary()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! Gives the **primary type** of the Imaginary.
     * \return \b 1
     */
    csl::PrimaryType getPrimaryType() const override
    {
        return csl::PrimaryType::Literal;
    }

    /*! \brief Gives the \b type of the Imaginary.
     * \return \b 4
     */
    csl::Type getType() const override
    {
        return csl::Type::Imaginary;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    csl::ComplexProperty getComplexProperty() const override
    {
        return csl::ComplexProperty::Imaginary;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    /*! \brief \b Tries to evaluate the Imaginary **as a real**.
     * \details This function returns \b 0 and prints a \b warning message
     * because \b i should not be interpreted as a real in a calculation.
     * \return \b 0
     */
    long double evaluateScalar() const override;

    /*! \brief \b Tries to evaluate the Imaginary.
     * \details This function should return the object itself since we cannot
     * further evaluate \b i. Because of the shared_ptr machinery this object
     * cannot do that and the functions returns \b nullptr. We then need the
     * use of the Recover() function to keep a valid expr. \return \b nullptr
     */
    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    /*! \brief Derives the Imaginary wrt \b expr.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;
};

/*************************************************/
// Inline functions (non virtual and short)      //
/*************************************************/
inline Variable::Variable(const std::shared_ptr<VariableParent> &t_parent)
    : AbstractLiteral(), parent(t_parent)
{
}

inline Constant::Constant(const std::shared_ptr<ConstantParent> &t_parent)
    : AbstractLiteral(), parent(t_parent)
{
}

inline IntFactorial::IntFactorial() : AbstractLiteral(), value(0)
{
}

inline IntFactorial::IntFactorial(int t_value)
    : AbstractLiteral(), value(t_value)
{
}

inline Imaginary::Imaginary() : AbstractLiteral()
{
}

/*! \fn Expr intfactorial_s(int value)
 * \param value \b Initializer of the IntFactorial.
 * \return IntFactorial(value) if value > 2
 * \return Number(value) else
 * \note This function can be used to return a Symbol. The Symbol constructor
 * Symbol::Symbol(const Expr&) is implemented and allows to
 * put the return value of this function in a Symbol.
 */
Expr intfactorial_s(int value);

Expr constant_s(std::string const   &name,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

Expr constant_s(std::string const   &name,
                long double          value,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

Expr constant_s(std::string const   &name,
                Expr const          &value,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

Expr variable_s(std::string const   &name,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

Expr variable_s(std::string const   &name,
                long double          value,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

Expr variable_s(std::string const   &name,
                Expr const          &value,
                csl::ComplexProperty prop = csl::ComplexProperty::Real);

/*************************************************/
// Global (useful) variables                     //
// (Numerical or Literal)                        //
/*************************************************/

inline csl::allocator<Constant> alloc_constant;
inline csl::allocator<Variable> alloc_variable;

} // namespace csl

/*!
 * \var I
 * \brief Represents the imaginary unit \b i in the program.
 */
inline const csl::Expr CSL_I = csl::make_shared<csl::Imaginary>();

/*!
 * \var PI
 * \brief Represents the number \b pi to include in expressions.
 */
inline const csl::Expr CSL_PI
    = csl::constant_s("\\pi", M_PI, csl::ComplexProperty::Real);

/*!
 * \var E
 * \brief Represents the number \b e to include in expressions.
 */
inline const csl::Expr CSL_E
    = csl::constant_s("e", M_E, csl::ComplexProperty::Real);

/*!
 * \varCSL_INF
 * \brief Represents \b infinity in the program.
 * \details When an infinity appears in the abstract evaluation the program
 * should return \bCSL_INF. \bCSL_INF is treated like a Variable. It is simply
 * here to inform the user that *something bad happened** in the expr. \bug
 * Will not work for many cases, in particular log(0)!=\bCSL_INF. Not yet
 * implemented.
 */
inline const csl::Expr CSL_INF
    = csl::constant_s("INF", csl::ComplexProperty::Real);

/*! \var CSL_UNDEF
 * \brief Constant that is useful in return of non-defined calculations
 * as for example Commutator(): [A,B] returns CSL_UNDEF when it is not
 * defined (more than AB-BA). It allows the program to avoid circular
 * calculations and seg fault.
 */
inline const csl::Expr CSL_UNDEF
    = csl::constant_s("CSL_UNDEF", csl::ComplexProperty::Real);

#endif
