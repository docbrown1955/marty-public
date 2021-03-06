#pragma once

namespace csl {

class Expr;

int  matchBOnA(csl::Expr const &A, csl::Expr const &B);
bool hardComparison(csl::Expr const &, csl::Expr const &);
bool hardOrdering(csl::Expr const &, csl::Expr const &);

} // namespace csl
