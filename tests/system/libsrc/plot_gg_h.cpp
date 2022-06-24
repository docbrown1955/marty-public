#include "widths_lib.h"
#include <fstream>

using namespace widths_lib;

int main() {

    param_t params;
    params.M_W = 80.379;
    params.e_em = std::sqrt(4*M_PI / 137.);
    params.g_s = std::sqrt(4*M_PI*0.110);
    params.M_Z = 91.2;
    params.theta_W = std::acos(params.M_W / params.M_Z);
    params.m_h = 125.1;
    params.m_t = 173;
    params.Finite = 1;
    const double gammaH = 4.2e-3;

    std::ofstream fout("data.txt");
    for (double ECM = 1; ECM <= 1000; ++ECM) {
        const double E = ECM / 2;
        const double m = 2*E;
        params.s_12 = 2*E;
        params.s_13 = m*E;
        params.s_23 = m*E;
        std::complex<double> ic = {0, 1};
        std::complex<double> prop = 1. / (m*m - params.m_h*params.m_h + ic*params.m_h*gammaH);
        double prop2 = (prop * std::conj(prop)).real();
        double factor = 0.38941e6 * 1./256;
        std::cout << factor * M2_hGG(params)*prop2 << '\n';
        params.m_t = 10;
        fout << ECM << " " << factor * M2_hGG(params).real()*prop2 << '\t';
        params.m_t = 100;
        fout << " " << factor * M2_hGG(params).real()*prop2 << '\t';
        params.m_t = 173;
        fout << " " << factor * M2_hGG(params).real()*prop2 << '\t';
        params.m_t = 1000;
        fout << " " << factor * M2_hGG(params).real()*prop2 << '\n';
    }
}
