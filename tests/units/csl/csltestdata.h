#ifndef CSL_TEST_DATA_H_INCLUDED
#define CSL_TEST_DATA_H_INCLUDED

#include <csl.h>

namespace data {

inline const csl::Expr a = csl::constant_s("a");
inline const csl::Expr b = csl::constant_s("b");
inline const csl::Expr x = csl::variable_s("x");
inline const csl::Expr y = csl::variable_s("y");

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
