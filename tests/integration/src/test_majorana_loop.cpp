#include "marty.h"
#include <gtest/gtest.h>

namespace {

void ExpectSimplifiedDiagrams(mty::Amplitude const &amplitude)
{
    for (const auto &diagram : amplitude.getDiagrams()) {
        csl::Expr diagExpr = diagram.getExpression();
        csl::Evaluate(diagExpr, csl::eval::abbreviation);
        bool simplified = csl::AllOfLeafs(diagExpr, [](csl::Expr const &sub) {
            return !csl::IsIndicialTensor(sub)
                   || bool(dynamic_cast<mty::QuantumField const *>(sub.get()));
        });
        ASSERT_TRUE(simplified) << "Diagram could not be simplified with "
                                   "Majorana fermion loops: \n"
                                << diagExpr;
    }
}

} // namespace

TEST(majorana_loop, fermion_loops_are_simplified)
{
    mty::option::decomposeInOperators = false;
    mty::Model model(
        {mty::GaugeGroupSpec{mty::group::Type::U, 1, "U_1", "\\gamma"}});

    int           Q      = 1; // U(1) charge of S, P and psi
    mty::Particle psi    = diracfermion_s("\\psi", model);
    mty::Particle lambda = diracfermion_s("\\lambda", model);
    mty::Particle S      = scalarboson_s("S", model);
    mty::Particle P      = scalarboson_s("P", model);

    csl::Expr m = csl::constant_s("m");
    psi->setGroupRep("U_1", Q);
    psi->setMass(m);
    S->setGroupRep("U_1", Q);
    S->setMass(m);
    P->setGroupRep("U_1", Q);
    P->setMass(m);
    lambda->setSelfConjugate(true);

    model.addParticles({psi, lambda, S, P});

    csl::Tensor   delta  = mty::dirac4.getDelta();
    csl::Tensor   gamma5 = mty::dirac4.gamma_chir;
    csl::Tensor   gamma  = mty::dirac4.gamma;
    mty::Particle A      = model.getParticle("\\gamma");
    csl::Index    a1     = mty::DiracIndex();
    csl::Index    a2     = mty::DiracIndex();
    csl::Index    mu     = mty::MinkowskiIndex();
    auto          cc
        = [](csl::Expr const &init) { return csl::GetComplexConjugate(init); };
    csl::Expr SP_term
        = CSL_HALF * Q * Q * csl::pow_s(cc(S) * P - cc(P) * S, 2);
    csl::Expr psiL_term
        = CSL_I * Q * cc(psi(a1))
          * (S * delta({a1, a2}) + CSL_I * gamma5({a1, a2}) * P) * lambda(a2);
    model.addLagrangianTerm(SP_term);
    model.addLagrangianTerm(psiL_term, true);

    model.addLagrangianTerm(CSL_HALF * csl::GetComplexConjugate(lambda(a1))
                            * A(mu) * gamma({+mu, a1, a2}) * lambda(a2));

    auto scalarAmplitude = model.computeAmplitude(mty::Order::OneLoop,
                                                  {Incoming(S),
                                                   Incoming(AntiPart(S)),
                                                   Outgoing(S),
                                                   Outgoing(AntiPart(S))});
    ExpectSimplifiedDiagrams(scalarAmplitude);

    auto pseudoscalarAmplitude
        = model.computeAmplitude(mty::Order::OneLoop,
                                 {Incoming(P),
                                  Incoming(AntiPart(P)),
                                  Outgoing(P),
                                  Outgoing(AntiPart(P))});
    ExpectSimplifiedDiagrams(pseudoscalarAmplitude);
}
