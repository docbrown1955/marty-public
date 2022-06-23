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

 #include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

Expr cc(Expr const &expr) { return GetComplexConjugate(expr); }

void calculateAndGenerateLib(
        std::string            const &,//initProcessName,
        mty::Model                        &model,
        std::vector<Insertion> const &insertions,
        mty::Library                      &,//lib,
        bool                          show = false,
        bool                          loop = false
        )
{
    const auto ampl  = (loop) ? 
        model.computeAmplitude(Order::OneLoop, insertions) 
        : model.computeAmplitude(Order::TreeLevel, insertions);
    
    if (show) {
        Display(ampl);
        // if (!ampl.empty())
        //     Show(ampl);
    }
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    mty::option::displayAbbreviations = false;
    ///////////////////////////////////////////////////
    // Gauge definition
    ///////////////////////////////////////////////////

    Model TIDM;
    Expr gs = constant_s("g_s");
    Expr gL = constant_s("g_L");
    Expr gY = constant_s("g_Y");
    Expr gD = constant_s("g_D");
    TIDM.addGaugedGroup(group::Type::SU, "C", 3, gs);
    TIDM.addGaugedGroup(group::Type::SU, "L", 2, gL);
    TIDM.addGaugedGroup(group::Type::U1, "Y", gY);
    TIDM.addGaugedGroup(group::Type::U1, "D", gD);
    TIDM.addFlavorGroup("F", 3);
    TIDM.init();

    TIDM.renameParticle("A_C", "G");

    ///////////////////////////////////////////////////
    // Particle content
    ///////////////////////////////////////////////////

    Particle QL = weylfermion_s("Q_L", TIDM, Chirality::Left);
    Particle LL = weylfermion_s("L_L", TIDM, Chirality::Left);
    Particle UR = weylfermion_s("U_R", TIDM, Chirality::Right);
    Particle DR = weylfermion_s("D_R", TIDM, Chirality::Right);
    Particle ER = weylfermion_s("E_R", TIDM, Chirality::Right);
    Particle XL  = weylfermion_s("XL", TIDM, Chirality::Left);
    Particle XR = weylfermion_s("XR", TIDM, Chirality::Left);

    Particle H    = scalarboson_s("H",    TIDM);
    Particle etaA = scalarboson_s("etaA", TIDM);
    Particle etaB = scalarboson_s("etaB", TIDM);
    Particle phi  = scalarboson_s("phi",  TIDM);

    ///////////////////////////////////////////////////
    // Irreducible representations
    ///////////////////////////////////////////////////

    QL->setFundamentalFlavorRep("F");
    QL->setGroupRep("C", {1, 0});
    QL->setGroupRep("L", {1});
    QL->setGroupRep("Y", {1, 6});

    LL->setFundamentalFlavorRep("F");
    LL->setGroupRep("L", {1});
    LL->setGroupRep("Y", {-1, 2});

    UR->setFundamentalFlavorRep("F");
    UR->setGroupRep("C", {1, 0});
    UR->setGroupRep("Y", {2, 3});

    DR->setFundamentalFlavorRep("F");
    DR->setGroupRep("C", {1, 0});
    DR->setGroupRep("Y", {-1, 3});

    ER->setFundamentalFlavorRep("F");
    ER->setGroupRep("Y", {-1});

    XL ->setGroupRep("D", -1);
    XR->setGroupRep("D", 1);

    H->setGroupRep("L", {1});
    H->setGroupRep("Y", {1, 2});

    etaA->setGroupRep("L", {1});
    etaA->setGroupRep("Y", {-1, 2});
    etaA->setGroupRep("D", {-1});

    etaB->setGroupRep("L", {1});
    etaB->setGroupRep("Y", {-1, 2});
    etaB->setGroupRep("D", {-1});

    phi->setGroupRep("D", {2});

    TIDM.addParticles({QL, LL, UR, DR, ER, XL, XR, H, etaA, etaB, phi});

    ///////////////////////////////////////////////////
    // Constant definition for the scalar potential
    ///////////////////////////////////////////////////

    Expr m2H    = constant_s("m2_H");
    Expr m2A    = constant_s("m2_eA");
    Expr m2B    = constant_s("m2_eB");
    Expr m2AB   = constant_s("m2_eAB");
    Expr mp    = constant_s("m2_phi");
    Expr lH    = constant_s("l_H");
    Expr lA    = constant_s("l_A");
    Expr lB    = constant_s("l_B");
    Expr lAB1  = constant_s("l_AB1");
    Expr lAB2  = constant_s("l_AB2");
    Expr lAB3  = constant_s("l_AB3");
    Expr lAB4  = constant_s("l_AB4");
    Expr lHA1  = constant_s("l_HAA1");
    Expr lHAB1 = constant_s("l_HAB1");
    Expr lHB1  = constant_s("l_HBB1");
    Expr lHA2  = constant_s("l_HAA2");
    Expr lHAB2 = constant_s("l_HAB2");
    Expr lHB3  = constant_s("l_HBB3");
    Expr lHA3  = constant_s("l_HAA3");
    Expr lHAB3 = constant_s("l_HAB3");
    Expr lHB2  = constant_s("l_HBB2");
    Expr lHp   = constant_s("l_Hphi");
    Expr lp    = constant_s("l_phi");

    ///////////////////////////////////////////////////
    // Indices and Tensors
    ///////////////////////////////////////////////////

    auto flavorSpace = GetVectorSpace(TIDM, "F", "Q_L");
    Tensor eps         = GetEpsilon(GetVectorSpace(TIDM, "L", "Q_L"));
    Tensor delta_flav  = GetDelta(flavorSpace);
    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();
    Index a  = DiracIndex();
    Index b  = DiracIndex();
    Index I  = FlavorIndex(TIDM, "F");
    Index J  = FlavorIndex(TIDM, "F");
    Index K  = FlavorIndex(TIDM, "F");
    Index L  = FlavorIndex(TIDM, "F");
    Index A  = GaugeIndex(TIDM, "C", "Q_L");
    Index i  = GaugeIndex(TIDM, "L", "Q_L");
    Index j  = GaugeIndex(TIDM, "L", "Q_L");

    ///////////////////////////////////////////////////
    // Scalar potential
    ///////////////////////////////////////////////////

    TIDM.addLagrangianTerm(m2H * cc(H(i)) * H(i)); // Sign for Mexican hat potential
    TIDM.addLagrangianTerm(-m2A  * cc(etaA(i)) * etaA(i));
    TIDM.addLagrangianTerm(-m2B  * cc(etaB(i)) * etaB(i));
    TIDM.addLagrangianTerm(-m2AB * cc(etaA(i)) * etaB(i), true);
    TIDM.addLagrangianTerm(mp * cc(phi) * phi); // Sign for Mexican hat potential

    TIDM.addLagrangianTerm(-lH * pow_s(cc(H(i)) * H(i), 2));
    TIDM.addLagrangianTerm(-lp * pow_s(cc(phi) * phi, 2));
    TIDM.addLagrangianTerm(-lHp * cc(H(i)) * H(i) * cc(phi) * phi);

    TIDM.addLagrangianTerm(-lA * pow_s(cc(etaA(i)) * etaA(i), 2));
    TIDM.addLagrangianTerm(-lB * pow_s(cc(etaB(i)) * etaB(i), 2));
    TIDM.addLagrangianTerm(-lAB1 * cc(etaA(i)) * etaA(i) * cc(etaB(j)) * etaB(j));
    TIDM.addLagrangianTerm(
            -lAB2 * cc(etaA(i)) * etaB(i) * cc(etaA(j)) * etaB(j),
            true
            );
    TIDM.addLagrangianTerm(
            -lAB3 * cc(etaA(i)) * etaA(i) * cc(etaA(j)) * etaB(j),
            true
            );
    TIDM.addLagrangianTerm(
            -lAB4 * cc(etaB(i)) * etaB(i) * cc(etaB(j)) * etaA(j),
            true
            );

    TIDM.addLagrangianTerm(-lHA1 * cc(H(i)) * H(i) * cc(etaA(j)) * etaA(j));
    TIDM.addLagrangianTerm(-lHB1 * cc(H(i)) * H(i) * cc(etaB(j)) * etaB(j));
    TIDM.addLagrangianTerm(-lHAB1 * cc(H(i)) * H(i) * cc(etaA(j)) * etaB(j), true);

    TIDM.addLagrangianTerm(-lHA3 * cc(H(i)) * etaA(i) * cc(etaA(j)) * H(j), true);
    TIDM.addLagrangianTerm(-lHB3 * cc(H(i)) * etaB(i) * cc(etaB(j)) * H(j), true);
    TIDM.addLagrangianTerm(-lHAB3 * cc(H(i)) * etaA(i) * cc(etaB(j)) * H(j), true);

    ///////////////////////////////////////////////////
    // Majorana masses for X1 and X2
    ///////////////////////////////////////////////////

    Expr mD = constant_s("m_D");
    Expr YX  = constant_s("Y_X",  csl::ComplexProperty::Complex);
    Expr YXc = constant_s("Y_Xc", csl::ComplexProperty::Complex);
    Tensor C = DiracCMatrix();

    TIDM.addLagrangianTerm(-mD                       * XR(a)* C({a, b}) * XL(b), true);
    TIDM.addLagrangianTerm(-CSL_HALF * YX  * phi * XL(a)  * C({a, b}) * XL(b),  true);
    TIDM.addLagrangianTerm(-CSL_HALF * YXc * cc(phi) * XR(a) * C({a, b}) * XR(b), true);

    ///////////////////////////////////////////////////
    // SM-like Yukawa definition
    ///////////////////////////////////////////////////

    Tensor Ye = Tensor("Y_e", {flavorSpace, flavorSpace});
    Tensor Yu = Tensor("Y_u", {flavorSpace, flavorSpace});
    Tensor Yd = Tensor("Y_d", {flavorSpace, flavorSpace});
    Ye->setComplexProperty(ComplexProperty::Complex);
    Yu->setComplexProperty(ComplexProperty::Complex);
    Yd->setComplexProperty(ComplexProperty::Complex);
    TIDM.addTensorCoupling(Ye);
    TIDM.addTensorCoupling(Yu);
    TIDM.addTensorCoupling(Yd);

    TIDM.addLagrangianTerm(
            -cc(QL({I, A, i, a}))
            * Yu({I, J})
            * UR({J, A, a})
            * eps({i, j})
            * cc(H(j)),
            true // Add also the complex conjugate of this term
            );
    TIDM.addLagrangianTerm(
            -cc(QL({I, A, i, a}))
            * Yd({I, J})
            * DR({J, A, a})
            * H(i),
            true // Add also the complex conjugate of this term
            );
    TIDM.addLagrangianTerm(
            -cc(LL({I, i, a}))
            * Ye({I, J})
            * ER({J, a})
            * H(i),
            true // Add also the complex conjugate of this term
            );

    ///////////////////////////////////////////////////
    // Fermion number violating interactions
    ///////////////////////////////////////////////////

    // Eq. 4 (first part) in 1208.3162
    Tensor YX1A("Y_X1A", flavorSpace);
    Tensor YX1B("Y_X1B", flavorSpace);
    YX1A->setComplexProperty(ComplexProperty::Complex);
    YX1B->setComplexProperty(ComplexProperty::Complex);
    
    TIDM.addLagrangianTerm(
            -YX1A(I) * XL(a) * C({a,b}) * LL({I, i, b}) * cc(etaA(i)),
            true
            );
    TIDM.addLagrangianTerm(
            -YX1B(I) * XL(a) * C({a,b}) * LL({I, i, b}) * cc(etaB(i)),
            true
            );
    
    ///////////////////////////////////////////////////
    // Gauge symmetry breaking
    ///////////////////////////////////////////////////

    TIDM.breakGaugeSymmetry("D");
    TIDM.breakGaugeSymmetry("Y");
    TIDM.breakGaugeSymmetry(
            "L",
            {"Q_L", "L_L", "H", "etaA", "etaB", "A_L", "c_A_L"},
            {{"U_L", "D_L"},
            {"Nu_L", "E_L"},
            {"H_1", "H_2"},
            {"etaA0; \\eta ^0_{A}", "eta1; \\eta ^-_{A}"},
            {"etaB0; \\eta ^0_{B}", "eta2; \\eta ^-_{B}"},
            {"W_1", "W_2", "W_3"},
            {"c_W_1; c_{W1}", "c_W_2; c_{W2}", "c_W_3; c_{W3}"},
            });

    ///////////////////////////////////////////////////
    // Yukawa diagonalization (CKM for quarks)
    ///////////////////////////////////////////////////

    Expr vH = constant_s("v_H");
    Tensor V_CKM = buildCKM(flavorSpace);
    TIDM.diagonalizeYukawa(
            "Y_e",                    // diagonalizing Y_e
            {"m_e", "m_mu", "m_tau"}, // Masses on the diagonal
            sqrt_s(2) / vH            // Global factor f in Y --> f*M
            );
    TIDM.diagonalizeYukawa(           // Same for Y_u
            "Y_u",
            {"m_u", "m_c", "m_t"},
            sqrt_s(2) / vH
            );
    Particle DL = TIDM.getParticle("D_L");
    TIDM.diagonalizeYukawa(
            "Y_d",
            {"m_d", "m_s", "m_b"},
            sqrt_s(2) / vH,
            V_CKM,                   // Apply CKM matrix while diagonalizing
            {DL, DR}                 // Apply it on D_L && D_R
            );

    ///////////////////////////////////////////////////
    // Flavor symmetry breaking
    ///////////////////////////////////////////////////

    BreakFlavorSymmetry(
            TIDM,
            "F",
            {"U_L", "U_R", "D_L", "D_R", "E_L", "E_R", "Nu_L"},
            {{"u_L", "c_L", "t_L"},
            {"u_R", "c_R", "t_R"},
            {"d_L", "s_L", "b_L"},
            {"d_R", "s_R", "b_R"},
            {"e_L", "mu_L; \\mu_L", "tau_L; \\tau_L"},
            {"e_R", "mu_R; \\mu_R", "tau_R; \\tau_R"},
            {"nu_e_L; \\nu_{e_L}", "nu_mu_L; \\nu_{\\mu_L}", "nu_tau_L; \\nu_{\\tau_L}"}}
            );

    ///////////////////////////////////////////////////
    // Expansion around VEVs
    ///////////////////////////////////////////////////

    // v_H for Higgs doublet VEV is defined with Yukawa diagonalization
    Expr vph = constant_s("vphi");
    Particle H1 = TIDM.getParticle("H_1");
    Particle H2 = TIDM.getParticle("H_2");
    Particle phiR = phi->generateSimilar("phiR ; \\phi_R");
    Particle phiI = phi->generateSimilar("phiI");
    Particle Hp = H1->generateSimilar("Hp");
    Particle Ah = H2->generateSimilar("Ah");
    Particle hh = H2->generateSimilar("hh ; h^0");
    phiR->setSelfConjugate(true);
    phiI->setSelfConjugate(true);
    Ah->setSelfConjugate(true);
    hh->setSelfConjugate(true);
    
    TIDM.replace(phi, (vph + phiR + CSL_I*phiI) / sqrt_s(2));
    TIDM.replace(H1 , Hp);
    TIDM.replace(H2 , (vH + hh + CSL_I*Ah) / sqrt_s(2));

    ///////////////////////////////////////////////////
    // W_1/2 -> W^+-
    ///////////////////////////////////////////////////

    Particle W1   = TIDM.getParticle("W_1");
    Particle W2   = TIDM.getParticle("W_2");
    Particle W = W1->generateSimilar("W");
    W->setSelfConjugate(false);

    Expr W_p = W(+mu);
    Expr W_m = cc(W(+mu));
    Expr F_W_p = W({+mu,+nu});
    Expr F_W_m = cc(W({+mu, +nu}));

    TIDM.replace(W1,         (W_p + W_m) / sqrt_s(2));
    TIDM.replace(W2, CSL_I * (W_p - W_m) / sqrt_s(2));
    TIDM.replace(GetFieldStrength(W1),         (F_W_p + F_W_m) / sqrt_s(2));
    TIDM.replace(GetFieldStrength(W2), CSL_I * (F_W_p - F_W_m) / sqrt_s(2));

    csl::Expr eem   = constant_s("e_em");
    csl::Expr thetaW = constant_s("theta_W");
    TIDM.replace(
            gL*gL + gY*gY,
            csl::pow_s(gL/csl::cos_s(thetaW), csl::int_s(2))
            );
    TIDM.replace(gY, eem / csl::cos_s(thetaW));
    TIDM.replace(gL, eem / csl::sin_s(thetaW));

    ///////////////////////////////////////////////////
    // 2x2 Mass matrices diagonalization
    ///////////////////////////////////////////////////

    TIDM.renameParticle("W_3", "Z");
    TIDM.renameParticle("A_D", "ZD");
    TIDM.renameParticle("A_Y", "A");
    //TIDM.diagonalizeMassMatrices();

    TIDM.rotateFields({"A", "Z", "ZD"}, true, 1);
    /*
    TIDM.renameParticle("W_3", "Z");
    TIDM.renameParticle("A_D", "ZD");
    TIDM.renameParticle("A_Y", "A");*/

    
    TIDM.rotateFields({"eta1", "eta2"}, true); // diagonalize
    TIDM.rotateFields({"etaA0", "etaB0"}, true); // diagonalize

    //TIDM.replace(TIDM.getParticle("X_L"), 1/sqrt_s(2)*XX1(al[0]));
    //TIDM.replace(TIDM.getParticle("X_R"), 1/sqrt_s(2)*cc(XX2(al[0])));
    // std::cout << TIDM << std::endl;
    // std::cin.get();

    TIDM.rotateFields({"XL", "XR"}, true); // diagonalize
    Particle xi1 = diracfermion_s("xi_1 ; \\xi_1", TIDM);
    Particle xi2 = diracfermion_s("xi_2 ; \\xi_2", TIDM);
    xi1->setSelfConjugate(true);
    xi2->setSelfConjugate(true);
    xi1->setMass(XL->getMass());
    xi2->setMass(XR->getMass());
    TIDM.replace(TIDM.getParticle("XL"),  xi1(a) / sqrt_s(2));
    TIDM.replace(TIDM.getParticle("XR"),  xi2(a) / sqrt_s(2));
    
    //TIDM.rotateFields({"xi_1", "xi_2"}, true); // diagonalize
    
    // Add widths
    TIDM.getParticle("xi_1")->setWidth(csl::constant_s("GX1"));
    TIDM.getParticle("xi_2")->setWidth(csl::constant_s("GX2"));
    
    TIDM.rotateFields({"hh", "phiR"}, true); // diagonalize

    TIDM.rotateFields({"Ah", "phiI"}, true); // diagonalize
    
    Rename(TIDM,"Ah",  "G01");
    Rename(TIDM,"phiI","G02");
    Rename(TIDM,"Hp",  "Gp");
    
    TIDM.promoteToGoldstone("Gp", "W");
    TIDM.promoteToGoldstone("G01", "Z");
    TIDM.promoteToGoldstone("G02", "ZD");
    /*
    TIDM.setGaugeChoice("W", gauge::Lorenz);
    TIDM.setGaugeChoice("Z", gauge::Lorenz);
    TIDM.setGaugeChoice("A", gauge::Lorenz);
    */
    ///////////////////////////////////////////////////
    // End model building, Feynman rules
    ///////////////////////////////////////////////////

    TIDM.refresh();
    //TIDM.printSubPart({"xi_1", "phiR"});

    auto const &rules = TIDM.getFeynmanRules();
    Display(rules);
    // Show(rules);
    std::cout << "Press enter to launch calculations ! :)" << std::endl;

    ///////////////////////////////////////////////////
    //  Getting particles groups for calculations
    ///////////////////////////////////////////////////

    auto leptons = TIDM.getPhysicalParticles([&](Particle p) {
        const auto name = p->getName();
        return (name == "e" || name == "mu" || name == "tau");
    });
    auto neutrinos = TIDM.getPhysicalParticles([&](Particle p) {
        const auto name = p->getName();
        return (name.find("nu_") != std::string::npos);
    });
    auto etai = TIDM.getPhysicalParticles([&](Particle p) {
        const auto name = p->getName();
        return (name.find("eta") != std::string::npos
                && name.find("0") == std::string::npos);
    });
    auto eta0i = TIDM.getPhysicalParticles([&](Particle p) {
        const auto name = p->getName();
        return (name.find("eta") != std::string::npos
                && name.find("0") != std::string::npos);
    });
    auto fermions = TIDM.getPhysicalParticles([&](Particle p) {
            return p->isFermionic();
    });
    auto vectors = TIDM.getPhysicalParticles([&](Particle p) {
        return p->getSpinDimension() == 3
            && p->getGaugedGroup() != TIDM.getGaugedGroup("C"); // no gluon
            // uncomment to remove U(1)_D gauge boson
            // && p->getGaugedGroup() != TIDM.getGaugedGroup("D");
    });

    ///////////////////////////////////////////////////
    // Library initialization
    ///////////////////////////////////////////////////

    mty::Library TIDMLib("LeptoGenesis", ".");
   
    mty::option::decomposeInOperators = false;
    mty::option::decomposeInLocalOperator = false;
    
    mty::option::excludeTadpoles = true;
    mty::option::excludeMassCorrections = true;
    mty::option::excludeTriangles = false;
    mty::option::excludeBoxes = true;
    
    // auto ins14 = {Incoming("mu"), Outgoing("e"), Outgoing("A")};
    auto ins10 = {Incoming("mu"), Outgoing("e"), Outgoing("Z")};
    // calculateAndGenerateLib("LFV", TIDM, ins14, TIDMLib,true,true);
    calculateAndGenerateLib("LFV", TIDM, ins10, TIDMLib,true,true);

    ///////////////////////////////////////////////////
    // Library generation
    ///////////////////////////////////////////////////

    TIDMLib.addIPath("/usr/local/Cellar/boost/1.75.0_1/include");
    TIDMLib.addIPath("/usr/local/Cellar/eigen/3.3.8_1/include");
    TIDMLib.addLPath("/usr/local/Cellar/boost/1.75.0_1/lib");
    TIDMLib.addLPath("/usr/local/lib");
    TIDMLib.addIPath("/usr/local/include");
    
    TIDMLib.generateSpectrum(TIDM);
    TIDMLib.cleanExistingSources();
    TIDMLib.print();

    return system("cd LeptoGenesis; make -j 4 >/dev/null");
}
