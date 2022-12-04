// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "diracology.h"
#include "../sgl/sgl.h"
#include "mrtError.h"
#include "mrtInterface.h"
#include "mrtUtils.h"
#include "polarization.h"
#include "quantumField.h"

using namespace std;
using namespace csl;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Helper functions                              //
/*************************************************/
///////////////////////////////////////////////////

csl::Expr slashed_s(csl::Tensor       p,
                    const csl::Index &alpha,
                    const csl::Index &beta,
                    const DiracSpace *space)
{
    csl::Index mu = csl::Minkowski.generateIndex();
    return space->gamma({+mu, alpha, beta}) * p(mu);
}

csl::Expr bar_s(const csl::Expr &tensor, const DiracSpace *space)
{
    csl::Index                 beta      = space->generateIndex();
    const csl::IndexStructure &structure = tensor->getIndexStructureView();
    csl::Index                 alpha;
    for (const auto &i : structure)
        if (i.getSpace() == space)
            alpha = i;

    csl::Expr res = csl::Replaced(tensor, alpha, beta);

    return csl::GetComplexConjugate(res) * space->gamma({0, beta, alpha});
}

///////////////////////////////////////////////////
/*************************************************/
// Class DiracSpace                              //
/*************************************************/
///////////////////////////////////////////////////

DiracSpace::DiracSpace(csl::Space const *t_spaceTime)
    : csl::Space("dirac",
                 getSpinorDimension(t_spaceTime->getDim()),
                 {"alpha", "beta", "gam", "del", "eps", "eta"}),
      spaceTime(t_spaceTime)
{
    gamma = csl::tensor_s("gamma", {spaceTime, this, this});

    sigma = csl::tensor_s("sigma", {spaceTime, spaceTime, this, this});

    gamma_chir
        = csl::tensor_s((dim == 4) ? "gamma5" : "gamma_chir", {this, this});

    P_L = csl::tensor_s("P_L", {this, this});

    P_R = csl::tensor_s("P_R", {this, this});

    C_matrix
        = csl::tensor_s("C", std::vector<csl::Space const *>({this, this}));

    initProperties();
}

DiracSpace::~DiracSpace()
{
}

csl::Space const *DiracSpace::getSpaceTime() const
{
    return spaceTime;
}

void setDiracTensor4(const DiracSpace *self)
{
    self->gamma->setTensor(
        csl::highdtensor_s({{{CSL_0, CSL_0, CSL_1, CSL_0},
                             {CSL_0, CSL_0, CSL_0, CSL_1},
                             {CSL_1, CSL_0, CSL_0, CSL_0},
                             {CSL_0, CSL_1, CSL_0, CSL_0}},

                            {{CSL_0, CSL_0, CSL_0, CSL_1},
                             {CSL_0, CSL_0, CSL_1, CSL_0},
                             {CSL_0, CSL_M_1, CSL_0, CSL_0},
                             {CSL_M_1, CSL_0, CSL_0, CSL_0}},

                            {{CSL_0, CSL_0, CSL_0, -CSL_I},
                             {CSL_0, CSL_0, CSL_I, CSL_0},
                             {CSL_0, CSL_I, CSL_0, CSL_0},
                             {-CSL_I, CSL_0, CSL_0, CSL_0}},

                            {{CSL_0, CSL_0, CSL_1, CSL_0},
                             {CSL_0, CSL_0, CSL_0, CSL_M_1},
                             {CSL_M_1, CSL_0, CSL_0, CSL_0},
                             {CSL_0, CSL_1, CSL_0, CSL_0}}}));

    self->C_matrix->setTensor(csl::matrix_s({{CSL_0, CSL_1, CSL_0, CSL_0},
                                             {CSL_M_1, CSL_0, CSL_0, CSL_0},
                                             {CSL_0, CSL_0, CSL_0, CSL_M_1},
                                             {CSL_0, CSL_0, CSL_1, CSL_0}}));

    self->gamma_chir->setTensor(csl::matrix_s({{CSL_M_1, CSL_0, CSL_0, CSL_0},
                                               {CSL_0, CSL_M_1, CSL_0, CSL_0},
                                               {CSL_0, CSL_0, CSL_1, CSL_0},
                                               {CSL_0, CSL_0, CSL_0, CSL_1}}));

    self->P_L->setTensor(csl::matrix_s({{CSL_1, CSL_0, CSL_0, CSL_0},
                                        {CSL_0, CSL_1, CSL_0, CSL_0},
                                        {CSL_0, CSL_0, CSL_0, CSL_0},
                                        {CSL_0, CSL_0, CSL_0, CSL_0}}));

    self->P_R->setTensor(csl::matrix_s({{CSL_0, CSL_0, CSL_0, CSL_0},
                                        {CSL_0, CSL_0, CSL_0, CSL_0},
                                        {CSL_0, CSL_0, CSL_1, CSL_0},
                                        {CSL_0, CSL_0, CSL_0, CSL_1}}));

    csl::Expr sigmaTensor = csl::highdtensor_s(std::vector<int>({4, 4, 4, 4}));
    csl::Expr gammaMatrix = dirac4.gamma->getTensor();
    for (int mu = 0; mu != 4; ++mu)
        for (int nu = 0; nu != 4; ++nu)
            for (int alpha = 0; alpha != 4; ++alpha)
                for (int beta = 0; beta != 4; ++beta) {
                    csl::Expr res = CSL_0;
                    for (int delta = 0; delta != 4; ++delta)
                        res = res
                              + gammaMatrix->getArgument({mu, alpha, delta})
                                    * gammaMatrix->getArgument(
                                        {nu, delta, beta})
                              - gammaMatrix->getArgument({nu, alpha, delta})
                                    * gammaMatrix->getArgument(
                                        {mu, delta, beta});
                    sigmaTensor->setArgument(Expanded(res * CSL_I / 2),
                                             {mu, nu, alpha, beta});
                }
    self->sigma->setTensor(sigmaTensor);
}

void DiracSpace::initProperties()
{
    /////////////////////////////////////////////////////
    // In this function hermitian conjugation properties
    // are determined considering fermion bilinears with
    // an implicit gamma^0 commuting with the bilinear.
    // Then, properties are different than usual because
    // the commutation with gamma^0 is taken directly
    // into account at the same time, allowing MARTY to
    // never use any gamma^0 matrix.
    /////////////////////////////////////////////////////

    sigma->addAntiSymmetry(0, 1);
    // gamma_chir->setFullySymmetric();
    // P_L->setFullySymmetric();
    // P_R->setFullySymmetric();

    csl::Index mu  = st_index();
    csl::Index nu  = st_index();
    csl::Index rho = st_index();
    csl::Index sig = st_index();
    csl::Index a1  = index();
    csl::Index a2  = index();
    csl::Index a3  = index();
    csl::Index a4  = index();
    csl::Index a5  = index();
    csl::Index a6  = index();

    gamma->addSelfContraction(gamma({+mu, a1, a2}),
                              gamma({mu, a2, a3}),
                              csl::DMinko * delta({a1, a3}));
    csl::vector_expr sign = spaceTime->getSignature();
    for (csl::Index i = 0; i != spaceTime->getDim(); ++i) {
        gamma->addSelfContraction(gamma({i, a1, a2}),
                                  gamma({i, a2, a3}),
                                  spaceTime->getMetric()({i, i})
                                      * delta({a1, a3}));
        gamma->addSelfContraction(gamma({i, a1, a2}),
                                  gamma({+i, a2, a3}),
                                  spaceTime->getDelta()({i, +csl::Index(i)})
                                      * delta({a1, a3}));
        gamma->addSelfContraction(gamma({+i, a1, a2}),
                                  gamma({i, a2, a3}),
                                  spaceTime->getDelta()({+csl::Index(i), i})
                                      * delta({a1, a3}));
        gamma->addSelfContraction(
            gamma({+i, a1, a2}),
            gamma({+i, a2, a3}),
            spaceTime->getInverseMetric()({+csl::Index(i), +csl::Index(i)})
                * delta({a1, a3}));
    }

    gamma_chir->addSelfContraction(
        gamma_chir({a1, a2}), gamma_chir({a2, a3}), delta({a1, a3}));
    P_L->addSelfContraction(P_L({a1, a2}), P_L({a2, a3}), P_L({a1, a3}));
    P_R->addSelfContraction(P_R({a1, a2}), P_R({a2, a3}), P_R({a1, a3}));

    P_L->addSelfContraction(P_L({a1, a2}), P_R({a2, a3}), CSL_0);
    P_L->addSelfContraction(P_L({a2, a1}), P_R({a3, a2}), CSL_0);

    csl::Tensor eps = spaceTime->getEpsilon();
    csl::Tensor g   = spaceTime->getMetric();
    eps->addSelfContraction(eps({mu, nu, rho, sig}),
                            sigma({+rho, +sig, a1, a2}),
                            -2 * CSL_I * sigma({mu, nu, a1, a3})
                                * gamma_chir({a3, a2}));
    eps->addSelfContraction(
        eps({mu, nu, rho, sig}),
        gamma({+sig, a1, a2}),
        -CSL_I
            * (gamma({mu, a1, a4}) * gamma({nu, a4, a5}) * gamma({rho, a5, a3})
               - g({mu, nu}) * gamma({rho, a1, a3})
               + g({mu, rho}) * gamma({nu, a1, a3})
               - g({nu, rho}) * gamma({mu, a1, a3}))
            * gamma_chir({a3, a2}));

    gamma->addComplexProperty(gamma({mu, a1, a4}),
                              gamma({2, a1, a2}) * gamma({mu, a2, a3})
                                  * gamma({2, a3, a4}));

    gamma->addHermitianProperty(
        this, gamma({mu, a1, a4}), gamma({mu, a1, a4}));

    gamma->addTransposedProperty(this,
                                 gamma({mu, a1, a6}),
                                 gamma({2, a1, a3}) * gamma({mu, a3, a4})
                                     * gamma({2, a4, a6}));

    gamma->addTraceLessNess(this);
    gamma_chir->addTraceLessNess(this);
    sigma->addTraceLessNess(this);
    C_matrix->addTraceLessNess(this);
    C_matrix->setComplexProperty(csl::ComplexProperty::Real);
    C_matrix->addHermitianProperty(
        this, C_matrix({a1, a4}), C_matrix({a1, a4}));
    C_matrix->addTransposedProperty(
        this, C_matrix({a1, a4}), C_matrix({a1, a4}));

    gamma_chir->addTransposedProperty(
        this, gamma_chir({a1, a2}), -gamma_chir({a1, a2}));
    gamma_chir->addHermitianProperty(
        this, gamma_chir({a1, a2}), -gamma_chir({a1, a2}));
    P_L->addTransposedProperty(this, P_L({a1, a2}), P_R({a1, a2}));
    P_R->addTransposedProperty(this, P_R({a1, a2}), P_L({a1, a2}));
    P_L->addHermitianProperty(this, P_L({a1, a2}), P_R({a1, a2}));
    P_R->addHermitianProperty(this, P_R({a1, a2}), P_L({a1, a2}));

    C_matrix->setComplexProperty(csl::ComplexProperty::Real);
    C_matrix->addSelfContraction(
        C_matrix({a1, a2}), C_matrix({a2, a3}), -delta({a1, a3}));
    C_matrix->addSelfContraction(
        C_matrix({a1, a2}), C_matrix({a3, a2}), delta({a1, a3}));
    C_matrix->addSelfContraction(
        C_matrix({a2, a1}), C_matrix({a2, a3}), delta({a1, a3}));
    C_matrix->addSelfContraction(
        C_matrix({a1, a2}), gamma({mu, a1, a2}), CSL_0);
    C_matrix->addSelfContraction(
        C_matrix({a1, a2}), gamma({mu, a2, a1}), CSL_0);

    sigma->addComplexProperty(sigma({mu, nu, a1, a4}),
                              -gamma({2, a1, a2}) * sigma({mu, nu, a2, a3})
                                  * gamma({2, a3, a4}));

    sigma->addHermitianProperty(
        this, sigma({mu, nu, a1, a4}), sigma({mu, nu, a1, a4}));

    sigma->addTransposedProperty(this,
                                 sigma({mu, nu, a1, a6}),
                                 -gamma({2, a1, a3}) * sigma({mu, nu, a3, a4})
                                     * gamma({2, a4, a1}));

    if (dim == 4)
        setDiracTensor4(this);
}

csl::vector_expr
DiracSpace::simplifyChain(csl::vector_expr const &tensors) const
{
    sgl::TensorSet tensorset{
        dirac4.gamma_chir, dirac4.C_matrix, dirac4.P_L, dirac4.P_R, {}};
    tensorset.gamma[0] = dirac4.delta;
    tensorset.gamma[1] = dirac4.gamma;
    tensorset.gamma[2] = dirac4.sigma;
    try {
        sgl::GExpr sglTest
            = sgl::csl_to_sgl(csl::prod_s(tensors, true), tensorset);
        sgl::Simplify(sglTest);
        return {sgl::sgl_to_csl(sglTest, tensorset)};
    }
    catch (sgl::Exception const &ex) {
        std::cerr << "Exception occured during simplification of : " << '\n';
        std::cerr << csl::prod_s(tensors, true) << '\n';
        std::cerr << ex.what() << std::endl;
        throw ex;
    }
}

size_t DiracSpace::getSpinorDimension(size_t spaceTimeDim)
{
    return pow(2, spaceTimeDim / 2);
}

bool DiracSpace::isGammaTensor(csl::Expr const &tensor) const
{
    if (not IsIndicialTensor(tensor))
        return false;
    auto parent = tensor->getParent_info();
    return (parent == gamma.get() or parent == sigma.get()
            or parent == gamma_chir.get() or parent == P_L.get()
            or parent == P_R.get() or parent == C_matrix.get());
}

bool DiracSpace::isDelta(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == delta.get();
}

bool DiracSpace::isGammaMu(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == gamma.get();
}

bool DiracSpace::isSigma(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == sigma.get();
}

bool DiracSpace::isGammaChir(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == gamma_chir.get();
}

bool DiracSpace::isP_L(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == P_L.get();
}

bool DiracSpace::isP_R(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == P_R.get();
}

bool DiracSpace::isProjector(csl::Expr const &tensor) const
{
    return isP_L(tensor) or isP_R(tensor);
}

bool DiracSpace::isCMatrix(csl::Expr const &tensor) const
{
    return tensor->getParent_info() == C_matrix.get();
}

bool DiracSpace::isSymmetric(csl::Expr const &tensor) const
{
    return isGammaChir(tensor) or isProjector(tensor) or isDelta(tensor)
           or isCMatrix(tensor);
}

bool DiracSpace::isGammaTensor(csl::Expr_info tensor) const
{
    if (not IsIndicialTensor(tensor))
        return false;
    auto parent = tensor->getParent_info();
    return (parent == gamma.get() or parent == sigma.get()
            or parent == gamma_chir.get() or parent == P_L.get()
            or parent == P_R.get() or parent == C_matrix.get());
}

bool DiracSpace::isDelta(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == delta.get();
}

bool DiracSpace::isGammaMu(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == gamma.get();
}

bool DiracSpace::isSigma(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == sigma.get();
}

bool DiracSpace::isGammaChir(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == gamma_chir.get();
}

bool DiracSpace::isP_L(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == P_L.get();
}

bool DiracSpace::isP_R(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == P_R.get();
}

bool DiracSpace::isProjector(csl::Expr_info tensor) const
{
    return isP_L(tensor) or isP_R(tensor);
}

bool DiracSpace::isCMatrix(csl::Expr_info tensor) const
{
    return tensor->getParent_info() == C_matrix.get();
}

bool DiracSpace::isSymmetric(csl::Expr_info tensor) const
{
    return isGammaChir(tensor) or isProjector(tensor) or isDelta(tensor)
           or isCMatrix(tensor);
}

bool DiracSpace::isDelta(csl::Index const &index) const
{
    return index.getType() == cslIndex::Fixed or index == dim + 4;
}

bool DiracSpace::isGammaMu(csl::Index const &index) const
{
    return index.getType() != cslIndex::Fixed or index.getValue() < dim;
}

bool DiracSpace::isGammaChir(csl::Index const &index) const
{
    return index.getType() == cslIndex::Fixed and index == dim + 1;
}

bool DiracSpace::isP_L(csl::Index const &index) const
{
    return index.getType() == cslIndex::Fixed and index == dim + 2;
}

bool DiracSpace::isP_R(csl::Index const &index) const
{
    return index.getType() == cslIndex::Fixed and index == dim + 3;
}

bool DiracSpace::isProjector(csl::Index const &index) const
{
    return isP_L(index) or isP_R(index);
}

bool DiracSpace::isCMatrix(csl::Index const &index) const
{
    return index.getType() == cslIndex::Fixed and index == dim + 5;
}

csl::Index DiracSpace::index() const
{
    return generateIndex();
}

csl::Index DiracSpace::st_index() const
{
    return spaceTime->generateIndex();
}

csl::Index DiracSpace::getSpaceTimeIndex(csl::Expr const &tensor) const
{
    if (isGammaMu(tensor))
        return tensor->getIndexStructureView()[0];
    else if (isGammaChir(tensor))
        return csl::Index(dim + 1);
    else if (isP_L(tensor))
        return csl::Index(dim + 2);
    else if (isP_R(tensor))
        return csl::Index(dim + 3);
    else if (isDelta(tensor))
        return csl::Index(dim + 4);
    else if (isCMatrix(tensor))
        return csl::Index(dim + 5);
    CallHEPError(mty::error::TypeError,
                 "Tensor " + toString(tensor)
                     + " not recognized in Dirac chain.");
    return csl::Index();
}

csl::Index DiracSpace::getFirstIndex(csl::Expr const &tensor) const
{
    const auto &structure = tensor->getIndexStructureView();
    HEPAssert(structure.size() >= 1,
              mty::error::TypeError,
              "Tensor " + toString(tensor) + " have less than one index.");
    return structure[structure.size() - 2];
}

csl::Index DiracSpace::getSecondIndex(csl::Expr const &tensor) const
{
    const auto &structure = tensor->getIndexStructureView();
    HEPAssert(structure.size() >= 2,
              mty::error::TypeError,
              "Tensor " + toString(tensor) + " have less than two indices.");
    return structure[structure.size() - 1];
}

} // End of namespace mty
