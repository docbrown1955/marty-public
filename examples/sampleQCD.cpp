/*
 * Simple QCD model
 *
 * Create a QCD model with a color triplet and a color sextet
 * and compute a complete cross-section in this model.
 */
#include <marty.h>

using namespace csl;
using namespace mty;

Particle colorTriplet(Model &QCD, std::string const &name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, "m_" + name);
    SetGroupRep(part, "C", {1, 0});
    AddParticle(QCD, part);
    return part;
}

Particle colorSextet(Model &QCD, std::string const &name)
{
    Particle part = diracfermion_s(name, QCD);
    SetMass(part, "m_" + name);
    SetGroupRep(part, "C", {2, 0});
    AddParticle(QCD, part);
    return part;
}

int main()
{

    Model QCD;
    AddGaugedGroup(QCD, group::Type::SU, "C", 3, constant_s("g"));
    Init(QCD);

    Particle u = colorTriplet(QCD, "u");
    Particle d = colorTriplet(QCD, "d");
    Particle X = colorSextet(QCD, "X");
    Rename(QCD, "A_C", "G");
    Particle gluon = GetParticle(QCD, "G");

    std::cout << QCD << std::endl;
    mty::option::searchAbreviations = false;
    auto rules                      = ComputeFeynmanRules(QCD);
    csl::option::printIndexIds      = false;
    Display(rules);
    Show(rules);

    QCD.saveModelFile("QCD", "pmssm.h");

    auto diff = QCD.computeAmplitude(Order::TreeLevel,
                                     {Incoming(u),
                                      Incoming(AntiPart(u)),
                                      Outgoing(d),
                                      Outgoing(AntiPart(d))});
    Display(diff);
    Show(diff);

    auto _66bar = QCD.computeAmplitude(Order::TreeLevel,
                                       {Incoming(X),
                                        Incoming(AntiPart(X)),
                                        Outgoing(u),
                                        Outgoing(AntiPart(u))});
    Display(_66bar);
    Show(_66bar);

    csl::Expr crossSection = QCD.computeSquaredAmplitude(_66bar);

    std::cout << "Squared amplitude = " << crossSection << std::endl;
    csl::Expr m_X  = X->getMass();
    csl::Expr m_u  = u->getMass();
    csl::Expr s_12 = GetSymbol("s_12", crossSection);
    csl::Expr s    = 2 * (m_X * m_X + s_12);
    Replace(crossSection, m_X * m_X + s_12, s / 2);
    crossSection
        = crossSection / (64 * CSL_PI * CSL_PI * s)
          * sqrt_s((1 - 4 * pow_s(m_u, 2) / s) / (1 - 4 * pow_s(m_X, 2) / s));

    std::cout << "Cross Section = " << crossSection << std::endl;

    return 0;
}
