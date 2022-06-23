#include "clooptools.h"
#include "c9_sm.h"
#include <fstream>

using namespace c9_sm;

#ifndef QUAD
 #define ACOS(x) std::acos(x)
 #define SIN(x) std::sin(x)
 #define REAL(x) (x.real())
 #define IMAG(x) (x.imag())
 #define POW(x, y) std::pow(x, y)
 #define LOG(x) std::log(x)
#else
 #define ACOS(x) acosq(x)
 #define SIN(x) sinq(x)
 #define REAL(x) crealq(x)
 #define IMAG(x) cimagq(x)
 #define POW(x, y) powq(x, y)
 #define LOG(x) logq(x)
#endif

real_t B0(const real_t x)
{
    const real_t lnx = LOG(x);
    return x / (4*POW(x - 1, 2))*lnx + 1 / (4*(1 - x));
}

real_t C0(const real_t x)
{
    const real_t lnx = LOG(x);
    const real_t x2 = x*x;
    return (3*x2 + 2*x) / (8*POW(1 - x, 2))*lnx
        + (-x2 + 6*x) / (8*(1 - x));
}

real_t D0(const real_t x)
{
    const real_t lnx = LOG(x);
    const real_t x2 = x*x;
    const real_t x3 = x2*x;
    const real_t x4 = x2*x2;
    return (-3*x4 + 30*x3 - 54*x2 + 32*x - 8) / (18*POW(1 - x, 4))*lnx
        + (-47*x3 + 237*x2 - 312*x + 104) / (108*POW(1 - x, 3));
}

real_t C9_box(param_t const &param)
{
    const real_t x = POW(param.m_t / param.M_W, 2);
    return -1 / POW(SIN(param.theta_W), 2) * B0(x);
}

real_t C9_Z(param_t const &param)
{
    const real_t x = POW(param.m_t / param.M_W, 2);
    const real_t s2W = POW(SIN(param.theta_W), 2);
    return C0(x)*(1 - 4*s2W) / s2W;
}

real_t C9_A_th(param_t const &param)
{
    const real_t x = POW(param.m_t / param.M_W, 2);
    return -D0(x);
}

real_t C9_tot(param_t const &param)
{
    return C9_box(param) + C9_Z(param) + C9_A_th(param);
}
int main() {

    param_t param;
    param.M_W = 80.379;
    const double M_Z = 91.2;
    param.m_Gp = param.M_W;
    param.M_Z = 91.1876;
    param.m_b = 4.18;
    param.m_s = 0.148;
    param.e_em = std::sqrt(4*M_PI*1./137);
    param.m_mu = 0;
    //param.s_11 = 100;//param.m_b;
    //param.s_22 = 100;//param.m_s;
    param.s_12 = (param.m_b*param.m_b + param.m_s*param.m_s) / 2.;
    param.s_34 = -param.m_mu*param.m_mu;
    param.Finite   = 1;
    // param.reg_int  = 1e-2;
    param.reg_prop = 1e-3;
    //param.theta_W = std::acos(param.M_W / param.M_Z);
    param.theta_W = ACOS(param.M_W / M_Z);
    auto mu = param.M_W;
    setMu(mu);

    std::ofstream fout("data.txt");
    fout.precision(15);
    for (double mt = 120; mt != 400; ++mt) {
        param.m_t = mt;
        auto c9_zb = REAL(C9_BZ(param));
        fout << mt << '\t';
        fout << c9_zb << '\t';
        fout << C9_Z(param) + C9_box(param) << '\t';
        auto c9_al = REAL(C9_A(param));
        fout << c9_al << '\t';
        fout << REAL(C9_Tot(param)) << '\t';
        fout << C9_A_th(param) << '\n';
    }
    fout.close();

    return 0;
}
