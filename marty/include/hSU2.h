/// class for BSM models implementing a simiple SU(2) horizontal symmetry.
// created : 22/02/22
// last edited : 22/02/22

#ifndef hSU2_H_INCLUDED
#define hSU2_H_INCLUDED

#include <csl.h>
#include "SM.h"
#include "model.h"

namespace mty{
/* hSU2 master class */

  class hSU2_Model: public mty::SM_Model{
   
    public:
      hSU2_Model(
          int init = 1,
          std::string const &saveFile = "saveFile.out"
          );

      ~hSU2_Model();

    protected: 
      void initContent(); // → Encapsulates the following methods
        void  initGauge();
        void  initLeptons();
        void  initQuarks();

      void initInteractions();
        void gatherhSU2Inputs();
        void horizontalSymmetryBreaking();
        void initHiggs();
        void getToLowEnergyLagrangian();
        void checkHermiticity();
        void computeFeynmanRules();
        void adjust_fermions(); // ?
     

    private:
      std::string saveFile;
  };
}  // end of hSU2
////////////////////////////////
//  namespace containing all hSU2 parameters. They should be public and custom editable.
///////////////////////////////

namespace mty::hSU2_input{
  
  /// hSU2 inputs  ? Should define a clean way to read and sample them. 

  inline 
    csl::Expr v_x = csl::constant_s("v_x"); // X spurion vev

  inline 
    csl::Expr v_y = csl::constant_s("v_y"); // Y spurion vev  

  inline 
    csl::Expr y_u0 = csl::constant_s("y_u0"); // yukawa for Q_0 u_r_a interaction
  inline 
    csl::Expr y_u = csl::constant_s("y_u"); // yukawa for \Psi_i Q_i interaction
  inline 
    csl::Expr y_d0 = csl::constant_s("y_d0"); // ydkawa for Q_0 d_r_a interaction
  inline 
    csl::Expr y_d = csl::constant_s("y_d"); // ydkawa for \Psi_i Q_i interaction
  inline 
    csl::Expr y_l0 = csl::constant_s("y_l0"); // ylkawa for Q_0 l_r_a interaction
  inline 
    csl::Expr y_l = csl::constant_s("y_l"); // ylkawa for \Psi_i Q_i interaction
  
  // Yukawa for vector like 
  inline 
    csl::Expr lambda_u1 = csl::constant_s("\\lambda_u1");
  inline 
    csl::Expr lambda_u2 = csl::constant_s("\\lambda_u2");
  inline 
    csl::Expr lambda_d1 = csl::constant_s("\\lambda_d1");
  inline 
    csl::Expr lambda_d2 = csl::constant_s("\\lambda_d2");
  inline 
    csl::Expr lambda_l1 = csl::constant_s("\\lambda_l1");
  inline 
    csl::Expr lambda_l2 = csl::constant_s("\\lambda_l2");

  // Mass terms for vector like fermions. Redundant parametrization : M_U_i = \lambda_Ui * v_y
  inline 
    csl::Expr M_U = csl::constant_s("M_U");
  inline
    csl::Expr M_D = csl::constant_s("M_D");
  inline
    csl::Expr M_L = csl::constant_s("M_L");

  inline 
    csl::Expr k_u0 = csl::constant_s("\\kappa_u0");
  inline
    csl::Expr k_u1 = csl::constant_s("\\kappa_u1");
  inline 
    csl::Expr k_u2 = csl::constant_s("\\kappa_u2"); // \tilde{\kappa_u} 


  inline 
    csl::Expr k_d0 = csl::constant_s("\\kappa_d0");
  inline
    csl::Expr k_d1 = csl::constant_s("\\kappa_d1");
  inline 
    csl::Expr k_d2 = csl::constant_s("\\kappa_d2"); // \tilde{\kappa_d} 


  inline 
    csl::Expr k_l0 = csl::constant_s("\\kappa_l0");
  inline
    csl::Expr k_l1 = csl::constant_s("\\kappa_l1");
  inline 
    csl::Expr k_l2 = csl::constant_s("\\kappa_l2"); // \tilde{\kappa_l}


}

#endif /* class hSU2_Model */
