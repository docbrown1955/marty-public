#include "clooptools.h"
#include "marty/looptools_init.h"
#include <cmath>
#include "common.h"

namespace test_spectrum {

void setMu(const double mu)
{
    setmudim(mu * mu);
}

void setLambda2(const double lambda2)
{
    setlambda(lambda2);
}

void setUVDiv(const double x)
{
    setuvdiv(x);
}

} // End of namespace test_spectrum

