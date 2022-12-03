#include <gtest/gtest.h>
#include <marty.h>
#include <marty/api/gamma.h>
#include <marty/sgl/sgl.h>

using namespace mty::gamma_api;
constexpr auto    expr_s = sgl::cslexpr_s;
static const auto D      = csl::DMinko;

const static sgl::TensorSet tensorSet
    = {mty::dirac4.gamma_chir,
       mty::dirac4.C_matrix,
       mty::dirac4.P_L,
       mty::dirac4.P_R,
       std::map<size_t, csl::Tensor>{{0, mty::dirac4.getDelta()},
                                     {1, mty::dirac4.gamma},
                                     {2, mty::dirac4.sigma}}};

#define COMPARE_SGL_EXPR(ASSERT_MACRO, lhs, rhs)  \
    ASSERT_MACRO(sgl::sgl_to_csl(lhs, tensorSet), \
                 sgl::sgl_to_csl(rhs, tensorSet))

TEST(marty_gamma_api, simple_simplification)
{
    keepSymbolic4D(true);
    auto c1        = chain({gamma(0), gamma(1), gamma(0)}, 0, 1);
    auto c1_simpli = expr_s(2) * chain({gamma(1)}, 0, 1)
                     - expr_s(D) * chain({gamma(1)}, 0, 1);
    auto c2        = chain({gamma(0), gamma5(), gamma(0)}, 0, 1);
    auto c2_simpli = expr_s(-D) * chain({gamma5()}, 0, 1);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c1), c1_simpli);
    COMPARE_SGL_EXPR(EXPECT_EQ, simplified(c2), c2_simpli);
}