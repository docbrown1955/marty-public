#include <marty>

using namespace csl;
using namespace mty;

int main() {

    Model QED;
    AddGaugedGroup(QED, group::Type::U1, "U1_em", constant_s("e"));
    Init(QED);

    Particle electron = diracfermion_s("e", QED);
    SetMass(electron, "m_e");
    SetGroupRep(electron, "U1_em", {-1, 1});
    AddParticle(QED, electron);
    Rename(QED, "A_U1_em", "A");
    Particle photon = GetParticle(QED, "A");

    std::cout << QED << std::endl;

    auto rules = ComputeFeynmanRules(QED);
    Display(rules);
    Show(rules);

    auto compton = QED.computeAmplitude(
                Order::TreeLevel,
                { Incoming(electron), Incoming(photon),
                  Outgoing(electron), Outgoing(photon)});
    Display(compton);
    Show(compton);

    auto diff = QED.computeAmplitude(
                Order::TreeLevel,
                { Incoming(electron), Incoming(electron),
                  Outgoing(electron), Outgoing(electron)});
    Display(diff);
    Show(diff);
    DisplayAbbreviations();

    return 0;
}
