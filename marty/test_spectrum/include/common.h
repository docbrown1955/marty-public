#ifndef CSL_LIB_test_spectrum_COMMON_H_INCLUDED
#define CSL_LIB_test_spectrum_COMMON_H_INCLUDED
#include <complex>
#define CSL_LT_DISABLE_ITERATOR
#include "librarytensor.h"

namespace test_spectrum {

using real_t = double;
using complex_t = std::complex<double>;
static constexpr complex_t _i_{0, 1};

void setMu(const double mu);
void setLambda2(const double lambda2);
void setUVDiv(const double x);

} // End of namespace test_spectrum

#endif
