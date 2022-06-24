#include "c9_mssm.h"
#include "ckm.h"
#include "marty/lha/lha.h"
#include "read_spectrum.h"
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace c9_mssm;

param_t params;

int main()
{

    std::ofstream file("C9_marty_tanb50_ss.txt");
    file.precision(10);
    file << std::left;

    const std::string folder = "../../data/input";
    auto              param1 = &params.M_2;
    auto              param2 = &params.mu_h;
    *param1                  = 0;
    *param2                  = 0;
    fillCKM(params);
    for (size_t i = 0; i <= 100; ++i) {
        std::cout << i << " / 100\n";
        std::string fileName
            = folder + "/spectra/spectrum_" + std::to_string(i) + ".txt";
        if (auto fileData = std::ifstream(fileName); fileData) {
            auto data = mty::lha::Reader::readFile(fileName);
            if (!data.empty()) {
                readSpectrum2(params, data);
            }
            else {
                file << std::setw(20) << (double) crealq(*param1);
                file << std::setw(20) << (double) crealq(*param2);
                file << "0 0 0 0\n";
                continue;
            }
        }
        else {
            file << std::setw(20) << (double) crealq(*param1);
            file << std::setw(20) << (double) crealq(*param2);
            file << "0 0 0 0\n";
            continue;
        }
        c9_mssm::setMu(80.379);
        auto s_12   = (params.m_b * params.m_b + params.m_s * params.m_s) / 2;
        params.s_12 = s_12;
        params.s_34 = -params.m_mu * params.m_mu;
        params.reg_prop          = 1e-3;
        const __complex128 c9b   = C9_B(params);
        const __complex128 c9z   = C9_Z(params);
        const __complex128 c9a   = C9_A(params);
        const __complex128 c9tot = C9_Tot(params);
        file << std::setw(20) << (double) crealq(*param1);
        file << std::setw(20) << (double) crealq(*param2);
        file << std::setw(20) << (double) crealq(c9b);
        file << std::setw(20) << (double) crealq(c9z);
        file << std::setw(20) << (double) crealq(c9a);
        file << std::setw(20) << (double) crealq(c9tot);
        file << std::endl;
    }
    return 0;
}
