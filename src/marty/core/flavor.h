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
#ifndef FLAVOR_H_INCLUDED
#define FLAVOR_H_INCLUDED

#include "group.h"
#include "representation.h"
#include <string_view>

namespace mty {

class SemiSimpleGroup;
class QuantumFieldParent;
class Irrep;
class FlavorIrrep;

struct FlavorFlag {
    bool nontrivial;

    FlavorFlag(bool v = false): nontrivial(v) {}

    operator bool() const { return nontrivial; }

    enum Rep {
        Trivial,
        Fundamental
    };
};

class FlavorGroup {

  public:
    FlavorGroup(std::string const &name,
                int                nFlavors,
                bool               complexFields = true);

    size_t getDim() const;

    std::string_view getName() const;

    csl::Index getCorrespondingIndex(const std::string &       index_name,
                                     const QuantumFieldParent &parent);

    const csl::Space *getVectorSpace(FlavorFlag rep = FlavorFlag::Fundamental) const;

    FlavorFlag getTrivialRep() const;

    FlavorFlag getFundamentalRep() const;

    bool isComplex() const;

  private:
    bool complexFields;
    std::unique_ptr<csl::Space> space;
    static std::unique_ptr<csl::Space> trivialSpace;
};

class Flavor {

    typedef std::vector<mty::FlavorGroup *> FlavList;

  public:
    ~Flavor();

    FlavorIrrep getTrivialRep() const;

    FlavorIrrep getFundamentalRep() const;

    bool empty() const
    {
        return flavors.empty();
    }

    size_t size() const
    {
        return flavors.size();
    }

    FlavList::iterator begin()
    {
        return flavors.begin();
    }

    FlavList::iterator end()
    {
        return flavors.end();
    }

    FlavList::const_iterator begin() const
    {
        return flavors.begin();
    }

    FlavList::const_iterator end() const
    {
        return flavors.end();
    }

    void addGroup(std::unique_ptr<FlavorGroup> &newPtr)
    {
        flavors.push_back(newPtr.release());
    }

    void addGroup(std::unique_ptr<FlavorGroup> &&newPtr)
    {
        flavors.push_back(newPtr.release());
    }

    void push_back(std::unique_ptr<FlavorGroup> &newPtr)
    {
        flavors.push_back(newPtr.release());
    }

    void push_back(std::unique_ptr<FlavorGroup> &&newPtr)
    {
        flavors.push_back(newPtr.release());
    }

    FlavorGroup *operator[](size_t i) const
    {
        return flavors[i];
    }

  private:
    FlavList flavors;
};

class FlavorIrrep {

    IMPLEMENTS_STD_VECTOR(FlavorFlag, rep);

  public:
    FlavorIrrep() = default;

    FlavorIrrep(Flavor const *t_flavors);

    FlavorIrrep(Flavor const *t_flavors, FlavorIrrep const &other);

    FlavorIrrep(FlavorIrrep const &other) = default;

    FlavorIrrep &operator=(FlavorIrrep const &other) = default;

    const Flavor *getFlavor() const;

    void setTrivialRepresentation(FlavorGroup const *flavorGroup);

    void setFundamentalRepresentation(FlavorGroup const *flavorGroup);

    void setRepresentation(FlavorGroup const *flavorGroup, FlavorFlag const &irrep);

    bool operator==(FlavorIrrep const &other) const;
    bool operator!=(FlavorIrrep const &other) const
    {
        return !(*this == other);
    }

    friend std::ostream &operator<<(std::ostream &     fout,
                                    const FlavorIrrep &irrep);

  private:
    Flavor const *flavors;

    std::vector<FlavorFlag> rep;
};

} // End of namespace mty

#endif
