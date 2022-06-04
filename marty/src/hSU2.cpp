#include "SM.h"
#include "hSU2.h"
#include "diracology.h"
#include "fermionicField.h"
#include "vectorField.h"
#include "scalarField.h"
#include "mrtInterface.h"
#include "SM.h"
#include "CKM.h"
#include "ghostField.h"

using namespace mty;
using namespace csl;

namespace mty{
 
   hSU2_Model::hSU2_Model(): mty::SM_Model() {}
  hSU2_Model::hSU2_Model
    (
       std::string const &t_saveFile
     ):
      mty::SM_Model(), // inherits all SM methods for EWSSB
      saveFile(t_saveFile)
  {
    std::ofstream save;
    if (!saveFile.empty())
      save.open(saveFile);
    std::cout << "Initializing gauge and particle content ..." 
      << std::endl;
    initContent();
    std::cout << "Initializing interactions ..." << std::endl;
    initInteractions();
    if (save) {
      save << "****************************" << std::endl;
      save << "**   Initial Lagrangian   **" << std::endl;
      save << "****************************" << std::endl;
      save << *this << "\n\n";
    }
    std::cout << "Gathering Model inputs ..." << std::endl;
    //      gatherhSU2Inputs();
    std::cout << "Getting to low energy Lagrangian ..." << std::endl;
    horizontalSymmetryBreaking();
    getToLowEnergyLagrangian();
    if (save) {
      save << "****************************" << std::endl;
      save << "**    Final Lagrangian    **" << std::endl;
      save << "****************************" << std::endl;
      save << *this << "\n\n";
    }
    std::cout << "Checking Hermiticity ..." << std::endl;
    checkHermiticity();
    refresh();
    getFeynmanRules();
    if (save) {
      std::ostream &out = save;
      mty::Display(ComputeFeynmanRules(*this), out);
      mty::DisplayAbbreviations(out);
    }
    if (save)
      save.close();

  } // End of base constructor for hSU2 class 

void hSU2_Model::initContent(){
  hSU2_Model::initGauge();
  hSU2_Model::initLeptons();
  hSU2_Model::initQuarks();
  hSU2_Model::initHiggs();
}
void hSU2_Model::initGauge(){
//  Init all Gauge and flavoured fields
    std::cout << "Initializing gauge content" << std::endl;
    addGaugedGroup(mty::group::Type::SU, "C", 3, sm_input::g_s);
    addGaugedGroup(mty::group::Type::SU, "L", 2);
    addGaugedGroup(mty::group::Type::U1, "Y");
    addGaugedGroup(mty::group::Type::SU, "X", 2);
    auto SU3_c = gauge->getGaugedGroup(0);
    auto SU2_L = gauge->getGaugedGroup(1);
    auto U1_Y  = gauge->getGaugedGroup(2);
    auto SU2_X = gauge->getGaugedGroup(3);
//    addFlavorGroup("SM_flavor", 3);
    Model::init();

    renameParticle("A_C", "G");
    renameParticle("A_L", "W");
    renameParticle("A_X", "V");
    renameParticle("A_Y", "B");
    renameCoupling("g_L", "g");
    renameCoupling("g_Y", "g'");
    auto B  = getParticle("B");
    auto Wi = getParticle("W");
    auto G  = getParticle("G");
    auto V  = getParticle("V");
    auto g_s = getScalarCoupling("g_s");
    auto g_L = getScalarCoupling("g");
    auto g_Y = getScalarCoupling("g'");
    auto g_X = getScalarCoupling("g_X");

    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
}

hSU2_Model::~hSU2_Model(){

}

void hSU2_Model::initLeptons(){
    std::cout << "Initializing Leptons" << std::endl;
    Particle L_L = weylfermion_s("L_L",*this,Chirality::Left);
    Particle l_L = weylfermion_s("l_L",*this,Chirality::Left);

    Particle E_R = weylfermion_s("E_R",*this,Chirality::Right);
    Particle e_R = weylfermion_s("E_R",*this,Chirality::Right);

    Particle Psi_lL = weylfermion_s("\\Psi_{lL}", *this, Chirality::Left);
    Particle Psi_lR = weylfermion_s("\\Psi_{lR}", *this, Chirality::Right);
    L_L->setGroupRep("L",1);
    L_L->setGroupRep("Y",{-1,2});
    L_L->setGroupRep("X",1);

    l_L->setGroupRep("L",1);
    l_L->setGroupRep("Y",{-1,2});

    E_R->setGroupRep("Y",-1);
    E_R->setGroupRep("X",1);

    e_R->setGroupRep("Y",-1);

    Psi_lL->setGroupRep("Y",{-1,2});
    Psi_lL->setGroupRep("X",1);

    Psi_lR->setGroupRep("Y", {-1,2});
    Psi_lR->setGroupRep("X",1);
    addParticles({L_L, l_L, E_R, e_R,Psi_lL, Psi_lR});
}
void hSU2_Model::initQuarks(){
    std::cout << "Initializing quarks" << std::endl;
    Particle Q_L = weylfermion_s("Q_L",*this,Chirality::Left);
    Particle U_R = weylfermion_s("U_R",*this,Chirality::Right);
    Particle D_R = weylfermion_s("D_R",*this,Chirality::Right);

    Particle q_L_0 = weylfermion_s("q_{L0}",*this, Chirality::Left);
    Particle u_R_0 = weylfermion_s("u_{R0}",*this, Chirality::Right);
    Particle d_R_0 = weylfermion_s("d_{R0}",*this, Chirality::Right);

    Q_L->setGroupRep("C",{1,0});
    Q_L->setGroupRep("L", 1); 
    Q_L->setGroupRep("Y", {1, 6});
    Q_L->setGroupRep("X", 1);

    q_L_0->setGroupRep("C", {1, 0});
    q_L_0->setGroupRep("L",1);
    q_L_0->setGroupRep("Y",{1,6});
    q_L_0->setGroupRep("X",0); // SM quark SU(2)_L doublet

    U_R->setGroupRep("C", {1, 0});
    U_R->setGroupRep("Y", {2, 3});
    U_R->setGroupRep("X",1);
    
    D_R->setGroupRep("C",{1,0});
    D_R->setGroupRep("Y", {-1, 3});
    D_R->setGroupRep("X",1);
   
    u_R_0->setGroupRep("C",{1,0});
    d_R_0->setGroupRep("C",{1,0});

    u_R_0->setGroupRep("Y", {2, 3});
    d_R_0->setGroupRep("Y", {-1, 3});
   
    u_R_0->setGroupRep("X",0);
    d_R_0->setGroupRep("X",0);

    addParticle(Q_L);
    addParticle(U_R);
    addParticle(D_R);

    addParticle(q_L_0);
    addParticle(u_R_0);
    addParticle(d_R_0);

    Particle Psi_uL = weylfermion_s("\\Psi_{uL}",*this, Chirality::Left);
    Particle Psi_uR = weylfermion_s("\\Psi_{uR}",*this, Chirality::Right);
    Particle Psi_dL = weylfermion_s("\\Psi_{dL}",*this, Chirality::Left);
    Particle Psi_dR = weylfermion_s("\\Psi_{dR}",*this, Chirality::Right);
    Particle X = scalarboson_s("X",*this);
    Particle Y = scalarboson_s("Y",*this);

    Psi_uL->setGroupRep("C",{1,0});
    Psi_uL->setGroupRep("L",0);
    Psi_uL->setGroupRep("X",1);
    Psi_uL->setGroupRep("Y",{2,3});
    
    Psi_uR->setGroupRep("C",{1,0});
    Psi_uR->setGroupRep("L",0);
    Psi_uR->setGroupRep("Y",{2,3});
    Psi_uR->setGroupRep("X",1);

    Psi_dL->setGroupRep("C",{1,0});
    Psi_dL->setGroupRep("L",0);
    Psi_dL->setGroupRep("Y",{-1,3});
    Psi_dL->setGroupRep("X",1);

    Psi_dR->setGroupRep("C",{1,0});
    Psi_dR->setGroupRep("L",0);
    Psi_dR->setGroupRep("Y",{-1,3});
    Psi_dR->setGroupRep("X",1);

    X->setGroupRep("L",0);
    X->setGroupRep("Y",0);
    X->setGroupRep("X",1); // Should be X_i

    Y->setGroupRep("L",0);
    Y->setGroupRep("Y",0);
    Y->setGroupRep("X",2); // Y_ij in the adjoint representation. 

    addParticle(Psi_uL);
    addParticle(Psi_uR);
    addParticle(Psi_dL);
    addParticle(Psi_dR);
    addParticle(X);
    addParticle(Y);

}

void hSU2_Model::initHiggs(){
  std::cout << "Initializing SM Higgs Field" << std::endl;
  Particle H = mty::scalarboson_s("H",*this);
  H->setGroupRep("L",1);
  H->setGroupRep("Y",{1,2});
//  csl::Expr m_H = csl::constant_s("m_H"); // higgs mass. Should be left as input. 
//  H->setMass(m_H);
  addParticle(H); // 
}
void hSU2_Model::initInteractions(){
  //Complete the Lagrangian with Yukawa interactions.   
  std::cout << "Building Yukawa interactions" << std::endl;
  // Yukawa interaction Q_L * H * Psi_uR
    std::cout << "Building Yukawa interactions" << std::endl;
    // Yukawa interaction Q_L * H * Psi_uR
    //get particles → need to add them as attributes.
    Particle H = GetParticle(*this, "H");
    Particle Q_L = GetParticle(*this, "Q_L");
    Particle Psi_uR = GetParticle(*this, "\\Psi_{uR}");
    Particle Psi_uL = GetParticle(*this, "\\Psi_{uL}");
    Particle Y = GetParticle(*this, "Y");
    Particle q_L_0 = GetParticle(*this, "q_{L0}");
    Particle u_R_0 = GetParticle(*this, "u_{R0}");
    // Indices 
    csl::Index a = generateIndex("C",Q_L); // SU(3)_c index
    csl::Index I  = generateIndex("X",Q_L); // SU(2)_h index
    std::vector< csl::Index > i = generateIndices(2,"L",Q_L);
    auto al = DiracIndex();
    // auto mu = MinkowskiIndices(1);
    // Tensor X = MinkowskiVector("X");

    // csl::Tensor half_sigma = getGenerator("L",H);
    csl::Tensor eps = getVectorSpace("L", "Q_L")->getEpsilon(); // = i * sigma^2
    // int which_sigma = 2;
    // Tensor gamma = DiracGamma();
    Expr y_u = constant_s("y_u");
    Expr lambda_u = constant_s("\\lambda_u");
    Expr tilde_lambda_u = constant_s("\\tilde{\\lambda}_u");

    // Interaction term : 
    addLagrangianTerm(
        y_u 
        * GetComplexConjugate(Q_L({a,i[0],I, al}))
        // * 2
        // * half_sigma({which_sigma,i[0],i[1]})
        * eps({i[0],i[1]})
        * GetComplexConjugate(H(i[1]))
        // * gamma({mu[0], al[0], al[1]})
        * Psi_uR({a,I,al}),
        true); // Add also the complex conjugate of this term

    std::vector<csl::Index> ii = generateIndices(2, "X", Psi_uL);
    csl::Index A = generateIndex("X", Y);  
    csl::Tensor half_sigma = getGenerator("X", Psi_uL); // SU(2)_X generators
    csl::Expr term = 
      lambda_u 
      * GetComplexConjugate(Psi_uL({a,ii[0],al})) 
      * Y(A)
      * half_sigma({A, ii[0], ii[1]}) 
      * Psi_uR({a,ii[1],al});
    addLagrangianTerm(term, true);
    
    addLagrangianTerm(
       tilde_lambda_u
       * GetComplexConjugate(Psi_uL({a,ii[0],al})) 
       * eps({i[0],i[1]})
       * Y(A)
       * eps({i[0],i[1]}) // = CSL_I * Sigma(2)
       * half_sigma({A,ii[0],ii[1]})
       * Psi_uR({a,ii[1],al})
       ,true );

    Expr y_u0 = constant_s("y_{u0}");
    addLagrangianTerm(
      y_u0 
      * GetComplexConjugate(q_L_0({a,i[0],al}))
      * eps({i[0],i[1]})
      * GetComplexConjugate(H(i[1]))
      * u_R_0({a,al})
      ,true);


}
void hSU2_Model::gatherhSU2Inputs(){

}
void hSU2_Model::horizontalSymmetryBreaking(){
    ////////////////////////////////////////////////
    // Horizontal symmetry breaking 
    ///////////////////////////////////////////////
//    BreakGaugeSymmetry(*this, "X");
//    renameParticle("")
}

void hSU2_Model::getToLowEnergyLagrangian(){

}
void hSU2_Model::checkHermiticity(){

}
void hSU2_Model::adjust_fermions(){

}
} // End of namespace mty
