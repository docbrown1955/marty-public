#ifndef MARTY_API_GAMMA_H_INCLUDED
#define MARTY_API_GAMMA_H_INCLUDED

#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace sgl {
class GExpr;
} // namespace sgl
namespace mty::gamma_api {

class GammaAPIError : public std::exception {
  public:
    template <class... Args>
    GammaAPIError(Args &&...args);

    char const *what() const noexcept override
    {
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
Expr gamma(int mu, int nu);
Expr gamma5();
Expr P_L();
Expr P_R();
Expr C();

// Create a chain of gamma matrices
Expr chain(std::vector<Expr> const &gammas, int iLeft, int iRight);

Expr simplified(Expr const &expr);

Expr applySingleFierz(Expr const &chain1, Expr const &chain2);

Expr applyDoubleFierz(Expr const &chain1, Expr const &chain2);

std::string generateString(Expr const &expr);
std::string generateLatex(Expr const &expr);

template <class... Args>
GammaAPIError::GammaAPIError(Args &&...args)
{
    std::ostringstream sout;
    (sout << ... << args);
    this->msg = sout.str();
}

}; // namespace mty::gamma_api

#endif
