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
