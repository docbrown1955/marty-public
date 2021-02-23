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
    expression(std::nullopt),
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
    expression = t_field;
    incoming = field_ptr->isIncoming();
    particle = field_ptr->isParticle();
}

Insertion::Insertion(std::string_view name)
    :Insertion(GetParticle(&name[0]).get())
{

}

Insertion::Insertion(const char name[])
    :Insertion(GetParticle(name).get())
{

}

QuantumFieldParent const *Insertion::getField() const {
    return field;
}

void Insertion::setField(QuantumFieldParent *t_field) {
    field = t_field;
}

bool Insertion::isIncoming() const
{
    return incoming;
}

void Insertion::setIncoming(bool t_incoming)
{
    incoming = t_incoming;
}

bool Insertion::isParticle() const
{
    return particle;
}

void Insertion::setParticle(bool t_particle)
{
    particle = t_particle;
}

bool Insertion::isOnShell() const
{
    return onShell;
}

void Insertion::setOnShell(bool t_onShell)
{
    onShell = t_onShell;
}

csl::Expr Insertion::getExpression() const
{
    if (expression)
        return expression.value();
    csl::Expr insertion = field->getInstance();
    QuantumField *insert_ptr = ConvertToPtr<QuantumField>(insertion);
    insert_ptr->setExternal(true);
    insert_ptr->setIncoming(incoming);
    insert_ptr->setParticle(particle);
    expression = insertion;

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
