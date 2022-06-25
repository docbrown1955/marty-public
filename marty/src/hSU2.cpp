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

    std::cout << "Finalizing Model building ..." << std::endl;
    refresh();

    std::cout << "Computing Feynman Rules ..." << std::endl;
    Model::computeFeynmanRules();

    if (save) {
      save << "****************************" << std::endl;
      save << "**    Final Lagrangian    **" << std::endl;
      save << "****************************" << std::endl;
      save << *this << "\n\n";
    }
    std::cout << "Checking Hermiticity ..." << std::endl;
    // checkHermiticity();

    if (save) {
      std::ostream &out = save;
      mty::Display(getFeynmanRules(), out);
      mty::DisplayAbbreviations(out);
      save.close();
    }

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
    B  = getParticle("B");
    Wi = getParticle("W");
    G  = getParticle("G");
    V  = getParticle("V");
    g_s = getScalarCoupling("g_s");
    g_L = getScalarCoupling("g");
    g_Y = getScalarCoupling("g_Y");
    g_X = getScalarCoupling("g_X");

    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
}
void hSU2_Model::initLeptons(){
  std::cout << "Initializing Leptons" << std::endl;
  Li = weylfermion_s("L_L",*this,Chirality::Left);
  L0 = weylfermion_s("l_L",*this,Chirality::Left);

  E0 = weylfermion_s("e_R_0",*this,Chirality::Right);
  E1 = weylfermion_s("e_R_1",*this,Chirality::Right);
  E2 = weylfermion_s("e_R_2",*this,Chirality::Right);

  Psi_lL = weylfermion_s("Psi_lL", *this, Chirality::Left);
  Psi_lR = weylfermion_s("Psi_lR", *this, Chirality::Right);
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
  Qi = weylfermion_s("Q_L",*this,Chirality::Left);
  Q0 = weylfermion_s("q_L",*this, Chirality::Left);
  
  U0 = weylfermion_s("u_R_0",*this, Chirality::Right);
  D0 = weylfermion_s("d_R_0",*this, Chirality::Right);
  
  U1 = weylfermion_s("u_R_1",*this, Chirality::Right);
  D1 = weylfermion_s("d_R_1",*this, Chirality::Right);
  
  U2 = weylfermion_s("u_R_2",*this, Chirality::Right);
  D2 = weylfermion_s("d_R_2",*this, Chirality::Right);

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

  Psi_uL = weylfermion_s("Psi_uL",*this, Chirality::Left);
  Psi_uR = weylfermion_s("Psi_uR",*this, Chirality::Right);
  Psi_dL = weylfermion_s("Psi_dL",*this, Chirality::Left);
  Psi_dR = weylfermion_s("Psi_dR",*this, Chirality::Right);
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
  addParticle(H); // 

  csl::Index i = GaugeIndex(*this, "L", H);
  csl::Index j = GaugeIndex(*this, "L", H);
  csl::Expr v = sm_input::v;
  csl::Expr mh = sm_input::m_h;
  csl::Expr H2 = csl::GetComplexConjugate(H(i)) * H(i);
  csl::Expr m2 = mh*mh / 2;
  csl::Expr lam = mh*mh / (2*v*v);

  // Mexican hat potential
  addLagrangianTerm(m2*H2);
  addLagrangianTerm(-lam*csl::pow_s(H2, 2));

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
  csl::Tensor eps = getVectorSpace("L", "Q_L")->getEpsilon(); // = i * sigma^2
  csl::Tensor eps_h = getVectorSpace("X", "Q_L")->getEpsilon(); // = i * sigma^2

  Expr y_u = constant_s("y_u");
  Expr lambda_u = constant_s("lam_u");
  Expr tilde_lambda_u = constant_s("tlam_u");
 
  Expr y_d = constant_s("y_d");
  Expr lambda_d = constant_s("lam_d");
  Expr tilde_lambda_d = constant_s("tlam_d");
  
  Expr y_l = constant_s("y_l");
  Expr lambda_l = constant_s("lam_l");
  Expr tilde_lambda_l = constant_s("tlam_l");

  // Interaction terms : 
  // up sector
  addLagrangianTerm(
      - y_u 
      * GetComplexConjugate(Qi({a,il[0],I, al}))
      * eps({il[0],il[1]})
      * GetComplexConjugate(H(il[1]))
      * Psi_uR({a,I,al}),
      true); // Add also the complex conjugate of this term

  addLagrangianTerm(
      - lambda_u 
      * GetComplexConjugate(Psi_uL({a,ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_uR({a,ih[1],al})
      ,true);

  addLagrangianTerm(
      - tilde_lambda_u
      * GetComplexConjugate(Psi_uL({a,ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_uR({a,ih[1],al})
      ,true );

  Expr y_u0 = constant_s("y_u0");
  addLagrangianTerm(
      - y_u0 
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
        - K_u[a_u]
        * GetComplexConjugate(Psi_uL({a,I,al}))
        * X(I)
        * U_R[a_u]({a,al})
        ,true);
    addLagrangianTerm(
        - tK_u[a_u]
        * GetComplexConjugate(Psi_uL({a,ih[0],al}))
        * eps_h({ih[0],ih[1]})
        * GetComplexConjugate(X(ih[1]))
        * U_R[a_u]({a,al})
        ,true);
  }
  std::cout << "Up quark sector interactions complete..." << std::endl;
  // Down sector 
  // No charge conjugation on Higgs
  addLagrangianTerm(
      - y_d
      * GetComplexConjugate(Qi({a,Il,I, al}))
     // * eps({il[0],il[1]})
      * H(Il)
      * Psi_dR({a,I,al}),
      true); // Add also the complex conjugate of this term
  addLagrangianTerm( 
      - lambda_d 
      * GetComplexConjugate(Psi_dL({a,ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_dR({a,ih[1],al})
      ,true);

  addLagrangianTerm(
      - tilde_lambda_d
      * GetComplexConjugate(Psi_dL({a,ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_dR({a,ih[1],al})
      ,true );

  Expr y_d0 = constant_s("y_d0");
  addLagrangianTerm(
      - y_d0 
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
        - K_d[a_d]
        * GetComplexConjugate(Psi_dL({a,I,al}))
        * X(I)
        * D_R[a_d]({a,al})
        ,true);
    addLagrangianTerm(
        - tK_d[a_d]
        * GetComplexConjugate(Psi_dL({a,ih[0],al}))
        * eps_h({ih[0],ih[1]})
        * GetComplexConjugate(X(ih[1]))
        * D_R[a_d]({a,al})
        ,true);
  }
  // Lepton sector
  addLagrangianTerm(
      - y_l
      * GetComplexConjugate(Li({Il,I, al}))
      * H(Il)
      * Psi_lR({I,al}),
      true); // Add also the complex conjugate of this term

  addLagrangianTerm(
      - lambda_l 
      * GetComplexConjugate(Psi_lL({ih[0],al})) 
      * Y(A)
      * half_sigma({A, ih[0], ih[1]}) 
      * Psi_lR({ih[1],al})
      ,true);

  addLagrangianTerm(
      - tilde_lambda_l
      * GetComplexConjugate(Psi_lL({ih[0],al})) 
      * eps({il[0],il[1]})
      * Y(A)
      * eps({il[0],il[1]}) // = CSL_I * Sigma(2)
      * half_sigma({A,ih[0],ih[1]})
      * Psi_lR({ih[1],al})
      ,true );

  Expr y_l0 = constant_s("y_l0");
  addLagrangianTerm(
      - y_l0
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
        - K_l[a_l]
        * GetComplexConjugate(Psi_lL({J,al}))
        * X(J)
        * E_R[a_l]({al})
        ,true);
    addLagrangianTerm(
        - tK_l[a_l]
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
       - m_psi_u 
       * GetComplexConjugate(Psi_uL({a,J,al}))
       * Psi_uR({a,J,al})
       , true);
  addLagrangianTerm(
       - m_psi_d
       * GetComplexConjugate(Psi_dL({a,J,al}))
       * Psi_dR({a,J,al})
       , true);
  addLagrangianTerm(
       - m_psi_l 
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
     std::cout << "Breaking SM SU(2)_L x U(1)_Y symmetry" << std::endl;
     breakSMSymmetry();
     std::cout << "Mixing fermions..." << std::endl;
     rotateFermions();
     std::cout << "Treating Yukawa sector" << std::endl;
     replaceUpYukawa();
     replaceLeptonYukawa();
     replaceDownYukawa();
     std::cout << "Adjusting Gauge..." << std::endl;
     adjustSM();
    
}
void hSU2_Model::horizontalSymmetryBreaking(){
    ////////////////////////////////////////////////
    // Horizontal symmetry breaking 
    ///////////////////////////////////////////////

     BreakGaugeSymmetry(*this, "X");
     expandAroundVEVs();
     setGaugeBosonsMass();
}

void hSU2_Model::expandAroundVEVs(){
  csl::Expr v_x = hSU2_input::v_x ;
  csl::Expr v_y = hSU2_input::v_y ;
  
  Particle Y1 = getParticle("Y_1");
  Particle Y2 = getParticle("Y_2");
  Particle Y3 = getParticle("Y_3"); 

  Particle X1 = getParticle("X_1");
  Particle X2 = getParticle("X_2");

  Particle x0 = scalarboson_s("x0 ; x^0", *this); // New scalar physical boson 
  Particle Gxp= scalarboson_s("Gxp; G_x^+", *this); // New goldstone boson.
  Particle Gx0= scalarboson_s("Gx0; G_x^0", *this); // New goldstone boson.
  Particle y0 = scalarboson_s("y0 ; y^0", *this); // New scalar boson 

  x0->setSelfConjugate(true);
  y0->setSelfConjugate(true);
  Gx0->setSelfConjugate(true);
 
  replace(X1, CSL_0);
  replace(X2, (x0() + v_x)/csl::sqrt_s(2));

  Replaced(*this,
      Y3,
      (v_y + y0)/csl::sqrt_s(2)
      );
  Replaced(*this,
      Y1,
      CSL_0);
  Replaced(*this,
      Y2,
      CSL_0);
  // Particle y1_0 = scalarboson_s("y_1; y_1^0", *this);
  // Particle y2_0 = scalarboson_s("")
}
void hSU2_Model::setGaugeBosonsMass(){
  
     Particle V_1 = getParticle("V_1");
     Particle V_2 = getParticle("V_2");
     Particle V_3 = getParticle("V_3");

    // Replaced(*this,
        // V_1->getMass(),
        // hSU2_input::M_V_1);
    // Replaced(*this,
        // V_2->getMass(),
        // hSU2_input::M_V_2);
    // Replaced(*this,
        // V_3->getMass(),
        // hSU2_input::M_V_3);
     rotateFields({V_1,V_2,V_3},true);
     SetGaugeChoice(V_1,gauge::Unitary);
     SetGaugeChoice(V_2,gauge::Unitary);
     SetGaugeChoice(V_3,gauge::Unitary);
}

void hSU2_Model::breakSMSymmetry(){ 
    ///////////////////////////////////////////////////
    // Breaking gauge SU(2)_L symmetry, renaming
    ///////////////////////////////////////////////////

    BreakGaugeSymmetry(*this, "Y");
    BreakGaugeSymmetry(*this, "L");
    
    ///////////////////////////////////////////////////
    // Replacements to get SM particles W +-
    ///////////////////////////////////////////////////

    Particle W1   = GetParticle(*this, "W_1");
    Particle W2   = GetParticle(*this, "W_2");
    Particle W_SM = W1->generateSimilar("W");
    W_SM->setSelfConjugate(false);

    Particle cW1 = getParticle("c_W_1");
    Particle cW2 = getParticle("c_W_2");
    Particle cWp = W_SM->getGhostBoson();
    cWp->setName("c_Wp ; c_{+}");
    Particle cWm = ghostboson_s("c_Wm; c_{-}", W_SM, true);
    W_SM->setConjugatedGhostBoson(cWm);

    csl::Index mu = MinkowskiIndex();
    csl::Index nu = MinkowskiIndex();
    csl::Expr W_p = W_SM(+mu);
    csl::Expr W_m = csl::GetComplexConjugate(W_SM(+mu));
    csl::Expr F_W_p = W_SM({+mu,+nu});
    csl::Expr F_W_m = csl::GetComplexConjugate(W_SM({+mu, +nu}));

    auto W1_expr = [](csl::Expr const &Wp, csl::Expr const &Wm) {
        return (Wp + Wm) / csl::sqrt_s(2);
    };
    auto W2_expr = [](csl::Expr const &Wp, csl::Expr const &Wm) {
        return CSL_I * (Wp - Wm) / csl::sqrt_s(2);
    };
    replace(W1, W1_expr(W_p, W_m));
    replace(W2, W2_expr(W_p, W_m));
    replace(W1->getFieldStrength(), W1_expr(F_W_p, F_W_m));
    replace(W2->getFieldStrength(), W2_expr(F_W_p, F_W_m));
    replace(cW1, W1_expr(cWp, cWm));
    replace(cW2, W2_expr(cWp, cWm));

    ///////////////////////////////////////////////////
    // Actual gauge (spontaneous) symmetry breaking
    ///////////////////////////////////////////////////

    csl::Expr v = sm_input::v;

    Particle H1 = getParticle("H_1");
    Particle H2 = getParticle("H_2");

    Particle h0 = scalarboson_s("h0 ; h^0", *this); // SM Higgs boson
    Particle Gp = scalarboson_s("Gp ; G^+", *this);
    Particle G0 = scalarboson_s("G0 ; G^0", *this);
    h0->setSelfConjugate(true);
    G0->setSelfConjugate(true);

    replace(H1, Gp());
    replace(H2, (h0() + CSL_I*G0() + v)/csl::sqrt_s(2));

    ///////////////////////////////////////////////////
    // Diagonalizing what can be
    ///////////////////////////////////////////////////

    diagonalizeMassMatrices();
    renameParticle("B", "A");
    renameParticle("W_3", "Z");
    gatherMass("W");
    gatherMass("Z");

    // csl::Expr g_Y = getScalarCoupling("g_Y");
    // csl::Expr g_L = getScalarCoupling("g_L");
    csl::Expr theta_Weinberg = sm_input::theta_W;
    csl::Expr e  = sm_input::e_em;

    replace(
            g_L*g_L + g_Y*g_Y,
            csl::pow_s(g_L/csl::cos_s(theta_Weinberg), csl::int_s(2)));
    replace(
            g_Y, 
            e / csl::cos_s(theta_Weinberg));
    replace(
            g_L, 
            e / csl::sin_s(theta_Weinberg));
}

void hSU2_Model::adjustSM(){

    using namespace sm_input;
    replace(v, (2 * M_W * csl::sin_s(theta_W)) / e_em);
    replace(getParticle("W")->getMass(), M_W);
    getParticle("W")->setMass(M_W);
    replace(getParticle("Z")->getMass(), M_Z);
    getParticle("Z")->setMass(M_Z);
    promoteToGoldstone("Gp", "W");
    promoteToGoldstone("G0", "Z");
    addGaugeSMFixingTerms();

}
void hSU2_Model::addGaugeSMFixingTerms(){

    using namespace sm_input;
    auto cc = [](csl::Expr const &expr) { return GetComplexConjugate(expr); };

    csl::Expr cosW = csl::cos_s(theta_W);
    csl::Expr g = e_em / csl::sin_s(theta_W);
    Particle h0 = getParticle("h0");
    Particle G0 = getParticle("G0");
    Particle Gp = getParticle("Gp");
    csl::Expr ap = csl::constant_s("ap");
    csl::Expr am = csl::constant_s("am");
    csl::Expr az = csl::constant_s("az");
    csl::Expr aa = csl::constant_s("aa");
    csl::Expr delta_G0 = -g/2 * (am*Gp + ap*cc(Gp))
        + g/(2*cosW) * az * (v + h0);
    csl::Expr delta_Gp = -CSL_I*g/2 * (v + h0 + CSL_I*G0)*ap
        - CSL_I*g*csl::cos_s(2*theta_W)/(2*cosW) * Gp*az
        + CSL_I*e_em*Gp*aa;
    csl::Expr delta_Gm = cc(Replaced(delta_Gp, ap, am));
    Particle W = getParticle("W");
    Particle Z = getParticle("Z");
    Particle A = getParticle("A");
    csl::Expr xi_W = W->getGaugeChoice().getXi();
    csl::Expr xi_Z = Z->getGaugeChoice().getXi();

    csl::Expr dF_Wp = -CSL_I*M_W*xi_W * delta_Gp;
    csl::Expr dF_Wm =  CSL_I*M_W*xi_W * delta_Gm;
    csl::Expr dF_Z  = -M_Z*xi_Z * delta_G0;

    csl::Expr c_A  = getParticle("c_A");
    csl::Expr c_Wp = getParticle("c_Wp");
    csl::Expr c_Wm = getParticle("c_Wm");
    csl::Expr c_Z  = getParticle("c_Z");
    for (const auto &[c1, delta] : std::array {
            std::pair {c_Z,  dF_Z},
            std::pair {c_Wp, dF_Wp},
            std::pair {c_Wm, dF_Wm}
            }) 
    {
        for (const auto &[a, c2] : std::array {
                std::pair {aa, c_A},
                std::pair {az, c_Z},
                std::pair {ap, c_Wp},
                std::pair {am, c_Wm}
                })
        {
            csl::Expr term = delta->derive(a.get()).value();
            if (term != CSL_0) {
                addLagrangianTerm(cc(c1) * term * c2);
            }
        }
    }
}
void hSU2_Model::rotateFermions(){
    Particle Q_0_1 = getParticle("q_L_1");
    Particle Q_1_1 = getParticle("Q_L_1_1");
    Particle Q_2_1 = getParticle("Q_L_2_1");

    Particle U_0 = getParticle("u_R_0");
    Particle U_1 = getParticle("u_R_1");
    Particle U_2 = getParticle("u_R_2");
    
    Particle Psi_uL_1 = getParticle("Psi_uL_1");
    Particle Psi_uR_1 = getParticle("Psi_uR_1");
    Particle Psi_uL_2 = getParticle("Psi_uL_2");
    Particle Psi_uR_2 = getParticle("Psi_uR_2");

    birotateFields(
        {Q_1_1, Q_2_1, Q_0_1, Psi_uL_1, Psi_uL_2},
        {U_1, U_2, U_0, Psi_uR_1, Psi_uR_2},
        true);

    Particle Q_0_2 = getParticle("q_L_2");
    Particle Q_1_2 = getParticle("Q_L_1_2");
    Particle Q_2_2 = getParticle("Q_L_2_2");

    Particle D_0 = getParticle("d_R_0");
    Particle D_1 = getParticle("d_R_1");
    Particle D_2 = getParticle("d_R_2");
    
    Particle Psi_dL_1 = getParticle("Psi_dL_1");
    Particle Psi_dR_1 = getParticle("Psi_dR_1");
    Particle Psi_dL_2 = getParticle("Psi_dL_2");
    Particle Psi_dR_2 = getParticle("Psi_dR_2");

    birotateFields(
        {Q_1_2, Q_2_2, Q_0_2, Psi_dL_1, Psi_dL_2},
        {D_1, D_2, D_0, Psi_dR_1, Psi_dR_2},
        true);
    Particle L_0_2 = getParticle("l_L_2");
    Particle L_1_2 = getParticle("L_L_1_2");
    Particle L_2_2 = getParticle("L_L_2_2");
   
    Particle E_0 = getParticle("e_R_0");
    Particle E_1 = getParticle("e_R_1");
    Particle E_2 = getParticle("e_R_2");

    Particle Psi_lL_1 = getParticle("Psi_lL_1");
    Particle Psi_lR_1 = getParticle("Psi_lR_1");
    Particle Psi_lL_2 = getParticle("Psi_lL_2");
    Particle Psi_lR_2 = getParticle("Psi_lR_2");

    birotateFields(
        {L_1_2, L_2_2, L_0_2, Psi_lL_1, Psi_lL_2},
        {E_1, E_2, E_0, Psi_lR_1, Psi_lR_2},
        true);
   static const std::vector<std::pair<std::string,std::string>> names = {
     {"q_L_1","t_L"}, {"q_L_2","b_L"},
     {"u_R_0", "t_R"}, {"d_R_0","b_R"},
     {"d_R_1", "d_R1 ; {d_R}_1"}, {"u_R_1","u_R1 ; {u_R}_1"},
     {"d_R_2", "d_R2 ; {d_R}_2"}, {"u_R_2","u_R2 ; {u_R}_2"},
     {"Q_L_1_1","u_L1 ; {u_L}_1"}, {"Q_L_1_2", "d_L1 ; {d_L}_1"},
     {"Q_L_2_1","u_L2 ; {u_L}_2"}, {"Q_L_2_2", "d_L2 ; {d_L}_2"},
     {"l_L_1","nut ; \\nu_{\\tau}"},{"l_L_2","tau_L ; \\tau_L"},
     {"L_L_1_1","nu1 ; \\nu_1"}, {"L_L_1_2","l_1"},
     {"L_L_2_1","nu2 ; \\nu_2"}, {"L_L_2_2","l_2"},
     {"e_R_0","tau_R ; \\tau_R"},{"e_R_1","e_R1 ; {e_R}_1"},{"e_R_2","e_R2 ; {e_R}_2"}
   };
   for (const auto [previous, next]: names)
     renameParticle(previous,next);
}
// void hSU2_Model::checkHermiticity(){
//
// }
// void hSU2_Model::adjust_fermions(){
//
// }

void hSU2_Model::replaceUpYukawa()
{
    // csl::Tensor Yu = GetYukawa(*this, "Yu");
    csl::Tensor Yu("Yu",{&Euclid_R3,&Euclid_R3});
    const csl::Space* flavorSpace = GetSpace(Yu);
    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
    csl::Expr v = sm_input::v ;
    csl::Expr factor = csl::sqrt_s(2) / v;
    csl::Expr m_u   = sm_input::m_u;
    csl::Expr m_c   = sm_input::m_c;
    csl::Expr m_t   = sm_input::m_t;
    csl::Tensor M_u = csl::tensor_s(
            "M_u",
            {flavorSpace, flavorSpace},
            csl::matrix_s({{m_u, CSL_0, CSL_0},
                      {CSL_0, m_c, CSL_0},
                      {CSL_0, CSL_0, m_t}}));

    replace(Yu, factor*M_u({f_i, f_j}));
}
void hSU2_Model::replaceDownYukawa()
{
    csl::Tensor Yd("Yd", {&Euclid_R3,&Euclid_R3});
    const csl::Space* flavorSpace = GetSpace(Yd);
    csl::Expr v = sm_input::v;
    csl::Expr factor = csl::sqrt_s(2) / v;
    csl::Expr m_d   = sm_input::m_d;
    csl::Expr m_s   = sm_input::m_s;
    csl::Expr m_b   = sm_input::m_b;
    csl::Tensor M_d = csl::tensor_s(
            "M_d",
            {flavorSpace, flavorSpace},
            csl::matrix_s({{m_d, CSL_0, CSL_0},
                      {CSL_0, m_s, CSL_0},
                      {CSL_0, CSL_0, m_b}}));


    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
    csl::Index f_k = GetIndex(flavorSpace);

    buildCKM(flavorSpace);

    replace(Yd({f_i, f_j}),
            csl::prod_s({factor,
                         V_CKM({f_i, f_k}), 
                         M_d({f_k, f_j})}, 
                         true));

    
  birotateFields({"d_L1", "d_L2", "b_L"}, {"d_R1", "d_R2", "b_R"});
}
void hSU2_Model::replaceLeptonYukawa()
{
    ///////////////////////////////////////////////////
    // Taking care of Yukawa couplings
    ///////////////////////////////////////////////////

    csl::Tensor Ye("Ye",{&Euclid_R3,&Euclid_R3});

    csl::Expr m_e   = sm_input::m_e;
    csl::Expr m_mu  = sm_input::m_mu;
    csl::Expr m_tau = sm_input::m_tau;

    const csl::Space* flavorSpace = GetSpace(Ye);
    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
    csl::Expr v = sm_input::v; 
    csl::Expr factor = csl::sqrt_s(2) / v;
    csl::Tensor M_e = csl::tensor_s(
            "M_e",
            {flavorSpace, flavorSpace},
            csl::matrix_s({{m_e, CSL_0, CSL_0},
                      {CSL_0, m_mu, CSL_0},
                      {CSL_0, CSL_0, m_tau}}));

    replace(Ye, factor*M_e({f_i, f_j}));
}
} // End of namespace mty
