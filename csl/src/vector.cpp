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

#include <list>
#include "vector.h"
#include "literal.h"
#include "numerical.h"
#include "comparison.h"
#include "operations.h"
#include "mathFunctions.h"
#include "replace.h"
#include "interface.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractVectorial                       //
/*************************************************/
///////////////////////////////////////////////////

AbstractVectorial::AbstractVectorial(): Abstract()
{
    dim = 0;
    nArgs = 0;
    argument = csl::vector_expr(0);
    shape = vector<int>(0);
}

std::string const &AbstractVectorial::getName() const
{
    return argument[0]->getName();
}

void AbstractVectorial::print(
        int mode,
        std::ostream& out,
        bool) const
{
    if (mode == 0)
    {
        if (dim == 1) 
            out<<"Vec";
        else if (dim == 2) 
            out<<"Mat";
        else 
            out<<"Tensor";
    }
    out<<"{";
    for (int i=0; i<nArgs; i++) {
        argument[i]->print(1, out);
        if (i < nArgs-1) {
            out<<" , ";
            if (mode == 0 and dim == 1)
                out<<"\n    ";
            if (mode == 0 and dim == 2) 
                out<<"\n    ";
            if (mode == 0 and dim == 3) 
                out<<"\n       ";
            if (dim > 1) {
                for (int j=0; j<dim-1; j++)
                    out<<endl;
            }
        }
    }
    out<<"}";
    if (mode == 0) 
        out<<endl;
}

string AbstractVectorial::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0) {
        if (dim == 1) 
            sout<<"Vec";
        else if (dim == 2) 
            sout<<"Mat";
        else 
            sout<<"Tensor";
    }
    sout<<"{ ";
    for (int i=0; i<nArgs; i++) {
        argument[i]->print(1);
        if (i < nArgs-1)
            sout<<" , ";
    }
    sout<<"}";
    if (mode == 0) 
        sout<<endl;

    return sout.str();
}

void AbstractVectorial::printCode(
        int           mode,
        std::ostream &out
        ) const
{
    if (csl::IsVector(this)) {
        out << "csl::vector_s({";
        for (std::size_t i = 0; i != argument.size(); ++i) {
            argument[i]->printCode(1, out);
            if (i + 1 != argument.size())
                out << ", ";
        }
        out << "})";
    }
    else if (csl::IsMatrix(this)) {
        out << "csl::matrix_s({";
        for (std::size_t i = 0; i != argument.size(); ++i) {
            argument[i]->printCode(1, out);
            if (i + 1 != argument.size())
                out << ", \n";
        }
        out << "})";
    }
    else {
        out << "csl::highdtensor_from_args_s({";
        for (std::size_t i = 0; i != argument.size(); ++i) {
            argument[i]->printCode(1, out);
            if (i + 1 != argument.size())
                out << ", \n\n";
        }
        out << "})";
    }
    if (mode == 0)
        out << '\n';
}

std::vector<Parent> AbstractVectorial::getSubSymbols() const
{
    std::vector<Parent> dep;
    for (const auto &arg : *this) {
        std::vector<Parent> interm = arg->getSubSymbols();
        dep.insert(dep.end(),
                std::make_move_iterator(interm.begin()),
                std::make_move_iterator(interm.end()));
    }
    return dep;
}

optional<Expr> AbstractVectorial::evaluate(
        csl::eval::mode user_mode
        ) const
{
    if (dim == 0) 
        return CSL_0;
    if (nArgs > 0) {
        Expr foo = vectorialtensor_s(getShape());
        for (int i=0; i<nArgs; i++)
            foo->setArgument(Evaluated(argument[i], user_mode),i);
        return foo;
    }
    else 
        return CSL_0;
}

unique_Expr AbstractVectorial::copy_unique() const
{
    switch(getType()) {

        case csl::Type::Vector:
        return make_unique<Vector>(argument);

        case csl::Type::Matrix:
        return make_unique<Matrix>(argument);

        case csl::Type::HighDTensor:
        return make_unique<HighDTensor>(argument);

        default:
        CALL_SMERROR(CSLError::TypeError);
    }
    return nullptr;
}

Expr AbstractVectorial::deepCopy() const
{
    csl::vector_expr copyArg(argument.size());
    for (std::size_t i = 0; i != argument.size(); ++i)
        copyArg[i] = argument[i]->deepCopy();

    switch(getType()) {

        case csl::Type::Vector:
        return csl::make_shared<Vector>(copyArg);

        case csl::Type::Matrix:
        return csl::make_shared<Matrix>(copyArg);

        case csl::Type::HighDTensor:
        return csl::make_shared<HighDTensor>(copyArg);

        default:
        CALL_SMERROR(CSLError::TypeError);
    }
    return nullptr;
}

Expr AbstractVectorial::refresh() const
{
    switch(getType()) {

        case csl::Type::Vector:
        return vector_s(argument);

        case csl::Type::Matrix:
        return matrix_s(argument);

        case csl::Type::HighDTensor:
        return highdtensor_from_args_s(argument);

        default:
        CALL_SMERROR(CSLError::TypeError);
    }
    return nullptr;
}

Expr AbstractVectorial::deepRefresh() const
{
    csl::vector_expr refreshed(argument.size());
    for (std::size_t i = 0; i != argument.size(); ++i)
        refreshed[i] = argument[i]->deepRefresh();

    switch(getType()) {

        case csl::Type::Vector:
        return vector_s(refreshed);

        case csl::Type::Matrix:
        return matrix_s(refreshed);

        case csl::Type::HighDTensor:
        return highdtensor_from_args_s(refreshed);

        default:
        CALL_SMERROR(CSLError::TypeError);
    }
    return nullptr;
}

        
int AbstractVectorial::getNArgs(int axis) const
{
    if (axis >= dim or axis < 0) {
        cout<<"Warning: axis "<<axis<<" out of bounds of the vectorial expr.\n";
        return 0;
    }
    if (axis == 0) 
        return nArgs;
    if (nArgs == 0) 
        return 0;

    return argument[0]->getNArgs(axis-1);
}

std::size_t AbstractVectorial::size() const
{
    return argument.size();
}

csl::vector_expr::iterator AbstractVectorial::begin()
{
    return argument.begin();
}

csl::vector_expr::iterator AbstractVectorial::end()
{
    return argument.end();
}

bool AbstractVectorial::isReal() const
{
    for (const auto& arg : argument)
        if (not arg->isReal())
            return false;
    return true;
}

bool AbstractVectorial::isPurelyImaginary() const
{
    for (const auto& arg : argument)
        if (not arg->isPurelyImaginary())
            return false;
    return true;
}

optional<Expr> AbstractVectorial::getComplexConjugate()  const
{
    Expr res = Copy(this);
    for (auto& arg : *res)
        arg = GetComplexConjugate(arg);
    return res;
}

std::optional<Expr> AbstractVectorial::getRealPart() const
{
    Expr res = Copy(this);
    for (auto& arg : *res)
        arg = GetRealPart(arg);
    return res;
}

Expr AbstractVectorial::getImaginaryPart() const
{
    Expr res = Copy(this);
    for (auto& arg : *res)
        arg = GetImaginaryPart(arg);
    return res;
}

std::optional<Expr> AbstractVectorial::getComplexModulus() const
{
    Expr res = Copy(this);
    for (auto& arg : *res) {
        arg = GetComplexModulus(arg);
    }
    return res;
}

Expr const &AbstractVectorial::getArgument(int iArg) const
{
    if (iArg >= 0 and iArg < nArgs)
        return argument[iArg];
    print();
    CALL_SMERROR_SPEC(CSLError::IndexError,
                 toString(iArg) + " given for size "
                 + toString(size()) + ".");

    return CSL_0;
}

Expr& AbstractVectorial::getArgument(int iArg)
{
    return argument[iArg];
}

Expr const &AbstractVectorial::getArgument(const vector<int>& indices) const
{
    const int size = indices.size();
    if (size > dim or size == 0) {
        callWarning(cslError::InvalidDimension,
                "AbstractVectorial::getArgument",size);
        return CSL_0;
    }
    else if (size == 1)
        return getArgument(indices[0]);

    vector<int> newIndices(indices.size()-1);
    for (std::size_t i = 1; i < indices.size(); ++i)
        newIndices[i-1] = indices[i];

    return argument[indices[0]]->getArgument(newIndices);
}


Expr& AbstractVectorial::getArgument(const vector<int>& indices)
{
    const int size = indices.size();
    if (size == 1)
        return getArgument(indices[0]);

    vector<int> newIndices(indices.size()-1);
    for (std::size_t i = 1; i < indices.size(); ++i)
        newIndices[i-1] = indices[i];

    return argument[indices[0]]->getArgument(newIndices);
}

bool AbstractVectorial::dependsOn(Expr_info expr) const
{
    if (operator==(expr))
        return true;

    for (const auto& arg : argument)
        if (arg->dependsOn(expr)) 
            return true;

    return false;
}

bool AbstractVectorial::dependsExplicitlyOn(Expr_info expr) const
{
    if (operator==(expr))
        return true;

    for (const auto& arg : argument)
        if (arg->dependsExplicitlyOn(expr)) 
            return true;

    return false;
}

bool AbstractVectorial::dependsOn(Parent_info expr) const
{
    if (not expr->isValued())
        return false;
    return expr->dependsOn(this);
}

bool AbstractVectorial::dependsExplicitlyOn(Parent_info expr) const
{
    if (not expr->isValued())
        return false;
    return expr->dependsExplicitlyOn(this);
}

bool AbstractVectorial::commutesWith(Expr_info expr, int sign) const
{
    for (auto& arg : argument)
        if (not arg->commutesWith(expr, sign))
            return false;
    return true;
}

const csl::vector_expr& AbstractVectorial::getVectorArgument() const
{
    return argument;
}

vector<int> AbstractVectorial::getShape() const
{
    return shape;
}

void AbstractVectorial::setComplexProperty(csl::ComplexProperty t_prop)
{
    for (const auto& arg : argument)
        arg->setComplexProperty(t_prop);
}

void AbstractVectorial::setArgument(const Expr& expr, int iArg)
{
    if (iArg < nArgs and dim == 1+expr->getDim())
        argument[iArg] = expr;
    else
        cout<<"Warning: index "<<iArg<<" out of bound of vectorial expr.\n";
}

void AbstractVectorial::setVectorArgument(const csl::vector_expr& t_argument)
{
    if (t_argument.size() > 0) {
        if (t_argument[0]->getDim() != dim-1)
            cout<<"Warning: wrong dimension in setVectorArgument in AbstractVectorial.\n";
        else {
            argument = t_argument;
            nArgs = argument.size();
        }
    }
    else {
        nArgs = 0;
        argument = t_argument;
    }
}

void AbstractVectorial::setArgument(const Expr& expr,
                                    const vector<int>& indices)
{
    const int size = indices.size();
    if (size == 0 or size > dim) {
        callWarning(cslError::InvalidDimension,
"AbstractVectorial::setArgument(const Expr&, const vector<int>&)",size);
        return;
    }
    if (size == 1)
        argument[indices[0]] = expr;
    else
        argument[indices[0]]->setArgument(expr,
                vector<int>(indices.begin()+1,indices.end()));
}

bool AbstractVectorial::matchShape(Expr_info expr, bool exact) const
{
    if (exact) 
        return exactMatchShape(expr);
    if (dim == 0 or expr->getDim() == 0) 
        return true;

    // if no object of dimension 0, we test the matching of the last 
    // dimension of this and the first of expr (for a dot product)
    return (getNArgs(dim-1) == expr->getNArgs(0));
}

bool AbstractVectorial::exactMatchShape(Expr_info expr) const
{
    int t_dim = expr->getDim();
    if (dim == 0 or t_dim == 0) 
        return true;
    if (dim != t_dim) 
        return false;
    vector<int> shape1(getShape()), shape2(expr->getShape());
    if (shape1.size() != shape2.size()) 
        return false;
    for (std::size_t i=0; i<shape1.size(); i++)
        if (shape1[i] != shape2[i])
            return false;

    return true;
}

Expr AbstractVectorial::getSum() const
{
    if (dim == 1)
        return sum_s(argument);
    else {
        csl::vector_expr intermediateSums(nArgs);
        for (int i=0; i<nArgs; i++)
            intermediateSums[i] = argument[i]->getSum();

        return sum_s(intermediateSums);
    }
}

Expr AbstractVectorial::getProduct() const
{
    if (dim == 1)
        return prod_s(argument);
    else {
        csl::vector_expr intermediateProducts(nArgs);
        for (int i=0; i<nArgs; i++)
            intermediateProducts[i] = argument[i]->getProduct();
        
        return prod_s(intermediateProducts);
    }
}

Expr AbstractVectorial::getSubVectorial(const vector<int>& exceptions) const
{
    if ((int)exceptions.size() != dim) {
        cout<<"Warning: taking subVectorial with wrong number of dimensions.\n";
        return CSL_0;
    }
    vector<int> shape = getShape();
    for (int i=0; i<dim; i++)
        --shape[i];
    Expr foo = vectorialtensor_s(shape);
    if (dim > 1) {
        vector<int> newExceptions(exceptions);
        newExceptions.erase(newExceptions.begin());
        int i=0; 
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i) {
            if (i == exceptions[0])
                ++i;
            *arg = argument[i]->getSubVectorial(newExceptions);
        }
    }
    else {
        int i=0;
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i) {
            if (i == exceptions[0])
                ++i;
            *arg = argument[i];
        }
    }

    return foo;
}

Expr AbstractVectorial::getSubVectorial(
        const vector<vector<int>>& keepIndices) const
{
    return getSubVectorial(keepIndices.begin(), keepIndices.end());
}

Expr AbstractVectorial::getSubVectorial(
        std::vector<std::vector<int>>::const_iterator begin,
        std::vector<std::vector<int>>::const_iterator end) const
{
    const vector<int>& keepDim = *begin;
    CSL_ASSERT_SPEC(not keepDim.empty(),
              CSLError::NotImplementedError,
              "getting sub vectorial with empty request.");
    if (keepDim.size() == 1) {
        if (keepDim[0] != -1) 
            return argument[keepDim[0]]->getSubVectorial(
                    std::next(begin), end);
        csl::vector_expr newArg(argument.size());
        auto selfIter = argument.begin();
        for (auto iter = newArg.begin(); iter != newArg.end(); ++iter,
                                                               ++selfIter)
            *iter = (**selfIter).getSubVectorial(std::next(begin), end);
        return highdtensor_from_args_s(newArg);
    }
    csl::vector_expr newArg;
    newArg.reserve(keepDim.size());
    for (std::size_t index : keepDim)
        newArg.push_back(argument[index]->getSubVectorial(
                    std::next(begin), 
                    end));
    return highdtensor_from_args_s(newArg);
}



Expr AbstractVectorial::getVectorialModulus() const
{
    Expr sum = CSL_0;
    if (dim == 1) {
        csl::vector_expr intermediateResults(nArgs);
        for (int i=0; i<nArgs; i++)
            intermediateResults[i] = pow_s(argument[i], CSL_2);

        return sqrt_s(sum_s(intermediateResults));
    }
    csl::vector_expr intermediateResults(nArgs);
    for (int i=0; i<nArgs; i++)
        intermediateResults[i] = argument[i]->getVectorialModulus();

    return sqrt_s(sum_s(intermediateResults));
}

Expr AbstractVectorial::dot(const Expr& expr) const
{
    int t_dim = expr->getDim();
    if (t_dim == 0) {
        Expr foo = Copy(this);
        if (dim == 1) {
            int i=0;
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
                *arg = prod_s(argument[i], expr, true);
            return foo;
        }
        int i=0;
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
            *arg = argument[i]->dot(expr);
        return foo;
    }
    else {
        vector<int> shape2(expr->getShape());
        vector<int> newShape(dim+shape2.size()-2);
        if (shape[dim-1] != shape2[0]) {
            CALL_SMERROR_SPEC(CSLError::MathError,
                         "shapes do not match in dot product.");
            return CSL_0;
        }
        for (int i=0; i<dim-1; i++)
            newShape[i] = shape[i];
        for (std::size_t j=dim-1; j<newShape.size(); j++)
            newShape[j] = shape2[2+j-dim];

        if (dim == 1) {
            Expr foo = CSL_0;
            std::vector<csl::Expr> args(nArgs);
            for (int i=0; i<nArgs; i++)
                args[i] = prod_s(argument[i], expr[i], true);
            return sum_s(args);
        }
        else {
            Expr foo = vectorialtensor_s(newShape);
            for (int i=0; i<nArgs; i++)
                foo->setArgument(argument[i]->dot(expr), i);
            return foo;
        }
    }
}

Expr AbstractVectorial::addition_own(const Expr& expr) const
{
    if (!exactMatchShape(expr.get())) {
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "shapes do not match in addition.");
        return CSL_0;
    }
    Expr foo = vectorialtensor_s(getShape());
    if (expr->getDim() == 0) {
        if (dim == 1) {
            int i=0;
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
                *arg = sum_s(expr, argument[i]);
            return foo;
        }
        int i=0;
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
            *arg = argument[i]->addition_own(expr);
        return foo;
    }
    if (dim == 1) {
        int i=0;
        auto arg2=expr->begin();
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
            *arg = sum_s(argument[i], *arg2);
    }
    else {
        int i=0;
        auto arg2=expr->begin();
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
            *arg = argument[i]->addition_own(*arg2);
    }

    return foo;
}

Expr AbstractVectorial::multiplication_own(const Expr& expr, bool side) const
{
    if (!exactMatchShape(expr.get())) {
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "shapes do not match in addition.");
        return CSL_0;
    }
    Expr foo = vectorialtensor_s(getShape());
    if (side) {
        if (expr->getDim() == 0) {
            if (dim == 1) {
                int i=0;
                for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i) 
                    *arg = prod_s(argument[i], expr);

                return foo;
            }
            int i=0;
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
                *arg = argument[i]->multiplication_own(expr);

            return foo;
        }
        if (dim == 1) {
            int i=0;
            auto arg2 = expr->begin();
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
                *arg = prod_s(argument[i],*arg2);
        }
        else {
            int i=0;
            auto arg2 = expr->begin();
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
                *arg = argument[i]->multiplication_own(*arg2);
        }
    }
    else {
        if (expr->getDim() == 0) {
            if (dim == 1) {
                int i=0;
                for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i) 
                    *arg = prod_s(expr, argument[i]);

                return foo;
            }
            int i=0;
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
                *arg = argument[i]->multiplication_own(expr, 0);

            return foo;
        }
        if (dim == 1) {
            int i=0;
            auto arg2 = expr->begin();
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
                *arg = prod_s(*arg2, argument[i]);
        }
        else {
            int i=0;
            auto arg2 = expr->begin();
            for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++arg2, ++i)
                *arg = argument[i]->multiplication_own(*arg2, 0);
        }
    }

    return foo;
}

Expr AbstractVectorial::tensordot(const Expr& expr) const
{
    if (expr->getDim() == 0)
        return multiplication_own(expr);
    vector<int> shape = getShape();
    vector<int> shape2 = expr->getShape();
    shape.insert(shape.end(), shape2.begin(), shape2.end());
    Expr foo = vectorialtensor_s(shape);
    if (dim > 1)  {
        int i=0;
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
            *arg = argument[i]->tensordot(expr);
    }
    else  {
        int i=0;
        for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
            *arg = expr->multiplication_own(argument[i],0);
    }
    
    return foo;
}

Expr AbstractVectorial::trace(int axis1, int axis2) const
{
    if (axis1 > dim or axis2 > dim) {
        CALL_SMERROR_SPEC(CSLError::IndexError,
                     "taking trace of out of bounds axis.");
        return CSL_0;
    }
    vector<int> newShape = shape;
    for (int i=0; i<dim; i++)
        if (i == axis1 or i == axis2)
            newShape[i] = 1;
    for (std::size_t i=0; i<newShape.size(); i++) {
        if (newShape[i] == 1) {
            newShape.erase(newShape.begin()+i);
            i--;
        }
    }

    Expr foo = vectorialtensor_s(newShape);
    vector<int> indices(dim, -1);
    if (axis1 == axis2) {
        for (int i=0; i<shape[axis1]; i++) {
            indices[axis1] = i;
            foo = sum_s(foo, getArgument(indices));
        }

        return foo;
    }
    if (shape[axis1] != shape[axis2]) {
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "taking trace of non-matching dimensions.");
        return CSL_0;
    }
    if (axis2 < axis1) {
        int foo = axis1;
        axis1 = axis2;
        axis2 = foo;
    }
    for (int i=0; i<shape[axis1]; i++) {
        indices[axis1] = i;
        indices[axis2] = i;
        foo = sum_s(foo, getArgument(indices));
    }

    return foo;
}

vector<list<int>> getNonZeroElements_impl(Expr_info vec)
{
    if (vec->getDim() == 1) {
        vector<list<int>> indices;
        indices.reserve(vec->size());
        for (std::size_t i = 0; i != vec->size(); ++i)
            if ((*vec)[i] != CSL_0)
                indices.push_back(list<int>(1, i));
        return indices;
    }
    vector<list<int>> indices;
    indices.reserve(vec->size());
    for (std::size_t i = 0; i != vec->size(); ++i) {
        vector<list<int>> recursiveIndices 
            = getNonZeroElements_impl((*vec)[i].get());
        for (auto& listIndices : recursiveIndices)
            listIndices.push_front(i);
        indices.insert(indices.end(),
                       recursiveIndices.begin(),
                       recursiveIndices.end());
    }
    return indices;
}

vector<vector<int>> getNonZeroElements(Expr_info vec)
{
    vector<list<int>> listIndices = getNonZeroElements_impl(vec);
    vector<vector<int>> vecIndices(listIndices.size());
    for (std::size_t i = 0; i != listIndices.size(); ++i)
        vecIndices[i].insert(vecIndices[i].begin(),
                             listIndices[i].begin(),
                             listIndices[i].end());

    return vecIndices;
}

Expr AbstractVectorial::symmetrise() const
{
    if (dim == 1)
        return copy();
    if (dim == 2) {
        return CSL_HALF * (copy() + transpose());
    }
    Expr sym = highdtensor_s(getShape(), CSL_0);
    vector<vector<int>> indices = getNonZeroElements(this);
    for (auto& index : indices) {
        Expr element = getArgument(index);
        std::sort(index.begin(), index.end());
        do {
            sym->setArgument(element, index);
        } while (std::next_permutation(index.begin(), index.end()));
    }

    return sym;
}

int sort_keep_sign(typename std::vector<int>::iterator begin,
                   typename std::vector<int>::iterator end)
{
    if (begin == end)
        return 1;

    int sign = 1;
    while (begin != end - 1) {
        typename std::vector<int>::iterator mini = begin;
        for (auto iter = begin; iter != end; ++iter)
            if (*iter < *mini)
                mini = iter;
        if (mini != begin) {
            swap(*mini, *begin);
            sign *= -1;
        }
        ++begin;
    }

    return sign;
}

Expr AbstractVectorial::antisymmetrise() const
{
    if (dim == 1)
        return copy();
    if (dim == 2) {
        return CSL_HALF * (copy() + transpose());
    }
    Expr sym = highdtensor_s(getShape(), CSL_0);
    vector<vector<int>> indices = getNonZeroElements(this);
    for (auto& index : indices) {
        Expr element = getArgument(index);
        int sign = sort_keep_sign(index.begin(), index.end());
        std::size_t counter = 1;
        do {
            // For explication on the counter and std::next_permutation:
            // https://stackoverflow.com/questions/43927942/signature-next-permutation-c
            // (Here antisymmetrizing so every index should be different: the 
            // rule applies)
            sym->setArgument(sign * element, index);
            if (++counter %2 == 0)
                sign *= -1;
        } while (std::next_permutation(index.begin(), index.end()));
    }

    return sym;
}

optional<Expr> AbstractVectorial::expand(bool full,
                                         bool inplace) const
{
    Expr foo = vectorialtensor_s(getShape());
    int i=0;
    for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
        *arg = Expanded(argument[i], full, inplace);

    return foo;
}

optional<Expr> AbstractVectorial::expand_if(
        std::function<bool(Expr const&)> const& f,
        bool full,
        bool inplace) const
{
    Expr foo = vectorialtensor_s(getShape());
    int i=0;
    for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
        *arg = ExpandedIf(argument[i], f, full, inplace);

    return foo;
}

optional<Expr> AbstractVectorial::factor(bool full) const
{
    Expr foo = vectorialtensor_s(getShape());
    int i=0;
    for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
        *arg = Factored(argument[i], full);

    return foo;
}

optional<Expr> AbstractVectorial::factor(Expr_info,
                                         bool      full) const
{
    Expr foo = vectorialtensor_s(getShape());
    int i=0;
    for (auto arg=foo->begin(); arg!=foo->end(); ++arg, ++i)
        *arg = Factored(argument[i], full);

    return foo;
}

bool AbstractVectorial::operator==(Expr_info expr) const
{
    if (int test = testDummy(expr); test != -1)
        return test;
    if (dim != expr->getDim() or nArgs != expr->getNArgs()) 
        return false;
    int i=0;
    for (auto arg=expr->begin(); arg!=expr->end(); ++arg, ++i)
        if (*argument[i] != *arg)
            return false;

    return true;
}

Expr const &AbstractVectorial::operator[](int iArg) const
{
    if (iArg < 0 or iArg >= nArgs) {
        print();
        CALL_SMERROR(CSLError::IndexError);
    }
    return argument[iArg];
}

Expr& AbstractVectorial::operator[](int iArg)
{
    if (iArg < 0 or iArg >= nArgs) {
        print();
        CALL_SMERROR(CSLError::IndexError);
    }
    return argument[iArg];
}

///////////////////////////////////////////////////
/*************************************************/
// Class Vector                                  //
/*************************************************/
///////////////////////////////////////////////////

Vector::Vector(): AbstractVectorial()
{
    dim = 1;
    shape = vector<int>(1,0);
}

Vector::Vector(int t_nElements): Vector()
{
    nArgs = t_nElements;
    argument = csl::vector_expr(nArgs,CSL_0);
    shape = vector<int>(1,nArgs);
}

Vector::Vector(int t_nElements, const Expr& expr, const Expr& index): Vector()
{
    if (expr->getDim() > 0) {
        print();
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "setting an element with a sequence.");
    }
    else {
        nArgs = t_nElements;
        argument = csl::vector_expr(nArgs, expr);
        if (index->getType() == csl::Type::Variable)
            for (auto it=argument.begin(); it!=argument.end(); ++it)
                Replace(*it,index,int_s(distance(argument.begin(),it)));
        else if (*index == CSL_0)
            for (auto& el : *this)
                el = Copy(el);
        shape = vector<int>(1,nArgs);
    }
}

Vector::Vector(const csl::vector_expr& t_argument): Vector()
{
    int dimOk = true;
    for (auto it=t_argument.begin(); it!=t_argument.end(); ++it) {
        if ((*it)->getDim() != 0) {
            dimOk = false;
            break;
        }
    }
    if (!dimOk) {
        print();
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "setting an element with a sequence.");
    }
    else {
        argument = t_argument;
        nArgs = argument.size();
        shape = vector<int>(1,nArgs);
    }
}

Expr Vector::getSubVectorial(int iExcept) const
{
    if (nArgs == 2) 
        return argument[1-iExcept];
    Expr foo = vector_s(nArgs-1);
    for (int i=0; i<nArgs; i++) {
        if (i < iExcept)
            foo->setArgument(argument[i],i);
        else if (i > iExcept)
            foo->setArgument(argument[i],i-1);
    }

    return foo;
}

void Vector::setElementary(bool t_elementary)
{
    for (auto& var : *this)
        if (var->getType() != csl::Type::Variable)
            CALL_SMERROR_SPEC(CSLError::TypeError,
                         "setting as elementary a non Variable type ("
                         + toString(var->getType()) + ").");
        else
            var->setElementary(t_elementary);
}

Expr vector_s(int t_nElements)
{
    return csl::make_shared<Vector>(t_nElements);
}

Expr vector_s(int t_nElements,const Expr& expr, const Expr& index)
{
    return csl::make_shared<Vector>(t_nElements, expr, index);
}

Expr vector_s(int t_nElements,const Expr& expr)
{
    return csl::make_shared<Vector>(t_nElements, expr);
}

Expr vector_s(const csl::vector_expr& t_argument)
{
    return csl::make_shared<Vector>(t_argument);
}

Expr vector_s(const std::initializer_list<Expr>& t_argument)
{
    return csl::make_shared<Vector>(csl::vector_expr(t_argument.begin(),
                t_argument.end()));
}

///////////////////////////////////////////////////
/*************************************************/
// Class Matrix                                  //
/*************************************************/
///////////////////////////////////////////////////

Matrix::Matrix(): AbstractVectorial()
{
    dim = 2;
    shape = vector<int>(2,0);
}

Matrix::Matrix(int t_nArgs): Matrix()
{
    nArgs = t_nArgs;
    argument = csl::vector_expr(nArgs);
    for (auto& arg : argument)
        arg = vector_s(nArgs);
    shape = vector<int>(2,nArgs);
}

Matrix::Matrix(int t_x_nArgs, int t_y_nArgs): Matrix()
{
    nArgs = t_x_nArgs;
    argument = csl::vector_expr(nArgs);
    for (auto& arg : argument)
        arg = csl::make_shared<Vector>(t_y_nArgs);
    shape = vector<int>(2);
    shape[0] = t_x_nArgs;
    shape[1] = t_y_nArgs;
}

Matrix::Matrix(int t_x_nArgs, int t_y_nArgs,
               const Expr& expr,
               const Expr& index_x, const Expr& index_y)
    :Matrix()
{
    if (expr->getDim() != 0) {
        print();
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "setting an element with a sequence.");
    }
    else {
        nArgs = t_x_nArgs;
        argument = csl::vector_expr(nArgs);
        Expr foo;
        for (auto it=argument.begin(); it!=argument.end(); ++it) {
            if (index_x != CSL_0)
                foo = Replaced(expr, index_x,
                        int_s(distance(argument.begin(),it)));
            else
                foo = expr;
            *it = csl::make_shared<Vector>(t_y_nArgs,foo,index_y);
        }
        shape = vector<int>(2);
        shape[0] = t_x_nArgs;
        shape[1] = t_y_nArgs;
    }
}

Matrix::Matrix(const csl::vector_expr& t_argument): Matrix()
{
    int dimOk = true;
    for (const auto& x: t_argument) {
        if (x->getDim() != 1) {
            dimOk = false;
            break;
        }
    }
    if (!dimOk) {
        print();
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "setting an element with a sequence.");
    }
    else {
        argument = t_argument;
        nArgs = argument.size();
        shape = vector<int>(2);
        shape[0] = nArgs;
        if (nArgs > 0) shape[1] = argument[0]->getNArgs();
        else           shape[1] = 0;
    }
}

Matrix::Matrix(const vector<csl::vector_expr>& t_argument): Matrix()
{
    int dimOk = true;
    for (const auto& row: t_argument) {
        for (const auto& x : row)
            if (x->getDim() != 0) {
                dimOk = false;
                break;
            }
    }
    if (!dimOk) {
        print();
        CALL_SMERROR_SPEC(CSLError::MathError,
                     "setting an element with a sequence.");
    }
    else {
        for (const auto& row : t_argument)
            argument.push_back(vector_s(row));
        nArgs = argument.size();
        shape = vector<int>(2);
        shape[0] = nArgs;
        if (nArgs > 0) shape[1] = argument[0]->getNArgs();
        else           shape[1] = 0;
    }
}


Expr Matrix::getSubVectorial(int iExcept, int jExcept) const
{
    vector<int> newShape = shape;
    newShape[0]--;
    newShape[1]--;
    if (newShape[0] == 1) 
        return argument[1-iExcept]->getSubVectorial(jExcept);
    Expr foo = vectorialtensor_s(newShape);
    for (int i=0; i<nArgs; i++) {
        if (i < iExcept)
            foo->setArgument(argument[i]->getSubVectorial(jExcept),i);
        else if (i > iExcept)
            foo->setArgument(argument[i]->getSubVectorial(jExcept),i-1);
    }
    return foo;
}

Expr Matrix::determinant() const
{
    if (nArgs != argument[0]->getNArgs()) {
        cout<<"Warning: taking determinant of non-square matrix.\n";
        return CSL_0;
    }

    // Former algo in N!
    
    /*
    Expr foo = CSL_0;
    if (nArgs > 2 and argument[0]->getNArgs() > 2) 
        for (int i=0; i<nArgs; i++)
            foo = sum_s(foo,
                        prod_s(argument[i]->getArgument(0),
                               prod_s(int_s(pow(-1,i)),
                                      getSubVectorial(i,0)->determinant())));    
    else 
        for (int i=0; i<nArgs; i++)
            foo = sum_s(foo,
                        prod_s(argument[i]->getArgument(0),
                               prod_s(int_s(pow(-1,i)),
                                      getSubVectorial(i,0))));    
    */
   

    // New algo in N^3
    Expr copy = DeepCopy(this);
    const int n = nArgs;
    const int m = argument[0]->getNArgs();
    int h = 0;
    int k = 0;
    int pivot = n-1;
    int sign = 1;
    auto arg_h = copy->begin();
    while (h < n and k < m) {
        if ((*arg_h)->getArgument(k) == CSL_0) {
            if (pivot == h)
                return CSL_0;
            Expr foo = copy->getArgument(pivot);
            copy->setArgument(*arg_h, pivot);
            *arg_h = foo;
            --pivot;
            sign *= -1;
        }
        else {
            pivot = n-1;
            for (auto arg_i=1+arg_h; arg_i!=copy->end(); ++arg_i) {
                Expr f = fraction_s((*arg_i)->getArgument(k),
                                   (*arg_h)->getArgument(k));
                (*arg_i)->setArgument(CSL_0,k);
                for (int j=k+1; j<m; ++j)
                    (*arg_i)->setArgument(minus_((*arg_i)->getArgument(j),
                                prod_s((*arg_h)->getArgument(j),f)),j);
            }
            ++h;
            ++arg_h;
            ++k;
        }
    };
    csl::vector_expr vec(nArgs);
    int i=0;
    for (auto arg=copy->begin(); arg!=copy->end(); ++arg, ++i) {
        vec[i] = (*arg)->getArgument(i);
    }


    return prod_s(int_s(sign),prod_s(vec));
}

Expr Matrix::trace() const
{
    if (nArgs != argument[0]->getNArgs()) {
        cout<<"Warning: taking the trace of non-square matrix.\n";
        return CSL_0;
    }
    csl::vector_expr vec(nArgs);
    for (int i=0; i<nArgs; i++)
        vec[i] = argument[i]->getArgument(i);

    return sum_s(vec);
}

Expr Matrix::transpose() const
{
    vector<int> newShape(shape);
    int fooInt = newShape[0];
    newShape[0] = newShape[1];
    newShape[1] = fooInt;
    Expr foo = vectorialtensor_s(newShape);
    int j=0;
    for (auto arg_i=foo->begin(); arg_i!=foo->end(); ++arg_i, ++j) {
        int i=0;
        for (auto arg_j=(**arg_i).begin(); arg_j!=(**arg_i).end(); ++arg_j, ++i)
            *arg_j = argument[i]->getArgument(j);
    }

    return foo;
}

Expr Matrix::transposedCoMatrix() const
{
    Expr foo = vectorialtensor_s(getShape());
    if (nArgs > 2 and argument[0]->getNArgs() > 2) 
        for (int i=0; i<nArgs; i++)
            for (int j=0; j<argument[0]->getNArgs(); j++) {
                foo->setArgument(prod_s(getSubVectorial(i,j)->determinant(),
                                        int_s(pow(-1,i+j))),{j,i});
            }
    else if (nArgs > 1)
        for (int i=0; i<nArgs; i++) 
            for (int j=0; j<argument[0]->getNArgs(); j++) 
                foo->setArgument(prod_s(getSubVectorial(i,j),
                                        int_s(pow(-1,i+j))),{j,i});
    else 
        foo->setArgument(CSL_1,{0,0});

    return foo;
}

Expr Matrix::hermitian() const
{
    Expr transposed = transpose();
    for (auto row = transposed->begin(); row != transposed->end(); ++row)
        for (auto column = (**row).begin(); column != (**row).end(); ++column)
            *column = GetComplexConjugate(*column);

    return transposed;
}

Expr Matrix::inverseMatrix() const
{
    Expr det = determinant();
    if (det->getPrimaryType() == csl::PrimaryType::Numerical
            and det->evaluateScalar() == 0) {
        cout<<"Warning: inverting matrix that has det = 0.\n";
        return CSL_0;
    }

    return prod_s(fraction_s(CSL_1,det),transposedCoMatrix());
}

Expr Matrix::symmetrise() const
{
    Expr foo = intfraction_s(1,2);
    return sum_s(prod_s(foo,Copy(this)),prod_s(foo,transpose()));
}

Expr Matrix::antisymmetrise() const
{
    Expr foo = intfraction_s(1,2);
    return minus_(prod_s(foo,Copy(this)),prod_s(foo,transpose()));
}

Expr matrix_s(int t_nArgs)
{
    return csl::make_shared<Matrix>(t_nArgs);
}

Expr matrix_s(int t_x_nArgs, int t_y_nArgs)
{
    return csl::make_shared<Matrix>(t_x_nArgs, t_y_nArgs);
}

Expr matrix_s(int t_x_nArgs, int t_y_nArgs,
             const Expr& expr,
             const Expr& index_x, const Expr& index_y)
{
    return csl::make_shared<Matrix>(t_x_nArgs, t_y_nArgs, expr, index_x, index_y);
}

Expr matrix_s(int t_x_nArgs, int t_y_nArgs,
             const Expr& expr)
{
    return csl::make_shared<Matrix>(t_x_nArgs, t_y_nArgs, expr);
}

Expr matrix_s(const csl::vector_expr& t_argument)
{
    return csl::make_shared<Matrix>(t_argument);
}

Expr matrix_s(const initializer_list<initializer_list<Expr> >& t_matrix)
{
    csl::vector_expr intermediate(0);
    for (const auto& vec : t_matrix)
        intermediate.push_back(vector_s(vec));

    return matrix_s(intermediate);
}

Expr diagonal_s(const csl::vector_expr& diag)
{
    Expr mat = matrix_s(diag.size());
    for (int i = 0; i != (int)diag.size(); ++i)
        mat->setArgument(diag[i], {i, i});

    return mat;
}

Expr identity_s(int dim)
{
    Expr res = matrix_s(dim);
    for (int i=0; i!=dim; ++i) 
        res->setArgument(CSL_1,{i,i});

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class HighDTensor                             //
/*************************************************/
///////////////////////////////////////////////////

HighDTensor::HighDTensor(): AbstractVectorial()
{
    shape = vector<int>(0);
}

HighDTensor::HighDTensor(const vector<int>& t_shape)
    :AbstractVectorial()
{
    shape = t_shape;
    dim = shape.size();
    if(dim > 0) {
        nArgs = shape[0];
        vector<int> newShape(shape.begin()+1, shape.end());
        if (dim > 1) {
            argument = csl::vector_expr(nArgs);
            for (auto& arg : argument)
                arg = vectorialtensor_s(newShape);
        }
        else {
            argument = csl::vector_expr(nArgs);
            for (auto& arg : argument)
                arg = Copy(arg);
        }
    }
    else argument = csl::vector_expr(0);
}

HighDTensor::HighDTensor(const vector<int>& t_shape, const Expr& filler)
    :AbstractVectorial()
{
    shape = t_shape;
    dim = shape.size();
    if(dim > 0) {
        nArgs = shape[0];
        vector<int> newShape(shape.begin()+1, shape.end());
        if (dim > 1) {
            argument = csl::vector_expr(nArgs);
            for (auto& arg : argument)
                arg = vectorialtensor_s(newShape, filler);
        }
        else {
            argument = csl::vector_expr(nArgs, filler);
            for (auto& arg : argument)
                arg = Copy(arg);
        }
    }
    else argument = csl::vector_expr(0);
}

HighDTensor::HighDTensor(const csl::vector_expr& t_argument)
{
    nArgs = t_argument.size();
    shape = vector<int>(1,nArgs);
    if (nArgs > 0) {
        argument = t_argument;
        dim = 1+argument[0]->getDim();
        if (dim > 1) {
            vector<int> followingShape = argument[0]->getShape();
            shape.insert(shape.end(),
                         followingShape.begin(), followingShape.end());
        }
    }
    else {
        argument = csl::vector_expr(0);
        nArgs = 0;
        dim = 0;
    }
}

Expr highdtensor_s(const vector<int>& shape)
{
    if (shape.empty())
        return CSL_0;
    if (shape.size() == 1)
        return vector_s(shape[0]);
    if (shape.size() == 2)
        return matrix_s(shape[0], shape[1]);
    return csl::make_shared<HighDTensor>(shape);
}

Expr highdtensor_s(const vector<int>& shape, const Expr& filler)
{
    if (shape.empty())
        return filler;
    if (shape.size() == 1)
        return vector_s(shape[0]);
    if (shape.size() == 2)
        return matrix_s(shape[0], shape[1]);
    return csl::make_shared<HighDTensor>(shape, filler);
}

Expr highdtensor_from_args_s(const csl::vector_expr& t_argument)
{
    if (t_argument.size() == 0)
        return CSL_0;
    switch (t_argument[0]->getDim()) {

        case 0:
        return vector_s(t_argument);
        break;

        case 1:
        return matrix_s(t_argument);
        break;

        default:
        return csl::make_shared<HighDTensor>(t_argument);
    }
}

Expr vectorialtensor_s(const std::vector<int>& shape)
{
    switch(shape.size())
    {
        case 0:
        return CSL_0;
        break;

        case 1:
        return vector_s(shape[0]);
        break;

        case 2:
        return matrix_s(shape[0],shape[1]);
        break;

        default:
        return highdtensor_s(shape);
    }
}

Expr vectorialtensor_s(const std::vector<int>& shape, const Expr& filler)
{
    switch(shape.size())
    {
        case 0:
        return filler;
        break;

        case 1:
        return vector_s(shape[0], filler);
        break;

        case 2:
        return matrix_s(shape[0],shape[1], filler);
        break;

        default:
        return highdtensor_s(shape, filler);
    }
}

Expr highdtensor_s(const initializer_list<initializer_list<
        initializer_list<Expr> > >& t_tensor)
{
    csl::vector_expr intermediate(0);
    for (const auto& vec : t_tensor)
        intermediate.push_back(matrix_s(vec));

    return highdtensor_from_args_s(intermediate);
}


} // End of namespace csl
