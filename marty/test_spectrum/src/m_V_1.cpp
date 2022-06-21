#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_V_1.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_V_1(
        param_t const &param
        )
{
    clearcache();
    const real_t g_X = param.g_X;
    const real_t v_h = param.v_h;
    const real_t v_y = param.v_y;
    const complex_t IT_0000 = std::pow(v_h, 2);
    const complex_t IT_0001 = std::pow(v_y, 2);
    const complex_t IT_0002 = std::pow(IT_0000 + 4*IT_0001, 0.5);
    const complex_t IT_0003 = 0.5*g_X*IT_0002;
    return IT_0003;
}
} // End of namespace test_spectrum
