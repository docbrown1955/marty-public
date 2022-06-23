#include "#LIBRARYNAME.h"
#include <fstream>

using namespace #LIBRARYNAME;


real_t real(real_t x) { return x; }
#ifndef QUAD
real_t real(complex_t x) { return x.real(); }
#else
real_t real(complex_t x) { return crealq(x); }
#endif

real_t factor(real_t M)
{
    return 1 / (M*M);
}

real_t factor(complex_t M)
{
    return factor(real(M));
}

real_t factor(real_t M1, real_t M2) {
    if (M1 == M2) {
        return factor(M1);
    }
    return std::log(M1*M1 / (M2*M2)) / (M1*M1 - M2*M2);
}

template<class T, class U>
real_t factor(T M1, U M2) {
    return factor(real(M1), real(M2));
}

int main() {

    param_t params;

    params.reg_prop = 1e-3;
    params.Finite = 1;
    params.e_em = std::sqrt(4*M_PI / 137);
    params.M_W = 80.379;
    params.M_Z = 91.2;
    params.m_Gp = params.M_W;
    params.m_G0 = params.M_Z;
    params.xi = 1;
    params.xi_W = 1;
    params.xi_Z = 1;
    params.xi_A = 1;
    params.xi_G = 1;
    params.theta_W = std::acos(params.M_W/params.M_Z);
    params.m_b_SM = 4.2;
    params.m_s_SM = 0.095;
    params.m_d_SM = 5e-3;
    params.m_c_SM = 1.275;
    params.m_u_SM = 2.5e-3;
    params.m_t_SM = 173;
    params.m_e = 511e-6;
    params.m_mu = 105e-3;
    params.m_tau = 1.78;
    params.m_h = 125.38;

    params.V_cb = 0;
    params.V_cd = 0.;
    params.V_cs = 1;
    params.V_tb = 1;
    params.V_td = 0.;
    params.V_ts = 0.;
    params.V_ud = 1;
    params.V_us = 0;
    params.V_ub_mod = 0;
    params.delta_wolf = 0;

    Diagonalizer::precision = 0.1;

#PARAMDEF


    std::ofstream fout("data.txt");
    for (double M = 10; M < 10000; M += 100) {
#MASSDEF
        updateSpectrum(params);
        std::cout << "********************************" << '\n';
        std::cout << "M = " << M << '\n';
        std::cout << "C_VLL = " << Cmix_VLL(params).real()/factor(#MASSPARAM) << '\n';
        std::cout << "C_VRR = " << Cmix_VRR(params).real()/factor(#MASSPARAM) << '\n';
        std::cout << "C_VLR = " << Cmix_VLR(params).real()/factor(#MASSPARAM) << '\n';
        std::cout << "C_VRL = " << Cmix_VRL(params).real()/factor(#MASSPARAM) << '\n';

        fout << Cmix_VLL(params).real()/factor(#MASSPARAM) << '\t';
        fout << Cmix_VRR(params).real()/factor(#MASSPARAM) << '\t';
        fout << Cmix_VLR(params).real()/factor(#MASSPARAM) << '\t';
        fout << Cmix_VRL(params).real()/factor(#MASSPARAM) << '\n';
    }

    fout.close();

    return 0;
}
