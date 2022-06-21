#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_{Q_L}_i_1_1.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_{Q_L}_i_1_1(
        param_t const &param
        )
{
    clearcache();
    const real_t v_h = param.v_h;
    const real_t y_u = param.y_u;
    const complex_t IT_0000 = 0.707106781186548*v_h*y_u;
    return IT_0000;
}
} // End of namespace test_spectrum
