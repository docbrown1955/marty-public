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

 #include <marty>
#include "pmssm_lem.h"

auto cc (csl::Expr e) { return csl::GetComplexConjugate(e); }
auto hc (csl::Expr e) { return csl::GetHermitianConjugate(e, &mty::dirac4); }

void print(csl::Expr e)
{
    std::cout << "EXPR : " << e << '\n';
    std::cout << "cc   : " << cc(e) << '\n';
    std::cout << "hc   : " << hc(e) << '\n';
}

using namespace mty;
void square(Model & model, std::vector<Insertion> const &ins)
{
    mty::option::displayAbbreviations = false;
    auto ampl = model.computeAmplitude(TreeLevel, ins);
    //for (auto &diag : ampl.getDiagrams())
    //    csl::Evaluate(diag.getExpression(), csl::eval::abbreviation);
    std::cout << "FINAL AMPLITUDE : " << '\n';
    Display(ampl);
    //Show(ampl);
    // auto square = model.computeSquaredAmplitude(ampl);
    // std::cout << csl::Evaluated(square, csl::eval::abbreviation) << '\n';
    DisplayAbbreviations("EXT");
}

int main() {

    // PMSSM_LEM pmssm;
    // std::cout << pmssm << '\n';
    // auto amplmssm = pmssm.computeAmplitude(TreeLevel, {Incoming("C_1"), Incoming(AntiPart("N_1")), Outgoing("W")});
    // for (auto &diag : amplmssm.getDiagrams())
    //     csl::Evaluate(diag.getExpression(), csl::eval::abbreviation);
    // Display(amplmssm);
    // Show(amplmssm);
    // return 0;

    using namespace csl;
    Model model;
    model.init();

    auto N1 = diracfermion_s("N1", model);
    auto N2 = diracfermion_s("N2", model);
    auto N3 = diracfermion_s("N3", model);
    N1->setSelfConjugate(true);
    N2->setSelfConjugate(true);
    auto C = diracfermion_s("C", model);
    auto CL = C->getWeylFermion(Chirality::Left);
    auto CR = C->getWeylFermion(Chirality::Right);

    auto W = vectorboson_s("W", model);
    //W->setSelfConjugate(false);

    model.addParticles({N1, N2, N3, C, W});
    auto gamma = dirac4.gamma;
    auto Cm = dirac4.C_matrix;
    auto a = DiracIndices(10);
    auto mu = MinkowskiIndex();
    auto aL = constant_s("a_L");
    auto aR = constant_s("a_R");

    model.addLagrangianTerm(
            aL * W(mu) * cc(CR(a[0])) * gamma({+mu, a[0], a[1]}) * N2(a[1]),
            true
            );
    model.addLagrangianTerm(
            aR * W(mu) * gamma({+mu, a[1], a[2]}) * cc(N2(a[1]))*Cm({a[0],a[2]}) * cc(CL(a[0])),
            true
            );
    //model.addLagrangianTerm(
    //        -aR * W(mu) * gamma({+mu, a[1], a[2]}) * cc(N2(a[1]))*Cm({a[2],a[0]}) * cc(CL(a[0])),
    //        true
    //        );
    //model.addLagrangianTerm(
    //        aR * W(mu) *CR(a[0])* N2(a[2])*gamma({+mu,a[1],a[2]})*Cm({a[1],a[0]}),
    //        true
    //        );

    model.refresh();
    std::cout << model << '\n';
    Display(model.getFeynmanRules());

    auto ampl2 = model.computeAmplitude(TreeLevel, {Incoming("C"), Incoming("N2"), Outgoing("W")});
    Display(ampl2);
    //auto squared = model.computeSquaredAmplitude(ampl);
    //std::cout << squared << '\n';
    //std::cout << csl::Evaluated(squared, csl::eval::abbreviation) << '\n';

    return 0;

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between 
    // 1 and 4 (inclusive)
    // Type 2 is the MSSM-like 2HDM
    ///////////////////////////////////////////////////

    constexpr int type = 2;

    mty::TwoHDM_Model<type> THDM;
    std::cout << THDM << std::endl;
    
    return 0;
}
