#include <gtest/gtest.h>
#include <marty.h>
#include <marty/models/sm.h>

TEST(marty_sm_params, get_sm_param_values)
{
    for (const auto &value : mty::sm_input::all_params) {
        EXPECT_EQ(value->evaluate(csl::eval::all), std::nullopt);
    }
}

TEST(marty_sm_params, undefine_sm_param_values)
{
    mty::sm_input::redefineNumericalValues();
    mty::sm_input::undefineNumericalValues();
    for (const auto &value : mty::sm_input::all_params) {
        EXPECT_EQ(value->evaluate(csl::eval::all), std::nullopt);
    }
}

TEST(marty_sm_params, redefine_sm_param_values)
{
    mty::sm_input::undefineNumericalValues();
    mty::sm_input::redefineNumericalValues();
    for (std::size_t i = 0; i != mty::sm_input::all_params.size(); ++i) {
        csl::Expr new_value
            = csl::Evaluated(mty::sm_input::all_params[i], csl::eval::all);
        EXPECT_NE(new_value, CSL_UNDEF);
    }
}

TEST(marty_sm_params, test_multiple_calls)
{
    mty::sm_input::undefineNumericalValues();
    mty::sm_input::undefineNumericalValues();
    mty::sm_input::redefineNumericalValues();
    mty::sm_input::redefineNumericalValues();
    mty::sm_input::undefineNumericalValues();
}