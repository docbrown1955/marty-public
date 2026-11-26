#include "c7_pmssm.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <complex>

using namespace c7_pmssm;

double MA  = 100;

template<class T>
std::string toString(T const &x)
{
    std::ostringstream sout;
    sout << x;
    return sout.str();
}

param_t params;

void setParam(
        const double tanb_par,
        const double M_2_par
        )
{
    params.M_W = 80.379;
    params.mu_h = 100;
    params.beta = std::atan(tanb_par);
    // params.reg_int = 0;

    params.M_1 = 50;
    params.M_2 = M_2_par;

    params.A_t = 500;
    params.A_b = 0.0;
    params.A_tau = 100;

    params.M_q1L = 1000;
    params.M_q3L = 1000;
    params.M_qbR = 1000;
    params.M_qdR = 1000;
    params.M_qtR = 1000;

    params.M_tauL = 3000;
    params.M_tauR = 3000;

    params.m_b = 4.18;
    params.m_s = 0.148;
    params.m_t = 173.34;
}

std::complex<double> C7_mu(
        const double M_2_par,
        const double mu_par
        )
{
    using namespace c7_pmssm;

    setParam(50, M_2_par);
    params.mu_h = mu_par;

    updateDiagonalization(params);

    params.s_12 = (params.m_b*params.m_b + params.m_s*params.m_s) / 2;

    auto res = c7_pmssm::C7(params);

    return res;
}

double getPoint(
        const double min,
        const double max,
        const size_t i,
        const size_t N
        )
{
    return min + (max - min) * i * 1. / N;
}

int main() {

    c7_pmssm::setMu(1000);
    constexpr double M2_min   = -1000;
    constexpr double M2_max   = 1000;
    constexpr double mu_min   = -800;
    constexpr double mu_max   = 800;
    constexpr size_t nPoints  = 50;

    setParam(2, 300);

    std::ofstream file("C7_marty_tanb50_ss.txt");
    file.precision(10);
    file << std::left;
    for (size_t i = 0; i != nPoints; ++i) {
        for (size_t j = 0; j != nPoints; ++j) {
            std::cout << "Iteration " << i * nPoints + j
                << " / " << nPoints * nPoints << std::endl;
            const double M2   = getPoint(M2_min,   M2_max,   i, nPoints); 
            const double mu   = getPoint(mu_min,   mu_max,   j, nPoints); 
            const std::complex<double> c7 = C7_mu(M2, mu);
            file << std::setw(20) << M2;
            file << std::setw(20) << mu;
            file << std::setw(20) << std::real(c7);
            file << std::setw(20) << std::imag(c7);
            file << std::endl;
        }
    }
    file.close();

    return 0;
}
