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

#include "insertion.h"
#include "quantumField.h"
#include "mrtInterfaceCurrent.h"

namespace mty {

Insertion::Insertion(QuantumFieldParent *t_field)
    :field(t_field),
    validExpression(false),
    expression(CSL_UNDEF),
    incoming(true),
    particle(true),
    onShell(true)
{

}

Insertion::Insertion(Particle t_field)
    :Insertion(t_field.get())
{

}

Insertion::Insertion(csl::Expr const& t_field)
{
    HEPAssert(IsOfType<QuantumField>(t_field),
            mty::error::TypeError,
            "Building insertion from " + toString(t_field) + " that is not"
            + " a quantum field.");
    QuantumField *field_ptr = ConvertToPtr<QuantumField>(t_field);
    field = field_ptr->getQuantumParent();
    expression  = t_field;
    incoming    = field_ptr->isIncoming();
    particle    = field_ptr->isParticle();
    onShell     = field_ptr->isOnShell();
    partnerShip = field_ptr->getPartnerShip();
    validExpression = true;
}

Insertion::Insertion(std::string_view name)
    :Insertion(GetParticle(&name[0]).get())
{

}

Insertion::Insertion(const char name[])
    :Insertion(GetParticle(name).get())
{

}

QuantumFieldParent *Insertion::getField() const {
    return field;
}

void Insertion::setField(QuantumFieldParent *t_field) {
    validExpression = false;
    expression = CSL_UNDEF;
    field = t_field;
}

void Insertion::setIncoming(bool t_incoming)
{
    validExpression = false;
    incoming = t_incoming;
}

void Insertion::setParticle(bool t_particle)
{
    validExpression = false;
    particle = t_particle;
}

void Insertion::setOnShell(bool t_onShell)
{
    validExpression = false;
    onShell = t_onShell;
}

void Insertion::setPartnerShip(PartnerShip const &t_partnerShip)
{
    validExpression = false;
    partnerShip = t_partnerShip;
}

csl::Expr Insertion::getExpression() const
{
    if (validExpression)
        return expression;
    csl::Expr insertion = (expression == CSL_UNDEF) ? 
        field->getInstance() : expression;
    QuantumField *insert_ptr = ConvertToPtr<QuantumField>(insertion);
    insert_ptr->setExternal(true);
    insert_ptr->setIncoming(incoming);
    insert_ptr->setParticle(particle);
    insert_ptr->setOnShell(onShell);
    insert_ptr->setPartnerShip(partnerShip);
    expression = insertion;
    validExpression = true;

    return insertion;
}

Insertion OnShell(Insertion const& init)
{
    Insertion other(init);
    other.setOnShell(true);
    return other;
}

Insertion OffShell(Insertion const& init)
{
    Insertion other(init);
    other.setOnShell(false);
    return other;
}

Insertion Incoming(Insertion const& init)
{
    Insertion other(init);
    other.setIncoming(true);
    return other;
}

Insertion Outgoing(Insertion const& init)
{
    Insertion other(init);
    other.setIncoming(false);
    return other;
}

Insertion AntiPart(Insertion const& init)
{
    Insertion other(init);
    other.setParticle(!init.isParticle());
    return other;
}

Insertion Left(Insertion const &init)
{
    const auto left = init.getField()->getWeylFermion(Chirality::Left);
    HEPAssert(left,
            mty::error::TypeError,
            "Expecting a Dirac fermion for Left-handed insertion, " 
            + toString(init.getExpression()) + " given instead.")
    Insertion other(init);
    other.setField(left.get());
    return other;
}

Insertion Right(Insertion const &init)
{
    const auto right = init.getField()->getWeylFermion(Chirality::Right);
    HEPAssert(right,
            mty::error::TypeError,
            "Expecting a Dirac fermion for Right-handed insertion, " 
            + toString(init.getExpression()) + " given instead.")
    Insertion other(init);
    other.setField(right.get());
    return other;
}

std::vector<Insertion> AntiPart(std::vector<Insertion> const &init)
{
    auto res {init};
    for (auto &ins : res)
        ins = AntiPart(ins);
    return res;
}

csl::Expr GetExpression(Insertion const& init)
{
    return init.getExpression();
}

std::vector<csl::Expr> GetExpression(std::vector<Insertion> const& insertions)
{
    std::vector<csl::Expr> res;
    res.reserve(insertions.size());
    for (const auto& i : insertions)
        res.emplace_back(i.getExpression());
    return res;
}

std::vector<Insertion> GetInsertion(std::vector<csl::Expr> const& insertions)
{
    std::vector<Insertion> res;
    res.reserve(insertions.size());
    for (const auto& i : insertions)
        res.emplace_back(Insertion(i));
    return res;
}

}
