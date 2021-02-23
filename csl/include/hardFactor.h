#pragma once

namespace csl {

class Expr;

bool HardFactorImplementation(
        Expr &sum,
        bool applyRecursively = true
        );

Expr HardFactored(Expr const &init);
void HardFactor(Expr &init);
Expr DeepHardFactored(Expr const &init);
void DeepHardFactor(Expr &init);

}
