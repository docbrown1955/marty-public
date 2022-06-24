#include "c8_sm.h"
#include <fstream>

using namespace c8_sm;

double C8_SI(double x)
{
    double x3 = std::pow(x, 3);
    double x2 = std::pow(x, 2);
    double lnx = std::log(x);

    return -0.5*(
                 3*x2 / (2 * std::pow(1 - x, 4))*lnx\
               + (5 * x3 - 9*x2 + 30*x - 8) / (12 * std::pow(1 - x, 3))
            );
}

int main() {

    // double MW = 80.379;
    double mb = 4.18;
    double ms = 95e-3;
    double s12 = (mb*mb + ms*ms) / 2;

    param_t param;
    param.g_s = 0.1;
    param.m_b = mb;
    param.m_s = ms;
    param.e_em = 0.1;
    param.s_12 = s12;
    param.Finite = 1;
    // param.reg_int = 0;
    param.theta_W = 1;

    auto out = std::ofstream("C8_SM.txt");
    for (double mt = 50; mt <= 300; mt += 1) {
        param.m_t = mt;
        double c8m = C8(param).real();
        out << c8m << " " << std::endl; // C8_SI(mt*mt/MW/MW) << std::endl;
    }
    out.close();

    return 0;
}
