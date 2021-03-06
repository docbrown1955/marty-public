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
#ifndef GAUGED_H_INCLUDED
#define GAUGED_H_INCLUDED

#include "../../csl/csl.h"
#include "group.h"
#include "quantumField.h"

/*
class Gauged {

    public:

    explicit Gauged(Gauge* gauge,
                    Group* t_group,
                    const std::string& couplingName = "")
    : group(t_group){
        if (couplingName == "")
            couplingConstant = constant_s("g"+t_group->getName());
        else
            couplingConstant = constant_s(couplingName);
        vectorBoson = QuantumFieldParent("A", 1, gauge, );
    }

    csl::Expr getCoupling() const {
        return couplingConstant;
    }

    private:

    Group* group;
    csl::Expr   couplingConstant;
    QuantumFieldParent vectorBoson;
};
*/

#endif
