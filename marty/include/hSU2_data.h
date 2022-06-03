#include "quantumField.h" 


struct hSU2_Data{


    ///////////////////////////////////////////////////
    // Gauge sector
    ///////////////////////////////////////////////////

    mty::GaugedGroup * SU3_c;
    mty::GaugedGroup * SU2_L;
    mty::GaugedGroup * U1_Y;
    mty::GaugedGroup * SU2_X;

    mty::Particle G; // Gluon 
    mty::Particle Wi; // SU(2)L boson
    mty::Particle B; // Hypercharge boson
  
    mty::Particle V; // SU(2)X boson

    csl::Expr g_s; // SU(3)_C coupling constant
    csl::Expr g_L; // SU(2)_L coupling constant
    csl::Expr g_L; // U(1)_Y coupling constant
    csl::Expr g_X; // SU(2)_X coupling constant 


    ////////////////////////////////////////////////////
    // Final fermion masses 
    ///////////////////////////////////////////////////
    csl::Expr m_e; 
    csl::Expr m_mu; 
    csl::Expr m_tau;
    csl::Expr m_u;  
    csl::Expr m_c;  
    csl::Expr m_t;  
    csl::Expr m_d;  
    csl::Expr m_s;      
    csl::Expr m_b; 
    
    // Additional vector-like masses.
    csl::Expr m_psi_uL;
    csl::Expr m_psi_uR;
    csl::Expr m_psi_dL;
    csl::Expr m_psi_dR;
    csl::Expr m_psi_lL;
    csl::Expr m_psi_lR; 

    /*************************************************
     * In the following we take the convention : 
     *  fi → SU(2)X doublet for the fermion type f
     *  f0 → SU(2))X singlet for the fermion type f
     * ***********************************************/

    ///////////////////////////////////////////////////
    // Lepton sector
    ///////////////////////////////////////////////////

    mty::Particle Li; // Left-handed neutrinos+leptons
    mty::Particle L0; // Left-handed neutrinos+leptons
    
    mty::Particle Ei; // Right-handed leptons
    mty::Particle E0; // Right-handed leptons 

    csl::Tensor Ye; // Lepton SM Yukawa coupling 
    csl::Tensor ye; // Lepton SU(2)_X Yukawa coupling
    csl::Tensor Ke; // Lepton Trilinear coupling (Psi-X-l)
   
    ///////////////////////////////////////////////////
    // Quark sector
    ///////////////////////////////////////////////////

    mty::Particle Qi; // Left-handed quarks
    mty::Particle Q0; // Left-handed quarks

    mty::Particle Ui; // Right-handed up-type quarks
    mty::Particle Di; // Right-handed down-type quarks
    mty::Particle U0; // Right-handed up-type quarks
    mty::Particle D0; // Right-handed down-type quarks

 
    csl::Tensor Yu; // Up-type Yukawa coupling
    csl::Tensor yu; // Up-type SU(2)_X Yukawa coupling
    csl::Tensor Yd; // Down-type Yukawa coupling
    csl::Tensor yd; // Down-type SU(2)_X Yukawa coupling
    csl::Tensor Ku; // Up-type trilinear coupling (Psi-X-u)
    csl::Tensor Kd; // Down-type trilinear coupling (Psi-X-d)


    ///////////////////////////////////////////////////
    // Scalar sector
    ///////////////////////////////////////////////////

    mty::Particle H; // Higgs doublet 
    mty::Particle X; // SU(2)X spurion in the fundamental representation. 
    mty::Particle Y; // SU(2)X spurion in the adjoint representation. 

    csl::Expr v_h; // Higgs' VEV 
    csl::Expr v_y; // Y VEV : v_y^2 = v_y1^2 + v_y2^2 
    csl::Expr v_x; // X VEV  


}
