// First test code for horizontal SU(2) flavour symmetries. 
//
// Created : 07/02/22

#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

std::string path_to_generated_library = ".";

int main(){
  
  // Model building;
  
  Model horizSU2;
  // SM  
  horizSU2.addGaugedGroup(group::Type::SU,"C",3); // SU(3)_c
  horizSU2.addGaugedGroup(group::Type::SU,"L",2); // SU(2)_L 
  horizSU2.addGaugedGroup(group::Type::U1,"Y");   // U(1)_Y
  // BSM 
  horizSU2.addGaugedGroup(group::Type::SU,"H",2); // SU(2)_h

  horizSU2.init();
  
  horizSU2.renameParticle("A_L", "W"); // W_i
  horizSU2.renameParticle("A_Y", "B"); // B
// SU(3)_c is completely deconnected. Should wait
//  horizSU2.renameParticle("A_C", "G"); // gluon 

  // N.B. : c_A_grp = ghost boson of group "grp"

  Particle Q_L = weylfermion_s("Q_L", horizSU2, Chirality::Left);
  Particle u_R = weylfermion_s("u_R", horizSU2, Chirality::Right);
  Particle d_R = weylfermion_s("d_R", horizSU2, Chirality::Right);

  Particle q_L_0 = weylfermion_s("q_L_0",horizSU2, Chirality::Left);
  Particle u_R_0 = weylfermion_s("u_R_0",horizSU2, Chirality::Right);
  Particle d_R_0 = weylfermion_s("d_R_0",horizSU2, Chirality::Right);

  Q_L->setGroupRep("L", 1); 
  Q_L->setGroupRep("Y", {1, 6});
  Q_L->setGroupRep("H", 1);

  q_L_0->setGroupRep("L",1);
  q_L_0->setGroupRep("Y",{1,6});
  q_L_0->setGroupRep("H",0); // SM quark SU(2)_L doublet

  u_R->setGroupRep("Y", {2, 3});
  d_R->setGroupRep("Y", {-1, 3});
  u_R->setGroupRep("H",1);
  d_R->setGroupRep("H",1);
  u_R_0->setGroupRep("Y", {2, 3});
  d_R_0->setGroupRep("Y", {-1, 3});
  u_R_0->setGroupRep("H",0);
  d_R_0->setGroupRep("H",0);

  horizSU2.addParticle(Q_L);
  horizSU2.addParticle(u_R);
  horizSU2.addParticle(d_R);

  horizSU2.addParticle(q_L_0);
  horizSU2.addParticle(u_R_0);
  horizSU2.addParticle(d_R_0);

  // Breaking muultiplets 
  Particle Psi_U = vectorboson_s("Psi_U",horizSU2);
  Particle Psi_D = vectorboson_s("Psi_D",horizSU2);
  Particle phi_1 = scalarboson_s("phi_1",horizSU2);
  Particle phi_2 = scalarboson_s("phi_2",horizSU2);

  Psi_U->setGroupRep("L",0);
  Psi_U->setGroupRep("Y",{2,3});
  Psi_U->setGroupRep("H",1);

  Psi_D->setGroupRep("L",0);
  Psi_D->setGroupRep("Y",{-1,3});
  Psi_D->setGroupRep("H",1);

  phi_1->setGroupRep("L",0);
  phi_1->setGroupRep("Y",0);
  phi_1->setGroupRep("H",1);

  phi_2->setGroupRep("L",0);
  phi_2->setGroupRep("Y",0);
  phi_2->setGroupRep("H",1);

  horizSU2.addParticle(Psi_U);
  horizSU2.addParticle(Psi_D);
  horizSU2.addParticle(phi_1);
  horizSU2.addParticle(phi_2);
  
  horizSU2.breakGaugeSymmetry("H");
  
 //Particle A_H_1 = horizSU2.getParticle("A_H_1") ;
 //Particle A_H_2 = horizSU2.getParticle("A_H_2") ;
 //Particle A_H_3 = horizSU2.getParticle("A_H_3") ;

  
  cout << horizSU2 << endl ;

  return 0;
}
