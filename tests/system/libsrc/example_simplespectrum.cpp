#include "simplespectrum.h"
#include <fstream>

using namespace simplespectrum;

int main() {

    param_t params;
    params.g_D = 0.05;
    params.lSH = 0.25;
    params.lSS = -0.33;
    params.vph = 0.76;
    params.MPsi = 10.;
    params.mhH2 = 54.;

    updateSpectrum(params);

    std::ofstream fout("data.txt");
    fout.precision(15);
    fout << params.m_H.get() << std::endl;
    fout << params.m_VW.get() << std::endl;
    fout << params.m_VZ.get() << std::endl;
    fout << params.m_phR.get() << std::endl;
    fout << params.m_Psi_1.get() << std::endl;
    fout << params.m_Psi_2.get() << std::endl;

    return 0;
}
