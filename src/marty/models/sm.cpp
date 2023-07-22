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

#include "sm.h"
#include "../core/ckm.h"
#include "../core/diracology.h"
#include "../core/fermionicField.h"
#include "../core/ghostField.h"
#include "../core/group.h"
#include "../core/mrtInterface.h"

using namespace mty::sm_input;
using namespace csl;

namespace mty {

// Write the SM initialization
SM_Model::SM_Model(bool initialize)
{
    if (initialize) {
        SM_Model::init();
        refresh();
    }
}

void SM_Model::init()
{
    initContent();
    getToLowEnergyLagrangian();
}

void SM_Model::getToLowEnergyLagrangian()
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

void SM_Model::initContent()
{
    initGauge();
    initFermions();
    initHiggsPotential();
    initYukawas();
}

void SM_Model::initGauge()
{
    addGaugedGroup(group::Type::SU, "C", 3, g_s);
    addGaugedGroup(group::Type::SU, "L", 2, csl::constant_s("g_L"));
    addGaugedGroup(group::Type::U1, "Y", csl::constant_s("g_Y"));
    addFlavorGroup("SM_flavor", 3);
    Model::init();
    renameParticle("A_Y", "B");
    renameParticle("A_L", "W");
    renameParticle("A_C", "G");
    getParticle("G")->setDrawType(drawer::ParticleType::Gluon);
}

void SM_Model::initFermions()
{
    Particle Q = weylfermion_s("Q", *this, Chirality::Left);
    Q->setGroupRep("C", {1, 0});
    Q->setGroupRep("L", 1);
    Q->setGroupRep("Y", {1, 6});
    Q->setFundamentalFlavorRep("SM_flavor");

    Particle U = weylfermion_s("U_R", *this, Chirality::Right);
    U->setGroupRep("C", {1, 0});
    U->setGroupRep("Y", {2, 3});
    U->setFundamentalFlavorRep("SM_flavor");

    Particle D = weylfermion_s("D_R", *this, Chirality::Right);
    D->setGroupRep("C", {1, 0});
    D->setGroupRep("Y", {-1, 3});
    D->setFundamentalFlavorRep("SM_flavor");

    Particle L = weylfermion_s("L", *this, Chirality::Left);
    L->setGroupRep("L", 1);
    L->setGroupRep("Y", {-1, 2});
    L->setFundamentalFlavorRep("SM_flavor");

    Particle E = weylfermion_s("E_R", *this, Chirality::Right);
    E->setGroupRep("Y", -1);
    E->setFundamentalFlavorRep("SM_flavor");

    addParticles({Q, U, D, L, E});
}

void SM_Model::initHiggsPotential()
{
    Particle H = scalarboson_s("H", *this);
    H->setGroupRep("L", 1);
    H->setGroupRep("Y", {1, 2});
    addParticle(H);

    csl::Index i = generateIndex("L", H);
    csl::Index j = generateIndex("L", H);

    csl::Expr mh  = sm_input::m_h;
    csl::Expr H2  = csl::GetComplexConjugate(H(i)) * H(i);
    csl::Expr m2  = mh * mh / 2;
    csl::Expr lam = mh * mh / (2 * v * v);

    // Mexican hat potential
    addLagrangianTerm(m2 * H2);
    addLagrangianTerm(-lam * csl::pow_s(H2, 2));
    // later on: m   = m_h / sqrt(2)
    //           lam = mh^2 / (2*v^2)
    //           (With H0 -> (v + h0) / sqrt(2))
}

void SM_Model::initYukawas()
{
    auto  *flavorSpace = getVectorSpace("SM_flavor");
    Tensor Yu("Yu", {flavorSpace, flavorSpace});
    Yu->setComplexProperty(ComplexProperty::Complex);
    Tensor Yd("Yd", {flavorSpace, flavorSpace});
    Yd->setComplexProperty(ComplexProperty::Complex);
    Tensor Ye("Ye", {flavorSpace, flavorSpace});
    Ye->setComplexProperty(ComplexProperty::Complex);
    Tensor eps = getVectorSpace("L", "Q")->getEpsilon();
    Index  I   = flavorSpace->generateIndex();
    Index  J   = flavorSpace->generateIndex();
    Index  a   = generateIndex("C", "Q");
    Index  i   = generateIndex("L", "Q");
    Index  j   = generateIndex("L", "Q");
    Index  al  = DiracIndex();

    Particle Q = getParticle("Q");
    Particle U = getParticle("U_R");
    Particle D = getParticle("D_R");
    Particle L = getParticle("L");
    Particle E = getParticle("E_R");
    Particle H = getParticle("H");

    addLagrangianTerm(Yu({I, J}) * GetComplexConjugate(H(i)) * eps({i, j})
                          * GetComplexConjugate(Q({I, a, j, al}))
                          * U({J, a, al}),
                      true);
    addLagrangianTerm(-Yd({I, J}) * H(i)
                          * GetComplexConjugate(Q({I, a, i, al}))
                          * D({J, a, al}),
                      true);
    addLagrangianTerm(-Ye({I, J}) * H(i) * GetComplexConjugate(L({I, i, al}))
                          * E({J, al}),
                      true);

    addTensorCoupling(Ye);
    addTensorCoupling(Yu);
    addTensorCoupling(Yd);
}

void SM_Model::gaugeSymmetryBreaking()
{
    ///////////////////////////////////////////////////
    // Breaking gauge SU(2)_L symmetry, renaming
    ///////////////////////////////////////////////////

    BreakGaugeSymmetry(*this, "Y");
    BreakGaugeSymmetry(*this, "L");
    renameParticle("Q_1", "U_L");
    renameParticle("Q_2", "D_L");
    renameParticle("L_1", "Nu_L");
    renameParticle("L_2", "E_L");

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

    csl::Index mu    = MinkowskiIndex();
    csl::Index nu    = MinkowskiIndex();
    csl::Expr  W_p   = W_SM(+mu);
    csl::Expr  W_m   = csl::GetComplexConjugate(W_SM(+mu));
    csl::Expr  F_W_p = W_SM({+mu, +nu});
    csl::Expr  F_W_m = csl::GetComplexConjugate(W_SM({+mu, +nu}));

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
}

void SM_Model::HiggsVEVExpansion()
{
    ///////////////////////////////////////////////////
    // Actual gauge (spontaneous) symmetry breaking
    ///////////////////////////////////////////////////

    csl::Expr v = sm_input::v;

    Particle H1 = getParticle("H_1");
    Particle H2 = getParticle("H_2");

    Particle h0 = scalarboson_s("h", *this); // SM Higgs boson
    Particle Gp = scalarboson_s("Gp ; G^+", *this);
    Particle G0 = scalarboson_s("G0 ; G^0", *this);
    h0->setSelfConjugate(true);
    G0->setSelfConjugate(true);

    replace(H1, Gp());
    replace(H2, (h0() + CSL_I * G0() + v) / csl::sqrt_s(2));
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

    csl::Expr gY             = getScalarCoupling("g_Y");
    csl::Expr gL             = getScalarCoupling("g_L");
    csl::Expr theta_Weinberg = sm_input::theta_W;
    csl::Expr e              = sm_input::e_em;

    replace(gL * gL + gY * gY,
            csl::pow_s(gL / csl::cos_s(theta_Weinberg), csl::int_s(2)));
    replace(gY, e / csl::cos_s(theta_Weinberg));
    replace(gL, e / csl::sin_s(theta_Weinberg));
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

    const csl::Space *flavorSpace = GetSpace(Ye);
    csl::Index        f_i         = GetIndex(flavorSpace);
    csl::Index        f_j         = GetIndex(flavorSpace);
    csl::Expr         factor      = csl::sqrt_s(2) / v;
    csl::Tensor       M_e         = csl::tensor_s("M_e",
                                    {flavorSpace, flavorSpace},
                                    csl::matrix_s({{m_e, CSL_0, CSL_0},
                                                                 {CSL_0, m_mu, CSL_0},
                                                                 {CSL_0, CSL_0, m_tau}}));

    replace(Ye, factor * M_e({f_i, f_j}));
}

void SM_Model::replaceUpYukawa()
{
    csl::Tensor       Yu          = GetYukawa(*this, "Yu");
    const csl::Space *flavorSpace = GetSpace(Yu);
    csl::Index        f_i         = GetIndex(flavorSpace);
    csl::Index        f_j         = GetIndex(flavorSpace);
    csl::Expr         factor      = csl::sqrt_s(2) / v;
    csl::Expr         m_u         = sm_input::m_u;
    csl::Expr         m_c         = sm_input::m_c;
    csl::Expr         m_t         = sm_input::m_t;
    csl::Tensor       M_u         = csl::tensor_s(
        "M_u",
        {flavorSpace, flavorSpace},
        csl::matrix_s(
            {{m_u, CSL_0, CSL_0}, {CSL_0, m_c, CSL_0}, {CSL_0, CSL_0, m_t}}));

    replace(Yu, factor * M_u({f_i, f_j}));
}

void SM_Model::replaceDownYukawa()
{
    csl::Tensor       Yd          = GetYukawa(*this, "Yd");
    const csl::Space *flavorSpace = GetSpace(Yd);
    csl::Expr         factor      = csl::sqrt_s(2) / v;
    csl::Expr         m_d         = sm_input::m_d;
    csl::Expr         m_s         = sm_input::m_s;
    csl::Expr         m_b         = sm_input::m_b;
    csl::Tensor       M_d         = csl::tensor_s(
        "M_d",
        {flavorSpace, flavorSpace},
        csl::matrix_s(
            {{m_d, CSL_0, CSL_0}, {CSL_0, m_s, CSL_0}, {CSL_0, CSL_0, m_b}}));

    csl::Index f_i = GetIndex(flavorSpace);
    csl::Index f_j = GetIndex(flavorSpace);
    csl::Index f_k = GetIndex(flavorSpace);

    buildCKM(flavorSpace);

    replace(Yd({f_i, f_j}),
            csl::prod_s({factor, V_CKM({f_i, f_k}), M_d({f_k, f_j})}, true));

    mty::Particle D_L = getParticle("D_L");
    csl::Index    a1  = DiracIndex();
    csl::Index    A   = GaugeIndex(*this, "C", D_L);
    replace(D_L({f_j, A, a1}), V_CKM({f_j, f_k}) * D_L({f_k, A, a1}));
}

void SM_Model::flavorSymmetryBreaking()
{
    ///////////////////////////////////////////////////
    // Finally breaking SM flavor symmetry
    // to get the 3 fermion generations
    ///////////////////////////////////////////////////

    BreakFlavorSymmetry(*this, "SM_flavor");
    static const std::vector<std::pair<std::string, std::string>> names = {
        {"U_L_1", "u_L"},
        {"U_L_2", "c_L"},
        {"U_L_3", "t_L"},
        {"D_L_1", "d_L"},
        {"D_L_2", "s_L"},
        {"D_L_3", "b_L"},
        {"E_L_1", "e_L"},
        {"E_L_2", "mu_L;\\mu_L"},
        {"E_L_3", "tau_L;\\tau_L"},
        {"Nu_L_1", "nu_e;\\nu_e"},
        {"Nu_L_2", "nu_mu;\\nu_\\mu"},
        {"Nu_L_3", "nu_tau;\\nu_\\tau"},
        {"U_R_1", "u_R"},
        {"U_R_2", "c_R"},
        {"U_R_3", "t_R"},
        {"D_R_1", "d_R"},
        {"D_R_2", "s_R"},
        {"D_R_3", "b_R"},
        {"E_R_1", "e_R"},
        {"E_R_2", "mu_R;\\mu_R"},
        {"E_R_3", "tau_R;\\tau_R"},
    };
    for (const auto &[previous, next] : names)
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
    addGaugeFixingTerms();
}

void SM_Model::addGaugeFixingTerms()
{
    using namespace sm_input;

    auto cc = [](csl::Expr const &expr) { return GetComplexConjugate(expr); };

    csl::Expr cosW = csl::cos_s(theta_W);
    csl::Expr g    = e_em / csl::sin_s(theta_W);
    Particle  h0   = getParticle("h");
    Particle  G0   = getParticle("G0");
    Particle  Gp   = getParticle("Gp");
    csl::Expr ap   = csl::constant_s("ap");
    csl::Expr am   = csl::constant_s("am");
    csl::Expr az   = csl::constant_s("az");
    csl::Expr aa   = csl::constant_s("aa");
    csl::Expr delta_G0
        = -g / 2 * (am * Gp + ap * cc(Gp)) + g / (2 * cosW) * az * (v + h0);
    csl::Expr delta_Gp
        = -CSL_I * g / 2 * (v + h0 + CSL_I * G0) * ap
          - CSL_I * g * csl::cos_s(2 * theta_W) / (2 * cosW) * Gp * az
          + CSL_I * e_em * Gp * aa;
    csl::Expr delta_Gm = cc(Replaced(delta_Gp, ap, am));
    Particle  W        = getParticle("W");
    Particle  Z        = getParticle("Z");
    Particle  A        = getParticle("A");
    csl::Expr xi_W     = W->getGaugeChoice().getXi();
    csl::Expr xi_Z     = Z->getGaugeChoice().getXi();

    csl::Expr dF_Wp = -CSL_I * M_W * xi_W * delta_Gp;
    csl::Expr dF_Wm = CSL_I * M_W * xi_W * delta_Gm;
    csl::Expr dF_Z  = -M_Z * xi_Z * delta_G0;

    csl::Expr c_A  = getParticle("c_A");
    csl::Expr c_Wp = getParticle("c_Wp");
    csl::Expr c_Wm = getParticle("c_Wm");
    csl::Expr c_Z  = getParticle("c_Z");
    for (const auto &[c1, delta] : std::array{std::pair{c_Z, dF_Z},
                                              std::pair{c_Wp, dF_Wp},
                                              std::pair{c_Wm, dF_Wm}}) {
        for (const auto &[a, c2] : std::array{std::pair{aa, c_A},
                                              std::pair{az, c_Z},
                                              std::pair{ap, c_Wp},
                                              std::pair{am, c_Wm}}) {
            csl::Expr term = delta->derive(a.get()).value();
            if (term != CSL_0) {
                addLagrangianTerm(cc(c1) * term * c2);
            }
        }
    }
}

namespace sm_input {

static std::vector<csl::Expr> const &_generate_sm_param_values()
{
    static bool init = false;

    static std::vector<csl::Expr> _values(all_params.size());
    if (!init) {
        std::transform(
            all_params.begin(),
            all_params.end(),
            _values.begin(),
            [&](csl::Expr const &expr) {
                return expr->evaluate(csl::eval::literal).value_or(CSL_UNDEF);
            });
        init = true;
    }
    undefineNumericalValues();
    return _values;
}

static std::vector<csl::Expr> const &sm_param_values
    = _generate_sm_param_values();

void undefineNumericalValues()
{
    for (std::size_t i = 0; i != all_params.size(); ++i) {
        csl::Expr(all_params[i])->setValue(CSL_UNDEF);
    }
}

void redefineNumericalValues()
{
    for (std::size_t i = 0; i != all_params.size(); ++i) {
        csl::Expr(all_params[i])->setValue(sm_param_values[i]);
    }
}
} // namespace sm_input

} // End of namespace mty
