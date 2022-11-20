#ifndef MARTY_API_GAMMA_H_INCLUDED
#define MARTY_API_GAMMA_H_INCLUDED

#include <vector>
#include <string>
#include <sstream>
#include <exception>

namespace sgl {
class GExpr;
} // namespace sgl
namespace mty::gamma_api {


class GammaAPIError: public std::exception {
public:
    template<class ...Args>
    GammaAPIError(Args &&...args);

    char const *what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};	

using Expr = sgl::GExpr;

void keepSymbolic4D(bool symbolic);

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

Expr simplified(Expr const &expr);

Expr applyDoubleFierz(Expr const &chain1, Expr const &chain2);

template<class ...Args>
GammaAPIError::GammaAPIError(Args &&...args)
{
    std::ostringstream sout;
    (sout << ... << args);
    this->msg = sout.str();
}

}; // namespace mty::gamma_api

#endif
