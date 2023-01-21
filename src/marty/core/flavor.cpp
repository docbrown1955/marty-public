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

std::unique_ptr<csl::Space> FlavorGroup::trivialSpace = std::make_unique<csl::Space>(
    "trivial_flavor_space",
    1
);

FlavorGroup::FlavorGroup(string const &t_name,
                         int           nFlavors,
                         bool          t_complexFields)
    : complexFields(t_complexFields)
{
    space = std::make_unique<csl::Space>(t_name, nFlavors);
}

size_t FlavorGroup::getDim() const
{
    return getVectorSpace()->getDim();
}

string FlavorGroup::getName() const
{
    return space->getName();
}

csl::Index FlavorGroup::getCorrespondingIndex(const string &index_name,
                                              const QuantumFieldParent &)
{
    return space->generateIndex(index_name);
}

const csl::Space *FlavorGroup::getVectorSpace(FlavorFlag rep) const
{
    return (rep ? space : trivialSpace).get();
}

FlavorFlag FlavorGroup::getTrivialRep() const
{
    return FlavorFlag::Trivial;
}

FlavorFlag FlavorGroup::getFundamentalRep() const
{
    return FlavorFlag::Fundamental;
}

bool FlavorGroup::isComplex() const
{
    return complexFields;
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
        r = FlavorFlag::Trivial;
    return rep;
}

FlavorIrrep Flavor::getFundamentalRep() const
{
    FlavorIrrep rep(this);
    for (auto &r : rep)
        r = FlavorFlag::Fundamental;
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
            rep.push_back(f->getTrivialRep());
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
        HEPAssert((*flavors)[i]->getVectorSpace() == (*other.flavors)[i]->getVectorSpace(),
                  mty::error::ValueError,
                  "Flavor groups do not match in flavor replacement");
        rep.push_back(r);
        ++i;
    }
    for (; i != flavors->size(); ++i)
        rep.push_back((*flavors)[i]->getTrivialRep());
}

const Flavor *FlavorIrrep::getFlavor() const
{
    return flavors;
}

void FlavorIrrep::setTrivialRepresentation(FlavorGroup const *flavorGroup)
{
    auto rep = flavorGroup->getTrivialRep();
    setRepresentation(flavorGroup, rep);
}

void FlavorIrrep::setFundamentalRepresentation(FlavorGroup const *flavorGroup)
{
    setRepresentation(flavorGroup, FlavorFlag::Fundamental);
}

void FlavorIrrep::setRepresentation(FlavorGroup const *flavorGroup,
                                    FlavorFlag  const &irrep)
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
        if (irrep[i])
            fout << (*irrep.flavors)[i]->getDim();
        else
            fout << 1;
        if (i != irrep.size() - 1)
            fout << " , ";
    }
    fout << " )";
    return fout;
}

} // End of namespace mty
