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
    gatherhSU2Inputs();
    std::cout << "Getting to low energy Lagrangian ..." << std::endl;
    getToLowEnergyLagrangian();
    if (save) {
      save << "****************************" << std::endl;
      save << "**    Final Lagrangian    **" << std::endl;
      save << "****************************" << std::endl;
      save << *this << "\n\n";
    }
    std::cout << "Checking Hermiticity ..." << std::endl;
    // checkHermiticity();
    refresh();
    getFeynmanRules();
    std::cout << std::endl << std::endl << "Coucou! Checking for X..." << std::endl;

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

  E0 = weylfermion_s("{E_R}_0",*this,Chirality::Right);
  E1 = weylfermion_s("{E_R}_1",*this,Chirality::Right);
  E2 = weylfermion_s("{E_R}_2",*this,Chirality::Right);

  Psi_lL = weylfermion_s("\\Psi_{lL}", *this, Chirality::Left);
  Psi_lR = weylfermion_s("\\Psi_{lR}", *this, Chirality::Right);
  Li->setGroupRep("L",1);
  Li->setGroupRep("Y",{-1,2});
  Li->setGroupRep("X",1);

  L0->setGroupRep("L",1);
  L0->setGroupRep("Y",{-1,2});


  E0->setGroupRep("Y",-1);
  E1->setGroupRep("Y",-1);
  E2->setGroupRep("Y",-1);

  Psi_lL->setGroupRep("Y",-1);
  Psi_lL->setGroupRep("X",1);

  Psi_lR->setGroupRep("Y", -1);
  Psi_lR->setGroupRep("X",1);
  addParticles({Li, L0, E0, E1, E2, Psi_lL, Psi_lR});
}
void hSU2_Model::initQuarks(){
  std::cout << "Initializing quarks" << std::endl;
  Qi = weylfermion_s("{Q_L}_i",*this,Chirality::Left);
  Q0 = weylfermion_s("{Q_L}_0",*this, Chirality::Left);
  
  U0 = weylfermion_s("{U_R}_0",*this, Chirality::Right);
  D0 = weylfermion_s("{D_R}_0",*this, Chirality::Right);
  
  U1 = weylfermion_s("{U_R}_1",*this, Chirality::Right);
  D1 = weylfermion_s("{D_R}_1",*this, Chirality::Right);
  
  U2 = weylfermion_s("{U_R}_2",*this, Chirality::Right);
  D2 = weylfermion_s("{D_R}_2",*this, Chirality::Right);

  Qi->setGroupRep("C",{1,0});
  Qi->setGroupRep("L", 1); 
  Qi->setGroupRep("Y", {1, 6});
  Qi->setGroupRep("X", 1);

  Q0->setGroupRep("C", {1, 0});
  Q0->setGroupRep("L",1);
  Q0->setGroupRep("Y",{1,6});
  Q0->setGroupRep("X",0); // SM quark SU(2)_L doublet


  U0->setGroupRep("C",{1,0});
  D0->setGroupRep("C",{1,0});

  U0->setGroupRep("Y", {2, 3});
  D0->setGroupRep("Y", {-1, 3});

  U1->setGroupRep("C",{1,0});
  D1->setGroupRep("C",{1,0});

  U1->setGroupRep("Y", {2, 3});
  D1->setGroupRep("Y", {-1, 3});
  
  U2->setGroupRep("C",{1,0});
  D2->setGroupRep("C",{1,0});

  U2->setGroupRep("Y", {2, 3});
  D2->setGroupRep("Y", {-1, 3});

  addParticle(Qi);
  addParticle(Q0);
  
  addParticle(U0);
  addParticle(U1);
  addParticle(U2);
  addParticle(D0);
  addParticle(D1);
  addParticle(D2);

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
  H = mty::scalarboson_s("H",*this);
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

  // Indices 
  csl::Index a = generateIndex("C",Qi); // SU(3)_c index
  csl::Index I  = generateIndex("X",Qi); // SU(2)_h index
  csl::Index J  = generateIndex("X",Li); // SU(2)_h index
  csl::Index A = generateIndex("X", Y);  
  csl::Index Il = generateIndex("L", Qi);  
  std::vector<csl::Index> ih = generateIndices(2, "X", Psi_uL);
  std::vector<csl::Index> il = generateIndices(2, "L", Qi);
  auto al = DiracIndex();

  csl::Tensor half_sigma = getGenerator("X", Psi_uL); // SU(2)_X generators
  csl::Tensor eps = getVectorSpace("L", "{Q_L}_i")->getEpsilon(); // = i * sigma^2
  csl::Tensor eps_h = getVectorSpace("X", "{Q_L}_i")->getEpsilon(); // = i * sigma^2

  Expr y_u = constant_s("y_u");
  Expr lambda_u = constant_s("\\lambda_u");
  Expr tilde_lambda_u = constant_s("\\tilde{\\lambda}_u");
 
  Expr y_d = constant_s("y_d");
  Expr lambda_d = constant_s("\\lambda_d");
  Expr tilde_lambda_d = constant_s("\\tilde{\\lambda}_d");
  
  Expr y_l = constant_s("y_l");
  Expr lambda_l = constant_s("\\lambda_l");
  Expr tilde_lambda_l = constant_s("\\tilde{\\lambda}_l");

  // Interaction terms : 
  // up sector
  addLagrangianTerm(
      y_u 
      * GetComplexConjugate(Qi({a,il[0],I, al}))
      * eps({il[0],il[1]})
      * GetComplexConjugate(H(il[1]))
      * Psi_uR({a,I,al}),
      true); // Add also the complex conjugate of this term

  addLagrangianTerm(
      lambda_u 
      * GetComplexConjugate(Psi_uL({a,ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_uR({a,ih[1],al})
      ,true);

  addLagrangianTerm(
      tilde_lambda_u
      * GetComplexConjugate(Psi_uL({a,ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_uR({a,ih[1],al})
      ,true );

  Expr y_u0 = constant_s("y_{u0}");
  addLagrangianTerm(
      y_u0 
      * GetComplexConjugate(Q0({a,il[0],al}))
      * eps({il[0],il[1]})
      * GetComplexConjugate(H(il[1]))
      * U0({a,al})
      ,true);

  csl::Expr k_u0 = hSU2_input::k_u0 ;
  csl::Expr k_u1 = hSU2_input::k_u1 ;
  csl::Expr tk_u0 = hSU2_input::tk_u0 ;
  csl::Expr tk_u2 = hSU2_input::tk_u2 ;


  std::vector<csl::Expr> K_u  =  {k_u0, k_u1, CSL_0} ;
  std::vector<csl::Expr> tK_u = {tk_u0, CSL_0, tk_u2};

  std::vector<Particle> U_R = {U0, U1, U2};

  for(int a_u = 0; a_u < 3; a_u++){
    addLagrangianTerm(
        K_u[a_u]
        * GetComplexConjugate(Psi_uL({a,I,al}))
        * X(I)
        * U_R[a_u]({a,al})
        ,true);
    addLagrangianTerm(
        tK_u[a_u]
        * GetComplexConjugate(Psi_uL({a,ih[0],al}))
        * eps_h({ih[0],ih[1]})
        * GetComplexConjugate(X(ih[1]))
        * U_R[a_u]({a,al})
        ,true);
    refresh();
  }
  std::cout << "Up quark sector interactions complete..." << std::endl;
  // Down sector 
  // No charge conjugation on Higgs
  addLagrangianTerm(
      y_d
      * GetComplexConjugate(Qi({a,Il,I, al}))
     // * eps({il[0],il[1]})
      * H(Il)
      * Psi_dR({a,I,al}),
      true); // Add also the complex conjugate of this term
  addLagrangianTerm( 
      lambda_d 
      * GetComplexConjugate(Psi_dL({a,ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_dR({a,ih[1],al})
      ,true);

  addLagrangianTerm(
      tilde_lambda_d
      * GetComplexConjugate(Psi_dL({a,ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_dR({a,ih[1],al})
      ,true );

  Expr y_d0 = constant_s("y_{d0}");
  addLagrangianTerm(
      y_d0 
      * GetComplexConjugate(Q0({a,Il,al}))
      * H(Il)
      * D0({a,al})
      ,true);

  csl::Expr k_d0 = hSU2_input::k_d0 ;
  csl::Expr k_d1 = hSU2_input::k_d1 ;
  csl::Expr tk_d0 = hSU2_input::tk_d0 ;
  csl::Expr tk_d2 = hSU2_input::tk_d2 ;


  std::vector<csl::Expr> K_d  =  {k_d0, k_d1, CSL_0} ;
  std::vector<csl::Expr> tK_d = {tk_d0, CSL_0, tk_d2};

  std::vector<Particle> D_R = {D0, D1, D2};

  for(int a_d = 0; a_d < 3; a_d++){
    addLagrangianTerm(
        K_d[a_d]
        * GetComplexConjugate(Psi_dL({a,I,al}))
        * X(I)
        * D_R[a_d]({a,al})
        ,true);
    addLagrangianTerm(
        tK_d[a_d]
        * GetComplexConjugate(Psi_dL({a,ih[0],al}))
        * eps_h({ih[0],ih[1]})
        * GetComplexConjugate(X(ih[1]))
        * D_R[a_d]({a,al})
        ,true);
  }
  // Lepton sector
  addLagrangianTerm(
      y_l
      * GetComplexConjugate(Li({Il,I, al}))
      * H(Il)
      * Psi_lR({I,al}),
      true); // Add also the complex conjugate of this term

  addLagrangianTerm(
      lambda_l 
      * GetComplexConjugate(Psi_lL({ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_lR({ih[1],al})
      ,true);

  addLagrangianTerm(
      tilde_lambda_l
      * GetComplexConjugate(Psi_lL({ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_lR({ih[1],al})
      ,true );

  Expr y_l0 = constant_s("y_{l0}");
  addLagrangianTerm(
      y_l0
      * GetComplexConjugate(L0({Il,al}))
      * H(Il)
      * E0({al})
      ,true);
  
  csl::Expr k_l0 = hSU2_input::k_l0 ;
  csl::Expr k_l1 = hSU2_input::k_l1 ;
  csl::Expr tk_l0 = hSU2_input::tk_l0 ;
  csl::Expr tk_l2 = hSU2_input::tk_l2 ;


  std::vector<csl::Expr> K_l  =  {k_l0, k_l1, CSL_0} ;
  std::vector<csl::Expr> tK_l = {tk_l0, CSL_0, tk_l2};

  std::vector<Particle> E_R = {E0, E1, E2};

  for(int a_l = 0; a_l < 3; a_l++){
    addLagrangianTerm(
        K_l[a_l]
        * GetComplexConjugate(Psi_lL({J,al}))
        * X(J)
        * E_R[a_l]({al})
        ,true);
    addLagrangianTerm(
        tK_l[a_l]
        * GetComplexConjugate(Psi_lL({ih[0],al}))
        * eps_h({ih[0],ih[1]})
        * GetComplexConjugate(X(ih[1]))
        * E_R[a_l]({al})
        ,true);
  }
  // Vector-like mass terms 
  csl::Expr m_psi_u = hSU2_input::M_U;
  csl::Expr m_psi_d = hSU2_input::M_D;
  csl::Expr m_psi_l = hSU2_input::M_L;
  addLagrangianTerm(
       m_psi_u 
       * GetComplexConjugate(Psi_uL({a,J,al}))
       * Psi_uR({a,J,al})
       , true);
  addLagrangianTerm(
       m_psi_d
       * GetComplexConjugate(Psi_dL({a,J,al}))
       * Psi_dR({a,J,al})
       , true);
  addLagrangianTerm(
       m_psi_l 
       * GetComplexConjugate(Psi_lL({J,al}))
       * Psi_lR({J,al})
       ,true);
}
void hSU2_Model::gatherhSU2Inputs(){

}

void hSU2_Model::getToLowEnergyLagrangian(){
  
     std::cout << "Breaking the SU(2)_h symmetry...." << std::endl;
     horizontalSymmetryBreaking();
     std::cout << "Expanding around additional Scalars' VEV's" << std::endl; 
     expandAroundVEVs();
}
void hSU2_Model::horizontalSymmetryBreaking(){
    ////////////////////////////////////////////////
    // Horizontal symmetry breaking 
    ///////////////////////////////////////////////

     BreakGaugeSymmetry(*this, "X");
     renameParticle("{U_R}_0","u_R");
     renameParticle("{U_R}_1","c_R");
     renameParticle("{U_R}_2","t_R");

     renameParticle("{D_R}_0","d_R");
     renameParticle("{D_R}_1","s_R");
     renameParticle("{D_R}_2","b_R");
     
     renameParticle("{E_R}_0","e_R");
     renameParticle("{E_R}_1","\\mu_R");
     renameParticle("{E_R}_2","\\tau_R");
}

void hSU2_Model::expandAroundVEVs(){
  csl::Expr v_h = hSU2_input::v_h ;
  
  Particle Y1 = getParticle("Y_1");
  Particle Y2 = getParticle("Y_2");
  Particle Y3 = getParticle("Y_3"); 

  Particle X1 = getParticle("X_1");
  Particle X2 = getParticle("X_2");

  Particle x0 = scalarboson_s("x0 ; x^0", *this); // New scalar physical boson 
  Particle Gxp= scalarboson_s("Gxp; G_x^+", *this); // New goldstone boson.
  Particle Gx0= scalarboson_s("Gx0; G_x^0", *this); // New goldstone boson.

  x0->setSelfConjugate(true);
  Gx0->setSelfConjugate(true);
 
  replace(X1, Gxp());
  replace(X2, (x0() + CSL_I*Gx0() + v_h)/csl::sqrt_s(2));
  promoteToGoldstone("G_x^+","V_1");

  // Particle y1_0 = scalarboson_s("y_1; y_1^0", *this);
  // Particle y2_0 = scalarboson_s("")
}
void hSU2_Model::checkHermiticity(){

}
void hSU2_Model::adjust_fermions(){

}
} // End of namespace mty
