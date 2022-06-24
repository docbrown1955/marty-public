#include "bsmixingvlq_9_10.h"
#include <fstream>

using namespace bsmixingvlq_9_10;

real_t real(real_t x) { return x; }
#ifndef QUAD
real_t real(complex_t x)
{
    return x.real();
}
#else
real_t real(complex_t x)
{
    return crealq(x);
}
#endif

real_t factor(real_t M)
{
    return 1 / (M * M);
}

real_t factor(complex_t M)
{
    return factor(real(M));
}

real_t factor(real_t M1, real_t M2)
{
    if (M1 == M2)
    {
        return factor(M1);
    }
    return std::log(M1 * M1 / (M2 * M2)) / (M1 * M1 - M2 * M2);
}

template <class T, class U>
real_t factor(T M1, U M2)
{
    return factor(real(M1), real(M2));
}

int main()
{

    param_t params;

    // params.reg_prop = 1e-3;
    // params.Finite = 1;
    params.e_em = std::sqrt(4 * M_PI / 137);
    params.M_W = 80.379;
    params.M_Z = 91.2;
    params.m_Gp = params.M_W;
    params.m_G0 = params.M_Z;
    // params.xi = 1;
    // params.xi_W = 1;
    // params.xi_Z = 1;
    // params.xi_A = 1;
    // params.xi_G = 1;
    params.theta_W = std::acos(params.M_W / params.M_Z);
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
    // params.V_ub_mod = 0;
    // params.delta_wolf = 0;

    Diagonalizer::precision = 0.1;

    params.Ld_VQV_0 = 1.;
    params.Ld_VQV_1 = 1.;
    params.Ld_VQV_2 = 1.;
    params.Lu_VQV_0 = 1.;
    params.Lu_VQV_1 = 1.;
    params.Lu_VQV_2 = 1.;
    params.M_QV = 1.;
    params.V_ub = 1.;
    params.Lx_VQu_0 = 1.;
    params.Lx_VQu_1 = 1.;
    params.Lx_VQu_2 = 1.;
    params.M_Qu = 1.;
    params.m_U_L = 1.;
    params.m_b_L = 1.;
    params.m_c_L = 1.;
    params.m_c_Z = 1.;
    params.m_d_L = 1.;
    params.m_s_L = 1.;
    params.m_t_L = 1.;
    params.m_u_L = 1.;
    params.m_Du_L = 1.;
    params.m_Ux_L = 1.;
    params.m_c_Wm = 1.;
    params.m_c_Wp = 1.;
    params.U_d_00 = 1.;
    params.U_d_01 = 1.;
    params.U_d_02 = 1.;
    params.U_d_03 = 1.;
    params.U_d_10 = 1.;
    params.U_d_11 = 1.;
    params.U_d_12 = 1.;
    params.U_d_13 = 1.;
    params.U_d_20 = 1.;
    params.U_d_21 = 1.;
    params.U_d_22 = 1.;
    params.U_d_23 = 1.;
    params.U_d_30 = 1.;
    params.U_d_31 = 1.;
    params.U_d_32 = 1.;
    params.U_d_33 = 1.;
    params.U_u_00 = 1.;
    params.U_u_01 = 1.;
    params.U_u_02 = 1.;
    params.U_u_03 = 1.;
    params.U_u_04 = 1.;
    params.U_u_10 = 1.;
    params.U_u_11 = 1.;
    params.U_u_12 = 1.;
    params.U_u_13 = 1.;
    params.U_u_14 = 1.;
    params.U_u_20 = 1.;
    params.U_u_21 = 1.;
    params.U_u_22 = 1.;
    params.U_u_23 = 1.;
    params.U_u_24 = 1.;
    params.U_u_30 = 1.;
    params.U_u_31 = 1.;
    params.U_u_32 = 1.;
    params.U_u_33 = 1.;
    params.U_u_34 = 1.;
    params.U_u_40 = 1.;
    params.U_u_41 = 1.;
    params.U_u_42 = 1.;
    params.U_u_43 = 1.;
    params.U_u_44 = 1.;
    params.V_d_00 = 1.;
    params.V_d_01 = 1.;
    params.V_d_02 = 1.;
    params.V_d_03 = 1.;
    params.V_d_10 = 1.;
    params.V_d_11 = 1.;
    params.V_d_12 = 1.;
    params.V_d_13 = 1.;
    params.V_d_20 = 1.;
    params.V_d_21 = 1.;
    params.V_d_22 = 1.;
    params.V_d_23 = 1.;
    params.V_d_30 = 1.;
    params.V_d_31 = 1.;
    params.V_d_32 = 1.;
    params.V_d_33 = 1.;
    params.V_u_00 = 1.;
    params.V_u_01 = 1.;
    params.V_u_02 = 1.;
    params.V_u_03 = 1.;
    params.V_u_04 = 1.;
    params.V_u_10 = 1.;
    params.V_u_11 = 1.;
    params.V_u_12 = 1.;
    params.V_u_13 = 1.;
    params.V_u_14 = 1.;
    params.V_u_20 = 1.;
    params.V_u_21 = 1.;
    params.V_u_22 = 1.;
    params.V_u_23 = 1.;
    params.V_u_24 = 1.;
    params.V_u_30 = 1.;
    params.V_u_31 = 1.;
    params.V_u_32 = 1.;
    params.V_u_33 = 1.;
    params.V_u_34 = 1.;
    params.V_u_40 = 1.;
    params.V_u_41 = 1.;
    params.V_u_42 = 1.;
    params.V_u_43 = 1.;
    params.V_u_44 = 1.;

    std::ofstream fout("data.txt");
    for (double M = 10; M < 10000; M += 100)
    {
        params.M_QV = M;
        params.M_Qu = M;

        updateSpectrum(params);
        std::cout << "********************************" << '\n';
        std::cout << "M = " << M << '\n';
        std::cout << "C_VLL = " << Cmix_VLL(params).real() / factor(params.M_QV, params.M_Qu) << '\n';
        std::cout << "C_VRR = " << Cmix_VRR(params).real() / factor(params.M_QV, params.M_Qu) << '\n';
        std::cout << "C_VLR = " << Cmix_VLR(params).real() / factor(params.M_QV, params.M_Qu) << '\n';
        std::cout << "C_VRL = " << Cmix_VRL(params).real() / factor(params.M_QV, params.M_Qu) << '\n';

        fout << Cmix_VLL(params).real() / factor(params.M_QV, params.M_Qu) << '\t';
        fout << Cmix_VRR(params).real() / factor(params.M_QV, params.M_Qu) << '\t';
        fout << Cmix_VLR(params).real() / factor(params.M_QV, params.M_Qu) << '\t';
        fout << Cmix_VRL(params).real() / factor(params.M_QV, params.M_Qu) << '\n';
    }

    fout.close();

    return 0;
}
