#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_{Q_L}_0_2.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_{Q_L}_0_2(
        param_t const &param
        )
{
    clearcache();
    const real_t y_d0 = param.y_d0;
    const complex_t IT_0000 = (-174.103831663752)*y_d0;
    return IT_0000;
}
} // End of namespace test_spectrum