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
 * \version 1.0
 * \brief File containing the Standard Model itself as a mty::Model object.
 * \details It contains also all input parameters for the standard model, 
 * including all values of SMINPUTS block of slha conventions and other 
 * paralemers (CKM, masses, couplings etc). Values come from the august 2018 
 * edition of the PDG.
 */
#ifndef SM_H_INCLUDED
#define SM_H_INCLUDED

#include <csl.h>
#include "model.h"

namespace mty {

    /**
     * @brief Standard Model of particle physics.
     */
    class SM_Model: public Model {

        public:

        SM_Model();
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
    inline 
    csl::Expr alpha_em = csl::constant_s(
            "alpha_em",
            1 / csl::numericaleval_s(1.37035999139e+02L,
                                    0.00000000031e+02L));

    inline 
    csl::Expr e = csl::constant_s(
            "e",
            csl::Evaluated(
                csl::sqrt_s(4 * M_PI * alpha_em), 
                csl::eval::numerical | csl::eval::literal
                )
            );

    // At M_W
    inline
    csl::Expr G_F = csl::constant_s(
            "G_F",
            csl::numericaleval_s(1.1663787e-05L,
                                0.0000006e-05L));

    // At 0
    inline 
    csl::Expr alpha_s = csl::constant_s(
            "alpha_s",
            csl::numericaleval_s(1.1890e-01L,
                                0.0016e-01L));
    inline 
    csl::Expr g_s = csl::constant_s(
            "g_s",
            csl::Evaluated(
                csl::sqrt_s(4 * M_PI * alpha_s), 
                csl::eval::numerical | csl::eval::literal
                )
            );

    inline
    csl::Expr M_Z = csl::constant_s(
            "M_Z",
            csl::numericaleval_s(9.11876e+01,
                                0.00021e+01)); 

    inline
    csl::Expr m_b = csl::constant_s(
            "m_b",
            csl::numericaleval_s(4.18,
                                0.04,
                                0.03));

    inline 
    csl::Expr m_t = csl::constant_s(
            "m_t",
            csl::numericaleval_s(173,
                                0.4));

    inline
    csl::Expr m_tau = csl::constant_s(
            "m_tau",
            csl::numericaleval_s(1.77686,
                                0.00012));

    ///////////////////////////////////////////////////
    // Other inputs
    ///////////////////////////////////////////////////

    inline 
    csl::Expr M_W = csl::constant_s(
            "M_W",
            csl::numericaleval_s(80.379,
                                0.012)
            );

    inline 
    csl::Expr m_h = csl::constant_s(
            "m_h",
            csl::numericaleval_s(125.18,
                                0.16)
            );

    inline 
    csl::Expr v = csl::constant_s(
            "v",
            246.22
            );

    inline 
    csl::Expr m_e = csl::constant_s(
            "m_e",
            csl::numericaleval_s(5.109989461e-04L,
                                0.000000031e-04L)
            );

    inline 
    csl::Expr m_mu = csl::constant_s(
            "m_mu",
            csl::numericaleval_s(105.6583745e-03,
                                000.0000024e-04L)
            );

    inline
    csl::Expr m_u = csl::constant_s(
            "m_u", 
            csl::numericaleval_s(2.2e-03,
                                0.5e-03,
                                0.4e-03));

    inline
    csl::Expr m_d = csl::constant_s(
            "m_d", 
            csl::numericaleval_s(4.7e-03,
                                0.5e-03,
                                0.3e-03));

    inline
    csl::Expr m_c = csl::constant_s(
            "m_c", 
            csl::numericaleval_s(1.275,
                                0.025,
                                0.035));

    inline
    csl::Expr m_s = csl::constant_s(
            "m_s",
            csl::numericaleval_s(0.095,
                                0.009,
                                0.005));
    inline
    csl::Expr Gamma_W = csl::constant_s(
            "Gamma_W",
            csl::numericaleval_s(2.085,
                                0.042)
            );

    inline
    csl::Expr Gamma_Z = csl::constant_s(
            "Gamma_Z",
            csl::numericaleval_s(2.4952,
                                0.0023)
            );

    // From ATLAS, found in PDG chapter on higgs physics
    inline
    csl::Expr Gamma_h = csl::constant_s(
            "Gamma_h",
            csl::numericaleval_s(4.2e-3,
                                1.5e-3,
                                2.1e-3)
            );

    inline
    csl::Expr Gamma_t = csl::constant_s(
            "Gamma_t",
            csl::numericaleval_s(1.41,
                                0.19,
                                0.15)
            );

    // MSbar scheme at mu = 0GeV
    inline
    csl::Expr s2_theta_W = csl::constant_s(
            "s2_thetaW",
            csl::numericaleval_s(0.23057,
                                0.00005)
            );

    inline 
    csl::Expr theta_W = csl::constant_s(
            "theta_W",
            csl::Evaluated(
                csl::asin_s(csl::sqrt_s(s2_theta_W)), 
                csl::eval::numerical | csl::eval::literal
                )
            );

    ///////////////////////////////////////////////////
    //  CKM Matrix
    ///////////////////////////////////////////////////

    inline
    csl::Expr V_ud_mod = csl::constant_s(
            "V_ud_mod",
            csl::numericaleval_s(0.97420,
                                0.00021)
            );

    inline
    csl::Expr V_us_mod = csl::constant_s(
            "V_us_mod",
            csl::numericaleval_s(0.2243,
                                0.0005)
            );

    inline
    csl::Expr V_ub_mod = csl::constant_s(
            "V_ub_mod",
            csl::numericaleval_s(3.94e-03,
                                0.36e-03)
            );

    inline
    csl::Expr V_cb_mod = csl::constant_s(
            "V_cb_mod",
            csl::numericaleval_s(42.2e-03,
                                0.80e-03)
            );

    inline
    csl::Expr V_cd_mod = csl::constant_s(
            "V_cd_mod",
            csl::numericaleval_s(0.2180,
                                0.0004)
            );

    inline
    csl::Expr V_cs_mod = csl::constant_s(
            "V_cs_mod",
            csl::numericaleval_s(0.997,
                                0.017)
            );

    inline
    csl::Expr V_td_mod = csl::constant_s(
            "V_td_mod",
            csl::numericaleval_s(8.1e-03,
                                0.5e-03)
            );

    inline
    csl::Expr V_ts_mod = csl::constant_s(
            "V_ts_mod",
            csl::numericaleval_s(39.4e-03,
                                2.30e-03)
            );

    inline
    csl::Expr V_tb_mod = csl::constant_s(
            "V_tb_mod",
            csl::numericaleval_s(1.019,
                                0.025)
            );

    inline
    csl::Expr A_wolf = csl::constant_s(
            "A_wolf",
            csl::numericaleval_s(0.836,
                                0.015)
            );

    inline
    csl::Expr lambda_wolf = csl::constant_s(
            "lambda_wolf",
            csl::numericaleval_s(0.22453,
                                0.00044)
            );

    inline
    csl::Expr rho_bar_wolf = csl::constant_s(
            "rho_bar_wolf",
            csl::numericaleval_s(0.122,
                                0.018,
                                0.017)
            );

    inline
    csl::Expr eta_bar_wolf = csl::constant_s(
            "eta_bar_wolf",
            csl::numericaleval_s(0.355,
                                0.012,
                                0.011)
            );

    inline 
    csl::Expr delta_wolf = csl::constant_s(
            "delta_wolf",
            csl::Angle(csl::Evaluated(eta_bar_wolf, csl::eval::literal),
                       csl::Evaluated(rho_bar_wolf, csl::eval::literal))
                .evaluate(csl::eval::numerical).value()
            );

    inline
        csl::Expr V_ud = csl::constant_s(
                "V_ud",
                csl::numericaleval_s(0.97420,
                                    0.00021),
                csl::ComplexProperty::Real
                );

    inline
        csl::Expr V_us = csl::constant_s(
                "V_us",
                csl::numericaleval_s(0.2243,
                                    0.0005),
                csl::ComplexProperty::Real
                );

    inline
        csl::Expr V_cb = csl::constant_s(
                "V_cb",
                csl::numericaleval_s(42.2e-03,
                                     0.80e-03),
                csl::ComplexProperty::Real
                );

    inline
        csl::Expr V_tb = csl::constant_s(
                "V_tb",
                csl::numericaleval_s(1.019,
                                     0.025),
                csl::ComplexProperty::Complex
                );

    // As PDG says: angles in the first quadrant so cos > 0 and sin > 0

    inline 
        csl::Expr tan_CKM_12 = V_us / V_ud;
    inline
        csl::Expr sin_CKM_12 = csl::Evaluated(
                csl::sin_s(csl::atan_s(tan_CKM_12)),
                csl::eval::literal | csl::eval::numerical
                );
    inline
        csl::Expr cos_CKM_12 = csl::Evaluated(
                csl::cos_s(csl::atan_s(tan_CKM_12)),
                csl::eval::literal | csl::eval::numerical
                );

    inline 
        csl::Expr tan_CKM_23 = V_cb / V_tb;
    inline
        csl::Expr sin_CKM_23 = csl::Evaluated(
                csl::sin_s(csl::atan_s(tan_CKM_23)),
                csl::eval::literal | csl::eval::numerical
                );
    inline
        csl::Expr cos_CKM_23 = csl::Evaluated(
                csl::cos_s(csl::atan_s(tan_CKM_23)),
                csl::eval::literal | csl::eval::numerical
                );

    inline
        csl::Expr cos_CKM_13 = csl::Evaluated(
                csl::sqrt_s(V_us*V_us + V_ud*V_ud),
                csl::eval::literal | csl::eval::numerical
                );

    inline
        csl::Expr sin_CKM_13 = csl::Evaluated(
                csl::sqrt_s(V_cb*V_cb + V_tb*V_tb),
                csl::eval::literal | csl::eval::numerical
                );

    inline
    csl::Expr V_ub = V_ub_mod * csl::exp_s(-CSL_I * delta_wolf);

    inline
    csl::Expr V_cd = csl::constant_s(
            "V_cd",
            csl::Evaluated(
                - sin_CKM_12*cos_CKM_23 
                - cos_CKM_12*sin_CKM_23*sin_CKM_13*csl::exp_s(CSL_I*delta_wolf),
                csl::eval::literal | csl::eval::numerical
            ),
            csl::ComplexProperty::Complex
            );

    inline
    csl::Expr V_cs = csl::constant_s(
            "V_cs",
            csl::Evaluated(
                cos_CKM_12*cos_CKM_23 
                - sin_CKM_12*sin_CKM_23*sin_CKM_13*csl::exp_s(CSL_I*delta_wolf),
                csl::eval::literal | csl::eval::numerical
            ),
            csl::ComplexProperty::Complex
            );

    inline
    csl::Expr V_td = csl::constant_s(
            "V_td",
            csl::Evaluated(
                sin_CKM_12*sin_CKM_23 
                - cos_CKM_12*cos_CKM_23*sin_CKM_13*csl::exp_s(CSL_I*delta_wolf),
                csl::eval::literal | csl::eval::numerical
            ),
            csl::ComplexProperty::Complex
            );

    inline
    csl::Expr V_ts = csl::constant_s(
            "V_ts",
            csl::Evaluated(
                - cos_CKM_12*sin_CKM_23 
                - sin_CKM_12*cos_CKM_23*sin_CKM_13*csl::exp_s(CSL_I*delta_wolf),
            csl::eval::literal | csl::eval::numerical
            ),
            csl::ComplexProperty::Complex
            );

    // Wolfenstein parametrization

    inline
    csl::Expr V_ud_wolf = 1 - lambda_wolf*lambda_wolf / 2;

    inline
    csl::Expr V_cd_wolf = - lambda_wolf;

    inline
    csl::Expr V_td_wolf = rho_bar_wolf - CSL_I * eta_bar_wolf;

    inline
    csl::Expr V_us_wolf = lambda_wolf;

    inline
    csl::Expr V_cs_wolf = 1 - lambda_wolf*lambda_wolf / 2;

    inline
    csl::Expr V_ts_wolf = -A_wolf * lambda_wolf * lambda_wolf;

    inline
    csl::Expr V_ub_wolf = rho_bar_wolf + CSL_I * eta_bar_wolf;

    inline
    csl::Expr V_cb_wolf = A_wolf * lambda_wolf*lambda_wolf;

    inline
    csl::Expr V_tb_wolf = CSL_1;

} // End of namespace mty::sm_input

#endif
