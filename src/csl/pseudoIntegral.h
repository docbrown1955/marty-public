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
#ifndef PSEUDOINTEGRAL_H_INCLUDED
#define PSEUDOINTEGRAL_H_INCLUDED

#include "operations.h"
#include "mathFunctions.h"
#include "indicial.h"

namespace csl {

class PseudoIntegral
{
    public:

    PseudoIntegral();

    explicit
    PseudoIntegral(const csl::vector_expr& terms);

    PseudoIntegral(const csl::vector_expr& t_integrals,
                   const Expr&              t_argument);

    ~PseudoIntegral(){};

    void addTerm(const Expr& term);

    Expr getExpression() const;

    Expr evaluate();

    Expr applyOn(const Expr& expr) const;

    Expr const &getArgument() const;

    private:

    bool testBoundsInfinity(const Expr& inf,
                            const Expr& sup) const;

    void testIntegralDep(const Expr& term) const;

    void addSingleTerm(const Expr& term);

    void addSingleIntegral(const Expr& integral);

    void getRecursiveIntegralDependency(
            Expr& operand,
            const Expr& variable,
            std::vector<Expr*>& dependencies);

    Expr getExponentialDependency(
            std::vector<Expr*> const& dep) const;

    void expandExponentials();

    bool evaluateExpProduct(int         index,
                            Expr&       res,
                            std::vector<Expr*>& dependencies) const;

    bool evaluateDelta(int         index,
                       Expr&       res,
                       std::vector<Expr*>& dependencies) const;

    private:

    mutable
    bool argumentUpdated = true;
        
    csl::vector_expr vectorIntegrals;

    csl::vector_expr argumentVector;

    mutable
    Expr argument;
};


class AbstractIntegral: public Operator<AbstractFunc>{

    public:

    AbstractIntegral();

    explicit
    AbstractIntegral(const Expr&     operand);

    AbstractIntegral(const Expr&     operand,
                     bool            t_empty);

    AbstractIntegral(const Expr&     operand,
                     const Expr&     t_inf,
                     const Expr&     t_sup);

    AbstractIntegral(const Expr&     operand,
                     bool            t_empty,
                     const Expr&     t_inf,
                     const Expr&     t_sup);

    /*! \brief Destructor.
     */
    ~AbstractIntegral(){};

    Expr getOperand() const override;

    bool operatorAppliesOn(Expr_info expr) const override;

    Expr getSupBoundary() const override;

    Expr getInfBoundary() const override;

    void setSupBoundary(Expr const &t_inf) override;

    void setInfBoundary(Expr const &t_sup) override;

    void setOperand(const Expr& t_operand) override;
    
    Expr suppressTerm(Expr_info term) const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    std::optional<Expr> derive(Expr_info expr) const override;

    bool dependsOn(Expr_info var) const override;

    bool dependsOn(const AbstractParent* parent) const override;

    bool dependsExplicitlyOn(Expr_info var) const override;

    protected:

    Expr inf;

    Expr sup;
};

class ScalarIntegral: public AbstractIntegral {

    public:

    ScalarIntegral();

    explicit
    ScalarIntegral(const Expr& t_variable);

    ScalarIntegral(const Expr& operand,
                   const Expr& t_variable);

    ScalarIntegral(const Expr& t_variable,
                   const Expr& t_inf,     
                   const Expr& t_sup);  

    ScalarIntegral(const Expr& operand,
                   const Expr& t_variable,
                   const Expr& t_inf,
                   const Expr& t_sup);

    ScalarIntegral(const Expr& operand,
                   const Expr& t_variable,
                   bool        t_empty,
                   const Expr& t_inf,
                   const Expr& t_sup);

    /*! \brief Destructor.
     */
    ~ScalarIntegral(){};

    csl::Type getType() const override {
        return csl::Type::ScalarIntegral;
    }

    Expr getVariable() const override;

    void setVariable(Expr const &variable) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               LibraryMode libMode = LibraryMode::NoLib) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool operator==(Expr_info other) const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    Expr variable;
};

class VectorIntegral: public AbstractIntegral {

    public:

    VectorIntegral();

    explicit
    VectorIntegral(const Tensor& t_variables);

    VectorIntegral(const Expr&     operand,
                   const Tensor&  t_variables);

    VectorIntegral(const Expr&     operand,
                   const Tensor&  t_variable,
                   bool            t_empty);

    VectorIntegral(const Expr&     operand,
                   const Tensor&  t_variable,
                   const Expr&     t_inf,
                   const Expr&     t_sup);

    VectorIntegral(const Tensor&  t_variable,
                   const Expr&     t_inf,
                   const Expr&     t_sup);

    VectorIntegral(const Expr&     operand,
                   const Tensor&   t_variable,
                   bool            t_empty,
                   const Expr&     t_inf,
                   const Expr&     t_sup);


    /*! \brief Destructor.
     */
    ~VectorIntegral(){};

    csl::Type getType() const override {
        return csl::Type::VectorIntegral;
    }

    Expr getVariable() const override;

    Parent getParent() const override;

    Parent_info getParent_info() const override;

    void setParent(const Parent& t_variable) override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               LibraryMode libMode = LibraryMode::NoLib) const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool isIndexed() const override;

    bool operatorAppliesOn(Expr_info expr) const override;

    IndexStructure getIndexStructure() const override;

    bool operator==(Expr_info other) const override;

    bool compareWithDummy(Expr_info expr,
                          std::map<Index, Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    Tensor variables;
};

void MakeIntegral(Expr& init);

template<typename ...Args>
Expr scalarintegral_s(Args ...args) {
    Expr res = csl::make_shared<ScalarIntegral>(args...);
    MakeIntegral(res);
    return res;
}

Expr vectorintegral_s(const Tensor& variables);

template<typename ...Args>
Expr vectorintegral_s(const Expr&     operand,
                      const Tensor&  variables,
                      Args...         args) {
    Expr res = csl::make_shared<VectorIntegral>(operand, variables, args...);
    MakeIntegral(res);
    return res;
}

template<typename ...Args>
Expr vectorintegral_s(const Tensor&  variables,
                      Args...         args) {
    Expr res = csl::make_shared<VectorIntegral>(variables, args...);
    MakeIntegral(res);
    return res;
}

inline Expr vectorintegral_s(const Parent& variables)
{
    Tensor tensor = std::dynamic_pointer_cast<TensorParent>(variables);
    CSL_ASSERT_SPEC(tensor,
            CSLError::TypeError,
            "Expected a tensor for vector integral, " 
            + toString(variables->getName()) + " given.");
    return vectorintegral_s(tensor);
}

template<typename ...Args>
Expr vectorintegral_s(const Expr&     operand,
                      const Parent&  variables,
                      Args...         args) {
    Tensor tensor = std::dynamic_pointer_cast<TensorParent>(variables);
    CSL_ASSERT_SPEC(tensor,
            CSLError::TypeError,
            "Expected a tensor for vector integral, " 
            + toString(variables->getName()) + " given.");
    return vectorintegral_s(operand, tensor, std::forward<Args>(args)...);
}

template<typename ...Args>
Expr vectorintegral_s(const Parent&  variables,
                      Args...         args) {
    Tensor tensor = std::dynamic_pointer_cast<TensorParent>(variables);
    CSL_ASSERT_SPEC(tensor,
            CSLError::TypeError,
            "Expected a tensor for vector integral, " 
            + toString(variables->getName()) + " given.");
    return vectorintegral_s(tensor, std::forward<Args>(args)...);
}
} // End of namespace csl

#endif /* PSEUDOINTEGRAL_H_INCLUDED */
