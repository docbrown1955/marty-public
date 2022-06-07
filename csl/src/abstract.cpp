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

#include "abstract.h"
#include "utils.h"
#include "literal.h"
#include "numerical.h"
#include "operations.h"
#include "indicial.h"
#include "simplification.h"
#include "options.h"
#include "comparison.h"
#include "librarydependency.h"
using namespace std;

namespace csl {

Expr Abstract::self()
{
    return CopySelf(this);
}

static string emptyString = "";

std::string const &Abstract::getName() const
{
    return emptyString;
}

std::string const &Abstract::getLatexName() const
{
    return getName();
}

bool Abstract::getCommutable() const
{
    return true;
}

std::size_t Abstract::memoryOverhead() const
{
    return 0;
}

void Abstract::setName(const string&)
{

}

void Abstract::setCommutable(bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

int Abstract::getDim() const 
{
    return 0;
}

bool Abstract::getElementary() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

bool Abstract::getAllDependencies() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

const vector<Equation*>& Abstract::getProperties() const
{
    static std::vector<Equation*> empty;
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return empty;
}

void Abstract::printCode(
        int,
        std::ostream &
        ) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

std::string Abstract::regularName(std::string const &name)
{
    return regularName(std::string_view(name));
}

std::string Abstract::regularName(std::string_view name)
{
    if (name.empty())
        return "_";
    std::string reg(name);
    for (char &c : reg) {
        if (c >= 'a' and c <= 'z') continue;
        if (c >= 'A' and c <= 'Z') continue;
        if (c >= '0' and c <= '9') continue;
        if (c == '_') continue;
        if (c == '+')
            c = 'p';
        else if (c == '-')
            c = 'm';
        else if (c == '~')
            c = 's';
        else if (c != '[' and c != ']')
            c = '_';
    }
    if (reg[0] >= '0' and reg[0] <= '9')
        reg.insert(reg.begin(), '_');
    return reg;
}

std::string Abstract::regularLiteral(std::string const &name)
{
    return regularLiteral(std::string_view(name));
}

std::string Abstract::regularLiteral(std::string_view name)
{
    std::string reg(name);
    for (std::size_t i = 0; i != reg.size(); ++i)
        if (reg[i] == '\\') {
            reg.insert(reg.begin()+i, '\\');
            ++i;
        }
    return reg;
}

void Abstract::printProp(std::ostream&) const
{

}

void Abstract::printExplicit(int mode) const
{
    cout<<"EXPLICIT PRINT OF "; print(mode);
    cout<<"type="<<getType()<<" : ";
    print(1);
    cout<<" ;\n";
    if (getNArgs() > 0)
        for (int i=0; i<getNArgs(); i++)
            getArgument(i)->printExplicit(mode);

    cout<<endl;
    cout<<"END OF EXPLICIT PRINT OF "; print(mode);
}

string Abstract::printLaTeX(int) const 
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return "";
}

LibDependency Abstract::getLibDependency() const
{
    return LibDependency();
}

bool Abstract::getValued() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

long long int Abstract::getNum() const
{
    return 0;
}

long long int Abstract::getDenom() const
{
    return 1;
}

int Abstract::getNArgs(int) const
{
    //cout<<"Warning: function getNArgs of Abstract called. Should not be.\n";
    return 0;
}

int Abstract::getNIndices() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

Index Abstract::getIndex(int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return Index();
}

void Abstract::resetIndexStructure()
{
    
}

IndexStructure Abstract::getIndexStructure() const
{
    return IndexStructure();
}

IndexStructure Abstract::getIndexStructure(csl::Space const * space) const
{
    csl::IndexStructure structure = getIndexStructure();
    for (std::size_t i = 0; i != structure.size(); ++i) 
        if (structure[i].getSpace() != space) {
            structure.erase(structure.begin() + i);
            --i;
        }
    return structure;
}

const IndexStructure& Abstract::getIndexStructureView() const
{
    static IndexStructure empty;
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return empty;
}

IndexStructure& Abstract::getIndexStructureView()
{
    static IndexStructure empty;
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return empty;
}

IndexStructure Abstract::getFreeIndexStructure() const
{
    return getIndexStructure().getFreeStructure();
}

Parent Abstract::getParent() const
{
    return nullptr;
}

Parent_info Abstract::getParent_info() const
{
    return nullptr;
}

Tensor Abstract::getPoint() const 
{
    return nullptr;
}

void Abstract::setParent(const Parent&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

optional<Expr> Abstract::replaceIndex(
        const Index&,
        const Index&,
        bool) const
{
    return nullopt;
}

optional<Expr> Abstract::replaceIndices(
        std::vector<csl::Index> const&,
        std::vector<csl::Index> const&,
        bool,
        bool) const
{
    return nullopt;
}

 void Abstract::replaceIndexInPlace(
     Index const &,
     Index const &)
{

}

optional<Expr> Abstract::contractIndex(const Index& index) const
{
    Index dummyIndex = index;
    dummyIndex.setFree(false);
    return replaceIndex(index, dummyIndex);
}

csl::vector_expr Abstract::breakSpace(
        const Space*                brokenSpace,
        const vector<const Space*>& newSpace) const
{
    vector<string> names(newSpace.size());
    for (std::size_t i = 0; i != names.size(); ++i)
        if (newSpace[i])
            names[i] = "i";
    return breakSpace(brokenSpace, newSpace, names);
}

csl::vector_expr Abstract::breakSpace(
        const Space*,
        const vector<const Space*>&,
        const vector<string>&
        ) const
{
    return csl::vector_expr();
}

void Abstract::setIndexStructure(const IndexStructure&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setPoint(const Tensor&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool Abstract::checkIndexStructure(const vector<Index>& t_index) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return (t_index.size()==0);
}

bool Abstract::checkIndexStructure(const initializer_list<Index>& index) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return (index.size()==0);
}

bool Abstract::compareWithDummy(Expr_info expr, 
        map<Index,Index>&,
        bool) const
{
    return (*this == expr);
}

bool Abstract::compareWithDummy(Expr_info expr,
                                bool keepAllCosntraints) const
{
    std::map<Index, Index> m;
    return compareWithDummy(expr, m, keepAllCosntraints);
}


void Abstract::addProperty(Equation*)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::removeProperty(Equation*)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setFullySymmetric()
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setFullyAntiSymmetric()
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::addSymmetry(int, int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::addAntiSymmetry(int, int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

int Abstract::getNContractedPairs() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

int Abstract::permut(int, int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

csl::vector_expr Abstract::getPermutations(bool) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

set<pair<int,int> > Abstract::getContractedPair() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

vector<int> Abstract::getShape() const
{
    return vector<int>(1,1);
}

const csl::vector_expr& Abstract::getVectorArgument() const
{
    static const csl::vector_expr empty;
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return empty;
}

std::size_t Abstract::size() const
{
    return 0;
}

bool Abstract::empty() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

csl::vector_expr::iterator Abstract::begin()
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return csl::vector_expr::iterator();
}

csl::vector_expr::iterator Abstract::end()
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return csl::vector_expr::iterator();
}

csl::vector_expr::const_iterator Abstract::begin() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return csl::vector_expr::const_iterator();
}

csl::vector_expr::const_iterator Abstract::end() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return csl::vector_expr::const_iterator();
}

Expr const &Abstract::getArgument(int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

static Expr undef = CSL_UNDEF;

Expr& Abstract::getArgument(int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return undef;
}

Expr const &Abstract::getArgument(const vector<int>&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr& Abstract::getArgument(const vector<int>&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return undef;
}

bool Abstract::isBuildingBlock() const {
    return false;
}

int Abstract::getOrderOf(Expr_info expr) const {
    return (*this == expr);
}

bool Abstract::isArbitrary() const {
    return false;
}

bool Abstract::isIndexed() const {
    return false;
}

bool Abstract::isReal() const {
    return true;
}

bool Abstract::isPurelyImaginary() const {
    return false;
}

bool Abstract::isComplexConjugate() const {
    return false;
}

bool Abstract::isHermitianConjugate() const {
    return false;
}

csl::ComplexProperty Abstract::getComplexProperty() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return csl::ComplexProperty::Real;
}

void Abstract::setComplexProperty(csl::ComplexProperty)
{

}

void Abstract::setConjugated(bool)
{

}

bool Abstract::isInteger() const {
    return false;
}

long double Abstract::getValue() const 
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return std::nanl("1");
}

long double Abstract::getDeltaPlus() const
{
    return 0;
}

long double Abstract::getDeltaMinus() const
{
    return 0;
}

Expr Abstract::getNumericalFactor() const
{
    return CSL_1;
}

int Abstract::getNFactor() const
{
    return 1;
}

csl::vector_expr Abstract::getFactors() const
{
    return csl::vector_expr(0);
}

std::optional<Expr> Abstract::getTerm() const 
{
    return nullopt;
}

void Abstract::getExponents(
        std::vector<Expr> const &factors,
        std::vector<Expr>       &exponents
        ) const
{
    for (std::size_t i = 0; i != factors.size(); ++i) 
        if (*this == factors[i].get()) {
            exponents[i] = CSL_1;
            return;
        }
}

int Abstract::getOrder() const 
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

int Abstract::getSign() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

bool Abstract::isAnOperator() const
{
    return false;
}

bool Abstract::isEmpty() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

bool Abstract::operatorAppliesOn(Expr_info) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

Expr Abstract::getOperand() const
{
    print();
    cout << "Type = " << getType() << endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

optional<Expr> Abstract::getRealPart() const
{
    return nullopt;
}

Expr Abstract::getImaginaryPart() const
{
    return CSL_0;
}

optional<Expr> Abstract::getComplexModulus() const
{
    return nullopt;
}

optional<Expr> Abstract::getComplexArgument() const
{
    return CSL_0;
}

optional<Expr> Abstract::getComplexConjugate() const
{
    return nullopt;
}

Expr& Abstract::applySelfStructureOn(Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return undef;
}

optional<Expr> Abstract::getTransposed(
        const Space*,
        bool) const
{
    return nullopt;
}

optional<Expr> Abstract::getTransposed(
        const vector<const Space*>&,
        bool) const
{
    return nullopt;
}

optional<Expr> Abstract::getHermitianConjugate(
        const Space*) const
{
    return getComplexConjugate();
}

optional<Expr> Abstract::getHermitianConjugate(
        const vector<const Space*>&) const
{
    return getComplexConjugate();
}

Expr Abstract::deepCopy() const
{
    return copy();
}

Expr Abstract::refresh() const
{
    return copy();
}

Expr Abstract::deepRefresh() const
{
    return refresh();
}

void Abstract::insert(const Expr&, bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool Abstract::askTerm(Expr_info expr, bool) const
{
    return this->operator==(expr);
}

optional<Expr> Abstract::expand(bool, bool) const
{
    return nullopt;
}
optional<Expr> Abstract::expand_if(
        std::function<bool(Expr const&)> const&,
        bool,
        bool) const
{
    return nullopt;
}
optional<Expr> Abstract::factor(bool) const
{
    return nullopt;
}
optional<Expr> Abstract::factor(Expr_info, bool) const
{
    return nullopt;
}

optional<Expr> Abstract::collect(
        std::vector<Expr> const &,
        bool                     
        ) const
{
    return nullopt;
}

optional<Expr> Abstract::derive(Expr_info) const
{
    return CSL_0;
}

Expr Abstract::suppressTerm(Expr_info) const
{
    return float_s(1);
}

std::optional<Expr> Abstract::suppressExponent(
        Expr const &factor,
        Expr const &exponent
        ) const
{
    if (*this == factor.get() and exponent == CSL_1)
        return CSL_1;
    return std::nullopt;
}

void Abstract::setValue(long double)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setValue(Expr const &)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setElementary(bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setAllDependencies(bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::addDependency(Expr const&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::removeDependency(Expr const&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setArgument(const Expr&, int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setArgument(const Expr&, const vector<int>&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setEmpty(bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setOperandPrivate(const Expr&, bool)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setOperand(const Expr&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}
void Abstract::setVectorArgument(const csl::vector_expr&)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setVariable(Expr const &)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

long double Abstract::evaluateScalar() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return std::nanl("1");
}

Expr Abstract::addition_own(const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_0;
}

Expr Abstract::multiplication_own(const Expr&, bool) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_0;
}

Expr Abstract::division_own(const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_0;
}

Expr Abstract::exponentiation_own(const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_0;
}

bool Abstract::dependsOn(Expr_info expr) const
{
    return dependsExplicitlyOn(expr);
}

bool Abstract::dependsOn(Parent_info parent) const
{
    return parent->dependsOn(this);
}

bool Abstract::dependsExplicitlyOn(Expr_info expr) const
{
    return *this == expr;
}

bool Abstract::dependsExplicitlyOn(Parent_info parent) const
{
    return parent->dependsExplicitlyOn(this);
}

bool Abstract::commutesWith(Expr_info, int sign) const
{
    return sign == -1;
}

optional<Expr> Abstract::findSubExpression(
        Expr_info subExpression,
        const Expr& newExpression) const
{
    if (operator==(subExpression))
        return newExpression;
    return nullopt;
}

int Abstract::isPolynomial(Expr_info expr) const
{
    return dependsExplicitlyOn(expr);
}

Expr Abstract::getVariable() const
{
    return CSL_0;
}

optional<Expr> Abstract::getPolynomialTerm(
        Expr_info t_variable, int order) const
{
    if (order==1 and *this==t_variable)
        return CSL_1;
    if (order == 0)
        return nullopt;
    return CSL_0;
}

int Abstract::getParity(Expr_info) const
{
    return 1;
}

Expr Abstract::getRegularExpression() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

void Abstract::operator=(double)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool Abstract::operator!=(int t_value) const
{
    return !(this->operator==(t_value));
}

bool Abstract::operator!=(double t_value) const
{
    return !(this->operator==(t_value));
}

bool Abstract::operator==(int t_value) const
{
    return Abstract::operator==((double)t_value);
}

bool Abstract::operator==(double t_value) const
{
    if (getPrimaryType() == csl::PrimaryType::Numerical)
        return evaluateScalar() == t_value;
    if (getPrimaryType() == csl::PrimaryType::Literal and
            (getType() == csl::Type::Variable or
             getType() == csl::Type::Constant))
        return evaluateScalar() == t_value;
    return false;
}

Expr const &Abstract::operator[](int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}
Expr& Abstract::operator[](int)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return undef;
}

csl::vector_expr Abstract::getAlternateForms() const
{
    return csl::vector_expr();
}

Expr Abstract::applyOperator(const Expr&, bool) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}


bool Abstract::matchShape(Expr_info expr, bool exact) const
{
    // Return true if we compare to another scalar or if it is not exact
    // This function assumes that the Object called has dimension 0
    return (not exact or expr->getDim() == 0);
}

bool Abstract::hasContractionProperty(Expr_info) const 
{
    return false;
}

bool Abstract::hasChainContractionProperty() const 
{
    return false;
}

vector<ContractionChain> Abstract::getContractionProperties() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

Expr Abstract::contraction(Expr_info) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::contraction(const csl::vector_expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::tensordot(const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::dot(const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::getSum() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::getProduct() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::getVectorialModulus() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::getSubVectorial(int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::getSubVectorial(int, int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::getSubVectorial(const vector<int>&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::getSubVectorial(const vector<vector<int>>& keepIndices) const
{
    if (keepIndices.empty())
        return copy();
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::getSubVectorial(
        std::vector<std::vector<int>>::const_iterator begin,
        std::vector<std::vector<int>>::const_iterator end) const
{
    if (begin == end)
        return copy();
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}


Expr Abstract::determinant() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::trace() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::trace(int, int) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr Abstract::transpose() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::hermitian() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::symmetrise() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::antisymmetrise() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::inverseMatrix() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::getCanonicalPermutation() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::getInfBoundary() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

void Abstract::setSupBoundary(Expr const &)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

void Abstract::setInfBoundary(Expr const &)
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
}

Expr Abstract::getSupBoundary() const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr Abstract::applyDiracDelta(const Expr&, const Expr&) const
{
    print();
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_0;
}

int Abstract::testDummy(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()
            and expr->isArbitrary())
        return *expr == *this;

    return -1;
}

Expr::Expr(int value)
    :Expr(csl::int_s(value)) {}
Expr::Expr(long int value)
    :Expr(csl::int_s(value)) {}
Expr::Expr(long long int value)
    :Expr(csl::int_s(value)) {}
Expr::Expr(unsigned int value)
    :Expr(csl::int_s(value)) {}
Expr::Expr(unsigned long int value)
    :Expr(csl::int_s(value)) {}
Expr::Expr(unsigned long long int value)
    :Expr(csl::int_s(value)) {}

Expr::Expr(float value)
    :Expr(csl::autonumber_s(value)) {}
Expr::Expr(double value)
    :Expr(csl::autonumber_s(value)) {}
Expr::Expr(long double value)
    :Expr(csl::autonumber_s(value)) {}

std::ostream& operator<<(std::ostream& fout, const Expr& obj)
{
    obj->print(1, fout);
    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Arithmetic operators                          //
/*************************************************/
///////////////////////////////////////////////////

//Expr operator+(double a, const Expr& b)
//{
//    return csl::sum_s(csl::autonumber_s(a), b);
//}
//Expr operator+(const Expr& a, double b)
//{
//    return csl::sum_s(a, csl::autonumber_s(b));
//}
//Expr operator+(int a, const Expr& b)
//{
//    return csl::sum_s(csl::autonumber_s(a), b);
//}
//Expr operator+(const Expr& a, int b)
//{
//    return csl::sum_s(a, csl::autonumber_s(b));
//}
Expr operator+(const Expr& a, const Expr& b)
{
    return csl::sum_s(a,b);
}

Expr operator-(const Expr& a)
{
    if (a == CSL_1)
        return CSL_M_1;
    if (a == CSL_M_1)
        return CSL_1;
    if (a->getType() == csl::Type::Sum)
        return csl::Expanded(csl::prod_s(CSL_M_1,a));
    return csl::prod_s(CSL_M_1, a);
}

//Expr operator-(double a, const Expr& b)
//{
//    return csl::minus_(csl::autonumber_s(a), b);
//}
//Expr operator-(const Expr& a, double b)
//{
//    return csl::minus_(a, csl::autonumber_s(b));
//}
//Expr operator-(int a, const Expr& b)
//{
//    return csl::minus_(csl::autonumber_s(a), b);
//}
//Expr operator-(const Expr& a, int b)
//{
//    return csl::minus_(a, csl::autonumber_s(b));
//}
Expr operator-(const Expr& a, const Expr& b)
{
    return csl::minus_(a,b);
}

//Expr operator*(double a, const Expr& b)
//{
//    return csl::prod_s(csl::autonumber_s(a), b);
//}
//Expr operator*(const Expr& a, double b)
//{
//    return csl::prod_s(a, csl::autonumber_s(b));
//}
//Expr operator*(int a, const Expr& b)
//{
//    return csl::prod_s(csl::autonumber_s(a), b);
//}
//Expr operator*(const Expr& a, int b)
//{
//    return csl::prod_s(a, csl::autonumber_s(b));
//}
Expr operator*(const Expr& a, const Expr& b)
{
    return csl::prod_s(a,b);
}

//Expr operator/(double a, const Expr& b)
//{
//    return csl::fraction_s(csl::autonumber_s(a), b);
//}
//Expr operator/(const Expr& a, double b)
//{
//    return csl::fraction_s(a, csl::autonumber_s(b));
//}
//Expr operator/(int a, const Expr& b)
//{
//    return csl::fraction_s(csl::autonumber_s(a), b);
//}
//Expr operator/(const Expr& a, int b)
//{
//    return csl::fraction_s(a, csl::autonumber_s(b));
//}
Expr operator/(const Expr& a, const Expr& b)
{
    return csl::fraction_s(a,b);
}

//Expr operator^(double a, const Expr& b)
//{
//    return csl::pow_s(csl::autonumber_s(a), b);
//}
//Expr operator^(const Expr& a, double b)
//{
//    return csl::pow_s(a, csl::autonumber_s(b));
//}
//Expr operator^(int a, const Expr& b)
//{
//    return csl::pow_s(csl::autonumber_s(a), b);
//}
//Expr operator^(const Expr& a, int b)
//{
//    return csl::pow_s(a, csl::autonumber_s(b));
//}
//Expr operator^(const Expr& a, const Expr& b)
//{
//    return csl::pow_s(a,b);
//}

bool operator==(const Expr& a, const Expr& b) {
    return (a.get() == b.get()) or (*a==b);
}
bool operator==(const Expr& a, int b) {
    return (*a==b);
}
bool operator!=(const Expr& a, const Expr& b) {
    return (*a!=b);
}
bool operator!=(const Expr& a, int b) {
    return (*a!=b);
}
bool operator!=(const Expr& a, double b) {
    return (*a!=b);
}
bool operator>=(const Expr& a, const Expr& b) {
    return not (a < b);
}
bool operator<=(const Expr& a, const Expr& b) {
    return not (a > b);
}
bool operator>(const Expr& a, const Expr& b) {
    return (b < a);
}

bool operator<(const Expr& a, const Expr& b) {
    if (not csl::option::fullComparison) {
        auto typeA = a->getType();
        auto typeB = b->getType();
        if (typeA == typeB)
         return *a < b.get();
        return typeA < typeB;
    }
    else
        return *a < b.get();
}

} // End of namespace csl
