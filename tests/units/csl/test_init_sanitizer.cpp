#include "csltestdata.h"
#include <csl.h>
#include <csl/initSanitizer.h>
#include <gtest/gtest.h>
#include <cmath>
#include <string.h>

TEST(initSanitizer, create)
{
    csl::InitSanitizer<int> a;
    a.name = "a";
    EXPECT_EQ(strcmp("a", a.name), 0);
    csl::InitSanitizer<float> b("b");
    EXPECT_EQ(strcmp("b", b.name), 0);
    csl::InitSanitizer<float> c("c", 5.5);
    EXPECT_EQ(strcmp("c", c.name), 0);
    EXPECT_EQ(c.get(), 5.5);
}

TEST(initSanitizer, value_management)
{
    csl::InitSanitizer<int> a;
    EXPECT_THROW(a.get(), std::runtime_error);
    EXPECT_THROW((void)(a == 5), std::runtime_error);
    a = 5;
    EXPECT_EQ(a, 5);
    a.reset();
    EXPECT_THROW(static_cast<int>(a), std::runtime_error);
}

TEST(initSanitizer, equality)
{
    csl::InitSanitizer<int> a("a");
    csl::InitSanitizer<int> a2("a2");
    csl::InitSanitizer<float> b("b");
    a = 5;
    a2 = 5;
    b = 6.6;
    EXPECT_EQ(a, a);
    EXPECT_EQ(a, a2);
    EXPECT_EQ(b, b);
    EXPECT_EQ(a, a.get());
    EXPECT_EQ(b, b.get());
    EXPECT_NE(a, static_cast<int>(b));
    EXPECT_NE(a.get(), b);
    EXPECT_EQ(a.get(), a);
    EXPECT_EQ(b.get(), b);
    EXPECT_NE(a.get(), b);
    EXPECT_NE(a, b);
    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);
    EXPECT_NE(a, b);
}

TEST(initSanitizer, comparison)
{
    csl::InitSanitizer<int> a("a");
    csl::InitSanitizer<int> a2("a2");
    csl::InitSanitizer<float> b("b");
    a = 5;
    a2 = 6;
    b = 5.5;
    EXPECT_LT(a, a2);
    EXPECT_GT(a2, a);
    EXPECT_LE(a, a);
    EXPECT_LE(a, a2);
    EXPECT_GE(a2, a2);
    EXPECT_GE(a2, a);
    EXPECT_LT(a, a2.get());
    EXPECT_GT(a2, a.get());
    EXPECT_LE(a, a.get());
    EXPECT_LE(a, a2.get());
    EXPECT_GE(a2, a2.get());
    EXPECT_GE(a2, a.get());
    EXPECT_LT(a.get(), a2);
    EXPECT_GT(a2.get(), a);
    EXPECT_LE(a.get(), a);
    EXPECT_LE(a.get(), a2);
    EXPECT_GE(a2.get(), a2);
    EXPECT_GE(a2.get(), a);
    EXPECT_LT(a, b);
    EXPECT_GT(a2, b);
    EXPECT_LE(a, b);
    EXPECT_GE(a2, b);
    EXPECT_LT(a, b.get());
    EXPECT_GT(a2, b.get());
    EXPECT_LE(a.get(), b);
    EXPECT_GE(a2.get(), b);
}

TEST(initSanitizer, assign)
{
    csl::InitSanitizer<int> a("a");
    csl::InitSanitizer<int> a2("a2");
    csl::InitSanitizer<float> b("b", 5.5);
    EXPECT_EQ(strcmp(a.name, "a"), 0);
    EXPECT_NE(strcmp(a.name, "a2"), 0);
    a = 5;
    a2 = 6;
    EXPECT_EQ(a, 5);
    a = a2;
    EXPECT_EQ(a, 6);
    // Check name is not copied
    EXPECT_EQ(strcmp(a.name, "a"), 0);
    EXPECT_NE(strcmp(a.name, "a2"), 0);
    a = b;
    EXPECT_EQ(strcmp(a.name, "a"), 0);
}

TEST(initSanitizer, operations)
{
    csl::InitSanitizer<int> a("a", 3);
    csl::InitSanitizer<int> a2("a2", 4);
    csl::InitSanitizer<float> b("b", 5.5);
    float res1 = b*(a2 - a)/b;
    EXPECT_NEAR(res1, 1., 1e-5);
    float res2 = (5.5/b) * (a + a2) + 5;
    EXPECT_NEAR(res2, 12., 1e-4);
    float res3 = a2 / (2.*a);
    EXPECT_NEAR(res3, 2./3, 1e-5);
}

TEST(initSanitizer, math_function)
{
    csl::InitSanitizer<int> a("a", 2.);
    csl::InitSanitizer<float> b("b", 2.);
    EXPECT_NEAR(std::pow(a, 2.f), 4.f, 1e-5);
    EXPECT_NEAR(std::pow(b, 2.f), 4.f, 1e-5);
}