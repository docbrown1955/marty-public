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

#include "qcd.h"
#include "../core/mrtInterface.h"

namespace mty {

Particle colorTriplet(Model &QCD, std::string const &name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, csl::prefixedNameWithLatex("m_", name));
    SetGroupRep(part, "C", {1, 0});
    AddParticle(QCD, part);
    return part;
}

Particle colorSextet(Model &QCD, std::string const &name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, csl::prefixedNameWithLatex("m_", name));
    SetGroupRep(part, "C", {2, 0});
    AddParticle(QCD, part);
    return part;
}

QCD_Model::QCD_Model()
    : Model({GaugeGroupSpec{group::Type::SU,
                            3,
                            "C",
                            "G",
                            csl::constant_s("g")}})
{
    Particle u = colorTriplet(*this, "u");
    Particle d = colorTriplet(*this, "d");
    // Uncomment to have a color sextet X
    // Particle X = colorSextet(*this, "X");
    Particle gluon = getParticle("G");

    computeFeynmanRules();
}

} // namespace mty
