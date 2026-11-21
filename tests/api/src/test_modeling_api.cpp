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

#include <gtest/gtest.h>

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

TEST(ModelingApi, UnknownChiralityIsValueError)
{
    auto toy = makeToy();
    EXPECT_THROW(toy.weylFermion("Q_L", "X"), mty::api::ValueError);
}
