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

// To change by the user !
std::string path_to_generated_library = ".";

csl::Expr cc(csl::Expr const &init) {
    return csl::GetComplexConjugate(init);
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    // Model building
    
    // Dirac space
    Index a = DiracIndex();
    Index b = DiracIndex();
    
    Model toyModel;
    toyModel.addGaugedGroup(group::Type::SU, "L", 2);
    toyModel.addGaugedGroup(group::Type::U1, "Y");
    toyModel.addFlavorGroup("SM_flavor", 3,true);
    toyModel.init();
    
    toyModel.renameParticle("A_L", "W");
    toyModel.renameParticle("A_Y", "B");
    
    Particle H = scalarboson_s("H", toyModel);
    H->setGroupRep("L", {1});
    H->setGroupRep("Y", {-1, 2});
    
    
    Particle et = scalarboson_s("et", toyModel);
    et->setGroupRep("L", {1});
    et->setGroupRep("Y", {-1, 2});
    
    Particle Del = scalarboson_s("Del", toyModel);
    Del->setGroupRep("L", {2});
    Del->setGroupRep("Y", {-1, 1});
    
    Particle Li = weylfermion_s("L_L", toyModel, Chirality::Left);
    Li->setGroupRep("L", {1});
    Li->setGroupRep("Y", {-1, 2});
    Li->setFundamentalFlavorRep("SM_flavor");
    
    
    Particle Ei = weylfermion_s("E_R", toyModel, Chirality::Right);
    Ei->setGroupRep("Y", {-1, 1});
    Ei->setFundamentalFlavorRep("SM_flavor");
    
    Particle Ni = weylfermion_s("N", toyModel, Chirality::Right);
    Ni->setGroupRep("Y", {0});
    Ni->setFundamentalFlavorRep("SM_flavor");
    Ni->setSelfConjugate(true);
    
    toyModel.addParticle(H);
    toyModel.addParticle(Del);
    toyModel.addParticle(et);
    toyModel.addParticle(Li);
    toyModel.addParticle(Ei);
    toyModel.addParticle(Ni);
    
    // SM flavor space
    Index I = toyModel.generateIndex("SM_flavor", "L_L");
    Index J = toyModel.generateIndex("SM_flavor", "L_L");
    
    // SU(2)L space
    Index i = toyModel.generateIndex("L", "L_L");
    Index j = toyModel.generateIndex("L", "L_L");
    Tensor eps = i.getSpace()->getEpsilon();
    
    //Tensor pauli = toyModel.getGenerator("L", "L_L");
    Tensor pauli = GetGenerator(toyModel,"L", "L_L");
    auto ii = toyModel.generateIndices(3, "L", "L_L");
    auto A = toyModel.generateIndex("L", "Del");
    auto B = toyModel.generateIndex("L", "Del");
    
    
    auto flavorSpace = toyModel.getVectorSpace("SM_flavor", "L_L");
    Tensor Ye = csl::Tensor("Y_e", {flavorSpace, flavorSpace});
    Ye->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor YN = csl::Tensor("Y_N", {flavorSpace, flavorSpace});
    YN->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor Ynu = csl::Tensor("Y_{\\nu}", {flavorSpace, flavorSpace});
    Ynu->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MN = csl::Tensor("M_N", {flavorSpace, flavorSpace});
    
    csl::Expr muH2 = constant_s("muH2");
    csl::Expr muet2 = constant_s("muet2");
    csl::Expr muD2 = constant_s("muD2");
    csl::Expr muetD = constant_s("\\mu_{\\eta \\Delta}");
    csl::Expr muHD = constant_s("\\mu_{H \\Delta}");
    csl::Expr lh = constant_s("\\lambdaH");
    csl::Expr let = constant_s("\\lambda_{\\eta}");
    csl::Expr leth1 = constant_s("\\lambda_{\\eta H}");
    csl::Expr leth2 = constant_s("\\lambda^{\\prime}}_{\\eta H}");
    csl::Expr lD = constant_s("\\lambda_{\\Delta}");
    csl::Expr vh = constant_s("vh");
    csl::Expr vD = constant_s("vD");
    auto al = DiracIndices(2);

    Tensor C = dirac4.C_matrix;
    //Tensor C = dirac4.getDelta();
    toyModel.addLagrangianTerm(-H(i)*eps({i,j})*cc(Ei({I, a}))*Ye({I, J})*Li({J, j, a}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-Ni({I, al[0]})*Ynu({I, J})*cc(et(i))*C({al[0], al[1]})*Li({J, i, al[1]}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-Ni({I, al[0]})*MN({I, J})*C({al[0], al[1]})*Ni({J, al[1]}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-Li({I, ii[0], al[0]})*C({al[0], al[1]})*Ynu({I,J})*cc(Del(A)) * eps({ii[0],ii[1]})*pauli({A, ii[1], ii[2]}) * Li({J, ii[2], al[1]}),
                      true
                      );
    
    toyModel.addLagrangianTerm(muH2*cc(H(i))*H(i));
    csl::Expr prod = cc(H(i)) * H(i);
    
    toyModel.addLagrangianTerm(lh*prod*RenamedIndices(prod));
    
    toyModel.addLagrangianTerm(muet2*cc(et(i))*et(i));
    
    csl::Expr prod1 = cc(et(i))*et(i);
    toyModel.addLagrangianTerm(let*prod1*RenamedIndices(prod1));
    
    csl::Expr prod2 = cc(H(i))*et(i);
    toyModel.addLagrangianTerm(leth1*prod2*RenamedIndices(prod2),true);
    toyModel.addLagrangianTerm(leth2*cc(H(i))*H(i)*cc(et(j))*et(j));
    
    toyModel.addLagrangianTerm(muD2*Del(A)*pauli({A, ii[0], ii[1]})*cc(Del(B))*pauli({B, ii[1], ii[0]}));
    
    csl::Expr prod3 = Del(A)*pauli({A, ii[0], ii[1]})*cc(Del(B))*pauli({B, ii[1], ii[0]});
    toyModel.addLagrangianTerm(lD*prod3*RenamedIndices(prod3));
    
    toyModel.addLagrangianTerm(muetD*et(ii[0]) * cc(Del(A)) *eps({ii[0],ii[1]})* pauli({A, ii[1], ii[2]}) * et(ii[2]),true);
    
    toyModel.addLagrangianTerm(muHD*H(ii[0])* cc(Del(A)) *eps({ii[0],ii[1]})* pauli({A, ii[1], ii[2]}) * H(ii[2]),true);
    
    cout << toyModel << endl;
    
    // Model breaking

    csl::Expr m_e = constant_s("m_e");
    csl::Expr m_mu = constant_s("m_{\\mu}");
    csl::Expr m_tau = constant_s("m_{\\tau}");
    
    csl::Expr mN1 = constant_s("m_N1");
    csl::Expr mN2 = constant_s("m_N2");
    csl::Expr mN3 = constant_s("m_N3");
       
    csl::Tensor M_l = csl::tensor_s("M_l",{flavorSpace, flavorSpace},csl::matrix_s({{m_e, 0, 0},{0, m_mu, 0},{0, 0, m_tau}}));
    csl::Tensor M_N = csl::tensor_s("M_N",{flavorSpace, flavorSpace},csl::matrix_s({{mN1, 0, 0},{0, mN2, 0},{0, 0, mN3}}));
    Index f_i = GetIndex(flavorSpace);
    Index f_j = GetIndex(flavorSpace);
    toyModel.replace(Ye,sqrt_s(2)/(vh) * M_l({f_i, f_j}));
    toyModel.replace(MN,M_N({f_i, f_j}));
    
    toyModel.breakGaugeSymmetry("Y");
    toyModel.breakGaugeSymmetry("L");
    
    toyModel.renameParticle("L_L_1", "Nu_L");
    toyModel.renameParticle("L_L_2", "E_L");
    
    toyModel.renameParticle("H_1", "H0");
    toyModel.renameParticle("H_2", "H^-");
    
    toyModel.renameParticle("et_1", "et0");
    toyModel.renameParticle("et_2", "\\eta^-");
    
    toyModel.breakFlavorSymmetry("SM_flavor");
    toyModel.renameParticle("E_L_1","e_L");
    toyModel.renameParticle("E_L_2","\\mu_L");
    toyModel.renameParticle("E_L_3","\\tau_L");
    toyModel.renameParticle("E_R_1","e_R");
    toyModel.renameParticle("E_R_2","\\mu_R");
    toyModel.renameParticle("E_R_3","\\tau_R");

    toyModel.promoteToMajorana("N_1");
    toyModel.promoteToMajorana("N_2");
    toyModel.promoteToMajorana("N_3");
    /*toyModel.breakFlavorSymmetry("SM_flavor",
                                 {"E_L","E_R","Nu_L,","N"},
                                 {{"e_L", "\\mu_L", "\\tau_L"},
        {"e_R", "\\mu_R", "\\tau_R"},
        {"\\nu_{eL}", "\\nu_{\\mu L}", "\\nu_{\\tau L}"},
        {"N1","N2","N3"}});
    */
    
    // Higgs mechanism simulated
    Particle W_1 = toyModel.getParticle("W_1");
    Particle W_2 = toyModel.getParticle("W_2");
    Particle F_W_1 = W_1->getFieldStrength();
    Particle F_W_2 = W_2->getFieldStrength();
    Particle W = W_1->generateSimilar("W");
    W->setSelfConjugate(false);
    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();
    // W_1 goes to (W+ + W-) / sqrt(2)
    toyModel.replace(W_1, (W(mu) + GetComplexConjugate(W(mu))) / sqrt_s(2));
    toyModel.replace(F_W_1, (W({mu, nu}) + GetComplexConjugate(W({mu, nu}))) / sqrt_s(2));
    // W_2 goes to i*(W+ - W-) / sqrt(2)
    toyModel.replace(W_2, CSL_I * (W({mu}) - GetComplexConjugate(W({mu}))) / sqrt_s(2));
    toyModel.replace(F_W_2, CSL_I * (W({mu, nu}) - GetComplexConjugate(W({mu, nu}))) / sqrt_s(2));
    
    Particle Del_1 = toyModel.getParticle("Del_1");
    Particle Del_2 = toyModel.getParticle("Del_2");
    Particle Del_3 = toyModel.getParticle("Del_3");
    
    Particle Dmm = Del_1->generateSimilar("\\Delta^{--}");
    Dmm->setSelfConjugate(false);
    
    Particle Dm = Del_2->generateSimilar("\\Delta^-");
    Dm->setSelfConjugate(false);
    Particle D0 = Del_3->generateSimilar("D0");
    //D0->setSelfConjugate(false);
    Particle DR = D0->generateSimilar("\\Delta_R");
    DR->setSelfConjugate(true);
    Particle DI = D0->generateSimilar("\\Delta_I");
    DI->setSelfConjugate(true);
    
    
    toyModel.replace(Del_1, (Dmm + D0) / sqrt_s(2));
    toyModel.replace(Del_2, CSL_I * (D0 - Dmm) / sqrt_s(2));
    toyModel.replace(Del_3,Dm/sqrt_s(2));
    toyModel.replace(D0, (vD + DR + CSL_I*DI) / sqrt_s(2));

    
    Particle H0 = toyModel.getParticle("H0");
    Particle hh = H0->generateSimilar("hh");
    hh->setSelfConjugate(true);
    Particle Ah = H0->generateSimilar("Ah");
    Ah->setSelfConjugate(true);
    toyModel.replace(H0, (vh + hh + CSL_I*Ah) / sqrt_s(2));
    
    Particle et0 = toyModel.getParticle("et0");
    
    Particle etR = et0->generateSimilar("\\eta_R");
    etR->setSelfConjugate(true);
    Particle etI = et0->generateSimilar("\\eta_I");
    etI->setSelfConjugate(true);
    
    toyModel.replace(et0, (etR + CSL_I*etI)/ sqrt_s(2));
    
    toyModel.rotateFields({"\\eta_R","\\eta_I"},true);
    toyModel.rotateFields({"hh","\\Delta_R"},true);
    
    csl::Expr GDR = constant_s("GDR");
    toyModel.getParticle("\\Delta_R")->setWidth(GDR);
    
    toyModel.rotateFields({"Ah","\\Delta_I"},true);
    csl::Expr GDI = constant_s("GDI");
    toyModel.getParticle("\\Delta_I")->setWidth(GDI);
    toyModel.rotateFields({"H^-","\\Delta^-"},true);
    csl::Expr GDm = constant_s("GDm");
    toyModel.getParticle("\\Delta^-")->setWidth(GDm);

    toyModel.getParticle("Nu_L_1")->setSelfConjugate(true);
    toyModel.getParticle("Nu_L_2")->setSelfConjugate(true);
    toyModel.getParticle("Nu_L_3")->setSelfConjugate(true);
    toyModel.promoteToMajorana("Nu_L_1");
    toyModel.promoteToMajorana("Nu_L_2");
    toyModel.promoteToMajorana("Nu_L_3");

    toyModel.renameParticle("Nu_L_1", "nu_e ; \\nu _e");
    toyModel.renameParticle("Nu_L_2", "nu_mu ; \\nu _{\\mu}");
    toyModel.renameParticle("Nu_L_3", "nu_tau ; \\nu _{\\tau}");
    toyModel.rotateFields({"nu_e", "nu_mu", "nu_tau"}, true);

    
    //toyModel.rotateFields({"Dmm"},true);
    csl::Expr GDmm = constant_s("GDmm");
    toyModel.getParticle("\\Delta^{--}")->setWidth(GDmm);
    
    csl::Expr thetaW = constant_s("thetaW");
    csl::Expr cW = cos_s(thetaW);
    csl::Expr sW = sin_s(thetaW);
    // We give the rotation matrix explicitly here, between double curly braces
    toyModel.rotateFields({"W_3", "B"},{"Z"  , "A"},
                          {{cW , sW},
                          {-sW, cW}});
    toyModel.promoteToGoldstone("H^-", "W");
    toyModel.promoteToGoldstone("Ah", "Z");
    csl::Expr e = constant_s("e");
    csl::Expr gY = toyModel.getScalarCoupling("g_Y");
    csl::Expr gL = toyModel.getScalarCoupling("g_L");
    toyModel.replace(gY, e / cW);
    toyModel.replace(gL, e / sW);
    
    csl::Expr M_W = constant_s("M_W");
    csl::Expr M_Z = constant_s("M_Z");
    
    std::cout << toyModel << '\n';
    toyModel.refresh();
    std::cout << toyModel << std::endl;
    toyModel.printSubPart({"\\Delta^{--}", "e_L"});
    std::cout << "Press enter to launch Feynman rules" << std::endl;

    mty::option::displayAbbreviations = false;
    auto rules = toyModel.getFeynmanRules();
    for (const auto &r : rules)
        if (r.contains(toyModel.getParticle("\\Delta^{--}").get()) 
                && r.contains(toyModel.getParticle("e_L").get())) {
            std::cout << r << '\n';
        }
    // Display(rules); // Displays expressions in terminal
    //Show(rules); // Shows diagrams in the application
    
    mty::option::decomposeInOperators = true;
    mty::option::decomposeInLocalOperator = false;
    mty::option::excludeTadpoles = true;
    mty::option::excludeMassCorrections = true;
    // mty::option::excludeTriangles = true;
    // mty::option::excludeBoxes = true;
 
    std::cout << "Press enter to launch N1 --> e eta+ (tree-level)" << std::endl;
    auto res6=toyModel.computeAmplitude(Order::OneLoop,{Incoming("N_1"), Outgoing("e"), Outgoing(AntiPart("\\eta^-"))});
    Display(res6);
    // Show(res6);
    csl::Expr squared_ampl0 = toyModel.computeSquaredAmplitude(res6);
    cout << squared_ampl0 << endl;
    DisplayAbbreviations("EXT");

    std::cout << "Press enter to launch Z --> nu_e nu_e (tree-level)" << std::endl;
    auto res= toyModel.computeAmplitude(Order::OneLoop,{Incoming("Z"), Outgoing("nu_e"), Outgoing("nu_e")});
    Display(res);
    DisplayAbbreviations("EXT");
    // Show(res);
    
    std::cout << "Press enter to launch D-- --> e mu (tree-level)" << std::endl;
    auto res7 = toyModel.computeAmplitude(Order::OneLoop,{Incoming("\\Delta^{--}"), Outgoing("e"), Outgoing("\\mu")});
    Display(res7);
    DisplayAbbreviations("EXT");
    //Show(res7);

    std::cout << "Press enter to launch D-- --> eta- eta- (tree-level)" << std::endl;
    auto res8 = toyModel.computeAmplitude(Order::OneLoop,{Incoming("\\Delta^{--}"), Outgoing("\\eta^-"), Outgoing("\\eta^-")});
    Display(res8);
    DisplayAbbreviations("EXT");
    // Show(res8);

    std::cout << "Press enter to launch D-- --> e e (tree-level)" << std::endl;
    auto res1 = toyModel.computeAmplitude(Order::OneLoop,{Incoming("\\Delta^{--}"), Outgoing("e"), Outgoing("e")});
    Display(res1);
    DisplayAbbreviations("EXT");
    // Show(res1);

    std::cout << "Press enter to launch eta- eta- --> e e (tree-level)" << std::endl;
    auto res0 = toyModel.computeAmplitude(Order::OneLoop,{Incoming("\\eta^-"),Incoming("\\eta^-"), Outgoing("e"), Outgoing("e")});
    Display(res0);
    DisplayAbbreviations("EXT");
    std::cout << "Press enter to continue" << '\n';
    //Show(res0);
       
                                          
    csl::Expr squared_ampl1 = toyModel.computeSquaredAmplitude(res);
    cout << squared_ampl1 << endl;
    
    
    mty::Library myLib("SMtrip", path_to_generated_library);
    myLib.cleanExistingSources();
    myLib.addFunction("squared_ampl0", squared_ampl0);
    myLib.addFunction("squared_ampl1", squared_ampl1);

    myLib.applyDiagonalizationData(toyModel);
    
    myLib.print();

    return 0;
}
