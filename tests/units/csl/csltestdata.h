#ifndef CSL_TEST_DATA_H_INCLUDED
#define CSL_TEST_DATA_H_INCLUDED

#include <csl.h>
#include <map>
#include <unordered_map>

namespace data {

inline const csl::Expr a = csl::constant_s("a");
inline const csl::Expr b = csl::constant_s("b");
inline const csl::Expr c1
    = csl::constant_s("c1", csl::ComplexProperty::Complex);
inline const csl::Expr c2
    = csl::constant_s("c2", csl::ComplexProperty::Complex);
inline const csl::Expr x = csl::variable_s("x");
inline const csl::Expr y = csl::variable_s("y");
inline const csl::Expr z1
    = csl::variable_s("z1", csl::ComplexProperty::Complex);
inline const csl::Expr z2
    = csl::variable_s("z2", csl::ComplexProperty::Complex);

inline const csl::Index i1  = csl::Euclid_R2.generateIndex("i");
inline const csl::Index i2  = csl::Euclid_R2.generateIndex("i");
inline const csl::Index I1  = csl::Euclid_R3.generateIndex("I");
inline const csl::Index I2  = csl::Euclid_R3.generateIndex("I");
inline const csl::Index mu1 = csl::Minkowski.generateIndex("mu");
inline const csl::Index mu2 = csl::Minkowski.generateIndex("mu");

inline csl::Tensor T22("T22", {&csl::Euclid_R2, &csl::Euclid_R2});
inline csl::Tensor T33("T33", {&csl::Euclid_R3, &csl::Euclid_R3});
inline csl::Tensor T23("T23", {&csl::Euclid_R2, &csl::Euclid_R3});

} // namespace data

struct ExprExpectation {
    csl::Expr            expr;
    std::string          name;
    csl::ComplexProperty complexProperty;

    csl::Expr copy;
    csl::Expr refreshed;

    csl::Expr real;
    csl::Expr imag;
    csl::Expr mod;
    csl::Expr arg;

    csl::Expr conjugate;
    csl::Expr factored;
    csl::Expr expanded;

    std::unordered_map<csl::eval::mode, csl::Expr> evaluations;

    std::map<csl::Expr, csl::Expr> derivatives;

    ExprExpectation(csl::Expr const     &expr_,
                    std::string const   &name_ = "",
                    csl::ComplexProperty prop_ = csl::ComplexProperty::Real)
        : expr(expr_),
          name(name_),
          complexProperty(prop_),
          copy(expr_),
          refreshed(expr_),
          real(expr_),
          imag(CSL_0),
          mod(expr_),
          arg(CSL_UNDEF),
          conjugate(expr_),
          factored(expr_),
          expanded(expr_)
    {
        evaluations[csl::eval::base] = expr_;
    }
};

#define ASSERT_DEFINED_EXPR(A, B) \
    if (B != CSL_UNDEF) {         \
        ASSERT_EQ(A, B);          \
    }

#define TEST_EXPECTATION(expect)                                           \
    ASSERT_EQ(expect.expr->getName(), expect.name);                        \
    ASSERT_EQ(expect.expr->getComplexProperty(), expect.complexProperty);  \
    ASSERT_DEFINED_EXPR(expect.expr->copy(), expect.copy);                 \
    ASSERT_DEFINED_EXPR(expect.expr->refresh(), expect.refreshed);         \
    ASSERT_DEFINED_EXPR(csl::GetRealPart(expect.expr), expect.real);       \
    ASSERT_DEFINED_EXPR(csl::GetImaginaryPart(expect.expr), expect.imag);  \
    ASSERT_DEFINED_EXPR(csl::GetComplexModulus(expect.expr), expect.mod);  \
    ASSERT_DEFINED_EXPR(csl::GetComplexArgument(expect.expr), expect.arg); \
    ASSERT_DEFINED_EXPR(csl::GetComplexConjugate(expect.expr),             \
                        expect.conjugate);                                 \
    ASSERT_DEFINED_EXPR(csl::Factored(expect.expr), expect.factored);      \
    ASSERT_DEFINED_EXPR(csl::Expanded(expect.expr), expect.expanded);      \
    for (const auto &[mode, res] : expect.evaluations) {                   \
        ASSERT_DEFINED_EXPR((csl::Evaluated(expect.expr, mode)), res);     \
    }                                                                      \
    for (const auto &[var, res] : expect.derivatives) {                    \
        ASSERT_DEFINED_EXPR((csl::Derived(expect.expr, var)), res);        \
    }

#define PRINT_EXPR(expr)                                   \
    std::cout.setstate(std::ios_base::failbit);            \
    (expr)->print();                                       \
    (expr)->print(1);                                      \
    (expr)->print(2);                                      \
    (expr)->print(3);                                      \
    (expr)->print(4);                                      \
    (expr)->print(5);                                      \
    (expr)->print(0, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(1, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(2, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(3, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(4, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(5, std::cout, csl::LibraryMode::CLib);   \
    (expr)->print(0, std::cout, csl::LibraryMode::CppLib); \
    (expr)->print(1, std::cout, csl::LibraryMode::CppLib); \
    (expr)->print(2, std::cout, csl::LibraryMode::CppLib); \
    (expr)->print(3, std::cout, csl::LibraryMode::CppLib); \
    (expr)->print(4, std::cout, csl::LibraryMode::CppLib); \
    (expr)->print(5, std::cout, csl::LibraryMode::CppLib); \
    (expr)->printLaTeX();                                  \
    (expr)->printLaTeX(1);                                 \
    (expr)->printLaTeX(2);                                 \
    (expr)->printLaTeX(3);                                 \
    (expr)->printLaTeX(4);                                 \
    (expr)->printLaTeX(5);                                 \
    (expr)->printCode();                                   \
    (expr)->printCode(1);                                  \
    (expr)->printCode(2);                                  \
    (expr)->printCode(3);                                  \
    (expr)->printCode(4);                                  \
    (expr)->printCode(5);                                  \
    std::cout.setstate(std::ios_base::goodbit);

#endif
