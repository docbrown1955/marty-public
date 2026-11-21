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

#include "marty/api/common/error.h"
#include "marty/api/group_theory/group_theory.h"
#include "marty/api/modeling/modeling.h"
#include "marty/core/mrtInterface.h"
#include "marty/models/qed.h"
#include "marty/models/sm.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

namespace gt  = mty::api::group_theory;
namespace mod = mty::api::modeling;

namespace {

mod::Model makeToy()
{
    return mod::Model(std::vector<mod::GaugeGroup>{
        {gt::Group::su(2), "L", "W"},
        {gt::Group::u1(), "Y", "B"},
    });
}

} // namespace

TEST(ModelingApi, UnknownParticleIsLookupError)
{
    auto toy = makeToy();
    EXPECT_FALSE(toy.contains("no_such_particle"));
    EXPECT_THROW(toy.particle("no_such_particle"), mty::api::LookupError);
}

TEST(ModelingApi, ChargeOnNonU1GroupIsTypeError)
{
    auto toy = makeToy();
    auto q   = toy.weylFermion("Q_L", "L");
    EXPECT_THROW(q.setCharge("L", 1, 6), mty::api::TypeError);
    EXPECT_NO_THROW(q.setCharge("Y", 1, 6));
}

TEST(ModelingApi, LabelsOnU1GroupIsTypeError)
{
    auto toy = makeToy();
    auto q   = toy.weylFermion("Q_L", "L");
    EXPECT_THROW(q.setLabels("Y", {1}), mty::api::TypeError);
}

TEST(ModelingApi, CallsRestoreCurrentModel)
{
    auto        toy    = makeToy();
    auto const *before = mty::Model::current;
    auto        q      = toy.weylFermion("Q_L", "L");
    q.setCharge("Y", 1, 6);
    toy.contains("Q_L");
    EXPECT_THROW(toy.particle("no_such_particle"), mty::api::LookupError);
    EXPECT_EQ(mty::Model::current, before);
}

TEST(ModelingApi, RenameParticleChecksNames)
{
    auto toy = makeToy();
    toy.addParticle(toy.weylFermion("Q_L", "L"));
    toy.addParticle(toy.diracFermion("E"));
    EXPECT_THROW(toy.renameParticle("no_such_particle", "X"),
                 mty::api::LookupError);
    EXPECT_THROW(toy.renameParticle("Q_L", "E"), mty::api::ValueError);
    toy.renameParticle("Q_L", "Q");
    EXPECT_TRUE(toy.contains("Q"));
    EXPECT_FALSE(toy.contains("Q_L"));
    EXPECT_EQ(toy.particle("Q").name(), "Q");
}

TEST(ModelingApi, FlavorGroupNeedsAFlavor)
{
    std::vector<mod::GaugeGroup> gauge{{gt::Group::su(2), "L", "W"}};
    EXPECT_THROW(mod::Model(gauge, {mod::FlavorGroup{"F", 0}}),
                 mty::api::ValueError);
    EXPECT_THROW(mod::Model(gauge, {mod::FlavorGroup{"L", 3}}),
                 mty::api::ValueError);
    EXPECT_NO_THROW(mod::Model(gauge, {mod::FlavorGroup{"F", 3}}));
}

TEST(ModelingApi, ParticleFactoriesAndRefresh)
{
    auto toy = makeToy();
    toy.addParticle(toy.scalarBoson("phi"));
    toy.addParticle(toy.majoranaFermion("chi"));
    EXPECT_TRUE(toy.contains("phi"));
    EXPECT_TRUE(toy.contains("chi"));
    EXPECT_NO_THROW(toy.refresh());
}

TEST(ModelingApi, CouplingIsCreatedThenFound)
{
    auto toy = makeToy();
    EXPECT_THROW(toy.coupling("g"), mty::api::LookupError);
    auto created = toy.newCoupling("g");
    EXPECT_EQ(toy.coupling("g"), created);
}

TEST(ModelingApi, MassAndRotationRejectInconsistentInput)
{
    auto toy = makeToy();
    toy.addParticle(toy.scalarBoson("phi"));
    toy.addParticle(toy.weylFermion("Q_L", "L"));
    toy.addParticle(toy.weylFermion("Q_R", "R"));
    auto phi = toy.particle("phi");
    auto qL  = toy.particle("Q_L");
    auto qR  = toy.particle("Q_R");
    EXPECT_NO_THROW(toy.addBosonicMass(phi, toy.newCoupling("m")));
    EXPECT_NO_THROW(toy.addFermionicMass(qL, qR, toy.newCoupling("y")));
    EXPECT_THROW(toy.rotateFields({phi}, {phi, qL}, {{}}), mty::api::Error);
}

TEST(ModelingApi, UnknownChiralityIsValueError)
{
    auto toy = makeToy();
    EXPECT_THROW(toy.weylFermion("Q_L", "X"), mty::api::ValueError);
}

namespace {

std::vector<std::string> facadeNames(mod::Model const &model)
{
    std::vector<std::string> names;
    for (auto const &part : model.particles())
        names.push_back(part.name());
    return names;
}

std::vector<std::string> coreNames(mty::Model const &model)
{
    std::vector<std::string> names;
    for (auto const &part : model.getParticles())
        names.push_back(part->getName());
    return names;
}

std::vector<std::string> ruleHeaders(std::string const &document)
{
    std::vector<std::string> headers;
    std::istringstream       lines(document);
    for (std::string line; std::getline(lines, line);)
        if (line.rfind("Feynman vertex for ", 0) == 0)
            headers.push_back(line);
    return headers;
}

std::size_t countOccurrences(std::string const &text, std::string const &word)
{
    std::size_t count = 0;
    for (auto pos = text.find(word); pos != std::string::npos;
         pos      = text.find(word, pos + word.size()))
        ++count;
    return count;
}

} // namespace

TEST(ModelingApi, QedMatchesCoreParticlesAndRules)
{
    mty::QED_Model core;
    auto           model = mod::Model::qed();
    EXPECT_EQ(facadeNames(model), coreNames(core));
    EXPECT_EQ(model.feynmanRules().size(), core.getFeynmanRules().size());
}

TEST(ModelingApi, StandardModelMatchesCoreParticlesAndRules)
{
    mty::SM_Model core;
    auto          model = mod::Model::standardModel();
    EXPECT_EQ(facadeNames(model), coreNames(core));
    EXPECT_EQ(model.feynmanRules().size(), core.getFeynmanRules().size());
}

TEST(ModelingApi, LatexMatchesExistingTests)
{
    mty::QED_Model core;
    auto           rules     = mod::Model::qed().feynmanRules();
    auto const    &coreRules = core.getFeynmanRules();
    ASSERT_EQ(rules.size(), coreRules.size());

    // The drawing layout (label angles, offsets) differs between two core
    // instances, so the comparison covers the headers and the vertices only.
    EXPECT_EQ(ruleHeaders(rules.latexDocument()),
              ruleHeaders(mty::LatexFeynmanRules(coreRules)));
    for (std::size_t i = 0; i < rules.size(); ++i)
        EXPECT_EQ(
            countOccurrences(rules.latex(i), "\\vertex"),
            countOccurrences(mty::LatexFeynmanRule(coreRules[i]), "\\vertex"));
}

TEST(ModelingApi, FeynmanRuleIndexOutOfRangeIsLookupError)
{
    auto rules = mod::Model::qed().feynmanRules();
    EXPECT_THROW(rules.str(rules.size()), mty::api::LookupError);
    EXPECT_THROW(rules.latex(rules.size()), mty::api::LookupError);
}
