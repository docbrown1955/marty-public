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

#include "index.h"
#include "abstract.h"
#include "comparison.h"
#include "error.h"
#include "options.h"
#include "space.h"
#include "support.h"

#include <algorithm>

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class Index                                   //
/*************************************************/
///////////////////////////////////////////////////

Index::Index() : space(nullptr), id(0), nameOrValue(0), type(cslIndex::Free)
{
    setSign(false);
}

Index::Index(char t_value) : Index()
{
    setValue(t_value);
}

Index::Index(const std::string &t_name,
             const Space       *t_space,
             unsigned short     t_id)
    : space(t_space), id(t_id), nameOrValue(-1), type(cslIndex::Free)
{
    setName(t_name);
    setSign(false);
}

std::string_view Index::getName() const
{
    return space->getIndexNameView(std::abs(nameOrValue));
}

std::string Index::getIndexCodeName() const
{
    std::ostringstream sout;
    sout << "i_";
    space->printCode(sout, 0);
    sout << "[ " << getID() << " ]";

    return sout.str();
}

Index Index::rename() const
{
    if (space) {
        return space->generateSimilar(*this);
    }
    return *this;
}

int Index::getMax() const
{
    return space->getDim();
}

Index Index::getFlipped() const
{
    Index newIndex = *this;
    if (type == cslIndex::Fixed or space->getSignedIndex())
        newIndex.setSign(not getSign());
    return newIndex;
}

void Index::setName(const std::string &t_name)
{
    bool sign   = getSign();
    nameOrValue = space->getSpecFromIndexName(t_name);
    setSign(sign);
    *this = rename();
}

void Index::setID(ID_type t_id)
{
    id = t_id;
}

void Index::setSpace(const Space *t_space)
{
    space = t_space;
    // Here simply to check if the value corresponds to the space's dimension
    if (type == cslIndex::Fixed)
        setValue(getValue());
    if (getSign() and not space->getSignedIndex())
        setSign(false);
}

void Index::setValue(char t_value)
{
    if (t_value < 0 or (space and t_value >= space->getDim())) {
        std::cout << (int) t_value << " " << space->getName() << std::endl;
        CALL_SMERROR_SPEC(CSLError::IndexError,
                          "Index::setValue(). Setting a wrong value for "
                          "Index: "
                              + std::to_string(static_cast<int>(t_value)));
    }
    nameOrValue = t_value;
    type        = cslIndex::Fixed;
}

void Index::setFree(bool t_free)
{
    if (type == cslIndex::Fixed)
        return;
    if (t_free)
        type = cslIndex::Free;
    else
        type = cslIndex::Dummy;
}

void Index::setType(cslIndex::Type t_type)
{
    type = t_type;
}

void Index::setSign(bool t_sign)
{
    if (not space) {
        bool sign = getSign();
        if (t_sign != sign)
            nameOrValue *= -1;
    }
    else if (space->getSignedIndex()) {
        bool sign = getSign();
        if (t_sign != sign)
            nameOrValue *= -1;
    }
}

void Index::flipSign()
{
    setSign(not getSign());
}

void Index::print() const
{
    if (getSign())
        cout << "+";
    else
        cout << "-";

    if (type == cslIndex::Fixed) {
        if (nameOrValue < 0)
            cout << static_cast<int>(nameOrValue) - 1;
        else
            cout << std::abs(static_cast<int>(nameOrValue));
        return;
    }
    if (type != cslIndex::Free)
        cout << "%";
    cout << getName();
}

string Index::printLaTeX() const
{
    if (type == cslIndex::Fixed) {
        ostringstream sout;
        sout << static_cast<int>(nameOrValue);
        return sout.str();
    }
    return std::string(getName());
}

void Index::printDefinition(std::ostream &out, int indentSize) const
{
    out << std::string(indentSize, ' ') << "csl::Index ";
    out << Abstract::regularName(getIndexCodeName()) << " = ";
    space->printCode(out, 0);
    out << ".generateIndex();\n";
}

bool Index::compareWithDummy(const Index &t_index) const
{
    // equivalent to operator== for free indices,
    // more permissive for dummy indices: just tells if the
    // two indices can be equivalent with a renaming
    if (type == cslIndex::Free)
        return *this == t_index;

    return (type == t_index.type and space == t_index.space);
}

bool Index::compareWithoutSign(const Index &t_index) const
{
    if (type == cslIndex::Fixed)
        return *this == t_index;
    return (std::abs(nameOrValue) == std::abs(t_index.nameOrValue)
            and id == t_index.id and type == t_index.type
            and space == t_index.space);
}

bool Index::testContraction(const Index &t_index) const
{
    if (type == cslIndex::Fixed or t_index.getType() == cslIndex::Fixed)
        return false;
    if (space != t_index.space
        or std::abs(nameOrValue) != std::abs(t_index.nameOrValue)
        or id != t_index.id or type != cslIndex::Free
        or t_index.type != cslIndex::Free)
        return false;

    if (!csl::option::permissiveCovariantIndices && space->getSignedIndex())
        if (not(getSign() xor t_index.getSign())) {
            cout << *this << " " << t_index << endl;
            CALL_SMERROR(CSLError::RuntimeError);
        }

    return true;
}

bool Index::exactMatch(const Index &t_index) const
{
    if (csl::option::permissiveCovariantIndices) {
        return compareWithoutSign(t_index);
    }
    if (type == cslIndex::Fixed)
        return *this == t_index;
    return (nameOrValue == t_index.nameOrValue and id == t_index.id
            and type == t_index.type and space == t_index.space);
}

Index &Index::operator=(int t_value)
{
    setValue(t_value);
    return *this;
}

Index &Index::operator++()
{
    ++nameOrValue;
    return *this;
}

Index Index::operator++(int)
{
    Index copy(*this);
    ++(*this);
    return copy;
}

bool Index::operator==(const Index &t_index) const
{
    if (type == cslIndex::Fixed)
        return (t_index.getType() == cslIndex::Fixed
                and nameOrValue == t_index.nameOrValue
                and space == t_index.space);

    return (std::abs(nameOrValue) == std::abs(t_index.nameOrValue)
            and id == t_index.id and type == t_index.type
            and space == t_index.space);
}

bool Index::operator==(int t_value) const
{
    return static_cast<int>(nameOrValue) == t_value;
}

bool Index::operator==(size_t t_value) const
{
    return static_cast<int>(nameOrValue) == int(t_value);
}

bool Index::operator!=(int value) const
{
    return not(*this == value);
}

bool Index::operator!=(size_t value) const
{
    return not(*this == value);
}

bool Index::operator!=(const Index &t_index) const
{
    return not(*this == t_index);
}

bool Index::operator|=(const Index &t_index) const
{
    return operator==(t_index);
}

bool Index::operator&=(const Index &t_index) const
{
    return not operator|=(t_index);
}

bool Index::operator<(const Index &t_index) const
{
    const auto otherSpace = t_index.space;
    if (space < otherSpace)
        return true;
    if (space > otherSpace)
        return false;

    // if (type < t_index.type)
    //     return true;
    // else if (not (t_index.type < type)) {
    if (type != cslIndex::Fixed) {
        std::string_view nameA = getName();
        std::string_view nameB = t_index.getName();
        if (nameA < nameB)
            return true;
        else if (not(nameB < nameA)) {
            return id < t_index.id;
        }
    }
    else
        return nameOrValue < t_index.getValue();
    //}

    return false;
}

bool Index::operator>(const Index &t_index) const
{
    return t_index < *this;
}

bool Index::operator<=(const Index &t_index) const
{
    return (operator<(t_index) or operator|=(t_index));
}

bool Index::operator>=(const Index &t_index) const
{
    return (operator>(t_index) or operator|=(t_index));
}

Index operator!(const Index &index)
{
    Index newIndex(index);
    newIndex.setFree(not index.getFree());
    return newIndex;
}
Index operator+(const Index &index)
{
    Index newIndex = index;
    newIndex.setSign(1);

    return newIndex;
}
Index operator-(const Index &index)
{
    Index newIndex = index;
    newIndex.setSign(0);

    return newIndex;
}

ostream &operator<<(ostream &fout, const Index &index)
{
    if (index.space and index.space->getSignedIndex() and index.getSign())
        fout << "+";
    if (index.type == cslIndex::Fixed) {
        fout << static_cast<int>(index.nameOrValue);
        return fout;
    }
    if (index.type != cslIndex::Free)
        fout << "%";
    if (!index.space)
        return fout;
    std::string_view name = index.getName();
    fout << name;
    if (option::printIndexIds and index.id > 0)
        fout << "_" << index.id;

    return fout;
}

std::vector<Index> integerToIndices(const std::vector<int> &indices)
{
    vector<Index> res(indices.size());
    std::transform(indices.begin(), indices.end(), res.begin(), [](int value) {
        return Index(value);
    });
    return res;
}

///////////////////////////////////////////////////
/*************************************************/
// Class IndexStructure                          //
/*************************************************/
///////////////////////////////////////////////////

IndexStructure::IndexStructure(const vector<Index> &t_index) : index(t_index)
{
}

vector<Index>::const_iterator IndexStructure::find(const Index &t_index) const
{
    return std::find(begin(), end(), t_index);
}

vector<Index>::iterator IndexStructure::find(const Index &t_index)
{
    return std::find(begin(), end(), t_index);
}

IndexStructure IndexStructure::getFreeStructure() const
{
    // We create an empty structure and append only free indices to it
    IndexStructure structure;
    for (const auto &i : index) {
        if (i.getFree())
            structure += i;
    }
    return structure;
}

IndexStructure IndexStructure::getSinglePermutation(int i1, int i2) const
{
    const int size = index.size();
    if (i1 < 0 or i2 < 0 or i1 >= size or i2 >= size) {
        CALL_SMERROR_SPEC(CSLError::IndexError,
                          "IndexStructure::getSinglePermutation(int i1, int "
                          "i2) const"
                              + std::to_string((i1 < 0 or size) ? i1 : i2));
    }
    // Creates a copy of *this and permutes indices in position i1 and i2,
    // then returns the new IndexStructure
    IndexStructure rep = *this;
    swap(rep.index[i1], rep.index[i2]);

    return rep;
}

IndexStructure
IndexStructure::getPermutation(const vector<int> &permutation) const
{
    if (permutation.size() >= index.size()) {
        CALL_SMERROR_SPEC(CSLError::IndexError,
                          std::to_string(permutation.size()));
    }
    // Applies the permutation on copies of the indices of *this,
    // creates a new IndexStructure with this permutation and returns it.
    vector<Index> newIndex(0);
    for (size_t i = 0; i < index.size(); ++i)
        newIndex.push_back(index[permutation[i]]);

    return IndexStructure(newIndex);
}

IndexStructure &IndexStructure::operator+=(const Index &newIndex)
{
    Index t_new = newIndex;
    index.push_back(t_new);

    return *this;
}

std::vector<Index> const &IndexStructure::getIndex() const
{
    return index;
}

std::vector<Index> &IndexStructure::getIndex()
{
    return index;
}

const std::vector<Index> &IndexStructure::getIndexView() const
{
    return index;
}

void IndexStructure::setIndex(const Index &newIndex, int iIndex)
{
    if (iIndex < 0 or iIndex >= (int) index.size())
        CALL_SMERROR_SPEC(CSLError::IndexError,
                          "IndexStructure::setIndex(const Index&, int)"
                              + std::to_string(iIndex));
    index[iIndex] = newIndex;
}

void IndexStructure::reset()
{
    for (auto &i : index) {
        i = i.rename();
        i.setFree(true);
    }
}

IndexStructure &IndexStructure::operator+=(const IndexStructure &structure)
{
    // Using operator+=(Index) with all indices in structure.
    for (size_t i = 0; i != structure.size(); ++i)
        operator+=(structure[i]);

    return *this;
}

IndexStructure IndexStructure::operator+(const Index &index) const
{
    IndexStructure newStructure(*this);
    newStructure += index;

    return newStructure;
}

IndexStructure IndexStructure::operator+(const IndexStructure &structure) const
{
    // Using operator+(Index) with all indices in structure.
    IndexStructure newStructure(*this);
    for (size_t i = 0; i != structure.size(); ++i)
        newStructure += structure[i];

    return newStructure;
}

bool IndexStructure::hasCommonIndex(const IndexStructure &structure) const
{
    for (const auto &A : *this)
        for (const auto &B : structure)
            if (A == B)
                return true;
    return false;
}

bool IndexStructure::exactMatch(const IndexStructure &structure) const
{
    if (index.size() != structure.size())
        return false;

    for (size_t i = 0; i != index.size(); ++i)
        if (not index[i].exactMatch(structure[i])) {
            return false;
        }

    return true;
}

bool IndexStructure::compareWithDummy(const IndexStructure &structure) const
{
    map<Index, Index> constraints;
    return compareWithDummy(structure, constraints);
}

bool IndexStructure::compareWithDummy(const IndexStructure &structure,
                                      map<Index, Index>    &constraints,
                                      bool keepAllCosntraints) const
{
    // Here we search an exact match of the two structures, to a
    // renaming of dummy indices. The possible already found renamings
    // are in constraints (we must satisfy them) and we can add constraints
    // if we have the liberty to do so.
    // Ex: E_{ijk} A_j B_k == E_{iln} A_l B_n --> Comparison of E's adds
    // the constraints j=l and k=n. Then A_j==A_l and B_k==B_n: Ok.
    // Same argument: E_{ijk} A_j B_k != E_{iln} A_n B_l: constraint found
    // with E is not respected with A and B.
    if (index.size() != structure.size())
        return false;

    const vector<Index> &t_index = structure.getIndex();
    for (size_t i = 0; i != index.size(); ++i) {
        // If the index is free, they must be the same
        if ((index[i].getFree() and not Comparator::freeIndexComparisonActive)
            or index[i].getType() == cslIndex::Fixed) {
            if (not index[i].exactMatch(t_index[i])) {
                return false;
            }
        }
        // Else we check if the dummie are already constrained, if the
        // constraint is respected, or add a new constraint
        else {
            // t_index[i] must also be dummy here and must correspond (to a
            // renaming) to index[i]
            if (not index[i].compareWithDummy(t_index[i])) {
                return false;
            }
            if (constraints.find(index[i]) == constraints.end()) {
                constraints[index[i]] = t_index[i];
            }
            else {
                if (constraints[index[i]] != t_index[i]) {
                    return false;
                }
                if (not keepAllCosntraints)
                    constraints.erase(index[i]);
            }
        }
    }

    return true;
}

// Checks if two structures are equal, only regarding free-indices
// {ijj} == {i}, {ij} != {ik} for example.
bool IndexStructure::operator==(const IndexStructure &structure) const
{
    // Compare only free index Structure,
    // ignore dummy indices (just check compatibility)
    int         t_nIndices = structure.size();
    vector<int> indicesLeft(0);
    for (int i = 0; i < t_nIndices; ++i)
        // comparing only free indices
        if (structure[i].getFree())
            indicesLeft.push_back(i);

    for (size_t i = 0; i != index.size(); ++i) {
        // Comparing only free indices
        if (index[i].getFree()) {
            bool match = false;
            for (size_t j = 0; j != indicesLeft.size(); ++j) {
                // The free structure needs exact match to be correct
                if (index[i].exactMatch(structure[indicesLeft[j]])) {
                    match = true;
                    indicesLeft.erase(indicesLeft.begin() + j);
                    break;
                }
            }
            if (not match)
                return false;
        }
    }

    if (not indicesLeft.empty())
        return false;

    return true;
}

bool IndexStructure::operator!=(const IndexStructure &structure) const
{
    return (not operator==(structure));
}

bool IndexStructure::operator|=(const IndexStructure &structure) const
{
    return (not operator<(structure) and not operator>(structure));
}

bool IndexStructure::operator&=(const IndexStructure &structure) const
{
    return not operator|=(structure);
}

bool IndexStructure::operator<(const IndexStructure &structure) const
{
    const size_t sizeSelf  = index.size();
    const size_t sizeOther = structure.index.size();
    if (sizeSelf != sizeOther) {
        if (sizeSelf == 0)
            return true;
        if (sizeOther == 0)
            return false;
        return sizeSelf > sizeOther;
    }
    auto iter_self = begin();
    for (auto iter_other = structure.begin(); iter_other != structure.end();
         ++iter_self, ++iter_other)
        if (*iter_self < *iter_other)
            return true;
        else if (*iter_other < *iter_self)
            return false;

    return false;
}

bool IndexStructure::operator>(const IndexStructure &structure) const
{
    return structure < *this;
}

bool IndexStructure::operator<=(const IndexStructure &structure) const
{
    return (operator<(structure) or operator|=(structure));
}

bool IndexStructure::operator>=(const IndexStructure &structure) const
{
    return (operator>(structure) or operator|=(structure));
}

Index IndexStructure::operator[](int i) const
{
    if (i >= 0)
        return index[i];
    return index[index.size() + i];
}

Index &IndexStructure::operator[](int i)
{
    if (i >= 0)
        return index[i];
    return index[index.size() + i];
}

ostream &operator<<(ostream &fout, const IndexStructure &structure)
{
    if (structure.size() == 0)
        return fout;
    if (structure.size() > 1)
        fout << "{";
    vector<Index>::const_iterator iter;
    for (iter = structure.begin(); iter != structure.end() - 1; ++iter)
        fout << *iter << ",";
    fout << *iter;
    if (structure.size() > 1)
        fout << "}";

    return fout;
}

} // End of namespace csl
