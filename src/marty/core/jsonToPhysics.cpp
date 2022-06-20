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

#include "jsonToPhysics.h"
#include "diracology.h"
#include "flavor.h"
#include "gauge.h"
#include "group.h"
#include "model.h"
#include "mrtError.h"
#include "quantumField.h"

using namespace std;
using namespace csl;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class JSONToHEP                               //
/*************************************************/
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Public members
///////////////////////////////////////////////////

unique_ptr<Model> JSONToHEP::readModel(std::string const &nameFile)
{
    unique_ptr<JSON::Node> jsonTree = JSON::Reader::loadFromFile(nameFile);
    HEPAssert(not jsonTree->empty(),
              mty::error::TypeError,
              "Empty file for loading model: \"" + nameFile + "\".");

    std::unique_ptr<Gauge> gauge     = readGauge(jsonTree.get());
    Gauge *                ptr_gauge = gauge.get();

    std::unique_ptr<Flavor> flavor     = readFlavor(jsonTree.get());
    Flavor *                ptr_flavor = flavor.get();

    vector<QuantumNumber> numbers = readQuantumNumbers(jsonTree.get());

    unique_ptr<Model> model
        = make_unique<Model>(std::move(gauge), std::move(flavor));
    model->setQuantumNumbers(numbers);
    vector<shared_ptr<QuantumFieldParent>> fields = readField(
        jsonTree.get(), ptr_gauge, ptr_flavor, model.get(), numbers);
    for (auto &particle : fields)
        model->addParticle(particle);

    vector<pair<string, JSON::Node *>> yukawas
        = readYukawaNodes(JSON::Parser::parseNode(jsonTree.get(), "field"));
    for (const auto &[nameScalar, node] : yukawas) {
        csl::Expr interaction
            = interpretYukawaCoupling(model.get(), nameScalar, node);
        if (interaction != CSL_0)
            model->addLagrangianTerm(interaction);
    }

    return model;
}

///////////////////////////////////////////////////
// Private members
///////////////////////////////////////////////////

unique_ptr<Gauge> JSONToHEP::readGauge(JSON::Node *tree)
{
    JSON::Node *gaugeNode = JSON::Parser::parseNode(tree, "gauge");
    if (not gaugeNode)
        return nullptr;
    string nameGauge
        = JSON::Parser::parseArgument<string>(gaugeNode, "name", true).value();

    unique_ptr<Gauge> gauge = make_unique<Gauge>(nameGauge);
    // Taking only the list of groups in the gauge Node
    gaugeNode = JSON::Parser::parseNode(gaugeNode, "group", true);
    for (const auto &groupNode : *gaugeNode) {
        string name
            = JSON::Parser::parseArgument<string>(groupNode, "name", true)
                  .value();

        group::Type type = stringToGroupType(
            JSON::Parser::parseArgument<string>(groupNode, "type", true)
                .value());

        std::optional<int> optDim
            = JSON::Parser::parseArgument<int>(groupNode, "dim");

        JSON::Node *couplingNode
            = JSON::Parser::parseNode(groupNode, "coupling");

        JSON::Node *bosonNode = JSON::Parser::parseNode(groupNode, "boson");

        std::optional<std::string> nameBoson_opt
            = JSON::Parser::parseArgument<std::string>(bosonNode, "name");
        std::optional<bool> addGhost_opt
            = JSON::Parser::parseArgument<bool>(bosonNode, "ghost");

        std::string                 nameBoson = nameBoson_opt.value_or("");
        bool                        addGhost  = addGhost_opt.value_or(true);
        int                         dim       = (optDim) ? optDim.value() : -1;
        unique_ptr<SemiSimpleGroup> g         = createGroup(type, name, dim);
        if (couplingNode)
            gauge->addGroup(
                g, nameBoson, addGhost, readGaugeCoupling(couplingNode));
        else
            gauge->addGroup(g, nameBoson, addGhost);
    }

    return gauge;
}

unique_ptr<Flavor> JSONToHEP::readFlavor(JSON::Node *tree)
{
    JSON::Node *flavorNode = JSON::Parser::parseNode(tree, "flavor");
    if (not flavorNode)
        return nullptr;

    unique_ptr<Flavor> flavor = make_unique<Flavor>();
    // Taking only the list of groups in the gauge Node
    for (const auto &flav : *flavorNode) {
        string name
            = JSON::Parser::parseArgument<string>(flav, "name", true).value();

        int nFlavors
            = JSON::Parser::parseArgument<int>(flav, "nF", true).value();

        std::optional<bool> optComplex
            = JSON::Parser::parseArgument<bool>(flav, "complex");

        bool                    isComplex = optComplex.value_or(true);
        unique_ptr<FlavorGroup> f
            = make_unique<FlavorGroup>(name, nFlavors, isComplex);
        flavor->push_back(std::move(f));
    }

    return flavor;
}

vector<QuantumNumber> JSONToHEP::readQuantumNumbers(JSON::Node *tree)
{
    JSON::Node *qnNode = JSON::Parser::parseNode(tree, "quantum numbers");
    vector<QuantumNumber> numbers;
    if (not qnNode)
        return numbers;

    for (const auto &num : *qnNode) {
        string name = JSON::Parser::interpretObject<string>(num.get());
        auto   pos  = find_if(
            numbers.begin(), numbers.end(), [&](QuantumNumber const &n) {
                return n.getName() == name;
            });
        if (pos == numbers.end()) {
            numbers.push_back(QuantumNumber(name));
        }
    }
    return numbers;
}

csl::Expr JSONToHEP::readGaugeCoupling(JSON::Node *couplingNode)
{
    string name
        = JSON::Parser::parseArgument<string>(couplingNode, "name", true)
              .value();
    optional<double> value
        = JSON::Parser::parseArgument<double>(couplingNode, "value");
    if (value)
        return constant_s(name, value.value());
    return constant_s(name);
}

vector<shared_ptr<QuantumFieldParent>>
JSONToHEP::readField(JSON::Node *tree,
                     Gauge *     gauge,
                     Flavor *    flavor,
                     Model *     model,
                     std::vector<QuantumNumber> const &)
{
    JSON::Node *fieldNode = JSON::Parser::parseNode(tree, "field");
    vector<shared_ptr<QuantumFieldParent>> res;
    if (not fieldNode)
        return res;
    res.reserve(fieldNode->size());

    for (const auto &node : *fieldNode) {

        string name
            = JSON::Parser::parseArgument<string>(node, "name", true).value();
        int spin
            = JSON::Parser::parseArgument<int>(node, "spin", true).value();
        optional<string> chirality
            = JSON::Parser::parseArgument<string>(node, "chirality");
        JSON::Node *massNode = JSON::Parser::parseNode(node, "mass");

        JSON::Node *irrepNode = JSON::Parser::parseNode(node, "irrep");
        GaugeIrrep  irrep     = readIrrep(irrepNode, gauge);
        irrepNode             = JSON::Parser::parseNode(node, "flavor");
        FlavorIrrep flavorRep = readFlavorIrrep(irrepNode, flavor);
        Particle    newField;
        if (spin == 0)
            newField = scalarboson_s(name, irrep, flavorRep);
        else if (spin == 1 and chirality) {
            Chirality chir = (chirality.value() == "R") ? Chirality::Right
                                                        : Chirality::Left;
            newField = weylfermion_s(name, irrep, flavorRep, chir);
        }
        else if (spin == 1)
            newField = diracfermion_s(name, irrep, flavorRep);
        else if (spin == 2)
            newField = vectorboson_s(name, irrep, flavorRep);
        else
            CallHEPError(mty::error::ValueError,
                         "Spin " + toString(spin)
                             + " not taken into account in" + " json reader.");
        if (massNode)
            readMass(massNode, newField.get());
        JSON::Node *qNumNode
            = JSON::Parser::parseNode(node, "quantum numbers");
        if (qNumNode)
            for (const auto &n : *qNumNode) {
                std::string          name = n->getSpecifier();
                QuantumNumber::Value value
                    = JSON::Parser::interpretObject<int>(n.get());
                QuantumNumber &num = model->getQuantumNumber(name);
                newField->addQuantumNumber(num, value);
            }
        res.push_back(newField);
    }

    return res;
}

vector<pair<string, JSON::Node *>>
JSONToHEP::readYukawaNodes(JSON::Node *fields)
{
    vector<pair<string, JSON::Node *>> res;
    if (not fields)
        return res;
    for (const auto &node : *fields) {
        JSON::Node *yukawa = JSON::Parser::parseNode(node, "yukawa");
        if (yukawa) {
            string nameScalar
                = JSON::Parser::parseArgument<string>(node, "name", true)
                      .value();
            for (const auto &coupling : *yukawa)
                res.push_back(
                    make_pair(nameScalar, JSON::Parser::convert(coupling)));
        }
    }
    return res;
}

csl::Expr JSONToHEP::interpretYukawaCoupling(Model *       model,
                                             string const &nameScalar,
                                             JSON::Node *  node)
{
    string flavorName
        = JSON::Parser::parseArgument<string>(node, "flavor", true).value();
    string couplingName
        = JSON::Parser::parseArgument<string>(node, "coupling", true).value();
    JSON::Node *fieldList = JSON::Parser::parseNode(node, "fields");
    HEPAssert(fieldList->size() == 2,
              mty::error::TypeError,
              "Expecting two fields in yukawa coupling, "
                  + toString(fieldList->size()) + " given.");
    vector<string> fieldNames;
    for (const auto &f : *fieldList)
        fieldNames.push_back(JSON::Parser::interpretObject<string>(f.get()));

    Particle     scalar      = model->getParticle(nameScalar);
    Particle     field1      = model->getParticle(fieldNames[0]);
    Particle     field2      = model->getParticle(fieldNames[1]);
    Gauge *      gauge       = model->getGauge();
    GaugeIrrep   scalarRep   = scalar->getGaugeIrrep();
    GaugeIrrep   field1Rep   = field1->getGaugeIrrep();
    GaugeIrrep   field2Rep   = field2->getGaugeIrrep();
    FlavorGroup *flavorGroup = model->getFlavorGroup(flavorName);
    const Space *flavorSpace
        = flavorGroup->getVectorSpace(field1->getFlavorIrrep(flavorGroup));

    csl::Tensor Yukawa = tensor_s(couplingName, {flavorSpace, flavorSpace});
    Yukawa->setComplexProperty(csl::ComplexProperty::Complex);
    model->addTensorCoupling(Yukawa);
    vector<csl::Index> scalarIndices = scalar->getFullSetOfIndices();
    vector<csl::Index> field1Indices = field1->getFullSetOfIndices();
    vector<csl::Index> field2Indices = field2->getFullSetOfIndices();

    HEPAssert(scalarIndices.size() == 1,
              mty::error::TypeError,
              "Wrong number of indices for scalar in yukawa. Should have"
                  + static_cast<string>(" 1, ")
                  + toString(scalarIndices.size()) + " given.");

    HEPAssert(field1Indices.size() - field2Indices.size() == 1,
              mty::error::TypeError,
              "Wrong number of indices for fields in yukawa.");

    Index flavorIndex1;
    Index flavorIndex2;
    Index gaugeIndex;
    bool  offset = false;
    for (size_t i = 0; i != field1Indices.size(); ++i) {
        if (field1Indices[i].getSpace()
            != field2Indices[i - offset].getSpace()) {
            HEPAssert(not offset,
                      mty::error::TypeError,
                      "Indices do not match for fields in Yukawa");
            offset     = true;
            gaugeIndex = field1Indices[i];
        }
        else {
            if (offset)
                field2Indices[i - 1] = field1Indices[i].getFlipped();
            else {
                if (field1Indices[i].getSpace() == flavorSpace) {
                    flavorIndex1 = field1Indices[i];
                    flavorIndex2 = field2Indices[i];
                }
                else
                    field2Indices[i] = field1Indices[i].getFlipped();
            }
        }
    }

    csl::Expr     expression;
    Tensor        X           = model->getLagrangian().getPoint();
    Index         gaugeIndex2 = gaugeIndex.rename();
    SumGaugeIrrep prod = scalarRep * field1Rep.getConjugatedRep() * field2Rep;
    if (prod.find(gauge->getTrivialRep()) == -1) {
        prod = scalarRep.getConjugatedRep() * field1Rep.getConjugatedRep()
               * field2Rep;
        HEPAssert(prod.find(gauge->getTrivialRep()) >= 0,
                  mty::error::TypeError,
                  "Yukawa coupling does not contain trivial representation!");
        expression
            = Yukawa({flavorIndex1, flavorIndex2})
              * gaugeIndex.getSpace()->getEpsilon()({gaugeIndex, gaugeIndex2})
              * GetComplexConjugate(scalar(gaugeIndex2, X))
              * GetComplexConjugate(field1(field1Indices, X))
              * field2(field2Indices, X);
    }
    else
        expression = Yukawa({flavorIndex1, flavorIndex2})
                     * scalar(gaugeIndex, X)
                     * GetComplexConjugate(field1(field1Indices, X))
                     * field2(field2Indices, X);

    return -expression - GetHermitianConjugate(expression, &dirac4);
}

vector<int> JSONToHEP::stringToDinkinLabels(std::string const &str)
{
    vector<string> strLabels(1, "");
    int            pos = 0;
    for (const auto &c : str)
        if (c == ' ' or c == '\n') {
            if (not strLabels[pos].empty()) {
                ++pos;
                strLabels.push_back("");
            }
        }
        else
            strLabels[pos] += c;
    if (strLabels[pos].empty())
        strLabels.erase(strLabels.begin() + pos);

    vector<int> labels;
    for (const auto &s : strLabels) {
        istringstream sin(s);
        int           l;
        sin >> l;
        labels.push_back(l);
    }
    return labels;
}

GaugeIrrep JSONToHEP::readIrrep(JSON::Node *node, Gauge *gauge)
{
    if (not node or not gauge)
        return GaugeIrrep();

    vector<string>      groupName;
    vector<vector<int>> dinkinLabels;
    for (size_t i = 0; i != gauge->size(); ++i)
        groupName.push_back((*gauge)[i]->getName());

    for (size_t i = 0; i != groupName.size(); ++i) {
        string           name = groupName[i];
        optional<string> irrepSpec
            = JSON::Parser::parseArgument<string>(node, name);
        if (irrepSpec) {
            vector<int> labels = stringToDinkinLabels(irrepSpec.value());
            // If we treat the U1 group and only the numerator is given,
            // we add the denominator 1 to the fraction before creating the
            // rep.
            if ((*gauge)[i]->getType() == group::Type::U1
                and labels.size() == 1)
                labels = vector<int>{labels[0], 1};
            dinkinLabels.push_back(labels);
        }
        else
            dinkinLabels.push_back(vector<int>());
    }

    return gauge->getRepresentation(dinkinLabels);
}

FlavorIrrep JSONToHEP::readFlavorIrrep(JSON::Node *node, Flavor *flavor)
{
    if (not node or not flavor)
        return FlavorIrrep(flavor);

    vector<string_view> flavorName;
    vector<vector<int>> dinkinLabels;
    for (size_t i = 0; i != flavor->size(); ++i)
        flavorName.push_back((*flavor)[i]->getName());

    FlavorIrrep flavorRep(flavor);
    for (const auto &flavorNode : *node) {
        string fName = JSON::Parser::interpretObject<string>(flavorNode.get());
        auto   pos   = find(flavorName.begin(), flavorName.end(), fName);
        if (pos != flavorName.end()) {
            flavorRep.setFundamentalRepresentation(
                (*flavor)[distance(flavorName.begin(), pos)]);
        }
    }

    return flavorRep;
}

void JSONToHEP::readMass(JSON::Node *massNode, QuantumFieldParent *field)
{
    string name
        = JSON::Parser::parseArgument<string>(massNode, "name", true).value();
    optional<double> value
        = JSON::Parser::parseArgument<double>(massNode, "value");
    if (value)
        field->setMass(name, value.value());
    else
        field->setMass(name);
}

} // End of namespace mty
