#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_G0.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_G0(
        param_t const &
        )
{
    clearcache();
    return 91.1876;
}
} // End of namespace test_spectrum
