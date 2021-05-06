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

 #include "csldatahandler.h"
#include "jsonLoader.h"
#include "marty.h"

namespace mty::doc {

csl::Expr CSLDataHandler::read(
        csl::Expr const &expr,
        bool             abbrev
        )
{
    parseTensors(expr);
    parseAbbreviations(expr);
    parseLiterals(expr);
    if (!abbrev) {
        csl::Expr copy = csl::DeepCopy(expr);
        parseIndices(copy);
        expressions.push_back(copy);
        return copy;
    }
    return expr;
}

void CSLDataHandler::readJSON(std::string const &fileName)
{
    auto root = JSON::Reader::loadFromFile(fileName);
    auto spaceList   = JSON::Parser::parseNode(root.get(), "spaces",   true);
    auto tensorList  = JSON::Parser::parseNode(root.get(), "tensors",  true);
    auto literalList = JSON::Parser::parseNode(root.get(), "literals", true);
    auto indexList   = JSON::Parser::parseNode(root.get(), "indices",  true);
    for (const auto &spaceNode : *spaceList) {
        std::string name = *JSON::Parser::parseArgument<std::string>(
                spaceNode, "name", true
                );
        int dim          = *JSON::Parser::parseArgument<int>(
                spaceNode, "dim", true
                );
        treatSpace(name, dim);
    }

    for (const auto &tensorNode : *tensorList) {
        std::string name = *JSON::Parser::parseArgument<std::string>(
                tensorNode, "name", true
                );
        int         conj = *JSON::Parser::parseArgument<int>(
                tensorNode, "conj", true
                );
        auto spaceList = JSON::Parser::parseNode(tensorNode, "spaces", true);
        std::vector<csl::Space const*> tensorSpaces;
        tensorSpaces.reserve(spaceList->size());
        for (const auto &spaceNode : *spaceList) {
            std::string name = *JSON::Parser::parseArgument<std::string>(
                    spaceNode, "name", true
                    );
            int dim = *JSON::Parser::parseArgument<int>(
                    spaceNode, "dim", true
                    );
            tensorSpaces.push_back(treatSpace(name, dim));
        }
        treatTensor(name, tensorSpaces, conj);
    }

    for (const auto &literalNode : *literalList) {
        std::string name = *JSON::Parser::parseArgument<std::string>(
                literalNode, "name", true
                );
        int         conj = *JSON::Parser::parseArgument<int>(
                literalNode, "conj", true
                );
        treatLiteral(name, conj);
    }

    for (const auto &indexNode : *indexList) {
        std::string nameSpace = *JSON::Parser::parseArgument<std::string>(
                indexNode, "space", true
                );
        int dim = *JSON::Parser::parseArgument<int>(
                indexNode, "dim", true
                );
        int N = *JSON::Parser::parseArgument<int>(
                indexNode, "number", true
                );
        csl::Space const *space = treatSpace(nameSpace, dim);
        treatIndices(space, N);
    }
}

csl::Space const *CSLDataHandler::treatSpace(
        std::string const &name,
        int                dim
        )
{
    for (const auto &s : spaces)
        if (s->getName() == name)
            return s;
    if (name == csl::Minkowski.getName()) {
        spaces.push_back(&csl::Minkowski);
        return &csl::Minkowski;
    }
    if (name == mty::dirac4.getName()) {
        spaces.push_back(&mty::dirac4);
        return &mty::dirac4;
    }

    auto newSpace = std::make_shared<csl::Space>(name, dim);
    ownSpaces.push_back(newSpace);
    spaces.push_back(newSpace.get());
    return newSpace.get();
}

csl::Tensor CSLDataHandler::treatTensor(
        std::string const &name,
        std::vector<csl::Space const*> const &tensorSpaces,
        bool isComplex
        )
{
    for (const auto &t : tensors)
        if (t->getName() == name)
            return t;

    csl::Tensor T(name, tensorSpaces);
    if (isComplex)
        T->setComplexProperty(csl::ComplexProperty::Complex);
    tensors.push_back(T);
    return T;
}

csl::Expr CSLDataHandler::treatLiteral(
        std::string const &name,
        bool isComplex
        )
{
    for (const auto &l : literal)
        if (l->getName() == name)
            return l;

    csl::Expr l = csl::constant_s(name);
    if (isComplex)
        l->setComplexProperty(csl::ComplexProperty::Complex);
    literal.push_back(l);
    return l;
}

void CSLDataHandler::treatIndices(
        csl::Space const *space,
        size_t            N
        )
{
    if (nIndices.find(space) == nIndices.end() or N > nIndices[space])
        nIndices[space] = N;
}

void CSLDataHandler::parseTensors(csl::Expr const &expr)
{
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub) and !csl::Abbrev::find_opt(sub)) {
            csl::Tensor tensor = std::dynamic_pointer_cast<csl::TensorParent>(
                    sub->getParent()
                    );
            HEPAssert(
                    tensor,
                    mty::error::TypeError,
                    "Somehow " + toString(sub) + " does not give a csl::Tensor."
                    )
            if (csl::IsTensorField(sub) or csl::IsTensorialDerivative(sub)) {
                csl::Tensor point = sub->getPoint();
                const auto pos = std::find(
                        tensors.begin(), tensors.end(), point
                        );
                if (pos == tensors.end())
                    tensors.push_back(point);
            }
            const auto pos = std::find(tensors.begin(), tensors.end(), tensor);
            if (pos != tensors.end())
                return;
            tensors.push_back(tensor);
            std::vector<csl::Space const*> tensorSpaces = tensor->getSpace();
            for (csl::Space const *space : tensorSpaces) {
                const auto pos = std::find(spaces.begin(), spaces.end(), space);
                if (pos == spaces.end())
                    spaces.push_back(space);
            }
        }
    });
}

void CSLDataHandler::parseLiterals(csl::Expr const &expr)
{
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
    {
        if (csl::IsLiteral(sub) and !csl::Abbrev::find_opt(sub)) {
            if (sub == CSL_I and sub != CSL_PI and sub != CSL_E)
                return;
            const auto pos = std::find_if(
                    literal.begin(), 
                    literal.end(), 
                    [&](csl::Expr const &other) {
                        return other->getParent_info() == sub->getParent_info();
                    });
            if (pos == literal.end())
                literal.push_back(sub);
        }
    });
}

void CSLDataHandler::parseAbbreviations(csl::Expr const &expr)
{
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
    {
        if (csl::Size(sub) == 0 and csl::Abbrev::find_opt(sub)) {
            csl::AbstractParent *abbrev = csl::Abbrev::find(sub);
            const auto pos = std::find_if(
                    abbreviations.begin(),
                    abbreviations.end(),
                    [&](csl::Parent const &parent) {
                        return parent.get() == abbrev;
                    });
            if (pos == abbreviations.end()) {
                // Do not swap the two following lines: abbrev depends on its
                // encapsulated expr, so the latter must appear first in the 
                // data
                read(abbrev->getEncapsulated(), true);
                abbreviations.push_back(abbrev->shared_from_this());
            }

        }
    });
}

void CSLDataHandler::parseIndices(csl::Expr &expr)
{
    std::map<csl::Space const*, std::vector<csl::Index>> indices;
    auto insertIndex = [&](csl::Index &i)
    {
        auto pos = indices.find(i.getSpace());
        if (pos == indices.end()) {
            indices[i.getSpace()] = {i};
            i.setID(0);
            return;
        }
        auto posI = std::find(pos->second.begin(),
                              pos->second.end(),
                              i);
        if (posI == pos->second.end()) {
            pos->second.push_back(i);
            i.setID(pos->second.size() - 1);
            return;
        }
        if (i.getName() != posI->getName())
            i.setName(std::string(posI->getName()));
        i.setID(std::distance(pos->second.begin(), posI));
    };
    auto updateIndices = [&]() 
    {
        for (const auto &[space, index] : indices) {
            if (nIndices.find(space) == nIndices.end())
                nIndices[space] = index.size();
            else
                nIndices[space] = std::max(nIndices[space], index.size());
        }
    };
    csl::ForEachNode(expr, [&](csl::Expr &sub) {
        if (csl::IsIndicialTensor(sub)) {
            for (auto &index : sub->getIndexStructureView()) {
                insertIndex(index);
            }
        }
    });
    updateIndices();
}

void CSLDataHandler::print(std::ostream &out) const
{
    out << "CSL data handler: \n";
    out << "Spaces : \n";
    for (const auto &s : spaces)
        out << "  - " << s->getName() << ", dimension " << s->getDim() << '\n';
    out << "Literals: \n";
    for (const auto &l : literal)
        out << "  - " << l << '\n';
    out << "Tensors: \n";
    for (const auto &t : tensors) {
        out << "  - " << t->getName();
        out << ", dimension " << t->getSpace().size() << '\n';
    }
    out << "Abbreviation: \n";
    for (const auto &ab : abbreviations) {
        const auto abbrev = csl::Abbrev::find(ab->getName());
        out << "  - " << abbrev->getName() << " = ";
        out << abbrev->getEncapsulated() << '\n';
    }
    out << "Expressions: \n";
    for (const auto &e : expressions)
        out << "  - " << e << '\n';
    out.flush();
}

std::unique_ptr<JSON::Node> CSLDataHandler::getSpaceList(
        std::vector<csl::Space const*> const &spaces
        ) const
{
    auto listSpaces = JSON::List::make("spaces");
    for (const auto &s : spaces) {
        auto spaceNode = JSON::Node::make("spaces");
        auto nameLeaf = JSON::Leaf<std::string>::make("name", s->getName());
        auto dimLeaf  = JSON::Leaf<int>::make("dim", s->getDim());
        spaceNode->addChild(nameLeaf);
        spaceNode->addChild(dimLeaf);
        listSpaces->addChild(spaceNode);
    }

    return listSpaces;
}

void CSLDataHandler::printJSON(std::string const &nameFile) const
{
    JSON::Node tree("root");

    auto spaceList = getSpaceList(spaces);
    tree.addChild(spaceList);

    auto listTensors = JSON::List::make("tensors");
    for (const auto &t : tensors) {
        std::cout << t->getName() << std::endl;
        auto tensorNode = JSON::Node::make("tensors");
        auto nameLeaf = JSON::Leaf<std::string>::make("name", t->getName());
        auto conjLeaf = JSON::Leaf<int>::make(
                "conj", 
                t->getComplexProp() == csl::ComplexProperty::Complex
                );
        auto spaceList = getSpaceList(t->getSpace());
        tensorNode->addChild(nameLeaf);
        tensorNode->addChild(conjLeaf);
        tensorNode->addChild(spaceList);
        listTensors->addChild(tensorNode);
    }
    tree.addChild(listTensors);

    auto listLiterals = JSON::List::make("literals");
    for (const auto &l : literal) {
        std::cout << l << std::endl;
        auto literalNode = JSON::Node::make("literals");
        auto nameLeaf = JSON::Leaf<std::string>::make("name", l->getName());
        auto conjLeaf = JSON::Leaf<int>::make(
                "conj", 
                l->getComplexProperty() == csl::ComplexProperty::Complex
                );
        literalNode->addChild(nameLeaf);
        literalNode->addChild(conjLeaf);
        listLiterals->addChild(literalNode);
    }
    tree.addChild(listLiterals);

    auto listIndices = JSON::List::make("indices");
    for (const auto &[space, N] : nIndices) {
        std::cout << space->getName() << std::endl;
        auto indexNode = JSON::Node::make("indices");
        auto spaceLeaf = JSON::Leaf<std::string>::make("space", space->getName());
        auto dimLeaf = JSON::Leaf<int>::make("dim", space->getDim());
        auto numberLeaf = JSON::Leaf<int>::make("number", N);
        indexNode->addChild(spaceLeaf);
        indexNode->addChild(dimLeaf);
        indexNode->addChild(numberLeaf);
        listIndices->addChild(indexNode);
    }
    tree.addChild(listIndices);

    JSON::Reader::saveToFile(nameFile, &tree);
}

void CSLDataHandler::printComBlock(
        std::string_view com,
        std::ostream    &out,
        std::string_view indent
        ) const
{
    static constexpr auto comLine = "/////////////////////////////////////////";
    out << '\n';
    out << indent << comLine << '\n';
    out << indent << "// " << com << "\n";
    out << indent << comLine << '\n';
    out << '\n';
}

void CSLDataHandler::printCSLDefinition(
        csl::Tensor   tensor,
        std::ostream &out,
        size_t        indentSize
        ) const
{
    if (auto der = dynamic_cast<csl::TDerivativeParent const*>(tensor.get());
            der) {
        der->csl::TDerivativeParent::printDefinition(out, indentSize, true);
    }
    else if (auto field = dynamic_cast<csl::TensorFieldParent const*>(tensor.get());
            field) {
        field->csl::TensorFieldParent::printDefinition(out, indentSize, true);
    }
    else {
        tensor->csl::TensorParent::printDefinition(out, indentSize, true);
    }
}

void CSLDataHandler::printCode(std::ostream &out) const
{
    auto indentSize = 4;
    auto indent = std::string(indentSize, ' ');

    printComBlock("Spaces", out, indent);
    for (const auto &s : spaces) {
        s->printDefinition(out, indentSize, true);
    }
    printComBlock("Tensors", out, indent);
    for (const auto &t : tensors) {
        printCSLDefinition(t, out, indentSize);
    }
    printComBlock("Literals", out, indent);
    for (const auto &l : literal) {
        l->getParent()->printDefinition(out, indentSize, true);
    }
    printComBlock("Abbreviations", out, indent);
    for (const auto &ab : abbreviations) {
        ab->printDefinition(out, indentSize, true);
    }
    printComBlock("Indices", out, indent);
    for (const auto &[space, nIndex] : nIndices) {
        out << indent << "inline std::vector<csl::Index> i_" 
            << csl::Abstract::regularName(space->getName());
        out << " = ";
        space->printCode(out, 0);
        out << "->generateIndices(" << nIndex << ");\n";
    }
    printComBlock("Expressions", out, indent);
    size_t i = 0;
    for (const auto &e : expressions) {
        out << indent << "inline csl::Expr expr_" << i++ << " = ";
        e->printCode(1, out);
        out << ";\n";
    }
    out.flush();
}

}
