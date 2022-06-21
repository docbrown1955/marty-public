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

    csl::InitSanitizer<real_t> v_h { "v_h" };
    csl::InitSanitizer<real_t> y_d { "y_d" };
    csl::InitSanitizer<real_t> y_l { "y_l" };
    csl::InitSanitizer<real_t> y_u { "y_u" };
    csl::InitSanitizer<real_t> y_d0 { "y_d0" };
    csl::InitSanitizer<real_t> y_l0 { "y_l0" };
    csl::InitSanitizer<real_t> y_u0 { "y_u0" };
    csl::InitSanitizer<real_t> kappa_d1 { "kappa_d1" };
    csl::InitSanitizer<real_t> kappa_d2 { "kappa_d2" };
    csl::InitSanitizer<real_t> kappa_l1 { "kappa_l1" };
    csl::InitSanitizer<real_t> kappa_l2 { "kappa_l2" };
    csl::InitSanitizer<real_t> kappa_u1 { "kappa_u1" };
    csl::InitSanitizer<real_t> tildekappa_u2 { "tildekappa_u2" };
    csl::InitSanitizer<complex_t> g_X { "g_X" };
    csl::InitSanitizer<complex_t> v_y { "v_y" };


    ///////////////////////////////////////
    // Parameters functions of others  
    // through diagonalization or mass 
    // expressions, see updateSpectrum()  
    // in global.h or set them by hand  
    // 
    // And other default parameters  
    ///////////////////////////////////////

    csl::InitSanitizer<real_t> m_H { "m_H" };
    csl::InitSanitizer<real_t> m_W { "m_W" };
    csl::InitSanitizer<real_t> m_Z { "m_Z" };
    csl::InitSanitizer<real_t> m_V_1 { "m_V_1" };
    csl::InitSanitizer<real_t> m_V_2 { "m_V_2" };
    csl::InitSanitizer<real_t> m_V_3 { "m_V_3" };
    csl::InitSanitizer<real_t> m_{D_R}_1 { "m_{D_R}_1" };
    csl::InitSanitizer<real_t> m_{D_R}_2 { "m_{D_R}_2" };
    csl::InitSanitizer<real_t> m_{E_R}_1 { "m_{E_R}_1" };
    csl::InitSanitizer<real_t> m_{E_R}_2 { "m_{E_R}_2" };
    csl::InitSanitizer<real_t> m_{U_R}_1 { "m_{U_R}_1" };
    csl::InitSanitizer<real_t> m_{U_R}_2 { "m_{U_R}_2" };
    csl::InitSanitizer<real_t> m_{L_L}_0_2 { "m_{L_L}_0_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_0_1 { "m_{Q_L}_0_1" };
    csl::InitSanitizer<real_t> m_{Q_L}_0_2 { "m_{Q_L}_0_2" };
    csl::InitSanitizer<real_t> m_{L_L}_i_1_2 { "m_{L_L}_i_1_2" };
    csl::InitSanitizer<real_t> m_{L_L}_i_2_2 { "m_{L_L}_i_2_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_1_1 { "m_{Q_L}_i_1_1" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_1_2 { "m_{Q_L}_i_1_2" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_2_1 { "m_{Q_L}_i_2_1" };
    csl::InitSanitizer<real_t> m_{Q_L}_i_2_2 { "m_{Q_L}_i_2_2" };
    csl::InitSanitizer<complex_t> U_V_00 { "U_V_00" };
    csl::InitSanitizer<complex_t> U_V_01 { "U_V_01" };
    csl::InitSanitizer<complex_t> U_V_02 { "U_V_02" };
    csl::InitSanitizer<complex_t> U_V_10 { "U_V_10" };
    csl::InitSanitizer<complex_t> U_V_11 { "U_V_11" };
    csl::InitSanitizer<complex_t> U_V_12 { "U_V_12" };
    csl::InitSanitizer<complex_t> U_V_20 { "U_V_20" };
    csl::InitSanitizer<complex_t> U_V_21 { "U_V_21" };
    csl::InitSanitizer<complex_t> U_V_22 { "U_V_22" };

    void reset()
    {
        using real_params = std::array<csl::InitSanitizer<real_t>*, 34>;
        using complex_params = std::array<csl::InitSanitizer<complex_t>*, 11>;

        for (auto &par : real_params{
                &v_h, &y_d, &y_l, &y_u, &y_d0, 
                &y_l0, &y_u0, &kappa_d1, &kappa_d2, &kappa_l1, &kappa_l2, 
                &kappa_u1, &tildekappa_u2, &m_H, &m_W, &m_Z, &m_V_1, 
                &m_V_2, &m_V_3, &m_{D_R}_1, &m_{D_R}_2, &m_{E_R}_1, &m_{E_R}_2, 
                &m_{U_R}_1, &m_{U_R}_2, &m_{L_L}_0_2, &m_{Q_L}_0_1, &m_{Q_L}_0_2, &m_{L_L}_i_1_2, 
                &m_{L_L}_i_2_2, &m_{Q_L}_i_1_1, &m_{Q_L}_i_1_2, &m_{Q_L}_i_2_1, &m_{Q_L}_i_2_2, })
        {
            par->reset();
        }

        for (auto &par : complex_params{
                &g_X, &v_y, &U_V_00, &U_V_01, &U_V_02, 
                &U_V_10, &U_V_11, &U_V_12, &U_V_20, &U_V_21, &U_V_22, })
        {
            par->reset();
        }
    }

    void print(std::ostream &out = std::cout)
    {
        using real_params = std::array<csl::InitSanitizer<real_t> const*, 34>;
        using complex_params = std::array<csl::InitSanitizer<complex_t> const*, 11>;

        out << "param_t struct:\n";
        out << "Real parameters\n";
        for (auto const &par : real_params{
                &v_h, &y_d, &y_l, &y_u, &y_d0, 
                &y_l0, &y_u0, &kappa_d1, &kappa_d2, &kappa_l1, &kappa_l2, 
                &kappa_u1, &tildekappa_u2, &m_H, &m_W, &m_Z, &m_V_1, 
                &m_V_2, &m_V_3, &m_{D_R}_1, &m_{D_R}_2, &m_{E_R}_1, &m_{E_R}_2, 
                &m_{U_R}_1, &m_{U_R}_2, &m_{L_L}_0_2, &m_{Q_L}_0_1, &m_{Q_L}_0_2, &m_{L_L}_i_1_2, 
                &m_{L_L}_i_2_2, &m_{Q_L}_i_1_1, &m_{Q_L}_i_1_2, &m_{Q_L}_i_2_1, &m_{Q_L}_i_2_2, })
        {
            out << "  -> ";
            par->print(out);
        }

        out << "Complex parameters\n";
        for (auto const &par : complex_params{
                &g_X, &v_y, &U_V_00, &U_V_01, &U_V_02, 
                &U_V_10, &U_V_11, &U_V_12, &U_V_20, &U_V_21, &U_V_22, })
        {
            out << "  -> ";
            par->print(out);
        }
        out << "\n";
    }

    std::map<std::string, csl::InitSanitizer<real_t>*> realParams {
        {"v_h", &v_h},
        {"y_d", &y_d},
        {"y_l", &y_l},
        {"y_u", &y_u},
        {"y_d0", &y_d0},
        {"y_l0", &y_l0},
        {"y_u0", &y_u0},
        {"kappa_d1", &kappa_d1},
        {"kappa_d2", &kappa_d2},
        {"kappa_l1", &kappa_l1},
        {"kappa_l2", &kappa_l2},
        {"kappa_u1", &kappa_u1},
        {"tildekappa_u2", &tildekappa_u2},
        {"m_H", &m_H},
        {"m_W", &m_W},
        {"m_Z", &m_Z},
        {"m_V_1", &m_V_1},
        {"m_V_2", &m_V_2},
        {"m_V_3", &m_V_3},
        {"m_{D_R}_1", &m_{D_R}_1},
        {"m_{D_R}_2", &m_{D_R}_2},
        {"m_{E_R}_1", &m_{E_R}_1},
        {"m_{E_R}_2", &m_{E_R}_2},
        {"m_{U_R}_1", &m_{U_R}_1},
        {"m_{U_R}_2", &m_{U_R}_2},
        {"m_{L_L}_0_2", &m_{L_L}_0_2},
        {"m_{Q_L}_0_1", &m_{Q_L}_0_1},
        {"m_{Q_L}_0_2", &m_{Q_L}_0_2},
        {"m_{L_L}_i_1_2", &m_{L_L}_i_1_2},
        {"m_{L_L}_i_2_2", &m_{L_L}_i_2_2},
        {"m_{Q_L}_i_1_1", &m_{Q_L}_i_1_1},
        {"m_{Q_L}_i_1_2", &m_{Q_L}_i_1_2},
        {"m_{Q_L}_i_2_1", &m_{Q_L}_i_2_1},
        {"m_{Q_L}_i_2_2", &m_{Q_L}_i_2_2},
    };

    std::map<std::string, csl::InitSanitizer<complex_t>*> complexParams {
        {"g_X", &g_X},
        {"v_y", &v_y},
        {"U_V_00", &U_V_00},
        {"U_V_01", &U_V_01},
        {"U_V_02", &U_V_02},
        {"U_V_10", &U_V_10},
        {"U_V_11", &U_V_11},
        {"U_V_12", &U_V_12},
        {"U_V_20", &U_V_20},
        {"U_V_21", &U_V_21},
        {"U_V_22", &U_V_22},
    };

};


}

#endif
