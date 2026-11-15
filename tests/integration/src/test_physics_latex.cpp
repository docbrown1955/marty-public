#include "marty.h"
#include "marty/models/sm.h"
#include <gtest/gtest.h>

using namespace mty;

namespace {

std::string latex(csl::Expr const &expr)
{
    return expr->printLaTeX(0);
}

} // namespace

TEST(PhysicsLatex, fermionMassTerm)
{
    SM_Model    sm;
    csl::Index  al = DiracIndex("\\alpha");
    csl::Expr   e  = sm.getParticle("e")(al);
    EXPECT_EQ(latex(csl::GetComplexConjugate(e) * e), "\\bar{e}(X)e(X)");
}

TEST(PhysicsLatex, fermionVectorCurrent)
{
    SM_Model    sm;
    csl::Index  mu = MinkowskiIndex("\\mu");
    csl::Index  al = DiracIndex("\\alpha");
    csl::Index  be = DiracIndex("\\beta");
    csl::Expr   current
        = csl::GetComplexConjugate(sm.getParticle("e")(al))
          * dirac4.gamma({+mu, al, be}) * sm.getParticle("e")(be);
    EXPECT_EQ(latex(current), "\\bar{e}(X)\\gamma^{\\mu}e(X)");
}

TEST(PhysicsLatex, quarkFlavorChangingCurrent)
{
    SM_Model    sm;
    csl::Index  mu = MinkowskiIndex("\\mu");
    csl::Index  al = DiracIndex("\\alpha");
    csl::Index  be = DiracIndex("\\beta");
    csl::Index  c  = GaugeIndex("c", sm, "C", sm.getParticle("u"));
    csl::Expr   current
        = csl::GetComplexConjugate(sm.getParticle("u")({c, al}))
          * dirac4.gamma({+mu, al, be}) * sm.getParticle("d")({c, be});
    EXPECT_EQ(latex(current), "\\bar{u}_{c}(X)\\gamma^{\\mu}d_{c}(X)");
}

TEST(PhysicsLatex, coupledFermionVectorCurrent)
{
    SM_Model    sm;
    csl::Index  mu = MinkowskiIndex("\\mu");
    csl::Index  al = DiracIndex("\\alpha");
    csl::Index  be = DiracIndex("\\beta");
    csl::Expr   g  = csl::constant_s("g_e");
    csl::Expr   current
        = g * csl::GetComplexConjugate(sm.getParticle("e")(al))
          * dirac4.gamma({+mu, al, be}) * sm.getParticle("e")(be);
    EXPECT_EQ(latex(current), "g_e\\bar{e}(X)\\gamma^{\\mu}e(X)");
}

TEST(PhysicsLatex, higgsSquared)
{
    SM_Model sm;
    csl::Expr h = sm.getParticle("h")();
    EXPECT_EQ(latex(h * h), "h(X)^{2}");
}

TEST(PhysicsLatex, gaugeBosonConjugate)
{
    SM_Model   sm;
    csl::Index mu = MinkowskiIndex("\\mu");
    EXPECT_EQ(latex(csl::GetComplexConjugate(sm.getParticle("W")(mu))),
              "W_{\\mu}(X)^{\\dagger}");
}

TEST(PhysicsLatex, gaugeBosonKineticSquare)
{
    SM_Model   sm;
    csl::Index mu = MinkowskiIndex("\\mu");
    Particle   W  = sm.getParticle("W");
    EXPECT_EQ(latex(csl::GetComplexConjugate(W(+mu)) * W(-mu)),
              "W_{\\mu}(X)W_{\\mu}(X)^{\\dagger}");
}

TEST(PhysicsLatex, smInputNamesAreGreekAndSubscripted)
{
    EXPECT_EQ(latex(sm_input::alpha_em), "\\alpha_{em}");
    EXPECT_EQ(latex(sm_input::Gamma_W), "\\Gamma_{W}");
    EXPECT_EQ(latex(sm_input::lambda_wolf), "\\lambda_{\\mathrm{wolf}}");
}

TEST(PhysicsLatex, generatedVertexMomentumIsSubscripted)
{
    EXPECT_EQ(latex(csl::variable_s(
                  csl::prefixedNameWithLatex("p_", std::to_string(1)))),
              "p_{1}");
}

TEST(PhysicsLatex, fermionMassSum)
{
    SM_Model   sm;
    csl::Index al = DiracIndex("\\alpha");
    csl::Index  c    = GaugeIndex("c", sm, "C", sm.getParticle("u"));
    csl::Expr   eMass = csl::GetComplexConjugate(sm.getParticle("e")(al))
                       * sm.getParticle("e")(al);
    csl::Expr  uMass = csl::GetComplexConjugate(sm.getParticle("u")({c, al}))
                       * sm.getParticle("u")({c, al});
    EXPECT_EQ(latex(eMass - uMass), "\\bar{e}(X)e(X) - \\bar{u}_{c}(X)u_{c}(X)");
}
