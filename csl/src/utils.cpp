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

#include "utils.h"
#include "operations.h"
#include "mathFunctions.h"
#include "cslcomplex.h"
#include "indicial.h"
#include "field.h"
#include "tensorField.h"
#include "pseudoIntegral.h"
#include "space.h"
#include "comparison.h"
#include "commutation.h"
#include "algo.h"

using namespace std;

namespace csl {

std::vector<Expr> InverseTaylorExpand(
        Expr const& init,
        Expr const& big,
        size_t order)
{
    Expr eps = csl::variable_s("eps");
    Expr copy = Replaced(init, big, 1 / eps);
    std::vector<Expr> res = TaylorExpand(copy, eps, order);
    for (auto& expr : res) {
        expr = Replaced(expr, eps, 1 / big);
    }
    return res;
}

std::vector<Expr> TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order)
{
    Expr copy = DeepCopy(init);
    csl::Transform(copy, [&](Expr& expr)
    {
        if (expr->getType() == csl::Type::Sum) {
            int order = expr->getOrderOf(eps.get());
            if (order < 0) {
                for (auto& arg : *expr)
                    arg = arg * pow_s(eps, -order);
                expr = pow_s(eps, order) * expr;
                return true;
            }
        }
        return false;
    });

    return internal_TaylorExpand(copy, eps, order);
}

std::vector<Expr> internal_TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order)
{
    Expr f = init;
    Expr numFactor = CSL_1;
    Expr powerFactor = CSL_1;
    std::vector<Expr> res(order + 1);
    for (size_t i = 0; i != order+1; ++i) {
        Expr eval = Replaced(DeepCopy(f), eps, CSL_0);
        res[i] = powerFactor / numFactor * eval;
        if (i != order) {
            numFactor = numFactor * int_s(i+1);
            powerFactor = powerFactor * eps;
            f = f->derive(eps.get()).value_or(f);
        }
    }

    return res;
}

Expr CopySelf(Abstract* expr)
{
    return expr->copy();
}

Expr Copy(const Abstract* expr)
{    
    return expr->copy();
}

Expr Copy(const Expr& expr)
{
    return Copy(expr.get());
}

Expr DeepCopy(const Expr& expr)
{
    return DeepCopy(expr.get());
}

Expr DeepCopy(const Abstract* expr)
{
    return expr->deepCopy();
}

Expr Refreshed(const Abstract* expr)
{
    return expr->refresh();
}

Expr Refreshed(const Expr& expr)
{
    return expr->refresh();
}

Expr DeepRefreshed(const Expr& expr)
{
    return expr->deepRefresh();
}

void internal_ReplacedIndicial(
        Expr&           res,
        Expr const &old_abstract,
        Expr const &new_abstract)
{
    if (IsIndicialTensor(res) 
            and old_abstract->getParent().get() == res->getParent_info()) {
        if (new_abstract == CSL_0)
            res = CSL_0;
        else {
            IndexStructure structureToSave = res->getIndexStructure();
            Expr newRes = DeepCopy(new_abstract);
            RenameIndices(newRes);
            if (res->isComplexConjugate())
                res = GetComplexConjugate(newRes);
            else
                res = DeepCopy(newRes);
            if (not structureToSave.empty())
                ApplyIndices(
                        res, 
                        old_abstract->getIndexStructure(),
                        structureToSave);
        }
        return;
    }
    else if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) {
            Expr arg = res->getArgument(i);
            internal_ReplacedIndicial(
                    arg,
                    old_abstract,
                    new_abstract);
            res->setArgument(arg, i);
        }
    }
    else if (res->isAnOperator() and not res->isEmpty()) {
        Expr op = res->getOperand();
        internal_ReplacedIndicial(
                    op,
                    old_abstract,
                    new_abstract);
        res->setOperand(op);
    }
}

Expr ReplacedIndicial(
        Expr const &expr,
        Expr const &old_abstract,
        Expr const &new_abstract
        )
{
    CSL_ASSERT_SPEC(IsIndicialTensor(old_abstract),
            CSLError::TypeError,
            "Expected indicial tensor in replace, " + toString(expr) 
            + " given.");
    CSL_ASSERT_SPEC(old_abstract->getFreeIndexStructure() 
                == new_abstract->getFreeIndexStructure(),
                CSLError::ValueError,
                "Expected same index structures in replace for "
                + toString(old_abstract) + " and " + toString(new_abstract));
    Expr res = DeepCopy(expr);
    internal_ReplacedIndicial(
            res,
            old_abstract,
            new_abstract);
    return DeepRefreshed(res);
}

Expr Replaced(
        const Expr& expr,
        std::vector<Expr> const& old_abstract,
        std::vector<Expr> const& new_abstract)
{
    // if (old_abstract->getFreeIndexStructure().size() > 0)
    //     return ReplacedIndicial(expr, old_abstract, new_abstract);
    return internal_Replaced(expr, old_abstract, new_abstract);
}

Expr internal_Replaced(
        const Expr& expr,
        std::vector<Expr> const& old_abstract,
        std::vector<Expr> const& new_abstract)
{
    for (size_t i = 0; i != old_abstract.size(); ++i)  {
        if (*expr == old_abstract[i])
            return new_abstract[i];
        else if (*expr == GetComplexConjugate(old_abstract[i]))
            return GetComplexConjugate(new_abstract[i]);
        if (expr->getType() == csl::Type::Pow 
                and old_abstract[i]->getType() == csl::Type::Pow
                and expr->getArgument() == old_abstract[i]->getArgument()) {
            Expr ratio = expr->getArgument(1) 
                        / old_abstract[i]->getArgument(1);
            if (ratio->isInteger() and ratio->evaluateScalar() > 0) 
                return pow_s(new_abstract[i], ratio);
        }
    }

    Expr res = Copy(expr);
    if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) {
            res->setArgument(internal_Replaced(
                        res->getArgument(i),
                        old_abstract,
                        new_abstract), i);

        }
    }
    if (res->isAnOperator()) {
        res->setOperand(internal_Replaced(
                    res->getOperand(),
                    old_abstract,
                    new_abstract));
    }
    return Refreshed(res);
}

void Replace(
        Expr       &expr,
        std::vector<Expr> const &old_abstract,
        std::vector<Expr> const &new_abstract)
{
    expr = Replaced(
            static_cast<Expr const&>(expr), 
            static_cast<std::vector<Expr> const&>(old_abstract), 
            static_cast<std::vector<Expr> const&>(new_abstract)
            );
}

Expr Replaced(
        const Expr& expr,
        const Expr& old_abstract,
        const Expr& new_abstract)
{
    if (old_abstract->getFreeIndexStructure().size() > 0)
        return ReplacedIndicial(expr, old_abstract, new_abstract);
    return internal_Replaced(expr, old_abstract, new_abstract);
}

Expr internal_Replaced(
        const Expr& expr,
        const Expr& old_abstract,
        const Expr& new_abstract)
{
    if (*expr == old_abstract)
        return new_abstract;
    else if (*expr == GetComplexConjugate(old_abstract))
        return GetComplexConjugate(new_abstract);
    if (expr->getType() == csl::Type::Pow 
            and old_abstract->getType() == csl::Type::Pow
            and expr->getArgument() == old_abstract->getArgument()) {
        Expr ratio = expr->getArgument(1) / old_abstract->getArgument(1);
        if (ratio->isInteger() and ratio->evaluateScalar() > 0) 
            return pow_s(new_abstract, ratio);
    }

    Expr res = Copy(expr);
    if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) {
            res->setArgument(internal_Replaced(
                        res->getArgument(i),
                        old_abstract,
                        new_abstract), i);

        }
    }
    if (res->isAnOperator()) {
        res->setOperand(internal_Replaced(
                    res->getOperand(),
                    old_abstract,
                    new_abstract));
    }
    return Refreshed(res);
}

void ResetDummyIndices(Expr& expr)
{
    if (IsIndicialTensor(expr)) {
        IndexStructure index = expr->getIndexStructure();
        for (auto& i : index)
            if (not (i.getType() == cslIndex::Fixed))
                i.setFree(true);
        expr->setIndexStructure(index);
        expr = Refreshed(expr);
    }
    else if (expr->size() > 0) {
        for (size_t i = 0; i != expr->size(); ++i) {
            Expr arg = expr->getArgument(i);
            ResetDummyIndices(arg);
            expr->setArgument(arg, i);
        }
    }
    if (expr->isAnOperator()) {
        Expr operand = expr->getOperand();
        ResetDummyIndices(operand);
        expr->setOperand(operand);
    }
}

void ApplyIndices(Expr&           expr,
                  const IndexStructure& freeStructure)
{
    IndexStructure freeExprStructure = expr->getFreeIndexStructure();
    if (freeStructure.size() != freeExprStructure.size()) {
        expr->print();
        cout << expr->getType() << endl;
        cout << freeStructure << endl;
        cout << expr->getIndexStructure() << endl;
        cout << freeExprStructure << endl;
        callError(cslError::UndefinedBehaviour,
                "ApplyIndices(const Expr&, const IndexStructure&)");
    }
    expr = ReplaceIndices(expr, freeExprStructure, freeStructure);
    expr = RenamedIndices(expr);
}

void ApplyIndices(Expr&           expr,
                  const IndexStructure& oldStructure,
                  const IndexStructure& freeStructure)
{
    if (freeStructure.size() != oldStructure.size()) {
        expr->print();
        cout << expr->getType() << endl;
        cout << freeStructure << endl;
        cout << expr->getIndexStructure() << endl;
        cout << oldStructure << endl;
        callError(cslError::UndefinedBehaviour,
                "ApplyIndices(const Expr&, const IndexStructure&)");
    }
    expr = ReplaceIndices(expr, oldStructure, freeStructure);
}

Expr Replaced(const Expr&     expr,
             const Parent&  old_abstract,
             const Expr&     new_abstract)
{
    Expr res = DeepCopy(expr);
    csl::IndexStructure freeStruct = res->getFreeIndexStructure();
    ResetDummyIndices(res);
    Expr newAbstract = new_abstract;
    ResetDummyIndices(newAbstract);
    newAbstract = DeepRefreshed(newAbstract);
    internal_Replace(res,
                     old_abstract,
                     newAbstract);
    return DeepRefreshed(res);
}

void internal_Replace(Expr&           res,
                      const Parent&  old_abstract,
                      const Expr&     new_abstract)
{
    if ((IsIndicialTensor(res) 
                or IsConstant(res)
                or IsVariable(res))
            and old_abstract.get() == res->getParent_info()) {
        if (new_abstract == CSL_0)
            res = CSL_0;
        else {
            IndexStructure structureToSave = res->getFreeIndexStructure();
            if (res->isComplexConjugate())
                res = GetComplexConjugate(DeepCopy(new_abstract));
            else
                res = DeepCopy(new_abstract);
            if (not structureToSave.empty())
                ApplyIndices(res, structureToSave);
        }
        return;
    }
    else if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) {
            Expr arg = res->getArgument(i);
            internal_Replace(arg,
                             old_abstract,
                             new_abstract);
            res->setArgument(arg, i);
        }
    }
    else if (res->isAnOperator() and not res->isEmpty()) {
        Expr op = res->getOperand();
        internal_Replace(
                    op,
                    old_abstract,
                    new_abstract);
        res->setOperand(op);
    }
}

Expr Replaced(const Expr&     expr,
             std::vector<Parent> const&  old_abstract,
             std::vector<Expr> const&     new_abstract)
{
    Expr res = DeepCopy(expr);
    ResetDummyIndices(res);
    std::vector<Expr> newAbstract = new_abstract;
    for (auto &expr : newAbstract) {
        ResetDummyIndices(expr);
        DeepRefresh(expr);
    }
    internal_Replace(res,
                     old_abstract,
                     newAbstract);
    return DeepRefreshed(res);
}

void internal_Replace(Expr&           res,
                      std::vector<Parent> const&  old_abstract,
                      std::vector<Expr> const&     new_abstract)
{
    if (IsIndicialTensor(res)
            or IsConstant(res)
            or IsVariable(res))
        for (size_t i = 0; i != old_abstract.size(); ++i)  {
            if (old_abstract[i].get() == res->getParent_info()) {
                if (new_abstract[i] == CSL_0)
                    res = CSL_0;
                else {
                    IndexStructure structureToSave 
                        = res->getFreeIndexStructure();
                    if (res->isComplexConjugate())
                        res = GetComplexConjugate(DeepCopy(new_abstract[i]));
                    else
                        res = DeepCopy(new_abstract[i]);
                    if (not structureToSave.empty())
                        ApplyIndices(res, structureToSave);
                }
                return;
            }
        }
    else if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) {
            Expr arg = res->getArgument(i);
            internal_Replace(arg,
                             old_abstract,
                             new_abstract);
            res->setArgument(arg, i);
        }
    }
    if (res->isAnOperator() and not res->isEmpty()) {
        Expr op = res->getOperand();
        internal_Replace(
                    op,
                    old_abstract,
                    new_abstract);
        res->setOperand(op);
    }
}

Index internal_RenameIndex(const Index& index,
                           map<Index, Index>& mapping)
{
    if (auto pos = find_if(mapping.begin(), mapping.end(),
                [&index](const pair<Index,Index>& p) {
                    return p.second.getName() == index.getName()
                        and p.second.getID() == index.getID()
                        and p.second.getSpace() == index.getSpace();
                });
            pos != mapping.end())
        return index;

    if (auto pos = find_if(mapping.begin(), mapping.end(),
                [&index](const pair<Index,Index>& p) {
                    return p.first.getName() == index.getName()
                        and p.first.getID() == index.getID()
                        and p.first.getSpace() == index.getSpace();
                });
            pos != mapping.end()) {
        Index newIndex = index;
        newIndex.setName(string((*pos).second.getName()));
        newIndex.setID((*pos).second.getID());
        newIndex.setFree(index.getFree());
        return newIndex;
    }
    string name = string(index.getName());
    Index newIndex = index.getSpace()->generateIndex(name);
    newIndex.setSign(index.getSign());
    mapping[index] = newIndex;
    newIndex.setFree(index.getFree());
    return newIndex;
}

void RenameIndices(Expr& expr)
{
    // cout << "\n\n\n\n*****************************\n\n\n\n";
    map<Index, Index> mapping;
    // expr->print();
    internal_RenameIndices(expr, mapping);
    // cout << "RES = " << expr << endl;
}

void internal_RenameIndices(Expr& expr,
                            map<Index, Index>& mapping)
{
    // cout << "HERE\n";
    // for (const auto &m : mapping)
        // cout << m.first << " <-> " << m.second << endl;
    // expr->print();
    if (IsIndicialTensor(expr)) {
        IndexStructure index = expr->getIndexStructure();
        for (auto& i : index)
            if (i.getType() == cslIndex::Dummy) 
                i = internal_RenameIndex(i, mapping);
        expr->setIndexStructure(index);
    }
    
    for (size_t i = 0; i != expr->size(); ++i) {
        internal_RenameIndices((*expr)[i], mapping);
    }
    // cout << "END:\n";
    // for (const auto &m : mapping)
        // cout << m.first << " <-> " << m.second << endl;
    // expr->print();
}

Expr Replaced(const Expr&   expr,
             const Parent& old_parent,
             const Parent& new_parent,
             bool refresh)
{
    if (IsIndicialTensor(expr)
            or IsVariable(expr)
            or IsConstant(expr)) {
        if (expr->getParent_info() == old_parent.get()) {
            Expr res = Copy(expr);
            res->setParent(new_parent);
            return res;
        }
    }
    if (expr->getPrimaryType() == csl::PrimaryType::Field 
            and expr->getPoint().get() == old_parent.get()) {
        Expr foo = Copy(expr);
        foo->setPoint(dynamic_pointer_cast<TensorParent>(new_parent));
        return foo;
    }
    Expr foo = Copy(expr);
    for (size_t i=0; i<foo->size(); i++)
        foo->setArgument(
                Replaced(
                    foo[i],
                    old_parent, 
                    new_parent,
                    refresh),
                i);
    if (expr->isAnOperator())
        foo->setOperand(Replaced(foo->getOperand(), 
                                old_parent, 
                                new_parent,
                                refresh));
    if (refresh)
        foo = Refreshed(foo);
    return foo;
}

Expr Replaced(const Expr&   expr,
             std::vector<Parent> const& old_parent,
             std::vector<Parent> const& new_parent,
             bool refresh)
{
    if (IsIndicialTensor(expr)
            or IsVariable(expr)
            or IsConstant(expr)) {
        for (size_t i = 0; i != old_parent.size(); ++i)   
            if (expr->getParent_info() == old_parent[i].get()) {
                Expr res = Copy(expr);
                res->setParent(new_parent[i]);
                return res;
            }
    }
    if (expr->getPrimaryType() == csl::PrimaryType::Field) {
        for (size_t i = 0; i != old_parent.size(); ++i) 
            if (expr->getPoint().get() == old_parent[i].get()) {
                Expr foo = Copy(expr);
                foo->setPoint(dynamic_pointer_cast<TensorParent>(
                            new_parent[i]));
                return foo;
            } 
    }
    Expr foo = Copy(expr);
    for (size_t i=0; i<foo->size(); i++)
        foo->setArgument(Replaced(foo[i], 
                                 old_parent, 
                                 new_parent,
                                 refresh),i);
    if (expr->isAnOperator())
        foo->setOperand(Replaced(foo->getOperand(), 
                                old_parent, 
                                new_parent,
                                refresh));
    if (refresh)
        foo = Refreshed(foo);
    //cout << "RES = " << foo << endl;
    return foo;
}

csl::vector_expr applyOnAll(const csl::vector_expr& vec, 
        function<Expr(Expr)> func)
{
    csl::vector_expr newVec(0);
    for (auto& el : vec)
        newVec.push_back(func(el));

    return newVec;
}
} // End of namespace csl
