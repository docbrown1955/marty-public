#include "widths_lib.h"
#include <fstream>

using namespace widths_lib;

struct Kin_1_to_2 {
    double M0_2;
    double M1_2;
    double M2_2;
    double E1;
    double E2;
    double p2;

    static
    Kin_1_to_2 build(double const M0, double const M1, double const M2)
    {
        const auto M0_2 = M0*M0;
        const auto M1_2 = M1*M1;
        const auto M2_2 = M2*M2;
        const auto E1 = (M0_2 + M1_2 - M2_2) / (2 * M0);
        const auto E2 = (M0_2 - M1_2 + M2_2) / (2 * M0);
        const auto p2 = E2*E2 - M2_2;
        return Kin_1_to_2 { M0_2, M1_2, M2_2, E1, E2, p2 };
    }
};
// Fills the kinematics of 1 -> 2 for a param struct,
// uncomment the required quantities s_ij
template<class Param_t>
Kin_1_to_2 fillKinematics_1to2(
        Param_t     &param,
        double const M0,
        double const M1,
        double const M2
        )
{
    const auto kin = Kin_1_to_2::build(M0, M1, M2);

    param.s_12 = kin.E1*M0;
    param.s_13 = kin.E2*M0;
    param.s_23 = kin.E1*kin.E2 + kin.p2;

    return kin;
}

double width(
        double const      m2,
        const Kin_1_to_2 &kin
        )
{
     // Taking absolute value of p2 to ensure valid squared root even for 
     // impossible processes
    return 1 / (8 * M_PI) * m2 * std::sqrt(std::abs(kin.p2)) / kin.M0_2;
}

double GF(param_t const &param)
{
    double const e2 = std::pow(param.e_em, 2);
    double const MW2 = param.M_W * param.M_W;
    double const s2_thetaW = std::pow(std::sin(param.theta_W), 2);
    return std::sqrt(2) * e2 / (s2_thetaW * 8*MW2);
}

double Gamma_hbb(
        param_t const &param,
        double  const  mh
        )
{
    double const mb = param.m_b;
    return GF(param)*mb*mb*mh*3./(4*M_PI*std::sqrt(2))
        * std::pow(1 - 4*mb*mb/(mh*mh), 3./2);
}

double Gamma_hWW(
        param_t const &param, 
        double  const  mh, 
        double  const  MW
        )
{
    double const MW2 = MW*MW;
    double const mh2 = mh*mh;
    double const b2 = 1 - 4 * MW2 / mh2;
    double const b  = std::sqrt(std::abs(b2));
    double const a  = 1 - b2;
    return GF(param) * mh2*mh * b / (32*M_PI*std::sqrt(2)) * 
        (4 - 4*a + 3*a*a);
}

double Gamma_hZZ(
        param_t const &param, 
        double  const  mh,
        double  const  MZ
        )
{
    return Gamma_hWW(param, mh, MZ) / 2.;
}

double Gamma_Wlnu(
        param_t const &param
        )
{
    return std::sqrt(2) * GF(param) * std::pow(param.M_W, 3) / (12 * M_PI);
}

double Gamma_Wcb(
        param_t const &param
        )
{
    return std::sqrt(2) * GF(param) * std::pow(param.V_cb, 2) 
        * std::pow(param.M_W, 3) / (4 * M_PI);
}

double Gamma_Zss(
        param_t const &param
        )
{
    double const T3s = -1./2;
    double const Qs  = -1./3;
    double const s2_thetaW = std::pow(std::sin(param.theta_W), 2);
    return std::sqrt(2) * GF(param) * std::pow(param.M_Z, 3) / (2 * M_PI)
        * (std::pow(T3s - Qs*s2_thetaW, 2) 
            + std::pow(Qs*s2_thetaW, 2));
}

double f(double const tau)
{
    return std::pow(std::asin(std::sqrt(tau)), 2);
}

double Af(double const tau)
{
    const double tau2 = tau*tau;
    return 2 / tau2 * (tau + (tau - 1)*f(tau));
}

double AW(double const tau)
{
    const double tau2 = tau*tau;
    return -1. / tau2 * (2*tau2 + 3*tau + 3*(2*tau - 1)*f(tau));
}

double tau(double const mh, double const mt)
{
    return std::pow(mh / (2*mt), 2);
}

// arXiv : 0901.0927
double Gamma_hGG_1loop(
        param_t const &param
        )
{
    const double alpha_s = param.g_s*param.g_s / (4 * M_PI);
    const double coupling = alpha_s*alpha_s * GF(param)
        * std::pow(param.m_h, 3) / (16 * M_PI*M_PI*M_PI * std::sqrt(2));
    return coupling * std::pow(1./2 * Af(tau(param.m_h, param.m_t)), 2);
}

// arXiv : 0901.0927
double Gamma_hAA_t_1loop(
        param_t const &param
        )
{
    const double alpha_em = param.e_em*param.e_em / (4 * M_PI);
    const double coupling = alpha_em*alpha_em * GF(param)
        * std::pow(param.m_h, 3) / (128 * M_PI*M_PI*M_PI * std::sqrt(2));
    return coupling 
        * std::pow(3*std::pow(2./3, 2)*Af(tau(param.m_h, param.m_t)), 2);
}

// arXiv : 0901.0927
double Gamma_hAA_W_1loop(
        param_t const &param
        )
{
    const double alpha_em = param.e_em*param.e_em / (4 * M_PI);
    const double coupling = alpha_em*alpha_em * GF(param)
        * std::pow(param.m_h, 3) / (128 * M_PI*M_PI*M_PI * std::sqrt(2));
    return coupling * std::pow(AW(tau(param.m_h, param.M_W)), 2);
}


int main() {

    double const mt  = 173;
    double const mh  = 125;
    double const MW  = 80;
    double const MZ  = 90;
    double       mb  = 4;
    double const mc  = 0;
    double const ms = 0;
    double const mmu = 0;
    param_t param;
    param.m_b = mb;
    param.M_W = MW;
    param.M_Z = MZ;
    param.e_em  = std::sqrt(4*M_PI*1./137);
    param.V_cb = 0.0412;
    param.theta_W = std::acos(MW / MZ);
    param.m_h = mh;
    param.m_t = mt;
    param.g_s = std::sqrt(4*M_PI*0.01);
    // param.I_eps = 0;
    param.Finite = 1;

    std::ofstream fout("data.txt");
    fout.precision(10);

    /// // H -> bb
    /// auto kin = fillKinematics_1to2(param, mh, mb, mb);
    /// fout << width(M2_hbb(param).real(), kin) << '\n';
    /// // fout << Gamma_hbb(param, mh) << '\n';
    /// fout << '\n';

    /// // H -> WW
    /// kin = fillKinematics_1to2(param, mh, MW, MW);
    /// fout << width(M2_hWW(param).real(), kin) << '\n';
    /// // fout << Gamma_hWW(param, mh, MW) << '\n';
    /// fout << '\n';

    /// // H -> ZZ
    /// kin = fillKinematics_1to2(param, mh, MZ, MZ);
    /// fout << width(M2_hZZ(param).real(), kin) << '\n';
    /// // fout << Gamma_hZZ(param, mh, MZ) << '\n';
    /// fout << '\n';

    /// // W -> lnu
    /// kin = fillKinematics_1to2(param, MW, 0, mmu);
    /// fout << width(M2_Wlnu(param).real(), kin) << '\n';
    /// // fout << Gamma_Wlnu(param) << '\n';
    /// fout << '\n';

    /// mb = 0;
    /// param.m_b = mb;
    /// // W -> cb
    /// kin = fillKinematics_1to2(param, MW, mc, mb);
    /// fout << width(M2_Wcb(param).real(), kin) << '\n';
    /// // fout << Gamma_Wcb(param) << '\n';
    /// fout << '\n';

    /// // Z -> ss
    /// kin = fillKinematics_1to2(param, MZ, ms, ms);
    /// fout << width(M2_Zss(param).real(), kin) << '\n';
    /// // fout << Gamma_Zss(param) << '\n';
    /// fout << '\n';

    /// // h -> AA
    /// kin = fillKinematics_1to2(param, mh, 0, 0);
    /// fout << width(M2_hAA_t(param).real(), kin) << '\n';
    /// // fout << Gamma_hAA_t_1loop(param) << '\n';
    /// fout << '\n';

    // h -> AA
    auto kin = fillKinematics_1to2(param, mh, 0, 0);
    for (double MW = 70; MW < 300; ++MW) {
        param.M_W = MW;
        param.M_Z = MW / std::cos(param.theta_W);
        fout << MW << ' ';
        fout << width(M2_hAA_W(param).real(), kin) << ' ';
        fout << Gamma_hAA_W_1loop(param) << '\n';
        fout << '\n';
    }

    // h -> GG
    // kin = fillKinematics_1to2(param, mh, 0, 0);
    // fout << width(M2_hGG(param).real(), kin) << '\n';
    // // fout << Gamma_hGG_1loop(param) << '\n';
    // fout << '\n';

    return 0;
}
