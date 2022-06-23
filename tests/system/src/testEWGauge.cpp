#include <marty.h>
#include "marty/models/sm.h"

using namespace mty;

int main() {

    std::cout.setstate(std::ios_base::failbit);

    using namespace sm_input;
    for (auto &param : {M_W, M_Z, theta_W, m_h, e_em})
        param->setValue(CSL_UNDEF);

    SM_Model sm;

    auto ampl = sm.computeAmplitude(
            TreeLevel,
            {Incoming("W"), Incoming("Z"), Outgoing("W"), Outgoing("Z")}
            );
    // Show(ampl);
    auto noHiggs = ampl.filterOut([&](FeynmanDiagram const &diag) {
        return !diag.contains("h0");
    });
    auto V4 = noHiggs.filterOut([&](FeynmanDiagram const &diag) {
        return !diag.isMediator("W") && !diag.isMediator("Gp");
    });
    auto V3 = noHiggs.filterOut([&](FeynmanDiagram const &diag) {
        return diag.isMediator("W") || diag.isMediator("Gp");
    });

    auto M2tot = sm.computeSquaredAmplitude(ampl);
    auto M2V3  = sm.computeSquaredAmplitude(V3);
    auto M2V4  = sm.computeSquaredAmplitude(V4);
    auto M2W   = sm.computeSquaredAmplitude(noHiggs);

    Library{""}.setQuadruplePrecision(true);
    Library lib("EWGauge");
    lib.cleanExistingSources();
    lib.addFunction("M2_tot", M2tot);
    lib.addFunction("M2_V3", M2V3);
    lib.addFunction("M2_V4", M2V4);
    lib.addFunction("M2_nH", M2W);
    lib.print();

    [[maybe_unused]] int sysres = system(
            "cd EWGauge; rm data.txt; rm bin/*; sed -i 's/-O2 $(MATH_OPTI)//g' Makefile;");
    sysres = system("cp libscripts/example_ewgauge.cpp EWGauge/script");
    sysres = system("cp libscripts/kinematics.h EWGauge/include");
    sysres = system("cd EWGauge; make >/dev/null");
    sysres = system("cd EWGauge; bin/example_ewgauge.x>/dev/null");
    sysres = system("bin/comparedata.x data/EWgauge.txt EWGauge/data.txt 1e-3");

    return sysres;
}
