#pragma once

#include <string>
#include <vector>

namespace csl {
class Expr;
class Index;
} // namespace csl

namespace mty {

class Model;
struct Wilson;
struct WilsonSet;

enum class DiracCoupling { S, P, L, R, V, A, VL, VR, T, TA, TL, TR };

enum class ColorCoupling {
    Id,
    Generator,
    Crossed,
    InvCrossed // relevant only with conjugate representations (not SM)
};

struct ColorSpec {
    std::string   groupName;
    ColorCoupling coupling;
};

std::vector<Wilson>
getMagneticCoupling(DiracCoupling coupling, csl::Index alpha, csl::Index beta);

csl::Expr getMagneticGenerator(mty::Model const &model,
                               csl::Expr &       psi_star,
                               csl::Expr &       psi,
                               csl::Expr &       A);

std::vector<Wilson> chromoMagneticOperator(Model const &    model,
                                           WilsonSet const &wilsons,
                                           DiracCoupling    coupling);

std::vector<Wilson> dimension5Operator(Model const &    model,
                                       WilsonSet const &wilsons,
                                       DiracCoupling    current);

std::vector<Wilson>
dimension6Operator(Model const &                 model,
                   WilsonSet const &             wilsons,
                   DiracCoupling                 leftCurrent,
                   DiracCoupling                 rightCurrent,
                   std::vector<ColorSpec> const &colorCouplings,
                   std::vector<int>              fermionOrder = {});

inline std::vector<Wilson> dimension6Operator(Model const &    model,
                                              WilsonSet const &wilsons,
                                              DiracCoupling    leftCurrent,
                                              DiracCoupling    rightCurrent,
                                              std::vector<int> fermionOrder
                                              = {})
{
    return dimension6Operator(model,
                              wilsons,
                              leftCurrent,
                              rightCurrent,
                              std::vector<ColorSpec>{},
                              fermionOrder);
}

inline std::vector<Wilson> dimension6Operator(Model const &    model,
                                              WilsonSet const &wilsons,
                                              DiracCoupling    leftCurrent,
                                              DiracCoupling    rightCurrent,
                                              ColorSpec const &colorCoupling,
                                              std::vector<int> fermionOrder
                                              = {})
{
    return dimension6Operator(model,
                              wilsons,
                              leftCurrent,
                              rightCurrent,
                              std::vector<ColorSpec>{colorCoupling},
                              fermionOrder);
}

} // namespace mty
