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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef INSERTION_H_INCLUDED
#define INSERTION_H_INCLUDED

#include "../../csl/csl.h"
#include "quantumField.h"

namespace mty {

class Particle;

class Insertion {

  private:
    mutable QuantumFieldParent *field;
    mutable bool                validExpression{false};
    mutable csl::Expr           expression{CSL_UNDEF};
    bool                        incoming{true};
    bool                        particle{true};
    bool                        onShell{true};
    bool                        mediator{false};
    PartnerShip                 partnerShip;

  public:
    Insertion(QuantumFieldParent *t_field);
    Insertion(Particle t_field);
    Insertion(csl::Expr const &t_field);
    Insertion(std::string const &name);
    Insertion(std::string_view name);
    Insertion(const char name[]);
    Insertion(Insertion const &other) = default;
    Insertion(Insertion &&other)      = default;
    Insertion &operator=(Insertion const &other) = default;
    Insertion &operator=(Insertion &&other) = default;
    ~Insertion()                            = default;

    QuantumFieldParent *getField() const;
    void                setField(QuantumFieldParent *t_field);

    bool isIncoming() const
    {
        return incoming;
    }
    void setIncoming(bool t_incoming);

    bool isParticle() const
    {
        return particle;
    }
    void setParticle(bool t_particle);

    bool isIncomingParticle() const
    {
        return isIncoming() && isParticle();
    }
    bool isIncomingAntiParticle() const
    {
        return isIncoming() && !isParticle();
    }
    bool isOutgoingParticle() const
    {
        return !isIncoming() && isParticle();
    }
    bool isOutgoingAntiParticle() const
    {
        return !isIncoming() && !isParticle();
    }

    void setPartnerShip(PartnerShip const &t_partnerShip);

    bool isOnShell() const
    {
        return onShell;
    }
    void setOnShell(bool t_onShell);

    bool isMediator() const
    {
        return mediator;
    }
    void setMediator(bool t_mediator)
    {
        mediator = t_mediator;
        if (mediator)
            setOnShell(false);
    }

    csl::Expr getExpression() const;

    bool isEquivalent(Insertion const &other) const;

    bool operator==(Insertion const &other) const;

    bool operator!=(Insertion const &other) const
    {
        return !(*this == other);
    }
};

Insertion              OnShell(Insertion const &init);
Insertion              OffShell(Insertion const &init);
Insertion              Incoming(Insertion const &init);
Insertion              Outgoing(Insertion const &init);
Insertion              AntiPart(Insertion const &init);
Insertion              Mediator(Insertion const &init);
Insertion              Left(Insertion const &init);
Insertion              Right(Insertion const &init);
std::vector<Insertion> AntiPart(std::vector<Insertion> const &init);
csl::Expr              GetExpression(Insertion const &init);
std::vector<csl::Expr> GetExpression(std::vector<Insertion> const &insertions);
std::vector<Insertion> GetInsertion(std::vector<csl::Expr> const &insertions);

} // namespace mty

#endif
