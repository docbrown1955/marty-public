#include "QCD.h"
#include "mrtInterface.h"

namespace mty {

Particle colorTriplet(Model& QCD, std::string const& name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, "m_"+name);
    SetGroupRep(part, "SU3_c", {1, 0});
    AddParticle(QCD, part);
    return part;
}

Particle colorSextet(Model& QCD, std::string const& name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, "m_"+name);
    SetGroupRep(part, "SU3_c", {2, 0});
    AddParticle(QCD, part);
    return part;
}

QCD_Model::QCD_Model()
    :Model()
{
    addGaugedGroup(group::Type::SU, "C", 3, csl::constant_s("g"));
    init();

    Particle u = colorTriplet(*this, "u");
    Particle d = colorTriplet(*this, "d");
    // Uncomment to have a color sextet X
    // Particle X = colorSextet(*this, "X");
    renameParticle("A_C", "G");
    Particle gluon = getParticle("G");

    computeFeynmanRules();
}

}
