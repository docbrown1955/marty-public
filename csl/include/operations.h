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

/*! \file operations.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Objects handling multi-argument functions, primordial operations (+,*,/,etc)
 */
#ifndef OPERATIONS_H_INCLUDED
#define OPERATIONS_H_INCLUDED
#include "scalarFunc.h"
#include "operator.h"

namespace csl {

// inline bool testOperatorComparison = false;

/*! 
 * \brief Handles a sum, function of multiple arguments
 */
class Sum: public AbstractMultiFunc{

    public:

    /*! \brief Default constructor. Initializes \b argument to an empty std::vector.
     */
    Sum();

    /*! \brief Initializes \b argument to \a operands (copy each element)
     * \param operands Operands of the sum, copied in the object.
     */
    Sum(const csl::vector_expr& operands, bool explicitSum=false);

    /*! \brief Creates the sum of \& leftOperand and \a rightOperand.
     * \param leftOperand  Left operand.
     * \param rightOperand Right operand.
     */
    Sum(const Expr& leftOperand,
         const Expr& rightOperand,
         bool        explicitSum = false);


#ifdef DEBUG
    ~Sum(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the **type** of a Sum object.
     * \return 10
     */
    csl::Type getType() const override {
        return csl::Type::Sum;
    }

    virtual bool isIndexed() const override { return false; }

    /*! \return The numerical factor of the sum.
     */
    Expr getNumericalFactor() const override;

    int getOrderOf(Expr_info expr) const override;

    /*! \return The sum divided by its factor.
     */
    std::optional<Expr> getTerm() const override;

    /*! \brief Returns all the possible factors.
     * \return The concatenation of all possible factors for the arguments.
     */
    csl::vector_expr getFactors() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;
    
    std::optional<Expr> getComplexArgument() const override;

    void insert(const Expr& expr, bool explicitSum=false) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode=0,
            std::ostream& out=std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    /*! \brief Return the \b sum of all the arguments.
     * \return The sum of the scalar evaluation of all the arguments.
     * \note For this function to be justified, all the arguments must be \b 
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief Merge identical terms.
     * \details When identical terms (identical to a numerical factor) appear 
     * this function merge them in the same term. Example: x+y+2*x -> 3*x+y.
     * \return \b True if terms have been merged.
     * \return \b False else.
     */
    virtual bool mergeTerms();

    void orderTerms();

    /*! \brief Derives the sum.
     * \param expr Argument of the derivation.
     * \return The sum of the derivatives of the arguments
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief Factors the expr if common factors are found.
     * \param full If \b true factors recursively all the arguments.
     */
    std::optional<Expr> factor(bool full=false) const override;

    /*! \brief Factors the expr by expr if it is a common factor.
     * \param expr Factor.
     * \param full       If \b true factors recursively all the arguments.
     */
    std::optional<Expr> factor(Expr_info expr, bool full=false) const override;

    std::optional<Expr> collect(
            std::vector<Expr> const &factors,
            bool                     full
            ) const override;

    bool askTerm(Expr_info term, bool exact=false) const override;

    Expr suppressTerm(Expr_info term) const override;

    int getParity(Expr_info t_variable) const override;

    bool operator==(Expr_info expr) const override;

    csl::vector_expr getAlternateForms() const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    virtual void gatherFactors(
            csl::vector_expr& factors,
            csl::vector_expr& arg,
            bool full
            ) const;

    void clearRedundantFactors(csl::vector_expr& factors) const;
};

/*! 
 * \brief Handles a polynomial, function of multiple arguments
 */
class Polynomial: public AbstractMultiFunc{

    private:

    Expr variable;

    public:

    /*! \brief Default constructor. Initializes \b argument to an empty 
     * std::vector, \b order to 0.*/
    Polynomial();

    /*! \brief Initializes the polynomial from another Abstract. Detect the 
     *  proper terms in it.
     *  \param expr Abstract that we want to Transform in a polynomial
     * \param t_variable Variable of the polynomial.
     */
    Polynomial(const Expr& expr, const Expr& t_variable);
    
    /*! \brief Initializes the polynomial from a vector of terms, and a defined
     *  \b variable.
     *  \param terms for the creation of the polynomial.
     *  \param t_variable Variable of the polynomial.
     */
    Polynomial(const csl::vector_expr& terms, const Expr& t_variable);
    

#ifdef DEBUG
    ~Polynomial(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    bool mergeTerms();

    /*! \brief Gives the **type** of a Polynomial object.
     * \return 14
     */
    csl::Type getType() const override {
        return csl::Type::Polynomial;
    }

    int getOrder() const override;

    int getOrderOf(Expr_info expr) const override;

    Expr getVariable() const override;

    virtual bool isIndexed() const override;

    IndexStructure getIndexStructure() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode=0,
            std::ostream& out=std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    /*! \brief Return the \b polynomial of all the arguments.
     * \return The polynomial of the scalar evaluation of all the arguments.
     * \note For this function to be justified, all the arguments must be \b 
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief Derives the polynomial.
     * \param expr Argument of the derivation.
     * \return The polynomial of the derivatives of the arguments
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    Expr getRegularExpression() const override;

    Expr addition_own(const Expr& expr) const override;

    Expr multiplication_own(const Expr& expr, bool side=1) const override;
    
    Expr division_own(const Expr& expr) const override;

    std::optional<Expr> factor(bool full=false) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

void getExponentStructure(const Expr& argument, Expr& term, Expr& exponent);

/*! 
 * \brief Handles a product, function of multiple arguments
 */
class Prod: public AbstractMultiFunc{

    public:

    /*! \brief Default constructor. Initializes \b argument to an empty std::vector.
     */
    Prod();

    /*! \brief Initializes \b argument to \a operands (copy each element)
     * \param operands Operands of the product, copied in the object.
     * \param explicitProd Must be true if no modification of the vector \b 
     * argument is needed.
     */
    Prod(const csl::vector_expr& t_argument, bool explicitProd=0);

    /*! \brief Creates the product of \& leftOperand and \a rightOperand.
     * \param leftOperand  Left operand.
     * \param rightOperand Right operand.
     * \param explicitProd Must be true if no modification of the vector \b 
     * argument is needed.
     */
    Prod(const Expr& leftOperand, const Expr& rightOperand, bool explicitProd=0);


#ifdef DEBUG
    ~Prod(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the **type** of a Prod object.
     * \return 11
     */
    csl::Type getType() const override {
        return csl::Type::Prod;
    }

    virtual bool isIndexed() const override { return false; }

    int getOrderOf(Expr_info expr) const override;

    //IndexStructure getIndexStructure() const override;

    //virtual void selfCheckIndexStructure();

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;
    
    std::optional<Expr> getComplexArgument() const override;

    std::optional<Expr> getHermitianConjugate(
            const Space* space) const override;

    std::optional<Expr> getHermitianConjugate(
            const std::vector<const Space*>& spaces) const override;

    std::optional<Expr> findSubExpression(
            Expr_info   subExpression,
            const Expr& newExpression) const override;

    /*! \return The numerical factor of the product.
     */
    Expr getNumericalFactor() const override;

    /*! \return The product of all the terms but the nunmerical factor.
     */
    std::optional<Expr> getTerm() const override;

    /*! \brief Returns the number of possible factors in the expr.
     * \return \b nArgs
     */
    int getNFactor() const override;

    /*! \brief Returns all the possible factors.
     * \return The concatenation of all possible factors for the arguments.
     */
    csl::vector_expr getFactors() const override;

    void getExponents(
            std::vector<Expr> const &factors,
            std::vector<Expr>       &exponents
            ) const override;

    /*! \brief Tells if the factor \a expr appear in the product.
     * \param expr Factor to test.
     * \return \b True if expr can factor the product.
     * \return \b False else.
     */
    bool askTerm(Expr_info expr, bool exact=false) const override;

    /*! \brief Suppresses the term expr in the product.
     * \warning This function supposes that the function askTerm() has been called 
     * precedently and does not warn the user if the factor expr is not present.
     * \param expr Factor to suppress in the product.
     * \return \b The product amputated of \b expr.
     */
    Expr suppressTerm(Expr_info expr) const override;

    std::optional<Expr> suppressExponent(
            Expr const &factor,
            Expr const &exponent
            ) const override;

    void insert(const Expr& expr, bool side=1) override;
    virtual void leftInsert(const Expr& expr);
    virtual void rightInsert(const Expr& expr);

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    /*! \brief Return the \b product of all the arguments.
     * \return The product of the scalar evaluation of all the arguments.
     * \note For this function to be justified, all the arguments must be \b 
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool mergeNumericals();

    void mergeProducts();

    /*! \brief Merge identical terms.
     * \details When identical terms (identical to a numerical factor) appear 
     * this function merge them in the same term. Example: x*y*x^2 -> x^3+y.
     * \return \b True if terms have been merged.
     * \return \b False else.
     */
    virtual bool mergeTerms();

    void orderTerms();

    /*! \brief Derives the product.
     * \param expr Argument of the derivation.
     * \return The derivative following the formula ...
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief \b Develops the product by expanding the sums inside.
     * \param full If true the expandment is \b recursive through all the Abstract.
     * \return The expanded product.
     */
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

    int isPolynomial(Expr_info expr) const override;

    std::optional<Expr> getPolynomialTerm(
            Expr_info t_variable, int order) const override;

    int getParity(Expr_info t_variable) const override;
    
    virtual bool operator==(Expr_info expr) const override;

    virtual bool partialComparison(Expr_info expr) const;

    csl::vector_expr getAlternateForms() const override;

    bool operator<(const Abstract* expr) const override;
};

/*! 
 * \brief Handles the exponentiation of one Abstract wrt another.
 */
class Pow: public AbstractDuoFunc{

    public:

    /*! \brief Default constructor. Initializes both arguments to 0.
     */
    Pow();

    /*! \brief Creates the exponentiation of \& leftOperand wrt \a rightOperand.
     * \param leftOperand  Left operand.
     * \param rightOperand Right operand.
     */
    Pow(const Expr& leftOperand,
        const Expr& rightOperand,
        bool        explicitPow = false);


#ifdef DEBUG
    ~Pow(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the **type** of a Pow object.
     * \return 13
     */
    csl::Type getType() const override {
        return csl::Type::Pow;
    }

    Expr getNumericalFactor() const override;

    std::optional<Expr> getTerm() const override;

    int getOrderOf(Expr_info expr) const override;

    /*! \brief Returns the number of possible factors in the expr.
     * \return \b A function of the exponent if it is a Number.
     * \return \b 1 else.
     */
    int getNFactor() const override;

    /*! \brief Returns all the possible factors.
     * \return The concatenation of all possible factors for the arguments.
     */
    csl::vector_expr getFactors() const override;

    void getExponents(
            std::vector<Expr> const &factors,
            std::vector<Expr>       &exponents
            ) const override;

    /*! \brief Tells if the factor \a expr appear in the exponentiation.
     * \param expr Factor to test.
     * \return \b True if expr can factor the exponentiation.
     * \return \b False else.
     */
    bool askTerm(Expr_info expr, bool exact=false) const override;

    /*! \brief Suppresses the term expr in the exponentiation.
     * \warning This function supposes that the function askTerm() has been called 
     * precedently and does not warn the user if the factor expr is not present.
     * \param expr Factor to suppress in the exponentiation.
     * \return \b The exponentiation divided by \b expr.
     */
    Expr suppressTerm(Expr_info expr) const override;

    std::optional<Expr> suppressExponent(
            Expr const &factor,
            Expr const &exponent
            ) const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    LibDependency getLibDependency() const override;

    /*! \brief Return the \b exponentiation of the two arguments.
     * \return The exponentiation of the scalar evaluation of the two arguments.
     * \note For this function to be justified, all the arguments must be \b 
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief Merge composition of exponentiations.
     * \details This function replaces compositions of exponentiations. For example
     * (a^b)^c -> a^(b*c).
     * \return \b True if terms have been merged.
     * \return \b False else.
     */
    bool mergeTerms();

    /*! \brief Derives the exponentiation.
     * \param expr Argument of the derivation.
     * \return The derivative following the formula ...
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    /*! \brief \b Develops the exponentiation by expanding the sums inside.
     * \param full If true the expandment is \b recursive through all the Abstract.
     * \return The expanded exponentiation.
     */
    std::optional<Expr> expand(bool full=false,
                               bool inplace=false) const override;

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
            bool inplace=false) const override;

    int isPolynomial(Expr_info expr) const override;

    std::optional<Expr> getPolynomialTerm(
            Expr_info t_variable, int order) const override;

    int getParity(Expr_info t_variable) const override;
    
    bool operator==(Expr_info expr) const override;

    csl::vector_expr getAlternateForms() const override;

    bool operator<(const Abstract* expr) const override;
};


/*! 
 * \brief Handles the exponentiation of one Abstract wrt another.
 */
class Derivative: public Operator<AbstractDuoFunc>{

    protected:

    int order;

    public:

    /*! \brief Default constructor. Initializes both arguments to 0, \b order to 1.
     */
    Derivative();

    /*! \brief Creates the derivative operator wrt variable at order \b order. 
     * It is applied when it appears in a multiplication.
     * \param variable Variable wrt which we derive.
     * \param order Order of the derivation.
     */
    Derivative(const Expr& variable, int order);

    /*! \brief Creates the derivative of \& leftOperand wrt \a rightOperand, 
     * to \b order 1.
     * \param leftOperand  Left operand.
     * \param rightOperand Right operand.
     */
    Derivative(const Expr& leftOperand, const Expr& rightOperand);

    /*! \brief Creates the derivative of \& leftOperand wrt \a rightOperand, 
     * to \b order \a t_order.
     * \param leftOperand  Left operand.
     * \param rightOperand Right operand.
     */
    Derivative(const Expr& leftOperand, const Expr& rightOperand, int t_order);

    Derivative(const Expr& leftOperand, const Expr& rightOperand, 
               int t_order, bool t_empty);


#ifdef DEBUG
    ~Derivative(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the **type** of a Derivative object.
     * \return 29
     */
    csl::Type getType() const override {
        return csl::Type::Derivative;
    }

    Expr getVariable() const override;

    Expr getOperand() const override;

    int getOrder() const override;

    void setOperand(const Expr& t_operand) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    /*! \brief Return the \b derivative of the first argument wrt the second.
     * \return The derivative of the scalar evaluation of the two arguments.
     * \note For this function to be justified, all the arguments must be \b 
     * scalar and \b real.
     */
    long double evaluateScalar() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    /*! \brief Derives the derivative.
     * \param expr Argument of the derivation.
     * \return The derivative following the formula ...
     */
    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    bool operatorAppliesOn(Expr_info expr) const override;
    
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

class Integral: public Operator<AbstractDuoFunc>{

    protected:

    Expr inf;
    Expr sup;

    public:

    Integral();

    explicit Integral(const Expr& variable);

    Integral(const Expr& leftOperand, const Expr& rightOperand);

    Integral(const Expr& leftOperand, const Expr& rightOperand, bool t_empty);

    Integral(const Expr& leftOperand, const Expr& rightOperand,
            const Expr& t_inf, const Expr& t_sup);

    Integral(const Expr& leftOperand, const Expr& rightOperand, bool t_empty,
            const Expr& t_inf, const Expr& t_sup);


#ifdef DEBUG
    ~Integral(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    /*! \brief Gives the **type** of a Integral object.
     * \return 29
     */
    csl::Type getType() const override {
        return csl::Type::Integral;
    }

    Expr getVariable() const override;

    Expr getOperand() const override;

    Expr getSupBoundary() const override;

    Expr getInfBoundary() const override;

    void setOperand(const Expr& t_operand) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    //long double evaluateScalar() const override;
    
    Expr suppressTerm(Expr_info term) const override;

    Expr evaluateIntegral() const;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    std::optional<Expr> derive(Expr_info expr) const override;

    int getParity(Expr_info t_variable) const override;

    bool operatorAppliesOn(Expr_info expr) const override;

    bool dependsOn(Expr_info var) const override;

    bool dependsExplicitlyOn(Expr_info var) const override;
    
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    /*bool evaluateDelta(Expr& res, const Expr& dep) const;

    bool evaluateExpProduct(Expr& res, const Expr& dep) const;*/
};

void applyOperator(Expr& product);

/*************************************************/
// Inline functions (non virtual and short)      //
/*************************************************/
inline Sum::Sum(): AbstractMultiFunc(){}
inline Prod::Prod(): AbstractMultiFunc(){}
inline Polynomial::Polynomial(): AbstractMultiFunc(), variable(CSL_0){}
inline Pow::Pow(): AbstractDuoFunc(){}

inline Derivative::Derivative(): Operator<AbstractDuoFunc>(), order(1)
{
    argument[0] = CSL_1;
    argument[1] = CSL_0;
}
inline Derivative::Derivative(const Expr& t_variable,
                              int t_order)
    :Operator<AbstractDuoFunc>(), order(t_order)
{
    argument[0] = CSL_1;
    argument[1] = t_variable;
}
inline Derivative::Derivative(const Expr& leftOperand,
                              const Expr& rightOperand)
                              : Operator<AbstractDuoFunc>(), order(1)
{
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    if (*argument[1] != CSL_1)
        empty = false;
}
inline Derivative::Derivative(const Expr& leftOperand,
                              const Expr& rightOperand,
                              int t_order)
    :Operator<AbstractDuoFunc>(), order(t_order)
{
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    if (*argument[1] != CSL_1)
        empty = false;
}
inline Derivative::Derivative(const Expr& leftOperand, const Expr& rightOperand, 
               int t_order, bool t_empty)
    :Operator<AbstractDuoFunc>(), order(t_order)
{
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    empty = t_empty;
}

inline Integral::Integral(): Operator<AbstractDuoFunc>()
{
    argument[0] = CSL_1;
    argument[1] = CSL_0;
    inf = -CSL_INF;
    sup = CSL_INF;
}
inline Integral::Integral(const Expr& t_variable)
    :Integral()
{
    argument[1] = t_variable;
}
Expr tensor_s(const std::vector<int>& shape, const Expr& filler);
inline Integral::Integral(const Expr& leftOperand, const Expr& rightOperand)
    :Integral()
{
//    if (rightOperand->getDim() > 0)
 //       callError(cslError::UndefinedBehaviour,
 //               "Integral::Integral(const Expr&, const Expr&)");
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    inf = -CSL_INF;
    sup = CSL_INF;
    if (*argument[1] != CSL_1)
        empty = false;
}

inline Integral::Integral(const Expr& leftOperand, const Expr& rightOperand, 
               bool t_empty):Integral(leftOperand, rightOperand)
{
    empty = t_empty;
}
inline Integral::Integral(const Expr& leftOperand, const Expr& rightOperand,
        const Expr& t_inf, const Expr& t_sup)
    :Integral()
{
    inf = t_inf;
    sup = t_sup;
    argument[0] = leftOperand;
    argument[1] = rightOperand;
    if (*argument[1] != CSL_1)
        empty = false;
}

inline Integral::Integral(const Expr& leftOperand, const Expr& rightOperand, 
               bool t_empty, const Expr& t_inf, const Expr& t_sup)
    :Integral(leftOperand, rightOperand, t_inf, t_sup)
{
    empty = t_empty;
}

inline csl::allocator<Sum>  alloc_sum;
inline csl::allocator<Prod> alloc_prod;
inline csl::allocator<Pow>  alloc_pow;

} // End of namespace csl

#endif
