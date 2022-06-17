#include <marty.h>

using namespace csl;
using namespace mty;

int main()
{

    Model sQED;
    sQED.addGaugedGroup(group::Type::U1, "U1em", constant_s("e"));
    sQED.init();

    sQED.renameParticle("A_U1em", "A");

    Particle phi = scalarboson_s("phi ; \\phi", sQED);
    Expr     m   = constant_s("m");
    phi->setMass(m);
    phi->setGroupRep("U1em", -1);
    sQED.addParticle(phi);

    std::cout << sQED << std::endl;

    auto rules = ComputeFeynmanRules(sQED);
    Display(rules);
    Show(rules);

    mty::option::amputateExternalLegs = true;

    auto res = sQED.computeAmplitude(TreeLevel,
                                     {Incoming("phi"),
                                      Incoming(AntiPart("phi")),
                                      Outgoing("A"),
                                      Outgoing("A")});

    Display(res);
    Show(res);
    DisplayAbbreviations();

    return 0;
}
