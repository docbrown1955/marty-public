#pragma once

#include "params.h"
#include "global.h"
#include "lhaData.h"

inline void readSpectrum(
        c9_mssm::param_t         &param,
        mty::lha::LHAFileData const &data
        )
{
    param.e_em = std::sqrt(4*M_PI * data.getValue("SMINPUTS", 1).value());
    param.M_W = 80.385;
    param.M_Z = data.getValue("SMINPUTS", 4).value();
    param.m_b = data.getValue("SMINPUTS", 5).value();
    param.m_s = 0.150;
    param.m_t = data.getValue("SMINPUTS", 6).value();
    param.m_mu = 0; // 0.1;
    param.theta_W = crealq(cacosq(param.M_W / param.M_Z));

    param.beta = std::atan(data.getValue("MINPAR", 3).value());
    // param.mu = data.getValue("EXTPAR", 23).value();
    // param.M_1 = data.getValue("EXTPAR", 1).value();
    // param.M_2 = data.getValue("EXTPAR", 2).value();
    // param.A_b = data.getValue("EXTPAR", 12).value();
    // param.A_t = data.getValue("EXTPAR", 11).value();
    // param.M_A = data.getValue("EXTPAR", 26).value();
    // param.M_eL = data.getValue("EXTPAR", 31).value();
    // param.M_eR = data.getValue("EXTPAR", 34).value();
    // param.M_tauL = data.getValue("EXTPAR", 33).value();
    // param.M_tauR = data.getValue("EXTPAR", 36).value();
    // param.M_q1L = data.getValue("EXTPAR", 41).value();
    // param.M_q3L = data.getValue("EXTPAR", 43).value();
    // param.M_qbR = data.getValue("EXTPAR", 49).value();
    // param.M_qdR = data.getValue("EXTPAR", 47).value();
    // param.M_quR = data.getValue("EXTPAR", 44).value();
    // param.M_tauL = data.getValue("EXTPAR", 33).value();
    // param.M_qtR = data.getValue("EXTPAR", 46).value();
    // param.M_tauR = data.getValue("EXTPAR", 36).value();

    param.Finite = 1;
    param.reg_prop = 0;
    // param.reg_int = 1e-2;

    param.m_snu_e = data.getValue("MASS", 1000012).value();
    param.m_su_L = data.getValue("MASS", 1000002).value();
    param.m_su_R = data.getValue("MASS", 2000002).value();
    param.m_sc_L = data.getValue("MASS", 1000004).value();
    param.m_sc_R = data.getValue("MASS", 2000004).value();
    param.m_C1p = data.getValue("MASS", 1000024).value();
    param.m_C2p = data.getValue("MASS", 1000037).value();
    param.m_st_L = data.getValue("MASS", 1000006).value();
    param.m_st_R = data.getValue("MASS", 2000006).value();

    param.U_Wm1 = data.getValue("Umix", 1, 1).value();
    param.U_d1  = data.getValue("Umix", 1, 2).value();
    param.U_Wm2 = data.getValue("Umix", 2, 1).value();
    param.U_d2  = data.getValue("Umix", 2, 2).value();

    param.V_Wp1 = data.getValue("Vmix", 1, 1).value();
    param.V_u1  = data.getValue("Vmix", 1, 2).value();
    param.V_Wp2 = data.getValue("Vmix", 2, 1).value();
    param.V_u2  = data.getValue("Vmix", 2, 2).value();

    param.U_st_00 = data.getValue("stopmix", 1, 1).value();
    param.U_st_01 = data.getValue("stopmix", 2, 1).value();
    param.U_st_10 = data.getValue("stopmix", 1, 2).value();
    param.U_st_11 = data.getValue("stopmix", 2, 2).value();

    // updateSpectrum(param);
}

bool comp(
        std::string const &name,
        double a,
        double b
        )
{
    if (b == 0) {
        if (a != 0) {
            std::cout << name << " not good:" << '\n';
            std::cout << a << " != " << b << '\n';
            return false;
        }
    }
    else if (std::abs((b - a) / b) > 0.001) {
        std::cout << name << " has a discrepancy:" << '\n';
        std::cout << a << " != " << b << '\n';
        return false;
    }
    return true;
}

inline void readSpectrum2(
        c9_mssm::param_t         &param,
        mty::lha::LHAFileData const &data
        )
{
    double MZ = data.getValue("ADDIT", 0).value();
    // double MW = data.getValue("ADDIT", 1).value();
    // double gL = data.getValue("ADDIT", 2).value();
    // double gY = data.getValue("ADDIT", 3).value();
    double sw2 = data.getValue("ADDIT", 4).value();
    double m_st_L = data.getValue("ADDIT", 5).value();
    double m_st_R = data.getValue("ADDIT", 6).value();

    double m_C1p = data.getValue("ADDIT", 7).value();
    double m_C2p = data.getValue("ADDIT", 8).value();
    double U_Wm1  = data.getValue("ADDIT", 9).value();
    double U_d1  = data.getValue("ADDIT", 10).value();
    double U_Wm2  = data.getValue("ADDIT", 11).value();
    double U_d2  = data.getValue("ADDIT", 12).value();

    double V_Wp1  = data.getValue("ADDIT", 13).value();
    double V_u1  = data.getValue("ADDIT", 14).value();
    double V_Wp2  = data.getValue("ADDIT", 15).value();
    double V_u2  = data.getValue("ADDIT", 16).value();
    double U_st_00  = data.getValue("ADDIT", 17).value();
    double U_st_10  = data.getValue("ADDIT", 18).value();
    double U_st_01  = data.getValue("ADDIT", 19).value();

    double U_st_11  = data.getValue("ADDIT", 20).value();
    double m_u1 = data.getValue("ADDIT", 21).value();
    double m_u2 = data.getValue("ADDIT", 22).value();
    double m_sc_L = data.getValue("ADDIT", 23).value();
    double m_sc_R = data.getValue("ADDIT", 24).value();
    double tanb = data.getValue("ADDIT", 25).value();
    // double GF = data.getValue("SMINPUTS", 2).value();
    double alpha_em = data.getValue("SMINPUTS", 1).value();
    double e_em = std::sqrt(4*M_PI/alpha_em);
    double thetaW = std::asin(std::sqrt(sw2));


    param.M_A = data.getValue("EXTPAR", 26).value();
    param.e_em = e_em;
    //param.M_W = MW;
    param.M_W = MZ * std::cos(thetaW);
    param.M_Z = MZ;
    param.m_b = data.getValue("SMINPUTS", 5).value();
    param.m_s = 0.150;
    param.m_t = data.getValue("SMINPUTS", 6).value();
    param.m_mu = 0; // 0.1;
    param.theta_W = thetaW;

    param.beta = std::atan(tanb);

    param.Finite = 1;
    param.reg_prop = 0;
    // param.reg_int = 1e-2;

    param.m_snu_e = data.getValue("MASS", 1000012).value();
    // param.m_snu_mu = param.m_snu_e;

    param.m_su_L = m_u1;
    param.m_su_R = m_u2;

    param.m_sc_L = m_sc_L;
    param.m_sc_R = m_sc_R;

    param.m_C1p = m_C1p;
    param.m_C2p = m_C2p;

    param.m_st_L = m_st_L;
    param.m_st_R = m_st_R;

    param.U_Wm1 = U_Wm1;
    param.U_d1  = U_d1;
    param.U_Wm2 = U_Wm2;
    param.U_d2  = U_d2;

    param.V_Wp1 = V_Wp1;
    param.V_u1  = V_u1;
    param.V_Wp2 = V_Wp2;
    param.V_u2  = V_u2;

    param.U_st_00 = U_st_00;
    param.U_st_01 = U_st_01;
    param.U_st_10 = U_st_10;
    param.U_st_11 = U_st_11;
    // updateSpectrum(param);
}
