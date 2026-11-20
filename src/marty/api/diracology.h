#ifndef MARTY_API_DIRACOLOGY_H_INCLUDED
#define MARTY_API_DIRACOLOGY_H_INCLUDED

#include "csl/abstract.h"
#include "marty/sgl/abstractgammasym.h"

#include <cstddef>
#include <string>
#include <vector>

/**
 * @file diracology.h
 * @brief Typed API over the Dirac algebra (gamma matrices, traces, Fierz
 * identities), used by the C++ tests and the Python binding.
 *
 * Invalid arguments are reported with the typed errors of mty::api (see
 * common/error.h). Errors raised by the algebra core keep their GammaAPIError
 * type, except in the Fierz functions, which translate them to ValueError.
 */

namespace mty::api::diracology {

using Expr = sgl::GExpr;

void keepSymbolic4D(bool symbolic);

enum class FierzBasis { Standard, Chiral };

/// Result of a Fierz rearrangement: rhs is the rearranged product, lhs the
/// original product of the two currents.
struct FierzResult {
    Expr rhs;
    Expr lhs;
};

// Single expressions
Expr g(int mu, int nu);
Expr eps(int mu, int nu, int rho, int sig);
Expr gamma(int mu);
Expr gamma(int mu, int nu);
Expr gamma5();
Expr P_L();
Expr P_R();
Expr C();
Expr DMinko();
Expr imaginaryUnit();

// Create a chain of gamma matrices
Expr current(std::vector<Expr> const &gammas, int iLeft, int iRight);
Expr trace(std::vector<Expr> const &gammas);

Expr ordered(Expr const &expr);
Expr simplified(Expr const &expr);
Expr project(Expr const &expr, FierzBasis basis);

FierzResult singleFierz(Expr const &chain1,
                        Expr const &chain2,
                        FierzBasis  basis = FierzBasis::Chiral);

FierzResult doubleFierz(Expr const &chain1,
                        Expr const &chain2,
                        FierzBasis  basis = FierzBasis::Chiral);

std::string generateString(Expr const &expr);
std::string generateLatex(Expr const &expr);

/// Number of arguments of a product or sum (1 for an atom).
std::size_t size(Expr const &expr);
/// Argument i of expr; throws LookupError when i is out of range.
Expr argument(Expr const &expr, std::size_t i);

/// Canonical equality: both expressions are converted to CSL and compared.
/// The inputs must be convertible (a current, a metric, a scalar): a bare
/// gamma matrix is rejected by SGL.
bool equals(Expr const &a, Expr const &b);
/// Canonical strict ordering, consistent with equals() and with the same
/// input restriction.
bool less(Expr const &a, Expr const &b);

Expr deepCopy(Expr const &expr);

/// Scalar coefficients, as CSL numbers.
using Coefficient = csl::Expr;
Coefficient fromInt(long value);
Coefficient fromRational(long numerator, long denominator);
Coefficient fromDouble(double value);
/// coefficient * expr.
Expr scale(Expr const &expr, Coefficient const &coefficient);
/// Canonical equality between an expression and a scalar coefficient.
bool equalsCoefficient(Expr const &expr, Coefficient const &coefficient);

/// Sets the dimension convention (keepSymbolic4D) and restores the previous
/// one when the guard goes out of scope, also on exception.
class ScopedSymbolicDimension {
  public:
    explicit ScopedSymbolicDimension(bool keepSymbolic);
    ~ScopedSymbolicDimension();
    ScopedSymbolicDimension(ScopedSymbolicDimension const &) = delete;
    ScopedSymbolicDimension &operator=(ScopedSymbolicDimension const &)
        = delete;

  private:
    bool _previous;
};

/// Sets whether epsilon indices are ordered in products, and restores the
/// previous setting when the guard goes out of scope, also on exception.
class ScopedEpsilonIndexOrdering {
  public:
    explicit ScopedEpsilonIndexOrdering(bool order);
    ~ScopedEpsilonIndexOrdering();
    ScopedEpsilonIndexOrdering(ScopedEpsilonIndexOrdering const &) = delete;
    ScopedEpsilonIndexOrdering &operator=(ScopedEpsilonIndexOrdering const &)
        = delete;

  private:
    bool _previous;
};

} // namespace mty::api::diracology

#endif
