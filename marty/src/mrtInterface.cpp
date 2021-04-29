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

#include "mrtInterface.h"
#include "drawer.h"
#include "diracology.h"
#include "insertion.h"
#include "mrtOptions.h"
#include "amplitude.h"
#include "wilson.h"

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Expression utilities                          //
/*************************************************/
///////////////////////////////////////////////////

csl::Tensor MinkowskiVector(std::string const& name)
{
    return csl::tensor_s(name, &csl::Minkowski);
}

csl::Index MinkowskiIndex(std::string const& name)
{
    if (name.empty())
        return csl::Minkowski.generateIndex();
    return csl::Minkowski.generateIndex(name);
}

csl::Index DiracIndex(std::string const& name)
{
    if (name.empty())
        return mty::dirac4.generateIndex();
    return mty::dirac4.generateIndex(name);
}

std::vector<csl::Index> MinkowskiIndices(
        size_t n,
        std::string const &nameIndex)
{
    std::vector<csl::Index> indices(n);
    for (size_t i = 0; i != n; ++i) 
        indices[i] = MinkowskiIndex(nameIndex);

    return indices;
}

std::vector<csl::Index> DiracIndices(
        size_t n,
        std::string const &nameIndex)
{
    std::vector<csl::Index> indices(n);
    for (size_t i = 0; i != n; ++i) 
        indices[i] = DiracIndex(nameIndex);

    return indices;
}

csl::Index GaugeIndex(std::string const& nameIndex,
                      mty::Model  const& model,
                      std::string const& nameGroup,
                      Particle    const& particle)
{
    mty::Group const *group = model.getGroup(nameGroup);
    mty::Irrep irrep  = particle->getGroupIrrep(group);
    return group->getVectorSpace(irrep)->generateIndex(nameIndex);
}

csl::Index GaugeIndex(mty::Model  const& model,
                      std::string const& nameGroup,
                      Particle    const& particle)
{
    mty::Group const *group = model.getGroup(nameGroup);
    mty::Irrep irrep  = particle->getGroupIrrep(group);
    const csl::Space *space = group->getVectorSpace(irrep);
    HEPAssert(space,
            mty::error::TypeError,
            "Group " + group->getName() + " has no vector space for "
            + "representation " + toString(irrep));
    return space->generateIndex();
}

const csl::Space* VectorSpace(mty::Model  const& model,
                              std::string const& nameGroup,
                              Particle    const& particle)
{
    mty::Group const *group = model.getGroup(nameGroup);
    mty::Irrep irrep  = particle->getGroupIrrep(group);
    return group->getVectorSpace(irrep);
}

csl::Index GenerateIndex(
        csl::Space  const *space,
        std::string const &name
        )
{
    if (name.empty())
        return space->generateIndex();
    return space->generateIndex(name);
}

csl::Tensor Delta(const csl::Space* space)
{
    return space->getDelta();
}

csl::Tensor Metric(const csl::Space* space)
{
    return space->getMetric();
}

csl::Tensor Epsilon(const csl::Space* space) 
{
    return space->getEpsilon();
}

csl::Tensor DiracGamma() {
    return mty::dirac4.gamma;
}
csl::Tensor DiracGamma5() {
    return mty::dirac4.gamma_chir;
}
csl::Tensor DiracSigma() {
    return mty::dirac4.sigma;
}
csl::Tensor DiracPL() {
    return mty::dirac4.P_L;
}
csl::Tensor DiracPR() {
    return mty::dirac4.P_R;
}
csl::Tensor DiracCMatrix() {
    return mty::dirac4.C_matrix;
}

///////////////////////////////////////////////////
/*************************************************/
// Computation utilities                         //
/*************************************************/
///////////////////////////////////////////////////

Amplitude SelfEnergy(
            mty::Order     order,
            mty::Particle &particle,
            mty::Model    &model)
{
    csl::ScopedProperty prop(&mty::option::amputateExternalLegs, true);
    return model.computeAmplitude(
            order,
            {Incoming(OffShell(particle)), Outgoing(OffShell(particle))}
            );
}

Amplitude SelfEnergy(
            mty::Particle &particle,
            mty::Model    &model)
{
    return SelfEnergy(mty::Order::OneLoop, particle, model);
}

csl::Expr ComputeSquaredAmplitude(
        mty::Model      &model,
        Amplitude const &ampl
        )
{
    return model.computeSquaredAmplitude(ampl);
}

std::vector<FeynmanRule> ComputeFeynmanRules(mty::Model& model)
{
    return model.getFeynmanRules();
}

void ContractIndices(csl::Expr &init)
{
    init = csl::Evaluated(init, csl::eval::abbreviation);
    csl::LibEvalSession::parseExpression(init, false, false);
}

void ContractIndices(std::vector<csl::Expr> &init)
{
    for (auto &expr : init)
        ContractIndices(expr);
}

void Display(std::vector<FeynmanRule> const& rules,
             std::ostream                  & out)
{
    if (mty::option::searchAbreviations 
            and mty::option::displayAbbreviations)
        DisplayAbbreviations();
    size_t i = 0;
    for (const auto& rule : rules) {
        out << "(" << i++ << ") : ";
        out << rule << std::endl;
    }
}

void Show(std::vector<FeynmanRule> const& rules)
{
    std::vector<std::shared_ptr<wick::Graph>> graphs(rules.size());
    for (size_t i = 0; i != rules.size(); ++i)
        graphs[i] = rules[i].getDiagram();
    Show(graphs);
}

void Display(std::vector<csl::Expr> const& amplitudes,
             std::ostream           & out,
             bool                     simplify)
{
    if (mty::option::searchAbreviations 
            and mty::option::displayAbbreviations)
        DisplayAbbreviations();
    if (simplify) {
        csl::Comparator::setDummyVecIntComparisonActive(true);
        csl::Expr sum = csl::sum_s(amplitudes);
        sum = csl::DeepRefreshed(sum);
        csl::Comparator::setDummyVecIntComparisonActive(false);
        if (sum->getType() == csl::Type::Sum)
            Display(sum->getVectorArgument(), out, false);
        else
            Display({sum}, out, false);
        return;
    }
    size_t i = 0;
    for (auto& A : amplitudes) {
        out << i++ << "  :  ";
        if (A->getType() == csl::Type::Sum) {
            for (int i = 0; i != A->getNArgs(); ++i) {
                A->getArgument(i)->print();
                if (i != A->getNArgs()-1)
                    out << " + ";
            }
        }
        else
            A->print();
        out << "\n\n";
    }
    out << "Total : " << amplitudes.size() << " particle amplitudes.\n";
}

void Display(mty::Amplitude const& amplitudes,
             std::ostream        & out,
             bool                  simplify)
{
    Display(amplitudes.obtainExpressions(), out, simplify);
}

void Show(std::vector<std::shared_ptr<wick::Graph>> const& graphs)
{
    Drawer::launchViewer(graphs);
}

void Show(mty::Amplitude const& ampl)
{
    Drawer::launchViewer(ampl.obtainGraphs());
}

void Display(WilsonSet const& wilsons,
             std::ostream   & out)
{
    if (mty::option::searchAbreviations 
            and mty::option::displayAbbreviations)
        DisplayAbbreviations();
    out << wilsons.size() << " wilsons:" << std::endl;
    for (size_t i = 0; i != wilsons.size(); ++i)
        out << "(" << i << ") : " << wilsons[i] << std::endl;
}

void DisplayAbbreviations(std::ostream &out)
{
    csl::Abbrev::printAbbreviations(out);
}

void DisplayAbbreviations(
        std::string const &name,
        std::ostream &out)
{
    csl::Abbrev::printAbbreviations(name, out);
}

void ExportPDF(std::string                 const& nameFiles,
               std::vector<std::shared_ptr<wick::Graph>> const& graphs)
{
    mty::Drawer::exportPNG(nameFiles, graphs, "diagrams/PDF");
}

void ExportPNG(std::string                 const& nameFiles,
               std::vector<std::shared_ptr<wick::Graph>> const& graphs)
{
    mty::Drawer::exportPNG(nameFiles, graphs, "diagrams/PNG");
}

void ExportPDF(std::string              const& nameFiles,
               std::vector<FeynmanRule> const& rules)
{
    std::vector<std::shared_ptr<wick::Graph>> graphs(rules.size());
    std::transform(rules.begin(), rules.end(), graphs.begin(),
            [](FeynmanRule const& rule) { return rule.getDiagram(); });
    mty::Drawer::exportPNG(nameFiles, graphs, "diagrams/PDF");
}

void ExportPNG(std::string              const& nameFiles,
               std::vector<FeynmanRule> const& rules)
{
    std::vector<std::shared_ptr<wick::Graph>> graphs(rules.size());
    std::transform(rules.begin(), rules.end(), graphs.begin(),
            [](FeynmanRule const& rule) { return rule.getDiagram(); });
    mty::Drawer::exportPNG(nameFiles, graphs, "diagrams/PNG");
}

///////////////////////////////////////////////////
/*************************************************/
// Model building utilities                      //
/*************************************************/
///////////////////////////////////////////////////

void AddParticle(mty::Model         & model,
              mty::Particle      & particle,
              bool                 initTerms)
{
    model.addParticle(particle, initTerms);
}

void AddTerm(mty::Model& model,
             csl::Expr const& term,
             bool addCC)
{
    model.addLagrangianTerm(term, addCC);
}

void AddTerm(mty::Model&              model,
             std::vector<csl::Expr> const& terms,
             bool                     addCC)
{
    for (const auto& term : terms)
        AddTerm(model, term, addCC);
}

mty::Particle GetParticle(mty::Model  const& model,
                          std::string const& name)
{
    return model.getParticle(name);
}

csl::Expr GetMass(mty::Particle const& particle)
{
    return particle->getMass();
}

mty::Particle GenerateSimilarParticle(std::string   const& name,
                                      mty::Particle const& particle)
{
    return particle->generateSimilar(name);
}

mty::Particle GetFieldStrength(Particle const& particle)
{
    HEPAssert(particle->hasFieldStrength(),
            mty::error::TypeError,
            "Particle " + std::string(particle->getName()) + " has no field "
            + "strength.");
    return particle->getFieldStrength();
}

void PromoteGoldstone(mty::Model   & model,
                      mty::Particle& goldstone,
                      mty::Particle& gaugeBoson)
{
    model.promoteToGoldstone(goldstone, gaugeBoson);
}

void PromoteGoldstone(mty::Model       & model,
                      char        const* goldstone,
                      char        const* gaugeBoson)
{
    model.promoteToGoldstone(std::string(goldstone),
                             std::string(gaugeBoson));
}

csl::Expr GetCoupling(mty::Model  const& model,
                 std::string const& nameCoupling)
{
    return model.getScalarCoupling(nameCoupling);
}

csl::Tensor GetYukawa(mty::Model const& model,
                      std::string const& nameCoupling)
{
    return model.getTensorCoupling(nameCoupling);
}

void Rename(mty::Model       & model,
            std::string const& oldName,
            std::string const& newName)
{
    model.renameParticle(oldName, newName);
}

void SetSelfConjugate(Particle & part,
                      bool       selfConjugate)
{
    part->setSelfConjugate(selfConjugate);
}

void SetGroupRep(mty::Particle         & part,
                 std::string      const& nameGroup,
                 std::vector<int> const& highestWeight
                 )
{
    part->setGroupRep(nameGroup, highestWeight);
}
void SetGroupRep(mty::Particle         & part,
                 std::string      const& nameGroup,
                 int                     charge
                 )
{
    part->setGroupRep(nameGroup, charge);
}
void SetFlavorRep(mty::Particle &part,
                  std::string const &nameGroup)
{
    part->setFundamentalFlavorRep(nameGroup);
}

void Init(mty::Model & model)
{
    model.init();
}

void Replaced(mty::Model& model,
             csl::Expr const& init,
             csl::Expr const& target)
{
    model.replace(init, target);
}

void Replaced(mty::Model  & model,
             csl::Tensor& init,
             csl::Expr   const& target)
{
    model.replace(init, target);
}

void Replaced(mty::Model  & model,
             csl::Tensor& init,
             csl::Tensor& target)
{
    model.replace(init, target);
}

void Replaced(mty::Model                 & model,
             Particle              const& particle,
             csl::Expr                  const& newTerm)
{
    model.replace(particle, newTerm);
}

void Rotate(mty::Model & model,
            std::vector<mty::Particle> const& fields,
            std::vector<mty::Particle> const& newFields,
            std::vector<std::vector<csl::Expr>> const& rotation,
            bool diagonalizeMasses)
{
    model.rotateFields(fields, newFields, rotation, diagonalizeMasses);
}

void Rotate(mty::Model & model,
            std::vector<std::string>       const& fieldNames,
            std::vector<std::string>       const& newFieldNames,
            std::vector<std::vector<csl::Expr>> const& rotation,
            bool diagonalizeMasses)
{
    std::vector<Particle> fields(fieldNames.size());
    for (size_t i = 0; i != fields.size(); ++i)
        fields[i] = GetParticle(model, fieldNames[i]);
    std::vector<Particle> newFields(newFieldNames.size());
    for (size_t i = 0; i != newFields.size(); ++i)
        newFields[i] = GetParticle(model, newFieldNames[i]);
    model.rotateFields(fields, newFields, rotation, diagonalizeMasses);
}

void DiagonalizeMassMatrices(mty::Model& model)
{
    model.diagonalizeMassMatrices();
    model.refresh();
}

void BreakGaugeSymmetry(
        mty::Model                                 & model,
        std::string                           const& brokenGroup,
        std::vector<Particle>                 const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames)
{
    model.breakGaugeSymmetry(brokenGroup, brokenFields, newNames);
}

void BreakGaugeSymmetry(
        mty::Model                                 & model,
        std::string                           const& brokenGroup,
        std::initializer_list<std::string>    const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames)
{
    std::vector<Particle> particle(brokenFields.size());
    for (size_t i = 0; i != particle.size(); ++i)
        particle[i] = GetParticle(model, *(brokenFields.begin()+i));
    BreakGaugeSymmetry(model, brokenGroup, particle, newNames);
}

void BreakFlavorSymmetry(
        mty::Model                                 & model,
        std::string                           const& brokenFlavor,
        std::initializer_list<std::string>    const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    std::vector<Particle> particles;
    particles.reserve(brokenFields.size());
    for (const auto& name : brokenFields)
        particles.push_back(GetParticle(model, name));
    BreakFlavorSymmetry(model, brokenFlavor, particles, newNames);
}

void BreakFlavorSymmetry(
        mty::Model                                 & model,
        std::string                           const& brokenFlavor,
        std::vector<mty::Particle>            const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    model.breakFlavorSymmetry(brokenFlavor, brokenFields, newNames);
}

void Refreshed(mty::Model & model)
{
    model.refresh();
}

void IntegrateOutParticle(
        mty::Particle & particle,
        bool            value)
{
    particle->integrateOut(value);
}

///////////////////////////////////////////////////
/*************************************************/
// Group theory                                  //
/*************************************************/
///////////////////////////////////////////////////

std::unique_ptr<mty::SemiSimpleAlgebra> CreateAlgebra(
        algebra::Type type,
        int                  l)
{
    switch(type) {
        case algebra::Type::R:
            return std::make_unique<mty::algebra::R>();
        case algebra::Type::A:
            HEPAssert(l > 0,
                    mty::error::ValueError,
                    "Algebra Al expects l > 0, " + toString(l) + " given.");
            return std::make_unique<mty::algebra::A>(l);
        case algebra::Type::B:
            HEPAssert(l > 0,
                    mty::error::ValueError,
                    "Algebra Bl expects l > 0, " + toString(l) + " given.");
            return std::make_unique<mty::algebra::B>(l);
        case algebra::Type::C:
            HEPAssert(l > 0,
                    mty::error::ValueError,
                    "Algebra Cl expects l > 0, " + toString(l) + " given.");
            return std::make_unique<mty::algebra::C>(l);
        case algebra::Type::D:
            HEPAssert(l > 1,
                    mty::error::ValueError,
                    "Algebra Dl expects l > 1, " + toString(l) + " given.");
            return std::make_unique<mty::algebra::D>(l);
        case algebra::Type::E6:
            return std::make_unique<mty::algebra::E6>();
        case algebra::Type::E7:
            return std::make_unique<mty::algebra::E7>();
        case algebra::Type::E8:
            return std::make_unique<mty::algebra::E8>();
        case algebra::Type::F4:
            return std::make_unique<mty::algebra::F4>();
        case algebra::Type::G2:
            return std::make_unique<mty::algebra::G2>();
    }
    return nullptr;
}

mty::Irrep GetIrrep(mty::SemiSimpleAlgebra const* algebra,
                    std::vector<int> const& labels)
{
    return algebra->highestWeightRep(labels);
}

mty::Irrep GetIrrep(std::unique_ptr<mty::SemiSimpleAlgebra> const& algebra,
                    std::vector<int> const& labels)
{
    return algebra->highestWeightRep(labels);
}

mty::Irrep GetConjugateRep(mty::Irrep const& rep)
{
    return rep.getConjugatedRep();
}

mty::SemiSimpleAlgebra const* GetAlgebraOf(mty::Irrep const& rep)
{
    return rep.getAlgebra();
}

int GetDimension(mty::Irrep const& irrep)
{
    return irrep.getDim();
}

bool ContainsIrrep(mty::SumIrrep const& decomposition,
                   mty::Irrep    const& irrep)
{
    for (const auto& rep : decomposition)
        if (rep == irrep)
            return true;
    return false;
}

bool ContainsTrivialrep(mty::SumIrrep const& decomposition)
{
    for (const auto& rep : decomposition)
        if (rep.getDim() == 1)
            return true;
    return false;
}

void SetGaugeChoice(mty::Model & model,
                    std::string const& nameParticle,
                    mty::gauge::Type choice)
{
    SetGaugeChoice(GetParticle(model, nameParticle), choice);
}

void SetGaugeChoice(mty::Particle particle,
                    mty::gauge::Type choice)
{
    particle->setGaugeChoice(choice);
}

std::string FindProcessName(
        std::string            const &initName,
        std::vector<Insertion> const &insertions
        )
{
    std::ostringstream sout;
    sout << initName << '_';
    for (const auto &ins : insertions)
        if (ins.isIncoming()) {
            sout << ins.getField()->getName();
            if (!ins.isParticle())
                sout << 'c';
            sout << '_';
        }
    sout << "to_";
    for (const auto &ins : insertions)
        if (!ins.isIncoming()) {
            sout << ins.getField()->getName();
            if (!ins.isParticle())
                sout << 'c';
            sout << '_';
        }
    std::string name = sout.str();
    name.erase(name.end() - 1); // erasing the last '_'
    return name;
}

}
