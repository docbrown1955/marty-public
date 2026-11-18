#include "csltestdata.h"
#include <csl.h>
#include <gtest/gtest.h>

#include <sstream>

namespace {

std::string printed(csl::Expr const &expr)
{
    std::ostringstream output;
    expr->print(0, output);
    return output.str();
}

} // namespace

TEST(csl_literals, constant)
{
    ExprExpectation cst1(data::a, "a");
    TEST_EXPECTATION(cst1)
    ExprExpectation cst2(data::c1, "c1", csl::ComplexProperty::Complex);
    cst2.real = csl::make_shared<csl::RealPart>(cst2.expr);
    cst2.imag = csl::make_shared<csl::ImaginaryPart>(cst2.expr);
    cst2.mod  = csl::sqrt_s(cst2.real * cst2.real + cst2.imag * cst2.imag);
    cst2.arg  = csl::angle_s(cst2.real, cst2.imag);
    cst2.conjugate = data::c1->copy();
    cst2.conjugate->setConjugated(true);
    TEST_EXPECTATION(cst2)
}

TEST(csl_literals, variables)
{
    std::cout << (data::z1 == data::z1) << std::endl;
    std::cout << (csl::real_s(data::z1) == csl::real_s(data::z1)) << std::endl;
    std::cout << (csl::imaginary_s(data::z1) == csl::imaginary_s(data::z1))
              << std::endl;
    ExprExpectation cst1(data::x, "x");
    TEST_EXPECTATION(cst1)
    ExprExpectation cst2(data::z1, "z1", csl::ComplexProperty::Complex);
    cst2.real = csl::make_shared<csl::RealPart>(cst2.expr);
    cst2.imag = csl::make_shared<csl::ImaginaryPart>(cst2.expr);
    cst2.mod  = csl::sqrt_s(cst2.real * cst2.real + cst2.imag * cst2.imag);
    cst2.arg  = csl::angle_s(cst2.real, cst2.imag);
    cst2.conjugate = data::z1->copy();
    cst2.conjugate->setConjugated(true);
    TEST_EXPECTATION(cst2)
}

TEST(csl_literals, complexPropertyStaysOnTheSameLine)
{
    auto        constant = csl::constant_s("a", csl::ComplexProperty::Complex);
    auto        variable = csl::variable_s("v", csl::ComplexProperty::Complex);
    csl::Tensor tensor("T", {&csl::Euclid_R2});
    tensor->setComplexProperty(csl::ComplexProperty::Complex);
    auto tensorElement = tensor(csl::Euclid_R2.generateIndex("i"));

    constant->setConjugated(true);
    variable->setConjugated(true);
    tensorElement->setConjugated(true);

    EXPECT_EQ(printed(constant), "a^(*)\n");
    EXPECT_EQ(printed(variable), "v^(*)\n");
    auto const tensorText = printed(tensorElement);
    EXPECT_NE(tensorText.find("^(*)"), std::string::npos);
    EXPECT_EQ(tensorText.find('\n'), tensorText.size() - 1);
}
