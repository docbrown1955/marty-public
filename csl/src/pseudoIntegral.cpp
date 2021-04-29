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

#include "pseudoIntegral.h"
#include "scopedProperty.h"
#include "comparison.h"
#include "algo.h"
#include "error.h"
#include "interface.h"
#include "replace.h"
#include "indicial.h"
#include "space.h"
#include "options.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class PseudoIntegral                          //
/*************************************************/
///////////////////////////////////////////////////

PseudoIntegral::PseudoIntegral(): argument(CSL_1)
{}

PseudoIntegral::PseudoIntegral(const csl::vector_expr& terms): PseudoIntegral()
{
    for (const auto& term : terms)
        addTerm(term);
}

PseudoIntegral::PseudoIntegral(const csl::vector_expr& t_integrals,
        const Expr& t_argument)
    :vectorIntegrals(t_integrals), argument(CSL_1)
{
    addTerm(t_argument);
}

void PseudoIntegral::testIntegralDep(const Expr& term) const
{
    CSL_ASSERT_SPEC(csl::AllOfNodes(term,
                [](const Expr& el) {
                  return el->getType() != csl::Type::VectorIntegral;
                }),
               CSLError::TypeError,
               "Integral dependency encountered in sum."
               + toString(term) + ".")
}

void PseudoIntegral::addSingleTerm(const Expr& term)
{
    testIntegralDep(term);
    argumentVector.push_back(term);
    argument = prod_s(argument, term, true);
    argumentUpdated = false;
}

Expr const &PseudoIntegral::getArgument() const
{
    if (argumentUpdated)
        return argument;
    argument = prod_s(argumentVector);
    argumentUpdated = true;
    return argument;
}

void PseudoIntegral::addSingleIntegral(const Expr& integral)
{
    Expr operand = integral->getOperand();

    const int pos = vectorIntegrals.size();
    vectorIntegrals.push_back(Copy(integral));
    vectorIntegrals[pos]->setOperand(CSL_1);
    vectorIntegrals[pos]->setEmpty(true);
    if (operand != CSL_1)
        addTerm(operand);
}

void PseudoIntegral::addTerm(const Expr& term)
{
    if (term == CSL_0) {
        vectorIntegrals.clear();
        argument = CSL_0;
        return;
    }
    if (term->getType() == csl::Type::VectorIntegral) {
        addSingleIntegral(term);
    }
    else if (term->getType() == csl::Type::Prod) {
        for (const auto& arg : *term) {
            if (arg->getType() == csl::Type::VectorIntegral)
                addSingleIntegral(arg);
            else {
                addSingleTerm(arg);
            }
        }
    }
    else
        addSingleTerm(term);
}

bool PseudoIntegral::testBoundsInfinity(const Expr& inf, const Expr& sup) const
{
    if (inf->getDim() == 0 and inf != -CSL_INF)
        return false;
    else if (inf->getDim() > 0) 
        for (int i = 0; i != inf->getNArgs(); ++i)
            if (not testBoundsInfinity(inf->getArgument(i), sup))
                return false;

    if (sup->getDim() == 0 and sup !=CSL_INF)
        return false;
    else if (sup->getDim() > 0) 
        for (int i = 0; i != sup->getNArgs(); ++i)
            if (not testBoundsInfinity(inf, sup->getArgument(i)))
                return false;

    return true;
}

void suppressTerm(Expr& init, Expr const& term)
{
    if (term->getType() == csl::Type::Prod) {
        for (const auto& t : *term)
            suppressTerm(init, t);
        return;
    }
    csl::ForEachNode(init, [&](Expr& el)
    {
        if (*el == term.get())
            el = CSL_1;
    });
}

bool PseudoIntegral::evaluateDelta(
        int index,
        Expr& res, 
        std::vector<Expr*>& dependencies) const
{
    Expr integ = vectorIntegrals[index];
    Expr inf(integ->getInfBoundary());
    Expr sup(integ->getSupBoundary());
    Expr variable(integ->getVariable());
    AbstractParent& indicialVariable = *integ->getParent();
    if (indicialVariable.getSpace().size() != 1)
        callError(cslError::UndefinedBehaviour,
                "TensorParent\"" + string(indicialVariable.getName())
                + "\" variable of a VectorIntegral may be "
                + "1-dimensionnal only.");

    const Space* spaceVariable = indicialVariable.getSpace()[0];
    Index fooIndex = spaceVariable->generateIndex("foo");
    fooIndex.setFree(false);

    if (not testBoundsInfinity(inf, sup))
        return false;
    if (dependencies.size() == 1) {
        if ((**dependencies[0]).getType() != csl::Type::DiracDelta)
            return false;
        if ((**dependencies[0]).dependsExplicitlyOn(integ->getParent().get())) {
            Expr copyArg = (**dependencies[0]).copy();
            (*dependencies[0]) = CSL_1;
            res = copyArg->applyDiracDelta(
                    argument,
                    indicialVariable(fooIndex));
            return true;
        }
        return false;
    }
    for (size_t i = 0; i != dependencies.size(); ++i) {
        Expr& arg = (*dependencies[i]);
        if (arg->getType() != csl::Type::DiracDelta)
            continue;
        if (arg->getArgument()->dependsExplicitlyOn(
                    integ->getParent().get())) {
            Expr copyArg = arg->copy();
            arg = CSL_1;
            res = copyArg->applyDiracDelta(argument, indicialVariable(fooIndex));
            return true;
        }
    }
    return false;
}

bool PseudoIntegral::evaluateExpProduct(
        int index,
        Expr& res,
        std::vector<Expr*>& dependencies) const
{
    Expr integ = vectorIntegrals[index];
    Expr inf(integ->getInfBoundary());
    Expr sup(integ->getSupBoundary());
    Expr variable(integ->getVariable());
    AbstractParent& indicialVariable = *integ->getParent();
    if (indicialVariable.getSpace().size() != 1)
        callError(cslError::UndefinedBehaviour,
                "TensorParent\"" + string(indicialVariable.getName())
                + "\" variable of a VectorIntegral may be "
                + "1-dimensionnal only.");

    const Space* spaceVariable = indicialVariable.getSpace()[0];
    Index fooIndex = spaceVariable->generateIndex("foo");
    fooIndex.setFree(false);

    Expr foo = getExponentialDependency(dependencies);
    if (foo == CSL_0)
        return false;
    Prod case1({CSL_I,
                indicialVariable(fooIndex),
                Comparator::dummy(1)}, 1);
    Prod case2({CSL_M_1,
                CSL_I,
                indicialVariable(fooIndex),
                Comparator::dummy(1)}, 1);
    if (Comparator::dummyComparison(foo.get(), &case1)
            or Comparator::dummyComparison(foo.get(), &case2)) {
        //res = 2*PI*csl::make_shared<DiracDelta>(
        //        foo->suppressTerm(variable)->suppressTerm(I));
        res = argument;
        ScopedProperty p(&csl::option::freezeMerge, true);
        for (Expr* dep : dependencies)
            *dep = CSL_1;
        // if (dep->getType() != csl::Type::Prod)
        //     res = res->suppressTerm(dep.get());
        // else {
        //     for (const auto& arg : *dep)
        //         res = res->suppressTerm(arg.get());
        // }
        Expr term1 = (*integ->getParent())(fooIndex);
        res = prod_s(res,
                csl::make_shared<DiracDelta>(
                foo->suppressTerm(term1.get())->suppressTerm(CSL_I.get())),
                true);
                //->expand(true));
        return true;
    }
    return false;
}

void PseudoIntegral::getRecursiveIntegralDependency(
        Expr& operand,
        const Expr& integ,
        std::vector<Expr*>& dependencies)
{
    if (not operand->dependsOn(integ->getParent().get())) {
        return;
    }
    Expr dep = CSL_1;
    switch(operand->getType()) {
        case csl::Type::Prod:
        for (auto& arg : *operand)
            getRecursiveIntegralDependency(arg,
                                           integ,
                                           dependencies);
        break;

        default:
        dependencies.push_back(&operand);
    }
}

Expr PseudoIntegral::getExpression() const
{
    if (vectorIntegrals.empty())
        return argument;
    csl::vector_expr res = vectorIntegrals;
    res.push_back(argument);
    return prod_s(res);
}

Expr PseudoIntegral::getExponentialDependency(
        std::vector<Expr*> const& dep) const
{
    csl::vector_expr foo;
    foo.reserve(dep.size());
    for (Expr* arg : dep) {
        if ((**arg).getType() != csl::Type::Exp
                and ((**arg).getType() != csl::Type::Pow
                    or (**arg).getArgument() != CSL_E))
            return CSL_0;
        foo.push_back((**arg).getArgument());
    }

    return Factored(Expanded(sum_s(foo, true)));
}

void PseudoIntegral::expandExponentials()
{
    if (argument->getType() == csl::Type::Exp)
        Expand(argument, true);
    else if (argument->getType() == csl::Type::Prod){
        bool changed = false;
        for (int i = 0; i != argument->getNArgs(); ++i)
            if (argument->getArgument(i)->getType() == csl::Type::Exp) {
                argument->setArgument(Expanded(argument->getArgument(i), 1),
                                      i);
                changed = true;
            }
        if (changed)
            argument = Refreshed(argument);
    }
}

Expr PseudoIntegral::evaluate()
{
    Expr res;
    bool changed = false;
    // getArgument();
    if (argument->getType() == csl::Type::Sum) {
        std::vector<Expr> res;
        res.reserve(argument->size());
        for (const auto& arg : *argument)
            res.push_back(PseudoIntegral(vectorIntegrals, arg).evaluate());
        vectorIntegrals.clear();
        argument = sum_s(res);
        // cout << "Res = " << argument << endl;
        return argument;
    }
    expandExponentials();
    for (size_t i = 0; i != vectorIntegrals.size(); ++i) {
        Expr integ = vectorIntegrals[i];
        std::vector<Expr*> dependencies;
        dependencies.reserve(5);
        getRecursiveIntegralDependency(
                argument,
                integ,
                dependencies);
        if (dependencies.empty()) {
            continue;
        }
        if (evaluateDelta(i,res,dependencies) 
                or evaluateExpProduct(i,res,dependencies)) {
            argument = res;
            vectorIntegrals.erase(vectorIntegrals.begin()+i);
            --i;
            changed = true;
        }
    }
    if (changed)
        evaluate();

    return getExpression();
}

Expr PseudoIntegral::applyOn(const Expr& expr) const
{
    if (expr->getType() == csl::Type::Sum) {
        csl::vector_expr res(expr->size());
        for (size_t i = 0; i != res.size(); ++i)
            res[i] = applyOn((*expr)[i]);
        return sum_s(res);
    }
    if (expr->getType() == csl::Type::Prod) {
        for (const auto& term : *expr)
            if (term->getType() == csl::Type::Sum 
                    and csl::AnyOfNodes(
                    term,
                    [](const Expr& el) {
                        return el->getType() == csl::Type::VectorIntegral;
                    }))
            {
                Expr expanded = Expanded(
                        prod_s(prod_s(argumentVector, true), expr, true));
                csl::vector_expr res(expanded->size());
                for (size_t i = 0; i != res.size(); ++i)
                    res[i] = Evaluated(PseudoIntegral(
                                vectorIntegrals,
                                (*expanded)[i]).evaluate());
                return sum_s(res);
            }
    }

    PseudoIntegral copy(*this);
    copy.addTerm(expr);
    return Evaluated(copy.evaluate());
}

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractIntegral                        //
/*************************************************/
///////////////////////////////////////////////////

AbstractIntegral::AbstractIntegral()
    :Operator<AbstractFunc>(),
    inf(-CSL_INF), 
    sup(CSL_INF)
{
    argument = CSL_1;
}

AbstractIntegral::AbstractIntegral(const Expr& operand)
    :AbstractIntegral()
{
    argument = operand;
}

AbstractIntegral::AbstractIntegral(const Expr& operand,
                                   bool        t_empty)
    :AbstractIntegral(operand)
{
    empty = t_empty;
}

AbstractIntegral::AbstractIntegral(const Expr& operand,
                                   const Expr& t_inf,
                                   const Expr& t_sup)
    :AbstractIntegral(operand)
{
    inf = t_inf;
    sup = t_sup;
}

AbstractIntegral::AbstractIntegral(const Expr& operand,
                                   bool        t_empty,
                                   const Expr& t_inf,
                                   const Expr& t_sup)
    :AbstractIntegral(operand, t_empty)
{
    inf = t_inf;
    sup = t_sup;
}

Expr AbstractIntegral::getOperand() const
{
    return argument;
}

Expr AbstractIntegral::getSupBoundary() const
{
    return sup;
}

Expr AbstractIntegral::getInfBoundary() const
{
    return inf;
}

void AbstractIntegral::setSupBoundary(Expr const &t_sup)
{
    sup = t_sup;
}

void AbstractIntegral::setInfBoundary(Expr const &t_inf)
{
    inf = t_inf;
}

void AbstractIntegral::setOperand(const Expr& t_operand)
{
    argument = t_operand;
}

Expr AbstractIntegral::suppressTerm(Expr_info expr) const
{
    Expr res = Copy(this);
    res->setOperand(getOperand()->suppressTerm(expr));
    
    return res;
}

optional<Expr> AbstractIntegral::evaluate(
        csl::eval::mode user_mode
        ) const
{
    return AbstractFunc::evaluate(user_mode);
}

optional<Expr> AbstractIntegral::derive(Expr_info) const
{
    // if (derivative_s(expr,getVariable()) != CSL_0) {
    //     callError(cslError::UndefinedBehaviour,
    //         "AbstractIntegral::derive(Expr_info expr)",
    //         "\"deriving an integral whose variable depends on the argument\"");
    // }
    return AbstractFunc::evaluate();
}

bool AbstractIntegral::dependsOn(Expr_info expr) const 
{
    return AbstractFunc::dependsOn(expr)
        or getVariable()->dependsOn(expr)
        or inf->dependsOn(expr)
        or sup->dependsOn(expr);
}

bool AbstractIntegral::dependsOn(const AbstractParent* parent) const 
{
    return AbstractFunc::dependsOn(parent)
        or getVariable()->dependsOn(parent)
        or inf->dependsOn(parent)
        or sup->dependsOn(parent);
}


bool AbstractIntegral::dependsExplicitlyOn(Expr_info expr) const 
{
    return AbstractFunc::dependsExplicitlyOn(expr)
        or getVariable()->dependsExplicitlyOn(expr)
        or inf->dependsExplicitlyOn(expr)
        or sup->dependsExplicitlyOn(expr);
}

bool AbstractIntegral::operatorAppliesOn(Expr_info expr) const
{
    return expr->dependsOn(getVariable().get());
}

///////////////////////////////////////////////////
/*************************************************/
// Class ScalarIntegral                          //
/*************************************************/
///////////////////////////////////////////////////

ScalarIntegral::ScalarIntegral()
    :AbstractIntegral()
{

}

ScalarIntegral::ScalarIntegral(const Expr& t_variable)
    :AbstractIntegral()
{
    setVariable(t_variable);
}

ScalarIntegral::ScalarIntegral(const Expr& operand,
                               const Expr& t_variable)
    :AbstractIntegral(operand)
{
    setVariable(t_variable);
}

ScalarIntegral::ScalarIntegral(const Expr& t_variable,
                               const Expr& t_inf,     
                               const Expr& t_sup)  
    :AbstractIntegral(CSL_1, t_inf, t_sup)
{
    setVariable(t_variable);
}

ScalarIntegral::ScalarIntegral(const Expr& operand,
                               const Expr& t_variable,
                               const Expr& t_inf,
                               const Expr& t_sup)
    :AbstractIntegral(operand, t_inf, t_sup)
{
    setVariable(t_variable);
}

ScalarIntegral::ScalarIntegral(const Expr& operand,
                               const Expr& t_variable,
                               bool        t_empty,
                               const Expr& t_inf,
                               const Expr& t_sup)
    :AbstractIntegral(operand, t_empty, t_inf, t_sup)
{
    setVariable(t_variable);
}

Expr ScalarIntegral::getVariable() const
{
    return variable;
}

void ScalarIntegral::setVariable(Expr const &t_variable)
{
    CSL_ASSERT_SPEC(t_variable->getType() == csl::Type::Variable,
            CSLError::TypeError,
            "Expecting a variable in scalar integral, " + toString(t_variable)
            + " given.")
    variable = t_variable;
}

void ScalarIntegral::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out << "int{";
    out << variable->getName();
    if (inf != -CSL_INF or sup != CSL_INF) {
        out << ", ";
        inf->print(1, out);
        out << ", ";
        sup->print(1, out);
    }
    out << "}(";
    if (argument != CSL_1)
        argument->print(1, out);
    if (not empty)
        out << ")";
    if (mode == 0)
        out << endl;
}

void ScalarIntegral::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::scalarintegral_s(";
    argument->printCode(1, out);
    out << ", ";
    variable->printCode(1, out);
    if (sup != CSL_INF or inf != -CSL_INF) {
        out << ", ";
        inf->printCode(1, out);
        out << ", ";
        sup->printCode(1, out);
    }
    out << ")";
}

string ScalarIntegral::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "int";
    if (inf != -CSL_INF or sup != CSL_INF) {
        sout << "_{";
        sout << inf->printLaTeX(1);
        sout << "}^{";
        sout << sup->printLaTeX(1);
        sout << "}";
    }
    sout << "\\left(";
    sout << "d" << variable->getName() << "\\cdot";
    if (argument != CSL_1)
        sout << argument->printLaTeX(1);
    if (not empty)
        sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

unique_Expr ScalarIntegral::copy_unique() const
{
    return make_unique<ScalarIntegral>(argument,
                                       variable,
                                       empty,
                                       inf,
                                       sup);
}

Expr ScalarIntegral::deepCopy() const
{
    return csl::make_shared<ScalarIntegral>(argument->deepCopy(),
                                       variable->deepCopy(),
                                       empty,
                                       inf->deepCopy(),
                                       sup->deepCopy());
}

Expr ScalarIntegral::refresh() const
{
    return scalarintegral_s(argument,
                           variable,
                           empty,
                           inf,
                           sup);
}

Expr ScalarIntegral::deepRefresh() const
{
    return scalarintegral_s(argument->deepRefresh(),
                           variable->deepRefresh(),
                           empty,
                           inf->deepRefresh(),
                           sup->deepRefresh());
}


bool ScalarIntegral::operator==(Expr_info other) const
{
    if (int test = testDummy(other); test != -1)
        return test;
    if (other->getType() != csl::Type::ScalarIntegral)
        return false;
    return (argument     == other->getArgument()
            and variable == other->getVariable()
            and empty    == other->isEmpty()
            and inf      == other->getInfBoundary()
            and sup      == other->getSupBoundary());
}

///////////////////////////////////////////////////
/*************************************************/
// Class VectorIntegral                          //
/*************************************************/
///////////////////////////////////////////////////

VectorIntegral::VectorIntegral()
    :AbstractIntegral()
{

}

VectorIntegral::VectorIntegral(const Tensor& t_variables)
    :AbstractIntegral(), variables(t_variables)
{

}

VectorIntegral::VectorIntegral(const Expr&    operand,
                               const Tensor& t_variables)
    :AbstractIntegral(operand), variables(t_variables)
{

}

VectorIntegral::VectorIntegral(const Expr&     operand,
                               const Tensor&  t_variables,
                               bool            t_empty)
    :AbstractIntegral(operand, t_empty), variables(t_variables)
{
    
}

VectorIntegral::VectorIntegral(const Expr&     operand,
                               const Tensor&  t_variables,
                               const Expr&     t_inf,
                               const Expr&     t_sup)
    :AbstractIntegral(operand, t_inf, t_sup), variables(t_variables)
{

}

VectorIntegral::VectorIntegral(const Tensor&  t_variables,
                               const Expr&     t_inf,
                               const Expr&     t_sup)
    :VectorIntegral(CSL_1, t_variables, t_inf, t_sup)
{

}

VectorIntegral::VectorIntegral(const Expr&     operand,
                               const Tensor&   t_variables,
                               bool            t_empty,
                               const Expr&     t_inf,
                               const Expr&     t_sup)
    :AbstractIntegral(operand, t_empty, t_inf, t_sup), variables(t_variables)
{

}

Expr VectorIntegral::getVariable() const
{
    return variables->getTensor();
}

Parent VectorIntegral::getParent() const
{
    return variables;
}

Parent_info VectorIntegral::getParent_info() const
{
    return variables.get();
}

void VectorIntegral::setParent(const Parent& t_variable)
{
    if (t_variable->getPrimaryType() != cslParent::Indicial)
        callError(cslError::UndefinedBehaviour,
                "VectorIntegral::setParent()");
    Parent old_variable = variables;
    variables = std::dynamic_pointer_cast<TensorParent>(t_variable);
    Replace(argument, old_variable, variables);
    DeepRefresh(argument);
}

void VectorIntegral::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out << "int{";
    out << variables->getName() << "-" << variables->getSpace()[0]->getDim()
        << "d";
    if (inf != -CSL_INF or sup !=CSL_INF) {
        out << ", ";
        inf->print(1, out);
        out << ", ";
        sup->print(1, out);
    }
    out << "}(";
    if (argument != CSL_1)
        argument->print(1, out);
    else
        out << "#";
    if (not empty)
        out << ")";
    if (mode == 0)
        out << endl;
}

void VectorIntegral::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::vectorintegral_s(";
    argument->printCode(1, out);
    out << ", ";
    out << variables->getName();
    if (sup != CSL_INF or inf != -CSL_INF) {
        out << ", ";
        inf->printCode(1, out);
        out << ", ";
        sup->printCode(1, out);
    }
    out << ")";
}

string VectorIntegral::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << "int";
    if (inf != -CSL_INF or sup !=CSL_INF) {
        sout << "_{";
        sout << inf->printLaTeX(1);
        sout << "}^{";
        sout << sup->printLaTeX(1);
        sout << "}";
    }
    sout << "\\left(";
    sout << "d" << variables->getName() << "\\cdot";
    if (argument != CSL_1)
        sout << argument->printLaTeX(1);
    if (not empty)
        sout << "\\right)";
    if (mode == 0)
        sout << endl;

    return sout.str();
}

unique_Expr VectorIntegral::copy_unique() const
{
    return make_unique<VectorIntegral>(argument,
                                       variables,
                                       empty,
                                       inf,
                                       sup);
}

Expr VectorIntegral::deepCopy() const
{
    return csl::make_shared<VectorIntegral>(argument->deepCopy(),
                                       variables,
                                       empty,
                                       inf->deepCopy(),
                                       sup->deepCopy());
}

Expr VectorIntegral::refresh() const
{
    return vectorintegral_s(argument,
                           variables,
                           empty,
                           inf,
                           sup);
}

Expr VectorIntegral::deepRefresh() const
{
    return vectorintegral_s(DeepRefreshed(argument),
                           variables,
                           empty,
                           DeepRefreshed(inf),
                           DeepRefreshed(sup));
}


bool VectorIntegral::isIndexed() const
{
    return argument->isIndexed();
}

bool VectorIntegral::operatorAppliesOn(Expr_info expr) const
{
    return expr->dependsOn(variables.get());
}

IndexStructure VectorIntegral::getIndexStructure() const
{
    if (argument->isIndexed())
        return argument->getIndexStructure();
    return IndexStructure();
}

bool VectorIntegral::compareWithDummy(
        Expr_info other,
        map<Index,Index>& constraints,
        bool keepAllCosntraints
        ) const
{
    if (not isIndexed())
        return *this == other;
    if (int test = testDummy(other); test != -1)
        return test;
    if (other->getType() != csl::Type::VectorIntegral)
        return false;
    if (Comparator::getDummyVecIntComparisonActive()
            and not empty 
            and not other->isEmpty()
            and variables != other->getParent()) {
        Expr otherModified = DeepCopy(other);
        otherModified->setParent(variables);
        if (*this == otherModified.get())
            return true;
        if (not ((-inf)->expand() == sup)) {
            return false;
        }
        // Trying to invert the variable if the integral domain is 
        // symmetric around 0.
        Index ind = variables->getSpace()[0]->generateIndex();
        Replace(otherModified,
                (*variables)(ind),
                -(*variables)(ind));
        auto res = compareWithDummy(otherModified.get(),
                                constraints,
                                keepAllCosntraints);
        return res;
    }

    
    bool res =  (
            argument->compareWithDummy(other->getArgument().get(),
                                       constraints,
                                       keepAllCosntraints)
            and variables == other->getParent()
            and empty     == other->isEmpty()
            and inf       == other->getInfBoundary()
            and sup       == other->getSupBoundary());

    return res;
}

bool VectorIntegral::operator==(Expr_info other) const
{
    if (int test = testDummy(other); test != -1)
        return test;
    if (other->getType() != csl::Type::VectorIntegral)
        return false;
    if (Comparator::getDummyVecIntComparisonActive()
            and not empty 
            and not other->isEmpty()) {
        Comparator::setDummyVecIntComparisonActive(false);
        Expr otherModified = DeepCopy(other);
        if (variables != other->getParent())
            otherModified->setParent(variables);
        if (*this == otherModified.get()) {
            Comparator::setDummyVecIntComparisonActive(true);
            return true;
        }
        if (not (Expanded(-inf) == sup)) {
            Comparator::setDummyVecIntComparisonActive(true);
            return false;
        }
        // Trying to invert the variable if the integral domain is 
        // symmetric around 0.
        Index ind = variables->getSpace()[0]->generateIndex();
        Replace(otherModified,
                (*variables)(ind),
                -(*variables)(ind));
        bool res = (*this == otherModified.get());
        Comparator::setDummyVecIntComparisonActive(true);
        return res;
    }
    
    bool res =  (
            argument      == other->getArgument()
            and variables == other->getParent()
            and empty     == other->isEmpty()
            and inf       == other->getInfBoundary()
            and sup       == other->getSupBoundary());

    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Outsider functions                            //
/*************************************************/
///////////////////////////////////////////////////

void MakeScalarIntegral(Expr& init)
{
    Expr operand  = init->getOperand();
    Expr variable = init->getVariable();
    if (init->isEmpty()) {
        if (operand != CSL_1 
                and not operand->dependsOn(variable.get())) {
            init = CSL_0;
            return;
        }
    }
    else
        if (not operand->dependsOn(variable.get())) {
            init = CSL_0;
            return;
        }
    init->setOperand(CSL_1);
    if (operand == CSL_0) {
        init = CSL_0;
        return;
    }
    else if (operand->getType() == csl::Type::Sum) {
        csl::vector_expr args = operand->getVectorArgument();
        for (auto& arg : args) {
            Expr foo = Copy(init);
            arg = foo->applyOperator(arg);
        }
        init = sum_s(args);
        return;
    }
    if (operand != CSL_1)
        init = init->applyOperator(operand);
}

void MakeIntegral(Expr& init)
{
    if (init->getType() != csl::Type::VectorIntegral) {
        MakeScalarIntegral(init);
        return;
    }
    Expr operand  = init->getOperand();
    Expr variable = init->getVariable();
    if (init->isEmpty()) {
        if (operand != CSL_1 
                and not operand->dependsOn(init->getParent_info())) {
            init = CSL_0;
            return;
        }
    }
    else
        if (not operand->dependsOn(init->getParent_info())) {
            init = CSL_0;
            return;
        }
    init->setOperand(CSL_1);
    if (operand == CSL_0) {
        init = CSL_0;
        return;
    }
    else if (operand->getType() == csl::Type::Sum) {
        csl::vector_expr args = operand->getVectorArgument();
        for (auto& arg : args) {
            Expr foo = Copy(init);
            arg = foo->applyOperator(arg);
        }
        init = sum_s(args);
        return;
    }
    if (operand != CSL_1)
        init = init->applyOperator(operand);
}

Expr vectorintegral_s(const Tensor& variables) {
    Expr res = csl::make_shared<VectorIntegral>(variables);
    return res;
}
} // End of namespace csl
