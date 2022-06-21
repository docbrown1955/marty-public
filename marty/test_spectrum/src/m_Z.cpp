#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_Z.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_Z(
        param_t const &param
        )
{
    clearcache();
    const real_t v_h = param.v_h;
    const complex_t IT_0000 = 0.359477442758314*v_h;
    return IT_0000;
}
} // End of namespace test_spectrum
