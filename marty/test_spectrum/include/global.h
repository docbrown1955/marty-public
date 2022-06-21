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
    complex_t g_X;
    complex_t v_h;
    complex_t v_y;
};

////////////////////////////////////////////////////
// Here are the masses and mixings                 
// result of the diagonalization                   
////////////////////////////////////////////////////
struct SpectrumOutput {
    real_t m_V_1;
    real_t m_V_2;
    real_t m_V_3;

    complex_t U_V_00;
    complex_t U_V_01;
    complex_t U_V_02;
    complex_t U_V_10;
    complex_t U_V_11;
    complex_t U_V_12;
    complex_t U_V_20;
    complex_t U_V_21;
    complex_t U_V_22;
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
    diagData.g_X = input.g_X;
    diagData.v_h = input.v_h;
    diagData.v_y = input.v_y;
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
    output.U_V_00 = diagData.U_V_00;
    output.U_V_01 = diagData.U_V_01;
    output.U_V_02 = diagData.U_V_02;
    output.U_V_10 = diagData.U_V_10;
    output.U_V_11 = diagData.U_V_11;
    output.U_V_12 = diagData.U_V_12;
    output.U_V_20 = diagData.U_V_20;
    output.U_V_21 = diagData.U_V_21;
    output.U_V_22 = diagData.U_V_22;
}
void updateMassExpressions(param_t &params);


} // End of namespace test_spectrum

#endif
