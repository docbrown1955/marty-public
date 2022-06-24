#include "c7_sm.h"
#include <fstream>

using namespace c7_sm;

int main() {

    double mb = 4.18;
    double ms = 95e-3;
    double s12 = (mb*mb + ms*ms) / 2;
    param_t param;
    param.m_b = mb;
    param.m_s = ms;
    param.e_em = 0.01;
    param.s_12 = s12;
    // param.Finite = 1;
    // param.reg_int = 0;
    param.theta_W = 2;
    // param.reg_prop = 0;

    auto out = std::ofstream("C7_SM.txt");
    for (double mt = 50; mt <= 300; mt += 1) {
        param.m_t = mt;
        out << C7(param).real() << std::endl;
    }
    out.close();

    return 0;
}
