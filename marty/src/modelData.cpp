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

#include "modelData.h"
#include "jsonToPhysics.h"
#include "diracology.h"
#include "model.h"
#include "quantumFieldTheory.h"

namespace mty {

///////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////

ModelData::ModelData()
    :spaceTime(&csl::Minkowski),
    gauge(std::make_unique<mty::Gauge>()),
    flavor(nullptr)
{
}
ModelData::ModelData(std::unique_ptr<Gauge> &&t_gauge)
    :spaceTime(defaultSpaceTime),
    gauge(std::move(t_gauge)),
    flavor(nullptr)
{
    init();
}
ModelData::ModelData(
        std::unique_ptr<Gauge>  &&t_gauge,
        std::unique_ptr<Flavor> &&t_flavor
        )
    :spaceTime(defaultSpaceTime),
    gauge(std::move(t_gauge)),
    flavor(std::move(t_flavor))
{
    init();
}
ModelData::ModelData(
        std::unique_ptr<Gauge>  &&t_gauge,
        std::unique_ptr<Flavor> &&t_flavor,
        std::vector<Particle>    &t_particles)
    :ModelData(std::move(t_gauge), std::move(t_flavor))
{
    for (const auto& part : t_particles)
        addParticle(part);
}
ModelData::ModelData(std::string const& nameFile)
{
    std::unique_ptr<Model> model = JSONToHEP::readModel(nameFile);
    *this = std::move(*model);
}

ModelData::~ModelData()
{

}

///////////////////////////////////////////////////
// Check functions
///////////////////////////////////////////////////

csl::Expr hc(csl::Expr const &init)
{
    return csl::Refreshed(csl::GetHermitianConjugate(
                init, &mty::dirac4
                ));
}

void ModelData::checkHermiticity()
{
    bool hermitianLagrangian = true;
    std::vector<csl::Expr> terms;
    terms.reserve(L.interaction.size());
    std::vector<csl::Expr> hermitic;
    hermitic.reserve(L.interaction.size());
    for (size_t i = 0; i != L.interaction.size(); ++i) {
        csl::Expr expr = csl::Evaluated(
                L.interaction[i]->getTerm(),
                csl::eval::abbreviation);
        if (not csl::IsSum(expr)) {
            terms.push_back(expr);
            hermitic.push_back(hc(expr));
        }
        else {
            for (const auto &term : expr) {
                terms.push_back(term);
                hermitic.push_back(hc(term));
            }
        }
    }
    std::vector<size_t> indicesLeft(terms.size());
    std::iota(indicesLeft.begin(), indicesLeft.end(), 0);
    std::vector<csl::Expr> termsNotFound;
    for (size_t i = 0; i != terms.size(); ++i) {
        bool found = false;
        for (size_t j = 0; j < indicesLeft.size(); ++j) {
            if (csl::hardComparison(terms[i], hermitic[indicesLeft[j]])) {
                indicesLeft.erase(indicesLeft.begin() + j);
                found = true;
                break;
            }
        }
        if (not found) {
            hermitianLagrangian = false;
            termsNotFound.push_back(terms[i]);
        }
    }
    if (hermitianLagrangian) {
        std::cout << "The interaction Lagrangian is hermitian !" << std::endl;
    }
    else {
        std::cout << "Lagrangian is not hermitic !" << std::endl;
        std::cout << "The following terms have no (obvious) counter-part" 
            << std::endl;
        for (const auto &term : termsNotFound) {
            std::cout << term << std::endl;
            std::cout << "Hermitian conjugate is: " << std::endl;
            std::cout << hc(term) << std::endl;
        }
    }
}

///////////////////////////////////////////////////
// Write function
///////////////////////////////////////////////////

#define OUT out << indent
#define OUT2 out << indent << "    "
#define OUT3 out << indent << "        "
#define END std::endl
#define PRINT(arg) OUT << arg << END

void ModelData::saveModel(
        std::ostream    &out,
        int              indentSize
        )
{
    std::string indent(indentSize, ' ');
    std::vector<csl::Parent> defined;
    PRINT("std::unique_ptr<mty::Model> model "
          "= std::make_unique<mty::Model>();");
    PRINT("mty::Model &_model_ = *model;");
    writeHeader(out, indentSize, defined);
    writeGauge(out, indentSize, defined);
    writeFlavor(out, indentSize, defined);
    OUT << "model->init(false);" << END;
    writeMassAndWidth(out, indentSize, defined);
    writeParticles(out, indentSize, defined);
    std::vector<csl::Expr> kinetic;
    kinetic.reserve(L.kinetic.size());
    std::vector<csl::Expr> mass;
    mass.reserve(L.mass.size());
    std::vector<csl::Expr> interac;
    interac.reserve(L.interaction.size());
    for (const auto &k : L.kinetic)
        kinetic.push_back(k->getFullExpression());
    for (const auto &m : L.mass)
        mass.push_back(m->getFullExpression());
    for (const auto &i : L.interaction)
        interac.push_back(i->getFullExpression());
    writeDependencies(out, indentSize, defined, kinetic, mass, interac);
    writeLagrangian(out, indentSize, kinetic, mass, interac);
}
void ModelData::writeHeader(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &defined
        )
{
    std::string indent(indentSize, ' ');
    OUT << "csl::Tensor gamma  = mty::dirac4.gamma;" << END;
    OUT << "csl::Tensor gamma5 = mty::dirac4.gamma_chir;" << END;
    OUT << "csl::Tensor P_L    = mty::dirac4.P_L;" << END;
    OUT << "csl::Tensor P_R    = mty::dirac4.P_R;" << END;
    OUT << "csl::Tensor sigma  = mty::dirac4.sigma;" << END;
    defined.push_back(mty::dirac4.gamma);
    defined.push_back(mty::dirac4.gamma_chir);
    defined.push_back(mty::dirac4.P_L);
    defined.push_back(mty::dirac4.P_R);
    defined.push_back(mty::dirac4.sigma);
}

void ModelData::writeGauge(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &defined
        )
{
    std::string indent(indentSize, ' ');
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Gauge definition              //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (size_t i = 0; i != gauge->size(); ++i) {
        if (gauge->getGaugedGroup(i)->isBroken())
            continue;
        auto group = (*gauge)[i];
        csl::Expr coupling = group->getGaugedGroup()->getCouplingConstant();
        if (csl::IsVariable(coupling) or csl::IsConstant(coupling)) {
            csl::Parent parent = coupling->getParent();
            parent->printDefinition(out, indentSize);
            defined.push_back(parent);
        }
    }
    for (size_t i = 0; i != gauge->size(); ++i) {
        if (gauge->getGaugedGroup(i)->isBroken())
            continue;
        auto group = (*gauge)[i];
        OUT << "model->addGaugedGroup(" << END;
        OUT2 << "mty::group::Type::" << group->getType() << "," << END;
        OUT2 << "\"" << group->getName() << "\"," << END;
        OUT2 << group->getDim() << "," << END;
        OUT2;
        group->getGaugedGroup()->getCouplingConstant()->printCode(1, out);
        out << END;
        OUT2 << ");" << END;
    }
    for (size_t i = 0; i != gauge->size(); ++i) {
        if (gauge->getGaugedGroup(i)->isBroken())
            continue;
        OUT << "mty::GaugedGroup *" 
            << csl::Abstract::regularName((*gauge)[i]->getName())
            << " = model->getGauge()->getGaugedGroup(" << i << ");" << END;
    }
    OUT << END;
}

void ModelData::writeFlavor(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &
        )
{
    if (not flavor)
        return;
    std::string indent(indentSize, ' ');
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Flavor definition             //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &group : *flavor) {
        OUT << "model->addFlavorGroup(" << END;
        OUT2 << "\"" << csl::Abstract::regularLiteral(group->getName()) 
            << "\", " << group->getDim() << ", " << group->isComplex() << ");"
            << END;
    }
    for (size_t i = 0; i != flavor->size(); ++i) {
        OUT << "mty::FlavorGroup *" 
            << csl::Abstract::regularName((*flavor)[i]->getName())
            << " = (*model->getFlavor())[" << i << "];" << END;
    }
    OUT << END;
}

void ModelData::writeMassAndWidth(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &defined
        )
{
    std::string indent(indentSize, ' ');
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Particle masses definitions   //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &part : particles) {
        csl::Expr mass = part->getMass();
        std::vector<csl::Parent> dep = mass->getSubSymbols();
        if (dep.empty())
            continue;
        for (auto iter = dep.rbegin()+1; iter != dep.rend(); ++iter) {
            auto pos = std::find(defined.begin(), defined.end(), *iter);
            if (pos == defined.end()) {
                (**iter).printDefinition(out, indentSize);
                defined.push_back(*iter);
            }
        }
        auto pos = std::find(defined.begin(), defined.end(), dep.back());
        if (pos == defined.end()) {
            dep.back()->printDefinition(out, indentSize);
            defined.push_back(dep.back());
        }
    }
}

void ModelData::writeParticles(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &defined
        )
{
    std::string indent(indentSize, ' ');
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Particle definitions          //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &part : particles)
        if (part->getSpinDimension() == 3
                and part->getParticleType() != ParticleType::FieldStrength) {
            part->printDefinition(out, indentSize);
            defined.push_back(part);
            OUT << END;
        }
    for (const auto &part : particles)
        if (part->getSpinDimension() == 1) {
            part->printDefinition(out, indentSize);
            defined.push_back(part);
            OUT << END;
        }
    for (const auto &part : particles)
        if (part->getSpinDimension() == 2 and part->isChiral()) {
            part->printDefinition(out, indentSize);
            defined.push_back(part);
            OUT << END;
        }
    for (const auto &part : particles)
        if (part->getSpinDimension() == 2 and !part->isChiral()) {
            part->printDefinition(out, indentSize);
            defined.push_back(part);
            OUT << END;
        }
}

void ModelData::writeDependencies(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Parent> &defined,
        std::vector<csl::Expr> &kinetic,
        std::vector<csl::Expr> &mass,
        std::vector<csl::Expr> &interac
        )
{
    std::string indent(indentSize, ' ');
    std::set<csl::AbstractParent const*> parents;
    std::map<csl::Space const*, std::vector<csl::Index>> indices;
    std::map<csl::Space const*, size_t> nIndices;
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
        i.setName(std::string(posI->getName()));
        i.setID(std::distance(pos->second.begin(), posI));
    };
    auto updateIndices = [&]() 
    {
        for (const auto &[space, index] : indices) {
            nIndices[space] = std::max(nIndices[space], index.size());
        }
        indices.clear();
    };
    std::set<csl::Space const*> spaces;
    std::set<csl::AbstractParent const*> constants;
    auto gatherDependencies = [&](
            std::vector<csl::Expr> &terms
            )
    {
        for (auto &term : terms) {
            csl::ForEachNode(term, [&](csl::Expr &sub) {
                if (csl::IsField(sub)) 
                    parents.insert(sub->getPoint().get());
                if (csl::IsIndicialTensor(sub)
                        and !IsOfType<QuantumField>(sub)
                        and !dirac4.isGammaTensor(sub)) 
                    parents.insert(sub->getParent_info());
                if (csl::IsIndicialTensor(sub)) {
                    for (auto &index : sub->getIndexStructureView()) {
                        insertIndex(index);
                        spaces.insert(index.getSpace());
                    }
                }
                if (csl::IsVariable(sub) 
                        or csl::IsConstant(sub)) {
                    if (auto pos = std::find(
                                defined.begin(), 
                                defined.end(),
                                sub->getParent()); 
                            pos == defined.end()) {
                        defined.push_back(sub->getParent());
                        constants.insert(sub->getParent_info());
                    }
                }
            });
            updateIndices();
        }
    };
    gatherDependencies(kinetic);
    gatherDependencies(mass);
    gatherDependencies(interac);
    OUT << "///////////////////////////////////" << END;
    OUT << "// Spaces                        //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    for (const auto &s : spaces)
        s->printDefinition(out, indentSize);
    OUT << END << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Tensors                       //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    for (const auto &p : parents)
        p->printDefinition(out, indentSize);
    OUT << END << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Variable / Constants          //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    for (const auto &c : constants)
        c->printDefinition(out, indentSize);
    OUT << END << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Indices                       //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    for (const auto &[space, nIndex] : nIndices) {
        OUT << "std::vector<csl::Index> i_" 
            << csl::Abstract::regularName(space->getName());
        out << " = ";
        space->printCode(out, 0);
        out << ".generateIndices(" << nIndex << ");\n";
    }
    OUT << END << END;
}

void ModelData::writeLagrangian(
        std::ostream &out,
        int           indentSize,
        std::vector<csl::Expr> const &kinetic,
        std::vector<csl::Expr> const &mass,
        std::vector<csl::Expr> const &interac
        )
{
    std::string indent(indentSize, ' ');
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Kinetic terms                 //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &kin : kinetic) {
        OUT << "model->addLagrangianTerm(" << END;
        OUT2;
        kin->printCode(1, out);
        out << "," << END;
        OUT2 << "false);" << END;
    }
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Mass terms                    //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &mass : mass) {
        OUT << "model->addLagrangianTerm(" << END;
        OUT2;
        mass->printCode(1, out);
        out << "," << END;
        OUT2 << "false);" << END;
    }
    OUT << END;
    OUT << END;
    OUT << "///////////////////////////////////" << END;
    OUT << "// Interaction terms             //" << END;
    OUT << "///////////////////////////////////" << END;
    OUT << END;
    OUT << END;
    for (const auto &interac : interac) {
        OUT << "model->addLagrangianTerm(" << END;
        OUT2;
        interac->printCode(1, out);
        out << "," << END;
        OUT2 << "false);" << END;
    }
}

void ModelData::saveModelFunction(
        std::string_view name,
        std::ostream    &out,
        int              indentSize
        )
{
    std::string indent(indentSize, ' ');
    OUT << "inline std::unique_ptr<mty::Model> build_" << name << "()\n";
    OUT << "{\n";
    saveModel(out, 4 + indentSize);
    OUT2 << "return model;" << END;
    OUT << "}\n";
}

void ModelData::saveModelFunctionWithRef(
        std::string_view name,
        std::ostream    &out,
        int              indentSize
        )
{
    std::string indent(indentSize, ' ');
    saveModelFunction(name, out, indentSize);
    OUT << "\n";
    OUT << "inline mty::Model &get_" << name << "()\n";
    OUT << "{\n";
    OUT2 << "static std::unique_ptr<mty::Model> model = ";
    out << "build_" << name << "();\n";
    OUT2 << "return *model;\n";
    OUT << "}\n";
}

void ModelData::saveModelFile(
        std::string_view   name,
        std::string const &fileName
        )
{
    std::string upperCaseName(name);
    for (char &c : upperCaseName)
        if (c >= 'a' and c <= 'z')
            c += 'A' - 'a';
        else if (c < '0' or c > '9')
            c = '_';
    std::ofstream modelFile(fileName.c_str());
    modelFile << "#ifndef DEFINITION_" << upperCaseName << "_H_INCLUDED\n";
    modelFile << "#define DEFINITION_" << upperCaseName << "_H_INCLUDED\n\n";
    modelFile << "#include \"marty.h\"\n\n";
    modelFile << "namespace mty {\n\n";
    saveModelFunctionWithRef(
            csl::Abstract::regularName(name), 
            modelFile);
    modelFile << "\n} // End of namespace mty\n\n";
    modelFile << "#endif";
}

///////////////////////////////////////////////////
// Getters
///////////////////////////////////////////////////

mty::Lagrangian const &ModelData::getLagrangian() const
{
    return L;
}

csl::Space const *ModelData::getSpaceTime() const
{
    return spaceTime;
}

mty::Gauge *ModelData::getGauge() const
{
    return gauge.get();
}

mty::Flavor *ModelData::getFlavor() const
{
    return flavor.get();
}

std::vector<mty::Particle> const &ModelData::getParticles() const
{
    return particles;
}

std::vector<mty::Particle> ModelData::getPhysicalParticles(
        std::function<bool(Particle)> const &predicate
        ) const
{
    std::vector<mty::Particle> physicalParticles;
    physicalParticles.reserve(particles.size());

    for (const auto &p : particles) {
        if (!predicate(p))
            continue;
        if (!p->isPhysical())
            continue;
        if (p->getParticleType() == mty::ParticleType::WeylFermion) {
            if (auto dirac = p->getDiracParent(); dirac) {
                if (predicate(dirac) && findParticle(dirac))
                    continue;
            }
        }
        physicalParticles.push_back(p);
    }

    return physicalParticles;
}

std::vector<QuantumNumber> const &ModelData::getQuantumNumbers() const
{
    return quantumNumbers;
}
std::vector<QuantumNumber> &ModelData::getQuantumNumbers()
{
    return quantumNumbers;
}

std::vector<csl::Expr> const &ModelData::getScalarCouplings() const
{
    return scalarCouplings;
}

std::vector<csl::Tensor> const &ModelData::getTensorCouplings() const
{
    return tensorCouplings;
}

///////////////////////////////////////////////////
// Setters
///////////////////////////////////////////////////

void ModelData::setSpaceTime(csl::Space const *t_spaceTime)
{
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "You may not modify the space-time of a model containing "
            "particles.");
    CallHEPError(mty::error::NotImplementedError,
            "Mutable space-time is not yet enabled in CSL-HEP.");
    spaceTime = t_spaceTime;
}

void ModelData::setGauge(std::unique_ptr<mty::Gauge> &&t_gauge)
{
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "Cannot set a gauge for a model that already has particles.");
    gauge = std::move(t_gauge);
}

void ModelData::addGaugedGroup(
        group::Type        type,
        std::string_view name,
        int              dim,
        csl::Expr      const& coupling)
{
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "Cannot add a group for a model that already has particles.");
    auto g = createGroup(type, std::string(name), dim);
    (coupling) ? gauge->addGroup(g, coupling) 
               : gauge->addGroup(g);
}
void ModelData::addGaugedGroup(
        group::Type        type,
        std::string_view name,
        csl::Expr      const& coupling)
{
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "Cannot add a group for a model that already has particles.");
    auto g = createGroup(type, std::string(name));
    (coupling) ? gauge->addGroup(g, coupling) 
               : gauge->addGroup(g);
}

void ModelData::addFlavorGroup(
        std::string_view name,
        int              nFlavor,
        bool             complexFields
        )
{
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "Cannot add a flavor for a model that already has particles.");
    std::unique_ptr<FlavorGroup> flavorGroup
        = std::make_unique<FlavorGroup>(
                std::string(name), nFlavor, complexFields);
    if (not flavor)
        flavor = std::make_unique<Flavor>();
    flavor->addGroup(flavorGroup);
}

void ModelData::setFlavor(std::unique_ptr<mty::Flavor> &&t_flavor)
{
    HEPAssert(not flavor,
            mty::error::ModelBuildingError,
            "Cannot set a flavor for a model that already has one.");
    HEPAssert(particles.empty(),
            mty::error::ModelBuildingError,
            "Cannot set a flavor for a model that already has particles.");
    flavor = std::move(t_flavor);
}

void ModelData::addParticle(
        mty::Particle const &part,
        bool                 initializeTerms
        )
{
    if (spaceTime != defaultSpaceTime)
        part->setFieldSpace(spaceTime);
    if (findParticle(part) 
            or part->getParticleType() == mty::ParticleType::FieldStrength)
        return;
    if (!gaugeLocked and gauge->size() == 0)
        gaugeLocked = true;
    checkValidity(part);
    if (!part->getFlavor())
        part->setFlavor(flavor.get());
    particles.push_back(part);
    if (IsOfType<DiracFermion>(part)) {
        addParticle(part->getWeylFermion(Chirality::Left), false);
        addParticle(part->getWeylFermion(Chirality::Right), false);
    }
    if (initializeTerms) {
        csl::Tensor X = L.getPoint();
        csl::Expr kineticTerm = part->getKineticTerm(X);
        kineticTerm = csl::Refreshed(csl::DeepExpanded(kineticTerm));
        addLagrangianTerm(kineticTerm);
        csl::Expr massTerm = mty::MassTerm(part->getMass(), part.get());
        addLagrangianTerm(massTerm);
    }
}
void ModelData::addParticles(
        std::vector<mty::Particle> const &parts,
        bool                              initializeTerms)
{
    for (const auto& p : parts)
        addParticle(p, initializeTerms);
}

void ModelData::addParticlesIn(csl::Expr const &expr)
{
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub) {
        if (!csl::IsIndicialTensor(sub))
            return;
        auto ptr = std::dynamic_pointer_cast<mty::QuantumFieldParent>(
                sub->getParent());
        auto part = mty::Particle(ptr);
        if (part && !findParticle(part)) {
            addParticle(part, false);
        }
    });
}

void ModelData::removeParticle(mty::Particle const &part)
{
    for (size_t i = 0; i != particles.size(); ++i) {
        if (particles[i] == part) {
            auto left  = particles[i]->getWeylFermion(Chirality::Left);
            auto right = particles[i]->getWeylFermion(Chirality::Right);
            auto ghost = particles[i]->getGhost();
            auto fieldStrength = particles[i]->getFieldStrength();
            particles.erase(particles.begin() + i);
            if (left)
                removeParticle(left);
            if (right)
                removeParticle(right);
            // if (ghost)
            //     removeParticle(ghost);
            // if (fieldStrength)
            //     removeParticle(fieldStrength);
            break;
        }
    }
    L.removeParticle(part);
}

void ModelData::removeParticles(
        std::vector<mty::Particle> const &parts
        )
{
    for (const auto &part : parts)
        removeParticle(part);
}

void ModelData::removeParticle(std::string const &part)
{
    for (size_t i = 0; i != particles.size(); ++i) {
        if (particles[i]->getName() == part) {
            removeParticle(Particle(particles[i]));
            return;
        }
    }
}

void ModelData::removeParticles(
        std::vector<std::string> const &parts
        )
{
    for (const auto &part : parts)
        removeParticle(part);
}

void ModelData::addScalarCoupling(std::string_view coupling)
{
    HEPAssert(not findScalarCoupling(coupling),
            mty::error::NameError,
            "A coupling named \"" + toString(coupling) + "\" is already present"
            " in the model. Please choose another one.");
    scalarCouplings.push_back(csl::constant_s(std::string(coupling)));
}
void ModelData::addScalarCoupling(csl::Expr const &coupling)
{
    if (findScalarCoupling(coupling))
        return;
    if (coupling->isComplexConjugate())
        scalarCouplings.push_back(csl::GetComplexConjugate(coupling));
    else
        scalarCouplings.push_back(coupling);
}

void ModelData::addTensorCoupling(csl::Parent const &coupling)
{
    if (findTensorCoupling(coupling))
        return;
    csl::Tensor tensor(std::dynamic_pointer_cast<csl::TensorParent>(
            coupling
            ));
    HEPAssert(tensor, 
            mty::error::TypeError,
            "Expecting a tensor as tensor coupling in model, " 
            + toString(coupling->getName()) + " given.");
    tensorCouplings.push_back(tensor);
}

void ModelData::addQuantumNumber(
        std::string_view                  t_name,
        std::vector<mty::Particle> const& fields,
        std::vector<int>           const& values,
        bool                              conserved
        )
{
    HEPAssert(fields.size() == values.size() or values.empty(),
            mty::error::ValueError,
            "Expecting 1 value per particle for quantum numbers.");
    mty::QuantumNumber q(std::string(t_name), conserved);
    for (size_t i = 0; i != fields.size(); ++i) {
        int number = 
            (values.empty()) ? 
            defaultQuantumNumber
            :values[i];
        fields[i]->addQuantumNumber(q, number);
    }
    quantumNumbers.push_back(q);
}

void ModelData::setQuantumNumbers(
        std::vector<mty::QuantumNumber> const &numbers
        )
{
    quantumNumbers = numbers;
}

///////////////////////////////////////////////////
// Content
///////////////////////////////////////////////////

bool ModelData::findParticle(std::string_view t_name) const
{
    for (const auto& part : particles)
        if (part->getName() == t_name)
            return true;
    return false;
}
bool ModelData::findParticle(mty::Particle const &part) const
{
    return findParticle(part.get());
}
bool ModelData::findParticle(mty::QuantumFieldParent const *part) const
{
    for (const auto& p : particles)
        if (p.get() == part)
            return true;
    return false;
}

bool ModelData::findScalarCoupling(std::string_view t_name) const
{
    for (const auto& coupling : scalarCouplings)
        if (coupling->getName() == t_name)
            return true;
    return false;
}
bool ModelData::findScalarCoupling(csl::Expr const &t_coupling) const
{
    if (t_coupling->isComplexConjugate())
        return findScalarCoupling(csl::GetComplexConjugate(t_coupling));
    for (const auto& coupling : scalarCouplings)
        if (coupling == t_coupling)
            return true;
    return false;
}

bool ModelData::findTensorCoupling(std::string_view t_name) const
{
    for (const auto& tensor : tensorCouplings)
        if (tensor->getName() == t_name)
            return true;
    return false;
}
bool ModelData::findTensorCoupling(csl::Parent const &coupling) const
{
    return findTensorCoupling(coupling.get());
}
bool ModelData::findTensorCoupling(csl::AbstractParent const *coupling) const
{
    for (const auto& tensor : tensorCouplings)
        if (tensor.get() == coupling)
            return true;
    return false;
}

///////////////////////////////////////////////////
// Finders
///////////////////////////////////////////////////

mty::Group const *ModelData::getGroup(std::string_view t_name) const
{
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i]->getName() == t_name)
            return (*gauge)[i];
    if (flavor)
        for (size_t i = 0; i != flavor->size(); ++i)
            if ((*flavor)[i]->getName() == t_name)
                return (*flavor)[i]->getGroup();
    CallHEPError(mty::error::NameError,
            "Group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}
mty::Group *ModelData::getGroup(std::string_view t_name)
{
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i]->getName() == t_name)
            return (*gauge)[i];
    if (flavor)
        for (size_t i = 0; i != flavor->size(); ++i)
            if ((*flavor)[i]->getName() == t_name)
                return (*flavor)[i]->getGroup();
    CallHEPError(mty::error::NameError,
            "Group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}

mty::Group const *ModelData::getGroup(FlavorGroup const *flavGroup) const
{
    for (const auto &flav : *flavor)
        if (flav == flavGroup)
            return flav->getGroup();
    CallHEPError(mty::error::NameError,
            "Flavor group of name \"" + std::string(flavGroup->getName()) 
            + "\" not found in model.");
    return nullptr;
}
mty::Group *ModelData::getGroup(FlavorGroup const *flavGroup)
{
    for (const auto &flav : *flavor)
        if (flav == flavGroup)
            return flav->getGroup();
    CallHEPError(mty::error::NameError,
            "Flavor group of name \"" + std::string(flavGroup->getName()) 
            + "\" not found in model.");
    return nullptr;
}
mty::GaugedGroup const *ModelData::getGaugedGroup(
        std::string_view t_name
        ) const
{
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i]->getName() == t_name)
            return gauge->getGaugedGroup(i);
    CallHEPError(mty::error::NameError,
            "Gauged group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}
mty::GaugedGroup *ModelData::getGaugedGroup(std::string_view t_name)
{
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i]->getName() == t_name)
            return gauge->getGaugedGroup(i);
    CallHEPError(mty::error::NameError,
            "Gauged group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}

mty::FlavorGroup const *ModelData::getFlavorGroup(std::string_view t_name) const
{
    HEPAssert(flavor,
            mty::error::KeyError,
            "There is no flavor in the model");
    for (size_t i = 0; i != flavor->size(); ++i)
        if ((*flavor)[i]->getName() == t_name)
            return (*flavor)[i];
    CallHEPError(mty::error::NameError,
            "Flavor group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}
mty::FlavorGroup *ModelData::getFlavorGroup(std::string_view t_name)
{
    HEPAssert(flavor,
            mty::error::KeyError,
            "There is no flavor in the model");
    for (size_t i = 0; i != flavor->size(); ++i)
        if ((*flavor)[i]->getName() == t_name)
            return (*flavor)[i];
    CallHEPError(mty::error::NameError,
            "Flavor group of name \"" + std::string(t_name) 
            + "\" not found in model.");
    return nullptr;
}

static void particleNotFound(
        std::vector<mty::Particle> const &particles,
        std::string                const &name
        )
{
    std::cerr << "Candidates are :" << std::endl;
    for (const auto &p : particles) {
        std::cerr << p->getName() << " (" << p << " spin dim. = " 
            << p->getSpinDimension() << ")";
        if (auto fs = p->getFieldStrength(); fs) {
            std::cerr << " , Field Strength = " << fs->getName()
                << " (" << fs << ")";
        }
        std::cerr << '\n';
    }
    CallHEPError(
            mty::error::NameError,
            "Particle of name \"" + name + "\" not found in model."
            )
}

std::vector<mty::Particle> ModelData::getParticles(
        std::initializer_list<std::string_view> names
        ) const
{
    std::vector<mty::Particle> res(names.size());
    auto nameIter = begin(names);
    std::generate(begin(res), end(res),
            [&]() {
                return getParticle(*nameIter++);
            });

    return res;
}

mty::Particle ModelData::getParticle(std::string_view t_name) const
{
    for (const auto& part : particles) {
        if (part->getName() == t_name or part->getLatexName() == t_name)
            return part;
        if (auto fs = part->getFieldStrength(); fs && fs->getName() == t_name) {
            return part;
        }
    }
    particleNotFound(particles, std::string(t_name));
    return nullptr;
}
mty::Particle ModelData::getParticle(mty::QuantumFieldParent const *field) const
{
    for (const auto& part : particles) {
        if (part.get() == field)
            return part;
        if (auto fs = part->getFieldStrength(); fs && fs.get() == field) {
            return part;
        }
    }
    particleNotFound(particles, field->getName());
    return nullptr;
}
mty::Particle ModelData::getParticle(mty::QuantumFieldParent const &field) const
{
    return getParticle(&field);
}
mty::Particle ModelData::getParticle(mty::QuantumField const &field) const
{
    return getParticle(field.getQuantumParent());
}
mty::Particle ModelData::getParticle(csl::Expr const &field) const
{
    HEPAssert(IsOfType<mty::QuantumField>(field),
            mty::error::TypeError,
            "Expecting a quantum field, " + toString(field) + " given.");
    return getParticle(ConvertTo<mty::QuantumField>(field));
}

csl::Expr ModelData::getScalarCoupling(std::string_view t_name) const
{
    for (const auto& coupling : scalarCouplings)
        if (coupling->getName() == t_name or coupling->getLatexName() == t_name)
            return coupling;
    CallHEPError(mty::error::NameError,
            "Scalar coupling \"" + toString(t_name) + "\" not found in model.");
    return nullptr;
}

csl::Tensor ModelData::getTensorCoupling(std::string_view t_name) const
{
    for (const auto& coupling : tensorCouplings)
        if (coupling->getName() == t_name or coupling->getLatexName() == t_name)
            return coupling;
    CallHEPError(mty::error::NameError,
            "Tensor coupling \"" + toString(t_name) + "\" not found in model.");
    return nullptr;
}
csl::Tensor ModelData::getTensorCoupling(csl::Parent const &t_coupling) const
{
    for (const auto& coupling : tensorCouplings)
        if (coupling.get() == t_coupling.get())
            return coupling;
    CallHEPError(mty::error::NameError,
            "Tensor coupling \"" + toString(t_coupling->getName()) 
            + "\" not found in model.");
    return nullptr;
}

mty::GaugeIrrep ModelData::doGetGaugeIrrep(mty::Particle const &part) const
{
    return part->getGaugeIrrep();
}
mty::Irrep ModelData::doGetGroupIrrep(
        mty::Particle   const &part,
        mty::Group      const *group) const
{
    return part->getGroupIrrep(group);
}
csl::Expr ModelData::doGetCharge(
        mty::Particle const &particle,
        mty::Group    const *group
        ) const
{
    return particle->getGroupIrrep(group).getCharge();
}

mty::FlavorIrrep ModelData::doGetFlavorIrrep(mty::Particle const &part) const
{
    HEPAssert(flavor,
            mty::error::KeyError,
            "There is no flavor in the model");
    return part->getFlavorIrrep();
}
mty::Irrep ModelData::doGetFlavorIrrep(
        mty::Particle    const &part,
        mty::Group       const *flav) const
{
    HEPAssert(flavor,
            mty::error::KeyError,
            "There is no flavor in the model");
    for (const auto &flavGroup : *flavor)
        if (flavGroup->getGroup() == flav)
            return part->getFlavorIrrep(flavGroup);
    HEPAssert(false,
            mty::error::KeyError,
            "Flavor " + toString(flav->getName()) + " not found in the model");

    return Irrep();
}

const csl::Space* ModelData::doGetVectorSpace(
        mty::Group    const *group,
        mty::Particle const &field
        ) const
{
    return group->getVectorSpace(getGroupIrrep(field, group));
}

const csl::Space* ModelData::doGetVectorSpace(
        mty::FlavorGroup const *group,
        mty::Particle    const &field
        ) const
{
    return group->getVectorSpace(getFlavorIrrep(field, group));
}

csl::Index ModelData::doGenerateIndex(
        mty::Group    const *group,
        mty::Particle const &part
        ) const
{
    csl::Space const *vectorSpace = getVectorSpace(group, part);
    return vectorSpace->generateIndex();
}
csl::Index ModelData::doGenerateIndex(
        mty::Group    const *group
        ) const
{
    for (const auto &flavorGroup : *flavor) {
        if (flavorGroup->getGroup() == group)
            return group->getVectorSpace(
                    flavorGroup->getFundamentalRep()
                    )->generateIndex();
    }
    CallHEPError(mty::error::TypeError,
            "Non-flavor groups are not yet handle in this function, please give"
            " a particle for gauge representation indices.")
    return csl::Index();
}
csl::Index ModelData::doGenerateIndex(
        std::string_view     nameIndex, 
        mty::Group    const *group,
        mty::Particle const &part
        ) const
{
    csl::Space const *vectorSpace = getVectorSpace(group, part);
    return vectorSpace->generateIndex(std::string(nameIndex));
}

mty::Generator ModelData::doGetGenerator(
        mty::Group    const *group,
        mty::Particle const &part
        ) const
{
    return group->getGaugedGroup()->getGenerator(
            part->getGroupIrrep(group)
            );
}

mty::QuantumNumber const &ModelData::getQuantumNumber(
        std::string_view t_name
        ) const
{
    for (const auto& number : quantumNumbers)
        if (number.getName() == t_name)
            return number;
    CallHEPError(mty::error::NameError,
            "Quantum number \"" + toString(t_name) 
            + "\" not found in model.");
    return quantumNumbers[0];
}
mty::QuantumNumber &ModelData::getQuantumNumber(
        std::string_view t_name
        )
{
    for (auto& number : quantumNumbers)
        if (number.getName() == t_name)
            return number;
    CallHEPError(mty::error::NameError,
            "Quantum number \"" + toString(t_name) 
            + "\" not found in model.");
    return quantumNumbers[0];
}

///////////////////////////////////////////////////
// Model building
///////////////////////////////////////////////////

void ModelData::checkValidity(mty::Particle const &part) const
{
    HEPAssert(gaugeLocked,
            mty::error::ModelBuildingError,
            "You may not add a particle to a model that is not initialized. "
            "Consider calling model.init() after building the gauge.");
    HEPAssert(part->getGauge() == gauge.get(),
            mty::error::ModelBuildingError,
            "Gauge of particle " + toString(part->getName()) + " does not match"
            " model gauge.");
    HEPAssert(!part->getFlavor() or part->getFlavor() == flavor.get(),
            mty::error::ModelBuildingError,
            "Flavor of particle " + toString(part->getName()) + " does not match"
            " model flavor.");
}

void ModelData::checkValidity(mty::Lagrangian::TermType const &term) const
{
    std::vector<mty::QuantumField> const &content = term->getContent();
    auto trivialRep = gauge->getTrivialRep();
    mty::SumGaugeIrrep gaugeIrrep({trivialRep});
    for (const auto& field : content) {
        gaugeIrrep = gaugeIrrep * 
            ((field.isComplexConjugate()) ?
             field.getQuantumParent()->getGaugeIrrep()
             :field.getQuantumParent()->getGaugeIrrep().getConjugatedRep());
    }
    HEPAssert(gaugeIrrep.find(trivialRep) != -1,
            mty::error::PhysicsError,
            "Lagrangian term " + toString(term->getFullExpression())
            + " is not gauge invariant (does not contains trivial "
            "representation)");
}

void ModelData::addLagrangianTerm(
        csl::Expr const &term,
        bool        addHermitic)
{
    if (enableChecks) {
        std::vector<mty::Lagrangian::TermType> terms 
            = mty::InteractionTerm::createAndDispatch(term);
        for (const auto& t : terms) {
            addLagrangianTerm(t);
        }
    }
    else {
        addLagrangianTerm(std::make_shared<InteractionTerm>(term));
    }
    if (addHermitic) {
        csl::Expr hermitic = csl::GetHermitianConjugate(term, &mty::dirac4);
        addLagrangianTerm(hermitic, false);
    }
}
void ModelData::addLagrangianTerm(
        mty::Lagrangian::TermType const &term,
        bool
        )
{
    if (enableChecks)
        checkValidity(term);
    if (*term == CSL_0)
        return;
    L.push_back(term);
    std::vector<Particle> newParticles = term->getParticles();
    for (auto& newPart : newParticles) {
        bool found = false;
        for (const auto& part : particles)
            if (part.get() == newPart.get()
                    or (part->hasFieldStrength()
                        and part->getFieldStrength().get() == newPart.get())) {
                found = true;
                break;
            }
        HEPAssert(found,
                mty::error::ModelBuildingError,
                "Adding an interaction with an unkown particle "
                + toString(newPart->getName()) 
                + " in Model::addLagrangianTerm()");
    }
}

void ModelData::doAddBosonicMass(
        mty::Particle const &particle,
        csl::Expr          const &mass
        )
{
    particle->setMass(mass);
    for (size_t i = 0; i != L.mass.size(); ++i)
        if (L.mass[i]->containsExactly(particle.get())) {
            L.mass.erase(L.mass.begin() + i);
            --i;
        }
    L.push_back(MassTerm(mass, particle.get()));
}
void ModelData::doAddBosonicMass(
        mty::Particle const &particle,
        std::string_view     mass
        )
{
    doAddBosonicMass(particle, csl::constant_s(std::string(mass)));
}

void ModelData::doAddFermionicMass(
        mty::Particle const &left,
        mty::Particle const &right,
        csl::Expr          const &mass
        )
{
    left->setMass(mass);
    right->setMass(mass);
    for (size_t i = 0; i != L.mass.size(); ++i)
        if (L.mass[i]->containsExactly(left.get())
                or L.mass[i]->containsExactly(right.get())) {
            L.mass.erase(L.mass.begin() + i);
            --i;
        }
    L.push_back(MassTerm(mass, left.get(), right.get()));
}
void ModelData::doAddFermionicMass(
        mty::Particle const &left,
        mty::Particle const &right,
        std::string_view     mass
        )
{
    doAddFermionicMass(left, right, csl::constant_s(std::string(mass)));
}
void ModelData::doAddFermionicMass(
        mty::Particle const &diracFermion,
        csl::Expr          const &mass
        )
{
    doAddBosonicMass(diracFermion, mass);
}
void ModelData::doAddFermionicMass(
        mty::Particle const &diracFermion,
        std::string_view     mass
        )
{
    doAddBosonicMass(diracFermion, mass);
}

void ModelData::doRenameParticle(
        mty::Particle const &part,
        std::string_view     newName
        )
{
    switch(part->getParticleType()) {
        case ParticleType::ScalarBoson:
        case ParticleType::GhostBoson:
        case ParticleType::GoldstoneBoson:
        case ParticleType::VectorBoson:
        case ParticleType::WeylFermion:
            part->setName(std::string(newName));
            break;
        case ParticleType::DiracFermion:
            part->setName(std::string(newName));
            if (auto leftWeyl = part->getWeylFermion(Chirality::Left);
                    leftWeyl)
                leftWeyl->setName(std::string(newName) + "_L");
            if (auto rightWeyl = part->getWeylFermion(Chirality::Right);
                    rightWeyl)
                rightWeyl->setName(std::string(newName) + "_R");
            break;
        default:
            CallHEPError(mty::error::TypeError,
                    "Particle of type " + toString(part->getParticleType())
                    + " not known.");
    }
}

void ModelData::renameCoupling(
        std::string_view name,
        std::string_view newName
        )
{
    for (auto &scalar : scalarCouplings)
        if (scalar->getName() == name) {
            scalar->setName(std::string(newName));
            return;
        }
    for (auto &tensor : tensorCouplings)
        if (tensor->getName() == name) {
            tensor->setName(std::string(newName));
            return;
        }
    CallHEPError(mty::error::KeyError,
            "Coupling named " + toString(name) + " not found in model.");
}

///////////////////////////////////////////////////
// Display
///////////////////////////////////////////////////

void ModelData::printSubPart(
        std::initializer_list<std::string> particleNames,
        std::ostream &out
        ) const
{
    printSubPart(std::vector<std::string>(particleNames), out);
}
void ModelData::printSubPart(
        std::vector<std::string> const &particleNames,
        std::ostream &out
        ) const
{
    std::vector<mty::Particle> particles(particleNames.size());
    for (size_t i = 0; i != particles.size(); ++i)
        particles[i] = getParticle(particleNames[i]);
    out << "Interaction terms :" << std::endl;
    size_t i = 0;
    for (const auto &term : L.interaction) {
        std::vector<mty::QuantumField> const &content = term->getContent();
        bool cont = false;
        for (const auto &part : particles) {
            bool found = false;
            auto fs = (part->getFieldStrength()) ? 
                part->getFieldStrength().get() : nullptr;
            for (const auto &field : content)
                if (field.getQuantumParent() == part.get()
                        or field.getQuantumParent() == fs) {
                    found = true;
                    break;
                }
            if (not found)
                cont = true;
        }
        if (cont)
            continue;
        out << i++ << " : " << term->getFullExpression() << std::endl;
    }
}
void ModelData::printSubPart(
        std::string const &particle,
        std::ostream &out
        ) const
{
    printSubPart({particle}, out);
}

std::ostream &operator<<(
        std::ostream    &out, 
        ModelData const &model)
{
    out << "Gauge = " << *model.gauge << '\n';
    out << "Particle list, " << model.particles.size() << " particles:\n";
    for (const auto& part : model.particles) {
        out << "  -> " << part->getName();
        if (not part->isSelfConjugate())
            out << "(*)";
        out << ": \n";
        out << "    mass: " << part->getMass() << "\n";
        out << "    spin: " << (csl::int_s(part->getSpinDimension()-1)/2);
        out << "\n    irrep: " << part->getGaugeIrrep() << '\n';
    }
    out << model.L.fullSize() << "-terms lagrangian: \n";
    out << model.L << '\n';

    return out;
}

///////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////

void ModelData::init(bool initGaugeTerms)
{
    initGaugedGroups(initGaugeTerms);
}

void ModelData::initGaugedGroups(bool initGaugeTerms)
{
    HEPAssert(!gaugeLocked,
            mty::error::ModelBuildingError,
            "You may initialize a model only once, after the gauge is "
            "fully constructed.");
    gaugeLocked = true;
    auto gaugeBosons = gauge->getVectorBosons();
    for (auto& part : gaugeBosons) {
        part->setFlavor(flavor.get());
        addParticle(part, initGaugeTerms);
    }
    auto ghosts = gauge->getGhosts();
    for (const auto& part : ghosts) {
        part->setFlavor(flavor.get());
        addParticle(part, initGaugeTerms);
    }
    for (size_t i = 0; i != gauge->size(); ++i)
        addScalarCoupling(gauge->getGaugedGroup(i)->getCouplingConstant());
}

} // End of namespace mty
