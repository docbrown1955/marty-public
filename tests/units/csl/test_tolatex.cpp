
#include "csltestdata.h"
#include <csl.h>
#include <gtest/gtest.h>

TEST(csl_tolatex, simple_scalars)
{
    csl::Expr expr = 2 * data::a + data::a * data::x;
    EXPECT_EQ(csl::ToLatex(expr), "2{a}+{a}{x}");
}

TEST(csl_tolatex, latex_scalars)
{
    csl::Expr a    = csl::constant_s("a ; \\alpha");
    csl::Expr x    = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2 * a + a * x;
    EXPECT_EQ(csl::ToLatex(expr), "2{\\alpha}+{\\alpha}{\\chi}");
}

TEST(csl_tolatex, product_1)
{
    csl::Expr x    = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2 * x * (1 + x * x) / 3;
    EXPECT_EQ(csl::ToLatex(expr),
              "\\frac{2}{3}{\\chi}\\left(1+{\\chi}^{2}\\right)");
}

TEST(csl_tolatex, product_2)
{
    csl::Expr a    = csl::constant_s("a ; \\alpha");
    csl::Expr b    = csl::constant_s("b ; \\beta");
    csl::Expr x    = csl::variable_s("x ; \\chi");
    csl::Expr expr = 2 * a / (2 + x * x) * x / b / 3;
    EXPECT_EQ(csl::ToLatex(expr),
              "\\frac{2}{3}\\cdot "
              "\\frac{{\\alpha}{\\chi}}{{\\beta}\\left(2+{\\chi}^{2}\\right)"
              "}");
}

TEST(csl_tolatex, cdot_before_sum_1)
{
    csl::Expr a    = data::a;
    csl::Expr x    = data::x;
    csl::Expr expr = 2 * (a + x) * (1 + x * x);
    EXPECT_EQ(csl::ToLatex(expr),
              "2\\left({a}+{x}\\right)\\cdot \\left(1+{x}^{2}\\right)");
}

TEST(csl_tolatex, cdot_before_sum_2)
{
    csl::Expr a    = data::a;
    csl::Expr x    = data::x;
    csl::Expr expr = 2 * csl::pow_s(a, 2) * (1 + x * x);
    EXPECT_EQ(csl::ToLatex(expr), "2{a}^{2}\\cdot \\left(1+{x}^{2}\\right)");
}

TEST(csl_tolatex, no_cdot_before_sum_1)
{
    csl::Expr a    = data::a;
    csl::Expr x    = data::x;
    csl::Expr expr = 2 * a * (1 + x * x) * (a + x);
    EXPECT_EQ(csl::ToLatex(expr),
              "2{a}\\left({a}+{x}\\right)\\cdot \\left(1+{x}^{2}\\right)");
}

TEST(csl_tolatex, no_cdot_before_sum_2)
{
    csl::Expr a    = data::a;
    csl::Expr x    = data::x;
    csl::Expr expr = 2 * (1 + x * x) * (a + x);
    EXPECT_EQ(csl::ToLatex(expr),
              "2\\left({a}+{x}\\right)\\cdot \\left(1+{x}^{2}\\right)");
}

TEST(csl_tolatex, indexedTensor)
{
    csl::Tensor g   = csl::Minkowski.getMetric();
    csl::Tensor eps = csl::Minkowski.getEpsilon();
    csl::Index  mu("\\mu", &csl::Minkowski, 0);
    csl::Index  nu("\\nu", &csl::Minkowski, 0);
    csl::Index  rho("\\rho", &csl::Minkowski, 0);
    csl::Index  sigma("\\sigma", &csl::Minkowski, 0);
    EXPECT_EQ(csl::ToLatex(g({mu, nu})), "{g}{}_{\\mu \\nu}");
    EXPECT_EQ(csl::ToLatex(g({+mu, nu})), "{\\delta}{}_{\\nu}{}^{\\mu}");
    EXPECT_EQ(csl::ToLatex(g({mu, +nu})), "{\\delta}{}_{\\mu}{}^{\\nu}");
    EXPECT_EQ(csl::ToLatex(g({+mu, +nu})), "{g}{}^{\\mu \\nu}");
    EXPECT_EQ(csl::ToLatex(eps({+mu, nu, +rho, +sigma})),
              "{\\varepsilon}{}^{\\mu}{}_{\\nu}{}^{\\rho \\sigma}");
}

TEST(csl_tolatex, tensorFields)
{
    csl::Tensor chi("chi ; \\chi", &csl::Minkowski);
    chi->setComplexProperty(csl::ComplexProperty::Complex);
    csl::TensorField Psi(
        "psi ; \\psi", &csl::Minkowski, {&csl::Euclid_R3, &csl::Euclid_R3});
    Psi->setComplexProperty(csl::ComplexProperty::Complex);
    csl::Index mu("\\mu", &csl::Minkowski, 0);
    csl::Index i("i", &csl::Euclid_R3, 0);
    csl::Index j("j_{test_latex}", &csl::Euclid_R3, 0);
    EXPECT_EQ(csl::ToLatex(Psi({i, j}, chi)),
              "{\\psi}({\\chi}){}_{i j_{test_latex}}");
    EXPECT_EQ(csl::ToLatex(csl::GetComplexConjugate(Psi({i, j}, chi))),
              "{\\psi}({\\chi})^{*}{}_{i j_{test_latex}}");
    EXPECT_EQ(csl::ToLatex(csl::GetComplexConjugate(chi(+mu))),
              "{\\chi}^{*}{}^{\\mu}");
}

TEST(csl_tolatex, tensorialDerivative)
{
    csl::Tensor      X("X ; X_\\eta", &csl::Minkowski);
    csl::Index       mu("\\mu", &csl::Minkowski, 0);
    csl::TDerivative partialMinko = csl::tderivative_s("d", &csl::Minkowski);
    csl::Expr        expr         = partialMinko(+mu, X) * X(mu);
    EXPECT_EQ(csl::ToLatex(expr),
              "\\frac{\\partial^{\\mu}}{\\partial "
              "X_\\eta}\\left({X_\\eta}{}_{\\mu}\\right)");
}