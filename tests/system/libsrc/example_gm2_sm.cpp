#include "gm2_sm.h"
#include <fstream>
#include "clooptools.h"
#include "marty/core/looptools_init.h"
#include "marty/core/looptools_interface.h"

using namespace gm2_sm;

// PDG
double gm2_theo(param_t const &param)
{
  auto GF = param.alpha_em*4*M_PI *std::sqrt(2)/(8*powq(param.M_W*sinq(param.theta_W), 2));
  return GF * (double)(param.m_e*param.m_e) / (8*std::sqrt(2)*M_PI*M_PI)
    * (5./3 + 1./3 * powq(1 - 4*powq(sinq(param.theta_W), 2), 2));
}

int main() {

  // std::cout.precision(10);
  // std::cout << mty::lt::C0iC(18, 1, 1.0001, 0, 1.56e6, 1, 1, mty::lt::reg_int) << '\n';
  //return 0;
  param_t param;
  param.alpha_em = 1./137;
  param.M_W = 80.379;
  param.M_Z = 91.2;
  param.m_Gp = 80.379;
  param.m_G0 = 91.2;
  param.theta_W = acosq(param.M_W / param.M_Z);
  std::ofstream real("real.txt");
  std::ofstream imag("imag.txt");

  getIntegrationParameters()->reg_int = 1e-8;
  int n1(0), n2(0);
  // Numerical instability partly regularized below 10^(-4)
    for (double exp_me = -4; exp_me <= +10; exp_me += 0.1) {
        if (n1 == 1) ++n2;
        param.m_e = std::pow(10, exp_me);
        setmudim(param.m_e);
        param.s_12 = param.m_e*param.m_e;
        real << (double)param.m_e << '\t' << crealq(gm2_e(param)) << '\t' 
             << crealq(gm2_eW(param)) << '\t' << param.alpha_em/(2*M_PI) << '\t' << gm2_theo(param) << '\n';
    }
  real.close();
  imag.close();

    return 0;
}
