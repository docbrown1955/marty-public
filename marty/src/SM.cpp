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

#include "SM.h"
#include "CKM.h"
#include "group.h"
#include "diracology.h"
#include "mrtInterface.h"
#include "fermionicField.h"

using namespace mty::sm_input;
using namespace csl;

namespace mty {


SM_Model::SM_Model(bool initialize)
    :mty::Model("models/files/SM.json")
{
    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
    replace(getScalarCoupling("g_s"), g_s);
    getGaugedGroup("SU3c")->setCouplingConstant(g_s);

    Particle H = GetParticle(*this, "H");

    csl::Index i = GaugeIndex(*this, "SU2L", H);
    csl::Index j = GaugeIndex(*this, "SU2L", H);

    csl::Expr mh = sm_input::m_h;
    csl::Expr H2 = csl::GetComplexConjugate(H(i)) * H(i);
    csl::Expr m2 = mh*mh / 2;
    csl::Expr lam = mh*mh / (2*v*v);

    // Mexican hat potential
    addLagrangianTerm(m2*H2);
    addLagrangianTerm(-lam*csl::pow_s(H2, 2));
    // later on: m   = m_h / sqrt(2)
    //           lam = mh^2 / (2*v^2)
    //           (With H0 -> (v + h0) / sqrt(2))

    if (initialize) {
        init();
        refresh();
    }
}

void SM_Model::init()
{
    gaugeSymmetryBreaking();
    HiggsVEVExpansion();
    diagonalizeSMMassMatrices();
    replaceLeptonYukawa();
    replaceUpYukawa();
    replaceDownYukawa();
    flavorSymmetryBreaking();
    adjust();
}

void SM_Model::gaugeSymmetryBreaking()
{
    ///////////////////////////////////////////////////
    // Breaking gauge SU(2)_L symmetry, renaming
    ///////////////////////////////////////////////////

    BreakGaugeSymmetry(*this, "U1Y");
    BreakGaugeSymmetry( *this, "SU2L");
    renameParticle("Q_1", "U_L");
    renameParticle("Q_2", "D_L");
    renameParticle("L_1", "Nu_L");
    renameParticle("L_2", "E_L");

    ///////////////////////////////////////////////////
    // Replacements to get SM particles W +-
    ///////////////////////////////////////////////////

    Particle W1   = GetParticle(*this, "W_1");
    Particle W2   = GetParticle(*this, "W_2");
    Particle W_SM = GenerateSimilarParticle("W", W1);
    W_SM->setSelfConjugate(false);

    csl::Index mu = MinkowskiIndex();
    csl::Index nu = MinkowskiIndex();
    csl::Expr W_p = W_SM(+mu);
    csl::Expr W_m = csl::GetComplexConjugate(W_SM(+mu));
    csl::Expr F_W_p = W_SM({+mu,+nu});
    csl::Expr F_W_m = csl::GetComplexConjugate(W_SM({+mu, +nu}));

    replace(
            W1,
            (W_p + W_m) / csl::sqrt_s(2));
    replace(
            W2,
            CSL_I * (W_p - W_m) / csl::sqrt_s(2));
    replace(
            GetFieldStrength(W1),
            (F_W_p + F_W_m) / csl::sqrt_s(2));
    replace(
            GetFieldStrength(W2),
            CSL_I * (F_W_p - F_W_m) / csl::sqrt_s(2));
}

void SM_Model::HiggsVEVExpansion()
{
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
}

void SM_Model::diagonalizeSMMassMatrices()
{
    ///////////////////////////////////////////////////
    // Diagonalizing what can be
    ///////////////////////////////////////////////////

    diagonalizeMassMatrices();
    renameParticle("B", "A");
    renameParticle("W_3", "Z");
    gatherMass("W");
    gatherMass("Z");

    csl::Expr gY = getScalarCoupling("g_Y");
    csl::Expr gL = getScalarCoupling("g_L");
    csl::Expr theta_Weinberg = sm_input::theta_W;
    csl::Expr e  = sm_input::e_em;

    replace(
            gL*gL + gY*gY,
            csl::pow_s(gL/csl::cos_s(theta_Weinberg), csl::int_s(2)));
    replace(
            gY, 
            e / csl::cos_s(theta_Weinberg));
    replace(
            gL, 
            e / csl::sin_s(theta_Weinberg));
}

void SM_Model::replaceLeptonYukawa()
{
    ///////////////////////////////////////////////////
    // Taking care of Yukawa couplings
    ///////////////////////////////////////////////////

    csl::Tensor Ye = GetYukawa(*this, "Ye");

    csl::Expr m_e   = sm_input::m_e;
    csl::Expr m_mu  = sm_input::m_mu;
    csl::Expr m_tau = sm_input::m_tau;

    const csl::Space* flavorSpace = GetSpace(Ye);
    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
    csl::Expr factor = csl::sqrt_s(2) / v;
    csl::Tensor M_e = csl::tensor_s(
            "M_e",
            {flavorSpace, flavorSpace},
            csl::matrix_s({{m_e, CSL_0, CSL_0},
                      {CSL_0, m_mu, CSL_0},
                      {CSL_0, CSL_0, m_tau}}));

    replace(Ye, factor*M_e({f_i, f_j}));
}

void SM_Model::replaceUpYukawa()
{
    csl::Tensor Yu = GetYukawa(*this, "Yu");
    const csl::Space* flavorSpace = GetSpace(Yu);
    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
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

void SM_Model::replaceDownYukawa()
{
    csl::Tensor Yd = GetYukawa(*this, "Yd");
    const csl::Space* flavorSpace = GetSpace(Yd);
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
    csl::Index f_l = GetIndex(flavorSpace);

    buildCKM(flavorSpace);

    replace(Yd({f_i, f_j}),
            csl::prod_s({factor,
                         V_CKM({f_i, f_k}), 
                         M_d({f_k, f_l}),
                         GetHermitianConjugate(V_CKM({f_l, f_j}),
                                               flavorSpace)}, 
                         true));

    mty::Particle D_L = getParticle("D_L");
    mty::Particle D_R = getParticle("D_R");
    csl::Index a1  = DiracIndex();
    csl::Index A   = GaugeIndex(*this, "SU3c", D_L);
    replace(D_L({f_j, A, a1}), V_CKM({f_j, f_k}) * D_L({f_k, A, a1}));
    replace(D_R({f_i, A, a1}), V_CKM({f_i, f_j}) * D_R({f_j, A, a1}));
}

void SM_Model::flavorSymmetryBreaking()
{
    ///////////////////////////////////////////////////
    // Finally breaking SM flavor symmetry 
    // to get the 3 fermion generations
    ///////////////////////////////////////////////////

    BreakFlavorSymmetry(*this, "SM_flavor");
    static const std::vector<std::pair<std::string, std::string>> names = {
        {"U_L_1", "u_L"}, {"U_L_2", "c_L"}, {"U_L_3", "t_L"},
        {"D_L_1", "d_L"}, {"D_L_2", "s_L"}, {"D_L_3", "b_L"},
        {"E_L_1", "e_L"}, {"E_L_2", "mu_L;\\mu_L"}, {"E_L_3", "tau_L;\\tau_L"},
        {"Nu_L_1", "nu_e;\\nu_e"}, 
            {"Nu_L_2", "nu_mu;\\nu_\\mu"}, 
            {"Nu_L_3", "nu_tau;\\nu_\\tau"},
        {"U_R_1", "u_R"}, {"U_R_2", "c_R"}, {"U_R_3", "t_R"},
        {"D_R_1", "d_R"}, {"D_R_2", "s_R"}, {"D_R_3", "b_R"},
        {"E_R_1", "e_R"}, {"E_R_2", "mu_R;\\mu_R"}, {"E_R_3", "tau_R;\\tau_R"},
    };
    for (const auto [previous, next] : names) 
        renameParticle(previous, next);
}

void SM_Model::adjust()
{
    using namespace sm_input;
    replace(v, (2 * M_W * csl::sin_s(theta_W)) / e_em);
    replace(getParticle("W")->getMass(), M_W);
    getParticle("W")->setMass(M_W);
    replace(getParticle("Z")->getMass(), M_Z);
    getParticle("Z")->setMass(M_Z);
    promoteToGoldstone("Gp", "W");
    promoteToGoldstone("G0", "Z");
}

} // End of namespace mty
