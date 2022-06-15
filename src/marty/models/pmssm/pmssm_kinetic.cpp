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

void PMSSM_LEM::initKinetic0()
{
    addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 4),
                     F_G({+i_C_1_1[0], +i_Minko[0], +i_Minko[1]}, X),
                     F_G({+i_C_1_1[0], i_Minko[0], i_Minko[1]}, X)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(c_G({+i_C_1_1[0]}, X)),
                         0),
                     csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, c_G({+i_C_1_1[0]}, X), 0)}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 2),
                                   F_W({i_Minko[0], i_Minko[1]}, X),
                                   csl::GetComplexConjugate(
                                       F_W({+i_Minko[0], +i_Minko[1]}, X))}),
                      false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(
                 X, d_der, {i_Minko[0]}, csl::GetComplexConjugate(c_Wm(X)), 0),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, c_Wm(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(
                 X, d_der, {i_Minko[0]}, csl::GetComplexConjugate(c_Wp(X)), 0),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, c_Wp(X), 0)}),
        false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 4),
                                   F_A({i_Minko[0], i_Minko[1]}, X),
                                   F_A({+i_Minko[0], +i_Minko[1]}, X)}),
                      false);
    addLagrangianTerm(csl::prod_s({csl::intfraction_s(-1, 4),
                                   F_Z({i_Minko[0], i_Minko[1]}, X),
                                   F_Z({+i_Minko[0], +i_Minko[1]}, X)}),
                      false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::intfraction_s(1, 2),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, A0(X), 0),
             csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, A0(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, Hp(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {+i_Minko[0]},
                                       csl::GetComplexConjugate(Hp(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::intfraction_s(1, 2),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, h0(X), 0),
             csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, h0(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::intfraction_s(1, 2),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, H0(X), 0),
             csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, H0(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     M_W,
                     csl::cos_s(beta),
                     W({i_Minko[0]}, X),
                     csl::tderivativeelement_s(X,
                                               d_der,
                                               {+i_Minko[0]},
                                               csl::GetComplexConjugate(Hp(X)),
                                               0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {-1,
             CSL_I,
             M_W,
             csl::cos_s(beta),
             csl::GetComplexConjugate(W({+i_Minko[0]}, X)),
             csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, Hp(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {-1,
             M_W,
             csl::cos_s(beta),
             csl::pow_s(csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                        csl::intfraction_s(1, 2)),
             Z({i_Minko[0]}, X),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, A0(X), 0)}),
        false);
    addLagrangianTerm(
        csl::sum_s(
            {csl::prod_s(
                 {csl::pow_s(csl::cos_s(theta_W), (-2)),
                  csl::pow_s(csl::sin_s(theta_W), 2),
                  csl::pow_s(
                      csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                      (-1)),
                  csl::tderivativeelement_s(X,
                                            d_der,
                                            {i_Minko[0]},
                                            csl::GetComplexConjugate(c_A(X)),
                                            0),
                  csl::tderivativeelement_s(
                      X, d_der, {+i_Minko[0]}, c_A(X), 0)}),
             csl::prod_s(
                 {csl::pow_s(
                      csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                      (-1)),
                  csl::tderivativeelement_s(X,
                                            d_der,
                                            {i_Minko[1]},
                                            csl::GetComplexConjugate(c_A(X)),
                                            0),
                  csl::tderivativeelement_s(
                      X, d_der, {+i_Minko[1]}, c_A(X), 0)})}),
        false);
    addLagrangianTerm(
        csl::sum_s(
            {csl::prod_s(
                 {csl::pow_s(
                      csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                      (-1)),
                  csl::tderivativeelement_s(X,
                                            d_der,
                                            {i_Minko[0]},
                                            csl::GetComplexConjugate(c_Z(X)),
                                            0),
                  csl::tderivativeelement_s(
                      X, d_der, {+i_Minko[0]}, c_Z(X), 0)}),
             csl::prod_s(
                 {csl::pow_s(csl::cos_s(theta_W), (-2)),
                  csl::pow_s(csl::sin_s(theta_W), 2),
                  csl::pow_s(
                      csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                      (-1)),
                  csl::tderivativeelement_s(X,
                                            d_der,
                                            {i_Minko[1]},
                                            csl::GetComplexConjugate(c_Z(X)),
                                            0),
                  csl::tderivativeelement_s(
                      X, d_der, {+i_Minko[1]}, c_Z(X), 0)})}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(c({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       c({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(t({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       t({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(u({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       u({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(b({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       b({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(d({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       d({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(s({+i_C_1_0[0], +i_dirac[0]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       s({+i_C_1_0[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(e({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, e({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(mu({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, mu({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(tau({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, tau({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(nu_e({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, nu_e({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(nu_mu({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, nu_mu({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
             csl::GetComplexConjugate(nu_tau({+i_dirac[0]}, X)),
             csl::tderivativeelement_s(
                 X, d_der, {i_Minko[0]}, nu_tau({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sc_L({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sc_L({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, su_L({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(su_L({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sc_R({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sc_R({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, su_R({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(su_R({+i_C_1_0[0]}, X)),
                         0)}),
        false);
}

void PMSSM_LEM::initKinetic1()
{
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sd_L({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sd_L({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, ss_L({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(ss_L({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sd_R({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sd_R({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, ss_R({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(ss_R({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, se_L(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(se_L(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, smu_L(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(smu_L(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, se_R(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(se_R(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, smu_R(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(smu_R(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, snu_e(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(snu_e(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, snu_mu(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(snu_mu(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, snu_tau(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(snu_tau(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     C({+i_dirac[0], +i_dirac[1]}),
                     C({+i_dirac[2], +i_dirac[3]}),
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[3]}),
                     C_1_R({+i_dirac[1]}, X),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(C_1_R({+i_dirac[2]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     C({+i_dirac[0], +i_dirac[1]}),
                     C({+i_dirac[2], +i_dirac[3]}),
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[3]}),
                     C_2_R({+i_dirac[1]}, X),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(C_2_R({+i_dirac[2]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(C_1_L({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, C_1_L({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     gamma({+i_Minko[0], +i_dirac[0], +i_dirac[1]}),
                     csl::GetComplexConjugate(C_2_L({+i_dirac[0]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, C_2_L({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {CSL_I,
             P_L({+i_dirac[0], +i_dirac[1]}),
             gamma({+i_Minko[0], +i_dirac[2], +i_dirac[0]}),
             csl::GetComplexConjugate(sG({+i_C_1_1[0], +i_dirac[2]}, X)),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       sG({+i_C_1_1[0], +i_dirac[1]}, X),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     P_L({+i_dirac[0], +i_dirac[1]}),
                     gamma({+i_Minko[0], +i_dirac[2], +i_dirac[0]}),
                     csl::GetComplexConjugate(N_1({+i_dirac[2]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, N_1({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     P_L({+i_dirac[0], +i_dirac[1]}),
                     gamma({+i_Minko[0], +i_dirac[2], +i_dirac[0]}),
                     csl::GetComplexConjugate(N_2({+i_dirac[2]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, N_2({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     P_L({+i_dirac[0], +i_dirac[1]}),
                     gamma({+i_Minko[0], +i_dirac[2], +i_dirac[0]}),
                     csl::GetComplexConjugate(N_3({+i_dirac[2]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, N_3({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({CSL_I,
                     P_L({+i_dirac[0], +i_dirac[1]}),
                     gamma({+i_Minko[0], +i_dirac[2], +i_dirac[0]}),
                     csl::GetComplexConjugate(N_4({+i_dirac[2]}, X)),
                     csl::tderivativeelement_s(
                         X, d_der, {i_Minko[0]}, N_4({+i_dirac[1]}, X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, st_1({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(st_1({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, st_2({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(st_2({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sb_1({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sb_1({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(
                         X, d_der, {+i_Minko[0]}, sb_2({+i_C_1_0[0]}, X), 0),
                     csl::tderivativeelement_s(
                         X,
                         d_der,
                         {i_Minko[0]},
                         csl::GetComplexConjugate(sb_2({+i_C_1_0[0]}, X)),
                         0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, stau_1(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(stau_1(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, stau_2(X), 0),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {i_Minko[0]},
                                       csl::GetComplexConjugate(stau_2(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, Gp(X), 0),
             csl::tderivativeelement_s(
                 X, d_der, {i_Minko[0]}, csl::GetComplexConjugate(Gp(X)), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {M_W,
             csl::sum_s({1, csl::prod_s({CSL_I, csl::sin_s(beta)})}),
             csl::GetComplexConjugate(W({i_Minko[0]}, X)),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, Gp(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {M_W,
             csl::sum_s({1, csl::prod_s({-1, CSL_I, csl::sin_s(beta)})}),
             W({i_Minko[0]}, X),
             csl::tderivativeelement_s(X,
                                       d_der,
                                       {+i_Minko[0]},
                                       csl::GetComplexConjugate(Gp(X)),
                                       0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::intfraction_s(1, 2),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, G0(X), 0),
             csl::tderivativeelement_s(X, d_der, {i_Minko[0]}, G0(X), 0)}),
        false);
    addLagrangianTerm(
        csl::prod_s(
            {csl::sum_s(
                 {m_Z,
                  csl::prod_s(
                      {M_W,
                       csl::sin_s(beta),
                       csl::pow_s(
                           csl::sum_s({1, csl::pow_s(csl::tan_s(theta_W), 2)}),
                           csl::intfraction_s(1, 2))})}),
             Z({i_Minko[0]}, X),
             csl::tderivativeelement_s(X, d_der, {+i_Minko[0]}, G0(X), 0)}),
        false);
}

} // End of namespace mty
