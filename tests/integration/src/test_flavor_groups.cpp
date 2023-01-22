#include "marty.h"
#include <gtest/gtest.h>

TEST(flavor_groups, test_symbolic_flavor)
{
    csl::Expr Nf = csl::constant_s("Nf");
    mty::Model model;
    model.addFlavorGroup("F", Nf);
    model.addGaugedGroup(mty::group::Type::U1, "X");
    model.init();

    mty::Particle p1 = mty::scalarboson_s("p1", model);
    p1->setFundamentalFlavorRep("F");
    p1->setGroupRep("X", 1);
    mty::Particle p2 = mty::scalarboson_s("p2", model);
    p2->setFundamentalFlavorRep("F");
    p2->setGroupRep("X", 1);

    model.addParticles({p1, p2});

    std::cout << model << std::endl;

    csl::Space const *flavorSpace = model.getVectorSpace("F");
    ASSERT_NE(flavorSpace, nullptr);
    ASSERT_EQ(flavorSpace->getName(), "F");
    csl::Index i = model.generateIndex("F");
    ASSERT_EQ(i.getSpace(), flavorSpace);
    ASSERT_EQ(flavorSpace->getSymbolicDim(), Nf);

    {
        mty::Amplitude ampl = model.computeAmplitude(
            mty::Order::TreeLevel,
            {mty::Incoming("p1"), mty::Incoming(mty::AntiPart("p1")),
            mty::Outgoing("p2"), mty::Outgoing(mty::AntiPart("p2"))}
        );
        csl::Expr squared_ampl = model.computeSquaredAmplitude(ampl);
        csl::Expr eval = csl::Evaluated(squared_ampl, csl::eval::abbreviation);
        csl::Expr simplified = csl::DeepHardFactored(csl::DeepExpanded(eval));
        std::cout << simplified << std::endl;

        ASSERT_TRUE(simplified->dependsExplicitlyOn(Nf.get()));
        ASSERT_FALSE((simplified/(Nf*Nf))->dependsExplicitlyOn(Nf.get()));
    }
    {
        mty::Amplitude ampl = model.computeAmplitude(
            mty::Order::OneLoop,
            {mty::Incoming("A_X"), mty::Outgoing("A_X")}
        );
        csl::Expr squared_ampl = model.computeSquaredAmplitude(ampl);
        csl::Expr eval = csl::Evaluated(squared_ampl, csl::eval::abbreviation);
        csl::Expr simplified = csl::DeepHardFactored(csl::DeepExpanded(eval));

        std::cout << simplified << std::endl;
        ASSERT_TRUE(simplified->dependsExplicitlyOn(Nf.get()));
        ASSERT_FALSE((simplified/(Nf*Nf))->dependsExplicitlyOn(Nf.get()));
    }
}

TEST(flavor_groups, test_numerical_flavor)
{
    int Nf = 3;
    mty::Model model;
    model.addFlavorGroup("F", Nf);
    model.addGaugedGroup(mty::group::Type::U1, "X");
    model.init();

    mty::Particle p1 = mty::scalarboson_s("p1", model);
    p1->setFundamentalFlavorRep("F");
    p1->setGroupRep("X", 1);
    mty::Particle p2 = mty::scalarboson_s("p2", model);
    p2->setFundamentalFlavorRep("F");
    p2->setGroupRep("X", 1);

    model.addParticles({p1, p2});

    std::cout << model << std::endl;

    csl::Space const *flavorSpace = model.getVectorSpace("F");
    ASSERT_NE(flavorSpace, nullptr);
    ASSERT_EQ(flavorSpace->getName(), "F");
    csl::Index i = model.generateIndex("F");
    ASSERT_EQ(i.getSpace(), flavorSpace);
    ASSERT_EQ(flavorSpace->getDim(), Nf);
    ASSERT_EQ(static_cast<int>(std::round(flavorSpace->getSymbolicDim()->evaluateScalar())), Nf);

    {
        mty::Amplitude ampl = model.computeAmplitude(
            mty::Order::TreeLevel,
            {mty::Incoming("p1"), mty::Incoming(mty::AntiPart("p1")),
            mty::Outgoing("p2"), mty::Outgoing(mty::AntiPart("p2"))}
        );
        csl::Expr squared_ampl = model.computeSquaredAmplitude(ampl);
        csl::Expr eval = csl::Evaluated(squared_ampl, csl::eval::abbreviation);
        csl::Expr simplified = csl::DeepHardFactored(csl::DeepExpanded(eval));
        std::cout << simplified << std::endl;

        // The following assertion depends to the exact expression form,
        // reevaluate the expected result if some day the assertion fails
        // In particular, compare with the symbolic equivalent above
        ASSERT_EQ(simplified->getNumericalFactor(), CSL_1);
    }
    {
        mty::Amplitude ampl = model.computeAmplitude(
            mty::Order::OneLoop,
            {mty::Incoming("A_X"), mty::Outgoing("A_X")}
        );
        csl::Expr squared_ampl = model.computeSquaredAmplitude(ampl);
        csl::Expr eval = csl::Evaluated(squared_ampl, csl::eval::abbreviation);
        csl::Expr simplified = csl::DeepHardFactored(csl::DeepExpanded(eval));

        std::cout << simplified << std::endl;
        // The following assertion depends to the exact expression form,
        // reevaluate the expected result if some day the assertion fails
        // In particular, compare with the symbolic equivalent above
        ASSERT_EQ(simplified->getNumericalFactor(), 9/CSL_2);
    }
}