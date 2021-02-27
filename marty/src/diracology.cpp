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
#include "mrtError.h"
#include "polarization.h"
#include "mrtUtils.h"
#include "quantumField.h"
#include "mrtInterface.h"

using namespace std;
using namespace csl;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Helper functions                              //
/*************************************************/
///////////////////////////////////////////////////

csl::Expr slashed_s(
        csl::Tensor     p,
        const csl::Index& alpha,
        const csl::Index& beta,
        const DiracSpace* space)
{
    csl::Index mu = csl::Minkowski.generateIndex();
    return space->gamma({+mu,alpha,beta})*p(mu);
}

csl::Expr bar_s(const csl::Expr&       tensor,
           const DiracSpace* space)
{
    csl::Index beta = space->generateIndex();
    const csl::IndexStructure& structure = tensor->getIndexStructureView();
    csl::Index alpha;
    for (const auto& i : structure)
        if (i.getSpace() == space)
            alpha = i;

    csl::Expr res = csl::Replaced(tensor, alpha, beta);
    
    return csl::GetComplexConjugate(res) * space->gamma({0,beta,alpha});
}

///////////////////////////////////////////////////
/*************************************************/
// Class DiracSpace                              //
/*************************************************/
///////////////////////////////////////////////////

DiracSpace::DiracSpace(csl::Space const* t_spaceTime)
    :csl::Space("dirac",
                getSpinorDimension(t_spaceTime->getDim()),
                {"alpha", "beta", "gam", "del", "eps", "eta"}),
    spaceTime(t_spaceTime)
{
    gamma = csl::tensor_s(
            "gamma",
            {spaceTime, this, this});

    sigma = csl::tensor_s(
            "sigma",
            {spaceTime, spaceTime, this, this});

    gamma_chir = csl::tensor_s(
            (dim == 4) ? "gamma5" : "gamma_chir",
            {this, this});

    P_L = csl::tensor_s(
            "P_L",
            {this, this});

    P_R = csl::tensor_s(
            "P_R",
            {this, this});

    C_matrix = csl::tensor_s(
            "C",
            std::vector<csl::Space const*>({this, this})
            );

    initProperties();
}

DiracSpace::~DiracSpace()
{
    
}

csl::Space const* DiracSpace::getSpaceTime() const 
{
    return spaceTime;
}

bool DiracSpace::hasSpecialTraceProperty(
        const csl::vector_expr& tensors) const
{
    if (keepCycles) {
        for (const auto& t : tensors)
            if (not isGammaTensor(t))
                return false;
        return true;
    }

    return false;
}

void expandAbbreviations(csl::vector_expr& tensors)
{
    bool hasFc = false;
    for (const auto &t : tensors) {
        const auto &name = t->getName();
        if (name[0] == 'F' && name[1] == 'c') {
            hasFc = true;
            break;
        }
    }
    if (!hasFc)
        return;
    csl::vector_expr newTensors;
    newTensors.reserve(tensors.size());

    for (const auto& t : tensors) {
        csl::Abbrev::enableGenericEvaluation("Fc");
        csl::Expr eval = Evaluated(t);
        if (eval->getType() == csl::Type::Prod)
            for (const auto& child : *eval)
                newTensors.push_back(child);
        else
            newTensors.push_back(eval);
        csl::Abbrev::disableGenericEvaluation("Fc");
    }

    tensors = std::move(newTensors);
}

csl::Expr DiracSpace::calculateTrace(csl::vector_expr tensors) const
{
    for (const auto& t : tensors) {
        if (t->getType() == csl::Type::Sum) {
            csl::Expr expanded = Expanded(prod_s(tensors, true));
            if (expanded->getType() != csl::Type::Sum)
                return expanded;
            std::vector<csl::Expr> terms;
            terms.reserve(expanded->size());
            for (const auto& arg : *expanded) {
                terms.push_back(calculateTrace(arg));
            }
            return sum_s(terms);
        }
    }
    if (tensors.empty())
        return csl::int_s(dim);
    expandAbbreviations(tensors);
    for (const auto& t : tensors)
        if (!isGammaTensor(t))
            return prod_s(tensors, true);
    size_t count = countGammaMult(tensors);
    if (count % 2 == 1) {
        return CSL_0;
    }

    auto aligned = align(tensors);
    size_t pos = 0;
    csl::vector_expr res;
    res.reserve(1 + aligned.cutsBtwCycles.size());
    for (size_t i : aligned.cutsBtwCycles) {
        csl::vector_expr t(i - pos);
        std::move(aligned.tensors.begin()+pos,
                  aligned.tensors.begin()+i,
                  t.begin());
        res.push_back(compute(t));
        pos = i;
    }
    csl::vector_expr t(aligned.tensors.size() - pos);
    std::move(aligned.tensors.begin() + pos,
              aligned.tensors.end(),
              t.begin());
    res.push_back(compute(t));

    return csl::prod_s(res);
}

void setDiracTensor4(const DiracSpace* self)
{
    self->gamma->setTensor(
            csl::highdtensor_s(
            {{{CSL_0,CSL_0,CSL_1,CSL_0},
              {CSL_0,CSL_0,CSL_0,CSL_1},
              {CSL_1,CSL_0,CSL_0,CSL_0},
              {CSL_0,CSL_1,CSL_0,CSL_0}},

             {{CSL_0,CSL_0,CSL_0,CSL_1},
              {CSL_0,CSL_0,CSL_1,CSL_0},
              {CSL_0,CSL_M_1,CSL_0,CSL_0},
              {CSL_M_1,CSL_0,CSL_0,CSL_0}},

             {{CSL_0,CSL_0,CSL_0,-CSL_I},
              {CSL_0,CSL_0,CSL_I,CSL_0},
              {CSL_0,CSL_I,CSL_0,CSL_0},
              {-CSL_I,CSL_0,CSL_0,CSL_0}},

             {{CSL_0,CSL_0,CSL_1,CSL_0},
              {CSL_0,CSL_0,CSL_0,CSL_M_1},
              {CSL_M_1,CSL_0,CSL_0,CSL_0},
              {CSL_0,CSL_1,CSL_0,CSL_0}}}
        ));

        self->C_matrix->setTensor(
                csl::matrix_s(
                    {{CSL_0, CSL_1, CSL_0, CSL_0},
                    {CSL_M_1, CSL_0, CSL_0, CSL_0},
                    {CSL_0, CSL_0, CSL_0, CSL_M_1},
                    {CSL_0, CSL_0, CSL_1, CSL_0}}
                ));

        self->gamma_chir->setTensor(
             csl::matrix_s({{CSL_M_1,CSL_0,CSL_0,CSL_0},
                           {CSL_0,CSL_M_1,CSL_0,CSL_0},
                           {CSL_0,CSL_0,CSL_1,CSL_0},
                           {CSL_0,CSL_0,CSL_0,CSL_1}})
                );

        self->P_L->setTensor(
             csl::matrix_s({{CSL_1,CSL_0,CSL_0,CSL_0},
                           {CSL_0,CSL_1,CSL_0,CSL_0},
                           {CSL_0,CSL_0,CSL_0,CSL_0},
                           {CSL_0,CSL_0,CSL_0,CSL_0}})
                );

        self->P_R->setTensor(
             csl::matrix_s({{CSL_0,CSL_0,CSL_0,CSL_0},
                           {CSL_0,CSL_0,CSL_0,CSL_0},
                           {CSL_0,CSL_0,CSL_1,CSL_0},
                           {CSL_0,CSL_0,CSL_0,CSL_1}})
                );

    csl::Expr sigmaTensor = csl::highdtensor_s(std::vector<int>({4,4,4,4}));
    csl::Expr gammaMatrix = dirac4.gamma->getTensor();
    for (int mu = 0; mu != 4; ++mu)
        for (int nu = 0; nu != 4; ++nu) 
            for (int alpha = 0; alpha != 4; ++alpha) 
                for (int beta = 0; beta != 4; ++beta)  {
                    csl::Expr res = CSL_0;
                    for (int delta = 0; delta != 4; ++delta) 
                        res = res 
                            + gammaMatrix->getArgument({mu,alpha,delta})
                              * gammaMatrix->getArgument({nu,delta,beta})
                            - gammaMatrix->getArgument({nu,alpha,delta})
                              * gammaMatrix->getArgument({mu,delta,beta});
                    sigmaTensor->setArgument(Expanded(res*CSL_I/2),
                            {mu,nu,alpha,beta});
                }
    self->sigma->setTensor(sigmaTensor);
}

std::vector<csl::Index> DiracSpace::applyInvolution(
        csl::vector_expr& tensors,
        csl::Expr             & factor) const
{
    std::vector<csl::Index> indices;
    indices.reserve(tensors.size());
    indices.push_back(getSpaceTimeIndex(tensors[0]));
    if (isGammaMu(tensors[0])) {
        // Testing transposed
        csl::Index ind = getSecondIndex(tensors[0]);
        for (size_t i = 1; i != tensors.size(); ++i) {
            auto mu    = getSpaceTimeIndex(tensors[i]);
            auto alpha = getFirstIndex(tensors[i]);
            auto beta  = getSecondIndex(tensors[i]);
            if (alpha == ind)
                ind = beta;
            else {
                ind = alpha;
                if (true or not isSymmetric(tensors[i])) {
                    tensors[i]->print();
                    std::cout << "In " << std::endl;
                    for (const auto &t : tensors)
                        std::cout << t << " ; ";
                    std::cout << std::endl;
                    CallHEPError(mty::error::RuntimeError,
                            "Badly aligned gamma tensor encountered,"
                            + toString(mu) + " should not be T / *.");
                    if (tensors[i]->isComplexConjugate()) {
                        indices.push_back(0);
                        indices.push_back(mu);
                        indices.push_back(0);
                        continue;
                    }
                    else {
                        indices.push_back(0);
                        indices.push_back(2);
                        indices.push_back(mu);
                        indices.push_back(2);
                        indices.push_back(0);
                        factor = factor * CSL_M_1;
                        continue;
                    }
                }
            }
            if (tensors[i]->isComplexConjugate()) {
                CallHEPError(mty::error::RuntimeError,
                        "Badly conjugated gamma tensor encountered,"
                        + toString(mu) + " should not be T / *.");
                factor = factor * CSL_M_1;
                indices.push_back(2);
                indices.push_back(mu);
                indices.push_back(2);
                continue;
            }
            indices.push_back(mu);
        }
    }
    else
        for (size_t i = 1; i != tensors.size(); ++i) {
            auto st_index = getSpaceTimeIndex(tensors[i]);
            indices.push_back(st_index);
        }

    return indices;
}

void DiracSpace::applyChiralityProp(
        std::vector<csl::Index>& tensors,
        csl::Expr                   & factor) const
{
    for (size_t i = 1; i < tensors.size(); ++i) {
        if (isGammaChir(tensors[i])) {
            bool simplified = false;
            for (size_t j = i; j --> 0 ;)
                if (isGammaMu(tensors[j]))
                    factor = -factor;
                else if (isProjector(tensors[j])) {
                    factor = (isP_L(tensors[j])) ? -factor : factor;
                    tensors.erase(tensors.begin() + i);
                    simplified = true;
                    break;
                }
                else if (isGammaChir(tensors[j])){
                    tensors.erase(tensors.begin() + i);
                    tensors.erase(tensors.begin() + j);
                    i -= 2;
                    simplified = true;
                    break;
                }
                else
                    CallHEPError(mty::error::TypeError,
                            "Tensor " + toString(tensors[j]) + " should not "
                            "appear.");
            if (not simplified) {
                tensors.erase(tensors.begin() + i);
                tensors.insert(tensors.begin(), dim+1);
            }
        }
        else if (isProjector(tensors[i])) {
            for (size_t j = i; j --> 0 ;) {
                if (isGammaMu(tensors[j]))
                    flipChirality(tensors[i]);
                else if (isGammaChir(tensors[j])) {
                    factor = (isP_L(tensors[i])) ? -factor : factor;
                    tensors.erase(tensors.begin() + j);
                    --i;
                    break;
                }
                else if (isProjector(tensors[j])) {
                    if (tensors[i] == tensors[j]) {
                        tensors.erase(tensors.begin() + j);
                        --i;
                        break;
                    }
                    else {
                        factor = CSL_0;
                        return;
                    }
                }
                else
                    CallHEPError(mty::error::TypeError,
                            "Tensor " + toString(tensors[j]) + " should not "
                            "appear.");
            }
            tensors.insert(tensors.begin(), tensors[i]);
            tensors.erase(tensors.begin() + i+1);
        }
    }
}

void DiracSpace::simplifySquares(
        std::vector<csl::Index>& tensors,
        csl::Expr                   & factor) const
{
    for (size_t i = 0; i+1 < tensors.size(); ++i)
        if (tensors[i].getType() == cslIndex::Fixed
                and tensors[i+1].getType() ==  cslIndex::Fixed
                and tensors[i].getValue() == tensors[i+1].getValue()) {
            if (tensors[i].getSign() == tensors[i+1].getSign()) {
                if (tensors[i].getSign()) {
                    factor = factor * spaceTime
                        ->getInverseMetric()({tensors[i], tensors[i+1]});
                    tensors.erase(tensors.begin() + i);
                    tensors.erase(tensors.begin() + i);
                    simplifySquares(tensors, factor);
                    return;
                }
                else {
                    factor = factor * spaceTime
                        ->getMetric()({tensors[i], tensors[i+1]});
                    tensors.erase(tensors.begin() + i);
                    tensors.erase(tensors.begin() + i);
                    simplifySquares(tensors, factor);
                    return;
                }
            }
            else {
                factor = factor * spaceTime
                    ->getDelta()({tensors[i], tensors[i+1]});
                tensors.erase(tensors.begin() + i);
                tensors.erase(tensors.begin() + i);
                simplifySquares(tensors, factor);
                return;
            }
        }
        else if (tensors[i] == tensors[i+1] and isGammaMu(tensors[i])) {
            factor = factor * csl::DMinko;
            tensors.erase(tensors.begin() + i);
            tensors.erase(tensors.begin() + i);
            simplifySquares(tensors, factor);
            return;
        }
}

csl::Expr DiracSpace::applyRecursion(
        std::vector<csl::Index>& tensors,
        csl::Expr                   & factor) const
{
    if (tensors.empty())
        return factor * getDim();

    if (isProjector(tensors[0])) {
        factor = factor * CSL_HALF;
        bool sign = isP_L(tensors[0]);
        tensors.erase(tensors.begin());
        csl::Expr res = applyStandardRecursion(tensors);
        if (sign)
            res = res - applyChiralRecursion(tensors);
        else
            res = res + applyChiralRecursion(tensors);
        return factor * res;
    }
    if (isGammaChir(tensors[0])) {
        tensors.erase(tensors.begin());
        return factor * applyChiralRecursion(tensors);
    }
    return factor * applyStandardRecursion(tensors);
}

csl::Expr DiracSpace::applyStandardRecursion(
        std::vector<csl::Index> const& tensors,
        bool                           first) const
{
    if (tensors.size() % 2 == 1)
        return CSL_0;
    // Trace(1) = D
    if (tensors.empty())
        return csl::int_s(dim);
    // Trace(gamma_mu gamma_nu) = 4 g_mu,nu
    if (tensors.size() == 2)
        return 4 * spaceTime->getMetric()({tensors[0], tensors[1]});

    csl::vector_expr terms(tensors.size()-1);
    for (size_t j = 1; j != tensors.size(); ++j) {
        std::vector<csl::Index> copy = tensors;
        csl::Expr metricTerm = spaceTime->getMetric()({copy[0], copy[j]});
        copy.erase(copy.begin()+j);
        copy.erase(copy.begin());
        csl::Expr recursiveCall = applyStandardRecursion(copy, false);
        if (true or recursiveCall->getType() != csl::Type::Sum) {
            terms[j-1] = csl::prod_s({csl::pow_s(-CSL_1, csl::int_s(j-1)),
                                      Copy(metricTerm),
                                      recursiveCall},
                                      true);
        }
        else {
            terms[j-1] = recursiveCall;
            for (auto &arg : *terms[j-1])
                arg = csl::prod_s({csl::pow_s(-CSL_1, csl::int_s(j-1)),
                                   Copy(metricTerm),
                                   Copy(arg)});
        }
    }

    if (first) {
        for (auto &term : terms)
            term = csl::Refreshed(term);
        return sum_s(terms);
    }
    return sum_s(terms, true);
}

csl::Expr DiracSpace::applyChiralRecursion(
        std::vector<csl::Index> const& tensors,
        bool) const
{
    // Trace(gamma_5) = 0
    // Trace(gamma_5 g) = 0
    // Trace(gamma_5 g g) = 0
    // Trace(gamma_5 g g g) = 0
    if (tensors.size() < 4)
        return CSL_0;
    // Trace(gamma_5 g g g g) = 4i epsilon mu,nu,rho,sigma
    if (tensors.size() == 4) {
        return -4*CSL_I * spaceTime->getEpsilon()(tensors);
    }

    csl::vector_expr terms;
    for (size_t i = 0; i != tensors.size()-1; ++i) {
        int globalSign = (i%4 >= 2) ? -1 : 1;
        for (size_t j = i+1; j != tensors.size(); ++j) {
            int sign = ((i+j+1)%2 == 1) ? -globalSign : globalSign;
            std::vector<csl::Index> recursiveMinkoIndices(tensors.size()-2);
            size_t recursiveIndex = 0;
            for (size_t k = i+1; k != tensors.size(); ++k,
                                                      ++recursiveIndex)
                if (k != j)
                    recursiveMinkoIndices[recursiveIndex] = tensors[k];
                else 
                    --recursiveIndex;
            recursiveIndex = tensors.size()-(i+2);
            for (size_t k = i; k --> 0 ; ++recursiveIndex)
                recursiveMinkoIndices[recursiveIndex] = tensors[k];

            terms.push_back(
                        csl::prod_s({
                            csl::int_s(sign),
                            spaceTime->getMetric()({tensors[i],
                                                    tensors[j]}),
                            applyChiralRecursion(recursiveMinkoIndices)
                            },
                            true
                            )
                        );
        }
    }
    return sum_s(terms, true);
}

csl::Expr DiracSpace::compute(csl::vector_expr const& aligned) const
{
    csl::Expr factor = CSL_1;
    std::map<csl::Index, csl::Index> sigmaAntiSymmetry;
    csl::vector_expr finalTensors;

    finalTensors.reserve(aligned.size());
    for (const auto& tensor : aligned)
        if (isSigma(tensor)) {
            csl::Index mu = tensor->getIndexStructureView()[0];
            csl::Index nu = tensor->getIndexStructureView()[1];
            csl::Index alpha = tensor->getIndexStructureView()[2];
            csl::Index beta  = tensor->getIndexStructureView()[3];
            csl::Index gam = generateIndex();
            finalTensors.push_back(gamma({mu, alpha, gam}));
            finalTensors.push_back(gamma({nu, gam, beta}));
            factor = factor * CSL_I / 2;
            sigmaAntiSymmetry[mu] = nu;
        }
        else {
            finalTensors.push_back(tensor);
        }
    auto indices = applyInvolution(finalTensors, factor);
    applyChiralityProp(indices, factor);
    if (factor == CSL_0)
        return CSL_0;
    simplifySquares(indices, factor);

    csl::Expr res = applyRecursion(indices, factor);
    for (const auto& antiSymmetry : sigmaAntiSymmetry) {
        csl::Expr swapped = csl::Swapped(
                res,
                antiSymmetry.first,
                antiSymmetry.second,
                false);
        res = res - swapped;
    }

    return DeepRefreshed(res);
}

csl::Expr DiracSpace::calculateTrace(csl::Expr const& init) const
{
    csl::ScopedProperty p1(&keepCycles, true);
    csl::ScopedProperty p2(&csl::option::applyChainContractions, true);
    csl::Expr res = DeepRefreshed(init);
    return res;
}

void DiracSpace::initProperties()
{
    sigma->addAntiSymmetry(0, 1);
    // gamma_chir->setFullySymmetric();
    // P_L->setFullySymmetric();
    // P_R->setFullySymmetric();

    csl::Index mu = st_index();
    csl::Index nu = st_index();
    csl::Index rho = st_index();
    csl::Index sig = st_index();
    csl::Index a1 = index();
    csl::Index a2 = index();
    csl::Index a3 = index();
    csl::Index a4 = index();
    csl::Index a5 = index();
    csl::Index a6 = index();


    gamma->addSelfContraction(
            gamma({+mu, a1, a2}), gamma({mu, a2, a3}),
            csl::DMinko * delta({a1, a3}));
    csl::vector_expr sign = spaceTime->getSignature();
    for (csl::Index i = 0; i != spaceTime->getDim(); ++i) {
        gamma->addSelfContraction(
                gamma({i, a1, a2}), gamma({i, a2, a3}),
                spaceTime->getMetric()({i, i})
                        * delta({a1, a3}));
        gamma->addSelfContraction(
                gamma({i, a1, a2}), gamma({+i, a2, a3}),
                spaceTime->getDelta()({i, +csl::Index(i)})
                        * delta({a1, a3}));
        gamma->addSelfContraction(
                gamma({+i, a1, a2}), gamma({i, a2, a3}),
                spaceTime->getDelta()({+csl::Index(i), i})
                        * delta({a1, a3}));
        gamma->addSelfContraction(
                gamma({+i, a1, a2}), gamma({+i, a2, a3}),
                spaceTime->getInverseMetric()(
                    {+csl::Index(i), +csl::Index(i)})
                        * delta({a1, a3}));
    }

    gamma_chir->addSelfContraction(
            gamma_chir({a1, a2}), gamma_chir({a2, a3}),
            delta({a1, a3}));
    P_L->addSelfContraction(
            P_L({a1, a2}), P_L({a2, a3}),
            P_L({a1, a3}));
    P_R->addSelfContraction(
            P_R({a1, a2}), P_R({a2, a3}),
            P_R({a1, a3}));

    P_L->addSelfContraction(
            P_L({a1, a2}), P_R({a2, a3}),
            CSL_0);
    P_L->addSelfContraction(
            P_L({a2, a1}), P_R({a3, a2}),
            CSL_0);

    csl::Tensor eps = spaceTime->getEpsilon();
    csl::Tensor g   = spaceTime->getMetric();
    eps->addSelfContraction(
            eps({mu, nu, rho, sig}), sigma({+rho, +sig, a1, a2}),
            -2*CSL_I * sigma({mu, nu, a1, a3}) * gamma_chir({a3, a2})
            );
    eps->addSelfContraction(
            eps({mu, nu, rho, sig}), gamma({+sig, a1, a2}),
            -CSL_I * (
                gamma({mu, a1, a4})*gamma({nu, a4, a5})*gamma({rho, a5, a3})
                - g({mu, nu})  * gamma({rho, a1, a3})
                + g({mu, rho}) * gamma({nu, a1, a3})
                - g({nu, rho}) * gamma({mu, a1, a3})
                ) * gamma_chir({a3, a2})
            );

    gamma->addComplexProperty(gamma({mu, a1, a4}),
                             gamma({2,a1,a2})
                           * gamma({mu,a2,a3})
                           * gamma({2,a3,a4}));

    gamma->addHermitianProperty(this,
                               gamma({mu,a1, a4}),
                               gamma({mu,a1, a4}));

    gamma->addTransposedProperty(this,
                                gamma({mu,a1,a6}),
                                gamma({2,a1,a3})
                              * gamma({mu,a3,a4})
                              * gamma({2,a4,a6}));

    gamma->addTraceLessNess(this);
    gamma_chir->addTraceLessNess(this);
    sigma->addTraceLessNess(this);
    C_matrix->addTraceLessNess(this);
    C_matrix->setComplexProperty(csl::ComplexProperty::Real);
    C_matrix->addHermitianProperty(this,
                               C_matrix({a1, a4}),
                               -C_matrix({a1, a4}));
    C_matrix->addTransposedProperty(this,
                               C_matrix({a1, a4}),
                               -C_matrix({a1, a4}));

    gamma_chir->addTransposedProperty(
            this,
            gamma_chir({a1, a2}),
            - gamma_chir({a1, a2}));
    gamma_chir->addHermitianProperty(
            this,
            gamma_chir({a1, a2}),
            - gamma_chir({a1, a2}));
    P_L->addTransposedProperty(
            this,
            P_L({a1, a2}),
            P_R({a1, a2}));
    P_R->addTransposedProperty(
            this,
            P_R({a1, a2}),
            P_L({a1, a2}));
    P_L->addHermitianProperty(
            this,
            P_L({a1, a2}),
            P_R({a1, a2}));
    P_R->addHermitianProperty(
            this,
            P_R({a1, a2}),
            P_L({a1, a2}));

    C_matrix->setComplexProperty(csl::ComplexProperty::Real);
    C_matrix->addSelfContraction(
            C_matrix({a1, a2}), C_matrix({a2, a3}),
            -delta({a1, a3})
            );
    C_matrix->addSelfContraction(
            C_matrix({a1, a2}), C_matrix({a3, a2}),
            delta({a1, a3})
            );
    C_matrix->addSelfContraction(
            C_matrix({a2, a1}), C_matrix({a2, a3}),
            delta({a1, a3})
            );
    C_matrix->addSelfContraction(
            C_matrix({a1, a2}), gamma({mu, a1, a2}),
            CSL_0
            );
    C_matrix->addSelfContraction(
            C_matrix({a1, a2}), gamma({mu, a2, a1}),
            CSL_0
            );

    sigma->addComplexProperty(sigma({mu, nu, a1, a4}),
                             -gamma({2,a1,a2})
                           * sigma({mu,nu,a2,a3})
                           * gamma({2,a3,a4}));

    sigma->addHermitianProperty(this,
                               sigma({mu,nu,a1, a4}),
                               sigma({mu,nu,a1, a4}));

    sigma->addTransposedProperty(this,
                                sigma({mu,nu,a1,a6}),
                                -gamma({2,a1,a3})
                              * sigma({mu,nu,a3,a4})
                              * gamma({2,a4,a1}));

    if (dim == 4)
        setDiracTensor4(this);
}

DiracSpace::alignedCycle DiracSpace::align(csl::vector_expr tensors) const
{
    size_t pos_first = 0;
    for (size_t i = 0; i != tensors.size(); ++i) 
        if (isGammaMu(tensors[i])) {
            pos_first = i;
            break;
        }

    csl::vector_expr res;
    res.reserve(tensors.size());
    res.push_back(tensors[pos_first]);
    csl::Index index = getSecondIndex(tensors[pos_first]);
    csl::Index first = getFirstIndex(tensors[pos_first]);
    tensors.erase(tensors.begin() + pos_first);
    while (not tensors.empty()) {
        for (auto iter = tensors.begin(); iter != tensors.end(); ++iter)
            if (getFirstIndex(*iter) == index) {
                index = getSecondIndex(*iter);
                res.push_back(*iter);
                tensors.erase(iter);
                break;
            }
            else if (getSecondIndex(*iter) == index) {
                index = getFirstIndex(*iter);
                res.push_back(*iter);
                tensors.erase(iter);
                break;
            }
        if (index == first and not tensors.empty()) {
            auto nextCycle = align(tensors);
            for (auto& cut : nextCycle.cutsBtwCycles)
                cut += res.size();
            std::vector<size_t> cuts(1, res.size());
            cuts.insert(cuts.end(),
                        nextCycle.cutsBtwCycles.begin(),
                        nextCycle.cutsBtwCycles.end());
            res.insert(res.end(),
                       nextCycle.tensors.begin(),
                       nextCycle.tensors.end());
            return {res, cuts};
        }
    }

    return {res, std::vector<size_t>()};
}

DiracSpace::alignedCycle DiracSpace::alignOpen(csl::vector_expr tensors) const
{
    size_t pos_first = 0;
    for (size_t i = 0; i != tensors.size(); ++i) 
        if (isGammaMu(tensors[i])) {
            pos_first = i;
            break;
        }

    csl::vector_expr res;
    res.reserve(tensors.size());
    res.push_back(tensors[pos_first]);
    csl::Index index = getSecondIndex(tensors[pos_first]);
    csl::Index first = getFirstIndex(tensors[pos_first]);
    tensors.erase(tensors.begin() + pos_first);
    bool reverse = false;
    while (not tensors.empty()) {
        bool indexFound = false;
        for (auto iter = tensors.begin(); iter != tensors.end(); ++iter)
            if (getFirstIndex(*iter) == index) {
                index = getSecondIndex(*iter);
                if (reverse) 
                    res.insert(res.begin(), *iter);
                else 
                    res.push_back(*iter);
                tensors.erase(iter);
                indexFound = true;
                break;
            }
            else if (getSecondIndex(*iter) == index) {
                index = getFirstIndex(*iter);
                if (reverse) 
                    res.insert(res.begin(), *iter);
                else 
                    res.push_back(*iter);
                tensors.erase(iter);
                indexFound = true;
                break;
            }
        if (not reverse and not indexFound) {
            index = first;
            reverse = true;
        }
        else if (((reverse and not indexFound) or index == first) 
                    and not tensors.empty()) {
            auto nextCycle = alignOpen(tensors);
            std::vector<size_t> cuts(1, res.size());
            for (size_t &cut : nextCycle.cutsBtwCycles)
                cut += res.size();
            cuts.insert(cuts.end(),
                        nextCycle.cutsBtwCycles.begin(),
                        nextCycle.cutsBtwCycles.end());
            res.insert(res.end(),
                       nextCycle.tensors.begin(),
                       nextCycle.tensors.end());
            return {res, cuts};
        }
    }

    return {res, std::vector<size_t>()};
}

void DiracSpace::insert(csl::Index       const & toInsert,
                        std::vector<csl::Index>& tensors,
                        std::vector<size_t>    & cuts,
                        size_t i) const
{
    tensors.insert(tensors.begin() + i, toInsert);
    for (size_t& pos : cuts) {
        if (pos > i)
            ++pos;
    }
}

void DiracSpace::contract(std::vector<csl::Index>& tensors,
                          std::vector<size_t>    & cuts,
                          size_t i) const
{
    tensors.erase(tensors.begin() + i);
    for (size_t& pos : cuts) {
        if (pos > i)
            --pos;
    }
}

void DiracSpace::contract(std::vector<csl::Index>& tensors,
                          std::vector<size_t>    & cuts,
                          size_t i,
                          size_t j) const
{
    tensors.erase(tensors.begin() + j);
    tensors.erase(tensors.begin() + i);
    for (size_t& pos : cuts) {
        if (pos > j)
            pos -= 2;
    }
}

void DiracSpace::contractTensor(
        std::vector<csl::Index>& tensors,
        std::vector<size_t>    & cuts,
        size_t i,
        size_t j) const
{
    tensors.erase(tensors.begin() + j);
    tensors.erase(tensors.begin() + i);
    for (size_t& pos : cuts) {
        if (pos > i and pos > j)
            pos -= 2;
        else if (pos > i)
            --pos;
    }
}

std::vector<DiracSpace::Chain> DiracSpace::simplifyGammaProd(
        std::vector<csl::Index> const &indices,
        std::vector<size_t>     const &cuts,
        csl::Expr                    const &factor
        ) const
{
    std::vector<Chain> chains;
    std::vector<Chain> newChains(1, { factor, indices, cuts, {} });
    while (!newChains.empty()) {
        std::vector<Chain> simplified;
        simplified.reserve(newChains.size());
        for (auto &c : newChains) {
            std::vector<Chain> interm = simplifyGammaProd(c);
            if (interm.empty()) {
                //interm = simplifyFiertzProd(c);
                if (interm.empty()) {
                    chains.push_back(std::move(c));
                    continue;
                }
            }
            simplified.insert(
                    simplified.end(),
                    std::make_move_iterator(interm.begin()),
                    std::make_move_iterator(interm.end())
                    );
        }
        newChains = std::move(simplified);
    }
    return chains;
}

std::vector<DiracSpace::Chain> DiracSpace::simplifyGammaProd(
        Chain         const& init
        ) const
{

    if (init.size() < 2)
        return {};
    csl::Index minkoIndex;
    size_t index_cut = 0;
    std::vector<DiracSpace::Chain> chains;
    for (size_t i = 0; i != init.size(); ++i) {
        if (i == init.cuts[index_cut])
            ++index_cut;
        if (isGammaMu(init[i]) 
                and init[i].getType() != cslIndex::Fixed) {
            for (size_t j = i+1; j < init.cuts[index_cut]; ++j)
                if (init[i] == init[j]) {
                    if (j == i + 1) {
                        csl::Expr factor = csl::DMinko * init.factor;
                        Chain newChain = init;
                        newChain.factor *= csl::DMinko;
                        contract(newChain.indices, newChain.cuts, i, j);
                        return {newChain};
                    }
                }
        }
    }
    index_cut = 0;
    for (size_t i = 0; i != init.size(); ++i) {
        if (i == init.cuts[index_cut])
            ++index_cut;
        if (isGammaMu(init[i]) 
                and init[i].getType() != cslIndex::Fixed) {
            for (size_t j = i+1; j < init.cuts[index_cut]; ++j)
                if (init[i] == init[j]) {
                    std::vector<Chain> newChains;
                    newChains.reserve(j - i - 1);
                    for (size_t k = i + 1; k != j - 1; ++k) {
                        Chain newChain(init);
                        newChain.factor *= csl::pow_s(-1, k-(i+1)) * 2;
                        csl::Index movedIndex = newChain[k];
                        for (size_t l = k; l != j-1; ++l)
                            newChain[l] = newChain[l+1];
                        newChain[j-1] = movedIndex;
                        contract(
                                newChain.indices, 
                                newChain.cuts, 
                                i, 
                                j);
                        newChains.push_back(newChain);
                    }
                    Chain newChain(init);
                    newChain.factor *= csl::pow_s(-1, j - i - 1) 
                        * (csl::DMinko - 2);
                    contract(
                            newChain.indices, 
                            newChain.cuts, 
                            i, 
                            j);
                    newChains.push_back(newChain);
                    return newChains;
                }
        }
    }
    return {};
}

Chirality DiracSpace::getChirality(
        size_t pos,
        size_t cut,
        csl::Index proj
        ) const
{
    if (not isProjector(proj))
        return Chirality::None;
    Chirality chir = (isP_L(proj)) ? Chirality::Left : Chirality::Right;
    if ((cut - pos) % 2 == 1)
        return !chir;
    return chir;
}

std::vector<DiracSpace::FiertzContraction> DiracSpace::getFiertzContractions(
        DiracSpace::Chain const &init
        ) const
{
    std::vector<FiertzContraction> contractions;
    size_t index_cut = 0;
    for (size_t i = 0; i != init.size(); ++i) {
        if (i == init.cuts[index_cut])
            ++index_cut;
        size_t i_cut = init.cuts[index_cut];
        if (isGammaMu(init[i]) 
                and init[i].getType() != cslIndex::Fixed) {
            Chirality chir_i = getChirality(
                    i, 
                    i_cut,
                    init[i_cut - 1]);
            for (size_t j = i_cut; j != init.size(); ++j)
                if (init[i] == init[j]) {
                    size_t index_j_cut = index_cut+1;
                    for (size_t cut = index_cut+1; 
                            cut != init.cuts.size(); 
                            ++cut)
                        if (j >= init.cuts[cut])
                            ++index_j_cut;
                    size_t j_cut = init.cuts[index_j_cut];
                    Chirality chir_j = getChirality(
                            j, 
                            j_cut,
                            init[j_cut - 1]);
                    contractions.push_back({
                            i, j, index_cut, index_j_cut, chir_i, chir_j
                            });
                    break;
                }
        }
    }

    return contractions;
}

std::vector<DiracSpace::Chain> DiracSpace::simplifyFiertzProd(
        Chain         const& init
        ) const
{

    if (init.size() < 2 or init.cuts.size() < 2)
        return {};
    std::vector<DiracSpace::Chain> chains;
    std::vector<DiracSpace::FiertzContraction> contractions
        = getFiertzContractions(init);
    for (const auto &c : contractions)
        if (c.isChiral() and not c.areChiralitySame()) {
            Chain newChain(init);
            newChain.factor = newChain.factor * csl::DMinko / 2;
            newChain.fiertzFlipping.push_back({c.i_cut, c.j_cut});
            contractTensor(newChain.indices, newChain.cuts, c.i, c.j);
            return {newChain};
        }
    for (size_t i = 0; i != contractions.size(); ++i) {
        FiertzContraction const &c_i = contractions[i];
        if (!c_i.isChiral())
            continue;
        for (size_t j = i + 1; j < contractions.size(); ++j) {
            FiertzContraction const &c_j = contractions[j];
            if (c_i.i_cut == c_j.i_cut
                    and c_i.j_cut == c_j.j_cut) {
                bool sameSide = ((c_i.i < c_j.i) == (c_i.j < c_j.j));
                Chain newChain(init);
                if (!sameSide) {
                    newChain.factor *= CSL_M_1;
                    newChain.fiertzFlipping.push_back({
                            c_i.i_cut,
                            c_i.j_cut
                            });
                }

                if (sameSide)  {
                    for (size_t k = c_i.i+1; k < init.cuts[c_i.i_cut]-1; ++k) {
                            insert(
                                    init[k], 
                                    newChain.indices, 
                                    newChain.cuts, 
                                    c_i.j+1);
                    }
                    for (size_t k = c_i.i+1; k < init.cuts[c_i.i_cut]-1; ++k) {
                        contract(
                                newChain.indices, 
                                newChain.cuts, 
                                c_i.i+1);
                    }
                }
                else {
                    size_t ni = init.cuts[c_i.i_cut]-c_i.i-2;
                    size_t nj = init.cuts[c_i.j_cut]-c_i.j-2;
                    for (size_t k = 0; k < std::min(ni, nj); ++k) {
                        std::swap(newChain[c_i.i+1+k], newChain[c_i.j+1+k]);
                    }
                    size_t n = std::max(ni, nj) - std::min(ni, nj);
                    size_t beginInsert = (ni > nj) ? c_i.j+1 : c_i.i+1;
                    beginInsert += std::min(ni, nj);
                    size_t beginErase  = (ni > nj) ? c_i.i+1 : c_i.j+1;
                    beginErase  += std::min(ni, nj);
                    for (size_t k = 0; k != n; ++k) {
                            insert(
                                    init[beginErase+k], 
                                    newChain.indices, 
                                    newChain.cuts, 
                                    beginInsert+k);
                    }
                    for (size_t k = 0; k != n; ++k) {
                        contract(
                                newChain.indices, 
                                newChain.cuts, 
                                beginErase);
                    }
                }
                return {newChain};
            }
        }
    }

    return {};
}

void DiracSpace::applyUniqueChiralityStructure(
        std::vector<csl::Index>& tensors,
        std::vector<size_t>& cuts,
        csl::Expr& factor) const
{
    enum Mode { None, Chir, Left, Right };
    if (tensors.empty())
        return;
    size_t pos = 0;
    for (size_t p = 0; p != cuts.size(); ++p) {
        if (pos + 1 >= cuts[p]) {
            pos = cuts[p];
            continue;
        }
        for (size_t i = pos; i+1 < cuts[p]; ++i) {
            Mode mode = None;
            if (isGammaChir(tensors[i]))
                mode = Chir;
            else if (isP_L(tensors[i]))
                mode = Left;
            else if (isP_R(tensors[i]))
                mode = Right;
            if (mode == 0)
                continue;
            bool simplified = false;
            for (size_t j = i+1; j < cuts[p]; ++j) {
                if (isGammaChir(tensors[j])){
                    switch(mode) {
                        case Chir:
                            contract(tensors, cuts, i, j);
                            simplified = true;
                            break;
                        case Left:
                            factor *= -1;
                            contract(tensors, cuts, j);
                            --j;
                            break;
                        case Right:
                            contract(tensors, cuts, j);
                            --j;
                            break;
                        default: break;
                    }
                }
                else if (isP_L(tensors[j])) {
                    switch(mode) {
                        case Chir:
                            factor *= -1;
                            contract(tensors, cuts, i);
                            simplified = true;
                            break;
                        case Left:
                            contract(tensors, cuts, j);
                            --j;
                            break;
                        case Right:
                            factor = 0;
                            return;
                        default: break;
                    }
                }
                else if (isP_R(tensors[j])) {
                    switch(mode) {
                        case Chir:
                            factor *= -1;
                            contract(tensors, cuts, i);
                            simplified = true;
                            break;
                        case Left:
                            factor = 0;
                            return;
                        case Right:
                            contract(tensors, cuts, j);
                            --j;
                            break;
                        default: break;
                    }
                }
                else if (isGammaMu(tensors[j])) {
                    if (mode == Left)
                        mode = Right;
                    else if (mode == Right)
                        mode = Left;
                    else
                        factor *= -1;
                }
                if (simplified) {
                    --i;
                    break;
                }
            }
            if (not simplified) {
                for (size_t j = i; j < cuts[p]-1; ++j)
                    tensors[j] = tensors[j+1];
                switch(mode) {
                    case Chir:
                        tensors[cuts[p]-1] = Index(5);
                        break;
                    case Left:
                        tensors[cuts[p]-1] = Index(6);
                        break;
                    case Right:
                        tensors[cuts[p]-1] = Index(7);
                        break;
                    default: break;
                }
            }
        }
        pos = cuts[p];
    }
}

std::pair<csl::Index, csl::Index> DiracSpace::getBorderOfChain(
        std::vector<csl::Expr>::const_iterator first,
        std::vector<csl::Expr>::const_iterator last) const
{
    HEPAssert(first != last,
            mty::error::RuntimeError,
            "Cannot get border indices of an empty fermion chain !");

    std::pair<csl::Index, csl::Index> res;
    if (first+1 == last) {
        res.first = getFirstIndex(*first);
        res.second = getSecondIndex(*first);
        return res;
    }

    csl::Index A = getFirstIndex(*first);
    if (A == getFirstIndex(*(first+1))
            or A == getSecondIndex(*(first+1)))
        res.first = getSecondIndex(*first);
    else
        res.first = A;

    A = getFirstIndex(*(last-1));
    if (A == getFirstIndex(*(last-2))
            or A == getSecondIndex(*(last-2)))
        res.second = getSecondIndex(*(last-1));
    else
        res.second = A;
    return res;
}

std::vector<std::pair<csl::Index, csl::Index>>
    DiracSpace::getBorderOfChains(std::vector<csl::Expr>   const& tensors,
                                  std::vector<size_t>      & cuts) const
{
    std::vector<std::pair<csl::Index, csl::Index>> res;
    if (cuts.empty() or cuts[cuts.size()-1] != tensors.size())
        cuts.push_back(tensors.size());
    if (tensors.empty())
        return res;
    res.reserve(cuts.size());
    auto first = tensors.begin();
    for (size_t cut : cuts) {
        auto last = tensors.begin() + cut;
        res.push_back(getBorderOfChain(first, last));
        first = last;
    }

    return res;
}

csl::vector_expr DiracSpace::applyChainIndices(
        std::vector<csl::Index> const& tensors,
        std::vector<size_t>     const& cuts,
        std::vector<std::pair<size_t, size_t>>         const& flipped,
        std::vector<std::pair<csl::Index, csl::Index>> const& indices) const
{
    HEPAssert(cuts.size() == indices.size(),
            mty::error::RuntimeError,
            "The number of chains in cuts and indices do not match.");
    if (tensors.empty())
        return csl::vector_expr();

    auto getIndex = [&](size_t indexCut)
    {
        size_t currentCut = indexCut;
        for (const auto &[first ,second] : flipped)
            if (first == currentCut) 
                currentCut = second;
            else if (second == currentCut)
                currentCut = first;
        return indices[currentCut].second;
    };

    size_t indexCut = 0;
    csl::vector_expr exprTensors(tensors.size());
    csl::Index current = indices[0].first;
    for (size_t i = 0; i != tensors.size(); ++i) {
        csl::Index second;
        if (i + 1 == cuts[indexCut]) {
            second = getIndex(indexCut);
        }
        else
            second = generateIndex();
        exprTensors[i] = indexToExpr(tensors[i], current, second);
        if (i + 1 == cuts[indexCut]) {
            if (++indexCut == indices.size())
                break;
            current = indices[indexCut].first;
        }
        else
            current = second;
    }

    return exprTensors;
}

csl::vector_expr DiracSpace::simplifyChain(
        csl::vector_expr const& tensors) const
{
    auto aligned = alignOpen(tensors);
    aligned.cutsBtwCycles.push_back(aligned.tensors.size());
    auto indices = getBorderOfChains(aligned.tensors, aligned.cutsBtwCycles);
    csl::Expr factor = CSL_1;
    std::vector<csl::Index> tensorsIndices = exprToIndex(
            aligned.tensors, aligned.cutsBtwCycles);
    //std::cout << "HERE" << std::endl;
    //size_t index = 0;
    //for (const auto &i : tensorsIndices) {
    //    std::cout << i << " ";
    //    if (auto pos = std::find(aligned.cutsBtwCycles.begin(),
    //                aligned.cutsBtwCycles.end(), ++index);
    //            pos != aligned.cutsBtwCycles.end())
    //        std::cout << "| ";
    //}
    //std::cout << std::endl;
    // size_t pos = 0;
    // for (size_t cut_pos = 0;
    //         cut_pos != aligned.cutsBtwCycles.size();
    //         ++cut_pos) {
    //     std::vector<csl::Index> tensors(aligned.cutsBtwCycles[cut_pos]-pos);
    //     std::copy(tensorsIndices.begin()+pos,
    //               tensorsIndices.begin()+aligned.cutsBtwCycles[cut_pos],
    //               tensors.begin());
    //     applyChiralityProp(tensors,
    //                        factor);
    //     std::copy(tensors.begin(),
    //               tensors.end(),
    //               tensorsIndices.begin() + pos);

    //     size_t maxi = tensors.size() + pos;
    //     size_t Ndiff = aligned.cutsBtwCycles[cut_pos] - pos - tensors.size();
    //     if (Ndiff > 0) {
    //         for (size_t pos = cut_pos
    //                 ; pos != aligned.cutsBtwCycles.size()
    //                 ; ++pos)
    //             aligned.cutsBtwCycles[pos] -= Ndiff;
    //         for (size_t i = 0; i != Ndiff; ++i)
    //             tensorsIndices.erase(tensorsIndices.begin() + maxi);
    //     }
    //     pos = aligned.cutsBtwCycles[cut_pos];
    // }
    applyUniqueChiralityStructure(
            tensorsIndices,
            aligned.cutsBtwCycles,
            factor);
    auto chains = simplifyGammaProd(
            tensorsIndices,
            aligned.cutsBtwCycles,
            factor);

    std::vector<csl::Expr> terms;
    terms.reserve(chains.size());
    // std::cout << "RES" << std::endl;
    for (const auto &c : chains) {
        // std::cout << c.factor << " * " << std::endl;
        // for (const auto &i : c.indices)
        //     std::cout << i << " ";
        // std::cout << std::endl;
        auto newTensors = applyChainIndices(
                c.indices,
                c.cuts,
                c.fiertzFlipping,
                indices);
        newTensors.push_back(c.factor);
        terms.push_back(csl::prod_s(newTensors));
    }

    return terms;
}

size_t DiracSpace::getSpinorDimension(size_t spaceTimeDim)
{
    return pow(2, spaceTimeDim / 2);
}

size_t DiracSpace::countGammaMult(csl::vector_expr const& tensors) const
{
    size_t count = 0;
    for (const auto& t : tensors)
        if (isGammaMu(t))
            count += 1;
        else if (isSigma(t))
            count += 2;
        else if (isGammaChir(t))
            count += 4;
        else if (isProjector(t))
            count += 4;
        else if (isCMatrix(t))
            count += 2;

    return count;
}

size_t DiracSpace::countGammaMult(std::vector<csl::Index> const& tensors) const
{
    size_t count = 0;
    for (const auto& t : tensors)
        if (isGammaMu(t))
            count += 1;
        else if (isGammaChir(t))
            count += 4;
        else if (isProjector(t))
            count += 4;
        else if (isCMatrix(t))
            count += 2;

    return count;
}

std::vector<csl::Index> DiracSpace::exprToIndex(
        std::vector<csl::Expr> const& tensors,
        std::vector<size_t>    & cuts) const
{
    std::vector<csl::Index> indices;
    indices.reserve(tensors.size());
    for (size_t i = 0; i != tensors.size(); ++i) {
        csl::Index mu = getSpaceTimeIndex(tensors[i]);
        if (mu != dim+4)
            indices.push_back(mu);
        else
            for (size_t& cut : cuts)
                if (cut+1 > indices.size())
                    --cut;
    }

    return indices;
}

csl::Expr DiracSpace::indexToExpr(csl::Index const& spaceTimeIndex,
                             csl::Index const& first,
                             csl::Index const& second) const
{
    if (isGammaChir(spaceTimeIndex))
        return gamma_chir({first, second});
    if (isP_L(spaceTimeIndex))
        return P_L({first, second});
    if (isP_R(spaceTimeIndex))
        return P_R({first, second});
    if (isGammaMu(spaceTimeIndex))
        return gamma({spaceTimeIndex, first, second});
    if (isDelta(spaceTimeIndex))
        return delta({first, second});
    if (isCMatrix(spaceTimeIndex))
        return C_matrix({first, second});

    CallHEPError(mty::error::TypeError,
            "Gamma tensor of index " + toString(spaceTimeIndex) + " not "
            + "recognized in dirac space of dim" + toString(getDim()) + ".");

    return nullptr;
}

bool DiracSpace::isGammaTensor(csl::Expr const& tensor) const
{
    if (not IsIndicialTensor(tensor))
        return false;
    auto parent = tensor->getParent_info();
    return (parent == gamma.get()
            or parent == sigma.get()
            or parent == gamma_chir.get()
            or parent == P_L.get()
            or parent == P_R.get()
            or parent == C_matrix.get());
}

bool DiracSpace::isDelta(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == delta.get();
}

bool DiracSpace::isGammaMu(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == gamma.get();
}

bool DiracSpace::isSigma(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == sigma.get();
}

bool DiracSpace::isGammaChir(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == gamma_chir.get();
}

bool DiracSpace::isP_L(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == P_L.get();
}

bool DiracSpace::isP_R(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == P_R.get();
}

bool DiracSpace::isProjector(csl::Expr const& tensor) const
{
    return isP_L(tensor) or isP_R(tensor);
}

bool DiracSpace::isCMatrix(csl::Expr const& tensor) const
{
    return tensor->getParent_info() == C_matrix.get();
}

bool DiracSpace::isSymmetric(csl::Expr const& tensor) const
{
    return isGammaChir(tensor) 
        or isProjector(tensor) 
        or isDelta(tensor)
        or isCMatrix(tensor);
}

bool DiracSpace::isGammaTensor(csl::Expr_info tensor) const
{
    if (not IsIndicialTensor(tensor))
        return false;
    auto parent = tensor->getParent_info();
    return (parent == gamma.get()
            or parent == sigma.get()
            or parent == gamma_chir.get()
            or parent == P_L.get()
            or parent == P_R.get()
            or parent == C_matrix.get());
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
    return isGammaChir(tensor) 
        or isProjector(tensor) 
        or isDelta(tensor)
        or isCMatrix(tensor);
}

bool DiracSpace::isDelta(csl::Index const& index) const
{
    return index.getType() == cslIndex::Fixed
        or index == dim + 4;
}

bool DiracSpace::isGammaMu(csl::Index const& index) const
{
    return index.getType() != cslIndex::Fixed
        or index.getValue() < dim;
}

bool DiracSpace::isGammaChir(csl::Index const& index) const
{
    return index.getType() == cslIndex::Fixed
        and index == dim + 1;
}

bool DiracSpace::isP_L(csl::Index const& index) const
{
    return index.getType() == cslIndex::Fixed
        and index == dim + 2;
}

bool DiracSpace::isP_R(csl::Index const& index) const
{
    return index.getType() == cslIndex::Fixed
        and index == dim + 3;
}

bool DiracSpace::isProjector(csl::Index const& index) const
{
    return isP_L(index) or isP_R(index);
}

bool DiracSpace::isCMatrix(csl::Index const& index) const
{
    return index.getType() == cslIndex::Fixed
        and index == dim + 5;
}

csl::Index DiracSpace::index() const
{
    return generateIndex();
}

csl::Index DiracSpace::st_index() const
{
    return spaceTime->generateIndex();
}

void DiracSpace::flipChirality(csl::Index& tensor) const
{
    tensor = (isP_L(tensor)) ? dim + 3 : dim + 2;
}

csl::Index DiracSpace::flippedChirality(csl::Index const& tensor) const
{
    auto flipped {tensor};
    flipChirality(flipped);
    return flipped;
}

csl::Index DiracSpace::getSpaceTimeIndex(csl::Expr const& tensor) const
{
    if (isGammaMu(tensor))
        return tensor->getIndexStructureView()[0];
    else if (isGammaChir(tensor))
        return csl::Index(dim+1);
    else if (isP_L(tensor))
        return csl::Index(dim+2);
    else if (isP_R(tensor))
        return csl::Index(dim+3);
    else if (isDelta(tensor))
        return csl::Index(dim+4);
    else if (isCMatrix(tensor))
        return csl::Index(dim+5);
    CallHEPError(mty::error::TypeError,
            "Tensor " + toString(tensor) + " not recognized in Dirac chain.");
    return csl::Index();
}

csl::Index DiracSpace::getFirstIndex(csl::Expr const& tensor) const
{
    const auto& structure = tensor->getIndexStructureView();
    HEPAssert(structure.size() >= 1,
            mty::error::TypeError,
            "Tensor " + toString(tensor) + " have less than one index.");
    return structure[structure.size()-2];
}

csl::Index DiracSpace::getSecondIndex(csl::Expr const& tensor) const
{
    const auto& structure = tensor->getIndexStructureView();
    HEPAssert(structure.size() >= 2,
            mty::error::TypeError,
            "Tensor " + toString(tensor) + " have less than two indices.");
    return structure[structure.size()-1];
}

///////////////////////////////////////////////////
/*************************************************/
// Class ConjugationSimplifier                   //
/*************************************************/
///////////////////////////////////////////////////

bool ConjugationSimplifier::hasContractionProperty(
        csl::Expr_info self, 
        csl::Expr_info other
        ) const
{
    csl::IndexStructure otherStruct = other->getIndexStructure();
    csl::Index selfSecond = self->getIndexStructureView()[1];

    bool secondIndexContracted = false;
    for (const auto& index : otherStruct)
        if (index == selfSecond) {
            secondIndexContracted = true;
            break;
        }
    if (not secondIndexContracted) {
        return false;
    }
    if (not csl::IsSum(other)
            and not csl::IsProd(other)
            and not csl::IsIndicialTensor(other)
            and not csl::IsVectorIntegral(other)) {
        return false;
    }
    if (csl::IsVectorIntegral(other))
        return hasContractionProperty(self, other->getOperand().get());
    return csl::AnyOfLeafs(other, [&](csl::Expr_info sub)
    {
        if (not csl::IsIndicialTensor(sub))
            return true;
        auto parent = sub->getParent_info();
        if (parent->getDim(diracSpace) == 0)
            return true;
        return diracSpace->isGammaTensor(sub)
            or diracSpace->getDelta().get() == parent;
    });
}

csl::Expr ConjugationSimplifier::contraction(
        csl::Expr_info self,
        csl::Expr_info other
        ) const
{
    if (csl::IsITensor(other) 
            and other->getParent_info() == self->getParent_info()) {
        return self->copy() * other->copy();
    }
    csl::Index firstIndex = self->getIndexStructureView()[0];
    csl::Index secondIndex = self->getIndexStructureView()[1];
    csl::Expr commuted = other->copy();
    if (commutation(firstIndex, secondIndex, commuted)) {
        return commuted;
    }
    csl::Expr C = self->copy();
    C->getIndexStructureView()[0] = firstIndex;
    C->getIndexStructureView()[1] = secondIndex;
    return commuted * C;
}

bool ConjugationSimplifier::commutation(
        csl::Index &first,
        csl::Index &second,
        csl::Expr       &expr
        ) const
{
    if (csl::IsSum(expr))
        return commutationWithSum(first, second, expr);
    if (csl::IsProd(expr))
        return commutationWithProduct(first, second, expr);
    if (csl::IsVectorIntegral(expr))
        return commutationWithIntegral(first, second, expr);
    if (csl::IsIndicialTensor(expr)
            and (diracSpace->isGammaTensor(expr)
                or expr->getParent_info() == diracSpace->getDelta().get()))
        return commutationWithTensor(first, second, expr);
    if (csl::IsIndicialTensor(expr)
            and expr->hasContractionProperty(
                diracSpace->C_matrix({first, second}).get())) {
        expr = expr * diracSpace->C_matrix({first, second});
        return true;
    }
    HEPAssert(!csl::IsITensor(expr)
            or expr->getParent_info() != diracSpace->C_matrix.get(),
            mty::error::RuntimeError,
            "Conjugation matrix " + toString(expr->copy())
            + " should not appear in contraction.")
    return false;
}
bool ConjugationSimplifier::commutationWithIntegral(
        csl::Index &first,
        csl::Index &second,
        csl::Expr       &integral
        ) const
{
    HEPAssert(csl::IsVectorIntegral(integral),
            mty::error::RuntimeError,
            "Should not encounter non integral (" 
            + toString(integral->copy()) 
            + ") in conjugation matrix contraction.");

    csl::Expr operand = integral->getOperand();
    if (commutation(first, second, operand)) {
        integral->setOperand(operand);
        return true;
    }
    integral->setOperand(operand);
    return false;
}
bool ConjugationSimplifier::commutationWithTensor(
        csl::Index &first,
        csl::Index &second,
        csl::Expr       &tensor
        ) const
{
    HEPAssert(csl::IsIndicialTensor(tensor),
            mty::error::RuntimeError,
            "Should not encounter non tensor (" 
            + toString(tensor->copy()) 
            + ") in conjugation matrix contraction.");

    HEPAssert(diracSpace->isGammaTensor(tensor)
            or diracSpace->getDelta().get() == tensor->getParent_info(),
            mty::error::RuntimeError,
            "Tensor " + toString(tensor->copy()) + " not recognized in "
            "conjugation matrix contraction.");

    if (tensor->getParent_info() == diracSpace->C_matrix.get()) {
        tensor = diracSpace->C_matrix({first, second}) * tensor;
        return true;
    }

    tensor = Copy(tensor);
    for (auto &index : tensor->getIndexStructureView())
        if (index.getSpace() == diracSpace 
                and index != second) {
            if (index == first) {
                tensor = CSL_0;
                return true;
            }
            auto save_second = second;
            second = index;
            index = first;
            first = save_second;
            if (diracSpace->isSigma(tensor) 
                    or diracSpace->isGammaMu(tensor))
                tensor *= -1;
            return false;
        }
    HEPAssert(false,
            mty::error::RuntimeError,
            "Tensor " + toString(tensor->copy()) + " not recognized in "
            "conjugation matrix contraction.");
    return false;
}
bool ConjugationSimplifier::commutationWithProduct(
        csl::Index &first,
        csl::Index &second,
        csl::Expr       &prod
        ) const
{
    HEPAssert(csl::IsProd(prod),
            mty::error::TypeError,
            "Expecting a product, " + toString(prod) + " given.");
    size_t nArgs = prod->size();
    std::vector<csl::IndexStructure> indices(nArgs);
    for (size_t i = 0; i != nArgs; ++i)
        indices[i] = prod[i]->getIndexStructure();
    size_t nCommut = 0;
    bool commuted;
    do {
        commuted = false;
        for (size_t i = 0; i != nArgs; ++i) {
            for (const auto& index : indices[i])
                if (index == second) {
                    if (commutation(first, second, prod[i])) {
                        csl::Refresh(prod);
                        return true;
                    }
                    ++nCommut;
                    indices[i].clear();
                    commuted = true;
                    break;
                }
        }
    } while (commuted and nCommut < nArgs);

    csl::Refresh(prod);
    return false;
}
bool ConjugationSimplifier::commutationWithSum(
        csl::Index &first,
        csl::Index &second,
        csl::Expr       &sum
        ) const
{
    HEPAssert(csl::IsSum(sum),
            mty::error::TypeError,
            "Expecting a sum, " + toString(sum) + " given.");
    csl::Index first_save = first;
    csl::Index second_save = second;
    int value = -1;
    for (auto &arg : sum) {
        first = first_save;
        second = second_save;
        int valueArg = commutation(first, second, arg);
        if (value == -1)
            value = valueArg;
        else
            HEPAssert(value == valueArg,
                    mty::error::RuntimeError,
                    "Sum " + toString(sum) + " has bad commutation property"
                    " with Dirac Conjugation matrix.");
    }
    csl::Refresh(sum);
    return (value == 1);
}


} // End of namespace mty
