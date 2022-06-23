#include "zmumu.h"
#include "kinematics.h"
#include <fstream>

using namespace zmumu;

constexpr double nanoBarnConv = 2.56819e-6;

template<class Callable>
auto dsigmaCalc(
        param_t     &params,
        Callable   &&m2,
        const double E_CM,
        const double theta,
        bool         forward = true
        )
{
    Kinematics<2, 2> ee_to_mumu(0, 0, 0, 0, E_CM, theta, forward);
    const auto pf = ee_to_mumu.pSquared(2);
    const auto pi = ee_to_mumu.pSquared(0);
    params.s_12 = ee_to_mumu.lorentzInvariant(0, 1);
    params.s_13 = ee_to_mumu.lorentzInvariant(0, 2);
    params.s_14 = ee_to_mumu.lorentzInvariant(0, 3);
    params.s_23 = ee_to_mumu.lorentzInvariant(1, 2);
    params.s_24 = ee_to_mumu.lorentzInvariant(1, 3);
    //params.s_34 = ee_to_mumu.lorentzInvariant(2, 3);
    return 1. / (64 * M_PI*M_PI * E_CM*E_CM) * pf/pi * m2(params).real()
        / nanoBarnConv;
}

template<class Callable>
auto dsigmaCalcBack(
        param_t     &params,
        Callable   &&m2,
        const double E_CM,
        const double theta
        )
{
    return dsigmaCalc(params, std::forward<Callable>(m2), E_CM, theta, false);
}

int main(int argc, char const *argv[]) {

    const double e_em = std::sqrt(4 * M_PI * 1./137);
    const double MW  = 80.379;
    const double MZ  = 91.19;
    const double GZ  = 2.49;
    const double theta_W = std::acos(MW / MZ);
    const double cW = std::cos(theta_W);
    const double s2W = std::pow(std::sin(theta_W), 2);
    const double g_L = e_em / std::sin(theta_W);
    const double a_L = g_L / cW * (-1./2 + s2W);
    const double a_R = g_L / cW * s2W;
    param_t param;
    param.g_Y = e_em;
    param.aL = a_L;
    param.aR = a_R;
    param.M_Z  = MZ;
    param.G_Z  = GZ;
    param.reg_prop = 0;

    enum Mode {
        TotalXSec = 0,
        AFB,
        eLR
    };
    if (argc < 2) {
        std::cerr << "Argument missing !" << '\n';
        return 1;
    }
    Mode mode = static_cast<Mode>(TotalXSec + atoi(argv[1]));
    std::ofstream fout("data.txt");
    constexpr double dtheta = M_PI/100;
    switch(mode) {
        case TotalXSec:
            for (double E_CM = 4; E_CM < 150; E_CM += 0.1) {
                double sigmaF = 0;
                double sigmaB = 0;
                for (double theta = 0; theta < M_PI; theta += dtheta) {
                    const auto dsigmaF = dsigmaCalc(param, Zmumu_full, E_CM, theta);
                    const auto dsigmaB = dsigmaCalcBack(param, Zmumu_full, E_CM, theta);
                    const auto factor = dtheta*std::sin(theta)*2*M_PI;
                    sigmaF += factor*dsigmaF;
                    sigmaB += factor*dsigmaB;
                }
                fout <<  E_CM << " " << sigmaF << '\n';
            }
            break;
        case AFB:
            for (double E_CM = 55; E_CM < 185; E_CM += 0.1) {
                double sigmaF = 0;
                double sigmaB = 0;
                const double theta = M_PI / 3;
                const auto dsigmaF = dsigmaCalc(param, Zmumu_full, E_CM, theta);
                const auto dsigmaB = dsigmaCalcBack(param, Zmumu_full, E_CM, theta);
                sigmaF += dsigmaF;
                sigmaB += dsigmaB;
                fout <<  E_CM << " " << (sigmaF - sigmaB)/(sigmaB + sigmaF) << '\n';
            }
            break;
        case eLR:
            {
            const double E_CM = 87;
            auto maxtheta = std::acos(0.9);
            for (double theta = maxtheta; theta < M_PI-maxtheta; theta += dtheta) {
                const double factor = 2*M_PI;
                const auto m2L = factor*dsigmaCalc(param, Zmumu_L, E_CM, M_PI-theta);
                const auto m2R = factor*dsigmaCalc(param, Zmumu_R, E_CM, M_PI-theta);
                fout << E_CM << " " << theta << " " << m2L << " " << m2R << '\n';
            }
            }
            break;
        default:
            std::cerr << "Bad mode given in argument" << '\n';
            return 2;
    }
    return 0;
}
