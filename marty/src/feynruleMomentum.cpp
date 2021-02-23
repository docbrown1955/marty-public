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

#include "feynruleMomentum.h"
#include "feynmanRule.h"

namespace mty {


FeynruleMomentum::FeynruleMomentum(
        std::vector<mty::FeynmanRule> const& rules,
        std::vector<csl::Tensor>      const& vertices)
{
    HEPAssert(rules.size() == vertices.size(),
            mty::error::ValueError,
            "There must be as many rules (" + toString(rules.size())
            + ") as vertices (" + toString(vertices.size()) + ").");

    size_t nKeys = 0;
    for (size_t i = 0; i != rules.size(); ++i) {
        nKeys += rules[i].getSize();
    }
    keys.reserve(nKeys);
    mapping.reserve(nKeys);
    for (size_t i = 0; i != rules.size(); ++i) {
        std::vector<mty::QuantumField> const& content 
            = rules[i].getFieldProduct();
        for (const auto& field : content) {
            keys.push_back({ field , vertices[i] });
            mapping.push_back(-1);
        }
    }
}

bool FeynruleMomentum::isEmpty() const
{
    return keys.empty();
}

size_t FeynruleMomentum::getSize() const
{
    return keys.size();
}

void FeynruleMomentum::clear()
{
    keys.clear();
    mapping.clear();
    targets.clear();
}

std::vector<FeynruleKey>& FeynruleMomentum::getKeys()
{
    return keys;
}

std::vector<FeynruleKey> const& FeynruleMomentum::getKeys() const
{
    return keys;
}

std::vector<FeynruleTarget>& FeynruleMomentum::getTargets()
{
    return targets;
}

std::vector<FeynruleTarget> const& FeynruleMomentum::getTargets() const
{
    return targets;
}

std::vector<int>& FeynruleMomentum::getMomentumMapping()
{
    return mapping;
}

std::vector<int> const& FeynruleMomentum::getMomentumMapping() const
{
    return mapping;
}

bool compareKey(FeynruleKey       const& key,
                mty::QuantumField const& field)
{
    if (key.point.get() != field.getPoint().get())
        return false;
    if (field.getParent_info() != key.field.getParent_info())
        return false;
    if (field.isComplexConjugate() != key.field.isComplexConjugate())
        return false;
    return true;
}

std::vector<FeynruleKey>::const_iterator FeynruleMomentum::find(
        mty::QuantumField const& field) const
{
    auto iter = keys.begin();
    do {
        if (compareKey(*iter, field))
            break;
    } while (++iter != keys.end());

    return iter;
}

std::vector<FeynruleKey>::iterator FeynruleMomentum::find(
        mty::QuantumField const& field)
{
    auto iter = keys.begin();
    do {
        if (compareKey(*iter, field))
            break;
    } while (++iter != keys.end());

    return iter;
}

std::vector<FeynruleTarget>::const_iterator FeynruleMomentum::find(
        FeynruleKey const& key) const
{
    auto iter = keys.begin();
    do {
        if (*iter == key)
            break;
    } while (++iter != keys.end());
    if (iter != keys.end()) {
        size_t pos = std::distance(keys.begin(), iter);
        if (mapping[pos] != -1)
            return targets.begin() + pos;
    }
    return targets.end();
}

std::vector<FeynruleTarget>::iterator FeynruleMomentum::find(
        FeynruleKey const& key)
{
    auto iter = keys.begin();
    do {
        if (*iter == key)
            break;
    } while (++iter != keys.end());
    if (iter != keys.end()) {
        size_t pos = std::distance(keys.begin(), iter);
        if (mapping[pos] != -1)
            return targets.begin() + pos;
    }
    return targets.end();
}

std::vector<FeynruleTarget>::const_iterator FeynruleMomentum::find(
        csl::Tensor const& momentum) const
{
    auto iter = keys.begin();
    do {
        if (iter->field.getPoint().get() == momentum.get())
            break;
    } while (++iter != keys.end());
    if (iter != keys.end()) {
        size_t pos = std::distance(keys.begin(), iter);
        if (mapping[pos] != -1)
            return targets.begin() + mapping[pos];
    }
    return targets.end();
}

std::vector<FeynruleTarget>::iterator FeynruleMomentum::find(
        csl::Tensor const& momentum)
{
    auto iter = keys.begin();
    do {
        if (iter->field.getPoint().get() == momentum.get())
            break;
    } while (++iter != keys.end());
    if (iter != keys.end()) {
        size_t pos = std::distance(keys.begin(), iter);
        if (mapping[pos] != -1)
            return targets.begin() + mapping[pos];
    }
    return targets.end();
}

void FeynruleMomentum::push(mty::QuantumField const& field,
                            FeynruleTarget    const& target)
{
    auto iter = keys.begin();
    do {
        if (compareKey(*iter, field)) {
            int pos = std::distance(keys.begin(), iter);
            if (mapping[pos] == -1) {
                mapping[pos] = targets.size();
                targets.push_back(target);
                break;
            }
        }
    } while (++iter != keys.end());
}

std::optional<FeynruleTarget> FeynruleMomentum::pop(csl::Tensor momentum)
{
    auto target = find(momentum);
    if (target == targets.end())
        return std::nullopt;

    FeynruleTarget res = *target;
    int index = int(std::distance(targets.begin(), target));
    targets.erase(target);
    for (int& pos : mapping) {
        if (pos == index)
            pos = -1;
        else if (pos > index)
            --pos;
    }

    return res;
}

std::ostream& operator<<(std::ostream& out,
                         FeynruleMomentum const& mom)
{
    out << "Feynman rule mapping for:\n";
    for (size_t i = 0; i != mom.keys.size(); ++i) {
        out << "Key: ";
        out << mom.keys[i].field.printLaTeX(1);
        out << " , " << mom.keys[i].point->getName();
        if (mom.mapping[i] == -1)
            out << "  not mapped.";
        else {
            out << " mapped to ";
            if (mom.targets[mom.mapping[i]].factor != CSL_1)
                out << mom.targets[mom.mapping[i]].factor->printLaTeX(1);
            out << mom.targets[mom.mapping[i]].momentum->getName();
            out << "(" << mom.targets[mom.mapping[i]].momentum << ")";
        }
        out << '\n';
    }

    return out;
}

}


