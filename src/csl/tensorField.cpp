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

#include "tensorField.h"
#include "comparison.h"
#include "error.h"
#include "space.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// TensorFieldParent                             //
/*************************************************/
///////////////////////////////////////////////////

TensorFieldParent::TensorFieldParent() : TensorParent()
{
}

TensorFieldParent::TensorFieldParent(const string &t_name)
    : TensorParent(t_name)
{
}

TensorFieldParent::TensorFieldParent(const string &t_name, const Space *)
    : TensorParent(t_name)
{
}

TensorFieldParent::TensorFieldParent(const string                &t_name,
                                     const Space                 *t_field,
                                     const vector<const Space *> &spaces)
    : TensorParent(t_name, spaces), spaceField(t_field)
{
}

cslParent::PrimaryType TensorFieldParent::getPrimaryType() const
{
    return cslParent::Field;
}

cslParent::Type TensorFieldParent::getType() const
{
    return cslParent::TensorFieldElement;
}

void TensorFieldParent::printDefinition(std::ostream &out,
                                        int           indentSize,
                                        bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::TensorField " << regName << "("
        << "\"" << csl::Abstract::regularLiteral(name) << "\", ";
    out << csl::Abstract::regularName(spaceField->getName()) << ", {";
    for (size_t i = 0; i != space.size(); ++i) {
        out << csl::Abstract::regularName(space[i]->getName());
        if (i + 1 != space.size())
            out << ", ";
    }
    out << "});\n";
    printPropDefinition(out, indentSize, header);
}

void TensorFieldParent::checkFieldRequest(TensorParent *t_point)
{
    if (t_point->getSpace() != vector<const Space *>(1, spaceField))
        callError(cslError::BadFieldInitialization,
                  "FieldParent::operator()(const TensorParent& vector)");
}

const Space *TensorFieldParent::getFieldSpace() const
{
    return spaceField;
}

void TensorFieldParent::setFieldSpace(const Space *t_space)
{
    spaceField = t_space;
}

vector<Parent>
TensorFieldParent::breakSpace(const Space                 *broken,
                              const vector<const Space *> &newSpace,
                              const vector<size_t>        &pieces) const
{
    return TensorParent::breakSpace(broken, newSpace, pieces);
}

Expr TensorFieldParent::operator()(const Tensor &t_vector)
{
    checkFieldRequest(t_vector.get());
    if (keepBestPermutation)
        return tensorfieldelement_s(t_vector, self());
    return csl::make_shared<TensorFieldElement>(t_vector, self());
}

Expr TensorFieldParent::operator()(Index t_index, const Tensor &t_vector)
{
    createFixedIndices(t_index);
    checkIndicialAndFieldRequest(t_index, t_vector.get());
    if (keepBestPermutation)
        return tensorfieldelement_s(
            t_vector, self(), vector<Index>(1, t_index));
    return csl::make_shared<TensorFieldElement>(
        t_vector, self(), vector<Index>(1, t_index));
}

Expr TensorFieldParent::operator()(const vector<int> &indices,
                                   const Tensor      &t_vector)
{
    return (*this)(integerToIndices(indices), t_vector);
}

Expr TensorFieldParent::operator()(vector<Index> indices,
                                   const Tensor &t_vector)
{
    createFixedIndices(indices);
    checkIndicialAndFieldRequest(indices, t_vector.get());
    if (keepBestPermutation)
        return tensorfieldelement_s(t_vector, self(), indices);
    return csl::make_shared<TensorFieldElement>(t_vector, self(), indices);
}

void TensorFieldParent::checkIndicialAndFieldRequest(TensorParent *point)

{
    checkIndicialAndFieldRequest(vector<Index>(), point);
}

void TensorFieldParent::checkIndicialAndFieldRequest(const Index  &index,
                                                     TensorParent *point)
{
    checkIndicialAndFieldRequest(vector<Index>(1, index), point);
}

void TensorFieldParent::checkIndicialAndFieldRequest(
    const vector<Index> &indices, TensorParent *point)
{
    TensorParent::checkIndexRequest(indices);
    checkFieldRequest(point);
}

///////////////////////////////////////////////////
/*************************************************/
// TDerivativeParent                             //
/*************************************************/
///////////////////////////////////////////////////

TDerivativeParent::TDerivativeParent() : TensorFieldParent()
{
}

TDerivativeParent::TDerivativeParent(const string &t_name,
                                     const Space  *t_space)
    : TensorFieldParent(t_name,
                        t_space,
                        {
                            t_space,
                        })
{
    // Constructor works badly here
    // Forced to re-assign this attribute
    name = t_name;
}

void TDerivativeParent::printDefinition(std::ostream &out,
                                        int           indentSize,
                                        bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "csl::TDerivative " << regName << "_der("
        << "\"" << csl::Abstract::regularLiteral(name) << "\", &";
    out << csl::Abstract::regularName(spaceField->getName()) << ");\n";
}

Expr TDerivativeParent::operator()(Index index, const Tensor &point)
{
    return tderivativeelement_s(point, self(), index);
}

///////////////////////////////////////////////////
/*************************************************/
// TDerivativeElement                           //
/*************************************************/
///////////////////////////////////////////////////

TDerivativeElement::TDerivativeElement(const Tensor &t_point,
                                       const Parent &t_parent,
                                       const Index  &t_index)
    : Operator<TensorFieldElement>(
        t_point, t_parent, vector<Index>(1, t_index))
{
    selfCheckIndexStructure();
}

TDerivativeElement::TDerivativeElement(const Tensor &t_point,
                                       const Parent &t_parent,
                                       const Index  &t_index,
                                       const Expr   &t_operand,
                                       bool          t_empty)
    : TDerivativeElement(t_point, t_parent, t_index)
{
    empty   = t_empty;
    operand = t_operand;
    selfCheckIndexStructure();
}

csl::Type TDerivativeElement::getType() const
{
    return csl::Type::TDerivativeElement;
}

size_t TDerivativeElement::size() const
{
    return 1;
}

void TDerivativeElement::print(int mode, std::ostream &out, LibraryMode) const
{
    TensorFieldElement::print(max(1, mode), out);
    out << "(";
    if (not empty or operand != CSL_1)
        operand->print(1, out);
    if (not empty)
        out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

void TDerivativeElement::printCode(int mode, std::ostream &out) const
{
    if (isEmpty()) {
        TensorFieldElement::printCode(mode, out);
        return;
    }
    out << "csl::tderivativeelement_s(";
    out << regularName(point->getName()) << ", ";
    out << regularName(parent->getName()) << "_der, ";
    out << "{";
    for (size_t i = 0; i != index.size(); ++i) {
        if (index[i].getSign())
            out << '+';
        out << index[i].getIndexCodeName();
        if (i + 1 != index.size())
            out << ", ";
    }
    out << "}, ";
    operand->printCode(1, out);
    out << ", " << empty << ")";
}

string TDerivativeElement::printLaTeX(int mode) const
{
    ostringstream sout;
    sout << TensorFieldElement::printLaTeX(max(1, mode));
    sout << "(";
    if (not empty or operand != CSL_1)
        sout << operand->printLaTeX(1);
    if (not empty)
        sout << ")";
    printProp(sout);
    if (mode == 0)
        sout << endl;

    return sout.str();
}

std::vector<csl::Parent> TDerivativeElement::getSubSymbols() const
{
    std::vector<csl::Parent> dep    = {parent, point};
    std::vector<csl::Parent> argDep = operand->getSubSymbols();
    dep.insert(dep.end(),
               std::make_move_iterator(argDep.begin()),
               std::make_move_iterator(argDep.end()));
    return dep;
}

optional<Expr> TDerivativeElement::factor(bool full) const
{
    if (full) {
        optional<Expr> argFacto = operand->factor(full);
        if (argFacto) {
            Expr result = copy();
            result[0]   = *argFacto;
            return Refreshed(result);
        }
    }
    return nullopt;
}

optional<Expr> TDerivativeElement::factor(Expr_info expr, bool full) const
{
    if (full) {
        optional<Expr> argFacto = operand->factor(expr, full);
        if (argFacto) {
            Expr result = copy();
            result[0]   = *argFacto;
            return Refreshed(result);
        }
    }
    return nullopt;
}

optional<Expr> TDerivativeElement::collect(std::vector<Expr> const &factors,
                                           bool                     full) const
{
    if (full) {
        optional<Expr> argCollect = operand->collect(factors, full);
        if (argCollect) {
            Expr res = copy();
            res[0]   = *argCollect;
            return csl::Refreshed(res);
        }
    }
    return nullopt;
}
optional<Expr> TDerivativeElement::expand(bool full, bool inPlace) const
{
    Expr newOperand;
    if (full) {
        auto opt = operand->expand(true, inPlace);
        if (opt)
            newOperand = *opt;
        else
            newOperand = Copy(operand);
    }
    else
        newOperand = Copy(operand);
    if (newOperand->getType() == csl::Type::Sum) {
        for (auto &arg : *newOperand) {
            arg = Expanded(
                tderivativeelement_s(point, parent, index[0], arg, empty),
                full);
        }
        return newOperand;
    }
    else if (newOperand->getType() == csl::Type::Prod) {
        std::vector<Expr> summed;
        summed.reserve(newOperand->size());
        for (size_t i = 0; i != newOperand->size(); ++i) {
            std::vector<Expr> args = newOperand->getVectorArgument();
            args[i]                = tderivativeelement_s(
                point, parent, index[0], args[i], empty);
            summed.push_back(prod_s(args));
        }
        return sum_s(summed);
    }

    return nullopt;
}

optional<Expr> TDerivativeElement::expand_if(
    std::function<bool(Expr const &)> const &f, bool full, bool inPlace) const
{
    Expr newOperand;
    if (full) {
        auto opt = operand->expand_if(f, true, inPlace);
        if (opt)
            newOperand = *opt;
        else
            newOperand = Copy(operand);
    }
    else
        newOperand = Copy(operand);
    if (newOperand->getType() == csl::Type::Sum and f(newOperand)) {
        for (auto &arg : *newOperand) {
            arg = ExpandedIf(
                tderivativeelement_s(point, parent, index[0], arg, empty),
                f,
                full);
        }
        return newOperand;
    }
    else if (newOperand->getType() == csl::Type::Prod and f(newOperand)) {
        std::vector<Expr> summed;
        summed.reserve(newOperand->size());
        for (size_t i = 0; i != newOperand->size(); ++i) {
            std::vector<Expr> args = newOperand->getVectorArgument();
            args[i]                = tderivativeelement_s(
                point, parent, index[0], args[i], empty);
            summed.push_back(prod_s(args));
        }
        return sum_s(summed);
    }

    return nullopt;
}

unique_Expr TDerivativeElement::copy_unique() const
{
    unique_Expr res = make_unique<TDerivativeElement>(
        point, parent, index[0], operand, empty);

    applyComplexPropertiesOn(res);

    return res;
}

Expr TDerivativeElement::deepCopy() const
{
    return shared_ptr<TDerivativeElement>(new TDerivativeElement(
        point, parent, index[0], operand->deepCopy(), empty));
}

Expr TDerivativeElement::refresh() const
{
    Expr refreshed = operand->refresh();
    if (not empty and not operatorAppliesOn(refreshed.get()))
        return CSL_0;
    return tderivativeelement_s(point, parent, index[0], refreshed, empty);
}

Expr TDerivativeElement::deepRefresh() const
{
    Expr refreshed = operand->deepRefresh();
    if (not empty and not operatorAppliesOn(refreshed.get()))
        return CSL_0;

    return tderivativeelement_s(point, parent, index[0], refreshed, empty);
}

bool TDerivativeElement::operatorAppliesOn(Expr_info expr) const
{
    if (expr->getType() == csl::Type::TDerivativeElement)
        return false;
    return expr->dependsOn(point.get());
}

Expr const &TDerivativeElement::getArgument(int i) const
{
    CSL_ASSERT_SPEC(i == 0,
                    CSLError::IndexError,
                    "Index error in TDerivativeElement. Expected 0, "
                        + toString(i) + " given.");
    return operand;
}

Expr &TDerivativeElement::getArgument(int i)
{
    CSL_ASSERT_SPEC(i == 0,
                    CSLError::IndexError,
                    "Index error in TDerivativeElement. Expected 0, "
                        + toString(i) + " given.");
    return operand;
}

void TDerivativeElement::setArgument(Expr const &arg, int i)
{
    CSL_ASSERT_SPEC(i == 0,
                    CSLError::IndexError,
                    "Index error in TDerivativeElement. Expected 0, "
                        + toString(i) + " given.");
    setOperand(arg);
}

Expr TDerivativeElement::getOperand() const
{
    return operand;
}

void TDerivativeElement::setOperand(const Expr &t_operand)
{
    operand = t_operand;
    selfCheckIndexStructure();
}

void TDerivativeElement::setPoint(const Tensor &newPoint)
{
    TensorFieldElement::setPoint(newPoint);
    if (operand->getType() == csl::Type::TensorFieldElement
        or operand->getType() == csl::Type::ScalarField)
        operand->setPoint(newPoint);
}

bool TDerivativeElement::compareWithDummy(Expr_info          other,
                                          map<Index, Index> &constraints,
                                          bool keepAllCosntraints) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(other); test != -1)
            return test;
    }
    if (other->getType() != getType() or parent != other->getParent())
        return false;
    if (not Complexified::operator==(other))
        return false;

    map<Index, Index> copyConstraints = constraints;
    Expr              copy            = Copy(other);
    copy->setOperand(CSL_1);
    if (not TensorFieldElement::compareWithDummy(
            copy.get(), copyConstraints, keepAllCosntraints))
        return false;
    if (not operand->isIndexed()) {
        if (operand != other->getOperand())
            return false;
        constraints = copyConstraints;
        return true;
    }
    if (operand->compareWithDummy(
            other->getOperand().get(), copyConstraints, keepAllCosntraints)) {
        constraints = copyConstraints;
        return true;
    }
    return false;
}

csl::vector_expr
TDerivativeElement::breakSpace(const Space                 *brokenSpace,
                               const vector<const Space *> &newSpace,
                               const vector<string>        &indexNames) const
{
    if (operand == CSL_1) {
        return TensorFieldElement::breakSpace(
            brokenSpace, newSpace, indexNames);
    }
    Expr emptyCopy = copy();
    emptyCopy->setOperand(CSL_1);
    emptyCopy->setEmpty(true);
    Expr prod = prod_s(emptyCopy, operand, true);
    return prod->breakSpace(brokenSpace, newSpace, indexNames);
}

bool TDerivativeElement::operator==(Expr_info other) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(other); test != -1)
            return test;
    }
    if (other->getType() != getType() or parent != other->getParent())
        return false;
    if (not Complexified::operator==(other))
        return false;
    if (getIndexStructure().exactMatch(other->getIndexStructure())
        and point == other->getPoint() and operand == other->getOperand())
        return true;

    if (Comparator::getFreeIndexComparisonActive())
        return getIndexStructure().compareWithDummy(
            other->getIndexStructure(), Comparator::indexCorrespondance);

    map<Index, Index> constraints;
    if (compareWithDummy(other, constraints)) {
        if (constraints.empty())
            return true;
        for (const auto &c : constraints)
            if (c.first != c.second)
                return false;
        return true;
    }

    return false;
}

Expr const &TDerivativeElement::operator[](int i) const
{
    CSL_ASSERT_SPEC(i == 0,
                    CSLError::IndexError,
                    "Index error in TDerivativeElement. Expected 0, "
                        + toString(i) + " given.");
    return operand;
}

Expr &TDerivativeElement::operator[](int i)
{
    CSL_ASSERT_SPEC(i == 0,
                    CSLError::IndexError,
                    "Index error in TDerivativeElement. Expected 0, "
                        + toString(i) + " given.");
    return operand;
}

///////////////////////////////////////////////////
/*************************************************/
// TensorFieldElement                                   //
/*************************************************/
///////////////////////////////////////////////////

TensorFieldElement::TensorFieldElement(const Tensor &t_vector,
                                       const Parent &t_parent)
    : TensorElement(IndexStructure(), t_parent), point(t_vector)
{
}

TensorFieldElement::TensorFieldElement(const Tensor             &t_vector,
                                       const Parent             &t_parent,
                                       const std::vector<Index> &indices)
    : TensorElement(indices, t_parent), point(t_vector)
{
}

TensorFieldElement::TensorFieldElement(const Tensor         &t_vector,
                                       const Parent         &t_parent,
                                       const IndexStructure &indices)
    : TensorElement(indices, t_parent), point(t_vector)
{
}

csl::PrimaryType TensorFieldElement::getPrimaryType() const
{
    return csl::PrimaryType::Field;
}

csl::Type TensorFieldElement::getType() const
{
    return csl::Type::TensorFieldElement;
}

Tensor TensorFieldElement::getPoint() const
{
    return point;
}

void TensorFieldElement::setPoint(const Tensor &t_point)
{
    if (t_point->getSpace()
        != vector<const Space *>(1, parent->getFieldSpace()))
        callError(cslError::BadFieldInitialization,
                  "TensorFieldElement::setPoint(const Tensor&)");
    point = t_point;
}

unique_Expr TensorFieldElement::copy_unique() const
{
    unique_Expr res = make_unique<TensorFieldElement>(point, parent, index);
    applyComplexPropertiesOn(res);

    return res;
}

Expr TensorFieldElement::refresh() const
{
    return copy()->getCanonicalPermutation();
}

void TensorFieldElement::print(int mode, std::ostream &out, LibraryMode) const
{
    out << getName();
    if (index.size() > 0) {
        out << "_";
        out << index;
    }
    out << "(";
    out << point->getName();
    out << ")";
    printProp(out);
    if (mode == 0)
        out << endl;
}

void TensorFieldElement::printCode(int, std::ostream &out) const
{
    if (index.empty()) {
        if (conjugated)
            out << "csl::GetComplexConjugate(";
        out << regularName(parent->getName()) << "(";
        out << regularName(point->getName());
        out << ")";
        if (conjugated)
            out << ")";
        return;
    }
    if (conjugated)
        out << "csl::GetComplexConjugate(";
    out << regularName(parent->getName()) << "({";
    for (size_t i = 0; i != index.size(); ++i) {
        if (index[i].getSign())
            out << '+';
        out << index[i].getIndexCodeName();
        if (i + 1 != index.size())
            out << ", ";
    }
    out << "}, ";
    out << regularName(point->getName()) << ")";
    if (conjugated)
        out << ")";
}

string TensorFieldElement::printLaTeX(int) const
{
    ostringstream sout;
    sout << getLatexName();
    if (index.size() > 0) {
        sout << "_";
        sout << index;
    }
    sout << "(";
    sout << point->getName();
    sout << ")";
    printProp(sout);

    return sout.str();
}

std::vector<csl::Parent> TensorFieldElement::getSubSymbols() const
{
    return {parent, point};
}

optional<Expr> TensorFieldElement::evaluate(csl::eval::mode) const
{
    if (auto parentEval = AbstractElement::evaluate(); parentEval)
        return parentEval;
    return nullopt;
}

bool TensorFieldElement::commutesWith(Expr_info, int sign) const
{
    // To implement
    return sign == -1;
}

bool TensorFieldElement::dependsOn(Expr_info expr) const
{
    return point->dependsOn(expr) or TensorElement::dependsOn(expr);
}

bool TensorFieldElement::dependsOn(Parent_info t_parent) const
{
    return parent.get() == t_parent or point.get() == t_parent
           or TensorElement::dependsOn(t_parent);
}

bool TensorFieldElement::dependsExplicitlyOn(Expr_info expr) const
{
    return point->dependsExplicitlyOn(expr)
           or TensorElement::dependsExplicitlyOn(expr);
}

bool TensorFieldElement::compareWithDummy(Expr_info          other,
                                          map<Index, Index> &constraints,
                                          bool keepAllCosntraints) const
{
    if (other->getType() != getType()) {
        return false;
    }
    if (point != other->getPoint()) {
        return false;
    }
    return TensorElement::compareWithDummy(
        other, constraints, keepAllCosntraints);
}

bool TensorFieldElement::operator==(Expr_info expr) const
{
    if (Comparator::getDummyComparisonActive()) {
        if (int test = testDummy(expr); test != -1)
            return test;
    }
    if (expr->getType() != getType() or parent != expr->getParent()
        or point != expr->getPoint()) {
        return false;
    }
    if (not Complexified::operator==(expr))
        return false;
    if (Comparator::getFreeIndexComparisonActive()) {
        return index.compareWithDummy(expr->getIndexStructure(),
                                      Comparator::indexCorrespondance);
    }
    if (not index.exactMatch(expr->getIndexStructure()))
        return false;

    return true;
}

bool TDerivativeElement::dependsOn(Expr_info expr) const
{
    return TensorFieldElement::dependsOn(expr) or operand->dependsOn(expr);
}

bool TDerivativeElement::dependsOn(Parent_info t_parent) const
{
    return TensorFieldElement::dependsOn(t_parent)
           or operand->dependsOn(t_parent);
}

bool TDerivativeElement::dependsExplicitlyOn(Expr_info expr) const
{
    return TensorFieldElement::dependsExplicitlyOn(expr)
           or operand->dependsExplicitlyOn(expr);
}

bool TDerivativeElement::dependsExplicitlyOn(Parent_info parent) const
{
    return AbstractElement::dependsExplicitlyOn(parent)
           or operand->dependsExplicitlyOn(parent);
}

IndexStructure TDerivativeElement::getIndexStructure() const
{
    if (getOperand()->isIndexed())
        return index + getOperand()->getIndexStructure();
    return index;
}

void TDerivativeElement::setIndexStructure(const IndexStructure &t_structure)
{
    if (t_structure.size() > 1) {
        IndexStructure derivativeStruct = IndexStructure();
        derivativeStruct += t_structure[0];
        IndexStructure operandStruct = IndexStructure();
        for (size_t i = 1; i != t_structure.size(); ++i)
            operandStruct += t_structure[i];
        TensorElement::setIndexStructure(derivativeStruct);
        operand->setIndexStructure(operandStruct);
    }
    else
        TensorElement::setIndexStructure(t_structure);
}

void TDerivativeElement::selfCheckIndexStructure()
{
    if (operand->isIndexed()) {
        IndexStructure fooStruct        = operand->getIndexStructure();
        bool           contractionFound = false;
        for (size_t k = 0; k != fooStruct.size(); ++k)
            if (index[0] == fooStruct[k])
                if (fooStruct[k].getFree()
                    and index[0].testContraction(fooStruct[k])) {
                    if (contractionFound)
                        callError(cslError::UndefinedBehaviour,
                                  "TDerivativeElement::"
                                  "selfCheckIndexStructure()");
                    contractionFound = true;
                    operand          = ContractIndex(operand, fooStruct[k]);
                    index[0].setFree(false);
                    fooStruct[k].setFree(false);
                }
    }
}
void TDerivativeElement::replaceIndexInPlace(Index const &oldIndex,
                                             Index const &newIndex)
{
    TensorElement::replaceIndexInPlace(oldIndex, newIndex);
    operand->replaceIndexInPlace(oldIndex, newIndex);
}

optional<Expr> TDerivativeElement::replaceIndex(const Index &indexToReplace,
                                                const Index &newIndex,
                                                bool         refresh) const
{
    optional<Expr> optITensor
        = TensorElement::replaceIndex(indexToReplace, newIndex, refresh);
    optional<Expr> optOperand;
    if (operand->isIndexed())
        optOperand = operand->replaceIndex(indexToReplace, newIndex, refresh);
    if (optITensor) {
        Expr res = optITensor.value();
        if (optOperand)
            res->setOperand(optOperand.value());
        if (refresh)
            return Refreshed(res);
        return res;
    }
    if (optOperand) {
        Expr res = copy();
        res->setOperand(optOperand.value());
        if (refresh)
            return Refreshed(res);
        return res;
    }
    return nullopt;
}

optional<Expr>
TDerivativeElement::replaceIndices(std::vector<csl::Index> const &oldIndices,
                                   std::vector<csl::Index> const &newIndices,
                                   bool                           refresh,
                                   bool flipped) const
{
    optional<Expr> optITensor = TensorElement::replaceIndices(
        oldIndices, newIndices, refresh, flipped);
    optional<Expr> optOperand;
    if (operand->isIndexed())
        optOperand = operand->replaceIndices(
            oldIndices, newIndices, refresh, flipped);
    if (optITensor) {
        Expr res = optITensor.value();
        if (optOperand)
            res->setOperand(optOperand.value());
        if (refresh)
            return Refreshed(res);
        return res;
    }
    if (optOperand) {
        Expr res = copy();
        res->setOperand(optOperand.value());
        if (refresh)
            return Refreshed(res);
        return res;
    }
    return nullopt;
}

optional<Expr> TDerivativeElement::getComplexConjugate() const
{
    optional<Expr> opt = operand->getComplexConjugate();
    if (not opt)
        return nullopt;
    Expr conjugate = Copy(this);
    conjugate->setOperand(opt.value());
    return conjugate;
}
} // End of namespace csl
