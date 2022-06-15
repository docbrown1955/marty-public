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

#include "../../core/marty.h"
#include "pmssm_lem.h"

namespace mty {

void PMSSM_LEM::initMass0()
{
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   csl::pow_s(m_A0, 2),
                                   csl::pow_s(A0(X), 2)}),
                      false);
    addLagrangianTerm(
        csl::prod_s(
            {-1, csl::pow_s(m_Hp, 2), Hp(X), csl::GetComplexConjugate(Hp(X))}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   csl::pow_s(m_h0, 2),
                                   csl::pow_s(h0(X), 2)}),
                      false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   csl::pow_s(m_H0, 2),
                                   csl::pow_s(H0(X), 2)}),
                      false);
    addLagrangianTerm(
        csl::prod_s({csl::pow_s(M_W, 2),
                     W({+i_Minko[0]}, X),
                     csl::GetComplexConjugate(W({i_Minko[0]}, X))}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(1, 2),
                                   csl::pow_s(m_Z, 2),
                                   Z({i_Minko[0]}, X),
                                   Z({+i_Minko[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_c,
                                   csl::GetComplexConjugate(
                                       c({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   c({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_t,
                                   csl::GetComplexConjugate(
                                       t({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   t({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_u,
                                   csl::GetComplexConjugate(
                                       u({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   u({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_b,
                                   csl::GetComplexConjugate(
                                       b({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   b({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_d,
                                   csl::GetComplexConjugate(
                                       d({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   d({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   m_s,
                                   csl::GetComplexConjugate(
                                       s({+i_C_1_0[0], +i_dirac[0]}, X)),
                                   s({+i_C_1_0[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     m_e,
                     csl::GetComplexConjugate(e({+i_dirac[0]}, X)),
                     e({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     m_mu,
                     csl::GetComplexConjugate(mu({+i_dirac[0]}, X)),
                     mu({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     m_tau,
                     csl::GetComplexConjugate(tau({+i_dirac[0]}, X)),
                     tau({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sc_L, 2),
                     sc_L({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sc_L({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_su_L, 2),
                     su_L({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(su_L({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sc_R, 2),
                     sc_R({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sc_R({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_su_R, 2),
                     su_R({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(su_R({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sd_L, 2),
                     sd_L({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sd_L({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_ss_L, 2),
                     ss_L({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(ss_L({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sd_R, 2),
                     sd_R({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sd_R({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_ss_R, 2),
                     ss_R({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(ss_R({+i_C_1_0[0]}, X))}),
        false);
}

void PMSSM_LEM::initMass1()
{
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_se_L, 2),
                                   se_L(X),
                                   csl::GetComplexConjugate(se_L(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_smu_L, 2),
                                   smu_L(X),
                                   csl::GetComplexConjugate(smu_L(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_se_R, 2),
                                   se_R(X),
                                   csl::GetComplexConjugate(se_R(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_smu_R, 2),
                                   smu_R(X),
                                   csl::GetComplexConjugate(smu_R(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_snu_e, 2),
                                   snu_e(X),
                                   csl::GetComplexConjugate(snu_e(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_snu_mu, 2),
                                   snu_mu(X),
                                   csl::GetComplexConjugate(snu_mu(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_snu_tau, 2),
                                   snu_tau(X),
                                   csl::GetComplexConjugate(snu_tau(X))}),
                      false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     m_C1p,
                     csl::GetComplexConjugate(C_1({+i_dirac[0]}, X)),
                     C_1({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     m_C2p,
                     csl::GetComplexConjugate(C_2({+i_dirac[0]}, X)),
                     C_2({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   M_3,
                                   csl::GetComplexConjugate(
                                       sG({+i_C_1_1[0], +i_dirac[0]}, X)),
                                   sG({+i_C_1_1[0], +i_dirac[0]}, X)}),
                      false);
    addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 2),
                     m_N_1,
                     csl::GetComplexConjugate(N_1({+i_dirac[0]}, X)),
                     N_1({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 2),
                     m_N_2,
                     csl::GetComplexConjugate(N_2({+i_dirac[0]}, X)),
                     N_2({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 2),
                     m_N_3,
                     csl::GetComplexConjugate(N_3({+i_dirac[0]}, X)),
                     N_3({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 2),
                     m_N_4,
                     csl::GetComplexConjugate(N_4({+i_dirac[0]}, X)),
                     N_4({+i_dirac[0]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_st_L, 2),
                     st_1({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(st_1({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_st_R, 2),
                     st_2({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(st_2({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sb_L, 2),
                     sb_1({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sb_1({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(
        csl::prod_s({-1,
                     csl::pow_s(m_sb_R, 2),
                     sb_2({+i_C_1_0[0]}, X),
                     csl::GetComplexConjugate(sb_2({+i_C_1_0[0]}, X))}),
        false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_stau_L, 2),
                                   stau_1(X),
                                   csl::GetComplexConjugate(stau_1(X))}),
                      false);
    addLagrangianTerm(csl::prod_s({-1,
                                   csl::pow_s(m_stau_R, 2),
                                   stau_2(X),
                                   csl::GetComplexConjugate(stau_2(X))}),
                      false);
    addLagrangianTerm(
        csl::prod_s(
            {-1, csl::pow_s(m_Gp, 2), Gp(X), csl::GetComplexConjugate(Gp(X))}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   csl::pow_s(M_W, 2),
                                   csl::pow_s(G0(X), 2)}),
                      false);
}

} // End of namespace mty
