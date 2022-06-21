#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_\Psi_{lR}_1.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_\Psi_{lR}_1(
        param_t const &param
        )
{
    clearcache();
    const real_t M_L = param.M_L;
    const complex_t IT_0000 = -M_L;
    return IT_0000;
}
} // End of namespace test_spectrum
