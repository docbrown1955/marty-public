#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_\Psi_{dR}_1.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_\Psi_{dR}_1(
        param_t const &param
        )
{
    clearcache();
    const real_t M_D = param.M_D;
    const complex_t IT_0000 = -M_D;
    return IT_0000;
}
} // End of namespace test_spectrum
