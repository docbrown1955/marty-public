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

/**
 *  @file hSU2.h
 *  @brief  class for BSM models implementing a simiple SU(2) horizontal symmetry.
 *  @author Amine Boussejra
 *  @date : 2022-02-22
 */

#ifndef hSU2_H_INCLUDED
#define hSU2_H_INCLUDED

// #include <csl.h>
// #include "SM.h"
#include "model.h"
#include "hSU2_data.h"
  ////////////////////////////////
  //   namespace containing all hSU2 parameters. They should be public and custom editable.
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
    csl::Expr m_U = csl::constant_s("M_U");
  inline
    csl::Expr m_D = csl::constant_s("M_D");
  inline
    csl::Expr m_L = csl::constant_s("M_L");

  // Mass termes for new gauge bosons 
  inline 
    csl::Expr M_V_1 = csl::constant_s("M_V_1");
    csl::Expr M_V_2 = csl::constant_s("M_V_2");
    csl::Expr M_V_3 = csl::constant_s("M_V_3");
  inline
    csl::Expr k_u0 = csl::constant_s("\\kappa_u0");
  inline
    csl::Expr k_u1 = csl::constant_s("\\kappa_u1");
  inline
    csl::Expr k_u2 = csl::constant_s("\\kappa_u2"); // \tilde{\kappa_u}

  inline
    csl::Expr tk_u0 = csl::constant_s("\\tilde{\\kappa}_u0");
  inline
    csl::Expr tk_u1 = csl::constant_s("\\tilde{\\kappa}_u1");
  inline 
    csl::Expr tk_u2 = csl::constant_s("\\tilde{\\kappa}_u2"); 

  inline
    csl::Expr k_d0 = csl::constant_s("\\kappa_d0");
  inline
    csl::Expr k_d1 = csl::constant_s("\\kappa_d1");
  inline
    csl::Expr k_d2 = csl::constant_s("\\kappa_d2"); // \tilde{\kappa_d}

  inline 
    csl::Expr tk_d0 = csl::constant_s("\\kappa_d0");
  inline
    csl::Expr tk_d1 = csl::constant_s("\\kappa_d1");
  inline
    csl::Expr tk_d2 = csl::constant_s("\\kappa_d2"); // \tilde{\kappa_d}

  inline
    csl::Expr k_l0 = csl::constant_s("\\kappa_l0");
  inline
    csl::Expr k_l1 = csl::constant_s("\\kappa_l1");
  inline
    csl::Expr k_l2 = csl::constant_s("\\kappa_l2"); // \tilde{\kappa_l}

  inline
    csl::Expr tk_l0 = csl::constant_s("\\kappa_l0");
  inline
    csl::Expr tk_l1 = csl::constant_s("\\kappa_l1");
  inline
    csl::Expr tk_l2 = csl::constant_s("\\kappa_l2"); // \tilde{\kappa_l}

#define MTY_CST(arg) csl::constant_s(arg)
#define MTY_TVEC(name) \
  csl::matrix_s({\
      {MTY_CST(#name"_0"), MTY_CST(#name"_1"), MTY_CST(#name"_2")},\
      })
  /**
   * @brief Tricoupling Psi-X_uR vector 
   */
  inline csl::Expr K_u = MTY_TVEC(K_u);
  /**
   * @brief Adjoint Tricoupling Psi-X_uR vector 
   */
  inline csl::Expr tK_u = MTY_TVEC(tK_u);
  /**
   * @brief Tricoupling Psi-X_dR vector 
   */
  inline csl::Expr K_d = MTY_TVEC(K_d);
  /**
   * @brief Adjoint Tricoupling Psi-X_dR vector 
   */
  inline csl::Expr tK_d = MTY_TVEC(tK_d);
  /**
   * @brief Tricoupling Psi-X_lR vector 
   */
  inline csl::Expr K_l = MTY_TVEC(K_l);
  /**
   * @brief Adjoint Tricoupling Psi-X_lR vector 
   */
  inline csl::Expr tK_l = MTY_TVEC(tK_l);

#undef MTY_CST 
#undef MTY_TVEC 
}

namespace mty{
/* 
 * @brief Base class for simple hSU2 model. 
 * WIP : will contain any arbitrary horizontal symmetry group on top  of the SM gauge groups ?
 */

  class hSU2_Model: public mty::Model, protected hSU2_Data{

    public:
       hSU2_Model(); // default constructor
      hSU2_Model(
          std::string const &saveFile
          );

      ~hSU2_Model();

    protected:
      void initContent(); // Encapsulates the following methods
        void  initGauge();
        void  initLeptons();
        void  initQuarks();

        void initInteractions(); 
        void gatherhSU2Inputs();
        void initHiggs();

      void getToLowEnergyLagrangian();
        void horizontalSymmetryBreaking();
        void expandAroundVEVs();
        void setGaugeBosonsMass();
        void breakSMSymmetry();
          void rotateFermions();
          void adjutSM();
          void addGaugeSMFixingTerms();
        void checkHermiticity();
        void computeFeynmanRules();
        void adjust_fermions(); // ?


    private:
      std::string saveFile;
  }; /* mty::hSU2 class */
  
} /* end of namespace mty */  




#endif 
