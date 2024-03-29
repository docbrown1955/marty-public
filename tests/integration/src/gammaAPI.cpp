#include <gtest/gtest.h>
#include <marty.h>
#include <marty/api/gamma.h>
#include <marty/sgl/sgl.h>
#include <set>

using namespace mty::gamma_api;
constexpr auto    expr_s = sgl::cslexpr_s;
static const auto D      = DMinko();

const static sgl::TensorSet tensorSet
    = {mty::dirac4.gamma_chir,
       mty::dirac4.C_matrix,
       mty::dirac4.P_L,
       mty::dirac4.P_R,
       std::map<size_t, csl::Tensor>{{0, mty::dirac4.getDelta()},
                                     {1, mty::dirac4.gamma},
                                     {2, mty::dirac4.sigma}}};

csl::Expr comparableExpr(Expr const &expr)
{
    return sgl::sgl_to_csl(expr, tensorSet);
}

#define COMPARE_SGL_EXPR(ASSERT_MACRO, lhs, rhs) \
    ASSERT_MACRO(comparableExpr(lhs), comparableExpr(rhs))

#define TEST_ALL_EPSILON_COMBINATIONS                                \
    for (int m0 = 0; m0 != 4; ++m0)                                  \
        for (int m1 = 0; m1 != 4; ++m1)                              \
            for (int m2 = 0; m2 != 4; ++m2)                          \
                for (int m3 = 0; m3 != 4; ++m3)                      \
                    if (m0 != m1 && m0 != m2 && m0 != m3 && m1 != m2 \
                        && m1 != m3 && m2 != m3)

TEST(marty_gamma_api, equality)
{
    keepSymbolic4D(false);
    COMPARE_SGL_EXPR(EXPECT_NE, expr_s(0), expr_s(1));
    COMPARE_SGL_EXPR(EXPECT_NE, g(0, 1), g(0, 2));
    COMPARE_SGL_EXPR(EXPECT_NE, g(0, 0), g(0, 1));
    COMPARE_SGL_EXPR(EXPECT_NE, eps(0, 1, 2, 3), eps(1, 0, 2, 3));
    COMPARE_SGL_EXPR(EXPECT_NE, g(0, 1), eps(0, 1, 2, 3));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma(0)}, 0, 1), current({gamma(1)}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma(0)}, 0, 1), current({gamma(0)}, 0, 2));
    COMPARE_SGL_EXPR(EXPECT_NE, current({gamma(0)}, 0, 1), current({}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma(0)}, 0, 1), current({gamma5()}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma5()}, 0, 1), current({P_L()}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({P_L()}, 0, 1), current({P_R()}, 0, 1));
    COMPARE_SGL_EXPR(EXPECT_NE, current({P_R()}, 0, 1), current({C()}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma(0)}, 0, 1), current({gamma(0, 1)}, 0, 1));
    COMPARE_SGL_EXPR(
        EXPECT_NE, current({gamma(1, 0)}, 0, 1), current({gamma(0, 1)}, 0, 1));
    keepSymbolic4D(true);
    COMPARE_SGL_EXPR(EXPECT_NE, D, expr_s(4));
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(D), expr_s(4));
    keepSymbolic4D(false);
    COMPARE_SGL_EXPR(EXPECT_NE, D, expr_s(4));
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(D), expr_s(4));
}

static int sign_of(std::array<int, 4> eps_indices)
{
    if (std::set(eps_indices.begin(), eps_indices.end()).size() < 4) {
        // identical indices
        return 0;
    }
    int sign = 1;
    for (int i = 0; i != 4; ++i) {
        int i_min = i;
        for (int j = i + 1; j != 4; ++j) {
            if (eps_indices[j] < eps_indices[i_min])
                i_min = j;
        }
        if (i != i_min) {
            std::swap(eps_indices[i], eps_indices[i_min]);
            sign *= -1;
        }
    }
    return sign;
}

TEST(marty_gamma_api, metric_contractions)
{
    keepSymbolic4D(false);
    {
        auto expr        = g(0, 1) * g(0, 2);
        auto expr_simpli = g(1, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    {
        auto expr        = g(1, 0) * g(0, 2);
        auto expr_simpli = g(1, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    {
        auto expr        = g(0, 1) * g(2, 3);
        auto expr_simpli = g(0, 1) * g(2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    {
        keepSymbolic4D(true);
        auto expr        = g(0, 1) * g(0, 1);
        auto expr_simpli = D;
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
}

TEST(marty_gamma_api, epsilon_contractions)
{

    TEST_ALL_EPSILON_COMBINATIONS
    {
        auto expr = eps(m0, m1, m2, m2);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_s(0));
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        auto expr = g(m2, m3) * eps(m0, m1, m2, m3);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_s(0));
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        keepSymbolic4D(true);
        auto expr        = simplified(eps(0, 1, 2, 3) * eps(m0, m1, m2, m3));
        auto expr_simpli = -1 // -1 because eps(0123)*eps(+0123) = -1
                           * sign_of({m0, m1, m2, m3})
                           * simplified(D * (D - 1) * (D - 2) * (D - 3));
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        keepSymbolic4D(false);
        auto expr        = simplified(eps(0, 1, 2, 3) * eps(m0, m1, m2, m3));
        auto expr_simpli = -24 // -1 because eps(0123)*eps(+0123) = -1
                           * sign_of({m0, m1, m2, m3});
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_s(expr_simpli));
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        keepSymbolic4D(false);
        auto expr        = simplified(eps(0, 1, 2, 4) * eps(m0, m1, m2, m3));
        auto expr_simpli = -6 // -1 because eps(0123)*eps(+0123) = -1
                           * sign_of({m0, m1, m2, m3}) * g(3, 4);
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        keepSymbolic4D(false);
        auto expr        = simplified(eps(0, 1, 4, 5) * eps(m0, m1, m2, m3));
        auto expr_simpli = -2 // -1 because eps(0123)*eps(+0123) = -1
                           * sign_of({m0, m1, m2, m3})
                           * (g(2, 4) * g(3, 5) - g(2, 5) * g(3, 4));
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
    TEST_ALL_EPSILON_COMBINATIONS
    {
        keepSymbolic4D(false);
        auto expr = simplified(eps(0, 4, 5, 6) * eps(m0, m1, m2, m3));
        auto expr_simpli
            = -1 // -1 because eps(0123)*eps(+0123) = -1
              * sign_of({m0, m1, m2, m3})
              * (g(1, 4) * g(2, 5) * g(3, 6) - g(1, 4) * g(2, 6) * g(3, 5)
                 - g(1, 5) * g(2, 4) * g(3, 6) + g(1, 5) * g(2, 6) * g(3, 4)
                 - g(1, 6) * g(2, 5) * g(3, 4) + g(1, 6) * g(2, 4) * g(3, 5));
        COMPARE_SGL_EXPR(EXPECT_EQ, expr, expr_simpli);
    }
}

TEST(marty_gamma_api, gamma_contractions)
{
    keepSymbolic4D(false);
    {
        auto c        = current({gamma(0), gamma(0)}, 0, 1);
        auto c_simpli = expr_s(4) * current({}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({gamma5(), gamma5()}, 0, 1);
        auto c_simpli = current({}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({P_L(), P_L()}, 0, 1);
        auto c_simpli = current({P_L()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({P_R(), P_R()}, 0, 1);
        auto c_simpli = current({P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({P_L(), P_R()}, 0, 1);
        auto c_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({gamma5(), P_R()}, 0, 1);
        auto c_simpli = current({P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({gamma5(), P_L()}, 0, 1);
        auto c_simpli = expr_s(-1) * current({P_L()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c        = current({C(), C()}, 0, 1);
        auto c_simpli = expr_s(-1) * current({}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        keepSymbolic4D(true);
        auto c        = current({gamma(0), gamma(0, 1)}, 0, 1);
        auto c_simpli = (D - 1) * current({gamma(1)}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        keepSymbolic4D(true);
        auto c        = current({gamma(1), gamma(0, 1)}, 0, 1);
        auto c_simpli = (1 - D) * current({gamma(0)}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        keepSymbolic4D(true);
        auto c        = current({gamma(0, 1), gamma(0, 1)}, 0, 1);
        auto c_simpli = D * (1 - D) * current({}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        keepSymbolic4D(true);
        auto c        = current({gamma(0, 1), gamma(1, 0)}, 0, 1);
        auto c_simpli = D * (D - 1) * current({}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
}

TEST(marty_gamma_api, simple_simplification)
{
    keepSymbolic4D(true);

    auto c0            = current({gamma(0), gamma(0)}, 0, 1);
    auto c0_simpli     = D * current({}, 0, 1);
    auto c0_bad_simpli = expr_s(4) * current({}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c0), c0_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c0), c0_bad_simpli);

    auto c1        = current({gamma(0), gamma(1), gamma(0)}, 0, 1);
    auto c1_simpli = expr_s(2) * current({gamma(1)}, 0, 1)
                     - D * current({gamma(1)}, 0, 1);
    auto c1_bad_simpli = expr_s(2) * current({gamma(0)}, 0, 1)
                         - D * current({gamma(0)}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c1), c1_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c1), c1_bad_simpli);

    auto c2            = current({gamma(0), gamma5(), gamma(0)}, 0, 1);
    auto c2_simpli     = -D * current({gamma5()}, 0, 1);
    auto c2_bad_simpli = -D * current({gamma5()}, 0, 2);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c2), c2_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c2), c2_bad_simpli);

    auto c3            = current({gamma5(), gamma(0)}, 0, 1);
    auto c3_simpli     = expr_s(-1) * current({gamma(0), gamma5()}, 0, 1);
    auto c3_bad_simpli = expr_s(-1) * current({gamma(1), gamma5()}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c3), c3_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c3), c3_bad_simpli);

    auto c4 = current(
        {gamma5(), gamma(1), P_L(), gamma5(), gamma(0), P_R(), gamma5()},
        0,
        1);
    auto c4_simpli = expr_s(-1) * current({gamma(1), gamma(0), P_R()}, 0, 1);
    auto c4_bad_simpli
        = expr_s(-1) * current({gamma(1), gamma5(), P_L()}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c4), c4_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c4), c4_bad_simpli);

    auto c5 = current(
        {gamma5(), gamma(1), P_L(), C(), C(), gamma(0), P_R()}, 0, 1);
    auto c5_simpli = expr_s(-1) * current({gamma(1), gamma(0), P_R()}, 0, 1);
    auto c5_bad_simpli
        = expr_s(-1) * current({gamma(1), gamma5(), P_L()}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c5), c5_simpli);
    COMPARE_SGL_EXPR(EXPECT_NE, simplified(c5), c5_bad_simpli);
}

TEST(marty_gamma_api, basis_projection)
{
    // Project to chiral basis
    {
        auto c         = current({gamma(0)}, 0, 1);
        auto projected = project(c, FierzBasis::Chiral);
        auto c_proj    = current({gamma(0), P_L()}, 0, 1)
                      + current({gamma(0), P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({gamma(0), gamma5()}, 0, 1);
        auto projected = project(c, FierzBasis::Chiral);
        auto c_proj    = -current({gamma(0), P_L()}, 0, 1)
                      + current({gamma(0), P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({}, 0, 1);
        auto projected = project(c, FierzBasis::Chiral);
        auto c_proj    = current({P_L()}, 0, 1) + current({P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({gamma5()}, 0, 1);
        auto projected = project(c, FierzBasis::Chiral);
        auto c_proj    = -current({P_L()}, 0, 1) + current({P_R()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }

    // Project to standard basis
    {
        auto c         = current({P_L()}, 0, 1);
        auto projected = project(c, FierzBasis::Standard);
        auto c_proj    = expr_s(CSL_HALF) * current({}, 0, 1)
                      - expr_s(CSL_HALF) * current({gamma5()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({P_R()}, 0, 1);
        auto projected = project(c, FierzBasis::Standard);
        auto c_proj    = expr_s(CSL_HALF) * current({}, 0, 1)
                      + expr_s(CSL_HALF) * current({gamma5()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({gamma(0), P_L()}, 0, 1);
        auto projected = project(c, FierzBasis::Standard);
        auto c_proj    = expr_s(CSL_HALF) * current({gamma(0)}, 0, 1)
                      - expr_s(CSL_HALF) * current({gamma(0), gamma5()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
    {
        auto c         = current({gamma(0, 1), gamma(2), P_R()}, 0, 1);
        auto projected = project(c, FierzBasis::Standard);
        auto c_proj = expr_s(CSL_HALF) * current({gamma(0, 1), gamma(2)}, 0, 1)
                      + expr_s(CSL_HALF)
                            * current({gamma(0, 1), gamma(2), gamma5()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, projected, c_proj);
    }
}

TEST(marty_gamma_api, simple_traces)
{
    keepSymbolic4D(true);
    {
        auto tr        = trace({});
        auto tr_simpli = expr_s(4);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0), gamma(1)});
        auto tr_simpli = 4 * g(0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0, 1)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0), gamma(1), gamma(2)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr = trace({gamma(0), gamma(1), gamma(2), gamma(3)});
        auto tr_simpli
            = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3));
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0, 1), gamma(2, 3)});
        auto tr_simpli = 4 * (g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3));
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma5()});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({P_L()});
        auto tr_simpli = expr_s(2);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({P_R()});
        auto tr_simpli = expr_s(2);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        EXPECT_THROW(trace({C()}), sgl::MathError);
    }
    {
        EXPECT_THROW(trace({gamma(0), C()}), sgl::MathError);
    }
    {
        EXPECT_THROW(trace({gamma(0), gamma5(), C()}), sgl::MathError);
    }
    {
        auto tr        = trace({gamma(0), gamma5()});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0), gamma(1), gamma5()});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr        = trace({gamma(0), gamma(1), gamma(2), gamma5()});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr = trace({gamma(0), gamma(1), gamma(2), gamma(3), gamma5()});
        auto tr_simpli = expr_s(-4 * CSL_I) * eps(0, 1, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
}

TEST(marty_gamma_api, complex_traces)
{
    keepSymbolic4D(false);
    {
        auto tr = 2 * trace({gamma(0), gamma(1), gamma(2), gamma(3), P_L()});
        auto tr_simpli
            = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3))
              - expr_s(-4 * CSL_I) * eps(0, 1, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr = 2 * trace({gamma(0), gamma(1), gamma(2), gamma(3), P_R()});
        auto tr_simpli
            = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3))
              + expr_s(-4 * CSL_I) * eps(0, 1, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr = 2 * trace({gamma(2), gamma(3), P_L(), gamma(0), gamma(1)});
        auto tr_simpli
            = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3))
              - expr_s(-4 * CSL_I) * eps(0, 1, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr = 2 * trace({gamma(2), gamma(3), P_R(), gamma(0), gamma(1)});
        auto tr_simpli
            = 4 * (g(0, 1) * g(2, 3) + g(0, 3) * g(1, 2) - g(0, 2) * g(1, 3))
              + expr_s(-4 * CSL_I) * eps(0, 1, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr
            = 2
              * trace({gamma(2), gamma(3), P_R(), gamma(0), P_R(), gamma(1)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr
            = 2
              * trace({gamma(2), gamma(3), P_R(), P_L(), gamma(0), gamma(1)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }
    {
        auto tr
            = 2 * trace({gamma(2), gamma(3), gamma(0), gamma(1), gamma(4)});
        auto tr_simpli = expr_s(0);
        COMPARE_SGL_EXPR(EXPECT_EQ, tr, tr_simpli);
    }

    // 6-gamma matrices chiral traces
    Expr D      = expr_s(4);
    Expr factor = expr_s(-4 * CSL_I);
    {
        auto tr = g(0, 1)
                  * trace({gamma(0),
                           gamma(1),
                           gamma(2),
                           gamma(3),
                           gamma(4),
                           gamma(5),
                           gamma5()});
        auto tr_simpli = D * factor * eps(2, 3, 4, 5);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(tr), tr_simpli);
    }
    {
        auto tr = g(0, 2)
                  * trace({gamma(0),
                           gamma(1),
                           gamma(2),
                           gamma(3),
                           gamma(4),
                           gamma(5),
                           gamma5()});
        auto tr_simpli = (2 - D) * factor * eps(1, 3, 4, 5);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(tr), tr_simpli);
    }
    {
        auto tr = g(0, 3)
                  * trace({gamma(0),
                           gamma(1),
                           gamma(2),
                           gamma(3),
                           gamma(4),
                           gamma(5),
                           gamma5()});
        auto tr_simpli = expr_s(CSL_0); // this trace cancels
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(tr), tr_simpli);
    }
    {
        auto tr = g(0, 4)
                  * trace({gamma(0),
                           gamma(1),
                           gamma(2),
                           gamma(3),
                           gamma(4),
                           gamma(5),
                           gamma5()});
        auto tr_simpli = (D - 2) * factor * eps(1, 2, 3, 5);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(tr), tr_simpli);
    }
    {
        auto tr = g(0, 5)
                  * trace({gamma(0),
                           gamma(1),
                           gamma(2),
                           gamma(3),
                           gamma(4),
                           gamma(5),
                           gamma5()});
        auto tr_simpli = -D * factor * eps(1, 2, 3, 4);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(tr), tr_simpli);
    }
}

TEST(marty_gamma_api, chisholm_identities)
{
    keepSymbolic4D(false);

    // Simple chisholm identity
    TEST_ALL_EPSILON_COMBINATIONS
    {
        csl::ScopedProperty prop(&sgl::option::orderEspilonIndices, false);
        auto c = (1 * eps(m0, m1, m2, m3)) * current({gamma(m3)}, 0, 1);
        auto c_simpli
            = -expr_s(CSL_I)
                  * current({gamma(m0), gamma(m1), gamma(m2), gamma5()}, 0, 1)
              + expr_s(CSL_I) * g(m0, m1)
                    * current({gamma(m2), gamma5()}, 0, 1)
              - expr_s(CSL_I) * g(m0, m2)
                    * current({gamma(m1), gamma5()}, 0, 1)
              + expr_s(CSL_I) * g(m1, m2)
                    * current({gamma(m0), gamma5()}, 0, 1);
        auto c_bad_simpli
            = -expr_s(CSL_I)
                  * current({gamma(m0), gamma(m1), gamma(m2), gamma5()}, 0, 1)
              - expr_s(CSL_I) * g(m0, m1)
                    * current({gamma(m2), gamma5()}, 0, 1)
              + expr_s(CSL_I) * g(m0, m2)
                    * current({gamma(m1), gamma5()}, 0, 1)
              - expr_s(CSL_I) * g(m1, m2)
                    * current({gamma(m0), gamma5()}, 0, 1);

        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), simplified(c_simpli));
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_bad_simpli);
    }

    // Chisholm identity with sigma tensor
    TEST_ALL_EPSILON_COMBINATIONS
    {
        auto c        = eps(m0, m1, m2, m3) * current({gamma(m3, 4)}, 0, 1);
        auto c_simpli = expr_s(CSL_I) * g(m0, 4)
                            * current({gamma(m1, m2), gamma5()}, 0, 1)
                        - expr_s(CSL_I) * g(m1, 4)
                              * current({gamma(m0, m2), gamma5()}, 0, 1)
                        + expr_s(CSL_I) * g(m2, 4)
                              * current({gamma(m0, m1), gamma5()}, 0, 1);
        auto c_bad_simpli
            = expr_s(CSL_I) * g(m0, 4) * current({gamma(m1, m2)}, 0, 1)
              - expr_s(CSL_I) * g(m1, 4) * current({gamma(m0, m2)}, 0, 1)
              + expr_s(CSL_I) * g(m2, 4) * current({gamma(m0, m1)}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_bad_simpli);
    }

    // Chisholm identity with fully contracted sigma tensor
    keepSymbolic4D(true);
    TEST_ALL_EPSILON_COMBINATIONS
    {
        auto c = eps(m0, m1, m2, m3) * current({gamma(m2, m3)}, 0, 1);
        auto c_simpli
            = CSL_I * (2 - D) * current({gamma(m0, m1), gamma5()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
}

TEST(marty_gamma_api, conjugation_identities)
{
    keepSymbolic4D(false);
    {
        auto c          = current({C(), gamma(0), C()}, 1, 0);
        auto c_simpli   = current({gamma(0)}, 0, 1);
        auto c_simpli_T = current({gamma(0)}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli_T);
    }
    {
        auto c          = current({C(), C(), C()}, 1, 0);
        auto c_simpli   = expr_s(-1) * current({C()}, 0, 1);
        auto c_simpli_T = expr_s(-1) * current({C()}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli_T);
    }
    {
        auto c          = current({C(), gamma5(), C()}, 1, 0);
        auto c_simpli   = expr_s(-1) * current({gamma5()}, 0, 1);
        auto c_simpli_T = expr_s(-1) * current({gamma5()}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli_T);
    }
    {
        auto c          = current({C(), P_L(), C()}, 1, 0);
        auto c_simpli   = expr_s(-1) * current({P_L()}, 0, 1);
        auto c_simpli_T = expr_s(-1) * current({P_L()}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli_T);
    }
    {
        auto c          = current({C(), P_R(), C()}, 1, 0);
        auto c_simpli   = expr_s(-1) * current({P_R()}, 0, 1);
        auto c_simpli_T = expr_s(-1) * current({P_R()}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli_T);
    }
    {
        auto c          = current({C(), gamma(0, 1), C()}, 1, 0);
        auto c_simpli   = current({gamma(0, 1)}, 0, 1);
        auto c_simpli_T = current({gamma(0, 1)}, 1, 0);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
        COMPARE_SGL_EXPR(EXPECT_NE, simplified(c), c_simpli_T);
    }
    {
        auto c = current({C(), gamma(0)}, 1, 0) * current({gamma(1)}, 1, 2);
        auto c_simpli = current({C(), gamma(0), gamma(1)}, 0, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c
            = current({gamma(0), C()}, 1, 0) * current({C(), gamma(1)}, 1, 2);
        auto c_simpli = expr_s(-1) * current({gamma(0), gamma(1)}, 0, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c = current({gamma(1)}, 1, 2) * current({C(), gamma(0)}, 1, 0);
        auto c_simpli = current({C(), gamma(0), gamma(1)}, 0, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c
            = current({C(), gamma(1)}, 1, 2) * current({gamma(0), C()}, 1, 0);
        auto c_simpli = expr_s(-1) * current({gamma(0), gamma(1)}, 0, 2);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), c_simpli);
    }
    {
        auto c = current({C(),
                          gamma(1),
                          gamma5(),
                          gamma(0),
                          gamma5(),
                          C(),
                          C(),
                          P_L(),
                          gamma(2, 3),
                          P_L(),
                          C()},
                         1,
                         0);

        auto c_simpli
            = current({gamma(2, 3), gamma(0), gamma(1), P_L()}, 0, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c), simplified(c_simpli));
    }
}

TEST(marty_gamma_api, simple_fierz_identities)
{
    keepSymbolic4D(false);
    csl::ScopedProperty permissiveIndices(
        &csl::option::permissiveCovariantIndices, true);
    {
        // completeness relation chiral basis
        auto c1    = current({}, 0, 1);
        auto c2    = current({}, 2, 3);
        auto fierz = applySingleFierz(c1, c2, FierzBasis::Chiral);
        auto fierz_result
            = expr_s(CSL_HALF)
              * (current({P_L()}, 0, 3) * current({P_L()}, 2, 1)
                 + current({P_R()}, 0, 3) * current({P_R()}, 2, 1)
                 + current({gamma(0), P_R()}, 0, 3)
                       * current({gamma(0), P_L()}, 2, 1)
                 + current({gamma(0), P_L()}, 0, 3)
                       * current({gamma(0), P_R()}, 2, 1)
                 - expr_s(CSL_1 / 4) * current({gamma(0, 1), P_R()}, 0, 3)
                       * current({gamma(0, 1), P_R()}, 2, 1)
                 - expr_s(CSL_1 / 4) * current({gamma(0, 1), P_R()}, 0, 3)
                       * current({gamma(0, 1), P_L()}, 2, 1)
                 - expr_s(CSL_1 / 4) * current({gamma(0, 1), P_L()}, 0, 3)
                       * current({gamma(0, 1), P_R()}, 2, 1)
                 - expr_s(CSL_1 / 4) * current({gamma(0, 1), P_L()}, 0, 3)
                       * current({gamma(0, 1), P_L()}, 2, 1));
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        // completeness relation standard basis
        auto c1    = current({}, 0, 1);
        auto c2    = current({}, 2, 3);
        auto fierz = applySingleFierz(c1, c2, FierzBasis::Standard);
        auto fierz_result
            = expr_s(CSL_1 / 4)
              * (current({}, 0, 3) * current({}, 2, 1)
                 + current({gamma5()}, 0, 3) * current({gamma5()}, 2, 1)
                 + current({gamma(0)}, 0, 3) * current({gamma(0)}, 2, 1)
                 - current({gamma(0), gamma5()}, 0, 3)
                       * current({gamma(0), gamma5()}, 2, 1)
                 - expr_s(CSL_HALF) * current({gamma(0, 1)}, 0, 3)
                       * current({gamma(0, 1)}, 2, 1));
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1           = current({P_L()}, 0, 1);
        auto c2           = current({P_R()}, 2, 3);
        auto fierz        = applySingleFierz(c1, c2);
        auto fierz_result = expr_s(CSL_HALF) * current({gamma(0), P_R()}, 0, 3)
                            * current({gamma(0), P_L()}, 2, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1           = current({P_R()}, 0, 1);
        auto c2           = current({P_L()}, 2, 3);
        auto fierz        = applySingleFierz(c1, c2);
        auto fierz_result = expr_s(CSL_HALF) * current({gamma(0), P_L()}, 0, 3)
                            * current({gamma(0), P_R()}, 2, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1    = current({P_L()}, 0, 1);
        auto c2    = current({P_L()}, 2, 3);
        auto fierz = applySingleFierz(c1, c2);
        auto fierz_result
            = expr_s(CSL_HALF) * current({P_L()}, 0, 3)
                  * current({P_L()}, 2, 1)
              - expr_s(CSL_1 / 8) * current({gamma(0, 1), P_R()}, 0, 3)
                    * current({gamma(0, 1), P_L()}, 2, 1)
              - expr_s(CSL_1 / 8) * current({gamma(0, 1), P_L()}, 0, 3)
                    * current({gamma(0, 1), P_L()}, 2, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1    = current({P_R()}, 0, 1);
        auto c2    = current({P_R()}, 2, 3);
        auto fierz = applySingleFierz(c1, c2);
        auto fierz_result
            = expr_s(CSL_HALF) * current({P_R()}, 0, 3)
                  * current({P_R()}, 2, 1)
              - expr_s(CSL_1 / 8) * current({gamma(0, 1), P_R()}, 0, 3)
                    * current({gamma(0, 1), P_R()}, 2, 1)
              - expr_s(CSL_1 / 8) * current({gamma(0, 1), P_L()}, 0, 3)
                    * current({gamma(0, 1), P_R()}, 2, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1    = current({P_R(), gamma(0, 1)}, 0, 1);
        auto c2    = current({P_R(), gamma(1)}, 2, 3);
        auto fierz = applySingleFierz(c1, c2);
        auto fierz_result
            = expr_s(3 / CSL_2) * current({gamma(0), P_L()}, 0, 3)
                  * current({P_R()}, 2, 1)
              - expr_s(CSL_1 / 2) * current({gamma(1), P_L()}, 0, 3)
                    * current({gamma(0, 1), P_R()}, 2, 1);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
}

TEST(marty_gamma_api, double_fierz_identities)
{
    keepSymbolic4D(false);
    // Test of canonical elements: the double Fierz identity should
    // return the same fermion currents
    auto test_canonical_double_fierz
        = [](std::vector<Expr> g1, std::vector<Expr> g2, auto basis) {
              auto c1           = current(g1, 0, 1);
              auto c2           = current(g2, 2, 3);
              auto fierz        = applyDoubleFierz(c1, c2, basis);
              auto fierz_result = c1 * c2;
              COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
          };
    // Test canonical elements in the Chiral basis
    test_canonical_double_fierz(
        {P_L()}, {P_L()}, mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz(
        {P_L()}, {P_R()}, mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz(
        {P_L()}, {gamma(0), P_R()}, mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz({gamma(1), P_R()},
                                {gamma(0), P_R()},
                                mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz({gamma(1), P_R()},
                                {gamma(0), P_L()},
                                mty::gamma_api::FierzBasis::Chiral);
    // Not simplified enough
    // test_canonical_double_fierz(
    //    {gamma(0, 1)}, {gamma(2, 3)}, mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz(
        {gamma(0, 1), P_L()}, {P_L()}, mty::gamma_api::FierzBasis::Chiral);
    test_canonical_double_fierz(
        {gamma(0, 1), P_R()}, {P_L()}, mty::gamma_api::FierzBasis::Chiral);
    // Not simplified enough
    // test_canonical_double_fierz(
    //    {gamma(0, 1)}, {gamma(2), P_L()},
    //    mty::gamma_api::FierzBasis::Chiral);

    // Test canonical elements in the Standard basis
    test_canonical_double_fierz({}, {}, mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz(
        {}, {gamma5()}, mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz(
        {gamma5()}, {gamma5()}, mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz(
        {}, {gamma(0)}, mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz(
        {gamma5()}, {gamma(0)}, mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz({gamma5()},
                                {gamma(0), gamma5()},
                                mty::gamma_api::FierzBasis::Standard);
    test_canonical_double_fierz(
        {gamma(0, 1)}, {}, mty::gamma_api::FierzBasis::Standard);

    test_canonical_double_fierz(
        {gamma(0, 1)}, {gamma5()}, mty::gamma_api::FierzBasis::Standard);
    // Not simplified enough
    // test_canonical_double_fierz(
    //     {gamma(0, 1)}, {gamma(2)}, mty::gamma_api::FierzBasis::Standard);
    // test_canonical_double_fierz({gamma(0, 1)},
    //                             {gamma(2), gamma5()},
    //                             mty::gamma_api::FierzBasis::Standard);

    csl::ScopedProperty permissiveIndices(
        &csl::option::permissiveCovariantIndices, true);
    // Other tests
    {
        auto c1 = current({gamma(0), gamma(1), gamma(2), P_L()}, 0, 1);
        auto c2 = current({gamma(2), gamma(1), gamma(0), P_L()}, 2, 3);
        auto fierz
            = applyDoubleFierz(c1, c2, mty::gamma_api::FierzBasis::Chiral);
        auto fierz_result = expr_s(4) * current({gamma(0), P_L()}, 0, 1)
                            * current({gamma(0), P_L()}, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1 = current({gamma(0), gamma(1), gamma(2), P_L()}, 0, 1);
        auto c2 = current({gamma(1), gamma(2), gamma(0), P_L()}, 2, 3);
        auto fierz
            = applyDoubleFierz(c1, c2, mty::gamma_api::FierzBasis::Chiral);
        auto fierz_result = expr_s(4) * current({gamma(0), P_L()}, 0, 1)
                            * current({gamma(0), P_L()}, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
    {
        auto c1 = current({gamma(0), gamma(1), gamma(2), P_L()}, 0, 1);
        auto c2 = current({gamma(0), gamma(1), gamma(2), P_L()}, 2, 3);
        auto fierz
            = applyDoubleFierz(c1, c2, mty::gamma_api::FierzBasis::Chiral);
        auto fierz_result = expr_s(16) * current({gamma(0), P_L()}, 0, 1)
                            * current({gamma(0), P_L()}, 2, 3);
        COMPARE_SGL_EXPR(EXPECT_EQ, fierz, fierz_result);
    }
}