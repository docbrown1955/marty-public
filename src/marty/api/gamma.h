#ifndef MARTY_API_GAMMA_H_INCLUDED
#define MARTY_API_GAMMA_H_INCLUDED

#include <vector>

namespace sgl {
class GExpr;
} // namespace sgl
namespace mty::gamma_api {

using Expr = sgl::GExpr;

// Single expressions
Expr g(int mu, int nu);
Expr eps(int mu, int nu, int rho, int sig);
Expr gamma(int mu);
Expr sigma(int mu, int nu);
Expr gamma5();
Expr P_L();
Expr P_R();
Expr C();
Expr p(int mu);
Expr field(int species);

// Create a chain of gamma matrices
Expr chain(std::vector<Expr> const &gammas, int iLeft, int iRight);

}; // namespace mty::gamma_api

#endif
