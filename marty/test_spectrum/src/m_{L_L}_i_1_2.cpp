#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_{L_L}_i_1_2.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_{L_L}_i_1_2(
        param_t const &param
        )
{
    clearcache();
    const real_t y_l = param.y_l;
    const complex_t IT_0000 = (-174.103831663752)*y_l;
    return IT_0000;
}
} // End of namespace test_spectrum