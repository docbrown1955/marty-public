#ifndef CSL_LIB_PARAM_H_INCLUDED
#define CSL_LIB_PARAM_H_INCLUDED

#include <map>
#include <array>
#include "common.h"
#include "libcomplexop.h"
#include "csl/initSanitizer.h"

namespace test_spectrum {

struct param_t {

    ///////////////////////////////////////
    // Elementary parameters to be defined 
    ///////////////////////////////////////

    csl::InitSanitizer<complex_t> M_D { "M_D" };
    csl::InitSanitizer<complex_t> M_L { "M_L" };
    csl::InitSanitizer<complex_t> M_U { "M_U" };
    csl::InitSanitizer<complex_t> g_X { "g_X" };
    csl::InitSanitizer<complex_t> v_x { "v_x" };
    csl::InitSanitizer<complex_t> v_y { "v_y" };
    csl::InitSanitizer<complex_t> y_d { "y_d" };
    csl::InitSanitizer<complex_t> y_l { "y_l" };
    csl::InitSanitizer<complex_t> y_u { "y_u" };
    csl::InitSanitizer<complex_t> y_d0 { "y_d0" };
    csl::InitSanitizer<complex_t> y_l0 { "y_l0" };
    csl::InitSanitizer<complex_t> y_u0 { "y_u0" };
    csl::InitSanitizer<complex_t> kappa_d0 { "kappa_d0" };
    csl::InitSanitizer<complex_t> kappa_d1 { "kappa_d1" };
    csl::InitSanitizer<complex_t> kappa_d2 { "kappa_d2" };
    csl::InitSanitizer<complex_t> kappa_l0 { "kappa_l0" };
    csl::InitSanitizer<complex_t> kappa_l1 { "kappa_l1" };
    csl::InitSanitizer<complex_t> kappa_l2 { "kappa_l2" };
    csl::InitSanitizer<complex_t> kappa_u0 { "kappa_u0" };
    csl::InitSanitizer<complex_t> kappa_u1 { "kappa_u1" };
    csl::InitSanitizer<complex_t> lambda_d { "lambda_d" };
    csl::InitSanitizer<complex_t> lambda_l { "lambda_l" };
    csl::InitSanitizer<complex_t> lambda_u { "lambda_u" };
    csl::InitSanitizer<complex_t> tildekappa_u0 { "tildekappa_u0" };
    csl::InitSanitizer<complex_t> tildekappa_u2 { "tildekappa_u2" };
    csl::InitSanitizer<complex_t> tildelambda_d { "tildelambda_d" };
    csl::InitSanitizer<complex_t> tildelambda_l { "tildelambda_l" };
    csl::InitSanitizer<complex_t> tildelambda_u { "tildelambda_u" };


    ///////////////////////////////////////
    // Parameters functions of others  
    // through diagonalization or mass 
    // expressions, see updateSpectrum()  
    // in global.h or set them by hand  
    // 
    // And other default parameters  
    ///////////////////////////////////////

    csl::InitSanitizer<real_t> M_W { "M_W" };
    csl::InitSanitizer<real_t> M_Z { "M_Z" };
    csl::InitSanitizer<real_t> m_H { "m_H" };
    csl::InitSanitizer<real_t> m_G0 { "m_G0" };
    csl::InitSanitizer<real_t> m_Gp { "m_Gp" };
    csl::InitSanitizer<real_t> m_h0 { "m_h0" };
    csl::InitSanitizer<real_t> m_V_1 { "m_V_1" };
    csl::InitSanitizer<real_t> m_V_2 { "m_V_2" };
    csl::InitSanitizer<real_t> m_V_3 { "m_V_3" };
    csl::InitSanitizer<real_t> m_b_L { "m_b_L" };
    csl::InitSanitizer<real_t> m_c_Z { "m_c_Z" };
    csl::InitSanitizer<real_t> m_c_Wm { "m_c_Wm" };
    csl::InitSanitizer<real_t> m_c_Wp { "m_c_Wp" };
    csl::InitSanitizer<real_t> m_d_L_1 { "m_d_L_1" };
    csl::InitSanitizer<real_t> m_d_L_2 { "m_d_L_2" };
    csl::InitSanitizer<real_t> m_L_L_0_2 { "m_L_L_0_2" };
    csl::InitSanitizer<real_t> m_Q_L_0_1 { "m_Q_L_0_1" };
    csl::InitSanitizer<real_t> m_Q_L_0_2 { "m_Q_L_0_2" };
    csl::InitSanitizer<real_t> m_{d_L}_1 { "m_{d_L}_1" };
    csl::InitSanitizer<real_t> m_{d_L}_2 { "m_{d_L}_2" };
    csl::InitSanitizer<real_t> m_Psi_dL_1 { "m_Psi_dL_1" };
    csl::InitSanitizer<real_t> m_Psi_dL_2 { "m_Psi_dL_2" };
    csl::InitSanitizer<real_t> m_Psi_lL_1 { "m_Psi_lL_1" };
    csl::InitSanitizer<real_t> m_Psi_lL_2 { "m_Psi_lL_2" };
    csl::InitSanitizer<real_t> m_Psi_uL_1 { "m_Psi_uL_1" };
    csl::InitSanitizer<real_t> m_Psi_uL_2 { "m_Psi_uL_2" };
    csl::InitSanitizer<real_t> m_L_L_i_2_2 { "m_L_L_i_2_2" };
    csl::InitSanitizer<real_t> m_Q_L_i_2_1 { "m_Q_L_i_2_1" };
    csl::InitSanitizer<real_t> m_Q_L_i_2_2 { "m_Q_L_i_2_2" };
    csl::InitSanitizer<real_t> m_{L_L}_0_2 { "m_{L_L}_0_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_0_1 { "m_{Q_L}_0_1" };
    csl::InitSanitizer<real_t> m_\Psi_{dL}_1 { "m_\Psi_{dL}_1" };
    csl::InitSanitizer<real_t> m_\Psi_{dL}_2 { "m_\Psi_{dL}_2" };
    csl::InitSanitizer<real_t> m_\Psi_{lL}_1 { "m_\Psi_{lL}_1" };
    csl::InitSanitizer<real_t> m_\Psi_{lL}_2 { "m_\Psi_{lL}_2" };
    csl::InitSanitizer<real_t> m_\Psi_{uL}_1 { "m_\Psi_{uL}_1" };
    csl::InitSanitizer<real_t> m_\Psi_{uL}_2 { "m_\Psi_{uL}_2" };
    csl::InitSanitizer<real_t> m_{L_L}_i_1_2 { "m_{L_L}_i_1_2" };
    csl::InitSanitizer<real_t> m_{L_L}_i_2_2 { "m_{L_L}_i_2_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_1_1 { "m_{Q_L}_i_1_1" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_1_2 { "m_{Q_L}_i_1_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_2_1 { "m_{Q_L}_i_2_1" };
    csl::InitSanitizer<complex_t> U_L_00 { "U_L_00" };
    csl::InitSanitizer<complex_t> U_L_01 { "U_L_01" };
    csl::InitSanitizer<complex_t> U_L_02 { "U_L_02" };
    csl::InitSanitizer<complex_t> U_L_03 { "U_L_03" };
    csl::InitSanitizer<complex_t> U_L_04 { "U_L_04" };
    csl::InitSanitizer<complex_t> U_L_10 { "U_L_10" };
    csl::InitSanitizer<complex_t> U_L_11 { "U_L_11" };
    csl::InitSanitizer<complex_t> U_L_12 { "U_L_12" };
    csl::InitSanitizer<complex_t> U_L_13 { "U_L_13" };
    csl::InitSanitizer<complex_t> U_L_14 { "U_L_14" };
    csl::InitSanitizer<complex_t> U_L_20 { "U_L_20" };
    csl::InitSanitizer<complex_t> U_L_21 { "U_L_21" };
    csl::InitSanitizer<complex_t> U_L_22 { "U_L_22" };
    csl::InitSanitizer<complex_t> U_L_23 { "U_L_23" };
    csl::InitSanitizer<complex_t> U_L_24 { "U_L_24" };
    csl::InitSanitizer<complex_t> U_L_30 { "U_L_30" };
    csl::InitSanitizer<complex_t> U_L_31 { "U_L_31" };
    csl::InitSanitizer<complex_t> U_L_32 { "U_L_32" };
    csl::InitSanitizer<complex_t> U_L_33 { "U_L_33" };
    csl::InitSanitizer<complex_t> U_L_34 { "U_L_34" };
    csl::InitSanitizer<complex_t> U_L_40 { "U_L_40" };
    csl::InitSanitizer<complex_t> U_L_41 { "U_L_41" };
    csl::InitSanitizer<complex_t> U_L_42 { "U_L_42" };
    csl::InitSanitizer<complex_t> U_L_43 { "U_L_43" };
    csl::InitSanitizer<complex_t> U_L_44 { "U_L_44" };
    csl::InitSanitizer<complex_t> U_Q_00 { "U_Q_00" };
    csl::InitSanitizer<complex_t> U_Q_01 { "U_Q_01" };
    csl::InitSanitizer<complex_t> U_Q_02 { "U_Q_02" };
    csl::InitSanitizer<complex_t> U_Q_03 { "U_Q_03" };
    csl::InitSanitizer<complex_t> U_Q_04 { "U_Q_04" };
    csl::InitSanitizer<complex_t> U_Q_10 { "U_Q_10" };
    csl::InitSanitizer<complex_t> U_Q_11 { "U_Q_11" };
    csl::InitSanitizer<complex_t> U_Q_12 { "U_Q_12" };
    csl::InitSanitizer<complex_t> U_Q_13 { "U_Q_13" };
    csl::InitSanitizer<complex_t> U_Q_14 { "U_Q_14" };
    csl::InitSanitizer<complex_t> U_Q_20 { "U_Q_20" };
    csl::InitSanitizer<complex_t> U_Q_21 { "U_Q_21" };
    csl::InitSanitizer<complex_t> U_Q_22 { "U_Q_22" };
    csl::InitSanitizer<complex_t> U_Q_23 { "U_Q_23" };
    csl::InitSanitizer<complex_t> U_Q_24 { "U_Q_24" };
    csl::InitSanitizer<complex_t> U_Q_30 { "U_Q_30" };
    csl::InitSanitizer<complex_t> U_Q_31 { "U_Q_31" };
    csl::InitSanitizer<complex_t> U_Q_32 { "U_Q_32" };
    csl::InitSanitizer<complex_t> U_Q_33 { "U_Q_33" };
    csl::InitSanitizer<complex_t> U_Q_34 { "U_Q_34" };
    csl::InitSanitizer<complex_t> U_Q_40 { "U_Q_40" };
    csl::InitSanitizer<complex_t> U_Q_41 { "U_Q_41" };
    csl::InitSanitizer<complex_t> U_Q_42 { "U_Q_42" };
    csl::InitSanitizer<complex_t> U_Q_43 { "U_Q_43" };
    csl::InitSanitizer<complex_t> U_Q_44 { "U_Q_44" };
    csl::InitSanitizer<complex_t> U_V_00 { "U_V_00" };
    csl::InitSanitizer<complex_t> U_V_01 { "U_V_01" };
    csl::InitSanitizer<complex_t> U_V_02 { "U_V_02" };
    csl::InitSanitizer<complex_t> U_V_10 { "U_V_10" };
    csl::InitSanitizer<complex_t> U_V_11 { "U_V_11" };
    csl::InitSanitizer<complex_t> U_V_12 { "U_V_12" };
    csl::InitSanitizer<complex_t> U_V_20 { "U_V_20" };
    csl::InitSanitizer<complex_t> U_V_21 { "U_V_21" };
    csl::InitSanitizer<complex_t> U_V_22 { "U_V_22" };
    csl::InitSanitizer<complex_t> U_d_00 { "U_d_00" };
    csl::InitSanitizer<complex_t> U_d_01 { "U_d_01" };
    csl::InitSanitizer<complex_t> U_d_02 { "U_d_02" };
    csl::InitSanitizer<complex_t> U_d_10 { "U_d_10" };
    csl::InitSanitizer<complex_t> U_d_11 { "U_d_11" };
    csl::InitSanitizer<complex_t> U_d_12 { "U_d_12" };
    csl::InitSanitizer<complex_t> U_d_20 { "U_d_20" };
    csl::InitSanitizer<complex_t> U_d_21 { "U_d_21" };
    csl::InitSanitizer<complex_t> U_d_22 { "U_d_22" };
    csl::InitSanitizer<complex_t> V_L_00 { "V_L_00" };
    csl::InitSanitizer<complex_t> V_L_01 { "V_L_01" };
    csl::InitSanitizer<complex_t> V_L_02 { "V_L_02" };
    csl::InitSanitizer<complex_t> V_L_03 { "V_L_03" };
    csl::InitSanitizer<complex_t> V_L_04 { "V_L_04" };
    csl::InitSanitizer<complex_t> V_L_10 { "V_L_10" };
    csl::InitSanitizer<complex_t> V_L_11 { "V_L_11" };
    csl::InitSanitizer<complex_t> V_L_12 { "V_L_12" };
    csl::InitSanitizer<complex_t> V_L_13 { "V_L_13" };
    csl::InitSanitizer<complex_t> V_L_14 { "V_L_14" };
    csl::InitSanitizer<complex_t> V_L_20 { "V_L_20" };
    csl::InitSanitizer<complex_t> V_L_21 { "V_L_21" };
    csl::InitSanitizer<complex_t> V_L_22 { "V_L_22" };
    csl::InitSanitizer<complex_t> V_L_23 { "V_L_23" };
    csl::InitSanitizer<complex_t> V_L_24 { "V_L_24" };
    csl::InitSanitizer<complex_t> V_L_30 { "V_L_30" };
    csl::InitSanitizer<complex_t> V_L_31 { "V_L_31" };
    csl::InitSanitizer<complex_t> V_L_32 { "V_L_32" };
    csl::InitSanitizer<complex_t> V_L_33 { "V_L_33" };
    csl::InitSanitizer<complex_t> V_L_34 { "V_L_34" };
    csl::InitSanitizer<complex_t> V_L_40 { "V_L_40" };
    csl::InitSanitizer<complex_t> V_L_41 { "V_L_41" };
    csl::InitSanitizer<complex_t> V_L_42 { "V_L_42" };
    csl::InitSanitizer<complex_t> V_L_43 { "V_L_43" };
    csl::InitSanitizer<complex_t> V_L_44 { "V_L_44" };
    csl::InitSanitizer<complex_t> V_Q_00 { "V_Q_00" };
    csl::InitSanitizer<complex_t> V_Q_01 { "V_Q_01" };
    csl::InitSanitizer<complex_t> V_Q_02 { "V_Q_02" };
    csl::InitSanitizer<complex_t> V_Q_03 { "V_Q_03" };
    csl::InitSanitizer<complex_t> V_Q_04 { "V_Q_04" };
    csl::InitSanitizer<complex_t> V_Q_10 { "V_Q_10" };
    csl::InitSanitizer<complex_t> V_Q_11 { "V_Q_11" };
    csl::InitSanitizer<complex_t> V_Q_12 { "V_Q_12" };
    csl::InitSanitizer<complex_t> V_Q_13 { "V_Q_13" };
    csl::InitSanitizer<complex_t> V_Q_14 { "V_Q_14" };
    csl::InitSanitizer<complex_t> V_Q_20 { "V_Q_20" };
    csl::InitSanitizer<complex_t> V_Q_21 { "V_Q_21" };
    csl::InitSanitizer<complex_t> V_Q_22 { "V_Q_22" };
    csl::InitSanitizer<complex_t> V_Q_23 { "V_Q_23" };
    csl::InitSanitizer<complex_t> V_Q_24 { "V_Q_24" };
    csl::InitSanitizer<complex_t> V_Q_30 { "V_Q_30" };
    csl::InitSanitizer<complex_t> V_Q_31 { "V_Q_31" };
    csl::InitSanitizer<complex_t> V_Q_32 { "V_Q_32" };
    csl::InitSanitizer<complex_t> V_Q_33 { "V_Q_33" };
    csl::InitSanitizer<complex_t> V_Q_34 { "V_Q_34" };
    csl::InitSanitizer<complex_t> V_Q_40 { "V_Q_40" };
    csl::InitSanitizer<complex_t> V_Q_41 { "V_Q_41" };
    csl::InitSanitizer<complex_t> V_Q_42 { "V_Q_42" };
    csl::InitSanitizer<complex_t> V_Q_43 { "V_Q_43" };
    csl::InitSanitizer<complex_t> V_Q_44 { "V_Q_44" };
    csl::InitSanitizer<complex_t> V_d_00 { "V_d_00" };
    csl::InitSanitizer<complex_t> V_d_01 { "V_d_01" };
    csl::InitSanitizer<complex_t> V_d_02 { "V_d_02" };
    csl::InitSanitizer<complex_t> V_d_10 { "V_d_10" };
    csl::InitSanitizer<complex_t> V_d_11 { "V_d_11" };
    csl::InitSanitizer<complex_t> V_d_12 { "V_d_12" };
    csl::InitSanitizer<complex_t> V_d_20 { "V_d_20" };
    csl::InitSanitizer<complex_t> V_d_21 { "V_d_21" };
    csl::InitSanitizer<complex_t> V_d_22 { "V_d_22" };

    void reset()
    {
        using real_params = std::array<csl::InitSanitizer<real_t>*, 42>;
        using complex_params = std::array<csl::InitSanitizer<complex_t>*, 155>;

        for (auto &par : real_params{
                &M_W, &M_Z, &m_H, &m_G0, &m_Gp, 
                &m_h0, &m_V_1, &m_V_2, &m_V_3, &m_b_L, &m_c_Z, 
                &m_c_Wm, &m_c_Wp, &m_d_L_1, &m_d_L_2, &m_L_L_0_2, &m_Q_L_0_1, 
                &m_Q_L_0_2, &m_{d_L}_1, &m_{d_L}_2, &m_Psi_dL_1, &m_Psi_dL_2, &m_Psi_lL_1, 
                &m_Psi_lL_2, &m_Psi_uL_1, &m_Psi_uL_2, &m_L_L_i_2_2, &m_Q_L_i_2_1, &m_Q_L_i_2_2, 
                &m_{L_L}_0_2, &m_{Q_L}_0_1, &m_\Psi_{dL}_1, &m_\Psi_{dL}_2, &m_\Psi_{lL}_1, &m_\Psi_{lL}_2, 
                &m_\Psi_{uL}_1, &m_\Psi_{uL}_2, &m_{L_L}_i_1_2, &m_{L_L}_i_2_2, &m_{Q_L}_i_1_1, &m_{Q_L}_i_1_2, 
                &m_{Q_L}_i_2_1, })
        {
            par->reset();
        }

        for (auto &par : complex_params{
                &M_D, &M_L, &M_U, &g_X, &v_x, 
                &v_y, &y_d, &y_l, &y_u, &y_d0, &y_l0, 
                &y_u0, &kappa_d0, &kappa_d1, &kappa_d2, &kappa_l0, &kappa_l1, 
                &kappa_l2, &kappa_u0, &kappa_u1, &lambda_d, &lambda_l, &lambda_u, 
                &tildekappa_u0, &tildekappa_u2, &tildelambda_d, &tildelambda_l, &tildelambda_u, &U_L_00, 
                &U_L_01, &U_L_02, &U_L_03, &U_L_04, &U_L_10, &U_L_11, 
                &U_L_12, &U_L_13, &U_L_14, &U_L_20, &U_L_21, &U_L_22, 
                &U_L_23, &U_L_24, &U_L_30, &U_L_31, &U_L_32, &U_L_33, 
                &U_L_34, &U_L_40, &U_L_41, &U_L_42, &U_L_43, &U_L_44, 
                &U_Q_00, &U_Q_01, &U_Q_02, &U_Q_03, &U_Q_04, &U_Q_10, 
                &U_Q_11, &U_Q_12, &U_Q_13, &U_Q_14, &U_Q_20, &U_Q_21, 
                &U_Q_22, &U_Q_23, &U_Q_24, &U_Q_30, &U_Q_31, &U_Q_32, 
                &U_Q_33, &U_Q_34, &U_Q_40, &U_Q_41, &U_Q_42, &U_Q_43, 
                &U_Q_44, &U_V_00, &U_V_01, &U_V_02, &U_V_10, &U_V_11, 
                &U_V_12, &U_V_20, &U_V_21, &U_V_22, &U_d_00, &U_d_01, 
                &U_d_02, &U_d_10, &U_d_11, &U_d_12, &U_d_20, &U_d_21, 
                &U_d_22, &V_L_00, &V_L_01, &V_L_02, &V_L_03, &V_L_04, 
                &V_L_10, &V_L_11, &V_L_12, &V_L_13, &V_L_14, &V_L_20, 
                &V_L_21, &V_L_22, &V_L_23, &V_L_24, &V_L_30, &V_L_31, 
                &V_L_32, &V_L_33, &V_L_34, &V_L_40, &V_L_41, &V_L_42, 
                &V_L_43, &V_L_44, &V_Q_00, &V_Q_01, &V_Q_02, &V_Q_03, 
                &V_Q_04, &V_Q_10, &V_Q_11, &V_Q_12, &V_Q_13, &V_Q_14, 
                &V_Q_20, &V_Q_21, &V_Q_22, &V_Q_23, &V_Q_24, &V_Q_30, 
                &V_Q_31, &V_Q_32, &V_Q_33, &V_Q_34, &V_Q_40, &V_Q_41, 
                &V_Q_42, &V_Q_43, &V_Q_44, &V_d_00, &V_d_01, &V_d_02, 
                &V_d_10, &V_d_11, &V_d_12, &V_d_20, &V_d_21, &V_d_22, })
        {
            par->reset();
        }
    }

    void print(std::ostream &out = std::cout)
    {
        using real_params = std::array<csl::InitSanitizer<real_t> const*, 42>;
        using complex_params = std::array<csl::InitSanitizer<complex_t> const*, 155>;

        out << "param_t struct:\n";
        out << "Real parameters\n";
        for (auto const &par : real_params{
                &M_W, &M_Z, &m_H, &m_G0, &m_Gp, 
                &m_h0, &m_V_1, &m_V_2, &m_V_3, &m_b_L, &m_c_Z, 
                &m_c_Wm, &m_c_Wp, &m_d_L_1, &m_d_L_2, &m_L_L_0_2, &m_Q_L_0_1, 
                &m_Q_L_0_2, &m_{d_L}_1, &m_{d_L}_2, &m_Psi_dL_1, &m_Psi_dL_2, &m_Psi_lL_1, 
                &m_Psi_lL_2, &m_Psi_uL_1, &m_Psi_uL_2, &m_L_L_i_2_2, &m_Q_L_i_2_1, &m_Q_L_i_2_2, 
                &m_{L_L}_0_2, &m_{Q_L}_0_1, &m_\Psi_{dL}_1, &m_\Psi_{dL}_2, &m_\Psi_{lL}_1, &m_\Psi_{lL}_2, 
                &m_\Psi_{uL}_1, &m_\Psi_{uL}_2, &m_{L_L}_i_1_2, &m_{L_L}_i_2_2, &m_{Q_L}_i_1_1, &m_{Q_L}_i_1_2, 
                &m_{Q_L}_i_2_1, })
        {
            out << "  -> ";
            par->print(out);
        }

        out << "Complex parameters\n";
        for (auto const &par : complex_params{
                &M_D, &M_L, &M_U, &g_X, &v_x, 
                &v_y, &y_d, &y_l, &y_u, &y_d0, &y_l0, 
                &y_u0, &kappa_d0, &kappa_d1, &kappa_d2, &kappa_l0, &kappa_l1, 
                &kappa_l2, &kappa_u0, &kappa_u1, &lambda_d, &lambda_l, &lambda_u, 
                &tildekappa_u0, &tildekappa_u2, &tildelambda_d, &tildelambda_l, &tildelambda_u, &U_L_00, 
                &U_L_01, &U_L_02, &U_L_03, &U_L_04, &U_L_10, &U_L_11, 
                &U_L_12, &U_L_13, &U_L_14, &U_L_20, &U_L_21, &U_L_22, 
                &U_L_23, &U_L_24, &U_L_30, &U_L_31, &U_L_32, &U_L_33, 
                &U_L_34, &U_L_40, &U_L_41, &U_L_42, &U_L_43, &U_L_44, 
                &U_Q_00, &U_Q_01, &U_Q_02, &U_Q_03, &U_Q_04, &U_Q_10, 
                &U_Q_11, &U_Q_12, &U_Q_13, &U_Q_14, &U_Q_20, &U_Q_21, 
                &U_Q_22, &U_Q_23, &U_Q_24, &U_Q_30, &U_Q_31, &U_Q_32, 
                &U_Q_33, &U_Q_34, &U_Q_40, &U_Q_41, &U_Q_42, &U_Q_43, 
                &U_Q_44, &U_V_00, &U_V_01, &U_V_02, &U_V_10, &U_V_11, 
                &U_V_12, &U_V_20, &U_V_21, &U_V_22, &U_d_00, &U_d_01, 
                &U_d_02, &U_d_10, &U_d_11, &U_d_12, &U_d_20, &U_d_21, 
                &U_d_22, &V_L_00, &V_L_01, &V_L_02, &V_L_03, &V_L_04, 
                &V_L_10, &V_L_11, &V_L_12, &V_L_13, &V_L_14, &V_L_20, 
                &V_L_21, &V_L_22, &V_L_23, &V_L_24, &V_L_30, &V_L_31, 
                &V_L_32, &V_L_33, &V_L_34, &V_L_40, &V_L_41, &V_L_42, 
                &V_L_43, &V_L_44, &V_Q_00, &V_Q_01, &V_Q_02, &V_Q_03, 
                &V_Q_04, &V_Q_10, &V_Q_11, &V_Q_12, &V_Q_13, &V_Q_14, 
                &V_Q_20, &V_Q_21, &V_Q_22, &V_Q_23, &V_Q_24, &V_Q_30, 
                &V_Q_31, &V_Q_32, &V_Q_33, &V_Q_34, &V_Q_40, &V_Q_41, 
                &V_Q_42, &V_Q_43, &V_Q_44, &V_d_00, &V_d_01, &V_d_02, 
                &V_d_10, &V_d_11, &V_d_12, &V_d_20, &V_d_21, &V_d_22, })
        {
            out << "  -> ";
            par->print(out);
        }
        out << "\n";
    }

    std::map<std::string, csl::InitSanitizer<real_t>*> realParams {
        {"M_W", &M_W},
        {"M_Z", &M_Z},
        {"m_H", &m_H},
        {"m_G0", &m_G0},
        {"m_Gp", &m_Gp},
        {"m_h0", &m_h0},
        {"m_V_1", &m_V_1},
        {"m_V_2", &m_V_2},
        {"m_V_3", &m_V_3},
        {"m_b_L", &m_b_L},
        {"m_c_Z", &m_c_Z},
        {"m_c_Wm", &m_c_Wm},
        {"m_c_Wp", &m_c_Wp},
        {"m_d_L_1", &m_d_L_1},
        {"m_d_L_2", &m_d_L_2},
        {"m_L_L_0_2", &m_L_L_0_2},
        {"m_Q_L_0_1", &m_Q_L_0_1},
        {"m_Q_L_0_2", &m_Q_L_0_2},
        {"m_{d_L}_1", &m_{d_L}_1},
        {"m_{d_L}_2", &m_{d_L}_2},
        {"m_Psi_dL_1", &m_Psi_dL_1},
        {"m_Psi_dL_2", &m_Psi_dL_2},
        {"m_Psi_lL_1", &m_Psi_lL_1},
        {"m_Psi_lL_2", &m_Psi_lL_2},
        {"m_Psi_uL_1", &m_Psi_uL_1},
        {"m_Psi_uL_2", &m_Psi_uL_2},
        {"m_L_L_i_2_2", &m_L_L_i_2_2},
        {"m_Q_L_i_2_1", &m_Q_L_i_2_1},
        {"m_Q_L_i_2_2", &m_Q_L_i_2_2},
        {"m_{L_L}_0_2", &m_{L_L}_0_2},
        {"m_{Q_L}_0_1", &m_{Q_L}_0_1},
        {"m_\Psi_{dL}_1", &m_\Psi_{dL}_1},
        {"m_\Psi_{dL}_2", &m_\Psi_{dL}_2},
        {"m_\Psi_{lL}_1", &m_\Psi_{lL}_1},
        {"m_\Psi_{lL}_2", &m_\Psi_{lL}_2},
        {"m_\Psi_{uL}_1", &m_\Psi_{uL}_1},
        {"m_\Psi_{uL}_2", &m_\Psi_{uL}_2},
        {"m_{L_L}_i_1_2", &m_{L_L}_i_1_2},
        {"m_{L_L}_i_2_2", &m_{L_L}_i_2_2},
        {"m_{Q_L}_i_1_1", &m_{Q_L}_i_1_1},
        {"m_{Q_L}_i_1_2", &m_{Q_L}_i_1_2},
        {"m_{Q_L}_i_2_1", &m_{Q_L}_i_2_1},
    };

    std::map<std::string, csl::InitSanitizer<complex_t>*> complexParams {
        {"M_D", &M_D},
        {"M_L", &M_L},
        {"M_U", &M_U},
        {"g_X", &g_X},
        {"v_x", &v_x},
        {"v_y", &v_y},
        {"y_d", &y_d},
        {"y_l", &y_l},
        {"y_u", &y_u},
        {"y_d0", &y_d0},
        {"y_l0", &y_l0},
        {"y_u0", &y_u0},
        {"kappa_d0", &kappa_d0},
        {"kappa_d1", &kappa_d1},
        {"kappa_d2", &kappa_d2},
        {"kappa_l0", &kappa_l0},
        {"kappa_l1", &kappa_l1},
        {"kappa_l2", &kappa_l2},
        {"kappa_u0", &kappa_u0},
        {"kappa_u1", &kappa_u1},
        {"lambda_d", &lambda_d},
        {"lambda_l", &lambda_l},
        {"lambda_u", &lambda_u},
        {"tildekappa_u0", &tildekappa_u0},
        {"tildekappa_u2", &tildekappa_u2},
        {"tildelambda_d", &tildelambda_d},
        {"tildelambda_l", &tildelambda_l},
        {"tildelambda_u", &tildelambda_u},
        {"U_L_00", &U_L_00},
        {"U_L_01", &U_L_01},
        {"U_L_02", &U_L_02},
        {"U_L_03", &U_L_03},
        {"U_L_04", &U_L_04},
        {"U_L_10", &U_L_10},
        {"U_L_11", &U_L_11},
        {"U_L_12", &U_L_12},
        {"U_L_13", &U_L_13},
        {"U_L_14", &U_L_14},
        {"U_L_20", &U_L_20},
        {"U_L_21", &U_L_21},
        {"U_L_22", &U_L_22},
        {"U_L_23", &U_L_23},
        {"U_L_24", &U_L_24},
        {"U_L_30", &U_L_30},
        {"U_L_31", &U_L_31},
        {"U_L_32", &U_L_32},
        {"U_L_33", &U_L_33},
        {"U_L_34", &U_L_34},
        {"U_L_40", &U_L_40},
        {"U_L_41", &U_L_41},
        {"U_L_42", &U_L_42},
        {"U_L_43", &U_L_43},
        {"U_L_44", &U_L_44},
        {"U_Q_00", &U_Q_00},
        {"U_Q_01", &U_Q_01},
        {"U_Q_02", &U_Q_02},
        {"U_Q_03", &U_Q_03},
        {"U_Q_04", &U_Q_04},
        {"U_Q_10", &U_Q_10},
        {"U_Q_11", &U_Q_11},
        {"U_Q_12", &U_Q_12},
        {"U_Q_13", &U_Q_13},
        {"U_Q_14", &U_Q_14},
        {"U_Q_20", &U_Q_20},
        {"U_Q_21", &U_Q_21},
        {"U_Q_22", &U_Q_22},
        {"U_Q_23", &U_Q_23},
        {"U_Q_24", &U_Q_24},
        {"U_Q_30", &U_Q_30},
        {"U_Q_31", &U_Q_31},
        {"U_Q_32", &U_Q_32},
        {"U_Q_33", &U_Q_33},
        {"U_Q_34", &U_Q_34},
        {"U_Q_40", &U_Q_40},
        {"U_Q_41", &U_Q_41},
        {"U_Q_42", &U_Q_42},
        {"U_Q_43", &U_Q_43},
        {"U_Q_44", &U_Q_44},
        {"U_V_00", &U_V_00},
        {"U_V_01", &U_V_01},
        {"U_V_02", &U_V_02},
        {"U_V_10", &U_V_10},
        {"U_V_11", &U_V_11},
        {"U_V_12", &U_V_12},
        {"U_V_20", &U_V_20},
        {"U_V_21", &U_V_21},
        {"U_V_22", &U_V_22},
        {"U_d_00", &U_d_00},
        {"U_d_01", &U_d_01},
        {"U_d_02", &U_d_02},
        {"U_d_10", &U_d_10},
        {"U_d_11", &U_d_11},
        {"U_d_12", &U_d_12},
        {"U_d_20", &U_d_20},
        {"U_d_21", &U_d_21},
        {"U_d_22", &U_d_22},
        {"V_L_00", &V_L_00},
        {"V_L_01", &V_L_01},
        {"V_L_02", &V_L_02},
        {"V_L_03", &V_L_03},
        {"V_L_04", &V_L_04},
        {"V_L_10", &V_L_10},
        {"V_L_11", &V_L_11},
        {"V_L_12", &V_L_12},
        {"V_L_13", &V_L_13},
        {"V_L_14", &V_L_14},
        {"V_L_20", &V_L_20},
        {"V_L_21", &V_L_21},
        {"V_L_22", &V_L_22},
        {"V_L_23", &V_L_23},
        {"V_L_24", &V_L_24},
        {"V_L_30", &V_L_30},
        {"V_L_31", &V_L_31},
        {"V_L_32", &V_L_32},
        {"V_L_33", &V_L_33},
        {"V_L_34", &V_L_34},
        {"V_L_40", &V_L_40},
        {"V_L_41", &V_L_41},
        {"V_L_42", &V_L_42},
        {"V_L_43", &V_L_43},
        {"V_L_44", &V_L_44},
        {"V_Q_00", &V_Q_00},
        {"V_Q_01", &V_Q_01},
        {"V_Q_02", &V_Q_02},
        {"V_Q_03", &V_Q_03},
        {"V_Q_04", &V_Q_04},
        {"V_Q_10", &V_Q_10},
        {"V_Q_11", &V_Q_11},
        {"V_Q_12", &V_Q_12},
        {"V_Q_13", &V_Q_13},
        {"V_Q_14", &V_Q_14},
        {"V_Q_20", &V_Q_20},
        {"V_Q_21", &V_Q_21},
        {"V_Q_22", &V_Q_22},
        {"V_Q_23", &V_Q_23},
        {"V_Q_24", &V_Q_24},
        {"V_Q_30", &V_Q_30},
        {"V_Q_31", &V_Q_31},
        {"V_Q_32", &V_Q_32},
        {"V_Q_33", &V_Q_33},
        {"V_Q_34", &V_Q_34},
        {"V_Q_40", &V_Q_40},
        {"V_Q_41", &V_Q_41},
        {"V_Q_42", &V_Q_42},
        {"V_Q_43", &V_Q_43},
        {"V_Q_44", &V_Q_44},
        {"V_d_00", &V_d_00},
        {"V_d_01", &V_d_01},
        {"V_d_02", &V_d_02},
        {"V_d_10", &V_d_10},
        {"V_d_11", &V_d_11},
        {"V_d_12", &V_d_12},
        {"V_d_20", &V_d_20},
        {"V_d_21", &V_d_21},
        {"V_d_22", &V_d_22},
    };

};


}

#endif
