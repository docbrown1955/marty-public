#include <marty.h>

using namespace csl;
using namespace mty;

int main()
{

    Model sQED("models/files/sQED.json");

    std::cout << sQED << std::endl;

    Particle phi    = sQED.getParticle("\\phi");
    Particle photon = sQED.getParticle("A");

    csl::Expr m = constant_s("m");
    csl::Expr g = constant_s("Gamma");
    phi->setMass(m);
    phi->setWidth(g);

    auto rules = ComputeFeynmanRules(sQED);
    Display(rules);
    Show(rules);

    mty::option::amputateExternalLegs = true;
    auto res                          = sQED.computeAmplitude(Order::TreeLevel,
                                     {Incoming(phi),
                                      Incoming(AntiPart(phi)),
                                      Outgoing(photon),
                                      Outgoing(photon)});

    Display(res);
    Show(res);
    DisplayAbbreviations();

    return 0;
}
