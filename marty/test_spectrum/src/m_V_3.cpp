#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_V_3.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_V_3(
        param_t const &param
        )
{
    clearcache();
    const real_t g_X = param.g_X;
    const real_t v_h = param.v_h;
    const complex_t IT_0000 = 0.5*g_X*v_h;
    return IT_0000;
}
} // End of namespace test_spectrum
