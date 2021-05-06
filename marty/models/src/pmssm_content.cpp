// This file is part of MARTY.
// 
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "marty.h"
#include "pmssm_lem.h"

namespace mty {

PMSSM_LEM::PMSSM_LEM()
    :mty::Model(),
    mty::PMSSM_data(mty::dirac4, csl::Minkowski)

{
    setEnableChecks(false);
    initContent();
    initKinetic();
    initMass();
    initInteractions();
    initSpectrum();
}

void PMSSM_LEM::initContent()
{
    gamma  = mty::dirac4.gamma;
    gamma5 = mty::dirac4.gamma_chir;
    P_L    = mty::dirac4.P_L;
    P_R    = mty::dirac4.P_R;
    sigma  = mty::dirac4.sigma;
    
    ///////////////////////////////////
    // Gauge definition              //
    ///////////////////////////////////
    
    
    g_s = sm_input::g_s;
    addGaugedGroup(
        mty::group::Type::SU,
        "C",
        3,
        g_s
        );
    C = getGauge()->getGaugedGroup(0);
    
    
    
    ///////////////////////////////////
    // Flavor definition             //
    ///////////////////////////////////
    
    init(false);
    
    ///////////////////////////////////
    // Particle masses definitions   //
    ///////////////////////////////////
    
    M_3 = mty::mssm_input::M3;
    M_2 = mty::mssm_input::M2;
    M_A = mty::mssm_input::MA;

    M_W = mty::sm_input::M_W;

    m_A0 = M_A;
    theta_W = mty::sm_input::theta_W;
    _beta = csl::atan_s(mty::mssm_input::tanb);
    M_Z = sm_input::M_Z;
    csl::Expr m_H_p_encaps = csl::pow_s(csl::sum_s({csl::pow_s(M_A, 2) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_W, 2)}) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_Z, 2)}) , csl::prod_s({csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 4)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 4)}) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 4), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_Hp = csl::Abbrev::makeAbbreviation("m_H^+", m_H_p_encaps);
    alpha = csl::constant_s("\\alpha");
    csl::Expr m_h_0_encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::pow_s(M_A, 2), csl::pow_s(csl::cos_s(alpha), 2)}) , csl::prod_s({-1
, csl::pow_s(M_A, 2), csl::pow_s(csl::cos_s(alpha), 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::cos_s(alpha), 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({2
, csl::pow_s(M_A, 2), csl::cos_s(_beta), csl::cos_s(alpha), csl::sin_s(_beta), csl::sin_s(alpha)}) , csl::prod_s({2
, csl::pow_s(M_Z, 2), csl::cos_s(_beta), csl::cos_s(alpha), csl::sin_s(_beta), csl::sin_s(alpha)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(alpha), 2)}) , csl::prod_s({csl::pow_s(M_A, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(alpha), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(alpha), 2)})}), csl::intfraction_s(1, 2));
    m_h0 = csl::Abbrev::makeAbbreviation("m_h^0", m_h_0_encaps);
    csl::Expr m_H_0_encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::cos_s(alpha), 2)}) , csl::prod_s({csl::pow_s(M_A, 2), csl::pow_s(csl::cos_s(alpha), 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::cos_s(alpha), 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, csl::pow_s(M_A, 2), csl::cos_s(_beta), csl::cos_s(alpha), csl::sin_s(_beta), csl::sin_s(alpha)}) , csl::prod_s({-2
, csl::pow_s(M_Z, 2), csl::cos_s(_beta), csl::cos_s(alpha), csl::sin_s(_beta), csl::sin_s(alpha)}) , csl::prod_s({csl::pow_s(M_A, 2), csl::pow_s(csl::sin_s(alpha), 2)}) , csl::prod_s({-1
, csl::pow_s(M_A, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(alpha), 2)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(alpha), 2)})}), csl::intfraction_s(1, 2));
    m_H0 = csl::Abbrev::makeAbbreviation("m_H^0", m_H_0_encaps);
    m_t = mty::sm_input::m_t;
    m_s = mty::sm_input::m_s;
    m_b = mty::sm_input::m_b;
    m_tau = mty::sm_input::m_tau;
    M_q1L = mty::mssm_input::Mq1L;
    csl::Expr m_su_L_encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_W, 2)}) , csl::pow_s(M_q1L, 2) , csl::prod_s({-1
, csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(-1, 6), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(1, 3), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_su_L = csl::Abbrev::makeAbbreviation("m_~u_L", m_su_L_encaps);
    M_quR = mty::mssm_input::MquR;
    csl::Expr m_su_R_encaps = csl::pow_s(csl::sum_s({csl::pow_s(M_quR, 2) , csl::prod_s({csl::intfraction_s(-2, 3), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(4, 3), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_su_R = csl::Abbrev::makeAbbreviation("m_~u_R", m_su_R_encaps);
    delta_wolf = mty::sm_input::delta_wolf;
    V_ts = mty::sm_input::V_ts;
    V_tb = mty::sm_input::V_tb;
    V_ub = mty::sm_input::V_ub;
    V_ub_mod = mty::sm_input::V_ub_mod;
    V_us = mty::sm_input::V_us;
    V_cs = mty::sm_input::V_cs;
    V_cb = mty::sm_input::V_cb;
    csl::Expr m_sd_L_encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::intfraction_s(-3, 2), csl::pow_s(M_W, 2)}) , csl::pow_s(M_q1L, 2) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 4), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_us, 4), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, csl::pow_s(V_cb, 2), V_cs, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_tb, V_ts, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_cs, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_tb, V_ts, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_cs, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_tb, V_ts, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_us, V_ub, csl::GetComplexConjugate(V_cs), csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_us, V_ub, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({3
, csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 4), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, csl::pow_s(V_cb, 2), V_cs, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_tb, V_ts, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_cs, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_tb, V_ts, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_us, 4), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_cs, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_tb, V_ts, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_us, V_ub, csl::GetComplexConjugate(V_cs), csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_us, V_ub, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_sd_L = csl::Abbrev::makeAbbreviation("m_~d_L", m_sd_L_encaps);
    m_ss_L = m_sd_L;
    M_qdR = mty::mssm_input::MqdR;
    csl::Expr m_sd_R_encaps = csl::pow_s(csl::sum_s({csl::pow_s(M_qdR, 2) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 4), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, csl::GetComplexConjugate(V_cs), V_tb, V_ts, csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, csl::pow_s(V_cb, 2), V_cs, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_tb, csl::GetComplexConjugate(V_tb), V_ts, csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({csl::pow_s(m_s, 2), csl::pow_s(V_us, 4), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_cs, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, V_cs, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_tb, V_ts, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, V_cb, csl::GetComplexConjugate(V_cs), V_us, V_ub, csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({2
, m_b, m_s, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), V_us, V_ub, csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2))}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 4), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_cb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_b, 2), csl::pow_s(V_tb, 2), csl::pow_s(csl::GetComplexConjugate(V_tb), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, csl::GetComplexConjugate(V_cs), V_tb, V_ts, csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::GetComplexConjugate(V_cs), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_tb, csl::GetComplexConjugate(V_tb), V_ts, csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_cs, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, csl::pow_s(V_cb, 2), V_cs, csl::GetComplexConjugate(V_cs), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_ts, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_cs, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_us, 4), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-1
, csl::pow_s(m_s, 2), csl::pow_s(V_us, 2), csl::pow_s(csl::GetComplexConjugate(V_ts), 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, V_cs, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_tb, V_ts, V_us, V_ub, csl::exp_s(csl::prod_s({CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, V_cb, csl::GetComplexConjugate(V_cs), V_us, V_ub, csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({-2
, m_b, m_s, csl::GetComplexConjugate(V_tb), csl::GetComplexConjugate(V_ts), V_us, V_ub, csl::exp_s(csl::prod_s({-1
, CSL_I, delta_wolf})), csl::pow_s(csl::cos_s(_beta), (-2)), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({-2
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_sd_R = csl::Abbrev::makeAbbreviation("m_~d_R", m_sd_R_encaps);
    m_ss_R = m_sd_R;
    M_eL = mty::mssm_input::MeL;
    csl::Expr m_se_L_encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_W, 2)}) , csl::pow_s(M_eL, 2) , csl::prod_s({csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_se_L = csl::Abbrev::makeAbbreviation("m_~e_L", m_se_L_encaps);
    M_eR = mty::mssm_input::MeR;
    csl::Expr m_se_R_encaps = csl::pow_s(csl::sum_s({csl::pow_s(M_eR, 2) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({-2
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::intfraction_s(1, 2));
    m_se_R = csl::Abbrev::makeAbbreviation("m_~e_R", m_se_R_encaps);
    csl::Expr m_s_nu__eL__encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_Z, 2)}) , csl::pow_s(M_eL, 2) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2)})}), csl::intfraction_s(1, 2));
    m_snu_e_L = csl::Abbrev::makeAbbreviation("m_~\\nu_{eL}", m_s_nu__eL__encaps);
    M__tauL = mty::mssm_input::MtauL;
    csl::Expr m_s_nu___tau_L__encaps = csl::pow_s(csl::sum_s({csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_Z, 2)}) , csl::pow_s(M__tauL, 2) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2)})}), csl::intfraction_s(1, 2));
    m_snu_tau_L = csl::Abbrev::makeAbbreviation("m_~\\nu_{\\tau L}", m_s_nu___tau_L__encaps);
    m_N_1 = csl::constant_s("m_N_1");
    m_N_2 = csl::constant_s("m_N_2");
    m_N_3 = csl::constant_s("m_N_3");
    m_N_4 = csl::constant_s("m_N_4");
    m_C1p = csl::constant_s("m_C1");
    m_C2p = csl::constant_s("m_C2");
    m_st_L = csl::constant_s("m_st1");
    m_st_R = csl::constant_s("m_st2");
    m_stau_L = csl::constant_s("m_stau1");
    m_stau_R = csl::constant_s("m_stau2");
    m_sb_L = csl::constant_s("m_sb1");
    m_sb_R = csl::constant_s("m_sb2");
    
    ///////////////////////////////////
    // Particle definitions          //
    ///////////////////////////////////
    
    G     = C->buildVectorBoson();
    G->setSelfConjugate(true);
    G->setGroupRep("C", {1, 1});
    G->setName("G");
    addParticle(G, false);
    F_G     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(G.get()));
    G->setFieldStrength(F_G);
    
    W     = mty::vectorboson_s("W ; W^+", *this);
    W->setMass(M_W);
    addParticle(W, false);
    F_W = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(W.get()));
    W->setFieldStrength(F_W);
    
    A     = mty::vectorboson_s("A ; A", *this);
    A->setSelfConjugate(true);
    addParticle(A, false);
    F_A     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(A.get()));
    A->setFieldStrength(F_A);
    
    Z     = mty::vectorboson_s("Z ; Z", *this);
    Z->setSelfConjugate(true);
    Z->setMass(M_Z);
    addParticle(Z, false);
    F_Z     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(Z.get()));
    Z->setFieldStrength(F_Z);
    
    c_A_C = C->buildGhost();
    addParticle(c_A_C, false);
       
    c_W_1 = mty::ghostboson_s("c_W_1", std::dynamic_pointer_cast<mty::VectorBoson>(W));
    addParticle(c_W_1, false);
    
    c_W_2 = mty::ghostboson_s("c_W_2", std::dynamic_pointer_cast<mty::VectorBoson>(W));
    addParticle(c_W_2, false);
    
    c_W_3 = mty::ghostboson_s("c_W_3", std::dynamic_pointer_cast<mty::VectorBoson>(W));
    addParticle(c_W_3, false);
    
    A0 = mty::scalarboson_s("A0 ; A^0", *this);
    A0->setSelfConjugate(true);
    A0->setMass(m_A0);
    addParticle(A0, false);
    
    Hp = mty::scalarboson_s("Hp ; H^+", *this);
    Hp->setMass(m_Hp);
    addParticle(Hp, false);
    
    h0 = mty::scalarboson_s("h0 ; h^0", *this);
    h0->setSelfConjugate(true);
    h0->setMass(m_h0);
    addParticle(h0, false);
    
    H0 = mty::scalarboson_s("H0 ; H^0", *this);
    H0->setSelfConjugate(true);
    H0->setMass(m_H0);
    addParticle(H0, false);
    
    su_L = mty::scalarboson_s("su_L ; \\tilde{u_L}", *this);
    su_L->setMass(m_su_L);
    su_L->setGroupRep("C", {1, 0});
    addParticle(su_L, false);
    
    sc_L = mty::scalarboson_s("sc_L ; \\tilde{c_L}", *this);
    sc_L->setMass(m_su_L);
    sc_L->setGroupRep("C", {1, 0});
    addParticle(sc_L, false);
    
    su_R = mty::scalarboson_s("su_R ; \\tilde{u_R}", *this);
    su_R->setMass(m_su_R);
    su_R->setGroupRep("C", {1, 0});
    addParticle(su_R, false);
    
    sc_R = mty::scalarboson_s("sc_R ; \\tilde{c_R}", *this);
    sc_R->setMass(m_su_R);
    sc_R->setGroupRep("C", {1, 0});
    addParticle(sc_R, false);
    
    sd_L = mty::scalarboson_s("sd_L ; \\tilde{d_L}", *this);
    sd_L->setMass(m_sd_L);
    sd_L->setGroupRep("C", {1, 0});
    addParticle(sd_L, false);
    
    ss_L = mty::scalarboson_s("ss_L ; \\tilde{s_L}", *this);
    ss_L->setMass(m_ss_L);
    ss_L->setGroupRep("C", {1, 0});
    addParticle(ss_L, false);
    
    sd_R = mty::scalarboson_s("sd_R ; \\tilde{d_R}", *this);
    sd_R->setMass(m_sd_R);
    sd_R->setGroupRep("C", {1, 0});
    addParticle(sd_R, false);
    
    ss_R = mty::scalarboson_s("ss_R ; \\tilde{s_R}", *this);
    ss_R->setMass(m_ss_R);
    ss_R->setGroupRep("C", {1, 0});
    addParticle(ss_R, false);
    
    se_L = mty::scalarboson_s("se_L ; \\tilde{e_L}", *this);
    se_L->setMass(m_se_L);
    addParticle(se_L, false);
    
    smu_L = mty::scalarboson_s("smu_L ; \\tilde{\\mu_L}", *this);
    smu_L->setMass(m_se_L);
    addParticle(smu_L, false);
    
    se_R = mty::scalarboson_s("se_R ; \\tilde{e_R}", *this);
    se_R->setMass(m_se_R);
    addParticle(se_R, false);
    
    smu_R = mty::scalarboson_s("smu_R ; \\tilde{\\mu_R}", *this);
    smu_R->setMass(m_se_R);
    addParticle(smu_R, false);
    
    snu_e_L = mty::scalarboson_s("snu_e_L ; \\tilde{\\nu_{eL}}", *this);
    snu_e_L->setMass(m_snu_e_L);
    addParticle(snu_e_L, false);
    
    snu_mu_L = mty::scalarboson_s("snu_mu_L ; \\tilde{\\nu_{\\mu L}}", *this);
    snu_mu_L->setMass(m_snu_e_L);
    addParticle(snu_mu_L, false);
    
    snu_tau_L = mty::scalarboson_s("snu_tau_L ; \\tilde{\\nu_{\\tau L}}", *this);
    snu_tau_L->setMass(m_snu_tau_L);
    addParticle(snu_tau_L, false);
    
    st_1 = mty::scalarboson_s("st_1 ; \\tilde{t_1}", *this);
    st_1->setMass(m_st_L);
    st_1->setGroupRep("C", {1, 0});
    addParticle(st_1, false);
    
    st_2 = mty::scalarboson_s("st_2 ; \\tilde{t_2}", *this);
    st_2->setMass(m_st_R);
    st_2->setGroupRep("C", {1, 0});
    addParticle(st_2, false);
    
    stau_1 = mty::scalarboson_s("stau_1 ; \\tilde{tau_1}", *this);
    stau_1->setMass(m_stau_L);
    addParticle(stau_1, false);
    
    stau_2 = mty::scalarboson_s("stau_2 ; \\tilde{tau_2}", *this);
    stau_2->setMass(m_stau_R);
    addParticle(stau_2, false);
    
    sb_1 = mty::scalarboson_s("sb_1 ; \\tilde{b_1}", *this);
    sb_1->setMass(m_sb_L);
    sb_1->setGroupRep("C", {1, 0});
    addParticle(sb_1, false);
    
    sb_2 = mty::scalarboson_s("sb_2 ; \\tilde{b_2}", *this);
    sb_2->setMass(m_sb_R);
    sb_2->setGroupRep("C", {1, 0});
    addParticle(sb_2, false);
    
    Gp = mty::goldstoneboson_s("Gp ; Gp", std::dynamic_pointer_cast<mty::VectorBoson>(W));
    addParticle(Gp);
    
    G0 = mty::goldstoneboson_s("G0 ; G0", std::dynamic_pointer_cast<mty::VectorBoson>(Z));
    addParticle(G0);
    
    sG = mty::weylfermion_s("sG ; \\tilde{G}", *this, Chirality::Left);
    sG->setSelfConjugate(true);
    sG->setMass(M_3);
    sG->setGroupRep("C", {1, 1});
    addParticle(sG, false);
       
    u_L = mty::weylfermion_s("u_L ; u_L", *this, Chirality::Left);
    u_L->setGroupRep("C", {1, 0});
    addParticle(u_L, false);
    
    c_L = mty::weylfermion_s("c_L ; c_L", *this, Chirality::Left);
    c_L->setGroupRep("C", {1, 0});
    addParticle(c_L, false);
    
    t_L = mty::weylfermion_s("t_L ; t_L", *this, Chirality::Left);
    t_L->setMass(m_t);
    t_L->setGroupRep("C", {1, 0});
    addParticle(t_L, false);
    
    u_R = mty::weylfermion_s("u_R ; u_R", *this, Chirality::Right);
    u_R->setGroupRep("C", {1, 0});
    addParticle(u_R, false);
    
    c_R = mty::weylfermion_s("c_R ; c_R", *this, Chirality::Right);
    c_R->setGroupRep("C", {1, 0});
    addParticle(c_R, false);
    
    t_R = mty::weylfermion_s("t_R ; t_R", *this, Chirality::Right);
    t_R->setMass(m_t);
    t_R->setGroupRep("C", {1, 0});
    addParticle(t_R, false);
    
    d_L = mty::weylfermion_s("d_L ; d_L", *this, Chirality::Left);
    d_L->setGroupRep("C", {1, 0});
    addParticle(d_L, false);
    
    s_L = mty::weylfermion_s("s_L ; s_L", *this, Chirality::Left);
    s_L->setMass(m_s);
    s_L->setGroupRep("C", {1, 0});
    addParticle(s_L, false);
    
    b_L = mty::weylfermion_s("b_L ; b_L", *this, Chirality::Left);
    b_L->setMass(m_b);
    b_L->setGroupRep("C", {1, 0});
    addParticle(b_L, false);
    
    d_R = mty::weylfermion_s("d_R ; d_R", *this, Chirality::Right);
    d_R->setGroupRep("C", {1, 0});
    addParticle(d_R, false);
    
    s_R = mty::weylfermion_s("s_R ; s_R", *this, Chirality::Right);
    s_R->setMass(m_s);
    s_R->setGroupRep("C", {1, 0});
    addParticle(s_R, false);
    
    b_R = mty::weylfermion_s("b_R ; b_R", *this, Chirality::Right);
    b_R->setMass(m_b);
    b_R->setGroupRep("C", {1, 0});
    addParticle(b_R, false);
    
    e_L = mty::weylfermion_s("e_L ; e_L", *this, Chirality::Left);
    addParticle(e_L, false);
    
    mu_L = mty::weylfermion_s("mu_L ; \\mu_L", *this, Chirality::Left);
    addParticle(mu_L, false);
    
    tau_L = mty::weylfermion_s("tau_L ; \\tau_L", *this, Chirality::Left);
    tau_L->setMass(m_tau);
    addParticle(tau_L, false);
    
    e_R = mty::weylfermion_s("e_R ; e_R", *this, Chirality::Right);
    addParticle(e_R, false);
    
    mu_R = mty::weylfermion_s("mu_R ; \\mu_R", *this, Chirality::Right);
    addParticle(mu_R, false);
    
    tau_R = mty::weylfermion_s("tau_R ; \\tau_R", *this, Chirality::Right);
    tau_R->setMass(m_tau);
    addParticle(tau_R, false);
    
    nu_e_L = mty::weylfermion_s("nu_e_L ; \\nu_{eL}", *this, Chirality::Left);
    addParticle(nu_e_L, false);
    
    nu_mu_L = mty::weylfermion_s("nu_mu_L ; \\nu_{\\mu L}", *this, Chirality::Left);
    addParticle(nu_mu_L, false);
    
    nu_tau_L = mty::weylfermion_s("nu_tau_L ; \\nu_{\\tau L}", *this, Chirality::Left);
    addParticle(nu_tau_L, false);
    
    N_1 = mty::weylfermion_s("N_1 ; N_1", *this, Chirality::Left);
    N_1->setMass(m_N_1);
    addParticle(N_1, false);
    
    N_2 = mty::weylfermion_s("N_2 ; N_2", *this, Chirality::Left);
    N_2->setMass(m_N_2);
    addParticle(N_2, false);
    
    N_3 = mty::weylfermion_s("N_3 ; N_3", *this, Chirality::Left);
    N_3->setMass(m_N_3);
    addParticle(N_3, false);
    
    N_4 = mty::weylfermion_s("N_4 ; N_4", *this, Chirality::Left);
    N_4->setMass(m_N_4);
    addParticle(N_4, false);
       
    C_1_R = mty::weylfermion_s("C_1_R ; C_{1R}^+", *this, Chirality::Right);
    C_1_R->setMass(m_C1p);
    addParticle(C_1_R, false);
    
    C_2_R = mty::weylfermion_s("C_2_R ; C_{2R}^+", *this, Chirality::Right);
    C_2_R->setMass(m_C2p);
    addParticle(C_2_R, false);
    
    C_1_L = mty::weylfermion_s("C_1_L ; C_{1L}^+", *this, Chirality::Left);
    C_1_L->setMass(m_C1p);
    addParticle(C_1_L, false);
    
    C_2_L = mty::weylfermion_s("C_2_L ; C_{2L}^+", *this, Chirality::Left);
    C_2_L->setMass(m_C2p);
    addParticle(C_2_L, false);
    
    t     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    t_R),
         std::dynamic_pointer_cast<mty::WeylFermion>(t_L));
    t->setMass(m_t);
    t->setGroupRep("C", {1, 0});
    addParticle(t, false);
    
    s     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    s_R),
         std::dynamic_pointer_cast<mty::WeylFermion>(s_L));
    s->setMass(m_s);
    s->setGroupRep("C", {1, 0});
    addParticle(s, false);
    
    b     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    b_R),
         std::dynamic_pointer_cast<mty::WeylFermion>(b_L));
    b->setMass(m_b);
    b->setGroupRep("C", {1, 0});
    addParticle(b, false);
    
    tau     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    tau_R),
         std::dynamic_pointer_cast<mty::WeylFermion>(tau_L));
    tau->setMass(m_tau);
    addParticle(tau, false);
    
    C_1     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    C_1_L),
         std::dynamic_pointer_cast<mty::WeylFermion>(C_1_R));
    C_1->setMass(m_C1p);
    addParticle(C_1, false);
    
    C_2     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    C_2_L),
         std::dynamic_pointer_cast<mty::WeylFermion>(C_2_R));
    C_2->setMass(m_C2p);
    addParticle(C_2, false);
    
    ///////////////////////////////////
    // Spaces                        //
    ///////////////////////////////////
    
    C_1_1 = C->getGroup()->getVectorSpace(C->getGroup()->highestWeightRep({1, 1, }));
    C_1_0 = C->getGroup()->getVectorSpace(C->getGroup()->highestWeightRep({1, 0, }));
    

    ///////////////////////////////////
    // Tensors                       //
    ///////////////////////////////////
    
    d_der = csl::TDerivative("d", &Minko);
    f_C_1_1 = C->getGenerator({1, 1});
    T_C_10_C_1_0 = C->getGenerator({1, 0});
    X = csl::Tensor("X", {&Minko});

    i_dirac = dirac.generateIndices(10);
    i_Minko = Minko.generateIndices(15);
    i_C_1_1 = C_1_1->generateIndices(20);
    i_C_1_0 = C_1_0->generateIndices(30);
    

    ///////////////////////////////////
    // Variable / Constants          //
    ///////////////////////////////////
    
    A_b = mty::mssm_input::Ab;
    mu = mty::mssm_input::mu;
    V_ud = mty::sm_input::V_ud;
    e = mty::sm_input::e;
    A_t = mty::mssm_input::At;
    V_cd = mty::sm_input::V_cd;
    V_td = mty::sm_input::V_td;
    V_Wp1 = csl::constant_s("V_Wp1");
    V_Wp1->setComplexProperty(csl::ComplexProperty::Complex);
    U_stau_11 = csl::constant_s("U_stau_11");
    U_stau_11->setComplexProperty(csl::ComplexProperty::Complex);
    U_sb_00 = csl::constant_s("U_sb_00");
    U_sb_00->setComplexProperty(csl::ComplexProperty::Complex);
    U_d2 = csl::constant_s("U_d2");
    U_d2->setComplexProperty(csl::ComplexProperty::Complex);
    U_st_10 = csl::constant_s("U_st_10");
    U_st_10->setComplexProperty(csl::ComplexProperty::Complex);
    U_sb_10 = csl::constant_s("U_sb_10");
    U_sb_10->setComplexProperty(csl::ComplexProperty::Complex);
    U_sb_11 = csl::constant_s("U_sb_11");
    U_sb_11->setComplexProperty(csl::ComplexProperty::Complex);
    U_sb_01 = csl::constant_s("U_sb_01");
    U_sb_01->setComplexProperty(csl::ComplexProperty::Complex);
    U_st_01 = csl::constant_s("U_st_01");
    U_st_01->setComplexProperty(csl::ComplexProperty::Complex);
    V_u1 = csl::constant_s("V_u1");
    V_u1->setComplexProperty(csl::ComplexProperty::Complex);
    N_u1 = csl::constant_s("N_u1");
    N_u1->setComplexProperty(csl::ComplexProperty::Complex);
    N_B2 = csl::constant_s("N_B2");
    N_B2->setComplexProperty(csl::ComplexProperty::Complex);
    N_d1 = csl::constant_s("N_d1");
    N_d1->setComplexProperty(csl::ComplexProperty::Complex);
    N_d4 = csl::constant_s("N_d4");
    N_d4->setComplexProperty(csl::ComplexProperty::Complex);
    V_Wp2 = csl::constant_s("V_Wp2");
    V_Wp2->setComplexProperty(csl::ComplexProperty::Complex);
    N_d2 = csl::constant_s("N_d2");
    N_d2->setComplexProperty(csl::ComplexProperty::Complex);
    N_W1 = csl::constant_s("N_W1");
    N_W1->setComplexProperty(csl::ComplexProperty::Complex);
    N_W4 = csl::constant_s("N_W4");
    N_W4->setComplexProperty(csl::ComplexProperty::Complex);
    N_B3 = csl::constant_s("N_B3");
    N_B3->setComplexProperty(csl::ComplexProperty::Complex);
    N_u3 = csl::constant_s("N_u3");
    N_u3->setComplexProperty(csl::ComplexProperty::Complex);
    N_B1 = csl::constant_s("N_B1");
    N_B1->setComplexProperty(csl::ComplexProperty::Complex);
    N_W3 = csl::constant_s("N_W3");
    N_W3->setComplexProperty(csl::ComplexProperty::Complex);
    N_u2 = csl::constant_s("N_u2");
    N_u2->setComplexProperty(csl::ComplexProperty::Complex);
    N_d3 = csl::constant_s("N_d3");
    N_d3->setComplexProperty(csl::ComplexProperty::Complex);
    N_B4 = csl::constant_s("N_B4");
    N_B4->setComplexProperty(csl::ComplexProperty::Complex);
    N_W2 = csl::constant_s("N_W2");
    N_W2->setComplexProperty(csl::ComplexProperty::Complex);
    N_u4 = csl::constant_s("N_u4");
    N_u4->setComplexProperty(csl::ComplexProperty::Complex);
    U_st_00 = csl::constant_s("U_st_00");
    U_st_00->setComplexProperty(csl::ComplexProperty::Complex);
    U_Wm1 = csl::constant_s("U_Wm1");
    U_Wm1->setComplexProperty(csl::ComplexProperty::Complex);
    U_d1 = csl::constant_s("U_d1");
    U_d1->setComplexProperty(csl::ComplexProperty::Complex);
    U_Wm2 = csl::constant_s("U_Wm2");
    U_Wm2->setComplexProperty(csl::ComplexProperty::Complex);
    V_u2 = csl::constant_s("V_u2");
    V_u2->setComplexProperty(csl::ComplexProperty::Complex);
    U_st_11 = csl::constant_s("U_st_11");
    U_st_11->setComplexProperty(csl::ComplexProperty::Complex);
    U_stau_00 = csl::constant_s("U_stau_00");
    U_stau_00->setComplexProperty(csl::ComplexProperty::Complex);
    U_stau_10 = csl::constant_s("U_stau_10");
    U_stau_10->setComplexProperty(csl::ComplexProperty::Complex);
    U_stau_01 = csl::constant_s("U_stau_01");
    U_stau_01->setComplexProperty(csl::ComplexProperty::Complex);
}

void PMSSM_LEM::initKinetic()
{
    initKinetic0();
    initKinetic1();
    initKinetic2();
}

void PMSSM_LEM::initMass()
{
    initMass0();
    initMass1();
}

void PMSSM_LEM::initInteractions()
{
    initInteractions0();
    initInteractions1();
    initInteractions2();
    initInteractions3();
    initInteractions4();
    initInteractions5();
    initInteractions6();
    initInteractions7();
    initInteractions8();
    initInteractions9();
    initInteractions10();
    initInteractions11();
    initInteractions12();
    initInteractions13();
    initInteractions14();
    initInteractions15();
    initInteractions16();
    initInteractions17();
    initInteractions18();
    initInteractions19();
    initInteractions20();
    initInteractions21();
    initInteractions22();
    initInteractions23();
    initInteractions24();
    initInteractions25();
    initInteractions26();
    initInteractions27();
    initInteractions28();
    initInteractions29();
    initInteractions30();
    initInteractions31();
    initInteractions32();
    initInteractions33();
    initInteractions34();
    initInteractions35();
    initInteractions36();
    initInteractions37();
    initInteractions38();
    initInteractions39();
    initInteractions40();
    initInteractions41();
    initInteractions42();
    initInteractions43();
    initInteractions44();
    initInteractions45();
    initInteractions46();
    initInteractions47();
    initInteractions48();
    initInteractions49();
    initInteractions50();
    initInteractions51();
    initInteractions52();
    initInteractions53();
    initInteractions54();
    initInteractions55();
    initInteractions56();
    initInteractions57();
    initInteractions58();
    initInteractions59();
    initInteractions60();
    initInteractions61();
    initInteractions62();
    initInteractions63();
    initInteractions64();
    initInteractions65();
    initInteractions66();
    initInteractions67();
    initInteractions68();
    initInteractions69();
    initInteractions70();
    initInteractions71();
    initInteractions72();
    initInteractions73();
    initInteractions74();
    initInteractions75();
    initInteractions76();
    initInteractions77();
    initInteractions78();
    initInteractions79();
    initInteractions80();
    initInteractions81();
    initInteractions82();
    initInteractions83();
    initInteractions84();
    initInteractions85();
    initInteractions86();
    initInteractions87();
    initInteractions88();
    initInteractions89();
    initInteractions90();
    initInteractions91();
    initInteractions92();
    initInteractions93();
    initInteractions94();
    initInteractions95();
    initInteractions96();
    initInteractions97();
    initInteractions98();
    initInteractions99();
    initInteractions100();
    initInteractions101();
    initInteractions102();
    initInteractions103();
    initInteractions104();
    initInteractions105();
    initInteractions106();
    initInteractions107();
    initInteractions108();
    initInteractions109();
    initInteractions110();
    initInteractions111();
    initInteractions112();
    initInteractions113();
    initInteractions114();
    initInteractions115();
    initInteractions116();
    initInteractions117();
    initInteractions118();
    initInteractions119();
    initInteractions120();
    initInteractions121();
    initInteractions122();
    initInteractions123();
    initInteractions124();
    initInteractions125();
    initInteractions126();
    initInteractions127();
    initInteractions128();
    initInteractions129();
    initInteractions130();
    initInteractions131();
    initInteractions132();
    initInteractions133();
    initInteractions134();
    initInteractions135();
    initInteractions136();
    initInteractions137();
}

void PMSSM_LEM::initSpectrum()
{
    csl::Expr M_1 = csl::constant_s("M_1");
    csl::Expr M_q3L = csl::constant_s("M_q3L");
    csl::Expr M_qtR = csl::constant_s("M_qtR");
    csl::Expr M_qbR = csl::constant_s("M_qbR");
    csl::Expr M__tauR = csl::constant_s("M_tauR");

    addSpectrum(
        {N_1, N_2, N_3, N_4, },
        {
        {M_1, 0, csl::prod_s({-2
, M_Z, csl::cos_s(_beta), csl::sin_s(theta_W)}), csl::prod_s({2
, M_Z, csl::sin_s(_beta), csl::sin_s(theta_W)})},
        {0, M_2, csl::prod_s({2
, M_W, csl::cos_s(_beta)}), csl::prod_s({-2
, M_W, csl::sin_s(_beta)})},
        {0, 0, 0, csl::prod_s({-2
, mu})},
        {0, 0, 0, 0},
        },
        {
        {N_B1, N_B2, N_B3, N_B4},
        {N_W1, N_W2, N_W3, N_W4},
        {N_d1, N_d2, N_d3, N_d4},
        {N_u1, N_u2, N_u3, N_u4},
        }
    );
    addSpectrum(
        {C_1, C_2 },
        {
        {M_2, csl::prod_s({csl::pow_s(2, csl::intfraction_s(1, 2)), M_W, csl::sin_s(_beta)})},
        {csl::prod_s({csl::pow_s(2, csl::intfraction_s(1, 2)), M_W, csl::cos_s(_beta)}), mu},
        },
        {
        {V_Wp1, V_Wp2},
        {V_u1, V_u2},
        },
        {
        {U_Wm1, U_Wm2},
        {U_d1, U_d2},
        }
    );
    addSpectrum(
        {st_1, st_2, },
        {
        {csl::sum_s({csl::pow_s(m_t, 2) , csl::pow_s(M_q3L, 2) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_W, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::intfraction_s(-1, 6), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(1, 6), csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::sum_s({csl::prod_s({-2
, mu, m_t, csl::cos_s(_beta), csl::pow_s(csl::sin_s(_beta), (-1))}) , csl::prod_s({2
, csl::pow_s(2, csl::intfraction_s(1, 2)), csl::pow_s(e, (-1)), A_t, M_W, csl::sin_s(_beta), csl::sin_s(theta_W)})})},
        {0, csl::sum_s({csl::pow_s(m_t, 2) , csl::pow_s(M_qtR, 2) , csl::prod_s({csl::intfraction_s(-2, 3), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(2, 3), csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})})},
        },
        {
        {U_st_00, U_st_01},
        {U_st_10, U_st_11},
        }
    );
    addSpectrum(
        {sb_1, sb_2, },
        {
        {csl::sum_s({csl::prod_s({csl::pow_s(V_cb, 2), csl::pow_s(M_q1L, 2)}) , csl::prod_s({V_tb, csl::GetComplexConjugate(V_tb), csl::pow_s(M_q3L, 2)}) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_W, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::intfraction_s(-1, 6), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(1, 6), csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::sum_s({csl::prod_s({-2
, mu, m_b, csl::pow_s(csl::cos_s(_beta), (-1)), csl::sin_s(_beta)}) , csl::prod_s({2
, csl::pow_s(2, csl::intfraction_s(1, 2)), csl::pow_s(e, (-1)), A_b, M_W, V_tb, csl::GetComplexConjugate(V_tb), csl::cos_s(_beta), csl::sin_s(theta_W)})})},
        {0, csl::sum_s({csl::prod_s({V_tb, csl::GetComplexConjugate(V_tb), csl::pow_s(M_qbR, 2)}) , csl::prod_s({csl::pow_s(V_cb, 2), csl::pow_s(M_qdR, 2)}) , csl::prod_s({csl::pow_s(m_b, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::intfraction_s(1, 3), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(-1, 3), csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})})},
        },
        {
        {U_sb_00, U_sb_01},
        {U_sb_10, U_sb_11},
        }
    );
    addSpectrum(
        {stau_1, stau_2, },
        {
        {csl::sum_s({csl::pow_s(M__tauL, 2) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_W, 2), csl::pow_s(csl::sin_s(_beta), 2)}) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_W, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::pow_s(m_tau, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::intfraction_s(1, 2), csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({csl::intfraction_s(-1, 2), csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})}), csl::sum_s({csl::prod_s({-2
, mu, m_tau, csl::pow_s(csl::cos_s(_beta), (-1)), csl::sin_s(_beta)}) , csl::prod_s({2
, csl::pow_s(2, csl::intfraction_s(1, 2)), csl::pow_s(e, (-1)), A_b, M_W, csl::cos_s(_beta), csl::sin_s(theta_W)})})},
        {0, csl::sum_s({csl::pow_s(M__tauR, 2) , csl::prod_s({csl::pow_s(m_tau, 2), csl::pow_s(csl::cos_s(_beta), (-2)), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})})}) , csl::prod_s({csl::pow_s(M_Z, 2), csl::pow_s(csl::sin_s(_beta), 2), csl::pow_s(csl::sin_s(theta_W), 2)}) , csl::prod_s({-1
, csl::pow_s(M_Z, 2), csl::sum_s({1 , csl::prod_s({-1
, csl::pow_s(csl::sin_s(_beta), 2)})}), csl::pow_s(csl::sin_s(theta_W), 2)})})},
        },
        {
        {U_stau_00, U_stau_01},
        {U_stau_10, U_stau_11},
        }
    );
}

} // End of namespace mty
