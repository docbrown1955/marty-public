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

/*! \file mathFunctions.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Objects handling mathematical functions.
 */
#ifndef MATHFUNCTIONS_H_INCLUDED
#define MATHFUNCTIONS_H_INCLUDED
#include "scalarFunc.h"

namespace csl {

/*!
 * \brief Final specialization of AbstractFunc, the abs function
 */
class Abs : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Abs();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Abs(const Expr &t_argument);

#ifdef DEBUG
    ~Abs()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the abs funtion function.
     * \return \b 27
     */
    csl::Type getType() const override
    {
        return csl::Type::Abs;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief Evaluates the absolute value, return |\b argument|.
     *
     * \return |\b argument|
     */
    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b abs(argument.evaluate())
     */
    long double evaluateScalar() const override;

    /*! \brief \b Derives the abs function.
     * \return sign(argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr abs_s(const Expr& expr)
 * \brief \b Creates an object of type Abs acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an abs funtion. Then the return
 * value is not an Abs object but something else, so we must implement a
 * function that will create the good object. For example abs(1) returns 1 (a
 * Number) and not abs(1) (an Abs). \param expr Argment of the \b abs funtion
 * function. \return Abs applied on expr
 */
Expr abs_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the exponential function
 */
class Exp : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Exp();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Exp(const Expr &t_argument);

#ifdef DEBUG
    ~Exp()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the exponential function.
     * \return \b 20
     */
    csl::Type getType() const override
    {
        return csl::Type::Exp;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b exp(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the exponential function.
     * \return (derivative of the \b argument)*\b exp(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    std::optional<Expr> expand(bool full    = false,
                               bool inplace = false) const override;

    std::optional<Expr> expand_if(std::function<bool(Expr const &)> const &f,
                                  bool full    = false,
                                  bool inplace = false) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr exp_s(const Expr& expr)
 * \brief \b Creates an object of type Exp acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an exponential. Then the return
 * value is not an Exp object but something else, so we must implement a
 * function that will create the good object. For example exp(0) returns 1 (a
 * Number) and not exp(0) (an Exp). \param expr Argment of the \b exponential
 * function. \return Exp applied on expr
 */
Expr exp_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the logarithm function
 */
class Log : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 1.
     */
    Log();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Log(const Expr &t_argument);

#ifdef DEBUG
    ~Log()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the logarithm function.
     * \return \b 21
     */
    csl::Type getType() const override
    {
        return csl::Type::Log;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b log(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the logarithm function.
     * \return (derivative of the \b argument)*1/\b argument
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr log_s(const Expr& expr)
 * \brief \b Creates an object of type Log acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an logarithm. Then the return value
 * is not an Log object but something else, so we must implement a function
 * that will create the good object. For example log(1) returns 0 (a Number)
 * and not log(1) (a Log). \param expr Argment of the \b logarithm function.
 * \return Log applied on expr
 */
Expr log_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the cos function
 */
class Cos : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Cos();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Cos(const Expr &t_argument);

#ifdef DEBUG
    ~Cos()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the cos function.
     * \return \b 22
     */
    csl::Type getType() const override
    {
        return csl::Type::Cos;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b cos(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the cos function.
     * \return (derivative of the \b argument)*-\b sin(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr cos_s(const Expr& expr)
 * \brief \b Creates an object of type Cos acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an cosonential. Then the return
 * value is not an Cos object but something else, so we must implement a
 * function that will create the good object. For example cos(0) returns 1 (a
 * Number) and not cos(0) (a Cos). \param expr Argment of the \b cos function.
 * \return Cos applied on expr
 */
Expr cos_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the sin function
 */
class Sin : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Sin();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Sin(const Expr &t_argument);

#ifdef DEBUG
    ~Sin()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the sin function.
     * \return \b 23
     */
    csl::Type getType() const override
    {
        return csl::Type::Sin;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b sin(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the sin function.
     * \return (derivative of the \b argument)*\b cos(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr sin_s(const Expr& expr)
 * \brief \b Creates an object of type Sin acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an sin. Then the return value is not
 * an Sin object but something else, so we must implement a function that will
 * create the good object. For example sin(0) returns 0 (a Number) and not
 * sin(0) (an Sin). \param expr Argment of the \b sin function. \return Sin
 * applied on expr
 */
Expr sin_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the tan function
 */
class Tan : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Tan();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Tan(const Expr &t_argument);

#ifdef DEBUG
    ~Tan()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the tan function.
     * \return \b 24
     */
    csl::Type getType() const override
    {
        return csl::Type::Tan;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b tan(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the tan function.
     * \return derivative(argument)*(1+tan^2(argument))
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr tan_s(const Expr& expr)
 * \brief \b Creates an object of type Tan acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an tan. Then the return value is not
 * an Tan object but something else, so we must implement a function that will
 * create the good object. For example tan(0) returns 0 (a Number) and not
 * tan(0) (a Tan). \param expr Argment of the \b tan function. \return Tan
 * applied on expr
 */
Expr tan_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the acos function
 */
class ACos : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ACos();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ACos(const Expr &t_argument);

#ifdef DEBUG
    ~ACos()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the acos function.
     * \return \b 33
     */
    csl::Type getType() const override
    {
        return csl::Type::ACos;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b acos(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the acos function.
     * \return (derivative of the \b argument)*-\b asin(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr acos_s(const Expr& expr)
 * \brief \b Creates an object of type ACos acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an acosonential. Then the return
 * value is not an ACos object but something else, so we must implement a
 * function that will create the good object. For example acos(0) returns pi/2
 * (a Number) and not acos(0) (a ACos). \param expr Argment of the \b acos
 * function. \return ACos applied on expr
 */
Expr acos_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the asin function
 */
class ASin : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ASin();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ASin(const Expr &t_argument);

#ifdef DEBUG
    ~ASin()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the asin function.
     * \return \b 34
     */
    csl::Type getType() const override
    {
        return csl::Type::ASin;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b asin(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the asin function.
     * \return (derivative of the \b argument)*\b acos(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr asin_s(const Expr& expr)
 * \brief \b Creates an object of type ASin acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an asin. Then the return value is
 * not an ASin object but something else, so we must implement a function that
 * will create the good object. For example asin(0) returns 0 (a Number) and
 * not asin(0) (an ASin). \param expr Argment of the \b asin function. \return
 * ASin applied on expr
 */
Expr asin_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the atan function
 */
class ATan : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ATan();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ATan(const Expr &t_argument);

#ifdef DEBUG
    ~ATan()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the atan function.
     * \return \b 25
     */
    csl::Type getType() const override
    {
        return csl::Type::ATan;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b atan(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the atan function.
     * \return derivative(argument)/(1+argument^2)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr atan_s(const Expr& expr)
 * \brief \b Creates an object of type ATan acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an atan. Then the return value is
 * not an ATan object but something else, so we must implement a function that
 * will create the good object. For example atan(0) returns 0 (a Number) and
 * not atan(0) (a ATan). \param expr Argment of the \b atan function. \return
 * ATan applied on expr
 */
Expr atan_s(const Expr &expr);

/*!
 * \brief Handles a angle of 2 arguments
 */
class Angle : public AbstractDuoFunc {

  public:
    /*! \brief Default constructor. Initializes both arguments to 0.
     */
    Angle();

    /*! \brief Creates the angle function of \& leftOperand and \a
     * rightOperand. \details Calculates in fine the \b angle of a point of
     * coordinate (a,b) in a plane. Usefull for the argument calculation of
     * complex numbers. \param leftOperand  Left operand, imaginary part.
     * \param rightOperand Right operand, real part.
     */
    Angle(const Expr &leftOperand, const Expr &rightOperand);

#ifdef DEBUG
    ~Angle()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of an Angle object.
     * \return 12
     */
    csl::Type getType() const override
    {
        return csl::Type::Angle;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief Return the \b angle of the two arguments.
     * \return The \b angle defined by the point that has the two arguments
     * as coordinates in the plane.
     * \note For this function to be justified, all the arguments must be \b
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief Derives the angle.
     * \param expr Argument of the derivation.
     * \return The derivative following the formula ...
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;

    int getParity(Expr_info t_variable) const override;
};

Expr angle_s(Expr const &a, Expr const &b);

/*!
 * \brief Final specialization of AbstractFunc, the cosh function
 */
class Cosh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Cosh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Cosh(const Expr &t_argument);

#ifdef DEBUG
    ~Cosh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the cosh function.
     * \return \b 30
     */
    csl::Type getType() const override
    {
        return csl::Type::Cosh;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b cosh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the cosh function.
     * \return (derivative of the \b argument)*\b sinh(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr cosh_s(const Expr& expr)
 * \brief \b Creates an object of type Cosh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an coshonential. Then the return
 * value is not an Cosh object but something else, so we must implement a
 * function that will create the good object. For example cosh(0) returns 1 (a
 * Number) and not cosh(0) (a Cosh). \param expr Argment of the \b cosh
 * function. \return Cosh applied on expr
 */
Expr cosh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the sinh function
 */
class Sinh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Sinh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Sinh(const Expr &t_argument);

#ifdef DEBUG
    ~Sinh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the sinh function.
     * \return \b 31
     */
    csl::Type getType() const override
    {
        return csl::Type::Sinh;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b sinh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the sinh function.
     * \return (derivative of the \b argument)*\b cosh(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr sinh_s(const Expr& expr)
 * \brief \b Creates an object of type Sinh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an sinh. Then the return value is
 * not an Sinh object but something else, so we must implement a function that
 * will create the good object. For example sinh(0) returns 0 (a Number) and
 * not sinh(0) (an Sinh). \param expr Argment of the \b sinh function. \return
 * Sinh applied on expr
 */
Expr sinh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the tanh function
 */
class Tanh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Tanh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Tanh(const Expr &t_argument);

#ifdef DEBUG
    ~Tanh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the tanh function.
     * \return \b 33
     */
    csl::Type getType() const override
    {
        return csl::Type::Tanh;
    }

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b tanh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the tanh function.
     * \return derivative(argument)*(1-tanh^2(argument))
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr tanh_s(const Expr& expr)
 * \brief \b Creates an object of type Tanh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an tanh. Then the return value is
 * not an Tanh object but something else, so we must implement a function that
 * will create the good object. For example tanh(0) returns 0 (a Number) and
 * not tanh(0) (a Tanh). \param expr Argment of the \b tanh function. \return
 * Tanh applied on expr
 */
Expr tanh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the acosh function
 */
class ACosh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ACosh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ACosh(const Expr &t_argument);

#ifdef DEBUG
    ~ACosh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the acosh function.
     * \return \b 35
     */
    csl::Type getType() const override
    {
        return csl::Type::ACosh;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b acosh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the acosh function.
     * \return (derivative of the \b argument)*\b asinh(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr acosh_s(const Expr& expr)
 * \brief \b Creates an object of type ACosh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an acoshonential. Then the return
 * value is not an ACosh object but something else, so we must implement a
 * function that will create the good object. For example acosh(0) returns 1 (a
 * Number) and not acosh(0) (a ACosh). \param expr Argment of the \b acosh
 * function. \return ACosh applied on expr
 */
Expr acosh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the asinh function
 */
class ASinh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ASinh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ASinh(const Expr &t_argument);

#ifdef DEBUG
    ~ASinh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the asinh function.
     * \return \b 36
     */
    csl::Type getType() const override
    {
        return csl::Type::ASinh;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b asinh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the asinh function.
     * \return (derivative of the \b argument)*\b acosh(\b argument)
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr asinh_s(const Expr& expr)
 * \brief \b Creates an object of type ASinh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an asinh. Then the return value is
 * not an ASinh object but something else, so we must implement a function that
 * will create the good object. For example asinh(0) returns 0 (a Number) and
 * not asinh(0) (an ASinh). \param expr Argment of the \b asinh function.
 * \return ASinh applied on expr
 */
Expr asinh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the atanh function
 */
class ATanh : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    ATanh();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit ATanh(const Expr &t_argument);

#ifdef DEBUG
    ~ATanh()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the atanh function.
     * \return \b 37
     */
    csl::Type getType() const override
    {
        return csl::Type::ATanh;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b atanh(argument.evaluate())
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the atanh function.
     * \return derivative(argument)*(1-atanh^2(argument))
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

/*! \fn Expr atanh_s(const Expr& expr)
 * \brief \b Creates an object of type ATanh acting on \b expr.
 * \details As all the outside-class functions, this one take into account the
 * fact that the final object could not be an atanh. Then the return value is
 * not an ATanh object but something else, so we must implement a function that
 * will create the good object. For example atanh(0) returns 0 (a Number) and
 * not atanh(0) (a ATanh). \param expr Argment of the \b atanh function.
 * \return ATanh applied on expr
 */
Expr atanh_s(const Expr &expr);

/*!
 * \brief Final specialization of AbstractFunc, the factorial function
 */
class Factorial : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    Factorial();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit Factorial(const Expr &t_argument);

#ifdef DEBUG
    ~Factorial()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the factorial function.
     * \return \b 28
     */
    csl::Type getType() const override
    {
        return csl::Type::Factorial;
    }

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief Evaluates the factorialolute value, return |\b argument|.
     * \return |\b argument|
     */
    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    /*! \brief \b Evaluates the function on the \b argument.
     * \return \b factorial(argument.evaluate())
     * \warning If the \b argument does not evaluate to an int, a warning is
     * printed in standard output and the calculation is done with
     * round(argument).
     */
    long double evaluateScalar() const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief \b Derives the factorial function, returns always \b 0: a
     * factorial cannot be derived.
     * \return \b 0
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;
};

Expr factorial_s(const Expr &expr);

class DiracDelta : public AbstractFunc {

  public:
    /*! \brief Default constructor. Initializes \b argument to 0.
     */
    DiracDelta();

    /*! \brief Initializes \b argument to \b t_argument.
     * \param t_argument
     */
    explicit DiracDelta(const Expr &t_argument);

#ifdef DEBUG
    ~DiracDelta()
    {
        __record_data_alloc(static_cast<int>(getType()));
    }
#endif

    /*! \brief Gives the **type** of the atanh function.
     * \return \b 37
     */
    csl::Type getType() const override
    {
        return csl::Type::DiracDelta;
    }

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    /*! \brief \b Derives the atanh function.
     * \return derivative(argument)*(1-atanh^2(argument))
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    Expr applyDiracDelta(const Expr &expr,
                         const Expr &variable) const override;

    bool operator==(Expr_info other) const override;
};

Expr diracdelta_s(const Expr &argument);

/////
// Inline functions
inline Abs::Abs() : AbstractFunc()
{
}
inline Abs::Abs(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Exp::Exp() : AbstractFunc()
{
}
inline Exp::Exp(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Log::Log() : AbstractFunc()
{
    argument = CSL_1;
}
inline Log::Log(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Angle::Angle() : AbstractDuoFunc()
{
}
inline Angle::Angle(const Expr &leftOperand, const Expr &rightOperand)
    : AbstractDuoFunc()
{
    argument[0] = leftOperand;
    argument[1] = rightOperand;
}

inline Cos::Cos() : AbstractFunc()
{
}
inline Cos::Cos(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Sin::Sin() : AbstractFunc()
{
}
inline Sin::Sin(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Tan::Tan() : AbstractFunc()
{
}
inline Tan::Tan(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Cosh::Cosh() : AbstractFunc()
{
}
inline Cosh::Cosh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Sinh::Sinh() : AbstractFunc()
{
}
inline Sinh::Sinh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Tanh::Tanh() : AbstractFunc()
{
}
inline Tanh::Tanh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ACos::ACos() : AbstractFunc()
{
}
inline ACos::ACos(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ASin::ASin() : AbstractFunc()
{
}
inline ASin::ASin(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ATan::ATan() : AbstractFunc()
{
}
inline ATan::ATan(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ACosh::ACosh() : AbstractFunc()
{
}
inline ACosh::ACosh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ASinh::ASinh() : AbstractFunc()
{
}
inline ASinh::ASinh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline ATanh::ATanh() : AbstractFunc()
{
}
inline ATanh::ATanh(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}

inline Factorial::Factorial() : AbstractFunc()
{
}
inline Factorial::Factorial(const Expr &t_argument) : AbstractFunc()
{
    argument = t_argument;
}
/////
} // End of namespace csl

#endif
