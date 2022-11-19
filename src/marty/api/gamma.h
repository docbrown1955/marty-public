#ifndef MARTY_API_GAMMA_H_INCLUDED
#define MARTY_API_GAMMA_H_INCLUDED

namespace mty::gamma_api {
    
    struct GammaExpr;

    GammaExpr g(int mu, int nu);
    GammaExpr eps(int mu, int nu);
    GammaExpr gamma(int mu);
    GammaExpr sigma(int mu, int nu);
    GammaExpr gamma5();
    GammaExpr P_L();
    GammaExpr P_R();
    GammaExpr C();
    GammaExpr p(int mu);
    GammaExpr field(int species);
 
}; // namespace mty::gamma_api

#endif
