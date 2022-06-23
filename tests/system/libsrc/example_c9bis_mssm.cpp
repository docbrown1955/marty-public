#include "c9_mssm.h"
#include "lha.h"
#include "read_spectrum.h"
#include "lhaData.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "ckm.h"

using namespace c9_mssm;

param_t params;

int main() {


    std::ofstream file("C9_marty_tanb50_ss.txt");
    file.precision(10);
    file << std::left;

    const std::string folder = "newdatafull_explicitspectrum";
    auto param1 = &params.M_2;
    auto param2 = &params.mu_h;
    *param1 = 0;
    *param2 = 0;
    fillCKM(params);
    size_t index = 0;
    for (size_t i = 0; i != 50; ++i) {
    for (size_t j = 0; j != 50; ++j) {
        if (index++ == 2307) continue; // Broken point
        std::cout << 50*i+j << " / " << 2500 << '\n';
        std::string fileName = folder + "/spectra/spectrum_" 
            + std::to_string(200*(4*i) + (4*j)) + ".txt";
        if (auto fileData = std::ifstream(fileName); fileData) {
            auto data = mty::lha::Reader::readFile(fileName);
            if (!data.empty()) {
                readSpectrum2(params, data);
            }
            else {
                file << std::setw(20) << (double)crealq(*param1);
                file << std::setw(20) << (double)crealq(*param2);
                file << "0 0 0 0\n";
                continue;
            }
        }
        else {
            file << std::setw(20) << (double)crealq(*param1);
            file << std::setw(20) << (double)crealq(*param2);
            file << "0 0 0 0\n";
            continue;
        }
        c9_mssm::setMu(80.379);
        auto s_12 = (params.m_b*params.m_b + params.m_s*params.m_s) / 2;
        params.s_12 = s_12;
        params.s_34 = -params.m_mu*params.m_mu;
        params.reg_prop = 1e-3;
        const __complex128 c9b = C9_B(params);
        const __complex128 c9z = C9_Z(params);
        const __complex128 c9a  = C9_A(params);
        const __complex128 c9tot  = C9_Tot(params);
        file << std::setw(20) << (double)crealq(*param1);
        file << std::setw(20) << (double)crealq(*param2);
        file << std::setw(20) << (double)crealq(c9b);
        file << std::setw(20) << (double)crealq(c9z);
        file << std::setw(20) << (double)crealq(c9a);
        file << std::setw(20) << (double)crealq(c9tot);
        file << std::endl;
    }
    }
    return 0;
}

