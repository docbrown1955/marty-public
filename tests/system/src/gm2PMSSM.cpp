#include "comparedata.h"
#include "testutility.h"
#include "marty.h"
#include "marty/models/pmssm_lem.h"

using namespace mty;
using namespace sm_input;

int main()
{
    mty::sm_input::redefineNumericalValues(); // for compatibility
    for (auto &value : {M_W, M_Z, theta_W, m_mu, e_em})
        value->setValue(CSL_UNDEF);

    PMSSM_LEM   pmssm;
    FeynOptions options;
    options.setTopology(Topology::Triangle);
    options.setWilsonOperatorCoefficient(-e_em / (4 * m_mu));
    options.addFilters(
        filter::disableParticles(
            {"W", "Z", "H0", "h", "A0", "Gp", "G0", "Hp"}),
        [&](FeynmanDiagram const &diag) { return !diag.isInLoop("A"); });

    Library::setQuadruplePrecision(true);
    Library lib("gm2PMSSM", "libs");
    lib.cleanExistingSources();
    lib.generateSpectrum(pmssm);
    csl::Expr aa = csl::constant_s("aa");

    mty::option::displayAbbreviations = false;
    {
        auto options_cpy = options;
        options_cpy.addFilter(filter::disableParticles({"C_1", "C_2"}));
        auto ampl = pmssm.computeAmplitude(
            OneLoop,
            {Incoming("mu"), Outgoing("mu"), Outgoing("A")},
            options_cpy);
        auto      wilsons = pmssm.getWilsonCoefficients(ampl, options);
        csl::Expr C       = getWilsonCoefficient(
            wilsons, chromoMagneticOperator(pmssm, wilsons, DiracCoupling::S));
        std::cout << "Neutralino" << '\n';
        lib.addFunction("gm2_N", C);
    }
    {
        auto options_cpy = options;
        options_cpy.addFilter(filter::forceParticles({"C_1", "C_2"}));
        auto ampl = pmssm.computeAmplitude(
            OneLoop,
            {Incoming("mu"), Outgoing("mu"), Outgoing("A")},
            options_cpy);
        auto      wilsons = pmssm.getWilsonCoefficients(ampl, options);
        csl::Expr C       = getWilsonCoefficient(
            wilsons, chromoMagneticOperator(pmssm, wilsons, DiracCoupling::S));
        std::cout << "Chargino" << '\n';
        std::cout << csl::Evaluated(C / 2, csl::eval::abbreviation) << '\n';
        lib.addFunction("gm2_C", C);
    }

    [[maybe_unused]] int sysres = system("rm -rf libs/gm2PMSSM");
    defineLibPath(lib);
    lib.print();
    sysres = system("cp libsrc/example_gm2pmssm.cpp libs/gm2PMSSM/script");
    sysres = system("cd libs/gm2PMSSM; make && bin/example_gm2pmssm.x");
    return assert_equal(
        "data/output/gm2_pmssm.txt", "libs/gm2PMSSM/data.txt", 1e-1);
}
