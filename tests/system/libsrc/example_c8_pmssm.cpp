#include "c8_pmssm.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace c8_pmssm;

__float128 MA  = 100;

template<class T>
std::string toString(T const &x)
{
    std::ostringstream sout;
    sout << x;
    return sout.str();
}

param_t params;

void setParam(
        const __float128 tanb_par,
        const __float128 M_2_par
        )
{
    params.M_W = 80.379;
    params.mu_h = 100;
    params.beta = atanq(tanb_par);

    params.M_1 = 50;
    params.M_2 = M_2_par;

    params.A_t = 500;
    params.A_b = 0;
    params.A_tau = 500;

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

    // params.reg_int  = 0;
    // params.reg_prop = 0;
}

__complex128 C8_mu(
        const __float128 M_2_par,
        const __float128 mu_par
        )
{
    using namespace c8_pmssm;

    setParam(50, M_2_par);
    params.mu_h = mu_par;

    updateDiagonalization(params);

    params.s_12 = (params.m_b*params.m_b + params.m_s*params.m_s) / 2;

    auto res = c8_pmssm::C8(params);

    return res;
}

__float128 getPoint(
        const __float128 min,
        const __float128 max,
        const size_t i,
        const size_t N
        )
{
    return min + (max - min) * i * 1. / N;
}

int main() {

    c8_pmssm::setMu(1000);
    constexpr __float128 M2_min   = -1000;
    constexpr __float128 M2_max   = 1000;
    constexpr __float128 mu_min   = -800;
    constexpr __float128 mu_max   = 800;
    constexpr size_t nPoints  = 50;

    setParam(2, 300);

    std::ofstream file("C8_marty_tanb50_ss.txt");
    file.precision(10);
    file << std::left;
    for (size_t i = 0; i != nPoints; ++i) {
        for (size_t j = 0; j != nPoints; ++j) {
            std::cout << "Iteration " << i * nPoints + j
                << " / " << nPoints * nPoints << std::endl;
            const __float128 M2   = getPoint(M2_min,   M2_max,   i, nPoints); 
            const __float128 mu   = getPoint(mu_min,   mu_max,   j, nPoints); 
            const __complex128 c8 = C8_mu(M2, mu);
            file << std::setw(20) << (double)crealq(c8);
            file << std::setw(20) << (double)cimagq(c8);
            file << std::endl;
        }
    }
    file.close();

    return 0;
}
