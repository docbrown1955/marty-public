#include "thdm_paper.h"
#include <iostream>
#include <fstream>

using namespace thdm_paper;

auto F1_7(real_t x)
{
    auto x2 = x*x;
    return x * (7 - 5*x - 8*x2) / (24*powq(x - 1, 3))
        + x2 * (3*x - 2) / (4 * powq(1 - x, 4)) * clogq(x);
}

auto F2_7(real_t x)
{
    return x * (3 - 5*x) / (12*powq(x - 1, 2))
        + x * (3*x - 2) / (6 * powq(x - 1, 3)) * clogq(x);
}

auto F1_8(real_t x)
{
    auto x2 = x*x;
    return x * (2 + 5*x - x2) / (8 * powq(x - 1, 3))
        - 3*x2 / (4 * powq(x - 1, 4)) * clogq(x);
}

auto F2_8(real_t x)
{
    return x * (3 - x) / (4 * powq(x - 1, 2))
        - x / (2 * powq(x - 1, 3)) * clogq(x);
}

auto BH0(real_t x, param_t const & params)
{
    auto MW2 = params.M_W * params.M_W;
    auto sW2 = powq(sinq(params.theta_W), 2);
    return x*(x - x*clogq(x) - 1) / (MW2*sW2*powq(x - 1, 2));
}

auto CH0(real_t x, param_t const &params)
{
    auto mH2 = powq(params.m_Hp, 2);
    auto mW2 = powq(params.M_W, 2);

    return mH2 / (8*mW2)*x*x*(-1 / (powq(x-1, 2))*clogq(x) + 1/(x-1));
}

auto CH0_mix(real_t x, param_t const &params)
{
    auto mW2 = powq(params.M_W, 2);
    auto sW2 = powq(sinq(params.theta_W), 2);

    return - (1 - 4*sW2) * x * (x*x - 2*x*clogq(x) - 1)
        / (16*mW2*sW2 * powq(x - 1, 3));
}

auto DH0(real_t x, param_t const &)
{
    auto x2 = x*x;
    auto x3 = x2*x;

    return x/18 * (
            (-3*x3 + 6*x - 4) / powq(x - 1, 4) * clogq(x)
            + (47*x2 - 79*x + 38) / (6*powq(x - 1, 3))
            );
}

enum class bsgMode {
    C7, C8
};

auto GF_sqrt2(param_t const &params)
{
    return powq(params.e_em / (2*sinq(params.theta_W)*params.M_W), 2);
}

complex_t bsg_theo(param_t const &params, bsgMode mode)
{
    auto f1 = 1  / (3*GF_sqrt2(params) * powq(params.m_t, 2));
    auto f2 = -1 / (GF_sqrt2(params) * params.m_b*params.m_t);

    //auto ckm = (params.V_tb * std::conj(params.V_ts.get()));

    auto x = powq(params.m_t / params.m_Hp, 2);

    if (mode == bsgMode::C7)
        return (f1*F1_7(x) + f2*F2_7(x));
    return (f1*F1_8(x) + f2*F2_8(x));
}

complex_t C9_Z_theo(param_t const & params)
{
    auto x = powq(params.m_t / params.m_Hp, 2);
    auto sW2 = powq(sinq(params.theta_W), 2);

    //auto ckm = (params.V_tb * std::conj(params.V_ts.get()));
    return 1 / (GF_sqrt2(params) * powq(params.m_t, 2)) 
        * (1 - 4*sW2)/sW2 * CH0(x, params)
        + params.m_b/(GF_sqrt2(params)*params.m_t)*CH0_mix(x, params);
}

complex_t C9_A_theo(param_t const & params)
{
    auto x = powq(params.m_t / params.m_Hp, 2);
    //auto ckm = (params.V_tb * std::conj(params.V_ts.get()));
    return -DH0(x, params) / (GF_sqrt2(params) * powq(params.m_t, 2));
}

complex_t C9_box_theo(param_t const & params)
{
    auto x = powq(params.m_t / params.m_Hp, 2);
    //auto ckm = (params.V_tb * std::conj(params.V_ts.get()));
    return 1 / (16*powq(GF_sqrt2(params)*params.m_t, 2))*BH0(x, params);
}

int main() {

    param_t params;
 
    // Miscellaneous parameters
    params.reg_prop = 1e-3;
    params.Finite = 1;

    // SM parameters
    params.M_W = 80.379;
    params.M_Z = 91.2;
    params.g_s = sqrtq(4*M_PI * 0.0111);
    params.e_em = sqrtq(4*M_PI / 137.);
    params.m_b = 4.18;
    params.m_s = 0;//0.150;
    params.m_t = 173.;
    params.m_mu = 0;
    params.theta_W = acosq(params.M_W / params.M_Z);
    // CKM
    //params.V_ud = 1;
    //params.V_us = 0;
    //params.V_ub_mod = 0;
    //params.delta_wolf = 0;
    // params.V_cd = 0;
    params.V_cs = complex_t{0.973304, (-0.000114673)};
    params.V_cb = 0.041;
    //params.V_td = 0;
    params.V_ts = complex_t{(-0.0476512), (-0.00283327)};
    params.V_tb = 1.013;


    // 2HDM parameters
    params.m_Hp = 150;
    params.beta = atanq(10);
    params.xil_mm = 1;
    params.xil_mt = 0;
    params.xiu_tt = 1;
    params.xiu_ct = 0;
    params.xid_bb = 1;
    params.xid_sb = 0;

    setMu(params.m_Hp);
   
    updateSpectrum(params);
    params.print();

    // Kinematics 
    params.s_12 = (powq(params.m_b, 2) + powq(params.m_s, 2)) / 2.;
    params.s_34 = -powq(params.m_mu, 2);

    std::ofstream out("data.txt");
    int i_ligne = 1;
    for (params.m_Hp = 100; params.m_Hp < 1000; params.m_Hp = params.m_Hp+1) {
        if (i_ligne++ == 74) {
            // Theoretical values are NaN for this particular point
            continue;
        }
        out << (double)crealq(C7(params)) << "\t";
        out << (double)crealq(bsg_theo(params, bsgMode::C7)) << "\t";
        out << (double)crealq(C8(params)) << "\t";
        out << (double)crealq(bsg_theo(params, bsgMode::C8)) << "\t";
        out << (double)crealq(C9_Z(params)) << "\t";
        out << (double)crealq(C9_Z_theo(params)) << "\t";
        out << (double)crealq(C9_box(params)) << "\t";
        out << (double)crealq(C9_box_theo(params)) << "\t";
        out << (double)crealq(C9_A(params)) << "\t";
        out << (double)crealq(C9_A_theo(params)) << "\t";
        out << "\n";
    }
    out.close();

    return 0;
}
