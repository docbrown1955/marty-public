#include <marty>
#pragma GCC diagnostic ignored "-Wunused-variable"

using namespace csl;
using namespace mty;

int main() {

    ///////////////////////////////////////////////////
    // Model definition
    ///////////////////////////////////////////////////

    Model ElectroWeak;
    AddGaugedGroup(ElectroWeak, group::Type::SU, "SU2_L", 2, constant_s("g"));
    AddGaugedGroup(ElectroWeak, group::Type::U1, "U1_Y", constant_s("g'"));
    Init(ElectroWeak);

    Rename(ElectroWeak, "A_SU2_L", "W");
    Rename(ElectroWeak, "A_U1_Y", "B");
    Particle W = GetParticle(ElectroWeak, "W");
    Particle B = GetParticle(ElectroWeak, "B");
    Particle ghost_W = GetParticle(ElectroWeak, "c_A_SU2_L");

    std::cout << ElectroWeak << std::endl;

    ///////////////////////////////////////////////////
    // Higgs and fermion sector
    ///////////////////////////////////////////////////

    Particle H = scalarboson_s("H", ElectroWeak);
    SetGroupRep(H, "SU2_L", {1});
    SetGroupRep(H, "U1_Y",  {1, 2});
    AddParticle(ElectroWeak, H);

    Particle q_L = weylfermion_s("q_L", ElectroWeak, Chirality::Left);
    SetGroupRep(q_L, "SU2_L", {1});
    SetGroupRep(q_L, "U1_Y", {1, 6});
    AddParticle(ElectroWeak, q_L);

    Particle u_R = weylfermion_s("u_R", ElectroWeak, Chirality::Right);
    SetGroupRep(u_R, "U1_Y", {2, 3});
    AddParticle(ElectroWeak, u_R);

    Particle d_R = weylfermion_s("d_R", ElectroWeak, Chirality::Right);
    SetGroupRep(d_R, "U1_Y", {-1, 3});
    AddParticle(ElectroWeak, d_R);

    Particle l_L = weylfermion_s("l_L", ElectroWeak, Chirality::Left);
    SetGroupRep(l_L, "SU2_L", {1});
    SetGroupRep(l_L, "U1_Y", {-1, 2});
    AddParticle(ElectroWeak, l_L);

    Particle e_R = weylfermion_s("e_R", ElectroWeak, Chirality::Right);
    SetGroupRep(e_R, "U1_Y", {-1, 1});
    AddParticle(ElectroWeak, e_R);

    ///////////////////////////////////////////////////
    // Additional couplings (H potential + Yukawa)
    ///////////////////////////////////////////////////

    csl::Expr v  = constant_s("v");
    csl::Expr mH = constant_s("m_h");
    csl::Expr Yu = sqrt_s(2)*constant_s("m_u")/v;
    csl::Expr Yd = sqrt_s(2)*constant_s("m_d")/v;
    csl::Expr Ye = sqrt_s(2)*constant_s("m_e")/v;
    Tensor X   = MinkowskiVector("X");
    Index alpha = DiracIndex();
    Index i     = GaugeIndex(ElectroWeak, "SU2_L", H);
    Index j     = GaugeIndex(ElectroWeak, "SU2_L", H);
    const Space* SU2_F_space = VectorSpace(ElectroWeak, "SU2_L", H);
    Tensor eps = Epsilon(SU2_F_space);

    csl::Expr H_squared = GetComplexConjugate(H(i, X)) * H(i, X);
    csl::Expr HiggsPotential = -pow_s(mH, 2)/(2*pow_s(v, 2)) 
                         * pow_s(H_squared - v*v/2, 2);

    AddTerm(ElectroWeak, HiggsPotential);
    AddTerm(ElectroWeak,
            -Yu * eps({i, j}) * GetComplexConjugate(H(j, X)) 
                * GetComplexConjugate(q_L({i, alpha}, X)) * u_R(alpha, X),
            true);
    AddTerm(ElectroWeak,
            -Yd * H(i, X) 
                * GetComplexConjugate(q_L({i, alpha}, X)) * d_R(alpha, X),
            true);
    AddTerm(ElectroWeak,
            -Ye * H(i, X) 
                * GetComplexConjugate(l_L({i, alpha}, X)) * e_R(alpha, X),
            true);

    ///////////////////////////////////////////////////
    // SU(2)_L symmetry breaking
    ///////////////////////////////////////////////////

    BreakGaugeSymmetry(
            ElectroWeak,
            "SU2_L",
            {H, W, q_L, l_L, ghost_W},
            {{"H0", "H1"},
            {"W1", "W2", "W3"},
            {"u_L", "d_L"},
            {"\\nu _{eL}", "e_L"},
            {"c_W1", "c_W2", "c_W3"}});

    ///////////////////////////////////////////////////
    // Replacements to get SM particles W +-
    ///////////////////////////////////////////////////

    Particle W1   = GetParticle(ElectroWeak, "W1");
    Particle W2   = GetParticle(ElectroWeak, "W2");
    Particle W_SM = GenerateSimilarParticle("W", W1);
    SetSelfConjugate(W_SM, false);

    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();
    csl::Expr W_p = W_SM(+mu, X);
    csl::Expr W_m = GetComplexConjugate(W_SM(+mu, X));
    csl::Expr F_W_p = W_SM({+mu,+nu}, X);
    csl::Expr F_W_m = GetComplexConjugate(W_SM({+mu, +nu}, X));

    Replaced(ElectroWeak,
            W1,
            (W_p + W_m) / sqrt_s(2));
    Replaced(ElectroWeak,
            W2,
            CSL_I * (W_p - W_m) / sqrt_s(2));
    Replaced(ElectroWeak,
            GetFieldStrength(W1),
            (F_W_p + F_W_m) / sqrt_s(2));
    Replaced(ElectroWeak,
            GetFieldStrength(W2),
            CSL_I * (F_W_p - F_W_m) / sqrt_s(2));

    ///////////////////////////////////////////////////
    // Actual gauge (spontaneous) symmetry breaking
    ///////////////////////////////////////////////////

    Particle H0 = GetParticle(ElectroWeak, "H0");
    Particle H1 = GetParticle(ElectroWeak, "H1");

    Particle h = scalarboson_s("h", ElectroWeak);
    Particle pi0 = scalarboson_s("\\pi _0", ElectroWeak);
    Particle pic = scalarboson_s("\\pi _c", ElectroWeak);
    SetSelfConjugate(pi0, true);
    SetSelfConjugate(h, true);

    Replaced(ElectroWeak,
            H0,
            CSL_0);
    Replaced(ElectroWeak,
            H1,
            (h(X) + v)/sqrt_s(2));

    std::cout << ElectroWeak << std::endl;

    ///////////////////////////////////////////////////
    // Gather masses of all particles, 
    // get standard conventions for A and Z
    ///////////////////////////////////////////////////

    DiagonalizeMassMatrices(ElectroWeak);

    Rename(ElectroWeak, "B_mix", "A");
    Rename(ElectroWeak, "W3_mix", "Z");

    csl::Expr e = constant_s("e");
    csl::Expr thetaW = constant_s("\\theta_W");
    csl::Expr g = GetCoupling(ElectroWeak, "g");
    csl::Expr g_p = GetCoupling(ElectroWeak, "g'");
    Replaced(ElectroWeak,
            pow_s(g, 2) + pow_s(g_p, 2),
            pow_s(g*g_p/e, 2));
    Replaced(ElectroWeak,
            g,
            e / sin_s(thetaW));
    Replaced(ElectroWeak,
            g_p,
            e / cos_s(thetaW));

    std::cout << ElectroWeak << std::endl;
    std::cout << "Computing feynman rules ... " << std::endl;

    auto rules = ComputeFeynmanRules(ElectroWeak);
    Display(rules);
    // ExportPNG("EW_rules", rules);

    std::cout << ElectroWeak << std::endl;
    
    W = W_SM;
    Particle Z = GetParticle(ElectroWeak, "Z");
    Timer t;
    auto WW_to_ZZ = 
        ElectroWeak.computeAmplitude(
                Order::TreeLevel,
                {Incoming(W), Incoming(AntiPart(W)),
                 Outgoing(Z), Outgoing(Z)});
    Display(WW_to_ZZ);
    Show(WW_to_ZZ);

    t.restart();
    auto ZZ_to_ZZ = 
        ElectroWeak.computeAmplitude(
                Order::OneLoop,
                {Incoming(Z), Incoming(Z),
                 Outgoing(Z), Outgoing(Z)});
    Display(ZZ_to_ZZ);
    Show(ZZ_to_ZZ);

    return 0;
}
