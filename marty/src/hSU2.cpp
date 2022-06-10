#include "SM.h"
#include "hSU2.h"
#include "hSU2_data.h"
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
 

///////////////////////////////////////////////////
/*************************************************/
// Class hSU2_Model                              //
/*************************************************/
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Constructor, Destructor
///////////////////////////////////////////////////

hSU2_Model::hSU2_Model(): mty::Model() {}

hSU2_Model::hSU2_Model(
       std::string const &t_saveFile
     ):
      mty::Model(),
      mty::hSU2_Data(),
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


hSU2_Model::~hSU2_Model(){

}

///////////////////////////////////////////////////
// Content initialization
///////////////////////////////////////////////////

void hSU2_Model::initContent()
{
    hSU2_Model::initGauge();
    hSU2_Model::initLeptons();
    hSU2_Model::initQuarks();
    hSU2_Model::initHiggs();
}
void hSU2_Model::initGauge(){
    addGaugedGroup(mty::group::Type::SU, "C", 3, sm_input::g_s);
    addGaugedGroup(mty::group::Type::SU, "L", 2);
    addGaugedGroup(mty::group::Type::U1, "Y");
    addGaugedGroup(mty::group::Type::SU, "X", 2);
    SU3_c = gauge->getGaugedGroup(0);
    SU2_L = gauge->getGaugedGroup(1);
    U1_Y  = gauge->getGaugedGroup(2);
    SU2_X = gauge->getGaugedGroup(3);
//    addFlavorGroup("SM_flavor", 3);
    init();

    renameParticle("A_C", "G");
    renameParticle("A_L", "W");
    renameParticle("A_X", "V");
    renameParticle("A_Y", "B");
    renameCoupling("g_L", "g");
    renameCoupling("g_Y", "g'");
    B  = getParticle("B");
    Wi = getParticle("W");
    G  = getParticle("G");
    V  = getParticle("V");
    g_s = getScalarCoupling("g_s");
    g_L = getScalarCoupling("g");
    g_Y = getScalarCoupling("g'");
    g_X = getScalarCoupling("g_X");

    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
}
void hSU2_Model::initLeptons(){
  std::cout << "Initializing Leptons" << std::endl;
  Li = weylfermion_s("{L_L}_i",*this,Chirality::Left);
  L0 = weylfermion_s("{L_L}_0",*this,Chirality::Left);

  Ei = weylfermion_s("{E_R}_i",*this,Chirality::Right);
  E0 = weylfermion_s("{E_R}_i",*this,Chirality::Right);

  Psi_lL = weylfermion_s("\\Psi_{lL}", *this, Chirality::Left);
  Psi_lR = weylfermion_s("\\Psi_{lR}", *this, Chirality::Right);
  Li->setGroupRep("L",1);
  Li->setGroupRep("Y",{-1,2});
  Li->setGroupRep("X",1);

  L0->setGroupRep("L",1);
  L0->setGroupRep("Y",{-1,2});

  Ei->setGroupRep("Y",-1);
  Ei->setGroupRep("X",1);

  E0->setGroupRep("Y",-1);

  Psi_lL->setGroupRep("Y",{-1,2});
  Psi_lL->setGroupRep("X",1);

  Psi_lR->setGroupRep("Y", {-1,2});
  Psi_lR->setGroupRep("X",1);
  addParticles({Li, L0, Ei, E0,Psi_lL, Psi_lR});
}
void hSU2_Model::initQuarks(){
  std::cout << "Initializing quarks" << std::endl;
  Qi = weylfermion_s("{Q_L}_i",*this,Chirality::Left);
  Ui = weylfermion_s("{U_R}_i",*this,Chirality::Right);
  Di = weylfermion_s("{D_L}_i",*this,Chirality::Right);

  Q0 = weylfermion_s("{Q_L}_0",*this, Chirality::Left);
  U0 = weylfermion_s("{U_R}_0",*this, Chirality::Right);
  D0 = weylfermion_s("{D_R}_0",*this, Chirality::Right);

  Qi->setGroupRep("C",{1,0});
  Qi->setGroupRep("L", 1); 
  Qi->setGroupRep("Y", {1, 6});
  Qi->setGroupRep("X", 1);

  Q0->setGroupRep("C", {1, 0});
  Q0->setGroupRep("L",1);
  Q0->setGroupRep("Y",{1,6});
  Q0->setGroupRep("X",0); // SM quark SU(2)_L doublet

  Ui->setGroupRep("C", {1, 0});
  Ui->setGroupRep("Y", {2, 3});
  Ui->setGroupRep("X",1);

  Di->setGroupRep("C",{1,0});
  Di->setGroupRep("Y", {-1, 3});
  Di->setGroupRep("X",1);

  U0->setGroupRep("C",{1,0});
  D0->setGroupRep("C",{1,0});

  U0->setGroupRep("Y", {2, 3});
  D0->setGroupRep("Y", {-1, 3});

  U0->setGroupRep("X",0);
  D0->setGroupRep("X",0);

  addParticle(Qi);
  addParticle(Ui);
  addParticle(Di);

  addParticle(Q0);
  addParticle(U0);
  addParticle(D0);

  Psi_uL = weylfermion_s("\\Psi_{uL}",*this, Chirality::Left);
  Psi_uR = weylfermion_s("\\Psi_{uR}",*this, Chirality::Right);
  Psi_dL = weylfermion_s("\\Psi_{dL}",*this, Chirality::Left);
  Psi_dR = weylfermion_s("\\Psi_{dR}",*this, Chirality::Right);
  X = scalarboson_s("X",*this);
  Y = scalarboson_s("Y",*this);

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
  csl::Expr m_H = csl::constant_s("m_H"); // higgs mass. Should be left as input. 
  H->setMass(m_H);
  addParticle(H); // 
}
void hSU2_Model::initInteractions(){
  //Complete the Lagrangian with Yukawa interactions.   
  std::cout << "Building Yukawa interactions" << std::endl;
  std::cout << "Building Yukawa interactions" << std::endl;
  Particle H = GetParticle(*this, "H");
  Qi = GetParticle(*this, "{Q_L}_i");
  Psi_uR = GetParticle(*this, "\\Psi_{uR}");
  Psi_uL = GetParticle(*this, "\\Psi_{uL}");
  Y = GetParticle(*this, "Y");
  X = GetParticle(*this, "X");
  Q0 = GetParticle(*this, "{Q_L}_0");
  U0 = GetParticle(*this, "{U_R}_0");
  // Indices 
  csl::Index a = generateIndex("C",Qi); // SU(3)_c index
  csl::Index I  = generateIndex("X",Qi); // SU(2)_h index
  csl::Index J  = generateIndex("X",Qi); // SU(2)_h index
  std::vector< csl::Index > i = generateIndices(2,"L",Qi);
  auto al = DiracIndex();

  csl::Tensor eps = getVectorSpace("L", "{Q_L}_i")->getEpsilon(); // = i * sigma^2
  Expr y_u = constant_s("y_u");
  Expr lambda_u = constant_s("\\lambda_u");
  Expr tilde_lambda_u = constant_s("\\tilde{\\lambda}_u");

  // Interaction term : 
  addLagrangianTerm(
      y_u 
      * GetComplexConjugate(Qi({a,i[0],I, al}))
      * eps({i[0],i[1]})
      * GetComplexConjugate(H(i[1]))
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
      * GetComplexConjugate(Q0({a,i[0],al}))
      * eps({i[0],i[1]})
      * GetComplexConjugate(H(i[1]))
      * U0({a,al})
      ,true);
    
  csl::Expr k_u0 = hSU2_input::k_u0 ;
  csl::Expr k_u1 = hSU2_input::k_u1 ;
  auto  hsu2_space = getVectorSpace("X",Ui);

  addLagrangianTerm(
      k_u0 
      * GetComplexConjugate(Psi_uR({a,I,al}))
      * X(I) 
      * U0({a,al})
      ,true);
 
  csl::Tensor K_u = csl::tensor_s(
      "\\kappa_u",
      {hsu2_space},
      csl::matrix_s({k_u1,CSL_0})
      );
  std::cout << "Tensor K_u : " << std::endl;
  std::cout << K_u->getTensor() << std::endl;
  addLagrangianTerm(
      K_u(J)
      * GetComplexConjugate(Psi_uR({a,I,al}))
      * X(I) 
      * Ui({a,J,al})
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
