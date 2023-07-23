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
 * @file 2HDM.h
 * @brief
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2020-11-05
 */

#ifndef _2HDM_H_INCLUDED
#define _2HDM_H_INCLUDED

#include "../core/ckm.h"
#include "../core/ghostField.h"
#include "../core/model.h"
#include "../core/mrtInterface.h"
#include "sm.h"

namespace mty {

template <int type>
struct Z2_charges {
};

template <>
struct Z2_charges<1> {
    static constexpr int u = 1;
    static constexpr int d = 1;
    static constexpr int e = 1;
};
template <>
struct Z2_charges<2> {
    static constexpr int u = 1;
    static constexpr int d = -1;
    static constexpr int e = -1;
};
template <>
struct Z2_charges<3> {
    static constexpr int u = 1;
    static constexpr int d = -1;
    static constexpr int e = 1;
};
template <>
struct Z2_charges<4> {
    static constexpr int u = 1;
    static constexpr int d = 1;
    static constexpr int e = -1;
};

std::pair<csl::Expr, csl::Expr> Z2_coef(int charge);
csl::Expr Z2_mass_coef(csl::Expr const &v1, csl::Expr const &v2, int charge);

template <int type>
class THDM_Model : public mty::SM_Model {

  public:
    THDM_Model(bool initialize = true);
    ~THDM_Model() override;

    void init();

    void initContent();
    void initHiggsSector();
    void initYukawas();

    void replaceHiggs();
    void diagonalize2By2Matrices();
    void replaceYukawas();
    void adjust();

    template <int t_type>
    friend std::ostream &operator<<(std::ostream     &out,
                                    THDM_Model const &model);

  protected:
    // Higgs potential parameters
    csl::Expr m11, m12, m22, lambda1, lambda2, lambda3, lambda4, lambda5;

    csl::Expr v, v1, v2;                 // VEV parameters
    csl::Expr alpha, beta, tan_beta;     // Higgs rotatioin parameters
    csl::Expr e, gY, gL, theta_Weinberg; // Electroweak parameters
};

template <int type>
THDM_Model<type>::THDM_Model(bool initialize) : SM_Model(false)
{
    if (initialize)
        init();
}

template <int type>
void THDM_Model<type>::init()
{
    initContent();
    SM_Model::gaugeSymmetryBreaking();
    replaceHiggs();
    diagonalize2By2Matrices();
    replaceYukawas();
    SM_Model::flavorSymmetryBreaking();
    adjust();
}
template <int type>
void THDM_Model<type>::initContent()
{
    SM_Model::initGauge();
    SM_Model::initFermions();
    THDM_Model<type>::initHiggsSector();
    THDM_Model<type>::initYukawas();
}

template <int type>
void THDM_Model<type>::initHiggsSector()
{
    m11     = csl::constant_s("m_11");
    m12     = csl::constant_s("m_12");
    m22     = csl::constant_s("m_22");
    lambda1 = csl::constant_s("lambda_1");
    lambda2 = csl::constant_s("lambda_2");
    lambda3 = csl::constant_s("lambda_3");
    lambda4 = csl::constant_s("lambda_4");
    lambda5 = csl::constant_s("lambda_5");

    Particle Phi1 = scalarboson_s("Phi1 ; \\Phi_{1}", *this);
    Phi1->setSelfConjugate(false);
    Phi1->setGroupRep("L", 1);
    Phi1->setGroupRep("Y", {1, 2});
    Particle Phi2 = scalarboson_s("Phi2 ; \\Phi_{2}", *this);
    Phi2->setSelfConjugate(false);
    Phi2->setGroupRep("L", 1);
    Phi2->setGroupRep("Y", {1, 2});
    addParticles({Phi1, Phi2});

    csl::Index i = GaugeIndex(*this, "L", Phi1);
    csl::Index j = GaugeIndex(*this, "L", Phi1);

    csl::Expr s11 = csl::GetComplexConjugate(Phi1(i)) * Phi1(i);
    csl::Expr s12 = csl::GetComplexConjugate(Phi1(i)) * Phi2(i);
    csl::Expr s21 = csl::GetComplexConjugate(Phi2(j)) * Phi1(j);
    csl::Expr s22 = csl::GetComplexConjugate(Phi2(j)) * Phi2(j);

    csl::Expr term_m11 = -m11 * m11 * s11;
    csl::Expr term_m22 = -m22 * m22 * s22;
    csl::Expr term_m12 = m12 * m12 * (s12 + s21);

    csl::Expr term_l1 = -lambda1 / 2 * csl::pow_s(s11, 2);
    csl::Expr term_l2 = -lambda2 / 2 * csl::pow_s(s22, 2);
    csl::Expr term_l3 = -lambda3 * s11 * s22;
    csl::Expr term_l4 = -lambda4 * s12 * s21;
    csl::Expr term_l5
        = -lambda5 / 2 * (csl::pow_s(s12, 2) + csl::pow_s(s21, 2));

    AddTerm(*this,
            {term_m11,
             term_m12,
             term_m22,
             term_l1,
             term_l2,
             term_l3,
             term_l4,
             term_l5});
}

template <int type>
void THDM_Model<type>::initYukawas()
{
    auto       *flavorSpace = getVectorSpace("SM_flavor");
    csl::Tensor Y1u("Y1u", {flavorSpace, flavorSpace});
    csl::Tensor Y1d("Y1d", {flavorSpace, flavorSpace});
    csl::Tensor Y1e("Y1e", {flavorSpace, flavorSpace});
    Y1u->setComplexProperty(csl::ComplexProperty::Complex);
    Y1d->setComplexProperty(csl::ComplexProperty::Complex);
    Y1e->setComplexProperty(csl::ComplexProperty::Complex);
    csl::Tensor Y2u("Y2u", {flavorSpace, flavorSpace});
    csl::Tensor Y2d("Y2d", {flavorSpace, flavorSpace});
    csl::Tensor Y2e("Y2e", {flavorSpace, flavorSpace});
    Y2u->setComplexProperty(csl::ComplexProperty::Complex);
    Y2d->setComplexProperty(csl::ComplexProperty::Complex);
    Y2e->setComplexProperty(csl::ComplexProperty::Complex);
    csl::Tensor eps = getVectorSpace("L", "Q")->getEpsilon();
    csl::Index  I   = flavorSpace->generateIndex();
    csl::Index  J   = flavorSpace->generateIndex();
    csl::Index  a   = generateIndex("C", "Q");
    csl::Index  i   = generateIndex("L", "Q");
    csl::Index  j   = generateIndex("L", "Q");
    csl::Index  al  = DiracIndex();

    Particle Q    = getParticle("Q");
    Particle U    = getParticle("U_R");
    Particle D    = getParticle("D_R");
    Particle L    = getParticle("L");
    Particle E    = getParticle("E_R");
    Particle Phi1 = getParticle("Phi1");
    Particle Phi2 = getParticle("Phi2");

    addLagrangianTerm(
        Y1u({I, J}) * csl::GetComplexConjugate(Phi1(i)) * eps({i, j})
            * csl::GetComplexConjugate(Q({I, a, j, al})) * U({J, a, al}),
        true);
    addLagrangianTerm(-Y1d({I, J}) * Phi1(i)
                          * csl::GetComplexConjugate(Q({I, a, i, al}))
                          * D({J, a, al}),
                      true);
    addLagrangianTerm(-Y1e({I, J}) * Phi1(i)
                          * csl::GetComplexConjugate(L({I, i, al}))
                          * E({J, al}),
                      true);

    addLagrangianTerm(
        Y2u({I, J}) * csl::GetComplexConjugate(Phi2(i)) * eps({i, j})
            * csl::GetComplexConjugate(Q({I, a, j, al})) * U({J, a, al}),
        true);
    addLagrangianTerm(-Y2d({I, J}) * Phi2(i)
                          * csl::GetComplexConjugate(Q({I, a, i, al}))
                          * D({J, a, al}),
                      true);
    addLagrangianTerm(-Y2e({I, J}) * Phi2(i)
                          * csl::GetComplexConjugate(L({I, i, al}))
                          * E({J, al}),
                      true);

    addTensorCoupling(Y1e);
    addTensorCoupling(Y1u);
    addTensorCoupling(Y1d);
    addTensorCoupling(Y2e);
    addTensorCoupling(Y2u);
    addTensorCoupling(Y2d);
}

template <int type>
void THDM_Model<type>::replaceHiggs()
{
    ///////////////////////////////////////////////////
    // Actual gauge (spontaneous) symmetry breaking
    ///////////////////////////////////////////////////

    v1       = csl::constant_s("v1 ; v_1");
    beta     = csl::constant_s("beta ; \\beta");
    tan_beta = csl::tan_s(beta);
    v        = csl::constant_s("v");
    v2       = v1 * tan_beta;

    Particle Phi_10 = GetParticle(*this, "Phi1_1");
    Particle Phi_11 = GetParticle(*this, "Phi1_2");
    Particle Phi_20 = GetParticle(*this, "Phi2_1");
    Particle Phi_21 = GetParticle(*this, "Phi2_2");

    Particle phi1_c = scalarboson_s("phi_1", *this);
    Particle phi2_c = scalarboson_s("phi_2", *this);
    Particle rho_1  = scalarboson_s("rho_1", *this);
    Particle rho_2  = scalarboson_s("rho_2", *this);
    Particle eta_1  = scalarboson_s("eta_1", *this);
    Particle eta_2  = scalarboson_s("eta_2", *this);
    SetSelfConjugate(rho_1, true);
    SetSelfConjugate(rho_2, true);
    SetSelfConjugate(eta_1, true);
    SetSelfConjugate(eta_2, true);

    Replaced(*this, Phi_10, phi1_c());
    Replaced(*this, Phi_11, (rho_1() + CSL_I * eta_1() + v1) / csl::sqrt_s(2));

    Replaced(*this, Phi_20, phi2_c());
    Replaced(*this, Phi_21, (rho_2() + CSL_I * eta_2() + v2) / csl::sqrt_s(2));

    Replaced(*this,
             csl::pow_s(m11, 2),
             -CSL_HALF * lambda1 * v1 * v1 + v2 / v1 * m12 * m12
                 - v2 * v2 / 2 * lambda3 - v2 * v2 / 2 * lambda4
                 - v2 * v2 / 2 * lambda5);

    Replaced(*this,
             csl::pow_s(m22, 2),
             -CSL_HALF * lambda2 * v2 * v2 + v1 / v2 * m12 * m12
                 - v1 * v1 / 2 * lambda3 - v1 * v1 / 2 * lambda4
                 - v1 * v1 / 2 * lambda5);
}

template <int type>
void THDM_Model<type>::diagonalize2By2Matrices()
{
    ///////////////////////////////////////////////////
    // Diagonalizing what can be
    ///////////////////////////////////////////////////

    alpha = csl::constant_s("alpha ; \\alpha");

    mty::Particle eta_u = getParticle("eta_1");
    mty::Particle eta_d = getParticle("eta_2");
    mty::Particle G0    = scalarboson_s("G0; G^0", *this);
    mty::Particle A0    = scalarboson_s("A0; A^0", *this);
    mty::SetSelfConjugate(G0, true);
    mty::SetSelfConjugate(A0, true);
    rotateFields({eta_u, eta_d},
                 {G0, A0},
                 {{csl::cos_s(beta), -csl::sin_s(beta)},
                  {csl::sin_s(beta), csl::cos_s(beta)}},
                 true,
                 1 // diagonalize, one massless state
    );

    mty::Particle phi_u = getParticle("phi_1");
    mty::Particle phi_d = getParticle("phi_2");
    mty::Particle Gp    = scalarboson_s("Gp; G^+", *this);
    mty::Particle Hp    = scalarboson_s("Hp; H^+", *this);
    rotateFields({phi_u, phi_d},
                 {Gp, Hp},
                 {{csl::cos_s(beta), -csl::sin_s(beta)},
                  {csl::sin_s(beta), csl::cos_s(beta)}},
                 true,
                 1 // diagonalize, one massless state
    );

    mty::Particle rho_u = getParticle("rho_1");
    mty::Particle rho_d = getParticle("rho_2");
    mty::Particle h     = scalarboson_s("h; h^0", *this);
    mty::Particle H     = scalarboson_s("H0; H^0", *this);
    mty::SetSelfConjugate(h, true);
    mty::SetSelfConjugate(H, true);
    rotateFields({rho_u, rho_d},
                 {h, H},
                 {{-csl::sin_s(alpha), csl::cos_s(alpha)},
                  {csl::cos_s(alpha), csl::sin_s(alpha)}},
                 true // diagonalize, no massless state
    );

    DiagonalizeMassMatrices(*this);
    Rename(*this, "B", "A");
    Rename(*this, "W_3", "Z");

    gY             = GetCoupling(*this, "g_Y");
    gL             = GetCoupling(*this, "g_L");
    theta_Weinberg = csl::constant_s("theta_W ; \\theta_W");
    e              = csl::constant_s("e_em ; e_{\\mathrm{em}}");

    Replaced(*this,
             gL * gL + gY * gY,
             csl::pow_s(gL / csl::cos_s(theta_Weinberg), csl::int_s(2)));
    Replaced(*this, gY, e / csl::cos_s(theta_Weinberg));
    Replaced(*this, gL, e / csl::sin_s(theta_Weinberg));
}

template <int type>
void THDM_Model<type>::replaceYukawas()
{
    ///////////////////////////////////////////////////
    // Taking care of yukawa couplings
    ///////////////////////////////////////////////////

    csl::Tensor Ye1 = GetYukawa(*this, "Y1e");
    csl::Tensor Yu1 = GetYukawa(*this, "Y1u");
    csl::Tensor Yd1 = GetYukawa(*this, "Y1d");

    csl::Tensor Ye2 = GetYukawa(*this, "Y2e");
    csl::Tensor Yu2 = GetYukawa(*this, "Y2u");
    csl::Tensor Yd2 = GetYukawa(*this, "Y2d");

    csl::Expr m_e   = csl::constant_s("m_e");
    csl::Expr m_mu  = csl::constant_s("m_mu ; m_\\mu");
    csl::Expr m_tau = csl::constant_s("m_tau ; m_\\tau");
    csl::Expr m_u   = csl::constant_s("m_u");
    csl::Expr m_c   = csl::constant_s("m_c");
    csl::Expr m_t   = csl::constant_s("m_t");
    csl::Expr m_d   = csl::constant_s("m_d");
    csl::Expr m_s   = csl::constant_s("m_s");
    csl::Expr m_b   = csl::constant_s("m_b");

    const csl::Space *flavorSpace = GetSpace(Ye1);
    csl::Tensor       M_e         = csl::tensor_s("M_e",
                                                  {flavorSpace, flavorSpace},
                                    csl::matrix_s({{m_e, CSL_0, CSL_0},
                                                                 {CSL_0, m_mu, CSL_0},
                                                                 {CSL_0, CSL_0, m_tau}}));

    csl::Tensor M_u = csl::tensor_s(
        "M_u",
        {flavorSpace, flavorSpace},
        csl::matrix_s(
            {{m_u, CSL_0, CSL_0}, {CSL_0, m_c, CSL_0}, {CSL_0, CSL_0, m_t}}));

    csl::Tensor M_d = csl::tensor_s(
        "M_d",
        {flavorSpace, flavorSpace},
        csl::matrix_s(
            {{m_d, CSL_0, CSL_0}, {CSL_0, m_s, CSL_0}, {CSL_0, CSL_0, m_b}}));

    csl::Index  f_i        = GetIndex(flavorSpace);
    csl::Index  f_j        = GetIndex(flavorSpace);
    csl::Index  f_k        = GetIndex(flavorSpace);
    csl::Index  f_l        = GetIndex(flavorSpace);
    csl::Tensor delta_flav = Delta(flavorSpace);

    buildCKM(flavorSpace);

    auto e_coefs = Z2_coef(Z2_charges<type>::e);
    auto u_coefs = Z2_coef(Z2_charges<type>::u);
    auto d_coefs = Z2_coef(Z2_charges<type>::d);
    Replaced(*this, Ye1, e_coefs.first * Ye1({f_i, f_j}));
    Replaced(*this, Ye2, e_coefs.second * Ye1({f_i, f_j}));
    Replaced(*this, Yu1, u_coefs.first * Yu1({f_i, f_j}));
    Replaced(*this, Yu2, u_coefs.second * Yu1({f_i, f_j}));
    Replaced(*this, Yd1, d_coefs.first * Yd1({f_i, f_j}));
    Replaced(*this, Yd2, d_coefs.second * Yd1({f_i, f_j}));

    csl::Expr e_mass_coef = Z2_mass_coef(v1, v2, Z2_charges<type>::e);
    csl::Expr u_mass_coef = Z2_mass_coef(v1, v2, Z2_charges<type>::u);
    csl::Expr d_mass_coef = Z2_mass_coef(v1, v2, Z2_charges<type>::d);
    Replaced(*this, Ye1, e_mass_coef * M_e({f_i, f_j}));
    Replaced(*this, Yu1, u_mass_coef * M_u({f_i, f_j}));
    Replaced(*this,
             Yd1({f_i, f_j}),
             d_mass_coef
                 * csl::prod_s(
                     {V_CKM({f_i, f_k}),
                      M_d({f_k, f_l}),
                      GetHermitianConjugate(V_CKM({f_l, f_j}), flavorSpace)},
                     true));

    mty::Particle D_L = GetParticle(*this, "D_L");
    mty::Particle D_R = GetParticle(*this, "D_R");
    csl::Index    a1  = DiracIndex();
    csl::Index    A   = GaugeIndex(*this, "C", D_L);
    Replaced(*this, D_L({f_j, A, a1}), V_CKM({f_j, f_k}) * D_L({f_k, A, a1}));
    Replaced(*this, D_R({f_i, A, a1}), V_CKM({f_i, f_j}) * D_R({f_j, A, a1}));
    Replaced(*this, 1 + csl::pow_s(tan_beta, -2), v * v / (v2 * v2));
}

template <int type>
void THDM_Model<type>::adjust()
{
    Replaced(*this, getParticle("W")->getMass(), sm_input::M_W);
    getParticle("W")->setMass(sm_input::M_W);
    Replaced(*this, getParticle("Z")->getMass(), sm_input::M_Z);
    getParticle("Z")->setMass(sm_input::M_Z);
    PromoteGoldstone(*this, "Gp", "W");
    PromoteGoldstone(*this, "G0", "Z");
    // Replaced(*this,
    //         1 + tan_beta*tan_beta,
    //         csl::pow_s(2 * getParticle("W")->getMass()
    //             * csl::sin_s(theta_Weinberg) / (e * v1), 2));
    Replaced(
        *this, tan_beta + 1 / tan_beta, (1 + tan_beta * tan_beta) / tan_beta);
    Replaced(*this, v1, v / csl::sqrt_s(1 + tan_beta * tan_beta));
    Replaced(*this, v, (2 * sm_input::M_W * csl::sin_s(theta_Weinberg)) / e);
    Replaced(*this,
             1 / (1 + tan_beta * tan_beta)
                 + tan_beta * tan_beta / (1 + tan_beta * tan_beta),
             CSL_1);
    Replaced(*this,
             1 + csl::pow_s(tan_beta, -2),
             1 / csl::pow_s(csl::sin_s(beta), 2));
    Replaced(*this,
             1 + csl::pow_s(tan_beta, 2),
             1 / csl::pow_s(csl::cos_s(beta), 2));
    Replaced(*this, csl::tan_s(beta), csl::sin_s(beta) / csl::cos_s(beta));
    refresh();
    L.mergeTerms();
}

template <int type>
THDM_Model<type>::~THDM_Model()
{
}

template <int t_type>
std::ostream &operator<<(std::ostream &out, THDM_Model<t_type> const &model)
{
    return out << *static_cast<Model const *>(&model);
}

} // End of namespace mty

#endif
