#include <marty.h>
#include "testutility.h"

using namespace std;

csl::Expr cc(csl::Expr const &init)
{
    return csl::GetComplexConjugate(init);
}

void Check(std::vector<csl::Expr> const &ampl)
{
    for (const auto& a : ampl)
        if (csl::AnyOfLeafs(a, [&](csl::Expr const &sub)
            {
                return csl::IsITensor(sub)
                    and mty::dirac4.isGammaTensor(sub);
            })) 
        {
            if (csl::IsSum(a))
                mty::Display(a->getVectorArgument());
            else
                std::cout << a << std::endl;
            exit(1);
        }
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    mty::option::decomposeInOperators = false;
    mty::Model model;
    model.addGaugedGroup(mty::group::Type::U1, "U_1");
    model.init();
    model.renameParticle("A_U_1", "\\gamma");

    int Q = 1; // U(1) charge of S, P and psi
    mty::Particle psi    = diracfermion_s("\\psi", model);
    mty::Particle lambda = diracfermion_s("\\lambda", model);
    mty::Particle S = scalarboson_s("S", model);
    mty::Particle P = scalarboson_s("P", model);

    csl::Expr m = csl::constant_s("m");
    psi->setGroupRep("U_1", Q);
    psi->setMass(m);
    S->setGroupRep("U_1", Q);
    S->setMass(m);
    P->setGroupRep("U_1", Q);
    P->setMass(m);
    lambda->setSelfConjugate(true);

    model.addParticles({psi, lambda, S, P});

    csl::Tensor delta = mty::dirac4.getDelta();
    csl::Tensor gamma5 = mty::dirac4.gamma_chir;
    csl::Tensor gamma = mty::dirac4.gamma;
    mty::Particle A = model.getParticle("\\gamma");
    csl::Index a1 = mty::DiracIndex();
    csl::Index a2 = mty::DiracIndex();
    csl::Index mu = mty::MinkowskiIndex();
    csl::Expr SP_term = CSL_HALF * Q * Q * csl::pow_s(cc(S)*P - cc(P)*S, 2);
    csl::Expr psiL_term = 
        CSL_I * Q * cc(psi(a1)) * 
        (
            S * delta({a1, a2}) 
            + CSL_I * gamma5({a1, a2}) * P
        ) 
        * lambda(a2);
    model.addLagrangianTerm(SP_term);
    model.addLagrangianTerm(psiL_term, true);

    model.addLagrangianTerm(
            CSL_HALF*csl::GetComplexConjugate(lambda(a1))*A(mu)*gamma({+mu,a1,a2})*lambda(a2)
            );

    std::cout << model << std::endl;

    auto feynmanRules = model.getFeynmanRules();

    Display(feynmanRules);

    auto majoranaLoop = model.computeAmplitude(
            mty::Order::OneLoop,
            {Incoming(S), Incoming(AntiPart(S)),
            Outgoing(S), Outgoing(AntiPart(S))}
            );

    Display(majoranaLoop);

    for (const auto &diagram : majoranaLoop.getDiagrams()) {
        csl::Expr diagExpr = diagram.getExpression();
        csl::Evaluate(diagExpr, csl::eval::abbreviation);
        auto OK = [&](csl::Expr const &expr) {
            return csl::AllOfLeafs(expr, [&](csl::Expr const &sub) {
                return !csl::IsIndicialTensor(sub)
                    || bool(dynamic_cast<mty::QuantumField const*>(sub.get()));
            });
        };
        if (!OK(diagExpr)) {
            error(buildMessage("Diagram could not be simplified",
                        " with Majorana fermion loops: \n", diagExpr));
            return 1;
        }
    }

    majoranaLoop = model.computeAmplitude(
            mty::Order::OneLoop,
            {Incoming(P), Incoming(AntiPart(P)),
            Outgoing(P), Outgoing(AntiPart(P))}
            );

    Display(majoranaLoop);
    //Show(majoranaLoop);

    for (const auto &diagram : majoranaLoop.getDiagrams()) {
        csl::Expr diagExpr = diagram.getExpression();
        csl::Evaluate(diagExpr, csl::eval::abbreviation);
        auto OK = [&](csl::Expr const &expr) {
            return csl::AllOfLeafs(expr, [&](csl::Expr const &sub) {
                return !csl::IsIndicialTensor(sub)
                    || bool(dynamic_cast<mty::QuantumField const*>(sub.get()));
            });
        };
        if (!OK(diagExpr)) {
            error(buildMessage("Diagram could not be simplified",
                        " with Majorana fermion loops: \n", diagExpr));
            return 1;
        }
    }
    success("Majorana fermion loops calculated.");

    return 0;
}
