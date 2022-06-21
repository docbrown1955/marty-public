#ifndef CSL_LIB_test_spectrum_G_H_INCLUDED
#define CSL_LIB_test_spectrum_G_H_INCLUDED

#include <array>
#include "common.h"
#include "librarytensor.h"
#include "callable.h"
#include "csl/initSanitizer.h"
#include "params.h"
#include "m_H.h"
#include "m_W.h"
#include "m_Z.h"
#include "m_{E_R}_1.h"
#include "m_{E_R}_2.h"
#include "m_{U_R}_1.h"
#include "m_{U_R}_2.h"
#include "m_{D_R}_1.h"
#include "m_{D_R}_2.h"
#include "m_{L_L}_0_2.h"
#include "m_{Q_L}_0_1.h"
#include "m_{Q_L}_0_2.h"
#include "m_{L_L}_i_1_2.h"
#include "m_{Q_L}_i_1_1.h"
#include "m_{Q_L}_i_1_2.h"
#include "m_Gp.h"
#include "m_h0.h"

namespace test_spectrum {


inline std::array<Callable<complex_t, param_t>, 17> f_G = {
    Callable{"m_H", m_H},
    Callable{"m_W", m_W},
    Callable{"m_Z", m_Z},
    Callable{"m_{E_R}_1", m_{E_R}_1},
    Callable{"m_{E_R}_2", m_{E_R}_2},
    Callable{"m_{U_R}_1", m_{U_R}_1},
    Callable{"m_{U_R}_2", m_{U_R}_2},
    Callable{"m_{D_R}_1", m_{D_R}_1},
    Callable{"m_{D_R}_2", m_{D_R}_2},
    Callable{"m_{L_L}_0_2", m_{L_L}_0_2},
    Callable{"m_{Q_L}_0_1", m_{Q_L}_0_1},
    Callable{"m_{Q_L}_0_2", m_{Q_L}_0_2},
    Callable{"m_{L_L}_i_1_2", m_{L_L}_i_1_2},
    Callable{"m_{Q_L}_i_1_1", m_{Q_L}_i_1_1},
    Callable{"m_{Q_L}_i_1_2", m_{Q_L}_i_1_2},
    Callable{"m_Gp", m_Gp},
    Callable{"m_h0", m_h0},
};

inline std::map<std::string, Callable<complex_t, param_t>> fmap_G {
    {"m_H", f_G[0]},
    {"m_W", f_G[1]},
    {"m_Z", f_G[2]},
    {"m_{E_R}_1", f_G[3]},
    {"m_{E_R}_2", f_G[4]},
    {"m_{U_R}_1", f_G[5]},
    {"m_{U_R}_2", f_G[6]},
    {"m_{D_R}_1", f_G[7]},
    {"m_{D_R}_2", f_G[8]},
    {"m_{L_L}_0_2", f_G[9]},
    {"m_{Q_L}_0_1", f_G[10]},
    {"m_{Q_L}_0_2", f_G[11]},
    {"m_{L_L}_i_1_2", f_G[12]},
    {"m_{Q_L}_i_1_1", f_G[13]},
    {"m_{Q_L}_i_1_2", f_G[14]},
    {"m_Gp", f_G[15]},
    {"m_h0", f_G[16]},
};


}
 // End of namespace test_spectrum

#endif
