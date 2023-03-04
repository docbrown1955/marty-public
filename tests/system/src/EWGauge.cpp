#include "comparedata.h"
#include "testutility.h"
#include "marty/models/sm.h"
#include <marty.h>

using namespace mty;

int main()
{

    using namespace sm_input;
    for (auto &param : {M_W, M_Z, theta_W, m_h, e_em})
        param->setValue(CSL_UNDEF);

    SM_Model sm;

    auto ampl = sm.computeAmplitude(
        TreeLevel,
        {Incoming("W"), Incoming("Z"), Outgoing("W"), Outgoing("Z")});
    // Show(ampl);
    auto noHiggs = ampl.filterOut(
        [&](FeynmanDiagram const &diag) { return !diag.contains("h"); });
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

    Library::setQuadruplePrecision(true);
    Library lib("EWGauge", "libs");
    lib.cleanExistingSources();
    lib.setOptimization(false);
    lib.addFunction("M2_tot", M2tot);
    lib.addFunction("M2_V3", M2V3);
    lib.addFunction("M2_V4", M2V4);
    lib.addFunction("M2_nH", M2W);

    [[maybe_unused]] int sysres = system("rm -rf libs/EWGauge");
    defineLibPath(lib);
    lib.print();
    sysres = system("cp libsrc/example_ewgauge.cpp libs/EWGauge/script");
    sysres = system("cp libsrc/kinematics.h libs/EWGauge/include");
    sysres = system("cd libs/EWGauge; make && bin/example_ewgauge.x");
    return assert_equal(
        "data/output/EWgauge.txt", "libs/EWGauge/data.txt", 1e-3);
}
