#include <gtest/gtest.h>
#include <marty.h>

TEST(marty_irrep_equality, same_algebra_object)
{
    mty::algebra::A su2(1);
    EXPECT_EQ(su2.highestWeightRep({1}), su2.highestWeightRep({1}));
    EXPECT_NE(su2.highestWeightRep({1}), su2.highestWeightRep({2}));
}

TEST(marty_irrep_equality, distinct_objects_of_same_type_and_rank)
{
    mty::algebra::A spin(1);
    mty::algebra::A weakIsospin(1);
    EXPECT_EQ(spin.highestWeightRep({1}), weakIsospin.highestWeightRep({1}));
    EXPECT_NE(spin.highestWeightRep({1}), weakIsospin.highestWeightRep({2}));
}

TEST(marty_irrep_equality, different_rank_or_type)
{
    mty::algebra::A su2(1);
    mty::algebra::A su3(2);
    mty::algebra::B so5(2);
    EXPECT_NE(su2.highestWeightRep({1}), su3.highestWeightRep({1, 0}));
    EXPECT_NE(su3.highestWeightRep({1, 0}), so5.highestWeightRep({1, 0}));
}
