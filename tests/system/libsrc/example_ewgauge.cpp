#include "ewgauge.h"
#include "kinematics.h"
#include <fstream>

using namespace ewgauge;

#ifdef QUAD
#define REAL(x) crealq(x)
#define ACOS(x) acosq(x)
#else
#define REAL(x) (x.real())
#define ACOS(x) std::acos(x)
#endif

std::pair<double, double> initKinematics(
        param_t &param,
        double   ECM,
        double   theta
        )
{
    Kinematics<2, 2> kinematics(
            (double)param.M_W, (double)param.M_Z, (double)param.M_W, (double)param.M_Z,
            ECM, theta
            );
    param.s_12 = kinematics.lorentzInvariant(0, 1);
    param.s_13 = kinematics.lorentzInvariant(0, 2);
    param.s_14 = kinematics.lorentzInvariant(0, 3);
    param.s_23 = kinematics.lorentzInvariant(1, 2);
    param.s_24 = kinematics.lorentzInvariant(1, 3);
    param.s_34 = kinematics.lorentzInvariant(2, 3);

    return {kinematics.pSquared(0), kinematics.pSquared(2)};
}

double xsec(double M2_int, double ECM, double pi2, double pf2)
{
    return 1 / (32*M_PI*M_PI*ECM*ECM) 
        * std::sqrt(std::abs(pf2/pi2)) * M2_int;
}

template<class Func>
double sigma(
        Func   &&f,
        param_t &param, 
        double   E_CM,
        size_t   Npts = 100)
{
    double integ = 0;
    for (size_t i = 0; i != Npts; ++i) {
        const double theta = i * M_PI/(Npts - 1);
        auto [pi2, pf2] = initKinematics(param, E_CM, theta);
        integ += xsec(std::sin(theta) * (double)REAL(f(param)), E_CM, pi2, pf2);
    }
    return integ;
}

int main() {

    param_t param;
    param.M_W = 80.379;
    param.M_Z = 91.10;
    param.e_em = std::sqrt(4*M_PI / 137.);
    param.m_h = 125.1;
    param.m_Gp = param.M_W;
    param.theta_W = ACOS(param.M_W / param.M_Z);
    param.reg_prop = 0;

    std::ofstream fout("data.txt");
    for (double E_CM = 500; E_CM <= 5000; E_CM += 100) {
        std::cout << E_CM << '\n';
        double m2t  = sigma(M2_tot, param, E_CM);
        double m2v3 = sigma(M2_V3, param, E_CM);
        double m2v4 = sigma(M2_V4, param, E_CM);
        double m2nh = sigma(M2_nH, param, E_CM);
        fout << E_CM << '\t';
        fout << m2v3 << '\t' << m2v4 << '\t' << m2nh << '\t' << m2t << '\n';
    }
    fout.close();

    return 0;
}
