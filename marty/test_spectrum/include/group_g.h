#ifndef CSL_LIB_test_spectrum_G_H_INCLUDED
#define CSL_LIB_test_spectrum_G_H_INCLUDED

#include <array>
#include "common.h"
#include "librarytensor.h"
#include "callable.h"
#include "csl/initSanitizer.h"
#include "params.h"
#include "m_H.h"
#include "m_c_Wm.h"
#include "m_h0.h"
#include "m_c_Z.h"
#include "m_Gp.h"
#include "m_G0.h"

namespace test_spectrum {


inline std::array<Callable<complex_t, param_t>, 6> f_G = {
    Callable{"m_H", m_H},
    Callable{"m_c_Wm", m_c_Wm},
    Callable{"m_h0", m_h0},
    Callable{"m_c_Z", m_c_Z},
    Callable{"m_Gp", m_Gp},
    Callable{"m_G0", m_G0},
};

inline std::map<std::string, Callable<complex_t, param_t>> fmap_G {
    {"m_H", f_G[0]},
    {"m_c_Wm", f_G[1]},
    {"m_h0", f_G[2]},
    {"m_c_Z", f_G[3]},
    {"m_Gp", f_G[4]},
    {"m_G0", f_G[5]},
};


}
 // End of namespace test_spectrum

#endif
