#include "bsmixingsm.h"
#include <fstream>

using namespace bsmixingsm;

double MW = 80.379;
double mt = 173;
double MW2 = MW*MW;
double MW4 = MW2*MW2;
double mt2 = mt*mt;
double mt4 = mt2*mt2;

double x = mt2 / MW2;

double F1(double x)
{
    return 2 * x * std::log(x) / std::pow(1 - x, 3)
        + (1 + x) / std::pow(1 - x, 2);
}
double F2(double x)
{
    return (1 + x) * std::log(x) / std::pow(1 - x, 3)
        + 2 / std::pow(1 - x, 2);
}

// Theoretical functions from Buras' book theory of weak decays
double WW_contrib()
{
    return F1(x);
}

double GG_contrib()
{
    return 1./4 * x * x * F1(x);
}

double GW_contrib()
{
    return -2 * x * x * F2(x);
}

double C()
{
    return 4 * MW2 * (WW_contrib() + GG_contrib() + GW_contrib());
}

int main() {

    param_t param;
    param.m_Gp = 80.379;
    std::ofstream fout("data.txt");
    fout.precision(10);
    for (x = 2; x < 100; ++x) {
        param.m_t = std::sqrt(x*MW2);
        fout << (C1(param) + C2(param)).real() << '\t';
        fout << (2*C1(param)).real() << '\t';
        fout << C() << '\n';
    }

    return 0;
}
