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

#include "qed.h"
#include "../core/fermionicField.h"
#include "../core/quantumFieldTheory.h"
#include "sm.h"

namespace mty {

QED_Model::QED_Model() : Model()
{
    csl::Expr e = csl::constant_s("e");
    addGaugedGroup(group::Type::U1, "em", e);
    init();

    Particle psi = diracfermion_s("\\psi", *this);
    auto     m   = csl::constant_s("m");
    psi->setGroupRep("em", -1);
    psi->setMass(m);
    addParticle(psi);
    renameParticle("A_em", "A");

    computeFeynmanRules();
}
} // namespace mty
