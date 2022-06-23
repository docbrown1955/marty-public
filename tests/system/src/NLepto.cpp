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

std::string processName(std::vector<Insertion> const &insertions)
{
    std::ostringstream sout;
    sout << "dec_";
    for (const auto &ins : insertions)
        if (ins.isIncoming()) {
            sout << ins.getField()->getName();
            if (!ins.isParticle())
                sout << 'c';
            sout << '_';
        }
    sout << "to_";
    for (const auto &ins : insertions)
        if (!ins.isIncoming()) {
            sout << ins.getField()->getName();
            if (!ins.isParticle())
                sout << 'c';
            sout << '_';
        }

    std::string name = sout.str();
    name.erase(name.end() - 1); // erasing the last '_'

    return name;
}

void addSquaredAmplitude(
        mty::Model                   &model,
        mty::Library                 &library,
        std::vector<Insertion> const &insertions
        )
{
    auto ampl = model.computeAmplitude(
            Order::TreeLevel,
            insertions
            );
    if (ampl.empty())
        return;
    auto sampl = model.computeSquaredAmplitude(ampl);
    library.addFunction(processName(insertions), sampl, "G");
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
    
    Expr thetaW = constant_s("thetaW");
    Expr cW = cos_s(thetaW);
    Expr sW = sin_s(thetaW);
    Expr e = constant_s("e");
    Expr gY = toyModel.getScalarCoupling("g_Y");
    Expr gL = toyModel.getScalarCoupling("g_L");
    toyModel.replace(gY, e / cW);
    toyModel.replace(gL, e / sW);
    
    Particle H = scalarboson_s("H", toyModel);
    H->setGroupRep("L", 1);
    H->setGroupRep("Y", {-1, 2});
    
    
    Particle Li = weylfermion_s("L_L", toyModel, Chirality::Left);
    Li->setGroupRep("L", 1);
    Li->setGroupRep("Y", {-1, 2});
    Li->setFundamentalFlavorRep("SM_flavor");
    
    
    Particle Ei = weylfermion_s("E_R", toyModel, Chirality::Right);
    Ei->setGroupRep("Y", {-1, 1});
    Ei->setFundamentalFlavorRep("SM_flavor");
    
    Particle Ni  = diracfermion_s("NR", toyModel);
    //Particle Ni  = weylfermion_s("N_R", toyModel, Chirality::Right);
    Ni->setFundamentalFlavorRep("SM_flavor");
    
    Particle Si  = diracfermion_s("SL", toyModel);
    //Particle Si  = weylfermion_s("S_L", toyModel, Chirality::Left);
    Si->setFundamentalFlavorRep("SM_flavor");

    Particle XL = weylfermion_s("X_L", toyModel, Chirality::Left);
    
    Particle XR = weylfermion_s("X_R", toyModel, Chirality::Right);
    
    Particle sig = scalarboson_s("ss", toyModel);
    
    toyModel.addParticle(H);
    toyModel.addParticle(Li);
    toyModel.addParticle(Ei);
    toyModel.addParticle(Ni);
    toyModel.addParticle(Si);
    toyModel.addParticle(XL);
    toyModel.addParticle(XR);
    toyModel.addParticle(sig);
    
    // SM flavor space
    Index I = toyModel.generateIndex("SM_flavor", "L_L");
    Index J = toyModel.generateIndex("SM_flavor", "L_L");
    
    // SU(2)L space
    Index i = toyModel.generateIndex("L", "L_L");
    Index j = toyModel.generateIndex("L", "L_L");
    Tensor eps = i.getSpace()->getEpsilon();
    
    //Tensor pauli = toyModel.getGenerator("L", "L_L");
    Tensor pauli = GetGenerator(toyModel,"L", "L_L");
    
    
    
    auto flavorSpace = toyModel.getVectorSpace("SM_flavor", "L_L");
    Tensor Ye = csl::Tensor("Ye", {flavorSpace, flavorSpace});
    Ye->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor YN = csl::Tensor("YN", {flavorSpace, flavorSpace});
    YN->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor MNs = csl::Tensor("MNS", {flavorSpace, flavorSpace});
    Tensor MS = csl::Tensor("MS", {flavorSpace, flavorSpace});
    
    
    Tensor YXL = csl::Tensor("YXL", flavorSpace);
    YXL->setComplexProperty(csl::ComplexProperty::Complex);
    
    Tensor YXR = csl::Tensor("YXR", flavorSpace);
    YXR->setComplexProperty(csl::ComplexProperty::Complex);
    
    Expr muH2 = constant_s("muH2");
    Expr lh = constant_s("lH");
    Expr mus2 = constant_s("muS2");
    Expr ls = constant_s("lS");
    Expr lsh = constant_s("lHS");
    Expr MX = constant_s("mX");
    auto al = DiracIndices(2);

    toyModel.addLagrangianTerm(-cc(Li({I, i, a}))*eps({i,j})*Ye({I, J})*Ei({J, a})*cc(H(j)),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-H(i)*cc(Li({I, i, a}))*YN({I, J})*Ni({J, a}),
                      true // Add also the complex conjugate of this term
                      );
    
    toyModel.addLagrangianTerm(-cc(Si({I, a}))*MNs({I, J})*Ni({J, a}),
                      true // Add also the complex conjugate of this term
                      );
    toyModel.addLagrangianTerm(-sig*cc(XL({a}))*YXL({I})*Ni({I,a}),true);
    toyModel.addLagrangianTerm(-cc(sig)*cc(Si({I,a}))*YXR({I})*XR({a}),true);
    
    toyModel.addLagrangianTerm(-Si({I, a})*MS({I, J})*Si({J, a}),true);
    
    toyModel.addLagrangianTerm(-cc(XL({a}))*MX*XR({a}),true);
    
    toyModel.addLagrangianTerm(-muH2*cc(H(i))*H(i));
    
    Expr prod = cc(H(i)) * H(i);
    toyModel.addLagrangianTerm(lh*prod*RenamedIndices(prod));
    
    toyModel.addLagrangianTerm(-mus2*cc(sig)*sig);
    
    Expr prod1 = cc(sig)*sig;
    toyModel.addLagrangianTerm(ls*prod1*prod1);
    
    Expr prod2 = cc(sig)*sig*cc(H(i))*H(i);
    toyModel.addLagrangianTerm(lsh*prod2);
    
    cout << toyModel << endl;
    
    // Model breaking

    /*
    Expr mN1 = constant_s("m_N1");
    Expr mN2 = constant_s("m_N2");
    Expr mN3 = constant_s("m_N3");
       
    csl::Tensor M_N = csl::tensor_s("M_N",{flavorSpace, flavorSpace},csl::matrix_s({{mN1, 0, 0},{0, mN2, 0},{0, 0, mN3}}));
    Index f_i = GetIndex(flavorSpace);
    Index f_j = GetIndex(flavorSpace);
    toyModel.replace(MNs,M_N({f_i, f_j}));
    */
    
    toyModel.breakFlavorSymmetry("SM_flavor");
    toyModel.renameParticle("L_L_1","L_e");
    toyModel.renameParticle("L_L_2","L_m");
    toyModel.renameParticle("L_L_3","L_t");
    toyModel.renameParticle("E_R_1","e_R");
    toyModel.renameParticle("E_R_2","m_R");
    toyModel.renameParticle("E_R_3","t_R");
    toyModel.renameParticle("NR_1","N_1");
    toyModel.renameParticle("NR_2","N_2");
    toyModel.renameParticle("NR_3","N_3");
    toyModel.renameParticle("SL_1","N_4");
    toyModel.renameParticle("SL_2","N_5");
    toyModel.renameParticle("SL_3","N_6");
    
    Particle N1 = diracfermion_s("N1", toyModel);
    Particle N2 = diracfermion_s("N2", toyModel);
    Particle N3 = diracfermion_s("N3", toyModel);
    Particle N4 = diracfermion_s("N4", toyModel);
    Particle N5 = diracfermion_s("N5", toyModel);
    Particle N6 = diracfermion_s("N6", toyModel);
    N1->setSelfConjugate(true);
    N2->setSelfConjugate(true);
    N3->setSelfConjugate(true);
    N4->setSelfConjugate(true);
    N5->setSelfConjugate(true);
    N6->setSelfConjugate(true);
    toyModel.replace(toyModel.getParticle("N_1"), 1/sqrt_s(2)*N1(al[0]));
    toyModel.replace(toyModel.getParticle("N_2"), 1/sqrt_s(2)*N2(al[0]));
    toyModel.replace(toyModel.getParticle("N_3"), 1/sqrt_s(2)*N3(al[0]));
    toyModel.replace(toyModel.getParticle("N_4"), 1/sqrt_s(2)*N4(al[0]));
    toyModel.replace(toyModel.getParticle("N_5"), 1/sqrt_s(2)*N5(al[0]));
    toyModel.replace(toyModel.getParticle("N_6"), 1/sqrt_s(2)*N6(al[0]));
    
    toyModel.rotateFields({"N1","N2","N3","N4","N5","N6"},true);
    
    Expr GN1 = constant_s("GN1");
    Expr GN2 = constant_s("GN2");
    Expr GN3 = constant_s("GN3");
    Expr GN4 = constant_s("GN4");
    Expr GN5 = constant_s("GN5");
    Expr GN6 = constant_s("GN6");
    toyModel.getParticle("N1")->setWidth(GN1);
    toyModel.getParticle("N2")->setWidth(GN2);
    toyModel.getParticle("N3")->setWidth(GN3);
    toyModel.getParticle("N4")->setWidth(GN4);
    toyModel.getParticle("N5")->setWidth(GN5);
    toyModel.getParticle("N6")->setWidth(GN6);
    
    
    //mty::option::simplifyConjugationMatrix = false;
    toyModel.refresh();
    std::cout << toyModel << std::endl;
    
    auto rules = toyModel.getFeynmanRules();
    Display(rules); // Displays expressions in terminal
    //Show(rules); // Shows diagrams in the application
    
    mty::Library myLib("Nlepto", path_to_generated_library); // Generating the Library
    
    std::vector<Particle> N = toyModel.getPhysicalParticles(
    [&](Particle p) {
        return p->getSpinDimension() == 2 // fermion
            && p->getName()[0] == 'N';    // name starts with 'N'
    });
    std::vector<Particle> L = toyModel.getPhysicalParticles(
    [&](Particle p) {
        return p->getSpinDimension() == 2 // fermion
            && p->getName()[0] == 'L';    // name starts with 'L'
    });

    addSquaredAmplitude(
            toyModel,
            myLib,
            {Incoming("ss"), Incoming(AntiPart("ss")), Outgoing("H"), Outgoing(AntiPart("H"))}
            );

    // Ni --> X ss^*
    // Ni --> Lj H^*
    for (size_t i = 0; i != N.size(); ++i) {
        Particle Ni = N[i];
        addSquaredAmplitude(
                toyModel,
                myLib,
                {Incoming(Ni), Outgoing("X"), Outgoing(AntiPart("ss"))}
                );
        for (size_t j = 0; j != L.size(); ++j) {
            Particle Lj = L[j];
            addSquaredAmplitude(
                    toyModel,
                    myLib,
                    {Incoming(Ni), Outgoing(Lj), Outgoing(AntiPart("H"))}
                    );
        }
    }

    for (size_t kk = 0; kk != 1; ++kk) {
    // ss --> X Li H^* , (ss, X) conjugated or not, (Li, H^*) conjugated or not
    for (size_t i = 0; i != L.size(); ++i) {
        Particle Li = L[i];
        addSquaredAmplitude(
                toyModel,
                myLib,
                {Incoming("ss"), Outgoing("X"), Outgoing(Li), Outgoing(AntiPart("H"))}
                );
        addSquaredAmplitude(
                toyModel,
                myLib,
                {Incoming("ss"), Outgoing("X"), Outgoing(AntiPart(Li)), Outgoing("H")}
                );
        addSquaredAmplitude(
                toyModel,
                myLib,
                {Incoming(AntiPart("ss")), Outgoing(AntiPart("X")), Outgoing(AntiPart(Li)), Outgoing(AntiPart("H"))}
                );
        addSquaredAmplitude(
                toyModel,
                myLib,
                {Incoming(AntiPart("ss")), Outgoing(AntiPart("X")), Outgoing(AntiPart(Li)), Outgoing("H")}
                );
    }
    }

    // X X^* --> Ni Nj^* , with j >= i
    for (size_t i = 0; i != N.size(); ++i) {
        Particle Ni = N[i];
        for (size_t j = i; j != N.size(); ++j) {
            Particle Nj = N[j];
            addSquaredAmplitude(
                    toyModel,
                    myLib,
                    {Incoming("X"), Incoming(AntiPart("X")), Outgoing(Ni), Outgoing(AntiPart(Nj))}
                    );
        }
    }
    myLib.generateSpectrum(toyModel);
    myLib.cleanExistingSources();
    // Writing all the relevant libraries
    myLib.addIPath("/usr/local/Cellar/boost/1.74.0/include");
    myLib.addIPath("/usr/local/Cellar/eigen/3.3.8_1/include");
    myLib.addLPath("/usr/local/Cellar/boost/1.74.0/lib");
    
    myLib.print();
    
    return system("cd Nlepto; make -j 4 >/dev/null");
}
