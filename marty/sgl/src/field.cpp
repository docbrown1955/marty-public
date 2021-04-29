#include "field.h"
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
