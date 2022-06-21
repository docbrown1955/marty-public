#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_{U_R}_2.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_{U_R}_2(
        param_t const &param
        )
{
    clearcache();
    const real_t v_h = param.v_h;
    const real_t tildekappa_u2 = param.tildekappa_u2;
    const complex_t IT_0000 = 0.707106781186548*v_h*tildekappa_u2;
    return IT_0000;
}
} // End of namespace test_spectrum
