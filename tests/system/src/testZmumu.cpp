/*
 * This program computes the e+e- -> mu+mu- squared amplitude, at the tree-level,
 * in a custom model with a photon-like boson and a Z-like boson.
 */
#include <marty.h>
#include "marty/models/sm.h"

using namespace csl;
using namespace mty;

// Returns the complex conjugate
auto cc(csl::Expr const &expr) { return GetComplexConjugate(expr); }

int main() {

    // Disabling output, comment for enabling it again
    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    // Removing the existing Zmumu library
    [[maybe_unused]] int sysres = system("rm -r Zmumu/* >/dev/null");

    // Building a U(1) gauge to have a photon-like interaction
    Model model;
    model.addGaugedGroup(group::Type::U1, "Y");
    model.init();

    // Setting up the particle content, with e, mu and the Z boson
    Particle el = diracfermion_s("e", model);
    Particle mu = diracfermion_s("mu ; \\mu", model);
    Particle Z  = vectorboson_s ("Z", model);
    el->setGroupRep("Y", -1); // -1 charge
    mu->setGroupRep("Y", -1); // -1 charge
    Z->setMass(constant_s("M_Z"));
    Z->setWidth(constant_s("G_Z"));
    Z->setSelfConjugate(true);
    model.addParticles({el, mu, Z});

    // In this part we build the P_L / P_R interations of the leptons with Z,
    // and we first have to gather tensors and indices to build the interactions
    auto a = DiracIndices(3);
    auto m = MinkowskiIndices(2);
    Tensor gamma = dirac4.gamma;
    Tensor P_L = dirac4.P_L;
    Tensor P_R = dirac4.P_R;
    Expr aL = constant_s("aL"); // Coupling to left-handed
    Expr aR = constant_s("aR"); // Coupling to right-handed

    for (auto f : {el, mu}) {
        model.addLagrangianTerm(
                aL * gamma({m[0], a[0], a[1]}) * P_L({a[1], a[2]})
                * cc(f(a[0])) * Z(+m[0]) * f(a[2])
                );
        model.addLagrangianTerm(
                aR * gamma({m[0], a[0], a[1]}) * P_R({a[1], a[2]})
                * cc(f(a[0])) * Z(+m[0]) * f(a[2])
                );
    }

    model.refresh();
    std::cout << model << '\n';

    mty::option::displayAbbreviations = false;

    Display(model.getFeynmanRules());

    Library lib("Zmumu"); // Creating the library (in the program for now)

    Amplitude resDD = model.computeAmplitude(
            TreeLevel,
            {Incoming("e"), Incoming(AntiPart("e")), 
            Outgoing("mu"), Outgoing(AntiPart("mu"))}
            );
    Display(resDD);
    Expr squaredDirac  = model.computeSquaredAmplitude(resDD);
    std::cout << squaredDirac << '\n';
    std::cout << csl::Evaluated(squaredDirac, csl::eval::abbreviation) << '\n';
    lib.addFunction("Zmumu_full", squaredDirac); // Adding the squared ampl
                                                 // to the library

    auto chiralities = {Left, Right};
    for (auto elChirality : chiralities) { // Calculating the same process
                                           // with chiral incoming electrons
        Amplitude resChiral = model.computeAmplitude(
                TreeLevel,
                {Incoming(elChirality("e")), Incoming(AntiPart(elChirality("e"))),
                Outgoing("mu"), Outgoing(AntiPart("mu"))}
                );
        Expr squaredChiral = model.computeSquaredAmplitude(resChiral);
        Display(resChiral);
        std::cout << squaredChiral << '\n';
        std::string name = "Zmumu_";
        name += (elChirality == Left) ? 'L' : 'R';
        lib.addFunction(name, squaredChiral);
    }
    lib.print();

   sysres = system("cp libscripts/example_zmumu.cpp libscripts/kinematics.h Zmumu/script");
   sysres = system("cd Zmumu; make >/dev/null");

   // Total cross-section calculation
   sysres = system("cd Zmumu; bin/example_zmumu.x 0 >/dev/null");
   int res = system("bin/comparedata.x data/ee_mumu_tot.txt Zmumu/data.txt 1e-5");

   // Forward-Backward asymetry
   sysres = system("cd Zmumu; bin/example_zmumu.x 1 >/dev/null");
   res |= system("bin/comparedata.x data/ee_mumu_AFB.txt Zmumu/data.txt 1e-5");

   // Left- and right-handed contributions
   sysres = system("cd Zmumu; bin/example_zmumu.x 2 >/dev/null");
   res |= system("bin/comparedata.x data/ee_mumu_LR.txt Zmumu/data.txt 1e-5");

   return res;
}
