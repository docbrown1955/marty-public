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
#ifndef FLAVOR_H_INCLUDED
#define FLAVOR_H_INCLUDED

#include <string_view>
#include "representation.h"
#include "group.h"

namespace mty {

class SemiSimpleGroup;
class QuantumFieldParent;
class Irrep;
class FlavorIrrep;

class FlavorGroup {

    public:

    FlavorGroup(std::string const& name,
                int                nFlavors,
                bool               complexFields = true);

    std::size_t getDim() const;

    std::string_view getName() const;

    csl::Index getCorrespondingIndex(
            const std::string&        index_name, 
            const QuantumFieldParent& parent);

    const csl::Space* getVectorSpace(const Irrep& irrep) const;

    const csl::Space* getFundamentalSpace() const;

    Irrep getTrivialRep() const;

    Irrep getFundamentalRep() const;

    SemiSimpleGroup* getGroup() const;

    bool isComplex() const;

    private:

    std::string name;

    std::unique_ptr<SemiSimpleGroup> group;
};

class Flavor {

    typedef std::vector<mty::FlavorGroup*> FlavList;

    public:

    ~Flavor();

    FlavorIrrep getTrivialRep() const;

    FlavorIrrep getFundamentalRep() const;

    bool empty() const {
        return flavors.empty();
    }

    std::size_t size() const {
        return flavors.size();
    }

    FlavList::iterator begin() {
        return flavors.begin();
    }

    FlavList::iterator end() {
        return flavors.end();
    }

    FlavList::const_iterator begin() const {
        return flavors.begin();
    }

    FlavList::const_iterator end() const {
        return flavors.end();
    }

    void addGroup(std::unique_ptr<FlavorGroup>& newPtr) {
        flavors.push_back(newPtr.release());
    }

    void addGroup(std::unique_ptr<FlavorGroup>&& newPtr) {
        flavors.push_back(newPtr.release());
    }

    void push_back(std::unique_ptr<FlavorGroup>& newPtr) {
        flavors.push_back(newPtr.release());
    }

    void push_back(std::unique_ptr<FlavorGroup>&& newPtr) {
        flavors.push_back(newPtr.release());
    }

    FlavorGroup* operator[](std::size_t i) const {
        return flavors[i];
    }

    private:

    FlavList flavors;
};

class FlavorIrrep {

    IMPLEMENTS_STD_VECTOR(Irrep, rep);

    public:

    FlavorIrrep() = default;

    FlavorIrrep(Flavor const* t_flavors);

    FlavorIrrep(Flavor const* t_flavors,
                FlavorIrrep const& other);

    FlavorIrrep(FlavorIrrep const& other) = default;

    FlavorIrrep& operator=(FlavorIrrep const& other) = default;

    const Flavor* getFlavor() const;

    void setTrivialRepresentation(FlavorGroup const* flavorGroup);

    void setFundamentalRepresentation(FlavorGroup const* flavorGroup);

    void setRepresentation(FlavorGroup const* flavorGroup,
                           Irrep const& irrep);

    bool operator==(FlavorIrrep const &other) const;
    bool operator!=(FlavorIrrep const &other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream&     fout,
                                    const FlavorIrrep& irrep);

    private:

    Flavor const* flavors;

    std::vector<Irrep> rep;
};

} // End of namespace mty

#endif
