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
#ifndef FERMIONCHAIN_H_INCLUDED
#define FERMIONCHAIN_H_INCLUDED

#include "../../csl/csl.h"

namespace mty {

class DiracSpace;
class FermionChain {

    public:

    struct Chain {
        std::vector<size_t> pos;
        bool                cycle = false;
    };

    explicit
    FermionChain(const csl::Space *t_diracSpace);

    bool applyOn(csl::Expr& prod,
                 bool  makeAbbreviation = true) const;

    static std::vector<Chain> getChains(
            csl::Expr       const& prod,
            DiracSpace const* diracSpace,
            bool              keepBorders = false);

    std::vector<csl::Expr>& getTensorsInChain();

    std::vector<csl::Expr> const& getTensorsInChain() const;

    private:

    const mty::DiracSpace *diracSpace;
    std::vector<csl::Expr> tensors;
};

}

#endif
