#include <gtest/gtest.h>
#include <marty.h>
#include <marty/models/sm.h>

class marty_sm_params : public ::testing::Test {
  protected:
    void SetUp() override
    {
        // Get the values and make sure they are well-defined
        sm_param_values.reserve(mty::sm_input::all_params.size());
        for (const auto &param : mty::sm_input::all_params) {
            csl::Expr eval
                = param->evaluate(csl::eval::literal).value_or(CSL_UNDEF);
            ASSERT_NE(eval, CSL_UNDEF);
            sm_param_values.push_back(eval);
        }
    }

    void TearDown() override
    {
        // Make sure we put the values back
        for (std::size_t i = 0; i != mty::sm_input::all_params.size(); ++i) {
            csl::Expr(mty::sm_input::all_params[i])
                ->setValue(sm_param_values[i]);
        }
    }

    std::vector<csl::Expr> const &get_values() const
    {
        return sm_param_values;
    }

    std::vector<csl::Expr> sm_param_values;
};

TEST_F(marty_sm_params, get_sm_param_values)
{
    for (const auto &value : sm_param_values) {
        ASSERT_NE(value, CSL_UNDEF);
    }
}

TEST_F(marty_sm_params, undefine_sm_param_values)
{
    mty::sm_input::undefineNumericalValues();
    for (const auto &value : mty::sm_input::all_params) {
        ASSERT_EQ(value->evaluate(csl::eval::all), std::nullopt);
    }
}

TEST_F(marty_sm_params, redefine_sm_param_values)
{
    mty::sm_input::undefineNumericalValues();
    mty::sm_input::redefineNumericalValues();
    for (std::size_t i = 0; i != mty::sm_input::all_params.size(); ++i) {
        csl::Expr old_value
            = csl::Evaluated(sm_param_values[i], csl::eval::all);
        csl::Expr new_value
            = csl::Evaluated(mty::sm_input::all_params[i], csl::eval::all);
        ASSERT_NE(old_value, CSL_UNDEF);
        ASSERT_EQ(old_value, new_value);
    }
}