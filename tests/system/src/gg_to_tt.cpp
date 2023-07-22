/*
 * This program calculates the gg -> ttbar squared amplitude at the tree-level
 * in the Standard Model. Ghost calculations are performed to remove
 * un-physical polarizations from the results.
 */
#include "comparedata.h"
#include "testutility.h"
#include "marty/models/sm.h"
#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

// If true, the program use the built-in SM, otherwise a custom model is
// created with only what is needed. In both cases the test must work.
#define USE_SM 1

int main()
{

    mty::sm_input::redefineNumericalValues(); // for compatibility
#if USE_SM == 1
    mty::sm_input::g_s->setValue(CSL_UNDEF);
    mty::sm_input::m_t->setValue(CSL_UNDEF);
    SM_Model toy;
#else
    // Model building
    Model toy;
    // Gauge group to set --> SU(3)
    toy.addGaugedGroup(group::Type::SU, "C", 3, csl::constant_s("g_s"));
    toy.init();

    toy.renameParticle("A_C", "G");

    // Particle content   --> top quark
    Particle t  = diracfermion_s("t", toy);
    Expr     mt = constant_s("m_t");
    t->setMass(mt);
    t->setGroupRep("C", {1, 0});
    toy.addParticle(t);
    toy.refresh();
#endif

    Display(toy);

    auto rules = toy.getFeynmanRules();
    Display(rules);

    // Calculations (amplitudes + squares)
    auto gg = toy.computeAmplitude(Order::TreeLevel,
                                   {Incoming("G"),
                                    Incoming("G"),
                                    Outgoing("t"),
                                    Outgoing(AntiPart("t"))});

    for (size_t i = 0; i != gg.size(); ++i) {
        csl::Evaluate(gg.expression(i), csl::eval::abbreviation);
    }
    Display(gg);
    Expr s_gg = toy.computeSquaredAmplitude(gg);
    csl::Evaluate(s_gg, csl::eval::abbreviation);
    std::cout << s_gg << '\n';

    // Calculating the two ghost squared amplitudes
    auto cc1 = toy.computeAmplitude(Order::TreeLevel,
                                    {Incoming("c_G"),
                                     Incoming(AntiPart("c_G")),
                                     Outgoing("t"),
                                     Outgoing(AntiPart("t"))});
    auto cc2 = toy.computeAmplitude(Order::TreeLevel,
                                    {Incoming(AntiPart("c_G")),
                                     Incoming("c_G"),
                                     Outgoing("t"),
                                     Outgoing(AntiPart("t"))});

    Expr s_cc1 = toy.computeSquaredAmplitude(cc1);
    Expr s_cc2 = toy.computeSquaredAmplitude(cc2);

    std::cout << csl::Evaluated(s_cc1, csl::eval::abbreviation) << '\n';
    std::cout << csl::Evaluated(s_cc2, csl::eval::abbreviation) << '\n';

    // Library generation
    [[maybe_unused]] int sysres = system("rm -rf libs/gg_to_tt/");
    Library              lib("gg_to_tt", "libs");
    lib.addFunction("M2", s_gg);
    lib.addFunction("M2_ghost", s_cc1);
    lib.addFunction("M2_ghostc", s_cc2);
    defineLibPath(lib);
    lib.print();

    sysres = system("cp libsrc/example_gg_to_tt.cpp libs/gg_to_tt/script");
    sysres = system("cd libs/gg_to_tt; make && bin/example_gg_to_tt.x");
    return assert_equal(
        "data/output/reinterpretation.txt", "libs/gg_to_tt/data.txt", 1e-5);
}
