#include <gtest/gtest.h>
#include <marty-plugins/boltzmann-solver/quantumnumberdata.h>

TEST(marty_boltzmann_solver, simple_test)
{
    mty::QuantumNumberData::QuantumNumber number;
    number.name = "test";
    // Check name assigned correctly
    EXPECT_EQ(number.name, "test");
}