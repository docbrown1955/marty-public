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

#include "librarydependency.h"
#include "abreviation.h"
#include "interface.h"
#include "indicial.h"
#include "tensorField.h"
#include "replace.h"
#include "space.h"
#include "utils.h"
#include "algo.h"
#include "lock.h"

#define OPT(expr, func) ((expr->func).value_or(expr))

using namespace std;

namespace csl {

Expr GetSymbol(
        std::string_view name,
        Expr      const &init
        )
{
    Expr res;
    Expr cpy = init;
    csl::FirstOfLeaf(cpy, [&](Expr const &sub)
    {
        if (sub->getName() == name) {
            res = sub;
            return true;
        }
        return false;
    });

    return res;
}

Parent GetParent(
        std::string_view name,
        Expr      const &init
        )
{
    Expr res = GetSymbol(name, init);
    if (res)
        return res->getParent();
    return nullptr;
}

void Expand(Expr& expression, bool full, bool inplace)
{
    expression = OPT(expression, expand(full, inplace));
}

Expr Expanded(const Expr& expression, bool full, bool inplace)
{
    return OPT(expression, expand(full, inplace));
}

void ExpandIf(Expr& expression, 
              std::function<bool(Expr const&)> const& f,
              bool full,
              bool inplace)
{
    expression = OPT(expression, expand_if(f, full, inplace));
}

void DeepExpandIf_lock(
        Expr &expression,
        std::function<bool(Expr const&)> const &f,
        int lockID,
        bool inplace,
        bool refactor
        )
{
    csl::Lock::lock(expression, lockID, f);
    csl::DeepExpandIf(expression, [&](Expr const &sub) {
        return csl::AnyOfNodes(sub, [&](Expr const &node) {
            return f(node);
        });
    }, inplace);
    if (refactor) {
        csl::DeepFactor(expression);
    }
    csl::Lock::unlock(expression, lockID);
}

Expr DeepExpandedIf_lock(
        Expr const &expression,
        std::function<bool(Expr const&)> const &f,
        int lockID,
        bool inplace,
        bool refactor
        )
{
    Expr other = DeepCopy(expression);
    csl::DeepExpandIf_lock(other, f, lockID, inplace, refactor);
    return other;
}

Expr ExpandedIf(const Expr& expression, 
                std::function<bool(Expr const&)> const& f,
                bool full,
                bool inplace)
{
    return OPT(expression, expand_if(f, full, inplace));
}

void Factor(Expr& expression, bool full)
{
    expression = OPT(expression, factor(full));
}

Expr Factored(const Expr& expression, bool full)
{
    return OPT(expression, factor(full));
}

void Factor(Expr& expression, Expr_info factor, bool full)
{
    expression = OPT(expression, factor(factor, full));
}

Expr Factored(const Expr& expression, Expr_info factor, bool full)
{
    return OPT(expression, factor(factor, full));
}

void Collect(
        Expr &expr,
        std::vector<Expr> const &factors
        )
{
    auto res = expr->collect(factors, false);
    if (res)
        expr = *res;
}
void DeepCollect(
        Expr &expr,
        std::vector<Expr> const &factors
        )
{
    auto res = expr->collect(factors, true);
    if (res)
        expr = *res;
}
Expr Collected(
        Expr const &expr,
        std::vector<Expr> const &factors
        )
{
    Expr copy = expr;
    Collect(copy, factors);
    return copy;
}
Expr DeepCollected(
        Expr const &expr,
        std::vector<Expr> const &factors
        )
{
    Expr copy = expr;
    DeepCollect(copy, factors);
    return copy;
}

Expr Distributed(Expr const& expression,
                 Parent_info   factor,
                 bool        full)
{
    Expr res = DeepCopy(expression);
    Distribute(res, factor, full);
    return res;
}

void Distribute(Expr&     expr,
                Parent_info factor,
                bool      full)
{
    if (expr->getType() == csl::Type::Prod) {
        for (size_t i = 0; i != expr->size(); ++i) {
            if (IsIndicialTensor(expr[i])
                    and expr[i]->getParent_info() == factor) {
                std::vector<Expr> results;
                for (size_t j = 0; j != expr->size(); ++j)
                    if (i != j and (*expr)[j]->getType() == csl::Type::Sum) {
                        results.push_back(Copy(expr));
                        results.back()->setArgument(CSL_1, i);
                        Expr newSum = Expanded((*expr)[i] * (*expr)[j]);
                        results.back()->setArgument(newSum, j);
                    }
                if (results.size() > 0) {
                    for (auto& res : results)
                        res = Refreshed(res);
                    Expr finalRes = sum_s(results);
                    if (finalRes->getType() == csl::Type::Sum) {
                        for (auto& arg : *finalRes)
                            Distribute(arg, factor, full);
                    }
                    else if (full)
                        Distribute(finalRes, factor, true);
                    else
                        return;
                    finalRes = Refreshed(finalRes);
                    return;
                }
            }
        }
    }
    if (full and expr->size() > 0)
        for (size_t i = 0; i != expr->size(); ++i)
            Distribute((*expr)[i], factor, full);
}

Expr Distributed(Expr const& expression,
                 Expr_info   factor,
                 bool        full)
{
    Expr res = DeepCopy(expression);
    Distribute(res, factor, full);
    return res;
}

void Distribute(Expr&     expr,
                Expr_info factor,
                bool      full)
{
    if (expr->getType() == csl::Type::Prod) {
        for (size_t i = 0; i != expr->size(); ++i) {
            if (*expr->getArgument(i) == factor) {
                std::vector<Expr> results;
                for (size_t j = 0; j != expr->size(); ++j)
                    if (i != j and (*expr)[j]->getType() == csl::Type::Sum) {
                        results.push_back(Copy(expr));
                        results.back()->setArgument(CSL_1, i);
                        Expr newSum = Expanded((*expr)[i] * (*expr)[j]);
                        results.back()->setArgument(newSum, j);
                    }
                if (results.size() > 0) {
                    for (auto& res : results)
                        res = Refreshed(res);
                    Expr finalRes = sum_s(results);
                    if (finalRes->getType() == csl::Type::Sum) {
                        for (auto& arg : *finalRes)
                            Distribute(arg, factor, full);
                    }
                    else if (full)
                        Distribute(finalRes, factor, true);
                    else
                        return;
                    finalRes = Refreshed(finalRes);
                    return;
                }
            }
        }
    }
    if (full and expr->size() > 0)
        for (size_t i = 0; i != expr->size(); ++i)
            Distribute((*expr)[i], factor, full);
}

Expr DistributedIf(
        Expr const& expression,
        std::function<bool(Expr const&)> f,
        bool full
        )
{
    Expr res = DeepCopy(expression);
    DistributeIf(res, f, full);
    return res;
}

void DistributeIf(
        Expr&       expr,
        std::function<bool(Expr const&)> f,
        bool full
        )
{
    if (expr->getType() == csl::Type::Prod) {
        for (size_t i = 0; i != expr->size(); ++i) {
            if (f(expr[i])) {
                std::vector<Expr> results;
                for (size_t j = 0; j != expr->size(); ++j)
                    if (i != j and (*expr)[j]->getType() == csl::Type::Sum) {
                        results.push_back(Copy(expr));
                        results.back()->setArgument(CSL_1, i);
                        Expr newSum = Expanded((*expr)[i] * (*expr)[j]);
                        results.back()->setArgument(newSum, j);
                    }
                if (results.size() > 0) {
                    for (auto& res : results)
                        res = Refreshed(res);
                    Expr finalRes = sum_s(results);
                    if (finalRes->getType() == csl::Type::Sum) {
                        for (auto& arg : *finalRes)
                            DistributeIf(arg, f, full);
                    }
                    else if (full)
                        DistributeIf(finalRes, f, true);
                    else
                        return;
                    finalRes = Refreshed(finalRes);
                    return;
                }
            }
        }
    }
    if (full and expr->size() > 0)
        for (size_t i = 0; i != expr->size(); ++i)
            DistributeIf((*expr)[i], f, full);
}

Expr GetTerm(const Expr& expression)
{
    return OPT(expression, getTerm());
}

Expr Derived(const Expr& expression,
            Expr_info   variable)
{
    return OPT(expression, derive(variable));
}

Expr GetPolynomialTerm(const Expr& expression,
                       Expr_info   variable,
                       int order)
{
    return OPT(expression, getPolynomialTerm(variable, order));
}

Expr Evaluated(const Expr& expression)
{
    return DeepRefreshed(OPT(expression, evaluate()));
}

Expr Evaluated(const Expr& expression,
              csl::eval::mode user_mode)
{
    return DeepRefreshed(OPT(expression, evaluate(user_mode)));
}

ComplexProperty GetComplexProperty(Expr const &expr)
{
    return expr->getComplexProperty();
}

void SetComplexProperty(
        Expr &expr,
        ComplexProperty prop
        )
{
    expr->setComplexProperty(prop);
}

ComplexProperty GetComplexProperty(csl::Parent const &parent)
{
    return parent->getComplexProp();
}

void SetComplexProperty(
        csl::Parent &parent,
        ComplexProperty prop
        )
{
    parent->setComplexProperty(prop);
}

Expr GetRealPart(const Expr& expression)
{
    return OPT(expression, getRealPart());
}

Expr GetImaginaryPart(const Expr& expression)
{
    return expression->getImaginaryPart();
}

Expr GetComplexModulus(const Expr& expression)
{
    return OPT(expression, getComplexModulus());
}

Expr GetComplexArgument(const Expr& expression)
{
    return OPT(expression, getComplexArgument());
}

Expr GetComplexConjugate(const Expr& expression)
{
    return OPT(expression, getComplexConjugate());
}


Expr GetTransposed(const Expr&  expression,
                   const Space* space,
                   bool         applyProp)
{
    return OPT(expression, getTransposed(space, applyProp));
}

Expr GetTransposed(const Expr&                 expression,
                   const vector<const Space*>& spaces,
                   bool                        applyProp)
{
    return OPT(expression, getTransposed(spaces, applyProp));
}

Expr GetHermitianConjugate(const Expr&  expression,
                           const Space* space)
{
    return OPT(expression, getHermitianConjugate(space));
}

Expr GetHermitianConjugate(const Expr&                 expression,
                           const vector<const Space*>& spaces)
{
    return OPT(expression, getHermitianConjugate(spaces));
}

Expr Swapped(const Expr&  expression,
               const Index& index1,
               const Index& index2,
               bool         refresh)
{
    auto foo = index1;
    foo.setName("FOO_INDEX");
    Expr res = Replaced(expression,
                       index1,
                       static_cast<csl::Index const&>(foo),
                       false);
    res = Replaced(res,
                  index2,
                  index1,
                  false);
    res = Replaced(res,
                  static_cast<csl::Index const&>(foo),
                  index2,
                  false);
    if (refresh)
        return res->deepRefresh();
    return res;
}

Expr Swapped(const Expr&  expression,
               const Expr& index1,
               const Expr& index2,
               bool         refresh)
{
    Expr foo = constant_s("FOO");
    Expr res = Replaced(expression,
                       index1,
                       foo);
    res = Replaced(res,
                  index2,
                  index1);
    res = Replaced(res,
                  foo,
                  index2);
    if (refresh)
        return res->deepRefresh();
    return res;
}

Expr Swapped(const Expr&  expression,
          const Parent& parent1,
          const Parent& parent2,
          bool         refresh)
{
    Tensor foo = tensor_s("FOO", &Euclid_R2);
    Expr res = Replaced(expression,
                       parent1,
                       foo);
    res = Replaced(res,
                  parent2,
                  parent1);
    res = Replaced(res,
                  foo,
                  parent2);
    if (refresh)
        return res->deepRefresh();
    return res;
}

// void ReplaceInPlace(
//         Expr&        expression,
//         const Index& oldIndex,
//         const Index& newIndex
//         )
// {
//     csl::ForEachLeaf(
//             expression,
//             [&oldIndex, &newIndex](Expr& expr) {
//               if(IsIndicialTensor(expr)) {
//                 expr->replaceIndex(oldIndex, newIndex);
//               } 
//             });
// }

Expr ContractIndex(const Expr&  expression,
                   const Index& index)
{
    return OPT(expression, contractIndex(index));
}

csl::Index GenerateIndex(const csl::Space* space,
                         const std::string& name)
{
    if (name.empty())
        return space->generateIndex();
    return space->generateIndex(name);
}

csl::Tensor GetDelta(const csl::Space* space)
{
    return space->getDelta();
}

csl::Tensor GetMetric(const csl::Space* space)
{
    return space->getMetric();
}

csl::Tensor GetEpsilon(const csl::Space* space)
{
    return space->getEpsilon();
}

csl::Tensor GetDelta(const csl::Space& space)
{
    return space.getDelta();
}

csl::Tensor GetMetric(const csl::Space& space)
{
    return space.getMetric();
}

csl::Tensor GetEpsilon(const csl::Space& space)
{
    return space.getEpsilon();
}

Parent GetParent(Expr const &tensor)
{
    return tensor->getParent();
}

Tensor GetTensorParent(Expr const &tensor)
{
    auto parent = std::dynamic_pointer_cast<TensorParent>(
            GetParent(tensor));
    CSL_ASSERT_SPEC(parent,
            CSLError::TypeError,
            "Expression " + toString(tensor) + " has no tensor parent.");
    return parent;
}
TensorField GetTensorFieldParent(Expr const &tensor)
{
    auto parent = std::dynamic_pointer_cast<TensorFieldParent>(
            GetParent(tensor));
    CSL_ASSERT_SPEC(parent,
            CSLError::TypeError,
            "Expression " + toString(tensor) + " has no tensor field parent.");
    return parent;
}
TDerivative GetTDerivativeParent(Expr const &tensor)
{
    auto parent = std::dynamic_pointer_cast<TDerivativeParent>(
            GetParent(tensor));
    CSL_ASSERT_SPEC(parent,
            CSLError::TypeError,
            "Expression " + toString(tensor) 
            + " has no tensorderivative parent.");
    return parent;
}


void AddSelfContraction(
        csl::Tensor &parent,
        Expr const &A,
        Expr const &B,
        Expr const &res
        )
{
    parent->addSelfContraction(A, B, res);
}

void AddContractionProperty(
        csl::Tensor &parent,
        std::vector<Expr> const &tensors,
        Expr const &res
        )
{
    parent->addContractionProperty(tensors, res);
}

void AddComplexProperty(
        csl::Tensor &parent,
        Expr const &A,
        Expr const &B
        )
{
    parent->addComplexProperty(A, B);
}

void AddTransposedProperty(
        csl::Tensor &parent,
        csl::Space const *space,
        Expr const &A,
        Expr const &B
        )
{
    parent->addTransposedProperty(space, A, B);
}

void AddHermitianProperty(
        csl::Tensor &parent,
        csl::Space const *space,
        Expr const &A,
        Expr const &B
        )
{
    parent->addHermitianProperty(space, A, B);
}

LibDependency GetLibraryDependencies(Expr const& expr)
{
    LibDependency dependencies;
    csl::VisitEachNode(expr, [&](Expr const& node)
    {
        if (csl::Abbrev::isAnAbbreviation(node)) {
            dependencies += GetLibraryDependencies(
                    node->getParent_info()->getEncapsulated());
        }
        else {
            dependencies += node->getLibDependency();
        }
    });

    return dependencies;
}

void ApplySelfProperty(Expr&         init,
                       csl::Tensor& tensor,
                       Expr const&   A,
                       Expr const&   B,
                       Expr const&   res)
{
    tensor->addSelfContraction(A, B, res);
    init = DeepRefreshed(init);
    tensor->removeSelfContraction(A, B);
}

void ApplyChainProperty(Expr&         init,
                        csl::Tensor& tensor,
                        std::vector<Expr> const& prod,
                        Expr const&              res)
{
    tensor->addContractionProperty(prod, res);
    init = DeepRefreshed(init);
    tensor->removeContractionProperty(prod, res);
}

bool CheckValidity(Expr const& init,
                   std::vector<Expr_info> encountered)
{
    if (not init)
        return false;
    if (std::find(encountered.begin(), encountered.end(), init.get())
            != encountered.end())
        return false;
    encountered.push_back(init.get());
    if (init->size() > 0){
        for (size_t i = 0; i != init->size(); ++i) {
            if (not CheckValidity((*init)[i], encountered))
                return false;
        }
    }
    return true;
}

size_t MemorySizeOf(Expr const& expression)
{
    size_t size = sizeof(Expr);
    size += sizeof(decltype(*expression));
    size += expression->memoryOverhead();
    if (expression->size() > 0) {
        if (expression->getPrimaryType() == csl::PrimaryType::ScalarFunction)
            size -= sizeof(Expr);
        if (expression->getPrimaryType() == csl::PrimaryType::MultiFunction
                and expression->getType() != csl::Type::Sum
                and expression->getType() != csl::Type::Prod)
            size -= 2*sizeof(Expr);
        for (size_t i = 0; i != expression->size(); ++i)
            size += MemorySizeOf((*expression)[i]);
    }
    if (csl::IsIndicialTensor(expression)) {
        size += sizeof(csl::IndexStructure);
        size += expression->getIndexStructureView().size()*sizeof(csl::Index);
    }

    return size;
}

} // End of namespace csl

#undef OPT
