#include "marty.h"
#include "marty/models/gthdm.h"

using namespace mty;
using namespace csl;
using namespace mty::sm_input;

constexpr bool usePause = false;

void pause()
{
    if constexpr (usePause) {
        std::cout << "Press enter to continue ..." << std::endl;
        std::cin.get();
    }
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    GTHDM_Model model;

    Display(model);
    Display(model.getFeynmanRules());

    auto forceH = filter::forceParticle("Hp");
    auto disableW = filter::disableParticles({"W", "Gp"});
    auto disableLightQuarks = filter::disableParticles({"u", "c"});
    auto disableNeutralHiggses = filter::disableParticles({"h0", "H0", "A0"});

    FeynOptions options;
    options.addFilters(forceH, disableLightQuarks, disableNeutralHiggses);

    for (auto &sm_param : 
            {M_W, M_Z, theta_W, m_b, m_s, m_t, m_mu, e_em, g_s,
            V_ud, V_us, V_ub_mod, V_cd, V_cs, V_cb, V_td, V_ts, V_tb})
    {
        std::cout << sm_param << " = " << csl::Evaluated(sm_param, csl::eval::all) << std::endl;
        sm_param->setValue(CSL_UNDEF);
    }
    Expr s2W = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr O7_factor = -csl::pow_s(e_em, 3) 
        * GetComplexConjugate(V_ts) * V_tb * m_b
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2W);
    Expr O8_factor = -csl::pow_s(e_em, 2) * g_s 
        * GetComplexConjugate(V_ts) * V_tb * m_b
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2W);
    Expr O9_factor = -csl::pow_s(e_em, 4) 
        * GetComplexConjugate(V_ts) * V_tb 
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2W);

    options.setWilsonOperatorCoefficient(O7_factor);
    auto bsgamma = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("s"), Outgoing("A")},
            options
            );
    Display(bsgamma);
    std::cout << "Result for b -> s A" << std::endl;
    pause();

    options.setWilsonOperatorCoefficient(O8_factor);
    auto bsg = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("s"), Outgoing("G")},
            options
            );
    Display(bsg);
    std::cout << "Result for b -> s g" << std::endl;
    pause();

    options.setTopology(Topology::Triangle | Topology::Mass);
    options.setFermionOrder({1, 0, 2, 3});
    options.setWilsonOperatorCoefficient(O9_factor);
    options.addFilter(filter::disableParticle("Z"));
    auto bsll_A = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("s"), 
            Outgoing("mu"), Outgoing(AntiPart("mu"))},
            options
            );
    Display(bsll_A);
    std::cout << "Result for b -> s l l (A)" << std::endl;
    pause();

    options.resetFilters();
    options.addFilters(forceH, disableLightQuarks, disableNeutralHiggses);
    options.addFilter(filter::disableParticle("A"));
    auto bsll_Z = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("s"), 
            Outgoing("mu"), Outgoing(AntiPart("mu"))},
            options
            );
    Display(bsll_Z);
    std::cout << "Result for b -> s l l (Z)" << std::endl;
    pause();

    options.resetFilters();
    options.addFilters(forceH, disableW, disableLightQuarks, disableNeutralHiggses);
    options.setTopology(Topology::Box);
    auto bsll_box = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("s"), 
            Outgoing("mu"), Outgoing(AntiPart("mu"))},
            options
            );
    Display(bsll_box);
    std::cout << "Result for b -> s l l (box)" << std::endl;
    pause();

    auto O7  = chromoMagneticOperator(model, bsgamma, DiracCoupling::R);
    auto O8  = chromoMagneticOperator(model, bsg,     DiracCoupling::R);
    auto O9_A  = dimension6Operator(model, bsll_A, DiracCoupling::VL, DiracCoupling::V);
    auto O10_A = dimension6Operator(model, bsll_A, DiracCoupling::VL, DiracCoupling::A);
    auto O9_Z  = dimension6Operator(model, bsll_Z, DiracCoupling::VL, DiracCoupling::V);
    auto O10_Z = dimension6Operator(model, bsll_Z, DiracCoupling::VL, DiracCoupling::A);
    auto O9_box  = dimension6Operator(model, bsll_box, DiracCoupling::VL, DiracCoupling::V);
    auto O10_box = dimension6Operator(model, bsll_box, DiracCoupling::VL, DiracCoupling::A);

    auto C7 = getWilsonCoefficient(bsgamma, O7);
    auto C8 = getWilsonCoefficient(bsg,     O8);
    auto C9_A  = getWilsonCoefficient(bsll_A, O9_A);
    auto C10_A = getWilsonCoefficient(bsll_A, O10_A);
    auto C9_Z  = getWilsonCoefficient(bsll_Z, O9_Z);
    auto C10_Z = getWilsonCoefficient(bsll_Z, O10_Z);
    auto C9_box  = getWilsonCoefficient(bsll_box, O9_box);
    auto C10_box = getWilsonCoefficient(bsll_box, O10_box);

    [[maybe_unused]] int sysres = system("rm -r THDM_paper");
    csl::LibraryGenerator::setQuadruplePrecision(true);
    Library lib("THDM_paper");
    lib.cleanExistingSources();
    lib.generateSpectrum(model);
    lib.addFunction("C7", C7);
    lib.addFunction("C8", C8);
    lib.addFunction("C9_A",  C9_A);
    lib.addFunction("C10_A", C10_A);
    lib.addFunction("C9_Z",  C9_Z);
    lib.addFunction("C10_Z", C10_Z);
    lib.addFunction("C9_box",  C9_box);
    lib.addFunction("C10_box", C10_box);
    lib.print();

    sysres = system("cp libscripts/example_thdm_paper.cpp THDM_paper/script");
    sysres = system("cd THDM_paper; make >/dev/null && bin/example_thdm_paper.x > /dev/null");
    std::cout.clear();
    sysres = system( "bin/comparedata.x data/gthdm.txt THDM_paper/data.txt 1e-5");

    return sysres;
}
