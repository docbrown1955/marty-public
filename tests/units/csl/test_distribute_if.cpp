#include <csl.h>
#include <gtest/gtest.h>

using namespace csl;

TEST(DistributeIf, matchedFactorIsCountedOnceWithTwoOtherSums)
{
    Expr x = variable_s("x");
    Expr y = variable_s("y");
    Expr u = variable_s("u");
    Expr v = variable_s("v");
    Expr z = variable_s("z");

    Expr expr     = z * (x + y) * (u + v);
    Expr expected = Expanded(expr, true);

    Expr result = DeepDistributedIf(
        expr, [&z](Expr const &factor) { return factor == z; });

    EXPECT_EQ(Expanded(result, true), expected);
}

TEST(Distribute, matchedFactorIsCountedOnceWithTwoOtherSums)
{
    Expr x = variable_s("x");
    Expr y = variable_s("y");
    Expr u = variable_s("u");
    Expr v = variable_s("v");
    Expr z = variable_s("z");

    Expr expr     = z * (x + y) * (u + v);
    Expr expected = Expanded(expr, true);

    Expr result = Copy(expr);
    DeepDistribute(result, z);

    EXPECT_FALSE(result == expr);
    EXPECT_EQ(Expanded(result, true), expected);
}
