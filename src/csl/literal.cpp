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

#include "literal.h"
#include "abreviation.h"
#include "commutation.h"
#include "comparison.h"
#include "equation.h"
#include "error.h"
#include "librarygenerator.h"
#include "operations.h"
#include "utils.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractLiteral                         //
/*************************************************/
///////////////////////////////////////////////////

size_t AbstractLiteral::memoryOverhead() const
{
    return sizeof(Equation) * props.size();
}

std::vector<Parent> AbstractLiteral::getSubSymbols() const
{
    return {};
}

const vector<Equation *> &AbstractLiteral::getProperties() const
{
    return props;
}

void AbstractLiteral::addProperty(Equation *property)
{
    for (const auto &p : props)
        if (p == property)
            return;
    props.push_back(property);
}

void AbstractLiteral::removeProperty(Equation *property)
{
    for (auto p_iter = props.begin(); p_iter != props.end(); ++p_iter)
        if (*p_iter == property) {
            props.erase(p_iter);
            break;
        }
}

///////////////////////////////////////////////////
/*************************************************/
// Class Constant                                //
/*************************************************/
///////////////////////////////////////////////////

csl::PrimaryType Constant::getPrimaryType() const
{
    return csl::PrimaryType::Literal;
}
csl::Type Constant::getType() const
{
    return csl::Type::Constant;
}

std::string const &Constant::getName() const
{
    return parent->getName();
}

std::string const &Constant::getLatexName() const
{
    return parent->getLatexName();
}

bool Constant::getCommutable() const
{
    return parent->getCommutable();
}

bool Constant::getValued() const
{
    return parent->isValued();
}

long double Constant::getValue() const
{
    if (*this == CSL_UNDEF.get())
        return std::nanl("1");
    return parent->getValue()->evaluateScalar();
}

bool Constant::dependsOn(Expr_info expr) const
{
    if (expr->getType() != csl::Type::Constant)
        return false;
    return expr->getParent_info() == parent.get();
}

Parent Constant::getParent() const
{
    return parent;
}

Parent_info Constant::getParent_info() const
{
    return parent.get();
}

csl::ComplexProperty Constant::getComplexProperty() const
{
    return parent->getComplexProp();
}

void Constant::setComplexProperty(csl::ComplexProperty prop)
{
    parent->setComplexProperty(prop);
}

void Constant::setValue(long double t_value)
{
    parent->setValue(t_value);
}

void Constant::setValue(Expr const &t_value)
{
    parent->setValue(t_value);
}

void Constant::setName(const string &name)
{
    parent->setName(name);
}

void Constant::setCommutable(bool commutable)
{
    parent->setCommutable(commutable);
}

void Constant::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    const bool conj = isComplexConjugate() && libMode != LibraryMode::NoLib;
    if (conj and LibraryGenerator::isQuadruplePrecision()) {
        out << "conjq(";
    }
    else if (conj && libMode == LibraryMode::CppLib) {
        out << "std::conj(";
    }
    else if (conj && libMode == LibraryMode::CLib) {
        out << "conj(";
    }
    std::string name(parent->getName());
    if (libMode != LibraryMode::NoLib)
        for (char &c : name)
            if (c == '\\')
                c = '_';
    if (mode == 0)
        out << name << endl;
    else
        out << name;
    if (conj)
        out << ")";
    else
        printProp(out);
}

void Constant::printCode(int, std::ostream &out) const
{
    if (*this == CSL_PI.get()) {
        out << "CSL_PI";
        return;
    }
    if (*this == CSL_E.get()) {
        out << "CSL_E";
        return;
    }
    if (*this == CSL_INF.get()) {
        out << "CSL_INF";
        return;
    }
    if (*this == CSL_UNDEF.get()) {
        out << "CSL_UNDEF";
        return;
    }
    if (not conjugated)
        out << regularName(parent->getName());
    else
        out << "csl::GetComplexConjugate(" << regularName(parent->getName())
            << ")";
}

string Constant::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0 and parent->isValued())
        sout << parent->getLatexName() << " = " << parent->getValue() << endl;
    else if (mode == 0)
        sout << parent->getLatexName() << endl;
    else
        sout << parent->getLatexName();

    return sout.str();
}

std::vector<Parent> Constant::getSubSymbols() const
{
    auto const &pointed = *parent;
    if (typeid(pointed) == typeid(Abbreviation<ConstantParent>)) {
        std::vector<Parent> dep
            = dynamic_cast<Abbreviation<ConstantParent> const *>(parent.get())
                  ->getEncapsulated()
                  ->getSubSymbols();
        dep.push_back(parent);
        return dep;
    }
    return {parent};
}

long double Constant::evaluateScalar() const
{
    if (*this == CSL_UNDEF.get()) {
        return std::nanl("2");
    }
    return parent->getValue()->evaluateScalar();
}

optional<Expr> Constant::evaluate(csl::eval::mode user_mode) const
{
    if (auto parentEval = parent->evaluate(this, user_mode); parentEval)
        return parentEval;
    if (parent->isValued()
        and eval::isContained(user_mode, csl::eval::literal)) {
        Expr value = parent->getValue();
        if (conjugated)
            return GetComplexConjugate(value);
        return value;
    }

    return nullopt;
}

int Constant::getParity(Expr_info t_variable) const
{
    if (*this == t_variable)
        return -1;
    else
        return 1;
}

unique_Expr Constant::copy_unique() const
{
    unique_Expr newConstant = make_unique<Constant>(parent);
    applyComplexPropertiesOn(newConstant);

    return newConstant;
}

void Constant::operator=(double t_value)
{
    setValue(t_value);
}

bool Constant::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != csl::Type::Constant)
        return false;
    if (not Complexified::operator==(expr))
        return false;

    return parent.get() == expr->getParent_info();
}

optional<Expr> Constant::derive(Expr_info expr) const
{
    if (*this == expr)
        return CSL_1;
    return CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Variable                                //
/*************************************************/
///////////////////////////////////////////////////

csl::PrimaryType Variable::getPrimaryType() const
{
    return csl::PrimaryType::Literal;
}
csl::Type Variable::getType() const
{
    return csl::Type::Variable;
}

std::string const &Variable::getName() const
{
    return parent->getName();
}

std::string const &Variable::getLatexName() const
{
    return parent->getLatexName();
}

bool Variable::getCommutable() const
{
    return parent->getCommutable();
}

bool Variable::getElementary() const
{
    return parent->isElementary();
}

bool Variable::getAllDependencies() const
{
    return parent->isAllDependencies();
}

bool Variable::getValued() const
{
    return parent->isValued();
}

long double Variable::getValue() const
{
    return parent->getValue()->evaluateScalar();
}

csl::ComplexProperty Variable::getComplexProperty() const
{
    return parent->getComplexProp();
}

void Variable::setComplexProperty(csl::ComplexProperty prop)
{
    parent->setComplexProperty(prop);
}

void Variable::setName(const string &name)
{
    parent->setName(name);
}

void Variable::setCommutable(bool commutable)
{
    parent->setCommutable(commutable);
}

Parent Variable::getParent() const
{
    return parent;
}

Parent_info Variable::getParent_info() const
{
    return parent.get();
}

bool Variable::dependsOn(Expr_info expr) const
{
    return parent->dependsOn(expr);
}

bool Variable::commutesWith(Expr_info expr, int sign) const
{
    return parent->commutesWith(expr, sign);
}

void Variable::setValue(long double t_value)
{
    parent->setValue(t_value);
}

void Variable::setValue(Expr const &t_value)
{
    parent->setValue(t_value);
}

void Variable::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    const bool conj = isComplexConjugate() && libMode != LibraryMode::NoLib;
    if (conj and LibraryGenerator::isQuadruplePrecision()) {
        out << "conjq(";
    }
    else if (conj && libMode == LibraryMode::CppLib) {
        out << "std::conj(";
    }
    else if (conj && libMode == LibraryMode::CLib) {
        out << "conj(";
    }
    std::string name(parent->getName());
    if (libMode != LibraryMode::NoLib)
        for (char &c : name)
            if (c == '\\')
                c = '_';
    if (mode == 0)
        out << name << endl;
    else
        out << name;
    if (conj)
        out << ")";
    else
        printProp(out);
}

void Variable::printCode(int, std::ostream &out) const
{
    if (not conjugated)
        out << regularName(parent->getName());
    else
        out << "csl::GetComplexConjugate(" << regularName(parent->getName())
            << ")";
}

string Variable::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0 and getValued())
        sout << getLatexName() << " = " << getValue() << endl;
    else if (mode == 0)
        sout << getLatexName() << endl;
    else
        sout << getLatexName();

    return sout.str();
}

std::vector<Parent> Variable::getSubSymbols() const
{
    auto const &pointed = *parent;
    if (typeid(pointed) == typeid(Abbreviation<VariableParent>)) {
        std::vector<Parent> dep
            = dynamic_cast<Abbreviation<VariableParent> const *>(parent.get())
                  ->getEncapsulated()
                  ->getSubSymbols();
        dep.push_back(parent);
        return dep;
    }
    return {parent};
}

long double Variable::evaluateScalar() const
{
    CSL_ASSERT_SPEC(getValued(), CSLError::ValueError, "variable not valued.");

    return getValue();
}

optional<Expr> Variable::evaluate(csl::eval::mode user_mode) const
{
    if (auto parentEval = parent->evaluate(this, user_mode); parentEval)
        return parentEval;
    if (parent->isValued()
        and eval::isContained(user_mode, csl::eval::literal)) {
        Expr value = parent->getValue();
        if (conjugated)
            return GetComplexConjugate(value);
        return value;
    }
    return nullopt;
    // if (getValued())
    //     return autonumber_s(getValue());
    // else
    //     return nullopt;
}

int Variable::getParity(Expr_info t_variable) const
{
    if (*this == t_variable)
        return -1;
    else
        return 1;
}

unique_Expr Variable::copy_unique() const
{
    unique_Expr newVariable = make_unique<Variable>(parent);
    applyComplexPropertiesOn(newVariable);

    return newVariable;
}

void Variable::setElementary(bool t_elementary)
{
    parent->setElementary(t_elementary);
}

void Variable::setAllDependencies(bool t_allDependencies)
{
    parent->setAllDependencies(t_allDependencies);
}

void Variable::addDependency(Expr const &expr)
{
    parent->addDependency(expr.get());
}

void Variable::removeDependency(Expr const &expr)
{
    parent->removeDependency(expr.get());
}

void Variable::operator=(double t_value)
{
    setValue(t_value);
}

bool Variable::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != csl::Type::Variable)
        return false;
    if (not Complexified::operator==(expr))
        return false;

    return parent->getName() == expr->getName();
}

optional<Expr> Variable::derive(Expr_info expr) const
{
    // dx/dx = 1
    if (operator==(expr))
        return CSL_1;
    // If elementary: dx/dy=0
    else if (not parent->isElementary() and dependsOn(expr))
        return csl::make_shared<Derivative>(copy(), Copy(expr));

    return CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Imaginary                               //
/*************************************************/
///////////////////////////////////////////////////

optional<Expr> Imaginary::getRealPart() const
{
    return CSL_0;
}

Expr Imaginary::getImaginaryPart() const
{
    return CSL_1;
}

optional<Expr> Imaginary::getComplexModulus() const
{
    return CSL_1;
}

optional<Expr> Imaginary::getComplexArgument() const
{
    return CSL_PI / CSL_2;
}

void Imaginary::print(int mode, std::ostream &out, LibraryMode libMode) const
{
    if (libMode == LibraryMode::CLib)
        out << "_Complex_I";
    else
        out << "i";
    if (mode == 0)
        out << endl;
    printProp(out);
}

void Imaginary::printCode(int, std::ostream &out) const
{
    out << "CSL_I";
}

string Imaginary::printLaTeX(int) const
{
    return "i";
}

long double Imaginary::evaluateScalar() const
{
    cout << "Warning: evaluating scalar (real)";
    cout << "of complex expr! replacing i by 0.\n";
    return 0;
}

optional<Expr> Imaginary::evaluate(csl::eval::mode user_mode) const
{
    if (eval::isContained(user_mode, eval::numerical))
        return csl::complex_s(CSL_0, CSL_1);
    return CSL_I;
}

unique_Expr Imaginary::copy_unique() const
{
    return make_unique<Imaginary>();
}

optional<Expr> Imaginary::derive(Expr_info) const
{
    return CSL_0;
}

bool Imaginary::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }

    return (expr->getType() == csl::Type::Imaginary);
}

///////////////////////////////////////////////////
/*************************************************/
// Class IntFactorial                              //
/*************************************************/
///////////////////////////////////////////////////

void IntFactorial::setValue(long double t_value)
{
    if (round(value) == value)
        value = t_value;
    else
        CALL_SMERROR_SPEC(CSLError::ValueError,
                          "floating value in IntFactorial.");
}

void IntFactorial::print(int mode, std::ostream &out, LibraryMode) const
{
    if (mode == 0)
        out << "IntFactorial: " << value << "!" << endl;
    else
        out << value << "!";
    printProp(out);
}

void IntFactorial::printCode(int, std::ostream &out) const
{
    out << "csl::intfactorial_s(" << value << ")";
}

string IntFactorial::printLaTeX(int mode) const
{
    ostringstream sout;
    if (mode == 0)
        sout << value << "!" << endl;
    else
        sout << value << "!";

    return sout.str();
}

long double IntFactorial::evaluateScalar() const
{
    return (double) factorial(value);
}

optional<Expr> IntFactorial::evaluate(csl::eval::mode) const
{
    return int_s(factorial(value));
}

unique_Expr IntFactorial::copy_unique() const
{
    return make_unique<IntFactorial>(value);
}

Expr IntFactorial::refresh() const
{
    return intfactorial_s(value);
}

void IntFactorial::operator=(int t_value)
{
    value = t_value;
}

bool IntFactorial::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != csl::Type::IntFactorial)
        return 0;

    return value == expr->getValue();
}

optional<Expr> IntFactorial::derive(Expr_info) const
{
    return CSL_0;
}

Expr intfactorial_s(int value)
{
    if (value < 0)
        return prod_s(pow_s(CSL_M_1, int_s(abs(value))),
                      intfactorial_s(-value));
    if (value == 0 or value == 1)
        return CSL_1;
    if (value == 2)
        return CSL_2;
    return csl::make_shared<IntFactorial>(value);
}

Expr constant_s(string const &name, csl::ComplexProperty prop)
{
    shared_ptr<ConstantParent> parent = csl::make_shared<ConstantParent>(name);
    parent->setComplexProperty(prop);

    return csl::make_shared<Constant, alloc_constant>(parent);
}

Expr constant_s(string const        &name,
                long double          value,
                csl::ComplexProperty prop)
{
    shared_ptr<ConstantParent> parent
        = csl::make_shared<ConstantParent>(name, value);
    parent->setComplexProperty(prop);

    return csl::make_shared<Constant, alloc_constant>(parent);
}

Expr constant_s(string const        &name,
                Expr const          &value,
                csl::ComplexProperty prop)
{
    shared_ptr<ConstantParent> parent
        = csl::make_shared<ConstantParent>(name, value);
    parent->setComplexProperty(prop);

    return csl::make_shared<Constant, alloc_constant>(parent);
}

Expr variable_s(string const &name, csl::ComplexProperty prop)
{
    shared_ptr<VariableParent> parent = csl::make_shared<VariableParent>(name);
    parent->setComplexProperty(prop);

    return csl::make_shared<Variable, alloc_variable>(parent);
}

Expr variable_s(string const        &name,
                long double          value,
                csl::ComplexProperty prop)
{
    shared_ptr<VariableParent> parent
        = csl::make_shared<VariableParent>(name, value);
    parent->setComplexProperty(prop);

    return csl::make_shared<Variable, alloc_variable>(parent);
}

Expr variable_s(string const        &name,
                Expr const          &value,
                csl::ComplexProperty prop)
{
    shared_ptr<VariableParent> parent
        = csl::make_shared<VariableParent>(name, value);
    parent->setComplexProperty(prop);

    return csl::make_shared<Variable, alloc_variable>(parent);
}

} // End of namespace csl
