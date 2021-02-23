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

#include "diracBasis.h"
#include "mrtError.h"

namespace mty {

csl::Expr recoverExpr(
        std::vector<csl::Index> const &tensors,
        csl::Index              const &a,
        csl::Index              const &b
        )
{
    std::vector<csl::Expr> prod;
    prod.reserve(tensors.size());
    csl::Index i = a;
    for (const auto &t : tensors) {
        csl::Index j = dirac4.generateIndex();
        prod.push_back(dirac4.indexToExpr(t, i, j));
        i = j;
    }
    csl::Replace(prod.back(), i, b);
    return csl::prod_s(prod);
}

std::vector<csl::Index> ChiralBasis::addElement(
        size_t                   i,
        std::vector<csl::Index> &tensors
        ) const
{
    auto mu = csl::Minkowski.generateIndices(2);
    for (auto &i : mu)
        i.setFree(false);
    switch(i) {
        case 0:
            tensors.push_back(6);
            return {6};
        case 1:
            tensors.push_back(7);
            return {7};
        case 2:
            tensors.push_back(mu[0]);
            tensors.push_back(6);
            return {+mu[0], 7};
        case 3:
            tensors.push_back(mu[0]);
            tensors.push_back(7);
            return {+mu[0], 6};
        case 4:
            tensors.push_back(mu[0]);
            tensors.push_back(mu[1]);
            return {+mu[0], +mu[1]};
        default:
            CallHEPError(mty::error::IndexError, toString(i))
    }
    return {};
}

csl::Expr ChiralBasis::getConjugateExpr(
        size_t                         i,
        std::vector<csl::Index> const &mu,
        csl::Index              const &a, 
        csl::Index              const &b
        ) const 
{
    switch(i) {
        case 0:
        case 1:
        case 2:
        case 3:
            return recoverExpr(mu, a, b);
        case 4:
            return CSL_HALF*dirac4.sigma({mu[0], mu[1], a, b});
        default:
            CallHEPError(mty::error::IndexError, toString(i))
    }
    return CSL_UNDEF;
}

std::vector<csl::Index> StandardBasis::addElement(
        size_t                   i,
        std::vector<csl::Index> &tensors
        ) const
{
    auto mu = csl::Minkowski.generateIndices(2);
    for (auto &i : mu)
        i.setFree(false);
    switch(i) {
        case 0:
            tensors.push_back(8);
            return {8};
        case 1:
            tensors.push_back(5);
            return {5};
        case 2:
            tensors.push_back(mu[0]);
            return {+mu[0]};
        case 3:
            tensors.push_back(mu[0]);
            tensors.push_back(5);
            return {5, +mu[0]};
        case 4:
            tensors.push_back(mu[0]);
            tensors.push_back(mu[1]);
            return {+mu[0], +mu[1]};
        default:
            CallHEPError(mty::error::IndexError, toString(i))
    }
    return {};
}

csl::Expr StandardBasis::getConjugateExpr(
        size_t                         i,
        std::vector<csl::Index> const &mu,
        csl::Index              const &a, 
        csl::Index              const &b
        ) const 
{
    switch(i) {
        case 0:
        case 1:
        case 2:
        case 3:
            return recoverExpr(mu, a, b);
        case 4:
            return dirac4.sigma({mu[0], mu[1], a, b});
        default:
            CallHEPError(mty::error::IndexError, toString(i))
    }
    return CSL_UNDEF;
}

std::vector<csl::Expr> toExprForTrace(std::vector<csl::Index> const &tensors)
{
    std::vector<csl::Expr> exprTensors;
    exprTensors.reserve(tensors.size());
    csl::Index first = dirac4.generateIndex();
    csl::Index i     = first;
    for (const auto &t : tensors) {
        if (t == 8)
            continue;
        csl::Index j = i.rename();
        exprTensors.push_back(dirac4.indexToExpr(t, i, j));
        i = j;
    }
    if (!exprTensors.empty()) {
        csl::Replace(exprTensors.back(), i, first);
    }

    return exprTensors;
}

std::vector<DiracPoint> projectOnStandardBasis(
        std::vector<csl::Index> const &tensors,
        csl::Index              const &a,
        csl::Index              const &b
        )
{
    // std::cout << "For tensors " << std::endl;
    // for (const auto &t : tensors)
    //     std::cout << t << " ";
    // std::cout << std::endl;
    // std::cout << "From " << a << " to " << b << std::endl;

    // Id
    auto traceTensors = tensors;
    auto TS = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gS = recoverExpr({8}, a, b);
    // std::cout << "TS = " << TS << std::endl;

    // Gamma5
    traceTensors = tensors;
    traceTensors.push_back(5);
    auto TP = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gP = recoverExpr({5}, a, b);
    std::cout << "TP = " << TP << std::endl;

    // Gamma mu
    csl::Index mu = csl::Minkowski.generateIndex();
    traceTensors = tensors;
    traceTensors.push_back(mu);
    auto TV = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gV = recoverExpr({+mu}, a, b);
    // std::cout << "TV = " << TV << std::endl;

    // Gamma_mu * Gamma5
    csl::Index nu = csl::Minkowski.generateIndex();
    traceTensors = tensors;
    traceTensors.push_back(nu);
    traceTensors.push_back(5);
    auto TA = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gA = recoverExpr({5, +nu}, a, b);
    // std::cout << "TA = " << TA << std::endl;

    csl::Index rho = csl::Minkowski.generateIndex();
    csl::Index sigma = csl::Minkowski.generateIndex();
    traceTensors = tensors;
    traceTensors.push_back(rho);
    traceTensors.push_back(sigma);
    auto TT = dirac4.calculateTrace(toExprForTrace(traceTensors));
    // std::cout << "TT = " << TT << std::endl;
    auto gT = dirac4.sigma({+rho, +sigma, a, b});
    auto g = csl::Minkowski.getMetric();

    // std::cout << "gS = " << gS << std::endl;
    // std::cout << "gP = " << gP << std::endl;
    // std::cout << "gV = " << gV << std::endl;
    // std::cout << "gA = " << gA << std::endl;
    // std::cout << "gT = " << gT << std::endl;

    return {
        {TS, gS / 4},
        {TP, gP / 4},
        {TV, gV / 4},
        {TA, gA / 4},
        {CSL_I / 8 * (TT - 4*TS*g({rho, sigma})), gT}
    };
}

std::vector<DiracPoint> projectOnChiralBasis(
        std::vector<csl::Index> const &tensors,
        csl::Index              const &a,
        csl::Index              const &b
        )
{
    // L
    auto traceTensors = tensors;
    traceTensors.push_back(6);
    auto TL = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gL = recoverExpr({6}, a, b);

    // R
    traceTensors = tensors;
    traceTensors.push_back(7);
    auto TR = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gR = recoverExpr({7}, a, b);

    // Gamma mu * L
    csl::Index mu = csl::Minkowski.generateIndex();
    traceTensors = tensors;
    traceTensors.push_back(mu);
    traceTensors.push_back(6);
    auto TVL = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gVL = recoverExpr({+mu, 6}, a, b);

    // Gamma mu * R
    traceTensors = tensors;
    traceTensors.push_back(mu);
    traceTensors.push_back(7);
    auto TVR = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gVR = recoverExpr({+mu, 7}, a, b);

    csl::Index rho = csl::Minkowski.generateIndex();
    csl::Index sigma = csl::Minkowski.generateIndex();
    traceTensors = tensors;
    traceTensors.push_back(rho);
    traceTensors.push_back(sigma);
    auto TT = dirac4.calculateTrace(toExprForTrace(traceTensors));
    auto gT = dirac4.sigma({+rho, +sigma, a, b});
    auto g = csl::Minkowski.getMetric();

    return {
        {TL, gL / 2},
        {TR, gR / 2},
        {TVL, gVR / 2},
        {TVR, gVL / 2},
        {CSL_I / 8 * (TT - 4*(TL + TR)*g({rho, sigma})), gT}
    };
}

std::vector<BiDiracPoint> applyFiertz(
        std::vector<csl::Index> const &tLeft,
        std::vector<csl::Index> const &tRight,
        csl::Index              const &aL,
        csl::Index              const &bL,
        csl::Index              const &aR,
        csl::Index              const &bR,
        DiracBasis              const &basis
        )
{
    std::vector<BiDiracPoint> res;
    for (size_t C = 0; C != basis.size(); ++C) {
        for (size_t D = 0; D != basis.size(); ++D) {
            auto tensors = tLeft;
            auto muC = basis.addElement(C, tensors);
            tensors.insert(
                    tensors.end(),
                    tRight.begin(), 
                    tRight.end()
                    );
            auto muD = basis.addElement(D, tensors);
            auto trace = dirac4.calculateTrace(toExprForTrace(tensors));
            if (trace != CSL_0)
                res.push_back({
                        trace / csl::pow_s(basis.getFactor(), 2), 
                        basis.getConjugateExpr(C, muC, aL, bR),
                        basis.getConjugateExpr(D, muD, bL, aR)
                        });
        }
    }
    return res;
}

std::vector<BiDiracPoint> applyFiertzTwice(
        std::vector<csl::Index> const &tLeft,
        std::vector<csl::Index> const &tRight,
        csl::Index              const &aL,
        csl::Index              const &bL,
        csl::Index              const &aR,
        csl::Index              const &bR,
        DiracBasis              const &basis
        )
{
    std::vector<BiDiracPoint> res;
    for (size_t C = 0; C != basis.size(); ++C) {
        for (size_t D = 0; D != basis.size(); ++D) {
            auto tensors = tLeft;
            auto muC = basis.addElement(C, tensors);
            tensors.insert(
                    tensors.end(),
                    tRight.begin(), 
                    tRight.end()
                    );
            auto muD = basis.addElement(D, tensors);
            auto trace = dirac4.calculateTrace(toExprForTrace(tensors));
            if (trace != CSL_0) {
                for (size_t E = 0; E != basis.size(); ++E) {
                    for (size_t F = 0; F != basis.size(); ++F) {
                        auto tensors = muC;
                        auto muE = basis.addElement(E, tensors);
                        tensors.insert(
                                tensors.end(),
                                muD.begin(), 
                                muD.end()
                                );
                        auto muF = basis.addElement(F, tensors);
                        auto trace2 = dirac4.calculateTrace(toExprForTrace(tensors));
                        if (trace2 != CSL_0) {
                            res.push_back({
                                    trace*trace2 / csl::pow_s(basis.getFactor(), 4), 
                                    basis.getConjugateExpr(E, muE, aL, aR),
                                    basis.getConjugateExpr(F, muF, bL, bR)
                                    });
                        }
                    }
                }
            }
        }
    }
    return res;
}

bool GeneralizedIndex::isEmpty() const 
{
    return empty();
}

bool GeneralizedIndex::isZero() const 
{
    const size_t sz = size();
    if (sz < 2)
        return false;
    for (size_t i = 0; i != sz; ++i) 
        for (size_t j = i+1; j < sz; ++j) 
            if (indices[i] == indices[j])
                return true;
    return false;
}

bool MetricTerm::applyOn(GeneralizedIndex &Gamma) const
{
    if (!Gamma.contains(first) && !Gamma.contains(second))
        return false;
    for (auto &i : Gamma) {
        if (i == first) {
            i = second;
            break;
        }
        else if (i == second) {
            i = first;
            break;
        }
    }
    return true;
}

void DiracChain::nulliFy()
{
    factor = CSL_0;
    g.clear();
    eps.clear();
    gamma.clear();
}

void DiracChain::applyMetrics() 
{
    for (size_t i = 0; i != g.size(); ++i) {
        auto predicate = [&](GeneralizedIndex &ind) { return g[i].applyOn(ind); };
        bool gammaSimpli = std::any_of(gamma.begin(), gamma.end(), predicate);
        if (gammaSimpli) {
            g.erase(g.begin() + i);
            --i;
            break;
        }
        bool epsSimpli = std::any_of(eps.begin(), eps.end(), predicate);
        if (epsSimpli) {
            g.erase(g.begin() + i);
            --i;
            break;
        }
    }
    refresh();
}

void DiracChain::refresh()
{
    auto isZero = [](GeneralizedIndex const &i) { return i.isZero(); };
    if (std::any_of(eps.begin(), eps.end(), isZero)
            || std::any_of(gamma.begin(), gamma.end(), isZero)) {
        nulliFy();
        return;
    }
    for (size_t i = 0; i != gamma.size(); ++i) {
        if (gamma[i].empty()) {
            gamma.erase(gamma.begin() + i);
            --i;
        }
    }
}

std::vector<DiracChain> DiracChain::contractInnerIndices() const
{
    return {*this};
}

} // namespace mty
