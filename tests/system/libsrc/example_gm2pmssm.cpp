#include "gm2pmssm.h"
#include <fstream>
#include <iomanip>
#include "marty/core/looptools_interface.h"

using namespace gm2pmssm;

#ifdef QUAD
    #define POW(x, n) powq(x, n)
    #define COS(x)  cosq(x)
    #define ACOS(x) acosq(x)
    #define SIN(x)  sinq(x)
    #define LOG(x)  logq(x)
    #define REAL(x) crealq(x)
    #define IMAG(x) cimagq(x)
    #define CONJ(x) conjq(x)
#else
    #define POW(x, n)  std::pow(x, n)
    #define COS(x)  std::cos(x)
    #define ACOS(x)  std::acos(x)
    #define SIN(x)  std::sin(x)
    #define LOG(x)  std::log(x)
    #define REAL(x) (x).real()
    #define IMAG(x) (x).imag()
    #define CONJ(x) std::conj(x)
#endif

void setParam(
        param_t         &params
        )
{
    params.e_em = std::sqrt(4*M_PI * 1./137);
    params.M_W = 80.379;
    params.M_Z = 91.2;
    params.theta_W = ACOS(params.M_W / params.M_Z);
    params.m_mu = 1e-2;
    params.mu_h = 100;
    params.beta = std::atan(10);
    params.s_12 = params.m_mu*params.m_mu;

    params.M_1 = 50;
    params.M_2 = 100;
    params.m_snu_e = 250;
    params.M_A = 250;

    params.A_t = 500;
    params.A_b = 0.;
    params.A_tau = 500;

    params.M_q1L = 1000;
    params.M_q3L = 1000;
    params.M_qbR = 1000;
    params.M_qdR = 1000;
    params.M_qtR = 1000;
    params.M_quR = 1000;

    params.M_eL = 250;
    params.M_eR = 250;
    params.M_tauL = 3000;
    params.M_tauR = 3000;
}

// Source : hep-ph/0103067
complex_t yu(param_t const &param)
{
    return param.m_mu * param.e_em / (std::sqrt(2) * param.M_W * SIN(param.theta_W)
        * COS(param.beta));
}

complex_t nR(param_t const &param, int i, int m)
{
    if (m == 2) {
        return std::sqrt(2) * param.e_em / COS(param.theta_W)
            * (&param.N_B1 + i-1)->get();
    }
    else if (m == 1)
        return yu(param) * (&param.N_d1 + i-1)->get();
    else throw 3;
}

complex_t nL(param_t const &param, int i, int m)
{
    if (m == 1) {
        return 1/std::sqrt(2) * param.e_em * (
               (&param.N_B1 + i-1)->get() / COS(param.theta_W)
               + (&param.N_W1 + i-1)->get() / SIN(param.theta_W));
    }
    else if (m == 2)
        return -yu(param) * (&param.N_d1 + i-1)->get();
    else throw 3;
}

complex_t cL(param_t const &param, int k)
{
    return -param.e_em / SIN(param.theta_W) 
        * (complex_t)*(&param.V_Wp1 + k-1);
}

complex_t cR(
        param_t const &param, 
        int            k
        )
{
    return yu(param) * (complex_t)*(&param.U_d1 + k-1);
}

real_t m_charg(param_t const &param, int i)
{
    return (&param.m_C1p + i-1)->get();
}

real_t m_neut(param_t const &param, int i)
{
    return (&param.m_N_1 + i-1)->get();
}

real_t m_smu(param_t const &param, int i)
{
    return (&param.m_smu_L + i-1)->get();
}

// Source : hep-ph/0103067
real_t F1N(real_t const x)
{
    real_t const x2 = x*x;
    real_t const x3 = x2*x;
    return 2 * (1 - 6*x + 3*x2 + 2*x3 - 6*x2*LOG(x)) / POW(1 - x, 4);
}

real_t F2N(real_t const x)
{
    real_t const x2 = x*x;
    return 3 * (1 - x2 + 2*x*LOG(x)) / POW(1 - x, 3);
}

real_t F1C(real_t const x)
{
    real_t const x2 = x*x;
    real_t const x3 = x2*x;
    return 2 * (2 + 3*x - 6*x2 + x3 + 6*x*LOG(x)) / POW(1 - x, 4);
}

real_t F2C(real_t const x)
{
    real_t const x2 = x*x;
    return -3./2 * (3 - 4*x + x2 + 2*LOG(x)) / POW(1 - x, 3);
}

// Source : hep-ph/0103067 , (g-2)_µ = 2*a_µ
real_t gmu_N(param_t const &param)
{
    real_t const m_mu = param.m_mu;
    real_t sum = 0;
    for (int i = 1; i <= 4; ++i) {
        for (int m = 1; m <= 2; ++m) {
            auto nl = nL(param, i, m);
            auto nr = nR(param, i, m);
            auto nl2 = REAL(nl * CONJ(nl));
            auto nr2 = REAL(nr * CONJ(nr));
            auto re_nlr = REAL(nl * CONJ(nr));
            auto xim = POW(m_neut(param, i) / m_smu(param, m), 2);
            sum -= m_mu / (12 * m_smu(param, m)*m_smu(param, m))*(nl2 + nr2)*F1N(xim);
            sum += m_neut(param,i)/POW(m_smu(param, m), 2) / (3)*re_nlr*F2N(xim);
        }
    }
    return m_mu / (16*M_PI*M_PI) * sum;
}

// Source : hep-ph/0103067
real_t gmu_C(param_t const &param)
{
    real_t const m_mu = param.m_mu;
    real_t sum = 0;
    for (int k = 1; k <= 2; ++k) {
        auto cl = cL(param, k);
        auto cr = cR(param, k);
        auto cl2 = REAL(cl * CONJ(cl));
        auto cr2 = REAL(cr * CONJ(cr));
        auto re_clr = REAL(cl * CONJ(cr));
        auto xk = POW(m_charg(param, k) / param.m_snu_e, 2);
        sum += m_mu / (12 * param.m_snu_e*param.m_snu_e)*(cl2 + cr2)*F1C(xk);
        sum += 2*xk / (3*m_charg(param, k))*re_clr*F2C(xk);
    }
    return m_mu / (16*M_PI*M_PI) * sum;
}

int main() {

    param_t params;
    setParam(params);

    constexpr size_t N = 50;
    std::array<real_t, N> M2;
    std::array<real_t, N> mu;
    for (size_t i = 0; i != N; ++i) {
        M2[i] = -1000 + 2000*i*1./(N-1);
        mu[i] = -1000 + 2000*i*1./(N-1);
    }

    // std::ostream &out = std::cout;
    std::ofstream out("data.txt");
    out.precision(10);
    out << std::left;
    setMu(params.m_mu);
    for (size_t i_mu = 0; i_mu != N; ++i_mu) {
        for (size_t i_M2 = 0; i_M2 != N; ++i_M2) {
            params.mu_h  = mu[i_mu];
            params.M_2 = M2[i_M2];
            updateSpectrum(params);
            real_t gm2_MN = REAL(gm2_N(params));
            real_t gm2_MC = REAL(gm2_C(params));
            real_t gm2_TN = gmu_N(params);
            real_t gm2_TC = gmu_C(params);
            out << std::setw(15) << REAL(complex_t(params.mu_h)) 
                << std::setw(15) << REAL(complex_t(params.M_2));
            out << std::setw(18) << gm2_MN << std::setw(18) << gm2_MC;
            out << std::setw(18) << gm2_TN << std::setw(18) << gm2_TC;
            out << '\n';
        }
    }

    return 0;
}
