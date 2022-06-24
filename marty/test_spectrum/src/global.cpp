#include "global.h"
#include "libdiagonalization.h"
#include "test_spectrum.h"
#include "libcomplexop.h"

namespace test_spectrum {


void updateSpectrum(param_t &params)
{
    updateDiagonalization(params);
    updateMassExpressions(params);
}

void updateDiagonalization(param_t &params)
{
    SpectrumInput  inputs;
    readDiagonalizationInputs (inputs,  params);
    SpectrumOutput outputs = updateDiagonalization(inputs);
    readDiagonalizationOutputs(outputs, params);
}

SpectrumOutput updateDiagonalization(SpectrumInput const &inputs)
{
    auto const &M_D = inputs.M_D;
    auto const &M_L = inputs.M_L;
    auto const &M_U = inputs.M_U;
    auto const &g_X = inputs.g_X;
    auto const &kappa_d0 = inputs.kappa_d0;
    auto const &kappa_d1 = inputs.kappa_d1;
    auto const &kappa_d2 = inputs.kappa_d2;
    auto const &kappa_l0 = inputs.kappa_l0;
    auto const &kappa_l1 = inputs.kappa_l1;
    auto const &kappa_l2 = inputs.kappa_l2;
    auto const &kappa_u0 = inputs.kappa_u0;
    auto const &kappa_u1 = inputs.kappa_u1;
    auto const &lambda_d = inputs.lambda_d;
    auto const &lambda_l = inputs.lambda_l;
    auto const &lambda_u = inputs.lambda_u;
    auto const &m_Q_L_0_2 = inputs.m_Q_L_0_2;
    auto const &m_Q_L_i_2_2 = inputs.m_Q_L_i_2_2;
    auto const &tildekappa_u0 = inputs.tildekappa_u0;
    auto const &tildekappa_u2 = inputs.tildekappa_u2;
    auto const &tildelambda_d = inputs.tildelambda_d;
    auto const &tildelambda_l = inputs.tildelambda_l;
    auto const &tildelambda_u = inputs.tildelambda_u;
    auto const &v_x = inputs.v_x;
    auto const &v_y = inputs.v_y;
    auto const &y_d = inputs.y_d;
    auto const &y_d0 = inputs.y_d0;
    auto const &y_l = inputs.y_l;
    auto const &y_l0 = inputs.y_l0;
    auto const &y_u = inputs.y_u;
    auto const &y_u0 = inputs.y_u0;

    SpectrumOutput outputs;

    Diagonalizer::applyDiagonalization(
        {
            0.25*std::pow(g_X, 2)*(std::pow(v_x, 2) + 4*std::pow(v_y, 2)),
            0,
            0,
            0,
            0.25*std::pow(g_X, 2)*(std::pow(v_x, 2) + 4*std::pow(v_y, 2)),
            0,
            0,
            0,
            0.25*std::pow(g_X, 2)*std::pow(v_x, 2),
        },
        {&outputs.U_V_00, &outputs.U_V_01, &outputs.U_V_02, &outputs.U_V_10, &outputs.U_V_11, &outputs.U_V_12, &outputs.U_V_20, &outputs.U_V_21, &outputs.U_V_22, },
        {&outputs.m_V_1, &outputs.m_V_2, &outputs.m_V_3, }
        );
    if (0 > outputs.m_V_1) {
        std::cerr << "Warning: negative squared mass for " << "m_V_1" << ".\n";
    }
    outputs.m_V_1 = std::sqrt(outputs.m_V_1);
    if (0 > outputs.m_V_2) {
        std::cerr << "Warning: negative squared mass for " << "m_V_2" << ".\n";
    }
    outputs.m_V_2 = std::sqrt(outputs.m_V_2);
    if (0 > outputs.m_V_3) {
        std::cerr << "Warning: negative squared mass for " << "m_V_3" << ".\n";
    }
    outputs.m_V_3 = std::sqrt(outputs.m_V_3);

    Diagonalizer::applyBiDiagonalization(
        {
            0,
            0,
            0,
            0,
            (-0.707107)*v_x*kappa_u1,
            0,
            0,
            0,
            (-0.707107)*v_x*tildekappa_u2,
            0,
            0,
            0,
            (-174.104)*y_u0,
            (-0.707107)*v_x*tildekappa_u0,
            (-0.707107)*v_x*kappa_u0,
            (-174.104)*y_u,
            0,
            0,
            -M_U + (-0.353553)*v_y*lambda_u + (-0.707107)*v_y*tildelambda_u,
            0,
            0,
            (-174.104)*y_u,
            0,
            0,
            -M_U + 0.353553*v_y*lambda_u + 0.707107*v_y*tildelambda_u,
        },
        {&outputs.U_Q_00, &outputs.U_Q_01, &outputs.U_Q_02, &outputs.U_Q_03, &outputs.U_Q_04, &outputs.U_Q_10, &outputs.U_Q_11, &outputs.U_Q_12, &outputs.U_Q_13, &outputs.U_Q_14, &outputs.U_Q_20, &outputs.U_Q_21, &outputs.U_Q_22, &outputs.U_Q_23, &outputs.U_Q_24, &outputs.U_Q_30, &outputs.U_Q_31, &outputs.U_Q_32, &outputs.U_Q_33, &outputs.U_Q_34, &outputs.U_Q_40, &outputs.U_Q_41, &outputs.U_Q_42, &outputs.U_Q_43, &outputs.U_Q_44, },
        {&outputs.V_Q_00, &outputs.V_Q_01, &outputs.V_Q_02, &outputs.V_Q_03, &outputs.V_Q_04, &outputs.V_Q_10, &outputs.V_Q_11, &outputs.V_Q_12, &outputs.V_Q_13, &outputs.V_Q_14, &outputs.V_Q_20, &outputs.V_Q_21, &outputs.V_Q_22, &outputs.V_Q_23, &outputs.V_Q_24, &outputs.V_Q_30, &outputs.V_Q_31, &outputs.V_Q_32, &outputs.V_Q_33, &outputs.V_Q_34, &outputs.V_Q_40, &outputs.V_Q_41, &outputs.V_Q_42, &outputs.V_Q_43, &outputs.V_Q_44, },
        {&outputs.m_{Q_L}_i_1_1, &outputs.m_Q_L_i_2_1, &outputs.m_Q_L_0_1, &outputs.m_Psi_uL_1, &outputs.m_Psi_uL_2, }
        );

    Diagonalizer::applyBiDiagonalization(
        {
            0,
            0,
            0,
            0,
            (-0.707107)*v_x*kappa_d1,
            0,
            0,
            0,
            (-0.707107)*v_x*kappa_d2,
            0,
            0,
            0,
            (-174.104)*y_d0,
            (-0.707107)*v_x*kappa_d0,
            (-0.707107)*v_x*kappa_d0,
            (-174.104)*y_d,
            0,
            0,
            -M_D + (-0.353553)*v_y*lambda_d + (-0.707107)*v_y*tildelambda_d,
            0,
            0,
            (-174.104)*y_d,
            0,
            0,
            -M_D + 0.353553*v_y*lambda_d + 0.707107*v_y*tildelambda_d,
        },
        {&outputs.U_Q_00, &outputs.U_Q_01, &outputs.U_Q_02, &outputs.U_Q_03, &outputs.U_Q_04, &outputs.U_Q_10, &outputs.U_Q_11, &outputs.U_Q_12, &outputs.U_Q_13, &outputs.U_Q_14, &outputs.U_Q_20, &outputs.U_Q_21, &outputs.U_Q_22, &outputs.U_Q_23, &outputs.U_Q_24, &outputs.U_Q_30, &outputs.U_Q_31, &outputs.U_Q_32, &outputs.U_Q_33, &outputs.U_Q_34, &outputs.U_Q_40, &outputs.U_Q_41, &outputs.U_Q_42, &outputs.U_Q_43, &outputs.U_Q_44, },
        {&outputs.V_Q_00, &outputs.V_Q_01, &outputs.V_Q_02, &outputs.V_Q_03, &outputs.V_Q_04, &outputs.V_Q_10, &outputs.V_Q_11, &outputs.V_Q_12, &outputs.V_Q_13, &outputs.V_Q_14, &outputs.V_Q_20, &outputs.V_Q_21, &outputs.V_Q_22, &outputs.V_Q_23, &outputs.V_Q_24, &outputs.V_Q_30, &outputs.V_Q_31, &outputs.V_Q_32, &outputs.V_Q_33, &outputs.V_Q_34, &outputs.V_Q_40, &outputs.V_Q_41, &outputs.V_Q_42, &outputs.V_Q_43, &outputs.V_Q_44, },
        {&outputs.m_{Q_L}_i_1_2, &outputs.m_Q_L_i_2_2, &outputs.m_Q_L_0_2, &outputs.m_Psi_dL_1, &outputs.m_Psi_dL_2, }
        );

    Diagonalizer::applyBiDiagonalization(
        {
            0,
            0,
            0,
            0,
            (-0.707107)*v_x*kappa_l1,
            0,
            0,
            0,
            (-0.707107)*v_x*kappa_l2,
            0,
            0,
            0,
            (-174.104)*y_l0,
            (-0.707107)*v_x*kappa_l0,
            (-0.707107)*v_x*kappa_l0,
            (-174.104)*y_l,
            0,
            0,
            -M_L + (-0.353553)*v_y*lambda_l + (-0.707107)*v_y*tildelambda_l,
            0,
            0,
            (-174.104)*y_l,
            0,
            0,
            -M_L + 0.353553*v_y*lambda_l + 0.707107*v_y*tildelambda_l,
        },
        {&outputs.U_L_00, &outputs.U_L_01, &outputs.U_L_02, &outputs.U_L_03, &outputs.U_L_04, &outputs.U_L_10, &outputs.U_L_11, &outputs.U_L_12, &outputs.U_L_13, &outputs.U_L_14, &outputs.U_L_20, &outputs.U_L_21, &outputs.U_L_22, &outputs.U_L_23, &outputs.U_L_24, &outputs.U_L_30, &outputs.U_L_31, &outputs.U_L_32, &outputs.U_L_33, &outputs.U_L_34, &outputs.U_L_40, &outputs.U_L_41, &outputs.U_L_42, &outputs.U_L_43, &outputs.U_L_44, },
        {&outputs.V_L_00, &outputs.V_L_01, &outputs.V_L_02, &outputs.V_L_03, &outputs.V_L_04, &outputs.V_L_10, &outputs.V_L_11, &outputs.V_L_12, &outputs.V_L_13, &outputs.V_L_14, &outputs.V_L_20, &outputs.V_L_21, &outputs.V_L_22, &outputs.V_L_23, &outputs.V_L_24, &outputs.V_L_30, &outputs.V_L_31, &outputs.V_L_32, &outputs.V_L_33, &outputs.V_L_34, &outputs.V_L_40, &outputs.V_L_41, &outputs.V_L_42, &outputs.V_L_43, &outputs.V_L_44, },
        {&outputs.m_{L_L}_i_1_2, &outputs.m_L_L_i_2_2, &outputs.m_L_L_0_2, &outputs.m_Psi_lL_1, &outputs.m_Psi_lL_2, }
        );

    Diagonalizer::applyBiDiagonalization(
        {
            0,
            0,
            0,
            0,
            m_Q_L_i_2_2,
            0,
            0,
            0,
            m_Q_L_0_2,
        },
        {&outputs.U_d_00, &outputs.U_d_01, &outputs.U_d_02, &outputs.U_d_10, &outputs.U_d_11, &outputs.U_d_12, &outputs.U_d_20, &outputs.U_d_21, &outputs.U_d_22, },
        {&outputs.V_d_00, &outputs.V_d_01, &outputs.V_d_02, &outputs.V_d_10, &outputs.V_d_11, &outputs.V_d_12, &outputs.V_d_20, &outputs.V_d_21, &outputs.V_d_22, },
        {&outputs.m_d_L_1, &outputs.m_d_L_2, &outputs.m_b_L, }
        );

    return outputs;
}

void updateMassExpressions(param_t &params)
{
    params.m_H = m_H(params).real();
    params.m_c_Wm = m_c_Wm(params).real();
    params.m_h0 = m_h0(params).real();
    params.m_c_Z = m_c_Z(params).real();
    params.m_Gp = m_Gp(params).real();
    params.m_G0 = m_G0(params).real();
}

} // End of namespace test_spectrum

