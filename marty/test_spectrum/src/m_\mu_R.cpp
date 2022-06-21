#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_\mu_R.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_\mu_R(
        param_t const &param
        )
{
    clearcache();
    const real_t v_h = param.v_h;
    const real_t kappa_l1 = param.kappa_l1;
    const complex_t IT_0000 = (-0.707106781186548)*v_h*kappa_l1;
    return IT_0000;
}
} // End of namespace test_spectrum
