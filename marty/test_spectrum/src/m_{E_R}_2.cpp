#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_{E_R}_2.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_{E_R}_2(
        param_t const &param
        )
{
    clearcache();
    const real_t v_x = param.v_x;
    const real_t kappa_l2 = param.kappa_l2;
    const complex_t IT_0000 = (-0.707106781186548)*v_x*kappa_l2;
    return IT_0000;
}
} // End of namespace test_spectrum
