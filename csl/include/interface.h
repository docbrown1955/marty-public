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
#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include "abstract.h"
#include "parent.h"
#include "options.h"

namespace csl {

class TensorField;
class TDerivative;

template<class T>
[[nodiscard]]
inline csl::Type GetType(T const &expr)
{
    return expr->getType();
}
template<class T>
[[nodiscard]]
inline csl::Type GetPrimaryType(T const &expr)
{
    return expr->getPrimaryType();
}

[[nodiscard]]
inline std::size_t Size(Expr const &expr)
{
    return expr->size();
}

[[nodiscard]]
inline Expr GetArgument(
        Expr const &expr,
        std::size_t      pos = 0
        )
{
    return expr->getArgument(int(pos));
}

[[nodiscard]]
inline bool GetCommutable(csl::Expr const &expr)
{
    return !csl::option::checkCommutations || expr->getCommutable();
}

[[nodiscard]]
inline bool GetCommutable(csl::Expr_info expr)
{
    return !csl::option::checkCommutations || expr->getCommutable();
}

[[nodiscard]]
inline bool GetCommutable(csl::Abstract const &expr)
{
    return !csl::option::checkCommutations || expr.getCommutable();
}

[[nodiscard]]
Expr GetSymbol(
        std::string_view name,
        Expr      const &init
        );

[[nodiscard]]
Parent GetParent(
        std::string_view name,
        Expr      const &init
        );

[[nodiscard]]
inline bool DependsOn(
        Expr const &init,
        Expr const &x)
{
    return init->dependsOn(x.get());
}

[[nodiscard]]
inline bool DependsExplicitlyOn(
        Expr const &init,
        Expr const &x)
{
    return init->dependsExplicitlyOn(x.get());
}

[[nodiscard]]
inline bool DependsOn(
        Expr const &init,
        csl::Parent const &x)
{
    return init->dependsOn(x.get());
}

[[nodiscard]]
inline bool DependsExplicitlyOn(
        Expr const &init,
        csl::Parent const &x)
{
    return init->dependsExplicitlyOn(x.get());
}

void Expand(Expr& expression,
            bool  full = false,
            bool inplace = false);

[[nodiscard]]
Expr Expanded(const Expr& expression,
              bool        full = false,
              bool        inplace = false);

void ExpandIf(Expr& expression,
              std::function<bool(Expr const&)> const& f,
              bool  full = false,
              bool  inplace = false);

[[nodiscard]]
Expr ExpandedIf(const Expr& expression,
                std::function<bool(Expr const&)> const& f,
                bool        full = false,
                bool        inplace = false);

inline void DeepExpand(
        Expr &expression,
        bool inplace = false
        )
{
    Expand(expression, true, inplace);
}

[[nodiscard]]
inline Expr DeepExpanded(
        Expr const &expression,
        bool inplace = false
        )
{
    return Expanded(expression, true, inplace);
}

inline void DeepExpandIf(
        Expr &expression,
        std::function<bool(Expr const&)> const& f,
        bool inplace = false
        )
{
    ExpandIf(expression, f, true, inplace);
}

[[nodiscard]]
inline Expr DeepExpandedIf(
        Expr const &expression,
        std::function<bool(Expr const&)> const& f,
        bool inplace = false
        )
{
    return ExpandedIf(expression, f, true, inplace);
}

void DeepExpandIf_lock(
        Expr &expression,
        std::function<bool(Expr const&)> const &f,
        int lockId = 0,
        bool inplace = false,
        bool refactor = false
        );

[[nodiscard]]
Expr DeepExpandedIf_lock(
        Expr const &expression,
        std::function<bool(Expr const&)> const &f,
        int lockId = 0,
        bool inplace = false,
        bool refactor = false
        );

void Factor(Expr& expression,
            bool  full = false);

void Factor(Expr&     expression,
            Expr_info factor,
            bool      full = false);

inline void Factor(
        Expr       &expression,
        Expr const &factor,
        bool        full = false)
{
    Factor(expression, factor.get(), full);
}

[[nodiscard]]
Expr Factored(const Expr& expression,
              bool        full = false);

[[nodiscard]]
Expr Factored(const Expr& expression,
              Expr_info   factor,
              bool        full = false);

[[nodiscard]]
inline Expr Factored(
        const Expr& expression,
        const Expr& factor,
        bool        full = false)
{
    return Factored(expression, factor.get(), full);
}

inline void DeepFactor(
        Expr& expression
        )
{
    Factor(expression, true);
}

inline void DeepFactor(
        Expr&     expression,
        Expr_info factor
        )
{
    Factor(expression, factor, true);
}

inline void DeepFactor(
        Expr       &expression,
        Expr const &factor
        )
{
    Factor(expression, factor, true);
}

[[nodiscard]]
inline Expr DeepFactored(
        const Expr& expression
        )
{
    return Factored(expression, true);
}

[[nodiscard]]
inline Expr DeepFactored(
        const Expr& expression,
        Expr_info   factor
        )
{
    return Factored(expression, factor, true);
}

[[nodiscard]]
inline Expr DeepFactored(
        const Expr& expression,
        const Expr& factor
        )
{
    return Factored(expression, factor, true);
}

void Collect(
        Expr &expr,
        std::vector<Expr> const &factors
        );
void DeepCollect(
        Expr &expr,
        std::vector<Expr> const &factors
        );
[[nodiscard]]
Expr Collected(
        Expr const &expr,
        std::vector<Expr> const &factors
        );
[[nodiscard]]
Expr DeepCollected(
        Expr const &expr,
        std::vector<Expr> const &factors
        );

[[nodiscard]]
Expr Distributed(Expr const& expression,
                 Expr_info   factor,
                 bool        full = false);

void Distribute(Expr&     expression,
                Expr_info factor,
                bool      full = false);

[[nodiscard]]
inline Expr Distributed(
        Expr const& expression,
        Expr const& factor,
        bool        full = false)
{
    return Distributed(expression, factor.get(), full);
}

inline void Distribute(
        Expr&       expression,
        Expr const& factor,
        bool        full = false)
{
    Distribute(expression, factor.get(), full);
}

[[nodiscard]]
inline Expr DeepDistributed(
        Expr const& expression,
        Expr_info   factor
        )
{
    return Distributed(expression, factor, true);
}

inline void DeepDistribute(
        Expr&     expression,
        Expr_info factor
        )
{
    Distribute(expression, factor, true);
}

[[nodiscard]]
inline Expr DeepDistributed(
        Expr const& expression,
        Expr const& factor)
{
    return Distributed(expression, factor, true);
}

inline void DeepDistribute(
        Expr&       expression,
        Expr const& factor
        )
{
    Distribute(expression, factor, true);
}

[[nodiscard]]
Expr Distributed(Expr const& expression,
                 Parent_info   factor,
                 bool        full = false);

void Distribute(Expr&     expression,
                Parent_info factor,
                bool      full = false);

[[nodiscard]]
inline Expr Distributed(
        Expr const& expression,
        Parent const& factor,
        bool        full = false)
{
    return Distributed(expression, factor.get(), full);
}

inline void Distribute(
        Expr&       expression,
        Parent const& factor,
        bool        full = false)
{
    Distribute(expression, factor.get(), full);
}

[[nodiscard]]
inline Expr DeepDistributed(
        Expr const& expression,
        Parent_info   factor
        )
{
    return Distributed(expression, factor, true);
}

inline void DeepDistribute(
        Expr&     expression,
        Parent_info factor
        )
{
    Distribute(expression, factor, true);
}

[[nodiscard]]
inline Expr DeepDistributed(
        Expr const& expression,
        Parent const& factor)
{
    return Distributed(expression, factor, true);
}

inline void DeepDistribute(
        Expr&       expression,
        Parent const& factor
        )
{
    Distribute(expression, factor, true);
}

[[nodiscard]]
Expr DistributedIf(
        Expr const& expression,
        std::function<bool(Expr const&)> f,
        bool full = false
        );

void DistributeIf(
        Expr&       expression,
        std::function<bool(Expr const&)> f,
        bool full = false
        );

[[nodiscard]]
inline Expr DeepDistributedIf(
        Expr const& expression,
        std::function<bool(Expr const&)> f
        )
{
    return DistributedIf(expression, f, true);
}

inline void DeepDistributeIf(
        Expr&       expression,
        std::function<bool(Expr const&)> f
        )
{
    return DistributeIf(expression, f, true);
}

[[nodiscard]]
Expr GetTerm(const Expr& expression);

[[nodiscard]]
Expr Derived(const Expr& expression,
             Expr_info   variable);

[[nodiscard]]
inline Expr Derived(
        const Expr& expression,
        const Expr& variable)
{
    return Derived(expression, variable.get());
}

inline void Derive(
        Expr& expression,
        Expr_info   variable)
{
    expression = Derived(expression, variable);
}

inline void Derive(
        Expr& expression,
        const Expr& variable)
{
    expression = Derived(expression, variable);
}

[[nodiscard]]
Expr GetPolynomialTerm(const Expr& expression,
                       Expr_info   variable,
                       int order);

[[nodiscard]]
inline Expr GetPolynomialTerm(
        const Expr& expression,
        const Expr& variable,
        int order)
{
    return GetPolynomialTerm(expression, variable.get(), order);
}

[[nodiscard]]
Expr Evaluated(const Expr& expression);

[[nodiscard]]
Expr Evaluated(const Expr& expression, 
               csl::eval::mode user_mode);

inline void Evaluate(Expr &expr)
{
    expr = Evaluated(expr);
}

inline void Evaluate(
        Expr &expr,
        eval::mode user_mode)
{
    expr = Evaluated(expr, user_mode);
}

[[nodiscard]]
ComplexProperty GetComplexProperty(Expr const &expr);

void SetComplexProperty(
        Expr &expr,
        ComplexProperty prop
        );

ComplexProperty GetComplexProperty(csl::Parent const &parent);

void SetComplexProperty(
        csl::Parent &parent,
        ComplexProperty prop
        );

template<class T>
inline void SetComplexProperty(
        T parent,
        ComplexProperty prop
        )
{
    csl::Parent p(parent);
    SetComplexProperty(p, prop);
}

[[nodiscard]]
Expr GetRealPart(const Expr& expression);

[[nodiscard]]
Expr GetImaginaryPart(const Expr& expression);

[[nodiscard]]
Expr GetComplexModulus(const Expr& expression);

[[nodiscard]]
Expr GetComplexArgument(const Expr& expression);

[[nodiscard]]
Expr GetComplexConjugate(const Expr& expression);


[[nodiscard]]
Expr GetTransposed(const Expr&  expression,
                   const Space* space,
                   bool         applyProp = true);

[[nodiscard]]
Expr GetTransposed(const Expr&                      expression,
                   const std::vector<const Space*>& spaces,
                   bool                             applyProp = true);

[[nodiscard]]
Expr GetHermitianConjugate(const Expr&  expression,
                           const Space* space);

[[nodiscard]]
Expr GetHermitianConjugate(const Expr&                      expression,
                           const std::vector<const Space*>& spaces);

[[nodiscard]]
Expr Swapped(const Expr&  expression,
          const Index& index1,
          const Index& index2,
          bool         refresh = true);

[[nodiscard]]
Expr Swapped(const Expr&  expression,
          const Expr& index1,
          const Expr& index2,
          bool         refresh = true);

[[nodiscard]]
Expr Swapped(const Expr&  expression,
          const Parent& index1,
          const Parent& index2,
          bool         refresh = true);

template<class T>
inline void Swap(
        Expr &expression,
        T const &old_obj,
        T const &new_obj
        )
{
    expression = Swapped(expression, old_obj, new_obj);
}

// void ReplaceInPlace(
//         Expr&        expression,
//         const Index& oldIndex,
//         const Index& newIndex);
// 
[[nodiscard]]
Expr ContractIndex(const Expr&  expression,
                   const Index& index);

[[nodiscard]]
csl::Index GenerateIndex(const csl::Space*  space,
                         const std::string& name = "");

[[nodiscard]]
csl::Tensor GetDelta(const csl::Space* space);
[[nodiscard]]
csl::Tensor GetMetric(const csl::Space* space);
[[nodiscard]]
csl::Tensor GetEpsilon(const csl::Space* space);

[[nodiscard]]
csl::Tensor GetDelta(const csl::Space& space);
[[nodiscard]]
csl::Tensor GetMetric(const csl::Space& space);
[[nodiscard]]
csl::Tensor GetEpsilon(const csl::Space& space);

[[nodiscard]]
Parent GetParent(Expr const &tensor);
[[nodiscard]]
Tensor GetTensorParent(Expr const &tensor);
[[nodiscard]]
TensorField GetTensorFieldParent(Expr const &tensor);
[[nodiscard]]
TDerivative GetTDerivativeParent(Expr const &tensor);

void AddSelfContraction(
        csl::Tensor &parent,
        Expr const &A,
        Expr const &B,
        Expr const &res
        );

void AddContractionProperty(
        csl::Tensor &parent,
        std::vector<Expr> const &tensors,
        Expr const &res
        );

void AddComplexProperty(
        csl::Tensor &parent,
        Expr const &A,
        Expr const &B
        );

void AddTransposedProperty(
        csl::Tensor &parent,
        csl::Space const *space,
        Expr const &A,
        Expr const &B
        );

void AddHermitianProperty(
        csl::Tensor &parent,
        csl::Space const *space,
        Expr const &A,
        Expr const &B
        );

[[nodiscard]]
LibDependency GetLibraryDependencies(Expr const& expression);

[[nodiscard]]
inline bool IsIndexed(Expr const &expr) {
    return expr->isIndexed();
}
[[nodiscard]]
inline bool IsNumerical(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Numerical;
}
[[nodiscard]]
inline bool IsLiteral(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Literal;
}
[[nodiscard]]
inline bool IsScalarFunction(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::ScalarFunction;
}
[[nodiscard]]
inline bool IsMultiFunction(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::MultiFunction;
}
[[nodiscard]]
inline bool IsVectorial(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Vectorial;
}
[[nodiscard]]
inline bool IsIndicial(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Indicial;
}
[[nodiscard]]
inline bool IsField(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Field;
}
[[nodiscard]]
inline bool IsArbitrary(Expr const &expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Arbitrary;
}

[[nodiscard]]
inline bool IsInteger(Expr const &expr) {
    return expr->getType() == csl::Type::Integer;
}
[[nodiscard]]
inline bool IsFloat(Expr const &expr) {
    return expr->getType() == csl::Type::Float;
}
[[nodiscard]]
inline bool IsIntFraction(Expr const &expr) {
    return expr->getType() == csl::Type::IntFraction;
}
[[nodiscard]]
inline bool IsIntFactorial(Expr const &expr) {
    return expr->getType() == csl::Type::IntFactorial;
}
[[nodiscard]]
inline bool IsComplex(Expr const &expr) {
    return expr->getType() == csl::Type::Complex;
}
[[nodiscard]]
inline bool IsNumericalEval(Expr const &expr) {
    return expr->getType() == csl::Type::NumericalEval;
}
[[nodiscard]]
inline bool IsImaginary(Expr const &expr) {
    return expr->getType() == csl::Type::Imaginary;
}
[[nodiscard]]
inline bool IsConstant(Expr const &expr) {
    return expr->getType() == csl::Type::Constant;
}
[[nodiscard]]
inline bool IsVariable(Expr const &expr) {
    return expr->getType() == csl::Type::Variable;
}
[[nodiscard]]
inline bool IsExp(Expr const &expr) {
    return expr->getType() == csl::Type::Exp;
}
[[nodiscard]]
inline bool IsLog(Expr const &expr) {
    return expr->getType() == csl::Type::Log;
}
[[nodiscard]]
inline bool IsDiracDelta(Expr const &expr) {
    return expr->getType() == csl::Type::DiracDelta;
}
[[nodiscard]]
inline bool IsAbs(Expr const &expr) {
    return expr->getType() == csl::Type::Abs;
}
[[nodiscard]]
inline bool IsCos(Expr const &expr) {
    return expr->getType() == csl::Type::Cos;
}
[[nodiscard]]
inline bool IsSin(Expr const &expr) {
    return expr->getType() == csl::Type::Sin;
}
[[nodiscard]]
inline bool IsTan(Expr const &expr) {
    return expr->getType() == csl::Type::Tan;
}
[[nodiscard]]
inline bool IsCosh(Expr const &expr) {
    return expr->getType() == csl::Type::Cosh;
}
[[nodiscard]]
inline bool IsSinh(Expr const &expr) {
    return expr->getType() == csl::Type::Sinh;
}
[[nodiscard]]
inline bool IsTanh(Expr const &expr) {
    return expr->getType() == csl::Type::Tanh;
}
[[nodiscard]]
inline bool IsACos(Expr const &expr) {
    return expr->getType() == csl::Type::ACos;
}
[[nodiscard]]
inline bool IsASin(Expr const &expr) {
    return expr->getType() == csl::Type::ASin;
}
[[nodiscard]]
inline bool IsATan(Expr const &expr) {
    return expr->getType() == csl::Type::ATan;
}
[[nodiscard]]
inline bool IsACosh(Expr const &expr) {
    return expr->getType() == csl::Type::ACosh;
}
[[nodiscard]]
inline bool IsASinh(Expr const &expr) {
    return expr->getType() == csl::Type::ASinh;
}
[[nodiscard]]
inline bool IsATanh(Expr const &expr) {
    return expr->getType() == csl::Type::ATanh;
}
[[nodiscard]]
inline bool IsScalar(Expr const &expr) {
    return expr->getType() == csl::Type::Scalar;
}
[[nodiscard]]
inline bool IsRealPart(Expr const &expr) {
    return expr->getType() == csl::Type::RealPart;
}
[[nodiscard]]
inline bool IsImaginaryPart(Expr const &expr) {
    return expr->getType() == csl::Type::ImaginaryPart;
}
[[nodiscard]]
inline bool IsITensor(Expr const &expr) {
    return expr->getType() == csl::Type::TensorElement;
}
[[nodiscard]]
inline bool IsScalarField(Expr const &expr) {
    return expr->getType() == csl::Type::ScalarField;
}
[[nodiscard]]
inline bool IsTensorField(Expr const &expr) {
    return expr->getType() == csl::Type::TensorFieldElement;
}
[[nodiscard]]
inline bool IsTensorialDerivative(Expr const &expr) {
    return expr->getType() == csl::Type::TDerivativeElement;
}
[[nodiscard]]
inline bool IsStandardDuo(Expr const &expr) {
    return expr->getType() == csl::Type::StandardDuo;
}
[[nodiscard]]
inline bool IsStandardMult(Expr const &expr) {
    return expr->getType() == csl::Type::StandardMult;
}
[[nodiscard]]
inline bool IsPow(Expr const &expr) {
    return expr->getType() == csl::Type::Pow;
}
[[nodiscard]]
inline bool IsProd(Expr const &expr) {
    return expr->getType() == csl::Type::Prod;
}
[[nodiscard]]
inline bool IsSum(Expr const &expr) {
    return expr->getType() == csl::Type::Sum;
}
[[nodiscard]]
inline bool IsPolynomial(Expr const &expr) {
    return expr->getType() == csl::Type::Polynomial;
}
[[nodiscard]]
inline bool IsDerivative(Expr const &expr) {
    return expr->getType() == csl::Type::Derivative;
}
[[nodiscard]]
inline bool IsCommutator(Expr const &expr) {
    return expr->getType() == csl::Type::Commutator;
}
[[nodiscard]]
inline bool IsAngle(Expr const &expr) {
    return expr->getType() == csl::Type::Angle;
}
[[nodiscard]]
inline bool IsFactorial(Expr const &expr) {
    return expr->getType() == csl::Type::Factorial;
}
[[nodiscard]]
inline bool IsIntegral(Expr const &expr) {
    return expr->getType() == csl::Type::Integral;
}
[[nodiscard]]
inline bool IsScalarIntegral(Expr const &expr) {
    return expr->getType() == csl::Type::ScalarIntegral;
}
[[nodiscard]]
inline bool IsVectorIntegral(Expr const &expr) {
    return expr->getType() == csl::Type::VectorIntegral;
}
[[nodiscard]]
inline bool IsVector(Expr const &expr) {
    return expr->getType() == csl::Type::Vector;
}
[[nodiscard]]
inline bool IsMatrix(Expr const &expr) {
    return expr->getType() == csl::Type::Matrix;
}
[[nodiscard]]
inline bool IsHighDTensor(Expr const &expr) {
    return expr->getType() == csl::Type::HighDTensor;
}

[[nodiscard]]
inline bool IsIndexed(Expr_info expr) {
    return expr->isIndexed();
}
[[nodiscard]]
inline bool IsNumerical(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Numerical;
}
[[nodiscard]]
inline bool IsLiteral(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Literal;
}
[[nodiscard]]
inline bool IsScalarFunction(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::ScalarFunction;
}
[[nodiscard]]
inline bool IsMultiFunction(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::MultiFunction;
}
[[nodiscard]]
inline bool IsVectorial(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Vectorial;
}
[[nodiscard]]
inline bool IsIndicial(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Indicial;
}
[[nodiscard]]
inline bool IsField(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Field;
}
[[nodiscard]]
inline bool IsArbitrary(Expr_info expr) {
    return expr->getPrimaryType() == csl::PrimaryType::Arbitrary;
}

[[nodiscard]]
inline bool IsInteger(Expr_info expr) {
    return expr->getType() == csl::Type::Integer;
}
[[nodiscard]]
inline bool IsFloat(Expr_info expr) {
    return expr->getType() == csl::Type::Float;
}
[[nodiscard]]
inline bool IsIntFraction(Expr_info expr) {
    return expr->getType() == csl::Type::IntFraction;
}
[[nodiscard]]
inline bool IsIntFactorial(Expr_info expr) {
    return expr->getType() == csl::Type::IntFactorial;
}
[[nodiscard]]
inline bool IsComplex(Expr_info expr) {
    return expr->getType() == csl::Type::Complex;
}
[[nodiscard]]
inline bool IsNumericalEval(Expr_info expr) {
    return expr->getType() == csl::Type::NumericalEval;
}
[[nodiscard]]
inline bool IsImaginary(Expr_info expr) {
    return expr->getType() == csl::Type::Imaginary;
}
[[nodiscard]]
inline bool IsConstant(Expr_info expr) {
    return expr->getType() == csl::Type::Constant;
}
[[nodiscard]]
inline bool IsVariable(Expr_info expr) {
    return expr->getType() == csl::Type::Variable;
}
[[nodiscard]]
inline bool IsExp(Expr_info expr) {
    return expr->getType() == csl::Type::Exp;
}
[[nodiscard]]
inline bool IsLog(Expr_info expr) {
    return expr->getType() == csl::Type::Log;
}
[[nodiscard]]
inline bool IsDiracDelta(Expr_info expr) {
    return expr->getType() == csl::Type::DiracDelta;
}
[[nodiscard]]
inline bool IsAbs(Expr_info expr) {
    return expr->getType() == csl::Type::Abs;
}
[[nodiscard]]
inline bool IsCos(Expr_info expr) {
    return expr->getType() == csl::Type::Cos;
}
[[nodiscard]]
inline bool IsSin(Expr_info expr) {
    return expr->getType() == csl::Type::Sin;
}
[[nodiscard]]
inline bool IsTan(Expr_info expr) {
    return expr->getType() == csl::Type::Tan;
}
[[nodiscard]]
inline bool IsCosh(Expr_info expr) {
    return expr->getType() == csl::Type::Cosh;
}
[[nodiscard]]
inline bool IsSinh(Expr_info expr) {
    return expr->getType() == csl::Type::Sinh;
}
[[nodiscard]]
inline bool IsTanh(Expr_info expr) {
    return expr->getType() == csl::Type::Tanh;
}
[[nodiscard]]
inline bool IsACos(Expr_info expr) {
    return expr->getType() == csl::Type::ACos;
}
[[nodiscard]]
inline bool IsASin(Expr_info expr) {
    return expr->getType() == csl::Type::ASin;
}
[[nodiscard]]
inline bool IsATan(Expr_info expr) {
    return expr->getType() == csl::Type::ATan;
}
[[nodiscard]]
inline bool IsACosh(Expr_info expr) {
    return expr->getType() == csl::Type::ACosh;
}
[[nodiscard]]
inline bool IsASinh(Expr_info expr) {
    return expr->getType() == csl::Type::ASinh;
}
[[nodiscard]]
inline bool IsATanh(Expr_info expr) {
    return expr->getType() == csl::Type::ATanh;
}
[[nodiscard]]
inline bool IsScalar(Expr_info expr) {
    return expr->getType() == csl::Type::Scalar;
}
[[nodiscard]]
inline bool IsRealPart(Expr_info expr) {
    return expr->getType() == csl::Type::RealPart;
}
[[nodiscard]]
inline bool IsImaginaryPart(Expr_info expr) {
    return expr->getType() == csl::Type::ImaginaryPart;
}
[[nodiscard]]
inline bool IsITensor(Expr_info expr) {
    return expr->getType() == csl::Type::TensorElement;
}
[[nodiscard]]
inline bool IsScalarField(Expr_info expr) {
    return expr->getType() == csl::Type::ScalarField;
}
[[nodiscard]]
inline bool IsTensorField(Expr_info expr) {
    return expr->getType() == csl::Type::TensorFieldElement;
}
[[nodiscard]]
inline bool IsTensorialDerivative(Expr_info expr) {
    return expr->getType() == csl::Type::TDerivativeElement;
}
[[nodiscard]]
inline bool IsStandardDuo(Expr_info expr) {
    return expr->getType() == csl::Type::StandardDuo;
}
[[nodiscard]]
inline bool IsStandardMult(Expr_info expr) {
    return expr->getType() == csl::Type::StandardMult;
}
[[nodiscard]]
inline bool IsPow(Expr_info expr) {
    return expr->getType() == csl::Type::Pow;
}
[[nodiscard]]
inline bool IsProd(Expr_info expr) {
    return expr->getType() == csl::Type::Prod;
}
[[nodiscard]]
inline bool IsSum(Expr_info expr) {
    return expr->getType() == csl::Type::Sum;
}
[[nodiscard]]
inline bool IsPolynomial(Expr_info expr) {
    return expr->getType() == csl::Type::Polynomial;
}
[[nodiscard]]
inline bool IsDerivative(Expr_info expr) {
    return expr->getType() == csl::Type::Derivative;
}
[[nodiscard]]
inline bool IsCommutator(Expr_info expr) {
    return expr->getType() == csl::Type::Commutator;
}
[[nodiscard]]
inline bool IsAngle(Expr_info expr) {
    return expr->getType() == csl::Type::Angle;
}
[[nodiscard]]
inline bool IsFactorial(Expr_info expr) {
    return expr->getType() == csl::Type::Factorial;
}
[[nodiscard]]
inline bool IsIntegral(Expr_info expr) {
    return expr->getType() == csl::Type::Integral;
}
[[nodiscard]]
inline bool IsScalarIntegral(Expr_info expr) {
    return expr->getType() == csl::Type::ScalarIntegral;
}
[[nodiscard]]
inline bool IsVectorIntegral(Expr_info expr) {
    return expr->getType() == csl::Type::VectorIntegral;
}
[[nodiscard]]
inline bool IsVector(Expr_info expr) {
    return expr->getType() == csl::Type::Vector;
}
[[nodiscard]]
inline bool IsMatrix(Expr_info expr) {
    return expr->getType() == csl::Type::Matrix;
}
[[nodiscard]]
inline bool IsHighDTensor(Expr_info expr) {
    return expr->getType() == csl::Type::HighDTensor;
}

/*!
 * \brief Declares a self contraction property just the time of a refresh in 
 * order to apply it on one expression.
 * \param init   Expression on which we apply the property.
 * \param tensor Indicial tensor that has the property.
 * \param A      First tensor in the contraction.
 * \param B      Second tensor in the contraction.
 * \param res    Result of the contraction.
 * \sa ApplyChainProperty(), AddSelfContraction()
 * , TensorParent::addSelfContraction().
 */
void ApplySelfProperty(Expr&         init,
                       csl::Tensor& tensor,
                       Expr const&   A,
                       Expr const&   B,
                       Expr const&   res);

/*!
 * \brief Declares a chain contraction property just the time of a refresh
 * in order to apply it on one expression.
 * \param init   Expression on which we apply the property.
 * \param tensor Indicial tensor that has the property.
 * \param prod   Tensors in the contraction.
 * \param res    Result of the contraction.
 * \sa ApplyChainProperty(), AddSelfContraction()
 * , TensorParent::addSelfContraction().
 */
void ApplyChainProperty(Expr&         init,
                        csl::Tensor& tensor,
                        std::vector<Expr> const& prod,
                        Expr const&              res);

/*!
 * \brief Checks the validity of an expression.
 * \details Browses the whole expression and checks that every node of the 
 * tree is valid (!= nullptr).
 * \param init        Expression to check.
 * \param encountered List of expressions already checked. Should not be 
 * given by the user in general.
 * \return \b True  if the expression is valid.
 * \return \b False else.
 */
bool CheckValidity(Expr const& init,
                   std::vector<Expr_info> encountered 
                        = std::vector<Expr_info>());

/*!
 * \brief Calculates an estimate of the total memory that an expression 
 * takes. 
 * \details This function is not exact. It should be extended. The problem
 * is that it is not possible for class with pointers to determine 
 * automatically the size of all elements and pointed elements. It has to be
 * hard coded. This function ignores all pointed elements that are not 
 * accessible by the Abstract::getArgument() function.
 * \param expression Expression from which we measure the size.
 * \return The total (recursive) size in bytes of the expression tree, 
 * ignoring all non-Expr 
 * pointed elements.
 */
std::size_t MemorySizeOf(Expr const& expression);

} // End of namespace csl

#endif
