#include "csltestdata.h"
#include <csl.h>
#include <gtest/gtest.h>

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
