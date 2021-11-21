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

#include "sglfield.h"
#include "exceptions.h"
#include "polarization.h"

namespace sgl {

    Field::Field(
            csl::Expr const &field,
            bool             t_particle,
            bool             t_incoming,
            bool             t_onshell,
            PartnerShip const &t_partnerShip,
            Order            t_order
            )
        :initField(csl::DeepCopy(field)),
        particle(t_particle),
        incoming(t_incoming),
        onshell(t_onshell),
        order(t_order),
        partnerShip(t_partnerShip)
    {
        auto tfield = dynamic_cast<csl::TensorFieldElement const*>(field.get());
        if (!tfield)
            throw Exception::TypeError;
        index = tfield->getIndexStructureView().back();
        psi = std::dynamic_pointer_cast<csl::TensorFieldParent>(
                tfield->getParent()
                );
        p = tfield->getPoint();
    }

    csl::Expr Field::toCSL(TensorSet const &) const
    {
        return initField;
    }

    csl::Expr Field::toCSL(TensorSet const &tensors, csl::Index index) const
    {
        csl::Expr res = toCSL(tensors);
        return csl::Replaced(
                res,
                res->getIndexStructureView().back(),
                index
                );
    }

    GExpr Field::copy() const
    {
        return field_s(
                initField, particle, incoming, onshell, partnerShip, order
                );
    }

    GExpr Field::refresh() const 
    {
        return copy();
    }

    void Field::print(std::ostream &out) const
    {
        out << psi->getName() << "(" << p->getName() << ", ";
        out << ((incoming) ? "->" : "<-") << ", ";
        out << ((particle) ? "+" : "-");
        out << ")";
    }

    bool Field::isSame(Field const &other) const
    {
        return psi.get() == other.psi.get()
            && p.get() == other.p.get()
            && incoming == other.incoming
            && onshell  == other.onshell
            && particle == other.particle;
    }

    void Field::conjugate()
    {
        particle = !particle;
        dynamic_cast<mty::QuantumField*>(initField.get())
            ->setParticle(particle);
    }

}
