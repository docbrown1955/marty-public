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

Expr cc(Expr const &init) {
    return csl::GetComplexConjugate(init);
}

Expr buildDiagonal(std::string const &name)
{
    Expr m1 = constant_s(name + "_1");
    Expr m2 = constant_s(name + "_2");
    Expr m3 = constant_s(name + "_3");
    return matrix_s(
            {
              {m1, 0, 0},
              {0, m2, 0},
              {0, 0, m3}
            });
}

Expr G(int index, bool loop = false) {
    static std::map<int, csl::Index> mu;
    static csl::Index al = DiracIndex();
    static csl::Index first = al;
    if (mu.find(index) == mu.end()) {
        mu[index] = MinkowskiIndex();
    }
    //else
    //    mu[index] = mu[index].getFlipped();

    std::cout << index << " : " << mu[index] << std::endl;
    csl::Index beta = (loop) ? first : DiracIndex();
    auto res = mty::dirac4.gamma({mu[index], al, beta});
    if (loop) {
        al = DiracIndex();
        first = al;
    }
    else
        al = beta;
    return res;
}

csl::Expr Tr(std::vector<int> indices)
{
    std::vector<csl::Expr> matrices(indices.size());
    for (size_t i = 0; i != matrices.size(); ++i)  {
        if (i+1 == matrices.size()) 
            matrices[i] = G(indices[i], true);
        else
            matrices[i] = G(indices[i]);
    }
    Expr init = csl::prod_s(matrices);
    return mty::dirac4.calculateTrace(init);
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);
    
    // Model building
    Model toyModel;
    toyModel.addGaugedGroup(group::Type::SU, "C", 3);
    toyModel.addGaugedGroup(group::Type::U1, "Q");
    toyModel.addFlavorGroup("SM_flavor", 3,true);
    toyModel.init();
    
    toyModel.renameParticle("A_C", "G");
    toyModel.renameParticle("A_Q", "Q");
    
    Particle uLs = scalarboson_s("uLt", toyModel);
    uLs->setGroupRep("C", {1,0});
    uLs->setGroupRep("Q", {2, 3});
    uLs->setFundamentalFlavorRep("SM_flavor");
    
    Particle dLs = scalarboson_s("dLt", toyModel);
    dLs->setGroupRep("C", {1,0});
    dLs->setGroupRep("Q", {-1, 3});
    dLs->setFundamentalFlavorRep("SM_flavor");
    
    Particle uRs = scalarboson_s("uRt", toyModel);
    uRs->setGroupRep("C", {0,1});
    uRs->setGroupRep("Q", {2, 3});
    uRs->setFundamentalFlavorRep("SM_flavor");
    
    Particle dRs = scalarboson_s("dRt", toyModel);
    dRs->setGroupRep("C", {0,1});
    dRs->setGroupRep("Q", {-1, 3});
    dRs->setFundamentalFlavorRep("SM_flavor");
    
    Particle uL = weylfermion_s("u_L", toyModel, Chirality::Left);
    uL->setGroupRep("C", {1,0});
    uL->setGroupRep("Q", {2, 3});
    uL->setFundamentalFlavorRep("SM_flavor");
    
    Particle dL = weylfermion_s("d_L", toyModel, Chirality::Left);
    dL->setGroupRep("C", {1,0});
    dL->setGroupRep("Q", {-1, 3});
    dL->setFundamentalFlavorRep("SM_flavor");
    
    Particle uR = weylfermion_s("u_R", toyModel, Chirality::Right);
    uR->setGroupRep("C", {0,1});
    uR->setGroupRep("Q", {2, 3});
    uR->setFundamentalFlavorRep("SM_flavor");
    
    Particle dR = weylfermion_s("d_R", toyModel, Chirality::Right);
    dR->setGroupRep("C", {0,1});
    dR->setGroupRep("Q", {-1, 3});
    dR->setFundamentalFlavorRep("SM_flavor");
    
    Particle XX = diracfermion_s("XX", toyModel);
    XX->setSelfConjugate(true);
    XX->setGroupRep("Q", {0});
    
    toyModel.addParticle(uLs);
    toyModel.addParticle(uRs);
    toyModel.addParticle(dLs);
    toyModel.addParticle(dRs);
    toyModel.addParticle(uL);
    toyModel.addParticle(dL);
    toyModel.addParticle(uR);
    toyModel.addParticle(dR);
    toyModel.addParticle(XX);
    
    // SM flavor space
    Index I = toyModel.generateIndex("SM_flavor", "u_L");
    Index J = toyModel.generateIndex("SM_flavor", "u_L");
    Index K = toyModel.generateIndex("SM_flavor", "u_L");
    
    // SU(2)L space
    Index i = toyModel.generateIndex("C", "u_L");
    Index j = toyModel.generateIndex("C", "u_L");
    
    // SU(3)C space
    Index a = toyModel.generateIndex("C", "u_R");
    Index b = toyModel.generateIndex("C", "u_R");
    Index c = toyModel.generateIndex("C", "u_R");
    Tensor feps = a.getSpace()->getEpsilon();
    
    auto ii = toyModel.generateIndices(3, "C", "u_L");
    
    auto flavorSpace = toyModel.getVectorSpace("SM_flavor", "u_L");
    Tensor Lpp = csl::Tensor("lpp", {flavorSpace, flavorSpace, flavorSpace});
    Lpp->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MuL = csl::Tensor("muL", {flavorSpace, flavorSpace});
    MuL->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MuR = csl::Tensor("muR", {flavorSpace, flavorSpace});
    MuR->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MdL = csl::Tensor("mdL", {flavorSpace, flavorSpace});
    MdL->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MdR = csl::Tensor("mdR", {flavorSpace, flavorSpace});
    MdR->setComplexProperty(csl::ComplexProperty::Complex);
    
    
    Expr mX = constant_s("mX");
    Tensor guL = csl::Tensor("guL", {flavorSpace, flavorSpace});
    Tensor guR = csl::Tensor("guR", {flavorSpace, flavorSpace});
    Tensor gdL = csl::Tensor("gdL", {flavorSpace, flavorSpace});
    Tensor gdR = csl::Tensor("gdR", {flavorSpace, flavorSpace});
    guL->setTensor(buildDiagonal("guL"));
    guR->setTensor(buildDiagonal("guR"));
    gdL->setTensor(buildDiagonal("gdL"));
    gdR->setTensor(buildDiagonal("gdR"));
    auto al = DiracIndices(2);
    Tensor C = dirac4.C_matrix;

    toyModel.addLagrangianTerm(-Lpp({I,J,K})*uRs({I,a})*feps({a,b,c})*dR({J,b,al[0]})*C({al[0],al[1]})*dR({K, c, al[1]}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-Lpp({I,J,K})*uR({I,a,al[0]})*feps({a,b,c})*C({al[0],al[1]})*dR({J,b,al[1]})*dRs({K, c}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-Lpp({I,J,K})*uR({I,a, al[0]})*C({al[0],al[1]})*feps({a,b,c})*dRs({J,b})*dR({K, c, al[1]}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-guL({I, J})*uLs({I,i})*cc(uL({J, i, al[0]}))*XX(al[0]),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-guR({I, J})*uRs({I,a})*cc(uR({J, a, al[0]}))*XX(al[0]),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-gdL({I, J})*dLs({I,i})*cc(dL({J, i, al[0]}))*XX(al[0]),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-gdR({I, J})*dRs({I,a})*cc(dR({J, a, al[0]}))*XX(al[0]),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-mX*cc(XX(al[0]))*XX(al[0]),
                      true
                      );
    
    toyModel.addLagrangianTerm(cc(uLs({I,i}))*MuL({I,J})*uLs({J,i}));
    toyModel.addLagrangianTerm(cc(uRs({I,a}))*MuR({I,J})*uRs({J,a}));
    toyModel.addLagrangianTerm(cc(dLs({I,i}))*MdL({I,J})*dLs({J,i}));
    toyModel.addLagrangianTerm(cc(dRs({I,a}))*MdR({I,J})*dRs({J,a}));
    
    cout << toyModel << endl;
    
    // Model breaking
    
    toyModel.breakFlavorSymmetry("SM_flavor");
    toyModel.renameParticle("u_L_1","uL");
    toyModel.renameParticle("u_L_2","cL");
    toyModel.renameParticle("u_L_3","tL");
    toyModel.renameParticle("u_R_1","uR");
    toyModel.renameParticle("u_R_2","cR");
    toyModel.renameParticle("u_R_3","tR");
    toyModel.renameParticle("d_L_1","dL");
    toyModel.renameParticle("d_L_2","sL");
    toyModel.renameParticle("d_L_3","bL");
    toyModel.renameParticle("d_R_1","dR");
    toyModel.renameParticle("d_R_2","sR");
    toyModel.renameParticle("d_R_3","bR");
    
    toyModel.renameParticle("uLt_1","uLt");
    toyModel.renameParticle("uLt_2","cLt");
    toyModel.renameParticle("uLt_3","tLt");
    toyModel.renameParticle("uRt_1","uRt");
    toyModel.renameParticle("uRt_2","cRt");
    toyModel.renameParticle("uRt_3","tRt");
    toyModel.renameParticle("dLt_1","dLt");
    toyModel.renameParticle("dLt_2","sLt");
    toyModel.renameParticle("dLt_3","bLt");
    toyModel.renameParticle("dRt_1","dRt");
    toyModel.renameParticle("dRt_2","sRt");
    toyModel.renameParticle("dRt_3","bRt");

    
    toyModel.rotateFields({"uLt","cLt","tLt"},true);
    Expr GuLt = constant_s("GuLt");
    toyModel.getParticle("uLt")->setWidth(GuLt);
    Expr GcLt = constant_s("GcLt");
    toyModel.getParticle("cLt")->setWidth(GcLt);
    Expr GtLt = constant_s("GtLt");
    toyModel.getParticle("tLt")->setWidth(GtLt);
    
    toyModel.rotateFields({"uRt","cRt","tRt"},true);
    Expr GuRt = constant_s("GuRt");
    toyModel.getParticle("uRt")->setWidth(GuRt);
    Expr GcRt = constant_s("GcRt");
    toyModel.getParticle("cRt")->setWidth(GcRt);
    Expr GtRt = constant_s("GtRt");
    toyModel.getParticle("tRt")->setWidth(GtRt);
    
    toyModel.rotateFields({"dLt","sLt","bLt"},true);
    Expr GdLt = constant_s("GdLt");
    toyModel.getParticle("dLt")->setWidth(GdLt);
    Expr GsLt = constant_s("GsLt");
    toyModel.getParticle("sLt")->setWidth(GsLt);
    Expr GbLt = constant_s("GbLt");
    toyModel.getParticle("bLt")->setWidth(GbLt);
    
    toyModel.rotateFields({"dRt","sRt","bRt"},true);
    Expr GdRt = constant_s("GdRt");
    toyModel.getParticle("dRt")->setWidth(GdRt);
    Expr GsRt = constant_s("GsRt");
    toyModel.getParticle("sRt")->setWidth(GsRt);
    Expr GbRt = constant_s("GbRt");
    toyModel.getParticle("bRt")->setWidth(GbRt);
    
    toyModel.refresh();
    std::cout << toyModel << std::endl;
    
    mty::option::displayAbbreviations = false;
    auto rules = toyModel.getFeynmanRules();
    Display(rules); // Displays expressions in terminal
    //Show(rules); // Shows diagrams in the application
    /**/
    auto res0 = toyModel.computeAmplitude(Order::TreeLevel,{Incoming("XX"),Incoming(AntiPart("XX")), Outgoing("uR"), Outgoing(AntiPart("uR"))});
    Display(res0);
    
    auto res1 = toyModel.computeAmplitude(Order::TreeLevel,{Incoming("uRt"), Outgoing(AntiPart("sR")), Outgoing(AntiPart("dR"))});
    Display(res1);
    
    auto res2 = toyModel.computeAmplitude(Order::TreeLevel,{Incoming(AntiPart("XX")), Outgoing("uR"), Outgoing("dR"), Outgoing("dR")});
    Display(res2);
    //Show(res2);
    //DisplayAbbreviations("EXT");
    /**/
     
    Expr squared_ampl0 = toyModel.computeSquaredAmplitude(res0);
    cout << squared_ampl0 << endl;
    
    Expr squared_ampl1 = toyModel.computeSquaredAmplitude(res1);
    cout << squared_ampl1 << endl;
    
    Expr squared_ampl2 = toyModel.computeSquaredAmplitude(res2);
    cout << squared_ampl2 << endl;
    
    Library::setQuadruplePrecision(true);

    mty::Library myLib("BRparity", path_to_generated_library);
    myLib.cleanExistingSources();
    myLib.addFunction("squared_ampl0", squared_ampl0);
    myLib.addFunction("squared_ampl1", squared_ampl1);
    myLib.addFunction("squared_ampl2", squared_ampl2);
    myLib.generateSpectrum(toyModel);
    /*
    myLib.addIPath("<marty-installation-path>/include");
    myLib.addIPath("<csl-installation-path>/include");
    myLib.addLPath("<marty-installation-path>/lib");
    myLib.addLPath("<csl-installation-path>/lib");
    */
    myLib.print();
    
    return system("cd BRparity; make -j 4 >/dev/null");
}
