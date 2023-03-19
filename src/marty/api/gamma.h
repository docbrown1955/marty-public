#ifndef MARTY_API_GAMMA_H_INCLUDED
#define MARTY_API_GAMMA_H_INCLUDED

#include <string>
#include <vector>
#include "gamma_api_error.h"

namespace sgl {
class GExpr;
} // namespace sgl
namespace mty::gamma_api {


using Expr = sgl::GExpr;

void keepSymbolic4D(bool symbolic);

enum class FierzBasis { Standard, Chiral };

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

Expr applySingleFierz(Expr const &chain1,
                      Expr const &chain2,
                      FierzBasis  basis = FierzBasis::Chiral);

Expr applyDoubleFierz(Expr const &chain1,
                      Expr const &chain2,
                      FierzBasis  basis = FierzBasis::Chiral);

std::string generateString(Expr const &expr);
std::string generateLatex(Expr const &expr);


} // namespace mty::gamma_api

#endif
