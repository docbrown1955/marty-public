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
 * @file numerical.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#ifndef NUMERICAL_H_INCLUDED
#define NUMERICAL_H_INCLUDED

#include "buildingBlock.h"

namespace csl {

/*! 
 * \brief Abstract class from which derive numerical types, i.e. 
 * Float, Integer, IntFraction.
 */
class AbstractNumerical: public AbstractBuildingBlock{
    public:

    AbstractNumerical();

    csl::PrimaryType getPrimaryType() const override {
        return csl::PrimaryType::Numerical;
    }

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::vector<Parent> getSubSymbols() const override;

    Expr getNumericalFactor() const override;
    std::optional<Expr> getTerm() const override;

    bool dependsOn(Expr_info) const override {
        return false;
    }
    bool dependsOn(Parent_info) const override {
        return false;
    }
    bool dependsExplicitlyOn(Expr_info) const override {
        return false;
    }
    bool dependsExplicitlyOn(Parent_info) const override {
        return false;
    }
};

inline AbstractNumerical::AbstractNumerical(): AbstractBuildingBlock(){}


/*************************************************/
// Numerical classes                             //
/*************************************************/

/*! 
 * \brief Handle numbers in expr
 * \details Handle the same manner for now double or int values (in a double).
 */
class Integer: public AbstractNumerical{

    protected:

    long long int value; /*!< \brief Value of the number. */

    public:

    /*! \brief Default constructor.
     * \details Initialize \b value to 0.
     */
    Integer();

    /*! \details Initialize a Integer with the \b value \a t_value.
     * \param t_value double.
     */
    explicit Integer(long long int t_value);

#ifdef DEBUG
    ~Integer(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Returns the \b type corresponding to an object Integer.
     * \return 0
     */
    csl::Type getType() const override;

    bool isInteger() const override { return true;}

    /*! \brief Sets the \b value of the Integer.
     * \param t_value New \b value for the Integer.
     */
    void setValue(long double t_value) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode=0) const override;

    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    /*! \brief Multiplicates two pure Numbers.
     * \details If \a expr is not a Number, returns 0. Else returns a 
     * Number which value is equal to the product of \b value and expr.value.
     * \param expr Other Number for the multiplication.
     * \return **value*expr.evaluateScalar()**.
     */
    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    /*! \brief Adds two pure Numbers.
     * \details If \a expr is not a Number, returns 0. Else returns a 
     * Number which value is equal to the sum of \b value and expr.value.
     * \param expr Other Number for the sum.
     * \return **value+expr.evaluateScalar()**.
     */
    Expr addition_own(const Expr& expr) const override;

    Expr division_own(const Expr& expr) const override;

    Expr exponentiation_own(const Expr& expr) const override;

    /*! \brief Derived the Number.
     * \details The derivative of a Number is \b always 0.
     * \param expr Argument of the derivation.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(long long int t_value);
   
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};


/*! 
 * \brief Handle numbers in expr
 * \details Handle the same manner for now double or int values (in a double).
 */
class Float: public AbstractNumerical{

    protected:

    long double value; /*!< \brief Value of the number. */

    public:

    /*! \brief Default constructor.
    expanded = CSL_0;
     * \details Initialize \b value to 0.
     */
    Float();

    /*! \details Initialize a Float with the \b value \a t_value.
     * \param t_value double.
     */
    explicit Float(long double t_value);

#ifdef DEBUG
    ~Float(){ __record_data_alloc(static_cast<int>(getType())); }
#endif


    /*! \brief Returns the \b type corresponding to an object Float.
     * \return 0
     */
    csl::Type getType() const override;

    bool isInteger() const override;

    /*! \brief Sets the \b value of the Float.
     * \param t_value New \b value for the Float.
     */
    void setValue(long double t_value) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode=0) const override;

    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    /*! \brief Multiplicates two numbers.
     * \details If \a expr is not a Number, returns 0. Else returns a 
     * Number which value is equal to the product of \b value and expr.value.
     * \param expr Other Number for the multiplication.
     * \return **value*expr.evaluateScalar()**.
     */
    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    /*! \brief Adds two pure Numbers.
     * \details If \a expr is not a Number, returns 0. Else returns a 
     * Number which value is equal to the sum of \b value and expr.value.
     * \param expr Other Number for the sum.
     * \return **value+expr.evaluateScalar()**.
     */
    Expr addition_own(const Expr& expr) const override;

    Expr division_own(const Expr& expr) const override;

    Expr exponentiation_own(const Expr& expr) const override;

    /*! \brief Derives the Float.
     * \details The derivative of a Float is \b always 0.
     * \param expr Argument of the derivation.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(int t_value);
   
    /*! \brief Sets value to t_value
     * \param t_value
     */
    void operator=(double t_value) override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

/*!
 * \brief Handles \b fractions of Number.
 * \details This class take care of fractionnal arithmetics i.e. reduction
 * of fractions by calculating the PGCD, finding common denominator etc.
 */
class IntFraction: public AbstractNumerical{

    protected:

    long long int num; /*!< Numerator. */
    long long int denom; /*!< Denominator. */

    public:

    /*! \brief Default Constructor.
     * \details Initializes \b num = 0 and \b denom = 1.
     */
    IntFraction();

    /*! \details Initializes \b num = \a t_num and \b denom = 1.
     * \param t_num Numerator.
     */
    explicit IntFraction(long long int t_num);

    /*! \details Initializes both the \b numerator and the \b denominator.
     * \param t_num Numerator.
     * \param t_denom Denominator.
     */
    IntFraction(long long int t_num, long long int t_denom);

    IntFraction(IntFraction const&) = default;

#ifdef DEBUG
    ~IntFraction(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the \b type of a IntFraction.
     * \return 2
     */
    csl::Type getType() const override;

    /*! \brief Returns the numerator.
     * \return \b num
     */
    long long int getNum() const override;

    /*! \brief Return the denominator.
     * \return \b denom
     */
    long long int getDenom() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    /*! \brief Evaluates the IntFraction.
     * \return double(\b num/\b denom)
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    /*! \brief \b Multiplicates the IntFraction with a Number or a IntFraction.
     * \details This function is specialy designed for the multiplication of 
     * numbers. It shouldn't be used directly by the user in general, because
     * more general product functions will \b automatically call this one when needed.
     * \param expr Number or IntFraction for the product.
     * \return The product.
     */
    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    /*! \brief \b Adds the IntFraction with a Number or a IntFraction.
     * \details This function is specialy designed for the multiplication of 
     * numbers. It shouldn't be used directly by the user in general, because
     * more general sum functions will \b automatically call this one when needed.
     * \param expr Number or IntFraction for the sum.
     * \return The sum.
     */
    Expr addition_own(const Expr& expr) const override;

    Expr division_own(const Expr& expr) const override;

    Expr exponentiation_own(const Expr& expr) const override;

    /*! \brief Derives the IntFraction wrt the argument.
     * \param expr Argument of the derivation.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief Sets num to t_value and denom to 1.
     * \param t_value
     */
    void operator=(long long int t_value);

    /*! \brief Sets num to t_value and denom to 1.
     * \param t_value
     */
    void operator=(double t_value) override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

/*************************************************/
// Inline functions (non virtual and short)      //
/*************************************************/
inline Integer::Integer(): AbstractNumerical(), value(0){}
inline Integer::Integer(long long int t_value)
    :AbstractNumerical(), value(t_value)
{}

inline Float::Float(): AbstractNumerical(), value(0){}
inline Float::Float(long double t_value): AbstractNumerical(), value(t_value){}

//The IntFraction constructor with t_num and t_denom is in the .cpp file (more complex).
inline IntFraction::IntFraction(): AbstractNumerical(), num(0), denom(1){}
inline IntFraction::IntFraction(long long int t_num)
    :AbstractNumerical(), num(t_num), denom(1){}

class Complex : public AbstractNumerical {

    protected:

    Expr real; /*!< Numerator. */

    Expr imag; /*!< Denominator. */

    public:

    /*! \brief Default Constructor.
     * \details Initializes \b num = 0 and \b denom = 1.
     */
    Complex();

    /*! \details Initializes \b num = \a t_num and \b denom = 1.
     * \param t_num Numerator.
     */
    explicit Complex(Expr t_real);

    /*! \details Initializes both the \b numerator and the \b denominator.
     * \param t_num Numerator.
     * \param t_denom Denominator.
     */
    Complex(Expr t_real,
            Expr t_imag);

#ifdef DEBUG
    ~Complex(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the \b type of a Complex.
     * \return 2
     */
    csl::Type getType() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode=0,
            std::ostream& out=std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexArgument() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexConjugate() const override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    Expr addition_own(const Expr& expr) const override;

    Expr division_own(const Expr& expr) const override;

    Expr exponentiation_own(const Expr& expr) const override;

    std::optional<Expr> derive(Expr_info expr) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;

    private:

    void checkNumerical(Expr const& expr) const;
};


Expr float_s(long double value);

Expr int_s(long long int value);

Expr autonumber_s(long double value);

Expr intfraction_s(long long int num, long long int denom);

Expr complex_s(Expr const& real, Expr const& imag);

inline csl::allocator<Integer>     alloc_integer;
inline csl::allocator<Float>       alloc_float;
inline csl::allocator<IntFraction> alloc_intfraction;
inline csl::allocator<Complex>     alloc_complex;

} // End of namespace csl

inline const csl::Expr CSL_0 = csl::make_shared<csl::Integer>(0);

inline const csl::Expr CSL_1 = csl::make_shared<csl::Integer>(1);

inline const csl::Expr CSL_2 = csl::make_shared<csl::Integer>(2);

inline const csl::Expr CSL_M_1 = csl::make_shared<csl::Integer>(-1);

inline const csl::Expr CSL_M_2 = csl::make_shared<csl::Integer>(-2);

inline const csl::Expr CSL_HALF    = csl::make_shared<csl::IntFraction>(1, 2);

inline const csl::Expr CSL_THIRD   = csl::make_shared<csl::IntFraction>(1, 3);

inline const csl::Expr CSL_M_HALF  = csl::make_shared<csl::IntFraction>(-1, 2);

inline const csl::Expr CSL_M_THIRD = csl::make_shared<csl::IntFraction>(-1, 3);

#endif
