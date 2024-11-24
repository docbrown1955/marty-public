#include "marty.h"
#include <gtest/gtest.h>

TEST(specific_gauge_couplings, scalar)
{
    mty::Model sQED;
    csl::Expr  e = csl::constant_s("e");
    sQED.addGaugedGroup(mty::group::Type::U1, "em", e);
    sQED.init();

    sQED.renameParticle("A_em", "A");

    mty::Particle A   = sQED.getParticle("A");
    mty::Particle phi = mty::scalarboson_s("phi ; \\phi", sQED);
    csl::Expr     m   = csl::constant_s("m");
    csl::Expr     l   = csl::constant_s("l");
    phi->setMass(m);
    phi->setGroupRep("em", -1);
    phi->setSpecificGaugeCoupling("em", l);
    sQED.addParticle(phi);

    std::cout << sQED << std::endl;

    bool term_found = false;
    for (const auto &term : sQED.getLagrangian().interaction) {
        if (term->count(phi.get()) == 2 && term->count(A.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&l](const csl::Expr &el) { return el == l; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &el) { return el == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);
}

TEST(specific_gauge_couplings, scalar_quadratic_term)
{
    mty::Model sQED;
    csl::Expr  e = csl::constant_s("e");
    sQED.addGaugedGroup(mty::group::Type::U1, "em", e);
    sQED.init();

    sQED.renameParticle("A_em", "A");

    mty::Particle A   = sQED.getParticle("A");
    mty::Particle phi = mty::scalarboson_s("phi ; \\phi", sQED);
    csl::Expr     m   = csl::constant_s("m");
    csl::Expr     l   = csl::constant_s("l");
    phi->setMass(m);
    phi->setGroupRep("em", -1);
    phi->setSpecificGaugeCoupling("em", l);
    sQED.addParticle(phi);

    std::cout << sQED << std::endl;

    bool term_found = false;
    for (const auto &term : sQED.getLagrangian().interaction) {
        if (term->count(phi.get()) == 2 && term->count(A.get()) == 2) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found
                = csl::FirstOfNode(factor, [&l](const csl::Expr &el) {
                      return el == csl::pow_s(l, 2);
                  });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found
                = csl::FirstOfNode(factor, [&e](const csl::Expr &el) {
                      return el == csl::pow_s(e, 2);
                  });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);
}

TEST(specific_gauge_coupling, fermion)
{
    csl::Expr  e = csl::constant_s("e");
    mty::Model QED;
    QED.addGaugedGroup(mty::group::Type::U1, "em", e);
    QED.init();

    QED.renameParticle("A_em", "A");
    mty::Particle A = QED.getParticle("A");

    mty::Particle psi = mty::diracfermion_s("\\psi", QED);
    csl::Expr     m   = csl::constant_s("m");
    psi->setGroupRep("em", -1);
    psi->setMass(m);
    csl::Expr l = csl::constant_s("l");
    psi->setSpecificGaugeCoupling("em", l);
    QED.addParticle(psi);

    std::cout << QED << std::endl;

    bool term_found = false;
    for (const auto &term : QED.getLagrangian().interaction) {
        if (term->count(psi.get()) == 2 && term->count(A.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&l](const csl::Expr &el) { return el == l; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &el) { return el == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);
}

TEST(specific_gauge_coupling, fermion_multi)
{
    csl::Expr  e = csl::constant_s("e");
    mty::Model QED;
    QED.addGaugedGroup(mty::group::Type::U1, "em", e);
    QED.init();

    QED.renameParticle("A_em", "A");
    mty::Particle A = QED.getParticle("A");

    mty::Particle el = mty::diracfermion_s("e", QED);
    el->setGroupRep("em", -1); // Charge -1
    csl::Expr C_e
        = csl::constant_s("C_e"); // Define coupling constant for electron
    el->setSpecificGaugeCoupling("em", C_e);

    mty::Particle mu = mty::diracfermion_s("mu", QED);
    mu->setGroupRep("em", -1); // Charge -1
    csl::Expr C_mu
        = csl::constant_s("C_mu"); // Define coupling constant for electron
    mu->setSpecificGaugeCoupling("em", C_mu);

    mty::Particle tau = mty::diracfermion_s("tau", QED);
    tau->setGroupRep("em", -1); // Charge -1

    QED.addParticles({el, mu, tau});

    std::cout << QED << std::endl;

    // Electron part
    bool term_found = false;
    for (const auto &term : QED.getLagrangian().interaction) {
        if (term->count(el.get()) == 2 && term->count(A.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&C_e](const csl::Expr &ex) { return ex == C_e; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &ex) { return ex == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);

    // Muon part
    term_found = false;
    for (const auto &term : QED.getLagrangian().interaction) {
        if (term->count(mu.get()) == 2 && term->count(A.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&C_mu](const csl::Expr &ex) { return ex == C_mu; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &ex) { return ex == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);

    // Tau part
    term_found = false;
    for (const auto &term : QED.getLagrangian().interaction) {
        if (term->count(tau.get()) == 2 && term->count(A.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&C_mu](const csl::Expr &ex) { return ex == C_mu; });
            ASSERT_FALSE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &ex) { return ex == e; });
            ASSERT_TRUE(normal_coupling_found);

            break;
        }
    }
}

TEST(specific_gauge_coupling, vector)
{
    csl::Expr  e = csl::constant_s("e");
    mty::Model toyVectorModel;
    toyVectorModel.addGaugedGroup(mty::group::Type::U1, "em", e);
    toyVectorModel.init();

    toyVectorModel.renameParticle("A_em", "A");
    mty::Particle A = toyVectorModel.getParticle("A");

    mty::Particle B   = mty::vectorboson_s("B", toyVectorModel);
    mty::Particle F_B = B->getFieldStrength();
    csl::Expr     m   = csl::constant_s("m");
    B->setGroupRep("em", -1);
    B->setMass(m);
    csl::Expr l = csl::constant_s("l");
    B->setSpecificGaugeCoupling("em", l);
    toyVectorModel.addParticle(B);

    std::cout << toyVectorModel << std::endl;

    bool term_found = false;
    for (const auto &term : toyVectorModel.getLagrangian().interaction) {
        if (term->count(B.get()) == 1 && term->count(A.get()) == 1
            && term->count(F_B.get()) == 1) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&l](const csl::Expr &el) { return el == l; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &el) { return el == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);
}

TEST(specific_gauge_coupling, vector_quadratic_term)
{
    csl::Expr  e = csl::constant_s("e");
    mty::Model toyVectorModel;
    toyVectorModel.addGaugedGroup(mty::group::Type::U1, "em", e);
    toyVectorModel.init();

    toyVectorModel.renameParticle("A_em", "A");
    mty::Particle A = toyVectorModel.getParticle("A");

    mty::Particle B = mty::vectorboson_s("B", toyVectorModel);
    csl::Expr     m = csl::constant_s("m");
    B->setGroupRep("em", -1);
    B->setMass(m);
    csl::Expr l = csl::constant_s("l");
    B->setSpecificGaugeCoupling("em", l);
    toyVectorModel.addParticle(B);

    std::cout << toyVectorModel << std::endl;

    bool term_found = false;
    for (const auto &term : toyVectorModel.getLagrangian().interaction) {
        if (term->count(B.get()) == 2 && term->count(A.get()) == 2) {
            term_found       = true;
            csl::Expr factor = term->getGlobalFactor();

            bool specific_coupling_found = csl::FirstOfLeaf(
                factor, [&l](const csl::Expr &el) { return el == l; });
            ASSERT_TRUE(specific_coupling_found);

            bool normal_coupling_found = csl::FirstOfLeaf(
                factor, [&e](const csl::Expr &el) { return el == e; });
            ASSERT_FALSE(normal_coupling_found);

            break;
        }
    }
    ASSERT_TRUE(term_found);
}