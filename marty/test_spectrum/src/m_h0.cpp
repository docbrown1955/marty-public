#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "m_h0.h"
#include "common.h"

#include "params.h"
#include "libcomplexop.h"

namespace test_spectrum {

complex_t m_h0(
        param_t const &param
        )
{
    clearcache();
    const real_t m_H = param.m_H;
    const complex_t IT_0000 = std::pow(m_H, 2);
    const complex_t IT_0001 = std::pow(23505.0486 + IT_0000, 0.5);
    return IT_0001;
}
} // End of namespace test_spectrum
