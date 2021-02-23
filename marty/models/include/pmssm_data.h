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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#ifndef DEFINITION_PMSSM_DATA_H_INCLUDED
#define DEFINITION_PMSSM_DATA_H_INCLUDED

#include "marty.h"

namespace mty {

class PMSSM_data {

protected:

    PMSSM_data(csl::Space const &t_dirac,
               csl::Space const &t_minko)
        :dirac(t_dirac),
        Minko(t_minko)
    {

    }

    csl::Tensor gamma;
    csl::Tensor gamma5;
    csl::Tensor P_L;
    csl::Tensor P_R;
    csl::Tensor sigma;

    csl::Space const &dirac;
    csl::Space const &Minko;

    mty::GaugedGroup *C;

    mty::Particle G;
    mty::Particle F_G;
    mty::Particle W;
    mty::Particle F_W;
    mty::Particle A;
    mty::Particle F_A;
    mty::Particle Z;
    mty::Particle F_Z;
    mty::Particle c_A_C;
    mty::Particle c_W_1;
    mty::Particle c_W_2;
    mty::Particle c_W_3;
    mty::Particle A0;
    mty::Particle Hp;
    mty::Particle h0;
    mty::Particle H0;
    mty::Particle su_L;
    mty::Particle sc_L;
    mty::Particle su_R;
    mty::Particle sc_R;
    mty::Particle sd_L;
    mty::Particle ss_L;
    mty::Particle sd_R;
    mty::Particle ss_R;
    mty::Particle se_L;
    mty::Particle smu_L;
    mty::Particle se_R;
    mty::Particle smu_R;
    mty::Particle snu_e_L;
    mty::Particle snu_mu_L;
    mty::Particle snu_tau_L;
    mty::Particle st_1;
    mty::Particle st_2;
    mty::Particle stau_1;
    mty::Particle stau_2;
    mty::Particle sb_1;
    mty::Particle sb_2;
    mty::Particle Gp;
    mty::Particle G0;
    mty::Particle sB_L;
    mty::Particle sB_R;
    mty::Particle sW_L;
    mty::Particle sW_R;
    mty::Particle sG_L;
    mty::Particle sG_R;
    mty::Particle sHu_L;
    mty::Particle sHu_R;
    mty::Particle sHd_L;
    mty::Particle sHd_R;
    mty::Particle sH_u_p_L;
    mty::Particle sH_u_p_R;
    mty::Particle sH_u_0_L;
    mty::Particle sH_u_0_R;
    mty::Particle sH_d_0_L;
    mty::Particle sH_d_0_R;
    mty::Particle sH_d_m_L;
    mty::Particle sH_d_m_R;
    mty::Particle sW_1_L;
    mty::Particle sW_1_R;
    mty::Particle sW_2_L;
    mty::Particle sW_2_R;
    mty::Particle sW_3_L;
    mty::Particle sW_3_R;

    mty::Particle sW_p_L;
    mty::Particle sW_p_R;
    mty::Particle sW_m_L;
    mty::Particle sW_m_R;
    mty::Particle u_L;
    mty::Particle c_L;
    mty::Particle t_L;
    mty::Particle u_R;
    mty::Particle c_R;
    mty::Particle t_R;
    mty::Particle d_L;
    mty::Particle s_L;
    mty::Particle b_L;
    mty::Particle d_R;
    mty::Particle s_R;
    mty::Particle b_R;
    mty::Particle e_L;
    mty::Particle mu_L;
    mty::Particle tau_L;
    mty::Particle e_R;
    mty::Particle mu_R;
    mty::Particle tau_R;
    mty::Particle nu_e_L;
    mty::Particle nu_mu_L;
    mty::Particle nu_tau_L;
    mty::Particle N_1_L;
    mty::Particle N_1_R;
    mty::Particle N_2_L;
    mty::Particle N_2_R;
    mty::Particle N_3_L;
    mty::Particle N_3_R;
    mty::Particle N_4_L;
    mty::Particle N_4_R;
    mty::Particle C_1_p_L;
    mty::Particle C_1_p_R;
    mty::Particle C_2_p_L;
    mty::Particle C_2_p_R;
    mty::Particle C_1_m_L;
    mty::Particle C_1_m_R;
    mty::Particle C_2_m_L;
    mty::Particle C_2_m_R;
    mty::Particle C_1_R;
    mty::Particle C_2_R;
    mty::Particle C_1_L;
    mty::Particle C_2_L;
    mty::Particle sG;
    mty::Particle N_1;
    mty::Particle N_2;
    mty::Particle N_3;
    mty::Particle N_4;
    mty::Particle t;
    mty::Particle s;
    mty::Particle b;
    mty::Particle tau;
    mty::Particle C_1;
    mty::Particle C_2;
    csl::Expr g_s;
    csl::Expr m_N_4;
    csl::Expr m_C2p;
    csl::Expr M_3;
    csl::Expr M_2;
    csl::Expr M_W;
    csl::Expr _beta;
    csl::Expr M_A;
    csl::Expr M_Z;
    csl::Expr m_A0;
    csl::Expr m_Hp;
    csl::Expr m_Gp;
    csl::Expr alpha;
    csl::Expr m_h0;
    csl::Expr m_H0;
    csl::Expr m_t;
    csl::Expr m_s;
    csl::Expr m_b;
    csl::Expr m_tau;
    csl::Expr theta_W;
    csl::Expr M_q1L;
    csl::Expr m_su_L;
    csl::Expr M_quR;
    csl::Expr m_su_R;
    csl::Expr V_ud;
    csl::Expr m_sd_L;
    csl::Expr delta_wolf;
    csl::Expr V_cs;
    csl::Expr m_ss_L;
    csl::Expr M_qdR;
    csl::Expr m_sd_R;
    csl::Expr m_ss_R;
    csl::Expr M_eL;
    csl::Expr m_se_L;
    csl::Expr M_eR;
    csl::Expr m_se_R;
    csl::Expr m_snu_e_L;
    csl::Expr M__tauL;
    csl::Expr m_snu_tau_L;
    csl::Expr m_N_1;
    csl::Expr m_N_2;
    csl::Expr m_N_3;
    csl::Expr m_C1p;
    csl::Expr m_st_L;
    csl::Expr m_st_R;
    csl::Expr m_stau_L;
    csl::Expr m_stau_R;
    csl::Expr m_sb_L;
    csl::Expr m_sb_R;
    csl::Expr e;
    csl::Expr mu;
    csl::Expr V_tb;
    csl::Expr V_us;
    csl::Expr A_b;
    csl::Expr A_t;
    csl::Expr U_sb_00;
    csl::Expr N_u4;
    csl::Expr V_ts;
    csl::Expr V_td;
    csl::Expr V_cd;
    csl::Expr V_cb;
    csl::Expr V_ub;
    csl::Expr V_ub_mod;
    csl::Expr U_sb_11;
    csl::Expr N_W2;
    csl::Expr U_stau_00;
    csl::Expr N_B4;
    csl::Expr N_B3;
    csl::Expr N_B1;
    csl::Expr V_Wp1;
    csl::Expr U_stau_10;
    csl::Expr N_d4;
    csl::Expr N_W4;
    csl::Expr N_d3;
    csl::Expr V_u2;
    csl::Expr N_u3;
    csl::Expr N_u2;
    csl::Expr N_u1;
    csl::Expr N_W3;
    csl::Expr V_u1;
    csl::Expr U_stau_01;
    csl::Expr U_st_10;
    csl::Expr U_st_00;
    csl::Expr V_Wp2;
    csl::Expr U_st_11;
    csl::Expr N_d2;
    csl::Expr U_Wm1;
    csl::Expr U_d2;
    csl::Expr U_st_01;
    csl::Expr U_sb_01;
    csl::Expr N_d1;
    csl::Expr U_Wm2;
    csl::Expr U_sb_10;
    csl::Expr U_stau_11;
    csl::Expr N_B2;
    csl::Expr U_d1;
    csl::Expr N_W1;
    csl::Expr m_G_0_encaps;
    csl::Expr m_G_0;

    csl::Expr M_1;
    csl::Expr M_q3L;
    csl::Expr M_qtR;
    csl::Expr M_qbR;
    csl::Expr M__tauR;
};

} // End of namespace mty

#endif
