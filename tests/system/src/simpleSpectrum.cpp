#include "comparedata.h"
#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

Expr cc(Expr const &expr) { return GetComplexConjugate(expr); }

int main() {

    // Dirac space
    Index a = DiracIndex();
    Index b = DiracIndex();
    
    Model toyModel;
    toyModel.addGaugedGroup(group::Type::SU, "D", 2);
    toyModel.addGaugedGroup(group::Type::SU, "L", 2);
    toyModel.addFlavorGroup("dark_flavor", 10,true);
    //toyModel.addFlavorGroup("Psi_flavor", 2,true);
    toyModel.init();
    
    toyModel.renameParticle("A_D", "VD");
    toyModel.renameParticle("A_L", "W");
    
    Particle H = scalarboson_s("H", toyModel);
    H->setGroupRep("L", 1);
    toyModel.addParticle(H);
    
    Particle Phi = scalarboson_s("phi", toyModel);
    Phi->setGroupRep("D", 1);
    toyModel.addParticle(Phi);
    
    Particle Psi = diracfermion_s("Psi", toyModel);
    Psi->setGroupRep("D", 1);
    Psi->setFundamentalFlavorRep("dark_flavor");
    toyModel.addParticle(Psi);
    
    // Dark flavor space
    Index I = toyModel.generateIndex("dark_flavor", "Psi");
    Index J = toyModel.generateIndex("dark_flavor", "Psi");
    
    // SU(2)L space
    Index i = toyModel.generateIndex("L", "H");
    Index iD = toyModel.generateIndex("D", "phi");
    
    Expr Mpsi = constant_s("MPsi");
    Expr muH2 = constant_s("mhH2");
    Expr muPS2 = constant_s("muPS2");
    Expr lSH = constant_s("lSH");
    Expr lSS = constant_s("lSS");
    //Expr vh = constant_s("vh");
    //Expr vS = constant_s("vS");
    
    auto al = DiracIndices(3);
    Tensor PL = dirac4.P_L;
    Tensor PR = dirac4.P_R;
    Tensor C = dirac4.C_matrix;

    toyModel.addLagrangianTerm(-Mpsi*cc(Psi({I,iD,a}))*Psi({I,iD,a}), true);
    
    Expr prod = cc(Phi(iD))*Phi(iD);
    toyModel.addLagrangianTerm(lSS*prod*RenamedIndices(prod));

    toyModel.addLagrangianTerm(muH2*cc(H(i))*H(i));
    
    toyModel.addLagrangianTerm(lSH*cc(Phi(iD))*Phi(iD)*cc(H(i))*H(i));
    
    cout << toyModel << endl;
    
    // Model breaking
        
    Expr gD = toyModel.getScalarCoupling("g_D");
    toyModel.breakGaugeSymmetry("D");

    toyModel.renameParticle("Psi_1","Psi_1 ; \\Psi_1");
    toyModel.renameParticle("Psi_2","Psi_2 ; \\Psi_2");

    toyModel.renameParticle("phi_1", "ph0");
    toyModel.renameParticle("phi_2", "php");

    // Dark-Higgs mechanism simulated
    Particle VD_1 = toyModel.getParticle("VD_1");
    Particle VD_2 = toyModel.getParticle("VD_2");
    Particle F_VD_1 = VD_1->getFieldStrength();
    Particle F_VD_2 = VD_2->getFieldStrength();
    Particle VW = VD_1->generateSimilar("VW");
    VW->setSelfConjugate(false);
    
    toyModel.renameParticle("VD_3","VZ");

    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();

    Particle ph0 = toyModel.getParticle("ph0");
    Particle phR = ph0->generateSimilar("phR");
    phR->setSelfConjugate(true);
    Particle phI = ph0->generateSimilar("phI");
    phI->setSelfConjugate(true);

    csl::Expr vph = constant_s("vph");
    toyModel.replace(ph0, (vph + phR + CSL_I*phI) / sqrt_s(2));

    // VD_1 goes to (VD+ + VD-) / sqrt(2)
    toyModel.replace(VD_1, (VW(mu) + GetComplexConjugate(VW(mu))) / sqrt_s(2));
    toyModel.replace(F_VD_1, (VW({mu, nu}) + GetComplexConjugate(VW({mu, nu}))) / sqrt_s(2));
    //// VD_2 goes to i*(VD+ - VD-) / sqrt(2)
    toyModel.replace(VD_2, CSL_I * (VW({mu}) - GetComplexConjugate(VW({mu}))) / sqrt_s(2));
    toyModel.replace(F_VD_2, CSL_I * (VW({mu, nu}) - GetComplexConjugate(VW({mu, nu}))) / sqrt_s(2));

    toyModel.promoteToGoldstone("php", "VW");
    toyModel.promoteToGoldstone("phI", "VZ");
 
    toyModel.renameParticle("phR","phR ; \\phi_R");
    
    toyModel.refresh();

    [[maybe_unused]] int sysres = system("rm -rf libs/simpleSpectrum");
    mty::Library lib("simpleSpectrum", "libs");
    lib.generateSpectrum(toyModel);
    lib.print();

    sysres = system("cp libsrc/example_simplespectrum.cpp libs/simpleSpectrum/script");
    sysres = system("cd libs/simpleSpectrum; make && bin/example_simplespectrum.x");
    return assert_equal(
        "data/output/simplespectrum.txt", "libs/simpleSpectrum/data.txt", 1e-10);

   }
