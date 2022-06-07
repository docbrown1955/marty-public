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

/**
 * @file indexchain.h
 * @brief sgl::IndexChain, main symbolic object of SGL representing a chain of
 * gamma-matrices objects.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include <string>
#include <map>
#include "gammaindex.h"
#include "momentumindex.h"
#include "sglfield.h"
#include "cslexpr.h"

namespace sgl {

    class IndexChain: public AbstractMultiFunction {

    public:

        // static inline bool applyMatrixOrdering = false;

        IndexChain() = default;
        IndexChain(IndexChain const &) = default;

        IndexChain(Field const &psi);

        IndexChain(
                csl::Index const &t_a,
                csl::Index const &t_b
                );

        IndexChain(
                GExpr      const &mu,
                csl::Index const &t_a,
                csl::Index const &t_b
                );

        IndexChain(
                std::vector<GExpr> const &mu,
                csl::Index         const &t_a,
                csl::Index         const &t_b
                );

        IndexChain(
                Field      const &t_a,
                csl::Index const &t_b
                );

        IndexChain(
                GExpr      const &mu,
                Field      const &t_a,
                csl::Index const &t_b
                );

        IndexChain(
                std::vector<GExpr> const &mu,
                Field              const &t_a,
                csl::Index         const &t_b
                );

        IndexChain(
                csl::Index const &t_a,
                Field      const &t_b
                );

        IndexChain(
                GExpr      const &mu,
                csl::Index const &t_a,
                Field      const &t_b
                );

        IndexChain(
                std::vector<GExpr> const &mu,
                csl::Index         const &t_a,
                Field              const &t_b
                );

        IndexChain(
                Field      const &t_a,
                Field      const &t_b
                );

        IndexChain(
                GExpr const &mu,
                Field const &t_a,
                Field const &t_b
                );

        IndexChain(
                std::vector<GExpr> const &mu,
                Field              const &t_a,
                Field              const &t_b
                );

        bool isZero() const override;

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

        std::shared_ptr<Field> getPsiL() const {
            return psiL;
        }
        std::shared_ptr<Field> getPsiR() const {
            return psiR;
        }
        std::pair<csl::Index, csl::Index> getBorderIndices() const {
            return {a, b};
        }

        bool isHappy() const;

        bool isReversed() const;

        bool isFierzFixed()    const { return fierzFixed; }
        void setFixFierz(bool value) { fierzFixed = value; }

        bool hasCommonIndex(IndexChain const &other) const;
        GExpr mergeChain(IndexChain const &other) const;
        GExpr mergeConjugateChain(IndexChain const &other) const;
        bool hasPropertyWith(GExpr const &other) const override;
        GExpr propertyWith(GExpr const &other) const override;

        bool contains(csl::Index const &) const override;
        void replace(csl::Index const&, csl::Index const&) override;

        GExpr copy() const override;
        GExpr refresh() const override;

        csl::Expr toCSL(TensorSet const &tensors) const override;

        std::pair<GExpr, IndexChain> cut(
                std::size_t pos, 
                std::size_t len = 1
                ) const;

        void erase(std::size_t pos, std::size_t len);

        GExpr applyEOM(MomentumIndex const &p) const;

        GExpr simplify() override;

        GExpr calculateTrace() const;
        GExpr calculateStandardTrace() const;
        GExpr calculateChiralTrace() const;

        std::optional<GExpr> reduceStep() const;

        std::optional<GExpr> reduceMomenta(
                MomentumIndex const &p1,
                MomentumIndex const &p2
                ) const;

        static GExpr trace(
                IndexChain A,
                IndexChain C,
                IndexChain B,
                IndexChain D
                );

        GExpr applyGeneralFierz(
                IndexChain const &other,
                bool              chiral = true
                ) const;
        GExpr applyGeneralFierzTwice(
                IndexChain const &other,
                bool              chiral = true
                ) const;

        void print(std::ostream &out) const override;

        bool isTrace() const { return a == b && !(psiL || psiR); }

        std::optional<GExpr> checkGammaAndConjugation() const;

        static csl::Index easyIndex(int i) {
            if (auto pos = m_easyIndex.find(i); pos != m_easyIndex.end())
                return pos->second;
            m_easyIndex[i] = diracSpace->generateIndex();
            return m_easyIndex[i];
        }

        GExpr identityFactor(GammaIndex const &other) const;
        GExpr factor(GammaIndex const &A, GammaIndex const &B) const;
        GExpr getContributionTo(IndexChain const &other) const;

        std::pair<int, IndexChain> conjugated() const;

        friend GExpr momentumGammaContraction(
                sgl::IndexChain const&, csl::Tensor,
                csl::Index const&, csl::Index const&, std::size_t, std::size_t
                );

        GExpr moveIndex(std::size_t init, std::size_t target) const;

    private:

        bool hasContraction(std::size_t i, std::size_t j) const;
        std::optional<std::pair<std::size_t, std::size_t>> firstMove() const;
        std::optional<std::pair<std::size_t, std::size_t>> lastMove() const;

        GExpr contraction(std::size_t i) const;

    public:

        static bool isDelta(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isDelta();
        }
        static bool isGammaMu(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isGammaMu();
        }
        static bool isSigma(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isSigma();
        }
        static bool isChiral(GExpr const &gamma) {
            return isGamma5(gamma) || isP_L(gamma) || isP_R(gamma);
        }
        static bool isGamma5(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isGamma5();
        }
        static bool isProjector(GExpr const &gamma) {
            return isP_L(gamma) || isP_R(gamma);
        }
        static bool isP_L(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isP_L();
        }
        static bool isP_R(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isP_R();
        }
        static bool isC(GExpr const &gamma) {
            return dynamic_cast<GammaIndex const*>(gamma.get())->isC();
        }

        static GExpr CMatrix();
        static GExpr Id();
        static GExpr gamma5();
        static GExpr P_L();
        static GExpr P_R();

    private:

        csl::Index a;
        csl::Index b;
        std::shared_ptr<Field> psiL { nullptr };
        std::shared_ptr<Field> psiR { nullptr };
        bool fierzFixed = false;
    };

    template<class ...Args>
    GExpr indexchain_s(Args &&...args) {
        auto chain = std::make_shared<IndexChain>(std::forward<Args>(args)...);
        if (chain->isZero()) 
            return cslexpr_s(CSL_0);
        auto factor = chain->getFactor();
        if (factor != CSL_1) {
            auto term = std::dynamic_pointer_cast<IndexChain>(chain->getTerm());
            auto opt_chain = term->checkGammaAndConjugation();
            return factor * opt_chain.value_or(term);
        }
        return chain->checkGammaAndConjugation().value_or(chain);
    }

    template<class ...Args>
    GExpr indexchain_s(
            std::initializer_list<GExpr> gammas,
            Args &&...args) {
        return indexchain_s(
                std::vector<GExpr>(gammas.begin(), gammas.end()),
                std::forward<Args>(args)...
                );
    }

    GExpr indexchain_s(
            int a,
            int b
            );

    GExpr indexchain_s(
            const char mu[],
            int a,
            int b
            );

    GExpr indexchain_s(
            std::vector<std::string> const &mu,
            int a,
            int b
            );
}
