#include "comparedata.h"
#include "testutility.h"
#include "marty.h"
#include "marty/models/gthdm.h"

using namespace mty;
using namespace csl;
using namespace mty::sm_input;

int main()
{

    mty::sm_input::redefineNumericalValues(); // for compatibility
    GTHDM_Model model;

    Display(model);
    Display(model.getFeynmanRules());

    auto forceH                = filter::forceParticle("Hp");
    auto disableW              = filter::disableParticles({"W", "Gp"});
    auto disableLightQuarks    = filter::disableParticles({"u", "c"});
    auto disableNeutralHiggses = filter::disableParticles({"h", "H0", "A0"});

    FeynOptions options;
    options.addFilters(forceH, disableLightQuarks, disableNeutralHiggses);

    for (auto &sm_param : {M_W,
                           M_Z,
                           theta_W,
                           m_b,
                           m_s,
                           m_t,
                           m_mu,
                           e_em,
                           g_s,
                           V_ud,
                           V_us,
                           V_ub_mod,
                           V_cd,
                           V_cs,
                           V_cb,
                           V_td,
                           V_ts,
                           V_tb}) {
        std::cout << sm_param << " = "
                  << csl::Evaluated(sm_param, csl::eval::all) << std::endl;
        sm_param->setValue(CSL_UNDEF);
    }
    Expr s2W       = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr O7_factor = -csl::pow_s(e_em, 3) * GetComplexConjugate(V_ts) * V_tb
                     * m_b / (32 * CSL_PI * CSL_PI * M_W * M_W * s2W);
    Expr O8_factor = -csl::pow_s(e_em, 2) * g_s * GetComplexConjugate(V_ts)
                     * V_tb * m_b / (32 * CSL_PI * CSL_PI * M_W * M_W * s2W);
    Expr O9_factor = -csl::pow_s(e_em, 4) * GetComplexConjugate(V_ts) * V_tb
                     / (32 * CSL_PI * CSL_PI * M_W * M_W * s2W);

    options.setWilsonOperatorCoefficient(O7_factor);
    auto bsgamma = model.computeWilsonCoefficients(
        OneLoop, {Incoming("b"), Outgoing("s"), Outgoing("A")}, options);
    Display(bsgamma);
    std::cout << "Result for b -> s A" << std::endl;

    options.setWilsonOperatorCoefficient(O8_factor);
    auto bsg = model.computeWilsonCoefficients(
        OneLoop, {Incoming("b"), Outgoing("s"), Outgoing("G")}, options);
    Display(bsg);
    std::cout << "Result for b -> s g" << std::endl;

    options.setTopology(Topology::Triangle | Topology::Mass);
    options.setFermionOrder({1, 0, 2, 3});
    options.setWilsonOperatorCoefficient(O9_factor);
    options.addFilter(filter::disableParticle("Z"));
    auto bsll_A = model.computeWilsonCoefficients(OneLoop,
                                                  {Incoming("b"),
                                                   Outgoing("s"),
                                                   Outgoing("mu"),
                                                   Outgoing(AntiPart("mu"))},
                                                  options);
    Display(bsll_A);
    std::cout << "Result for b -> s l l (A)" << std::endl;

    options.resetFilters();
    options.addFilters(forceH, disableLightQuarks, disableNeutralHiggses);
    options.addFilter(filter::disableParticle("A"));
    auto bsll_Z = model.computeWilsonCoefficients(OneLoop,
                                                  {Incoming("b"),
                                                   Outgoing("s"),
                                                   Outgoing("mu"),
                                                   Outgoing(AntiPart("mu"))},
                                                  options);
    Display(bsll_Z);
    std::cout << "Result for b -> s l l (Z)" << std::endl;

    options.resetFilters();
    options.addFilters(
        forceH, disableW, disableLightQuarks, disableNeutralHiggses);
    options.setTopology(Topology::Box);
    auto bsll_box = model.computeWilsonCoefficients(OneLoop,
                                                    {Incoming("b"),
                                                     Outgoing("s"),
                                                     Outgoing("mu"),
                                                     Outgoing(AntiPart("mu"))},
                                                    options);
    Display(bsll_box);
    std::cout << "Result for b -> s l l (box)" << std::endl;

    auto O7   = chromoMagneticOperator(model, bsgamma, DiracCoupling::R);
    auto O8   = chromoMagneticOperator(model, bsg, DiracCoupling::R);
    auto O9_A = dimension6Operator(
        model, bsll_A, DiracCoupling::VL, DiracCoupling::V);
    auto O10_A = dimension6Operator(
        model, bsll_A, DiracCoupling::VL, DiracCoupling::A);
    auto O9_Z = dimension6Operator(
        model, bsll_Z, DiracCoupling::VL, DiracCoupling::V);
    auto O10_Z = dimension6Operator(
        model, bsll_Z, DiracCoupling::VL, DiracCoupling::A);
    auto O9_box = dimension6Operator(
        model, bsll_box, DiracCoupling::VL, DiracCoupling::V);
    auto O10_box = dimension6Operator(
        model, bsll_box, DiracCoupling::VL, DiracCoupling::A);

    auto C7      = getWilsonCoefficient(bsgamma, O7);
    auto C8      = getWilsonCoefficient(bsg, O8);
    auto C9_A    = getWilsonCoefficient(bsll_A, O9_A);
    auto C10_A   = getWilsonCoefficient(bsll_A, O10_A);
    auto C9_Z    = getWilsonCoefficient(bsll_Z, O9_Z);
    auto C10_Z   = getWilsonCoefficient(bsll_Z, O10_Z);
    auto C9_box  = getWilsonCoefficient(bsll_box, O9_box);
    auto C10_box = getWilsonCoefficient(bsll_box, O10_box);

    [[maybe_unused]] int sysres = system("rm -rf libs/wilsons_GTHDM");
    Library::setQuadruplePrecision(true);
    Library lib("wilsons_GTHDM", "libs");
    lib.generateSpectrum(model);
    lib.addFunction("C7", C7);
    lib.addFunction("C8", C8);
    lib.addFunction("C9_A", C9_A);
    lib.addFunction("C10_A", C10_A);
    lib.addFunction("C9_Z", C9_Z);
    lib.addFunction("C10_Z", C10_Z);
    lib.addFunction("C9_box", C9_box);
    lib.addFunction("C10_box", C10_box);
    defineLibPath(lib);
    lib.print();

    sysres = system("cp libsrc/example_thdm_paper.cpp "
                    "libs/wilsons_GTHDM/script");
    sysres = system("cd libs/wilsons_GTHDM; make && bin/example_thdm_paper.x");
    return assert_equal(
        "data/output/gthdm.txt", "libs/wilsons_GTHDM/data.txt", 1e-5);
}
