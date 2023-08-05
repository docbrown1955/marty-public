#include "marty.h"
#include <gtest/gtest.h>

TEST(bidiagonalization, weyl_single_character)
{
    mty::Model model;
    model.init();
    mty::Particle l = mty::weylfermion_s("l", model, mty::Chirality::Left);
    mty::Particle r = mty::weylfermion_s("r", model, mty::Chirality::Right);
    mty::Particle L = mty::weylfermion_s("L", model, mty::Chirality::Left);
    mty::Particle R = mty::weylfermion_s("R", model, mty::Chirality::Right);
    model.addParticles({l, r, L, R});
    model.birotateFields({"l", "L"}, {"r", "R"});
    EXPECT_EQ(l->getMass().get(), r->getMass().get());
    EXPECT_EQ(l->getMass()->getName(), "m_l");
    EXPECT_EQ(L->getMass().get(), R->getMass().get());
    EXPECT_EQ(L->getMass()->getName(), "m_L");
}

TEST(bidiagonalization, weyl_multi_characters)
{
    mty::Model model;
    model.init();
    mty::Particle l = mty::weylfermion_s("al", model, mty::Chirality::Left);
    mty::Particle r = mty::weylfermion_s("br", model, mty::Chirality::Right);
    mty::Particle L = mty::weylfermion_s("cL", model, mty::Chirality::Left);
    mty::Particle R = mty::weylfermion_s("dR", model, mty::Chirality::Right);
    model.addParticles({l, r, L, R});
    model.birotateFields({"al", "cL"}, {"br", "dR"});
    EXPECT_EQ(l->getMass().get(), r->getMass().get());
    EXPECT_EQ(l->getMass()->getName(), "m_al"); // No rule for *l
    EXPECT_EQ(L->getMass().get(), R->getMass().get());
    EXPECT_EQ(L->getMass()->getName(), "m_c"); // Rule for *L
}

TEST(bidiagonalization, weyl_multi_characters_underscore)
{
    mty::Model model;
    model.init();
    mty::Particle l = mty::weylfermion_s("a_l", model, mty::Chirality::Left);
    mty::Particle r = mty::weylfermion_s("b_r", model, mty::Chirality::Right);
    mty::Particle L = mty::weylfermion_s("c_L", model, mty::Chirality::Left);
    mty::Particle R = mty::weylfermion_s("d_R", model, mty::Chirality::Right);
    model.addParticles({l, r, L, R});
    model.birotateFields({"a_l", "c_L"}, {"b_r", "d_R"});
    EXPECT_EQ(l->getMass().get(), r->getMass().get());
    EXPECT_EQ(l->getMass()->getName(), "m_a");
    EXPECT_EQ(L->getMass().get(), R->getMass().get());
    EXPECT_EQ(L->getMass()->getName(), "m_c");
}