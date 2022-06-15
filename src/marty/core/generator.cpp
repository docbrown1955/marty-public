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

#include "generator.h"
#include "gaugedGroup.h"

namespace mty {

void GeneratorParent::printDefinition(std::ostream &out,
                                      int           indentSize,
                                      bool          header) const
{
    std::string regName = getCodeName();
    std::string indent(indentSize, ' ');
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Generator " << regName << " = ";
    out << csl::Abstract::regularName(group->getGroup()->getName())
        << "->getGenerator({";
    auto highest = irrep.getHighestWeight();
    for (size_t i = 0; i != highest.size(); ++i) {
        out << highest[i];
        if (i != highest.size() - 1)
            out << ", ";
    }
    out << "});\n";
}

std::string GeneratorParent::getCodeName() const
{
    std::string regName = csl::Abstract::regularName(
        name + "_" + group->getGroup()->getName());
    auto highest = irrep.getHighestWeight();
    for (int label : highest)
        regName += "_" + toString(label);

    return regName;
}

csl::Expr GeneratorParent::operator()(csl::Index index)
{
    // Operator() for GeneratorParent that generates an GeneratorElement of
    // index index. Like X(mu) for an GeneratorParent X returns an
    // GeneratorElement.
    createFixedIndices(index);
    checkIndexRequest(index);

    csl::Index copyIndex = index;
    if (keepBestPermutation)
        return generatorelement_s(copyIndex, self());
    return csl::make_shared<GeneratorElement>(copyIndex, self());
}

csl::Expr GeneratorParent::operator()(std::vector<csl::Index> indices)
{
    // Operator() for GeneratorParent that generates an GeneratorElement of
    // indices indices. Like g({mu,nu}) for an GeneratorParent g returns an
    // GeneratorElement of rank 2.
    createFixedIndices(indices);
    checkIndexRequest(indices);
    std::vector<csl::Index> copyIndices(0);
    for (const auto &index : indices)
        copyIndices.push_back(index);

    if (keepBestPermutation)
        return generatorelement_s(copyIndices, self());
    return csl::make_shared<GeneratorElement>(copyIndices, self());
}

csl::Expr GeneratorParent::operator()(const std::vector<int> &indices)
{
    return (*this)(csl::integerToIndices(indices));
}

csl::unique_Expr GeneratorElement::copy_unique() const
{
    csl::unique_Expr res = std::make_unique<GeneratorElement>(index, parent);
    applyComplexPropertiesOn(res);

    return res;
}

csl::Expr GeneratorElement::refresh() const
{
    csl::Expr res = generatorelement_s(index, parent);
    if (csl::IsIndicialTensor(res)) {
        applyComplexPropertiesOn(res);
    }
    else if (res->getType() == csl::Type::Prod and res->size() == 2
             and csl::IsIndicialTensor((*res)[1])) {
        applyComplexPropertiesOn((*res)[1]);
    }
    return res;
}

void GeneratorElement::printCode(int mode, std::ostream &out) const
{
    if (index.empty()) {
        if (conjugated)
            out << "csl::GetComplexConjugate(;";
        out << std::dynamic_pointer_cast<GeneratorParent>(parent)
                   ->getCodeName()
            << "()";
        if (conjugated)
            out << ")";
        return;
    }
    if (conjugated)
        out << "csl::GetComplexConjugate(;";
    out << std::dynamic_pointer_cast<GeneratorParent>(parent)->getCodeName()
        << "({";
    for (size_t i = 0; i != index.size(); ++i) {
        if (index[i].getSign())
            out << '+';
        out << index[i].getIndexCodeName();
        if (i + 1 != index.size())
            out << ", ";
    }
    out << "})";
    if (conjugated)
        out << ")";
    if (mode == 0)
        out << '\n';
}

} // End of namespace mty
