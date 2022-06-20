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

#include "quantumNumber.h"
#include "quantumField.h"

namespace mty {

QuantumNumber::QuantumNumber(std::string const &t_name, bool t_conserved)
    : conserved(t_conserved), id(current_id++)
{
    name()[id] = t_name;
}

QuantumNumber::~QuantumNumber()
{
}

bool QuantumNumber::isConserved() const
{
    return conserved;
}

std::string QuantumNumber::getName() const
{
    return name()[id];
}

void QuantumNumber::setConservedProperty(bool t_conserved)
{
    conserved = t_conserved;
}

void QuantumNumber::setName(std::string const &t_name)
{
    name()[id] = t_name;
}

bool QuantumNumber::appearsIn(
    std::vector<mty::QuantumField> const &fields) const
{
    for (const auto &f : fields)
        if (f.getQuantumParent()->getQuantumNumber(&f, this) != 0)
            return true;
    return false;
}

int QuantumNumber::computeQuantumNumber(
    std::vector<mty::QuantumField> const &fields) const
{
    int res = 0;
    for (const auto &f : fields)
        res += f.getQuantumParent()->getQuantumNumber(&f, this);
    return res;
}

} // namespace mty
