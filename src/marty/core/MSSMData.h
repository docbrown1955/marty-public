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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef MSSM_DATA_H_INCLUDED
#define MSSM_DATA_H_INCLUDED

#include "quantumField.h"

namespace mty {

struct MSSM_Data {

    ///////////////////////////////////////////////////
    // Gauge sector
    ///////////////////////////////////////////////////

    mty::GaugedGroup *SU3_c;
    mty::GaugedGroup *SU2_L;
    mty::GaugedGroup *U1_Y;

    mty::Particle G;  // Gluon
    mty::Particle Wi; // SU(2)L boson
    mty::Particle B;  // Hypercharge boson

    mty::Particle s_G;  // Gluino
    mty::Particle s_Wi; // Wino
    mty::Particle s_B;  // Bino

    csl::Expr M1; // Bino mass
    csl::Expr M2; // Wino mass
    csl::Expr M3; // Gluino mass

    csl::Expr g_s; // Strong coupling constant
    csl::Expr g_L; // Weak coupling constant ( = g )
    csl::Expr g_Y; // Hypercharge coupling constant ( = g' )

    csl::Space const *flavorSpace;

    ///////////////////////////////////////////////////
    // Lepton sector
    ///////////////////////////////////////////////////

    mty::Particle Li; // Left-handed neutrinos+leptons
    mty::Particle Ei; // Right-handed leptons

    mty::Particle s_Li; // Left-handed sneutrinos + sleptons
    mty::Particle s_Ei; // Right-handed sleptons

    csl::Tensor M2_s_L; // Left-handed slepton mass squared matrix
    csl::Tensor M2_s_E; // Right-handed slepton mass squared matrix

    csl::Tensor Ye; // Lepton Yukawa coupling
    csl::Tensor Ae; // Lepton Trinlinear coupling

    // Lepton final masses
    csl::Expr m_e;
    csl::Expr m_mu;
    csl::Expr m_tau;
    csl::Expr m_u;
    csl::Expr m_c;
    csl::Expr m_t;
    csl::Expr m_d;
    csl::Expr m_s;
    csl::Expr m_b;

    ///////////////////////////////////////////////////
    // Quark sector
    ///////////////////////////////////////////////////

    mty::Particle Qi; // Left-handed quarks
    mty::Particle Ui; // Right-handed up-type quarks
    mty::Particle Di; // Right-handed down-type quarks

    mty::Particle s_Qi; // Left-handed squarks
    mty::Particle s_Ui; // Right-handed up-type squarks
    mty::Particle s_Di; // Right-handed down-type squarks

    csl::Tensor M2_s_Q; // Left-handed squarks mass squared matrix
    csl::Tensor M2_s_U; // Right-handed up-type squarks mass squared matrix
    csl::Tensor M2_s_D; // Right-handed down-type squarks mass squared matrix

    csl::Tensor Yu; // Up-type Yukawa coupling
    csl::Tensor Yd; // Down-type Yukawa coupling
    csl::Tensor Au; // Up-type trilinear coupling
    csl::Tensor Ad; // Down-type trilinear coupling

    ///////////////////////////////////////////////////
    // Higgs sector
    ///////////////////////////////////////////////////

    mty::Particle Hu; // Up-type higgs doublet
    mty::Particle Hd; // Down-type higgs doublet

    mty::Particle s_Hu; // Up-type higgsino doublet
    mty::Particle s_Hd; // Down-type higgsino doublet

    csl::Expr v_h;  // Higgs' VEV v^2 = v_u^2 + v_d^2
    csl::Expr mu_h; // mu constant in Higgs potential
    csl::Expr
              alpha_h; // Angle defining neutral higgses h^0 & H^0 from gauge states
    csl::Expr beta_h; // Angle between up and down VEVS, tan beta = v_u / v_d
    csl::Expr b_h; // Function of mu, m_sHu, m_sHd and beta, condition for VEV
    csl::Expr m_sHu; // Up-type higgsino mass
    csl::Expr m_sHd; // Down-type higgsino mass
    csl::Expr M_A0;  // Pseudo-scalar mass

    ///////////////////////////////////////////////////
    // Map between supersymmetric partners
    ///////////////////////////////////////////////////

    std::map<mty::Particle, mty::Particle> SUSY;
};

} // End of namespace mty

#endif
