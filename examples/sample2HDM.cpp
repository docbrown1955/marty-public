/*
 * Two Higgs Doublet Model example
 *
 * Creates a THDM model of type II and calculates Wilson coefficients
 * selecting particular contributions.
 */
#include <marty.h>
#include <marty/models/thdm.h>

using namespace mty;

int main()
{

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between
    // 1 and 4 (inclusive)
    // Type 2 is the MSSM-like 2HDM
    ///////////////////////////////////////////////////

    constexpr int type = 2;

    mty::THDM_Model<type> THDM;
    std::cout << THDM << std::endl;

    FeynOptions options;
    options.setTopology(Topology::Box);
    options.addFilter(filter::forceParticleCombination({"W", "t"}));
    options.addFilter(filter::disableParticles({"u_L", "c_L", "Hp", "Gp"}));
    options.setFermionOrder({3, 0, 1, 2});
    mty::option::keepOnlyFirstMassInLoop = true;

    auto ampl = THDM.computeAmplitude(OneLoop,
                                      {Incoming("d"),
                                       Incoming(AntiPart("s")),
                                       Outgoing(AntiPart("d")),
                                       Outgoing("s")},
                                      options);
    Display(ampl);
    Show(ampl);

    auto wilsons = THDM.getWilsonCoefficients(ampl);
    Display(wilsons);

    return 0;
}
