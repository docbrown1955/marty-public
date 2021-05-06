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

 #pragma once 

#include "abstractgammasym.h"

namespace sgl {

    struct TensorSet;

    void SimplifyChain(GExpr &init);
    void SimplifyMomenta(GExpr &init);
    void SimplifyTrace(GExpr &init);
    void OrderChains(GExpr &init);

    void ReorderFermionChains(GExpr &init);

    void Simplify(GExpr &init, bool applyFierzTwice = false);
    GExpr Simplified(GExpr const &init, bool applyFierzTwice = false);

    void CSLSimplify(GExpr &init);
    GExpr CSLSimplified(GExpr const &init);

    void SGLSimplify(csl::Expr &init, TensorSet const &t);
    csl::Expr SGLSimplified(csl::Expr const &init, TensorSet const &t);
}
