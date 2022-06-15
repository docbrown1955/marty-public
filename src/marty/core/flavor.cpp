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

#include "flavor.h"
#include "group.h"

using namespace std;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class Flavor                                  //
/*************************************************/
///////////////////////////////////////////////////

Flavor::~Flavor()
{
    for (size_t i = 0; i != flavors.size(); ++i)
        if (flavors[i])
            delete flavors[i];
}

///////////////////////////////////////////////////
/*************************************************/
// Class FlavorGroup                             //
/*************************************************/
///////////////////////////////////////////////////

FlavorGroup::FlavorGroup(string const &t_name,
                         int           nFlavors,
                         bool          complexFields)
    : name(t_name)
{
    if (complexFields) {
        group = make_unique<mty::group::SU>(nFlavors);
    }
    else {
        group = make_unique<mty::group::SO>(nFlavors);
    }
}

size_t FlavorGroup::getDim() const
{
    return getVectorSpace(getFundamentalRep())->getDim();
}

string_view FlavorGroup::getName() const
{
    return name;
}

csl::Index FlavorGroup::getCorrespondingIndex(const string &index_name,
                                              const QuantumFieldParent &parent)
{
    return group->getCorrespondingIndex(index_name, parent);
}

const csl::Space *FlavorGroup::getVectorSpace(const Irrep &irrep) const
{
    return group->getVectorSpace(irrep);
}

const csl::Space *FlavorGroup::getFundamentalSpace() const
{
    return group->getVectorSpace(getFundamentalRep());
}

Irrep FlavorGroup::getTrivialRep() const
{
    std::vector<int> labels(group->getAlgebra()->getOrderL(), 0);
    return group->highestWeightRep(labels);
}

Irrep FlavorGroup::getFundamentalRep() const
{
    std::vector<int> labels(group->getAlgebra()->getOrderL(), 0);
    labels[0] = 1;
    return group->highestWeightRep(labels);
}

SemiSimpleGroup *FlavorGroup::getGroup() const
{
    return group.get();
}

bool FlavorGroup::isComplex() const
{
    return group->getType() == group::Type::SU;
}

///////////////////////////////////////////////////
/*************************************************/
// Class Flavor                                  //
/*************************************************/
///////////////////////////////////////////////////

FlavorIrrep Flavor::getTrivialRep() const
{
    FlavorIrrep rep(this);
    for (auto &r : rep)
        r = r.getAlgebra()->getTrivialRep();
    return rep;
}

FlavorIrrep Flavor::getFundamentalRep() const
{
    FlavorIrrep rep(this);
    for (auto &r : rep) {
        auto             algebra = r.getAlgebra();
        std::vector<int> labels(algebra->getOrderL(), 0);
        labels[0] = 1;
        r         = r.getAlgebra()->highestWeightRep(labels);
    }
    return rep;
}

///////////////////////////////////////////////////
/*************************************************/
// Class FlavorIrrep                             //
/*************************************************/
///////////////////////////////////////////////////

FlavorIrrep::FlavorIrrep(Flavor const *t_flavors) : flavors(t_flavors)
{
    if (flavors) {
        rep.reserve(flavors->size());
        for (const auto &f : *flavors)
            rep.push_back(f->getGroup()->getTrivialRep());
    }
}

FlavorIrrep::FlavorIrrep(Flavor const *t_flavors, FlavorIrrep const &other)
    : flavors(t_flavors)
{
    HEPAssert(other.size() <= flavors->size(),
              mty::error::ValueError,
              "Copying a flavor representation in a smaller FlavorGroup set.");
    rep.reserve(flavors->size());
    size_t i = 0;
    for (const auto &r : other) {
        HEPAssert((*flavors)[i++]->getGroup()->getAlgebra() == r.getAlgebra(),
                  mty::error::ValueError,
                  "Flavor groups do not match in flavor replacement");
        rep.push_back(r);
    }
    for (; i != flavors->size(); ++i)
        rep.push_back((*flavors)[i]->getGroup()->getTrivialRep());
}

const Flavor *FlavorIrrep::getFlavor() const
{
    return flavors;
}

void FlavorIrrep::setTrivialRepresentation(FlavorGroup const *flavorGroup)
{
    Irrep rep = flavorGroup->getGroup()->getTrivialRep();
    setRepresentation(flavorGroup, rep);
}

void FlavorIrrep::setFundamentalRepresentation(FlavorGroup const *flavorGroup)
{
    int         dim = flavorGroup->getGroup()->getAlgebra()->getOrderL();
    vector<int> labels(dim, 0);
    labels[0] = 1;
    Irrep rep = flavorGroup->getGroup()->highestWeightRep(labels);
    setRepresentation(flavorGroup, rep);
}

void FlavorIrrep::setRepresentation(FlavorGroup const *flavorGroup,
                                    Irrep const &      irrep)
{
    auto pos = find_if(
        flavors->begin(),
        flavors->end(),
        [&flavorGroup](FlavorGroup const *el) { return el == flavorGroup; });
    if (pos != flavors->end())
        rep[std::distance(flavors->begin(), pos)] = irrep;
}

bool FlavorIrrep::operator==(FlavorIrrep const &other) const
{
    if (flavors != other.flavors)
        return false;
    if (size() != other.size())
        return false;
    for (size_t i = 0; i != size(); ++i)
        if (rep[i] != other[i])
            return false;
    return true;
}

ostream &operator<<(ostream &fout, const FlavorIrrep &irrep)
{
    fout << "( ";
    for (size_t i = 0; i != irrep.size(); ++i) {
        if (irrep[i].getAlgebra()
            and irrep[i].getAlgebra()->getType() != algebra::Type::R)
            fout << irrep[i].getDim();
        else if (irrep[i].getAlgebra()) {
            // U(1) case:
            // i^th group
            // first rep (single)
            // first term of sum (single)
            // first term of product (single)
            AlgebraState s = irrep[i][0];
            cout << s[0];
            if (s[1] != 1)
                cout << "/" << s[1];
        }

        if (i != irrep.size() - 1)
            fout << " , ";
    }
    fout << " )";
    return fout;
}

} // End of namespace mty
