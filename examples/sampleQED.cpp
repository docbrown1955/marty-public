/*
 * Simple QED model
 *
 * Create a QED model with a single electron (Dirac fermion) of charge -1
 * and compute several amplitudes in this model.
 */
#include <marty.h>

using namespace csl;
using namespace mty;

int main()
{
    Model QED;
    AddGaugedGroup(QED, group::Type::U1, "em", constant_s("e"));
    Init(QED);

    Particle electron = diracfermion_s("e", QED);
    SetMass(electron, "m_e");
    SetGroupRep(electron, "em", -1); // Set the charge -1 for group em
    AddParticle(QED, electron);
    Rename(QED, "A_em", "A"); // By default bosons are names A_<group_name>
    Particle photon = GetParticle(QED, "A");

    std::cout << QED << std::endl;

    auto rules = ComputeFeynmanRules(QED);
    Display(rules);
    Show(rules);

    auto compton = QED.computeAmplitude(Order::TreeLevel,
                                        {Incoming(electron),
                                         Incoming(photon),
                                         Outgoing(electron),
                                         Outgoing(photon)});
    Display(compton);
    Show(compton);

    auto diff = QED.computeAmplitude(Order::TreeLevel,
                                     {Incoming(electron),
                                      Incoming(electron),
                                      Outgoing(electron),
                                      Outgoing(electron)});
    Display(diff);
    Show(diff);
    DisplayAbbreviations();

    return 0;
}
