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
 * @version 1.3

 * \brief
 */
#ifndef FEYNRULEMOMENTUM_H_INCLUDED
#define FEYNRULEMOMENTUM_H_INCLUDED

#include "../../csl/csl.h"
#include "quantumField.h"

namespace mty {

class FeynmanRule;

struct FeynruleKey {
    mty::QuantumField field;
    csl::Tensor       point;

    bool operator==(FeynruleKey const &other) const
    {
        return field.getParent_info() == other.field.getParent_info()
               and field.isComplexConjugate()
                       == other.field.isComplexConjugate()
               and point.get() == other.point.get();
    }

    bool operator<(FeynruleKey const &other) const
    {
        int comp = csl::compare(point->getName(), other.point->getName());
        if (comp == -1)
            return true;
        else if (comp == 0)
            return field.TensorFieldElement::operator<(&other.field);
        return false;
    }
};

struct FeynruleTarget {
    csl::Expr   factor = CSL_1;
    csl::Tensor momentum;

    FeynruleTarget() : momentum(nullptr)
    {
    }
    FeynruleTarget(csl::Tensor const &m) : momentum(m)
    {
    }
    FeynruleTarget(csl::Expr const &f, csl::Tensor const &m)
        : factor(f), momentum(m)
    {
    }

    bool operator==(FeynruleTarget const &other) const
    {
        return momentum.get() == other.momentum.get()
               and factor == other.factor;
    }
};

class FeynruleMomentum {

  public:
    FeynruleMomentum(std::vector<mty::FeynmanRule> const &rules,
                     std::vector<csl::Tensor> const &     vertices);

    FeynruleMomentum()                              = default;
    FeynruleMomentum(FeynruleMomentum const &other) = default;
    FeynruleMomentum(FeynruleMomentum &&other)      = default;
    FeynruleMomentum &operator=(FeynruleMomentum const &other) = default;
    FeynruleMomentum &operator=(FeynruleMomentum &&other) = default;
    ~FeynruleMomentum()                                   = default;

    bool   isEmpty() const;
    size_t getSize() const;
    void   clear();

    std::vector<FeynruleKey> &      getKeys();
    std::vector<FeynruleKey> const &getKeys() const;

    std::vector<FeynruleTarget> &      getTargets();
    std::vector<FeynruleTarget> const &getTargets() const;

    std::vector<int> &      getMomentumMapping();
    std::vector<int> const &getMomentumMapping() const;

    std::vector<FeynruleKey>::iterator find(mty::QuantumField const &field);

    std::vector<FeynruleKey>::const_iterator
    find(mty::QuantumField const &field) const;

    std::vector<FeynruleTarget>::const_iterator
    find(FeynruleKey const &key) const;

    std::vector<FeynruleTarget>::iterator find(FeynruleKey const &key);

    std::vector<FeynruleTarget>::const_iterator
    find(csl::Tensor const &momentum) const;

    std::vector<FeynruleTarget>::iterator find(csl::Tensor const &momentum);

    void push(mty::QuantumField const &field, FeynruleTarget const &target);

    std::optional<FeynruleTarget> pop(csl::Tensor momentum);

    friend std::ostream &operator<<(std::ostream &          out,
                                    FeynruleMomentum const &mom);

  private:
    std::vector<FeynruleKey> keys;

    std::vector<FeynruleTarget> targets;

    std::vector<int> mapping;
};

} // namespace mty

#endif
