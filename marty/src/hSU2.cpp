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
  
  hSU2_Model::hSU2_Model
    (
      int init,
      std::string const &t_saveFile
     )
    :mty::SM_Model(init), // inherits all SM methods for EWSSB
    saveFile(t_saveFile)
    {
     if (init) {
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
  //    std::cout << "Gathering Model inputs ..." << std::endl;
//      gatherhSU2Inputs();
      std::cout << "Getting to low energy Lagrangian ..." << std::endl;
      getToLowEnergyLagrangian();
      if (save) {
          save << "****************************" << std::endl;
          save << "**    Final Lagrangian    **" << std::endl;
          save << "****************************" << std::endl;
          save << *this << "\n\n";
      }
      std::cout << "Checking Hermiticity ..." << std::endl;
      checkHermiticity();

      computeFeynmanRules();
      if (save) {
          std::ostream &out = save;
          mty::Display(ComputeFeynmanRules(*this), out);
          mty::DisplayAbbreviations(out);
      }
      if (save)
          save.close();
    }
  } // End of base constructor for hSU2 class 

void hSU2_Model::initContent(){
  hSU2_Model::initGauge();
  hSU2_Model::initLeptons();
  hSU2_Model::initQuarks();
  hSU2_Model::initHiggs();
}
void hSU2_Model::initGauge(){
// Init all Gauge and flavoured fields
    addGaugedGroup(mty::group::Type::SU, "C", 3, sm_input::g_s);
    addGaugedGroup(mty::group::Type::SU, "L", 2);
    addGaugedGroup(mty::group::Type::SU, "X", 2);
    addGaugedGroup(mty::group::Type::U1, "Y");
    auto SU3_c = gauge->getGaugedGroup(0);
    auto SU2_L = gauge->getGaugedGroup(1);
    auto SU2_X = gauge->getGaugedGroup(2);
    auto U1_Y  = gauge->getGaugedGroup(3);
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
    auto A  = getParticle("A");
    auto g_s = getScalarCoupling("g_s");
    auto g_L = getScalarCoupling("g");
    auto g_Y = getScalarCoupling("g'");
    auto g_X = getScalarCoupling("g_X");

    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
}
void hSU2_Model::initLeptons(){
  Particle L_L = weylfermion_s("L_L",*this,Chirality::Left);
  Particle l_L = weylfermion_s("l_L",*this,Chirality::Left);

  Particle E_R = weylfermion_s("E_R",*this,Chirality::Right);
  Particle e_R = weylfermion_s("E_R",*this,Chirality::Right);
 // Charge leptons. 
}
void hSU2_Model::initQuarks(){

  Particle Q_L = weylfermion_s("Q_L",*this,Chirality::Left);
  Particle U_R = weylfermion_s("U_R",*this,Chirality::Right);
  Particle D_R = weylfermion_s("D_R",*this,Chirality::Right);

  Particle q_L_0 = weylfermion_s("q_L_0",*this, Chirality::Left);
  Particle u_R_0 = weylfermion_s("u_R_0",*this, Chirality::Right);
  Particle d_R_0 = weylfermion_s("d_R_0",*this, Chirality::Right);

  Q_L->setGroupRep("L", 1); 
  Q_L->setGroupRep("Y", {1, 6});
  Q_L->setGroupRep("H", 1);

  q_L_0->setGroupRep("L",1);
  q_L_0->setGroupRep("Y",{1,6});
  q_L_0->setGroupRep("H",0); // SM quark SU(2)_L doublet

  U_R->setGroupRep("Y", {2, 3});
  D_R->setGroupRep("Y", {-1, 3});
  U_R->setGroupRep("H",1);
  D_R->setGroupRep("H",1);
  u_R_0->setGroupRep("Y", {2, 3});
  d_R_0->setGroupRep("Y", {-1, 3});
  u_R_0->setGroupRep("H",0);
  d_R_0->setGroupRep("H",0);

  addParticle(Q_L);
  addParticle(U_R);
  addParticle(D_R);

  addParticle(q_L_0);
  addParticle(u_R_0);
  addParticle(d_R_0);

  // Breaking muultiplets Needs edit./  
  Particle Psi_uL = weylfermion_s("\\Psi_uL",*this, Chirality::Left);
  Particle Psi_uR = weylfermion_s("\\Psi_uR",*this, Chirality::Right);
  Particle Psi_dL = weylfermion_s("\\Psi_dL",*this, Chirality::Left);
  Particle Psi_dR = weylfermion_s("\\Psi_dR",*this, Chirality::Right);
  Particle phi_1 = scalarboson_s("\\phi_1",*this);
  Particle phi_2 = scalarboson_s("\\phi_2",*this);

  Psi_uL->setGroupRep("L",0);
  Psi_uL->setGroupRep("L",0);
  Psi_uL->setGroupRep("Y",{2,3});
  
  Psi_uR->setGroupRep("H",1);
  Psi_uR->setGroupRep("Y",{2,3});
  Psi_uR->setGroupRep("H",1);

  Psi_dL->setGroupRep("L",0);
  Psi_dL->setGroupRep("Y",{-1,3});
  Psi_dL->setGroupRep("H",1);

  Psi_dR->setGroupRep("L",0);
  Psi_dR->setGroupRep("Y",{-1,3});
  Psi_dR->setGroupRep("H",1);

  phi_1->setGroupRep("L",0);
  phi_1->setGroupRep("Y",0);
  phi_1->setGroupRep("H",1);

  phi_2->setGroupRep("L",0);
  phi_2->setGroupRep("Y",0);
  phi_2->setGroupRep("H",1);

  addParticle(Psi_U);
  addParticle(Psi_D);
  addParticle(phi_1);
  addParticle(phi_2);

}
void hSU2_Model::initHiggs(){
  std::cout << "Define ref to vtable" << std::endl;
}
void hSU2_Model::initInteractions(){
  std::cout << "Define ref to vtable" << std::endl;
}
void hSU2_Model::gatherhSU2Inputs(){
  std::cout << "Define ref to vtable" << std::endl;

}
void hSU2_Model::horizontalSymmetryBreaking(){
  std::cout << "Define ref to vtable" << std::endl;

}
void hSU2_Model::getToLowEnergyLagrangian(){
  std::cout << "Define ref to vtable" << std::endl;

}
void hSU2_Model::checkHermiticity(){
  std::cout << "Define ref to vtable" << std::endl;

}
void hSU2_Model::computeFeynmanRules(){
  std::cout << "Define ref to vtable" << std::endl;

}
void hSU2_Model::adjust_fermions(){
  std::cout << "Define ref to vtable" << std::endl;

}
} // End of namespace mty
