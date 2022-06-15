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

#include "indexchain.h"
#include "contraction.h"
#include "cslexpr.h"
#include "debuglog.h"
#include "exceptions.h"
#include "fierz.h"
#include "metricindex.h"
#include "sglcommutation.h"
#include "sgloperations.h"
#include "simplify.h"
#include "tensorset.h"
#include "tracer.h"
#include "typecast.h"

namespace sgl {

IndexChain::IndexChain(Field const &psi)
    : a(psi.index), b(psi.index), psiL(field_s(psi)), psiR(field_s(psi))
{
}

IndexChain::IndexChain(csl::Index const &t_a, csl::Index const &t_b)
    : AbstractMultiFunction(gammaindex_s()), a(t_a), b(t_b)
{
}

IndexChain::IndexChain(GExpr const &     mu,
                       csl::Index const &t_a,
                       csl::Index const &t_b)
    : AbstractMultiFunction(mu), a(t_a), b(t_b)
{
}

IndexChain::IndexChain(std::vector<GExpr> const &mu,
                       csl::Index const &        t_a,
                       csl::Index const &        t_b)
    : a(t_a), b(t_b)
{
    m_argument.reserve(mu.size());
    for (const auto &i : mu)
        m_argument.push_back(i);
}

IndexChain::IndexChain(Field const &t_a, csl::Index const &t_b)
    : AbstractMultiFunction(gammaindex_s()),
      a(t_a.index),
      b(t_b),
      psiL(field_s(t_a))
{
}

IndexChain::IndexChain(GExpr const &     mu,
                       Field const &     t_a,
                       csl::Index const &t_b)
    : AbstractMultiFunction(mu), a(t_a.index), b(t_b), psiL(field_s(t_a))
{
}

IndexChain::IndexChain(std::vector<GExpr> const &mu,
                       Field const &             t_a,
                       csl::Index const &        t_b)
    : a(t_a.index), b(t_b), psiL(field_s(t_a))
{
    m_argument.reserve(mu.size());
    for (const auto &i : mu)
        m_argument.push_back(i);
}

IndexChain::IndexChain(csl::Index const &t_a, Field const &t_b)
    : AbstractMultiFunction(gammaindex_s()),
      a(t_a),
      b(t_b.index),
      psiR(field_s(t_b))
{
}

IndexChain::IndexChain(GExpr const &     mu,
                       csl::Index const &t_a,
                       Field const &     t_b)
    : AbstractMultiFunction(mu), a(t_a), b(t_b.index), psiR(field_s(t_b))
{
}

IndexChain::IndexChain(std::vector<GExpr> const &mu,
                       csl::Index const &        t_a,
                       Field const &             t_b)
    : a(t_a), b(t_b.index), psiR(field_s(t_b))
{
    m_argument.reserve(mu.size());
    for (const auto &i : mu)
        m_argument.push_back(i);
}

IndexChain::IndexChain(Field const &t_a, Field const &t_b)
    : AbstractMultiFunction(gammaindex_s()),
      a(t_a.index),
      b(t_b.index),
      psiL(field_s(t_a)),
      psiR(field_s(t_b))
{
}

IndexChain::IndexChain(GExpr const &mu, Field const &t_a, Field const &t_b)
    : AbstractMultiFunction(mu),
      a(t_a.index),
      b(t_b.index),
      psiL(field_s(t_a)),
      psiR(field_s(t_b))
{
}

IndexChain::IndexChain(std::vector<GExpr> const &mu,
                       Field const &             t_a,
                       Field const &             t_b)
    : a(t_a.index), b(t_b.index), psiL(field_s(t_a)), psiR(field_s(t_b))
{
    m_argument.reserve(mu.size());
    for (const auto &i : mu)
        m_argument.push_back(i);
}

std::optional<GExpr> IndexChain::checkGammaAndConjugation() const
{
    for (const auto &arg : m_argument)
        if (!IsType<GammaIndex>(arg)
            && (!IsType<CSLExpr>(arg) || arg->expr() != CSL_0)) {
            std::cerr << copy() << std::endl;
            std::cerr << arg << std::endl;
            throw Exception::MathError;
        }
    if (m_argument.empty())
        return std::nullopt;
    if (psiL && ConvertTo<GammaIndex>(m_argument.front())->isC()) {
        csl::Expr  factor = psiL->isComplexConjugated() ? CSL_1 : CSL_M_1;
        IndexChain cpy(*this);
        cpy.psiL->conjugate();
        cpy.m_argument.erase(cpy.m_argument.begin());
        return cslexpr_s(factor) * cpy.copy();
    }
    if (psiR && ConvertTo<GammaIndex>(m_argument.back())->isC()) {
        csl::Expr  factor = !psiR->isComplexConjugated() ? CSL_1 : CSL_M_1;
        IndexChain cpy(*this);
        cpy.psiR->conjugate();
        cpy.m_argument.erase(cpy.m_argument.end() - 1);
        return cslexpr_s(factor) * cpy.copy();
    }
    return std::nullopt;
}

bool IndexChain::isZero() const
{
    return std::any_of(m_argument.begin(),
                       m_argument.end(),
                       [](GExpr const &arg) { return arg->isZero(); });
}

csl::Expr IndexChain::getFactor() const
{
    csl::Expr f = CSL_1;
    for (const auto &arg : m_argument) {
        if (auto fi = arg->getFactor(); fi != CSL_1)
            f *= fi;
    }
    return f;
}

GExpr IndexChain::getTerm() const
{
    auto cpy = copy();
    for (auto &arg : cpy)
        arg = arg->getTerm();
    return cpy;
}

bool IndexChain::isHappy() const
{
    return !psiL || !psiR || a == b || psiL->isHappyWith(*psiR);
}

bool IndexChain::isReversed() const
{
    return psiL && psiR && isHappy() && psiR->isLeftField();
}

bool IndexChain::hasCommonIndex(IndexChain const &other) const
{
    if (a == other.b || b == other.a)
        return true;
    auto isConjugated = [](IndexChain const &chain, bool left) {
        return !chain.m_argument.empty()
               && isC(left ? chain.m_argument.front()
                           : chain.m_argument.back());
    };
    // Checking that one of the two chains (exactly) is conjugated
    if (a == other.a
        && (isConjugated(*this, true) ^ isConjugated(other, true))) {
        return true;
    }
    if (b == other.b
        && (isConjugated(*this, false) ^ isConjugated(other, false))) {
        return true;
    }
    return false;
}

GExpr IndexChain::mergeChain(IndexChain const &other) const
{
    SCOPELOG
    LOG("Merging chains", copy(), "and", other.copy())
    if (b == other.a) {
        std::vector<GExpr> newArgs(m_argument);
        newArgs.insert(
            newArgs.end(), other.m_argument.begin(), other.m_argument.end());
        for (size_t i = 0; i != newArgs.size(); ++i) {
            if (newArgs[i]->indices().size() == 0) {
                newArgs.erase(newArgs.begin() + i);
                --i;
            }
        }
        if (psiL) {
            if (other.psiR)
                return indexchain_s(newArgs, *psiL, *other.psiR);
            return indexchain_s(newArgs, *psiL, other.b);
        }
        else {
            if (other.psiR)
                return indexchain_s(newArgs, a, *other.psiR);
            return indexchain_s(newArgs, a, other.b);
        }
    }
    else if (a == other.b) {
        return other.mergeChain(*this);
    }
    return mergeConjugateChain(other);
}

GExpr IndexChain::mergeConjugateChain(IndexChain const &other) const
{
    SCOPELOG
    LOG("Merging conjugated chains", copy(), "and", other.copy())
    if (a == other.a) {
        // Checking that one conjugation matrix allows the connection
        if (isC(m_argument.front())) {
            IndexChain cpy{*this};
            cpy.m_argument.erase(cpy.m_argument.begin());
            auto [sign, chain] = cpy.conjugated();
            chain.m_argument.insert(chain.m_argument.begin(), CMatrix());
            sign *= -1;
            return cslexpr_s(sign) * chain.mergeChain(other);
        }
        else if (isC(other.m_argument.front())) {
            return other.mergeConjugateChain(*this);
        }
    }
    if (b == other.b) {
        // Checking that one conjugation matrix allows the connection
        if (!isC(m_argument.back())) {
            if (!isC(other.m_argument.back()))
                throw sgl::Exception::MathError;
            return other.mergeConjugateChain(*this);
        }
        IndexChain cpy{*this};
        cpy.m_argument.erase(cpy.m_argument.end() - 1);
        auto [sign, chain] = cpy.conjugated();
        chain.m_argument.insert(chain.m_argument.end(), CMatrix());
        sign *= -1;
        return cslexpr_s(sign) * chain.mergeChain(other);
    }
    throw sgl::Exception::MathError;
}

bool IndexChain::hasPropertyWith(GExpr const &other) const
{
    if (isTrace())
        return false;
    if (!IsType<IndexChain>(other) || ConvertTo<IndexChain>(other)->isTrace())
        return false;
    auto otherChain = ConvertTo<IndexChain>(other);
    if (hasCommonIndex(*otherChain))
        return true;
    return false;
    // return wouldBeHappierWith(*otherChain);
}

GExpr IndexChain::propertyWith(GExpr const &other) const
{
    auto otherChain = ConvertTo<IndexChain>(other);
    if (hasCommonIndex(*otherChain)) {
        auto res = mergeChain(*ConvertTo<IndexChain>(other));
        LOG("Merge result :", res)
        return res;
    }
    throw sgl::Exception::MathError;
    // // Fierz to mix chains
    // SCOPELOG
    // LOG("Applying fermion order", copy(), other)
    // auto res = applyGeneralFierz(*otherChain);
    // LOG("RES :", res)
    // return res;
}

bool IndexChain::contains(csl::Index const &index) const
{
    for (const auto &arg : m_argument)
        if (arg->contains(index))
            return true;
    return false;
}

void IndexChain::replace(csl::Index const &i, csl::Index const &j)
{
    for (auto &arg : m_argument) {
        if (arg->contains(i)) {
            arg = arg->copy();
            arg->replace(i, j);
            return;
        }
    }
}

GExpr IndexChain::copy() const
{
    return std::make_shared<IndexChain>(*this);
}

GExpr IndexChain::refresh() const
{
    GExpr res;
    if (psiL) {
        if (psiR)
            res = indexchain_s(m_argument, *psiL, *psiR);
        else
            res = indexchain_s(m_argument, *psiL, b);
    }
    else {
        if (psiR)
            res = indexchain_s(m_argument, a, *psiR);
        else
            res = indexchain_s(m_argument, a, b);
    }
    return res;
}

csl::Expr IndexChain::toCSL(TensorSet const &tensors) const
{
    SCOPELOG
    IndexChain cpy{*this};
    LOG("SGL to CSL for IndexChain", copy())
    csl::Expr fieldExpr = CSL_1;
    if (psiL) {
        auto newA = a.rename();
        fieldExpr *= psiL->toCSL(tensors, newA);
        cpy.a = newA;
    }
    if (psiR) {
        auto newB = b.rename();
        fieldExpr *= psiR->toCSL(tensors, newB);
        cpy.b = newB;
    }
    if (m_argument.empty()) {
        LOG("Res :",
            fieldExpr
                * csl::Tensor(tensors.gamma.begin()->second)({cpy.a, cpy.b}))
        return fieldExpr
               * csl::Tensor(tensors.gamma.begin()->second)({cpy.a, cpy.b});
    }
    std::vector<csl::Expr> terms(m_argument.size());
    csl::Index             i = cpy.a;
    csl::Index             j;
    for (size_t k = 0; k != m_argument.size(); ++k) {
        if (k == m_argument.size() - 1)
            j = cpy.b;
        else
            j = i.rename();
        auto gamma = ConvertTo<GammaIndex>(m_argument[k]);
        if (!gamma)
            throw Exception::TypeError;
        terms[k] = gamma->buildTensor(tensors, i, j);
        i        = j;
    }
    const auto res = fieldExpr * csl::prod_s(terms);
    LOG("Res :", res);
    return res;
}

std::pair<GExpr, IndexChain> IndexChain::cut(size_t pos, size_t len) const
{
    SCOPELOG
    LOG("Cutting at pos", pos, "(len =", len, ")in", copy())
    if (pos >= m_argument.size() || pos + len > m_argument.size()) {
        if (pos != 0 || len != 0)
            throw Exception::IndexError;
    }
    std::vector<GExpr> leftArgs(m_argument.begin(), m_argument.begin() + pos);
    std::vector<GExpr> rightArgs(m_argument.begin() + pos + len,
                                 m_argument.end());
    csl::Index         aL = diracSpace->generateIndex();
    csl::Index         bR = diracSpace->generateIndex();
    GExpr              left;
    if (psiL)
        left = indexchain_s(leftArgs, *psiL, aL);
    else
        left = indexchain_s(leftArgs, a, aL);
    GExpr right;
    if (psiR)
        right = indexchain_s(rightArgs, bR, *psiR);
    else
        right = indexchain_s(rightArgs, bR, b);
    LOG("Left part :", left)
    LOG("Right part :", right)
    LOG("Index chain :", indexchain_s(aL, bR))
    return {left * right, IndexChain(aL, bR)};
}

void IndexChain::erase(size_t pos, size_t len)
{
    if (pos >= m_argument.size() || pos + len > m_argument.size())
        throw Exception::IndexError;
    m_argument.erase(m_argument.begin() + pos, m_argument.begin() + pos + len);
}

bool IndexChain::hasContraction(size_t i, size_t j) const
{
    for (const auto &index : m_argument[i]->indices()) {
        if (index.getType() != cslIndex::Fixed
            && m_argument[j]->contains(index)) {
            return true;
        }
    }
    if (j == i + 1 && isChiral(m_argument[i]) && isChiral(m_argument[j])) {
        return true;
    }
    if (j == i + 1 && isC(m_argument[i]) && isC(m_argument[j]))
        return true;
    return false;
}

std::optional<std::pair<size_t, size_t>> IndexChain::firstMove() const
{
    auto isChiral = [&](GExpr const &expr) {
        auto const *ptr = dynamic_cast<GammaIndex const *>(expr.get());
        return ptr->isGamma5() || ptr->isP_L() || ptr->isP_R();
    };
    auto isNotChiral = [&](GExpr const &expr) { return !isChiral(expr); };

    auto first = std::find_if(m_argument.begin(), m_argument.end(), isChiral);
    auto nonChiral   = std::find_if(first, m_argument.end(), isNotChiral);
    auto conjugation = std::find_if(m_argument.begin(), m_argument.end(), isC);
    if (first != m_argument.end() && nonChiral != m_argument.end()
        && first != m_argument.end() - 1 && conjugation == m_argument.end()) {
        return std::make_pair(std::distance(m_argument.begin(), first),
                              size() - 1);
    }
    for (size_t i = 0; i != m_argument.size(); ++i) {
        for (size_t j = i + 2; j < m_argument.size(); ++j) {
            if (hasContraction(i, j))
                return std::make_pair(i, j - 1);
        }
    }
    return std::nullopt;
}

std::optional<std::pair<size_t, size_t>> IndexChain::lastMove() const
{
    for (size_t i = 0; i + 1 < m_argument.size(); ++i) {
        const size_t j = i + 1;
        if (!IsType<GammaIndex>(m_argument[i])
            || !IsType<GammaIndex>(m_argument[j]))
            continue;
        if (ConvertTo<GammaIndex>(m_argument[j])
                ->leftOf(*ConvertTo<GammaIndex>(m_argument[i]))) {
            return std::make_pair(i, j);
        }
    }
    return std::nullopt;
}

GExpr IndexChain::contraction(size_t i) const
{
    SCOPELOG
    LOG("Contraction of", m_argument[i], "and", m_argument[i + 1])
    auto [expr, chain]  = cut(i, 2);
    auto contractionRes = sgl::contraction(
        *dynamic_cast<GammaIndex const *>(m_argument[i].get()),
        *dynamic_cast<GammaIndex const *>(m_argument[i + 1].get()),
        chain.getBorderIndices().first,
        chain.getBorderIndices().second);
    LOG("Res :", expr, "*", contractionRes)
    return expr * contractionRes;
}

GExpr IndexChain::moveIndex(size_t init, size_t target) const
{
    SCOPELOG
    LOG("Move of index", init, "to", target, "in", copy())
    IndexChain current{*this};
    LOG("Current:", current.copy())
    GExpr              totalFactor = cslexpr_s(CSL_1);
    std::vector<GExpr> terms;
    terms.reserve(1 + ((target > init) ? target - init : init - target));
    int di = target > init ? 1 : -1;
    while (init != target) {
        auto [factor, chain] = current.cut((di == 1) ? init : init - 1, 2);
        auto first           = *dynamic_cast<GammaIndex const *>(
            current.m_argument[init].get());
        auto second = *dynamic_cast<GammaIndex const *>(
            current.m_argument[init + di].get());
        Commutation res = commute((di == 1) ? first : second,
                                  (di == 1) ? second : first,
                                  chain.getBorderIndices().first,
                                  chain.getBorderIndices().second);
        if (di == -1)
            std::swap(res.left, res.right);
        current.argument(init)      = res.left.copy();
        current.argument(init + di) = res.right.copy();
        if (!IsType<CSLExpr>(res.remnant) || res.remnant->expr() != CSL_0) {
            LOG("New term :", factor * res.remnant)
            terms.push_back(totalFactor * factor * res.remnant);
        }
        totalFactor *= res.factor;
        init += di;
    }
    terms.push_back(totalFactor * current.refresh());
    LOG("New term :", terms.back())
    LOG("Res :", sgl::sum_s(terms))
    return sgl::sum_s(terms);
}

std::optional<GExpr> IndexChain::reduceStep() const
{
    for (size_t i = 0; i + 1 < m_argument.size(); ++i) {
        if (hasContraction(i, i + 1)) {
            return contraction(i);
        }
    }
    auto optmove = firstMove();
    if (optmove) {
        return moveIndex(optmove->first, optmove->second);
    }
    if (m_argument.size() > 1 && isC(m_argument.front())
        && isC(m_argument.back())) {
        IndexChain cpy{*this};
        cpy.m_argument.erase(cpy.m_argument.begin());
        cpy.m_argument.erase(cpy.m_argument.end() - 1);
        auto [sign, res] = cpy.conjugated();
        return cslexpr_s(-sign) * res.copy();
    }
    // Obsolete: replaced by OrderChains in simplify.cpp
    // if (applyMatrixOrdering && !isTrace()) {
    //     optmove = lastMove();
    //     if (optmove) {
    //         return moveIndex(optmove->first, optmove->second);
    //     }
    // }
    return std::nullopt;
}

std::optional<GExpr> IndexChain::reduceMomenta(MomentumIndex const &p1,
                                               MomentumIndex const &p2) const
{
    auto const &mu  = p1.indices().front();
    auto const &nu  = p2.indices().front();
    auto        rho = mu.rename();
    auto        p   = p1.getP();
    if (!contains(mu) || !contains(nu) || p1.getP().get() != p2.getP().get())
        return std::nullopt;
    constexpr size_t npos = -1;
    size_t           i    = npos;
    size_t           j    = npos;
    for (size_t k = 0; k != m_argument.size(); ++k) {
        if (m_argument[k]->contains(mu) || m_argument[k]->contains(nu)) {
            (i == npos ? i : j) = k;
        }
    }
    if (i == npos) // Should not happen
        return std::nullopt;
    if (j == npos) // here two p's are contracted with a fully asym gamma.
        return cslexpr_s(CSL_0);
    if (j - i > 1) {
        return std::nullopt;
        // For now only contract consecutive momenta as they should be ordered
        // The following line will commute gamma matrices if needed
        // return p1.copy()*p2.copy()*moveIndex(i, j - 1);
    }
    return momentumGammaContraction(*this, p, mu, nu, i, j);
}

GExpr IndexChain::applyEOM(MomentumIndex const &p) const
{
    SCOPELOG
    LOG("Applying EOM for", p.copy())
    LOG("Chain :", copy())
    bool left = psiL && psiL->p == p.getP() && psiL->isOnShell();
    LOG((left ? "Left EOM" : "Right EOM"))
    bool      hasSign = (left) ? !psiL->particle : !psiR->particle;
    csl::Expr sign    = (hasSign) ? CSL_M_1 : CSL_1;
    for (size_t i = 0; i != m_argument.size(); ++i) {
        if (m_argument[i]->contains(p.indices()[0])) {
            size_t target = left ? 0 : m_argument.size() - 1;
            if (i == target) {
                if (m_argument[i]->indices().size() == 1) {
                    LOG("Simple Gamma EOM")
                    IndexChain res{*this};
                    res.m_argument.erase(res.m_argument.begin() + i);
                    LOG("Res :", sign * p.mass() * res.copy())
                    return sign * cslexpr_s(p.mass()) * res.copy();
                }
                else {
                    // Sigma _mu_nu
                    LOG("Sigma EOM")
                    bool firstIndex
                        = p.indices()[0] == m_argument[i]->indices()[0];
                    if (left != firstIndex)
                        sign *= -1;
                    // No need to commute
                    IndexChain res1{*this};
                    res1.m_argument[i] = m_argument[i]->copy();
                    res1.m_argument[i]->indices().erase(
                        res1.m_argument[i]->indices().begin() + !firstIndex);
                    IndexChain res2{*this};
                    auto       p_expr = momentumindex_s(
                        p.getP(), m_argument[i]->indices()[firstIndex]);
                    res2.m_argument.erase(res2.m_argument.begin() + i);
                    LOG("Term 1 :", p_expr, res2.copy())
                    LOG("Term 2 :", cslexpr_s(p.mass()), res1.copy())
                    LOG("Total res :",
                        sign * p_expr * res2.copy()
                            + sign * p.mass() * res1.copy())
                    return sign * p_expr * res2.copy()
                           + sign * cslexpr_s(p.mass()) * res1.copy();
                }
            }
            LOG("Movements to apply EOM")
            return p.copy() * moveIndex(i, target);
        }
    }
    throw Exception::MathError;
}

GExpr IndexChain::simplify()
{
    if (!isTrace())
        return AbstractGammaSym::simplify();
    return calculateTrace();
}

GExpr IndexChain::calculateTrace() const
{
    if (m_argument.empty())
        return cslexpr_s(diracSpace->getDim());
    if (isGamma5(m_argument.back())) {
        return calculateChiralTrace();
    }
    if (isP_L(m_argument.back())) {
        return (calculateStandardTrace() - calculateChiralTrace()) / 2;
    }
    if (isP_R(m_argument.back())) {
        return (calculateStandardTrace() + calculateChiralTrace()) / 2;
    }
    return calculateStandardTrace();
}

GExpr IndexChain::calculateStandardTrace() const
{
    std::vector<GammaIndex> indices;
    SCOPELOG
    LOG("Initialization standard Trace for ", copy())
    indices.reserve(m_argument.size());
    for (const auto &arg : m_argument) {
        auto gam = ConvertTo<GammaIndex>(arg);
        if (!gam) {
            errorPrint();
            LOG("Bad argument :", gam->copy())
            std::cerr << copy() << '\n';
            throw Exception::TypeError;
        }
        if (gam->isGammaMu() || gam->isSigma()) {
            LOG("Argument :", gam->copy())
            indices.push_back(*gam);
        }
        else if (indices.size() != m_argument.size() - 1) {
            std::cerr << copy() << '\n';
            throw Exception::MathError;
        }
    }

    return sgl::CSLSimplified(
        tracer::standardTrace(indices, diracSpace->getDim()));
}

GExpr IndexChain::calculateChiralTrace() const
{
    std::vector<GammaIndex> indices;
    LOG("Initialization chiral Trace for ", copy())
    indices.reserve(m_argument.size());
    for (const auto &arg : m_argument) {
        auto gam = ConvertTo<GammaIndex>(arg);
        if (!gam) {
            errorPrint();
            LOG("Bad argument :", gam->copy())
            throw Exception::TypeError;
        }
        if (gam->isGammaMu() || gam->isSigma()) {
            LOG("Argument :", gam->copy())
            indices.push_back(*gam);
        }
        else if (indices.size() != m_argument.size() - 1) {
            errorPrint();
            throw Exception::MathError;
        }
    }

    return sgl::CSLSimplified(
        tracer::chiralTrace(indices, diracSpace->getDim()));
}

GExpr IndexChain::trace(IndexChain A, IndexChain C, IndexChain B, IndexChain D)
{
    SCOPELOG
    LOG("Trace ACBD")
    LOG(A.copy())
    LOG(B.copy())
    LOG(C.copy())
    LOG(D.copy())
    C.a      = A.b; // ..A*C..
    B.a      = C.b; // ..C*B..
    D.a      = B.b; // ..B*D..  ==> Tr(ACBD)
    A.a      = D.b; // ..D*A..
    auto res = Simplified(A.copy() * B.copy() * C.copy() * D.copy());
    LOG("Res : ", res)
    return CSLSimplified(res);
}

std::tuple<GExpr, IndexChain, IndexChain> chiralBasisElement(size_t i)
{
    auto e  = diracSpace->generateIndices(4);
    auto mu = minkoSpace->generateIndex();
    auto nu = minkoSpace->generateIndex();
    switch (i) {
    case 0:
        return {cslexpr_s(1),
                IndexChain(IndexChain::P_L(), e[0], e[1]),
                IndexChain(IndexChain::P_L(), e[2], e[3])};
    case 1:
        return {cslexpr_s(1),
                IndexChain(IndexChain::P_R(), e[0], e[1]),
                IndexChain(IndexChain::P_R(), e[2], e[3])};
    case 2:
        return {
            cslexpr_s(1),
            IndexChain({gammaindex_s(mu), IndexChain::P_L()}, e[0], e[1]),
            IndexChain({gammaindex_s(+mu), IndexChain::P_R()}, e[2], e[3])};
    case 3:
        return {
            cslexpr_s(1),
            IndexChain({gammaindex_s(mu), IndexChain::P_R()}, e[0], e[1]),
            IndexChain({gammaindex_s(+mu), IndexChain::P_L()}, e[2], e[3])};
    case 4:
        return {cslexpr_s(-CSL_HALF),
                IndexChain(gammaindex_s({mu, nu}), e[0], e[1]),
                IndexChain(gammaindex_s({+mu, +nu}), e[2], e[3])};
    default:
        throw Exception::IndexError;
    }
}

std::tuple<GExpr, IndexChain, IndexChain> standardBasisElement(size_t i)
{
    auto e  = diracSpace->generateIndices(4);
    auto mu = minkoSpace->generateIndex();
    auto nu = minkoSpace->generateIndex();
    switch (i) {
    case 0:
        return {cslexpr_s(1), IndexChain(e[0], e[1]), IndexChain(e[2], e[3])};
    case 1:
        return {cslexpr_s(1),
                IndexChain(IndexChain::gamma5(), e[0], e[1]),
                IndexChain(IndexChain::gamma5(), e[2], e[3])};
    case 2:
        return {cslexpr_s(1),
                IndexChain(gammaindex_s(mu), e[0], e[1]),
                IndexChain(gammaindex_s(+mu), e[2], e[3])};
    case 3:
        return {
            cslexpr_s(1),
            IndexChain({gammaindex_s(mu), IndexChain::gamma5()}, e[0], e[1]),
            IndexChain({IndexChain::gamma5(), gammaindex_s(+mu)}, e[2], e[3])};
    case 4:
        return {cslexpr_s(-1),
                IndexChain(gammaindex_s({mu, nu}), e[0], e[1]),
                IndexChain(gammaindex_s({+mu, +nu}), e[2], e[3])};
    default:
        throw Exception::IndexError;
    }
}

std::tuple<GExpr, IndexChain, IndexChain> basisElement(size_t i, bool chiral)
{
    return (chiral) ? chiralBasisElement(i) : standardBasisElement(i);
}

GExpr IndexChain::applyGeneralFierz(IndexChain const &other, bool chiral) const
{
    if (psiL->isHappyWith(*other.psiL) || psiR->isHappyWith(*other.psiR)) {
        auto [sign, conj] = other.conjugated();
        return cslexpr_s(sign) * applyGeneralFierz(conj, chiral);
    }
    SCOPELOG
    LOG("General Fierz")
    LOG(copy())
    LOG(other.copy())
    int factor      = (chiral) ? 4 : 16;
    auto [expr1, A] = cut(0, size());
    auto [expr2, B] = other.cut(0, other.size());
    A.m_argument    = m_argument;
    B.m_argument    = other.m_argument;
    auto aa         = A.a;
    auto ab         = A.b;
    auto ac         = B.a;
    auto ad         = B.b;
    A.a             = A.a.rename();
    A.b             = A.b.rename();
    B.a             = B.a.rename();
    B.b             = B.b.rename();
    std::vector<GExpr> res;
    res.reserve(5 * 5);
    for (int c = 0; c != 5; ++c) {
        auto [fc, C, C_dual] = basisElement(c, chiral);
        C_dual.a             = ac;
        C_dual.b             = ab;
        for (int d = 0; d != 5; ++d) {
            auto [fd, D, D_dual] = basisElement(d, chiral);
            D_dual.a             = aa;
            D_dual.b             = ad;
            auto tr              = trace(A, C, B, D);
            if (!tr->isZero()) {
                res.push_back(fc * fd / factor * tr * expr1 * C_dual.copy()
                              * expr2 * D_dual.copy());
                sgl::fixFierz(res.back());
                LOG("New term :", res.back())
            }
        }
    }
    return sgl::sum_s(res);
}

GExpr IndexChain::applyGeneralFierzTwice(IndexChain const &other,
                                         bool              chiral) const
{
    SCOPELOG
    LOG("General Fierz")
    LOG(copy())
    LOG(other.copy())
    int factor      = (chiral) ? 16 : 256;
    auto [expr1, A] = cut(0, size());
    auto [expr2, B] = other.cut(0, other.size());
    A.m_argument    = m_argument;
    B.m_argument    = other.m_argument;
    auto aa         = A.a;
    auto ab         = A.b;
    auto ac         = B.a;
    auto ad         = B.b;
    A.a             = A.a.rename();
    A.b             = A.b.rename();
    B.a             = B.a.rename();
    B.b             = B.b.rename();
    std::vector<GExpr> res;
    res.reserve(5 * 5);
    for (int e = 0; e != 5; ++e) {
        auto [fe, E, E_dual] = basisElement(e, chiral);
        E_dual.a             = ac;
        E_dual.b             = ad;
        for (int f = 0; f != 5; ++f) {
            auto [ff, F, F_dual] = basisElement(f, chiral);
            F_dual.a             = aa;
            F_dual.b             = ab;
            std::vector<GExpr> factors;
            factors.reserve(5 * 5);
            for (int c = 0; c != 5; ++c) {
                auto [fc, C, C_dual] = basisElement(c, chiral);
                for (int d = 0; d != 5; ++d) {
                    auto [fd, D, D_dual] = basisElement(d, chiral);
                    auto TA              = trace(A, C, B, D);
                    if (TA->isZero())
                        continue;
                    auto TB = trace(C_dual, E, D_dual, F);
                    if (TB->isZero())
                        continue;
                    factors.push_back(sgl::prod_s({fc, fd, fe, ff, TA, TB}));
                    CSLSimplify(factors.back());
                }
            }
            if (!factors.empty()) {
                GExpr totalFactor = sgl::sum_s(factors);
                CSLSimplify(totalFactor);
                if (totalFactor->isZero())
                    continue;
                res.push_back(totalFactor / factor * expr1 * expr2
                              * E_dual.copy() * F_dual.copy());
            }
        }
    }
    return sgl::sum_s(res);
}

void IndexChain::print(std::ostream &out) const
{
    if (isTrace())
        out << "Tr";
    out << '{';
    printArgs(out, ' ');
    out << '}';
    if (!isTrace()) {
        out << "_{";
        if (psiL)
            psiL->print(out);
        else
            out << a;
        out << ",";
        if (psiR)
            psiR->print(out);
        else
            out << b;
        out << "}";
    }
}

GExpr IndexChain::identityFactor(GammaIndex const &gamma) const
{
    if (!gamma.isChiral() || gamma.isGamma5())
        return cslexpr_s(CSL_0);
    return cslexpr_s(CSL_HALF);
}

GExpr IndexChain::factor(GammaIndex const &A, GammaIndex const &B) const
{
    if (A.indices() == B.indices())
        return cslexpr_s(CSL_1);
    // Here A and B are chiral and different
    if (A.isP_L() && B.isP_R())
        return cslexpr_s(CSL_0);
    if (A.isP_R() && B.isP_L())
        return cslexpr_s(CSL_0);
    if (A.isGamma5()) {
        return cslexpr_s(B.isP_R() ? CSL_1 : CSL_M_1);
    }
    else {
        // Here B is gamma5 and A is a projector
        return cslexpr_s(A.isP_R() ? CSL_HALF : CSL_M_HALF);
    }
}

GExpr IndexChain::getContributionTo(IndexChain const &other) const
{
    if (!psiL or !psiR or !other.psiL or !other.psiR)
        return cslexpr_s(CSL_0);
    if (!psiL->isSame(*other.psiL) or !psiR->isSame(*other.psiR))
        return cslexpr_s(CSL_0);
    auto       iterA = begin();
    auto       iterB = other.begin();
    const auto endA  = end();
    const auto endB  = other.end();
    if (std::abs((endA - iterA) - (endB - iterB)) > 1)
        return cslexpr_s(CSL_0);
    while (iterA != endA && iterB != endB) {
        const bool isAChiral = isChiral(*iterA);
        const bool isBChiral = isChiral(*iterB);
        if (isAChiral || isBChiral)
            break;
        auto const &gA = ConvertTo<GammaIndex>(*iterA);
        auto const &gB = ConvertTo<GammaIndex>(*iterB);
        if (!gA->equalTo(*gB)) {
            return cslexpr_s(CSL_0);
        }
        ++iterA, ++iterB;
    }
    const bool isAEnd = (iterA == endA);
    const bool isBEnd = (iterB == endB);
    if (isAEnd && isBEnd)
        return cslexpr_s(CSL_1);
    if (isAEnd ^ isBEnd) {
        return identityFactor(
            *ConvertTo<GammaIndex>(isAEnd ? *iterB : *iterA));
    }
    return factor(*ConvertTo<GammaIndex>(*iterA),
                  *ConvertTo<GammaIndex>(*iterB));
}

std::pair<int, IndexChain> IndexChain::conjugated() const
{
    SCOPELOG
    LOG("Conjugate ", copy())
    IndexChain newChain{*this};
    newChain.psiL = (psiL) ? std::make_shared<Field>(*psiL) : nullptr;
    newChain.psiR = (psiR) ? std::make_shared<Field>(*psiR) : nullptr;
    int sign      = 1;
    if (newChain.psiL) {
        // If conjugated field there is a sign
        // bar{u} G C^T = bar{u} C^T G'^T = -G' v
        if (newChain.psiL->isComplexConjugated()) {
            sign *= -1;
        }
        newChain.psiL->conjugate();
    }
    if (newChain.psiR) {
        // If conjugated field there is a sign
        // C G bar{u} = G'^T C bar{u} = -v G'
        if (newChain.psiR->isComplexConjugated()) {
            sign *= -1;
        }
        newChain.psiR->conjugate();
    }
    std::reverse(newChain.begin(), newChain.end());
    std::swap(newChain.a, newChain.b);
    std::swap(newChain.psiL, newChain.psiR);
    std::for_each(begin(), end(), [&](GExpr const &gamma) {
        GammaIndex const *ptr = dynamic_cast<GammaIndex const *>(gamma.get());
        if (ptr) {
            if (ptr->isGammaMu() || ptr->isSigma() || ptr->isC())
                sign *= -1;
        }
    });
    LOG("RES :", sign, newChain.copy())
    return {sign, newChain};
}

GExpr indexchain_s(int a, int b)
{
    return indexchain_s(IndexChain::easyIndex(a), IndexChain::easyIndex(b));
}

GExpr indexchain_s(const char mu[], int a, int b)
{
    csl::Index index(mu, minkoSpace);
    index.setID(0);
    return indexchain_s(gammaindex_s(index),
                        IndexChain::easyIndex(a),
                        IndexChain::easyIndex(b));
}

GExpr indexchain_s(std::vector<std::string> const &mu, int a, int b)
{
    std::vector<csl::Index> indices;
    indices.reserve(mu.size());
    for (const auto &m : mu) {
        indices.push_back(csl::Index(m, minkoSpace));
        indices.back().setID(0);
    }
    return indexchain_s(gammaindex_s(indices),
                        IndexChain::easyIndex(a),
                        IndexChain::easyIndex(b));
}

GExpr IndexChain::CMatrix()
{
    return gammaindex_s(8);
}

GExpr IndexChain::Id()
{
    return gammaindex_s();
}

GExpr IndexChain::gamma5()
{
    return gammaindex_s(5);
}

GExpr IndexChain::P_L()
{
    return gammaindex_s(6);
}

GExpr IndexChain::P_R()
{
    return gammaindex_s(7);
}

} // namespace sgl
