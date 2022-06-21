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
    auto const &g_X = inputs.g_X;
    auto const &v_x = inputs.v_x;
    auto const &v_y = inputs.v_y;

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

    return outputs;
}

void updateMassExpressions(param_t &params)
{
    params.m_H = m_H(params).real();
    params.m_W = m_W(params).real();
    params.m_Z = m_Z(params).real();
    params.m_{E_R}_1 = m_{E_R}_1(params).real();
    params.m_{E_R}_2 = m_{E_R}_2(params).real();
    params.m_{U_R}_1 = m_{U_R}_1(params).real();
    params.m_{U_R}_2 = m_{U_R}_2(params).real();
    params.m_{D_R}_1 = m_{D_R}_1(params).real();
    params.m_{D_R}_2 = m_{D_R}_2(params).real();
    params.m_{L_L}_0_2 = m_{L_L}_0_2(params).real();
    params.m_{Q_L}_0_1 = m_{Q_L}_0_1(params).real();
    params.m_{Q_L}_0_2 = m_{Q_L}_0_2(params).real();
    params.m_{L_L}_i_1_2 = m_{L_L}_i_1_2(params).real();
    params.m_{Q_L}_i_1_1 = m_{Q_L}_i_1_1(params).real();
    params.m_{Q_L}_i_1_2 = m_{Q_L}_i_1_2(params).real();
    params.m_Gp = m_Gp(params).real();
    params.m_h0 = m_h0(params).real();
}

} // End of namespace test_spectrum

