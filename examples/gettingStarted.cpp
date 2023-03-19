// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

/*
 * This is the getting started example for MARTY!
 *
 * Follow the tutorial on the website: https://marty.in2p3.fr/gettingStarted.html
 */

#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

// To be changed by the user if needed !
std::string path_to_generated_library = ".";

int main()
{
    // Model building
    Model toyModel;
    toyModel.addGaugedGroup(group::Type::SU, "L", 2);
    toyModel.addGaugedGroup(group::Type::U1, "Y");
    toyModel.init();

    toyModel.renameParticle("A_L", "W");
    toyModel.renameParticle("A_Y", "B");

    Particle Q_L = weylfermion_s("Q_L", toyModel, Chirality::Left);
    Particle u_R = weylfermion_s("u_R", toyModel, Chirality::Right);
    Particle d_R = weylfermion_s("d_R", toyModel, Chirality::Right);

    Q_L->setGroupRep("L", 1);
    Q_L->setGroupRep("Y", {1, 6});
    u_R->setGroupRep("Y", {2, 3});
    d_R->setGroupRep("Y", {-1, 3});

    toyModel.addParticle(Q_L);
    toyModel.addParticle(u_R);
    toyModel.addParticle(d_R);

    toyModel.breakGaugeSymmetry("Y");
    toyModel.breakGaugeSymmetry("L");

    toyModel.renameParticle("Q_L_1", "u_L");
    toyModel.renameParticle("Q_L_2", "d_L");

    cout << toyModel << endl;

    // Higgs mechanism simulated
    Particle W_1   = toyModel.getParticle("W_1");
    Particle W_2   = toyModel.getParticle("W_2");
    Particle F_W_1 = W_1->getFieldStrength();
    Particle F_W_2 = W_2->getFieldStrength();
    Particle W     = W_1->generateSimilar("W");
    W->setSelfConjugate(false);
    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();
    // W_1 goes to (W+ + W-) / sqrt(2)
    toyModel.replace(W_1, (W(mu) + GetComplexConjugate(W(mu))) / sqrt_s(2));
    toyModel.replace(
        F_W_1, (W({mu, nu}) + GetComplexConjugate(W({mu, nu}))) / sqrt_s(2));
    // W_2 goes to i*(W+ - W-) / sqrt(2)
    toyModel.replace(
        W_2, CSL_I * (W({mu}) - GetComplexConjugate(W({mu}))) / sqrt_s(2));
    toyModel.replace(F_W_2,
                     CSL_I * (W({mu, nu}) - GetComplexConjugate(W({mu, nu})))
                         / sqrt_s(2));

    csl::Expr thetaW = constant_s("thetaW");
    csl::Expr cW     = cos_s(thetaW);
    csl::Expr sW     = sin_s(thetaW);
    // We give the rotation matrix explicitly here, between double curly braces
    toyModel.rotateFields({"W_3", "B"}, {"Z", "A"}, {{cW, sW}, {-sW, cW}});
    csl::Expr e  = constant_s("e");
    csl::Expr gY = toyModel.getScalarCoupling("g_Y");
    csl::Expr gL = toyModel.getScalarCoupling("g_L");
    toyModel.replace(gY, e / cW);
    toyModel.replace(gL, e / sW);

    csl::Expr M_W = constant_s("M_W");
    csl::Expr m_u = constant_s("m_u");
    csl::Expr m_d = constant_s("m_d");

    toyModel.addBosonicMass("W", M_W);
    toyModel.addFermionicMass("u_L", "u_R", m_u);
    toyModel.addFermionicMass("d_L", "d_R", m_d);

    toyModel.refresh();
    cout << toyModel << endl;

    auto rules = toyModel.getFeynmanRules();
    Display(rules); // Displays expressions in terminal
    Show(rules); // Shows diagrams in the application

    auto res = toyModel.computeAmplitude(Order::TreeLevel,
                                         {Incoming("u"),
                                          Incoming(AntiPart("u")),
                                          Outgoing("d"),
                                          Outgoing(AntiPart("d"))});

    Display(res);
    DisplayAbbreviations();
    Show(res);

    csl::Expr squared_ampl = toyModel.computeSquaredAmplitude(res);
    cout << squared_ampl << endl;

    mty::Library myLib("toy", path_to_generated_library);
    myLib.addFunction("squared_ampl", squared_ampl, "G");
    myLib.build();

    return 0;
}
