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

/**
 * @file sglfield.h
 * @brief SGL symbolic expression representing quantum fields, in particular
 * external spinors.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

#include "multifunction.h"

namespace sgl {

struct PartnerShip {
    int  self{-1};
    int  other{-1};
    bool isLeft{false};
};

class Field : public AbstractLiteral {

  public:
    enum Order { None, Left, Right };

    Field(csl::Expr const &  field,
          bool               particle,
          bool               incoming,
          bool               onShell,
          PartnerShip const &partnerShip,
          Order              order = None);

    bool contains(csl::Index const &) const override
    {
        return false;
    }
    void replace(csl::Index const &, csl::Index const &) override
    {
    }

    csl::Index getIndex() const
    {
        return index;
    }

    bool isOnShell() const
    {
        return onshell;
    }
    bool isIncomingParticle() const
    {
        return incoming && particle;
    }
    bool isIncomingAntiParticle() const
    {
        return incoming && !particle;
    }
    bool isOutgoingParticle() const
    {
        return !incoming && particle;
    }
    bool isOutgoingAntiParticle() const
    {
        return !incoming && !particle;
    }

    bool isComplexConjugated() const
    {
        return isIncomingAntiParticle() || isOutgoingParticle();
    }

    bool isZero() const override
    {
        return false;
    }

    bool isHappyWith(Field const &other) const
    {
        return partnerShip.self == -1
               || other.partnerShip.self == partnerShip.other;
    }

    bool isLeftField() const
    {
        return partnerShip.isLeft;
    }

    void conjugate();

    csl::Expr toCSL(TensorSet const &tensors) const override;

    csl::Expr toCSL(TensorSet const &tensors, csl::Index index) const;

    GExpr copy() const override;
    GExpr refresh() const override;

    void print(std::ostream &out = std::cout) const override;

    bool isSame(Field const &other) const;

  public:
    csl::Expr        initField;
    csl::Index       index;
    csl::TensorField psi;
    csl::Tensor      p;
    bool             particle;
    bool             incoming;
    bool             onshell;
    Order            order;
    PartnerShip      partnerShip;
};

template <class... Args>
std::shared_ptr<Field> field_s(Args &&... args)
{
    return std::make_shared<Field>(std::forward<Args>(args)...);
}
} // namespace sgl
