
#include "csltestdata.h"
#include <csl.h>
#include <gtest/gtest.h>

TEST(csl_tolatex, simple_scalars)
{
    csl::Expr expr = 2*data::a + data::a*data::x;
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2{a}+{a}{x}"
    );
}

TEST(csl_tolatex, latex_scalars)
{
    csl::Expr a = csl::constant_s("a ; \\alpha");
    csl::Expr x = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2*a + a*x;
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2{\\alpha}+{\\alpha}{\\chi}"
    );
}

TEST(csl_tolatex, product_1)
{
    csl::Expr x = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2*x*(1 + x*x)/ 3;
    EXPECT_EQ(
        csl::ToLatex(expr),
        "\\frac{2}{3}{\\chi}\\left(1+{\\chi}^{2}\\right)"
    );
}

TEST(csl_tolatex, product_2)
{
    csl::Expr a = csl::constant_s("a ; \\alpha");
    csl::Expr b = csl::constant_s("b ; \\beta");
    csl::Expr x = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2*a/(2 + x*x) * x / b / 3;
    EXPECT_EQ(
        csl::ToLatex(expr),
        "\\frac{2}{3}\\cdot \\frac{{\\alpha}{\\chi}}{{\\beta}\\left(2+{\\chi}^{2}\\right)}"
    );
}

TEST(csl_tolatex, cdot_before_sum_1)
{
    csl::Expr a = data::a;
    csl::Expr x = data::x;
    csl::Expr expr = 2*(a + x)*(1 + x*x);
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2\\left({a}+{x}\\right)\\left(2+{x}^{2}\\right)"
    );
}

TEST(csl_tolatex, cdot_before_sum_2)
{
    csl::Expr a = data::a;
    csl::Expr x = data::x;
    csl::Expr expr = 2*csl::pow_s(a, 2)*(1 + x*x);
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2{a}^{2}\\cdot\\left(2+{x}^{2}\\right)"
    );
}

TEST(csl_tolatex, no_cdot_before_sum_1)
{
    csl::Expr a = data::a;
    csl::Expr x = data::x;
    csl::Expr expr = 2*a*(1 + x*x)*(a + x);
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2{a}\\left({a}+{x}\\right)\\left(2+{x}^{2}\\right)"
    );
}

TEST(csl_tolatex, no_cdot_before_sum_2)
{
    csl::Expr a = data::a;
    csl::Expr x = data::x;
    csl::Expr expr = 2*(1 + x*x)*(a + x);
    EXPECT_EQ(
        csl::ToLatex(expr),
        "2\\left({a}+{x}\\right)\\left(2+{x}^{2}\\right)"
    );
}