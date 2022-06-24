#ifndef CSL_LIB_GLOBAL
#define CSL_LIB_GLOBAL
#include "params.h"
#include "common.h"

namespace test_spectrum {

void updateSpectrum(param_t &params);

struct SpectrumInput;
struct SpectrumOutput;

SpectrumOutput updateDiagonalization(SpectrumInput const&);

void updateDiagonalization(param_t &params);

////////////////////////////////////////////////////
// Here are the parameters to set before calling    
// updateDiagonalization()                          
////////////////////////////////////////////////////
struct SpectrumInput {
    complex_t M_D;
    complex_t M_L;
    complex_t M_U;
    complex_t g_X;
    complex_t kappa_d0;
    complex_t kappa_d1;
    complex_t kappa_d2;
    complex_t kappa_l0;
    complex_t kappa_l1;
    complex_t kappa_l2;
    complex_t kappa_u0;
    complex_t kappa_u1;
    complex_t lambda_d;
    complex_t lambda_l;
    complex_t lambda_u;
    complex_t m_Q_L_0_2;
    complex_t m_Q_L_i_2_2;
    complex_t tildekappa_u0;
    complex_t tildekappa_u2;
    complex_t tildelambda_d;
    complex_t tildelambda_l;
    complex_t tildelambda_u;
    complex_t v_x;
    complex_t v_y;
    complex_t y_d;
    complex_t y_d0;
    complex_t y_l;
    complex_t y_l0;
    complex_t y_u;
    complex_t y_u0;
};

////////////////////////////////////////////////////
// Here are the masses and mixings                 
// result of the diagonalization                   
////////////////////////////////////////////////////
struct SpectrumOutput {
    real_t m_V_1;
    real_t m_V_2;
    real_t m_V_3;
    real_t m_Q_L_i_1_1;
    real_t m_Q_L_i_2_1;
    real_t m_Q_L_0_1;
    real_t m_Psi_uL_1;
    real_t m_Psi_uL_2;
    real_t m_Q_L_i_1_2;
    real_t m_Q_L_i_2_2;
    real_t m_Q_L_0_2;
    real_t m_Psi_dL_1;
    real_t m_Psi_dL_2;
    real_t m_L_L_i_1_2;
    real_t m_L_L_i_2_2;
    real_t m_L_L_0_2;
    real_t m_Psi_lL_1;
    real_t m_Psi_lL_2;
    real_t m_d_L_1;
    real_t m_d_L_2;
    real_t m_b_L;

    complex_t U_L_00;
    complex_t U_L_01;
    complex_t U_L_02;
    complex_t U_L_03;
    complex_t U_L_04;
    complex_t U_L_10;
    complex_t U_L_11;
    complex_t U_L_12;
    complex_t U_L_13;
    complex_t U_L_14;
    complex_t U_L_20;
    complex_t U_L_21;
    complex_t U_L_22;
    complex_t U_L_23;
    complex_t U_L_24;
    complex_t U_L_30;
    complex_t U_L_31;
    complex_t U_L_32;
    complex_t U_L_33;
    complex_t U_L_34;
    complex_t U_L_40;
    complex_t U_L_41;
    complex_t U_L_42;
    complex_t U_L_43;
    complex_t U_L_44;
    complex_t U_Q_00;
    complex_t U_Q_01;
    complex_t U_Q_02;
    complex_t U_Q_03;
    complex_t U_Q_04;
    complex_t U_Q_10;
    complex_t U_Q_11;
    complex_t U_Q_12;
    complex_t U_Q_13;
    complex_t U_Q_14;
    complex_t U_Q_20;
    complex_t U_Q_21;
    complex_t U_Q_22;
    complex_t U_Q_23;
    complex_t U_Q_24;
    complex_t U_Q_30;
    complex_t U_Q_31;
    complex_t U_Q_32;
    complex_t U_Q_33;
    complex_t U_Q_34;
    complex_t U_Q_40;
    complex_t U_Q_41;
    complex_t U_Q_42;
    complex_t U_Q_43;
    complex_t U_Q_44;
    complex_t U_V_00;
    complex_t U_V_01;
    complex_t U_V_02;
    complex_t U_V_10;
    complex_t U_V_11;
    complex_t U_V_12;
    complex_t U_V_20;
    complex_t U_V_21;
    complex_t U_V_22;
    complex_t U_d_00;
    complex_t U_d_01;
    complex_t U_d_02;
    complex_t U_d_10;
    complex_t U_d_11;
    complex_t U_d_12;
    complex_t U_d_20;
    complex_t U_d_21;
    complex_t U_d_22;
    complex_t V_L_00;
    complex_t V_L_01;
    complex_t V_L_02;
    complex_t V_L_03;
    complex_t V_L_04;
    complex_t V_L_10;
    complex_t V_L_11;
    complex_t V_L_12;
    complex_t V_L_13;
    complex_t V_L_14;
    complex_t V_L_20;
    complex_t V_L_21;
    complex_t V_L_22;
    complex_t V_L_23;
    complex_t V_L_24;
    complex_t V_L_30;
    complex_t V_L_31;
    complex_t V_L_32;
    complex_t V_L_33;
    complex_t V_L_34;
    complex_t V_L_40;
    complex_t V_L_41;
    complex_t V_L_42;
    complex_t V_L_43;
    complex_t V_L_44;
    complex_t V_Q_00;
    complex_t V_Q_01;
    complex_t V_Q_02;
    complex_t V_Q_03;
    complex_t V_Q_04;
    complex_t V_Q_10;
    complex_t V_Q_11;
    complex_t V_Q_12;
    complex_t V_Q_13;
    complex_t V_Q_14;
    complex_t V_Q_20;
    complex_t V_Q_21;
    complex_t V_Q_22;
    complex_t V_Q_23;
    complex_t V_Q_24;
    complex_t V_Q_30;
    complex_t V_Q_31;
    complex_t V_Q_32;
    complex_t V_Q_33;
    complex_t V_Q_34;
    complex_t V_Q_40;
    complex_t V_Q_41;
    complex_t V_Q_42;
    complex_t V_Q_43;
    complex_t V_Q_44;
    complex_t V_d_00;
    complex_t V_d_01;
    complex_t V_d_02;
    complex_t V_d_10;
    complex_t V_d_11;
    complex_t V_d_12;
    complex_t V_d_20;
    complex_t V_d_21;
    complex_t V_d_22;
};

////////////////////////////////////////////////////
// Here is a generic function to read results      
// of the diagonalization in a corresponding struct
////////////////////////////////////////////////////

template<class Type>
void readDiagonalizationInputs(
        SpectrumInput &diagData,
        Type    const &input
        )
{
    diagData.M_D = input.M_D;
    diagData.M_L = input.M_L;
    diagData.M_U = input.M_U;
    diagData.g_X = input.g_X;
    diagData.kappa_d0 = input.kappa_d0;
    diagData.kappa_d1 = input.kappa_d1;
    diagData.kappa_d2 = input.kappa_d2;
    diagData.kappa_l0 = input.kappa_l0;
    diagData.kappa_l1 = input.kappa_l1;
    diagData.kappa_l2 = input.kappa_l2;
    diagData.kappa_u0 = input.kappa_u0;
    diagData.kappa_u1 = input.kappa_u1;
    diagData.lambda_d = input.lambda_d;
    diagData.lambda_l = input.lambda_l;
    diagData.lambda_u = input.lambda_u;
    diagData.m_Q_L_0_2 = input.m_Q_L_0_2;
    diagData.m_Q_L_i_2_2 = input.m_Q_L_i_2_2;
    diagData.tildekappa_u0 = input.tildekappa_u0;
    diagData.tildekappa_u2 = input.tildekappa_u2;
    diagData.tildelambda_d = input.tildelambda_d;
    diagData.tildelambda_l = input.tildelambda_l;
    diagData.tildelambda_u = input.tildelambda_u;
    diagData.v_x = input.v_x;
    diagData.v_y = input.v_y;
    diagData.y_d = input.y_d;
    diagData.y_d0 = input.y_d0;
    diagData.y_l = input.y_l;
    diagData.y_l0 = input.y_l0;
    diagData.y_u = input.y_u;
    diagData.y_u0 = input.y_u0;
}

template<class Type>
void readDiagonalizationOutputs(
        SpectrumOutput const &diagData,
        Type                 &output
        )
{
    output.m_V_1 = diagData.m_V_1;
    output.m_V_2 = diagData.m_V_2;
    output.m_V_3 = diagData.m_V_3;
    output.m_Q_L_i_1_1 = diagData.m_Q_L_i_1_1;
    output.m_Q_L_i_2_1 = diagData.m_Q_L_i_2_1;
    output.m_Q_L_0_1 = diagData.m_Q_L_0_1;
    output.m_Psi_uL_1 = diagData.m_Psi_uL_1;
    output.m_Psi_uL_2 = diagData.m_Psi_uL_2;
    output.m_Q_L_i_1_2 = diagData.m_Q_L_i_1_2;
    output.m_Q_L_i_2_2 = diagData.m_Q_L_i_2_2;
    output.m_Q_L_0_2 = diagData.m_Q_L_0_2;
    output.m_Psi_dL_1 = diagData.m_Psi_dL_1;
    output.m_Psi_dL_2 = diagData.m_Psi_dL_2;
    output.m_L_L_i_1_2 = diagData.m_L_L_i_1_2;
    output.m_L_L_i_2_2 = diagData.m_L_L_i_2_2;
    output.m_L_L_0_2 = diagData.m_L_L_0_2;
    output.m_Psi_lL_1 = diagData.m_Psi_lL_1;
    output.m_Psi_lL_2 = diagData.m_Psi_lL_2;
    output.m_d_L_1 = diagData.m_d_L_1;
    output.m_d_L_2 = diagData.m_d_L_2;
    output.m_b_L = diagData.m_b_L;
    output.U_L_00 = diagData.U_L_00;
    output.U_L_01 = diagData.U_L_01;
    output.U_L_02 = diagData.U_L_02;
    output.U_L_03 = diagData.U_L_03;
    output.U_L_04 = diagData.U_L_04;
    output.U_L_10 = diagData.U_L_10;
    output.U_L_11 = diagData.U_L_11;
    output.U_L_12 = diagData.U_L_12;
    output.U_L_13 = diagData.U_L_13;
    output.U_L_14 = diagData.U_L_14;
    output.U_L_20 = diagData.U_L_20;
    output.U_L_21 = diagData.U_L_21;
    output.U_L_22 = diagData.U_L_22;
    output.U_L_23 = diagData.U_L_23;
    output.U_L_24 = diagData.U_L_24;
    output.U_L_30 = diagData.U_L_30;
    output.U_L_31 = diagData.U_L_31;
    output.U_L_32 = diagData.U_L_32;
    output.U_L_33 = diagData.U_L_33;
    output.U_L_34 = diagData.U_L_34;
    output.U_L_40 = diagData.U_L_40;
    output.U_L_41 = diagData.U_L_41;
    output.U_L_42 = diagData.U_L_42;
    output.U_L_43 = diagData.U_L_43;
    output.U_L_44 = diagData.U_L_44;
    output.U_Q_00 = diagData.U_Q_00;
    output.U_Q_01 = diagData.U_Q_01;
    output.U_Q_02 = diagData.U_Q_02;
    output.U_Q_03 = diagData.U_Q_03;
    output.U_Q_04 = diagData.U_Q_04;
    output.U_Q_10 = diagData.U_Q_10;
    output.U_Q_11 = diagData.U_Q_11;
    output.U_Q_12 = diagData.U_Q_12;
    output.U_Q_13 = diagData.U_Q_13;
    output.U_Q_14 = diagData.U_Q_14;
    output.U_Q_20 = diagData.U_Q_20;
    output.U_Q_21 = diagData.U_Q_21;
    output.U_Q_22 = diagData.U_Q_22;
    output.U_Q_23 = diagData.U_Q_23;
    output.U_Q_24 = diagData.U_Q_24;
    output.U_Q_30 = diagData.U_Q_30;
    output.U_Q_31 = diagData.U_Q_31;
    output.U_Q_32 = diagData.U_Q_32;
    output.U_Q_33 = diagData.U_Q_33;
    output.U_Q_34 = diagData.U_Q_34;
    output.U_Q_40 = diagData.U_Q_40;
    output.U_Q_41 = diagData.U_Q_41;
    output.U_Q_42 = diagData.U_Q_42;
    output.U_Q_43 = diagData.U_Q_43;
    output.U_Q_44 = diagData.U_Q_44;
    output.U_V_00 = diagData.U_V_00;
    output.U_V_01 = diagData.U_V_01;
    output.U_V_02 = diagData.U_V_02;
    output.U_V_10 = diagData.U_V_10;
    output.U_V_11 = diagData.U_V_11;
    output.U_V_12 = diagData.U_V_12;
    output.U_V_20 = diagData.U_V_20;
    output.U_V_21 = diagData.U_V_21;
    output.U_V_22 = diagData.U_V_22;
    output.U_d_00 = diagData.U_d_00;
    output.U_d_01 = diagData.U_d_01;
    output.U_d_02 = diagData.U_d_02;
    output.U_d_10 = diagData.U_d_10;
    output.U_d_11 = diagData.U_d_11;
    output.U_d_12 = diagData.U_d_12;
    output.U_d_20 = diagData.U_d_20;
    output.U_d_21 = diagData.U_d_21;
    output.U_d_22 = diagData.U_d_22;
    output.V_L_00 = diagData.V_L_00;
    output.V_L_01 = diagData.V_L_01;
    output.V_L_02 = diagData.V_L_02;
    output.V_L_03 = diagData.V_L_03;
    output.V_L_04 = diagData.V_L_04;
    output.V_L_10 = diagData.V_L_10;
    output.V_L_11 = diagData.V_L_11;
    output.V_L_12 = diagData.V_L_12;
    output.V_L_13 = diagData.V_L_13;
    output.V_L_14 = diagData.V_L_14;
    output.V_L_20 = diagData.V_L_20;
    output.V_L_21 = diagData.V_L_21;
    output.V_L_22 = diagData.V_L_22;
    output.V_L_23 = diagData.V_L_23;
    output.V_L_24 = diagData.V_L_24;
    output.V_L_30 = diagData.V_L_30;
    output.V_L_31 = diagData.V_L_31;
    output.V_L_32 = diagData.V_L_32;
    output.V_L_33 = diagData.V_L_33;
    output.V_L_34 = diagData.V_L_34;
    output.V_L_40 = diagData.V_L_40;
    output.V_L_41 = diagData.V_L_41;
    output.V_L_42 = diagData.V_L_42;
    output.V_L_43 = diagData.V_L_43;
    output.V_L_44 = diagData.V_L_44;
    output.V_Q_00 = diagData.V_Q_00;
    output.V_Q_01 = diagData.V_Q_01;
    output.V_Q_02 = diagData.V_Q_02;
    output.V_Q_03 = diagData.V_Q_03;
    output.V_Q_04 = diagData.V_Q_04;
    output.V_Q_10 = diagData.V_Q_10;
    output.V_Q_11 = diagData.V_Q_11;
    output.V_Q_12 = diagData.V_Q_12;
    output.V_Q_13 = diagData.V_Q_13;
    output.V_Q_14 = diagData.V_Q_14;
    output.V_Q_20 = diagData.V_Q_20;
    output.V_Q_21 = diagData.V_Q_21;
    output.V_Q_22 = diagData.V_Q_22;
    output.V_Q_23 = diagData.V_Q_23;
    output.V_Q_24 = diagData.V_Q_24;
    output.V_Q_30 = diagData.V_Q_30;
    output.V_Q_31 = diagData.V_Q_31;
    output.V_Q_32 = diagData.V_Q_32;
    output.V_Q_33 = diagData.V_Q_33;
    output.V_Q_34 = diagData.V_Q_34;
    output.V_Q_40 = diagData.V_Q_40;
    output.V_Q_41 = diagData.V_Q_41;
    output.V_Q_42 = diagData.V_Q_42;
    output.V_Q_43 = diagData.V_Q_43;
    output.V_Q_44 = diagData.V_Q_44;
    output.V_d_00 = diagData.V_d_00;
    output.V_d_01 = diagData.V_d_01;
    output.V_d_02 = diagData.V_d_02;
    output.V_d_10 = diagData.V_d_10;
    output.V_d_11 = diagData.V_d_11;
    output.V_d_12 = diagData.V_d_12;
    output.V_d_20 = diagData.V_d_20;
    output.V_d_21 = diagData.V_d_21;
    output.V_d_22 = diagData.V_d_22;
}
void updateMassExpressions(param_t &params);


} // End of namespace test_spectrum

#endif
