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

/*! \file SM.h
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief File containing the Standard Model itself as a mty::Model object.
 * \details It contains also all input parameters for the standard model,
 * including all values of SMINPUTS block of slha conventions and other
 * paralemers (CKM, masses, couplings etc). Values come from the august 2018
 * edition of the PDG.
 */
#ifndef SM_H_INCLUDED
#define SM_H_INCLUDED

#include "../core/model.h"

namespace mty {

/**
 * @brief Standard Model of particle physics.
 */
class SM_Model : public mty::Model {

  public:
    SM_Model(bool init = true);

    void init();
    void initContent();
    void getToLowEnergyLagrangian();

    void initGauge();
    void initFermions();
    void initHiggsPotential();
    void initYukawas();

    void HiggsVEVExpansion();
    void diagonalizeSMMassMatrices();
    void gaugeSymmetryBreaking();
    void replaceLeptonYukawa();
    void replaceUpYukawa();
    void replaceDownYukawa();
    void flavorSymmetryBreaking();
    void adjust();
    void addGaugeFixingTerms();
};

} // End of namespace mty

/**
 * @brief Namespace containing SM input parameters, in particular those
 * modifiable with LHA file.
 *
 * @sa lha.h, lhaBuiltIn.h, mty::lha
 */
namespace mty::sm_input {

///////////////////////////////////////////////////
// SMINPUTS block SLHA conventions
///////////////////////////////////////////////////

// alpha_em(0) = 1 / ( 137.035999139(31) )
inline csl::Expr alpha_em = csl::constant_s(
    "alpha_em ; \\alpha_{\\mathrm{em}}", 1 / csl::float_s(1.37035999139e+02L));

inline csl::Expr e_em = csl::constant_s(
    "e_em ; e_{\\mathrm{em}}",
    csl::Evaluated(csl::sqrt_s(4 * M_PI * alpha_em),
                   csl::eval::numerical | csl::eval::literal));

// At M_W
inline csl::Expr G_F = csl::constant_s("G_F", csl::float_s(1.1663787e-05L));

// At 0
inline csl::Expr alpha_s
    = csl::constant_s("alpha_s ; \\alpha_s", csl::float_s(1.1890e-01L));
inline csl::Expr g_s = csl::constant_s(
    "g_s",
    csl::Evaluated(csl::sqrt_s(4 * M_PI * alpha_s),
                   csl::eval::numerical | csl::eval::literal));

inline csl::Expr M_Z = csl::constant_s("M_Z", csl::float_s(9.11876e+01));

inline csl::Expr m_b = csl::constant_s("m_b", csl::float_s(4.18));

inline csl::Expr m_t = csl::constant_s("m_t", csl::float_s(173));

inline csl::Expr m_tau
    = csl::constant_s("m_tau ; m_\\tau", csl::float_s(1.77686));

///////////////////////////////////////////////////
// Other inputs
///////////////////////////////////////////////////

inline csl::Expr M_W = csl::constant_s("M_W", csl::float_s(80.379));

inline csl::Expr m_h = csl::constant_s("m_h", csl::float_s(125.18));

inline csl::Expr v = csl::constant_s("v", 246.22);

inline csl::Expr m_e = csl::constant_s("m_e", csl::float_s(5.109989461e-04L));

inline csl::Expr m_mu
    = csl::constant_s("m_mu ; m_\\mu", csl::float_s(105.6583745e-03));

inline csl::Expr m_u = csl::constant_s("m_u", csl::float_s(2.2e-03));

inline csl::Expr m_d = csl::constant_s("m_d", csl::float_s(4.7e-03));

inline csl::Expr m_c = csl::constant_s("m_c", csl::float_s(1.275));

inline csl::Expr m_s = csl::constant_s("m_s", csl::float_s(0.095));
inline csl::Expr Gamma_W
    = csl::constant_s("Gamma_W ; \\Gamma_W", csl::float_s(2.085));

inline csl::Expr Gamma_Z
    = csl::constant_s("Gamma_Z ; \\Gamma_Z", csl::float_s(2.4952));

// From ATLAS, found in PDG chapter on higgs physics
inline csl::Expr Gamma_h
    = csl::constant_s("Gamma_h ; \\Gamma_h", csl::float_s(4.2e-3));

inline csl::Expr Gamma_t
    = csl::constant_s("Gamma_t ; \\Gamma_t", csl::float_s(1.41));

// MSbar scheme at mu = 0GeV
inline csl::Expr s2_theta_W
    = csl::constant_s("s2_thetaW ; s^2_{\\theta_W}", csl::float_s(0.23057));

inline csl::Expr theta_W = csl::constant_s(
    "theta_W ; \\theta_W",
    csl::Evaluated(csl::asin_s(csl::sqrt_s(s2_theta_W)),
                   csl::eval::numerical | csl::eval::literal));

///////////////////////////////////////////////////
//  CKM Matrix PDG 2020
///////////////////////////////////////////////////

// Set this option to false to use bare measurements
// The Wolfenstein parametrization yields a unitary CKM, quite handy
inline constexpr bool useWolfensteinCKM = true;

inline csl::Expr V_ud_mod
    = csl::constant_s("V_ud_mod ; |V_{ud}|", csl::float_s(0.97370));

inline csl::Expr V_us_mod
    = csl::constant_s("V_us_mod ; |V_{us}|", csl::float_s(0.2245));

inline csl::Expr V_ub_mod
    = csl::constant_s("V_ub_mod ; |V_{ub}|", csl::float_s(3.82e-03));

inline csl::Expr V_cb_mod
    = csl::constant_s("V_cb_mod ; |V_{cb}|", csl::float_s(41.0e-03));

inline csl::Expr V_cd_mod
    = csl::constant_s("V_cd_mod ; |V_{cd}|", csl::float_s(0.2210));

inline csl::Expr V_cs_mod
    = csl::constant_s("V_cs_mod ; |V_{cs}|", csl::float_s(0.987));

inline csl::Expr V_td_mod
    = csl::constant_s("V_td_mod ; |V_{td}|", csl::float_s(8.0e-03));

inline csl::Expr V_ts_mod
    = csl::constant_s("V_ts_mod ; |V_{ts}|", csl::float_s(38.8e-03));

inline csl::Expr V_tb_mod
    = csl::constant_s("V_tb_mod ; |V_{tb}|", csl::float_s(1.013));

// Wolfenstein parametrization from PDG 2021
inline csl::Expr A_wolf
    = csl::constant_s("A_wolf ; A_{\\mathrm{Wolf}}", csl::float_s(0.790));

inline csl::Expr lambda_wolf = csl::constant_s("lambda_wolf ; "
                                               "\\lambda_{\\mathrm{Wolf}}",
                                               csl::float_s(0.22650));

inline csl::Expr rho_bar_wolf = csl::constant_s("rho_bar_wolf ; "
                                                "\\bar{\\rho}_{\\mathrm{Wolf}"
                                                "}",
                                                csl::float_s(0.141));

inline csl::Expr eta_bar_wolf = csl::constant_s("eta_bar_wolf ; "
                                                "\\bar{\\eta}_{\\mathrm{Wolf}"
                                                "}",
                                                csl::float_s(0.357));

inline csl::Expr s_12_wolf = lambda_wolf;

inline csl::Expr s_23_wolf = A_wolf * lambda_wolf * lambda_wolf;

inline csl::Expr s_13_wolf
    = A_wolf * csl::pow_s(lambda_wolf, 3)
      * csl::sqrt_s(rho_bar_wolf * rho_bar_wolf + eta_bar_wolf * eta_bar_wolf);

inline csl::Expr c_12_wolf = csl::sqrt_s(1 - s_12_wolf * s_12_wolf);

inline csl::Expr c_13_wolf = csl::sqrt_s(1 - s_13_wolf * s_13_wolf);

inline csl::Expr c_23_wolf = csl::sqrt_s(1 - s_23_wolf * s_23_wolf);

inline csl::Expr delta_wolf = csl::constant_s(
    "delta_wolf; \\delta_{\\mathrm{Wolf}}",
    csl::Evaluated(csl::angle_s(rho_bar_wolf, eta_bar_wolf), csl::eval::all));

inline csl::Expr V_ud_wolf = c_12_wolf * c_13_wolf;

inline csl::Expr V_us_wolf = s_12_wolf * c_13_wolf;

inline csl::Expr V_ub_wolf = s_13_wolf * csl::exp_s(-CSL_I * delta_wolf);

inline csl::Expr V_cd_wolf
    = -s_12_wolf * c_23_wolf
      - c_12_wolf * s_23_wolf * s_13_wolf * csl::exp_s(CSL_I * delta_wolf);

inline csl::Expr V_cs_wolf
    = c_12_wolf * c_23_wolf
      - s_12_wolf * s_23_wolf * s_13_wolf * csl::exp_s(CSL_I * delta_wolf);

inline csl::Expr V_cb_wolf = s_23_wolf * c_13_wolf;

inline csl::Expr V_td_wolf
    = s_12_wolf * s_23_wolf
      - c_12_wolf * c_23_wolf * s_13_wolf * csl::exp_s(CSL_I * delta_wolf);

inline csl::Expr V_ts_wolf
    = -c_12_wolf * s_23_wolf
      - s_12_wolf * c_23_wolf * s_13_wolf * csl::exp_s(CSL_I * delta_wolf);

inline csl::Expr V_tb_wolf = c_23_wolf * c_13_wolf;

inline csl::Expr V_ud = csl::constant_s(
    "V_ud ; V_{ud}",
    (useWolfensteinCKM) ? csl::Evaluated(V_ud_wolf, csl::eval::all)
                        : V_ud_mod->getValue(),
    csl::ComplexProperty::Real);

inline csl::Expr V_us = csl::constant_s(
    "V_us ; V_{us}",
    (useWolfensteinCKM) ? csl::Evaluated(V_us_wolf, csl::eval::all)
                        : V_us_mod->getValue(),
    csl::ComplexProperty::Real);

inline csl::Expr V_cb = csl::constant_s(
    "V_cb ; V_{cb}",
    (useWolfensteinCKM) ? csl::Evaluated(V_cb_wolf, csl::eval::all)
                        : V_cb_mod->getValue(),
    csl::ComplexProperty::Real);

inline csl::Expr V_tb = csl::constant_s(
    "V_tb ; V_{tb}",
    (useWolfensteinCKM) ? csl::Evaluated(V_tb_wolf, csl::eval::all)
                        : V_tb_mod->getValue(),
    csl::ComplexProperty::Real);

// As PDG says: angles in the first quadrant so cos > 0 and sin > 0

inline csl::Expr tan_CKM_12 = V_us / V_ud;
inline csl::Expr sin_CKM_12
    = csl::Evaluated(csl::sin_s(csl::atan_s(tan_CKM_12)),
                     csl::eval::literal | csl::eval::numerical);
inline csl::Expr cos_CKM_12
    = csl::Evaluated(csl::cos_s(csl::atan_s(tan_CKM_12)),
                     csl::eval::literal | csl::eval::numerical);

inline csl::Expr tan_CKM_23 = V_cb / V_tb;
inline csl::Expr sin_CKM_23
    = csl::Evaluated(csl::sin_s(csl::atan_s(tan_CKM_23)),
                     csl::eval::literal | csl::eval::numerical);
inline csl::Expr cos_CKM_23
    = csl::Evaluated(csl::cos_s(csl::atan_s(tan_CKM_23)),
                     csl::eval::literal | csl::eval::numerical);

inline csl::Expr cos_CKM_13
    = csl::Evaluated(csl::sqrt_s(V_us * V_us + V_ud * V_ud),
                     csl::eval::literal | csl::eval::numerical);

inline csl::Expr sin_CKM_13
    = csl::Evaluated(csl::sqrt_s(1 - csl::pow_s(cos_CKM_13, 2)),
                     csl::eval::literal | csl::eval::numerical);

inline csl::Expr V_cd = csl::constant_s(
    "V_cd ; V_{cd}",
    (useWolfensteinCKM)
        ? csl::Evaluated(V_cd_wolf, csl::eval::all)
        : csl::Evaluated(-sin_CKM_12 *cos_CKM_23
                             - cos_CKM_12 * sin_CKM_23 * sin_CKM_13
                                   * csl::exp_s(CSL_I * delta_wolf),
                         csl::eval::literal | csl::eval::numerical),
    csl::ComplexProperty::Complex);

inline csl::Expr V_cs = csl::constant_s(
    "V_cs ; V_{cs}",
    (useWolfensteinCKM)
        ? csl::Evaluated(V_cs_wolf, csl::eval::all)
        : csl::Evaluated(cos_CKM_12 *cos_CKM_23
                             - sin_CKM_12 * sin_CKM_23 * sin_CKM_13
                                   * csl::exp_s(CSL_I * delta_wolf),
                         csl::eval::literal | csl::eval::numerical),
    csl::ComplexProperty::Complex);

inline csl::Expr V_td = csl::constant_s(
    "V_td ; V_{td}",
    (useWolfensteinCKM)
        ? csl::Evaluated(V_td_wolf, csl::eval::all)
        : csl::Evaluated(sin_CKM_12 *sin_CKM_23
                             - cos_CKM_12 * cos_CKM_23 * sin_CKM_13
                                   * csl::exp_s(CSL_I * delta_wolf),
                         csl::eval::literal | csl::eval::numerical),
    csl::ComplexProperty::Complex);

inline csl::Expr V_ts = csl::constant_s(
    "V_ts ; V_{ts}",
    (useWolfensteinCKM)
        ? csl::Evaluated(V_ts_wolf, csl::eval::all)
        : csl::Evaluated(-cos_CKM_12 *sin_CKM_23
                             - sin_CKM_12 * cos_CKM_23 * sin_CKM_13
                                   * csl::exp_s(CSL_I * delta_wolf),
                         csl::eval::literal | csl::eval::numerical),
    csl::ComplexProperty::Complex);

inline csl::Expr V_ub = csl::constant_s(
    "V_ub ; V_{ub}",
    (useWolfensteinCKM) ? csl::Evaluated(V_ub_wolf, csl::eval::all)
                        : csl::Evaluated( // Unitary triangle condition
                            -(csl::GetComplexConjugate(V_cd) * V_cb
                              + csl::GetComplexConjugate(V_td) * V_tb)
                                / csl::GetComplexConjugate(V_ud),
                            csl::eval::literal | csl::eval::numerical),
    csl::ComplexProperty::Complex);

inline const std::vector<csl::Expr> all_params = {
    // 14 scalar parameters
    alpha_em,
    alpha_s,
    M_Z,
    M_W,
    m_h,
    m_e,
    m_mu,
    m_tau,
    m_u,
    m_d,
    m_c,
    m_s,
    m_t,
    m_b,

    // CKM (dof: 3 angles + 1 phase)
    V_ud,
    V_us,
    V_ub,
    V_cd,
    V_cs,
    V_cb,
    V_td,
    V_ts,
    V_tb,

    // Secondary parameters
    e_em,
    G_F,
    g_s,
    v,
    theta_W,
    s2_theta_W,
    Gamma_W,
    Gamma_Z,
    Gamma_h,
    Gamma_t,
};

/*
 * @brief Removes all numerical values from the SM parameters (in the
 * `all_inputs` vector).
 *
 * @details This can be used to avoid numerical evaluation before the
 * library generation and therefore allow the parameters to be tuned
 * in the latter libraries.
 */
void undefineNumericalValues();

/*
 * @brief Redefine all numerical values from the SM parameters (in the
 * `all_inputs` vector).
 *
 * @details This can be used after a call to `undefineNumericalValues()`
 * to recover all the numerical values of the SM parameters.
 */
void redefineNumericalValues();

} // End of namespace mty::sm_input

#endif
