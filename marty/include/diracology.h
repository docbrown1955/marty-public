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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief
 */
#ifndef DIRACOLOGY_H
#define DIRACOLOGY_H

#include <csl.h>
#include "quantumField.h"

namespace mty {

inline std::shared_ptr<csl::TDerivativeParent> partialMinko_shared
    = csl::tderivative_s("d", &csl::Minkowski);

inline csl::TDerivativeParent& partialMinko = *partialMinko_shared;

class DiracSpace: public csl::Space {

    public:

    struct alignedCycle {
        csl::vector_expr tensors;
        std::vector<size_t> cutsBtwCycles;
    };

    struct Chain {
        csl::Expr factor;
        std::vector<csl::Index> indices;
        std::vector<size_t>     cuts;
        std::vector<std::pair<size_t, size_t>> fiertzFlipping;

        IMPLEMENTS_STD_VECTOR(csl::Index, indices)
    };

    struct FiertzContraction {

        size_t i;
        size_t j;
        size_t i_cut;
        size_t j_cut;
        Chirality i_chir;
        Chirality j_chir;

        bool isChiral() const {
            return i_chir != Chirality::None
               and j_chir != Chirality::None;
        }

        bool areChiralitySame() const {
            return i_chir == j_chir;
        }
    };

    private:

    csl::Space const* spaceTime;

    public:

    mutable
    csl::Tensor gamma;

    mutable
    csl::Tensor sigma;

    mutable
    csl::Tensor gamma_chir;

    mutable
    csl::Tensor P_L;

    mutable
    csl::Tensor P_R;

    mutable
    csl::Tensor C_matrix;

    public:

    DiracSpace(csl::Space const* t_spaceTime);

    ~DiracSpace();

    csl::Space const* getSpaceTime() const;

    bool hasSpecialTraceProperty(
            const csl::vector_expr& tensors) const override;

    csl::Expr calculateTrace(csl::vector_expr tensors) const override;

    csl::Expr calculateTrace(csl::Expr const& init) const;

    csl::Index index() const;

    csl::Index st_index() const;

    friend void setDiracTensor4(const DiracSpace* self);

    void initProperties();

    alignedCycle align(csl::vector_expr tensors) const;

    alignedCycle alignOpen(csl::vector_expr tensors) const;

    void insert(csl::Index       const & toInsert,
                std::vector<csl::Index>& tensors,
                std::vector<size_t>    & cuts,
                size_t i) const;

    void contract(std::vector<csl::Index>& tensors,
                  std::vector<size_t>    & cuts,
                  size_t i) const;

    void contract(std::vector<csl::Index>& tensors,
                  std::vector<size_t>    & cuts,
                  size_t i,
                  size_t j) const;

    void contractTensor(
            std::vector<csl::Index>& tensors,
            std::vector<size_t>    & cuts,
            size_t i,
            size_t j) const;

    std::vector<Chain> simplifyGammaProd(
            std::vector<csl::Index> const &indices,
            std::vector<size_t>     const &cuts,
            csl::Expr                    const &factor
            ) const;

    std::vector<Chain> simplifyGammaProd(
            Chain         const &init
            ) const;

    Chirality getChirality(
            size_t pos,
            size_t cut,
            csl::Index proj
            ) const;

    std::vector<FiertzContraction> getFiertzContractions(
            Chain const &init
            ) const;

    std::vector<Chain> simplifyFiertzProd(
            Chain         const &init
            ) const;
    
    void applyUniqueChiralityStructure(
            std::vector<csl::Index>& tensors,
            std::vector<size_t>& cuts,
            csl::Expr& factor) const;

    std::pair<csl::Index, csl::Index>
        getBorderOfChain(std::vector<csl::Expr>::const_iterator first,
                         std::vector<csl::Expr>::const_iterator last) const;

    std::vector<std::pair<csl::Index, csl::Index>>
        getBorderOfChains(std::vector<csl::Expr>   const& tensors,
                          std::vector<size_t>      & cuts) const;

    csl::vector_expr applyChainIndices(
            std::vector<csl::Index> const& tensors,
            std::vector<size_t>     const& cuts,
            std::vector<std::pair<size_t, size_t>>         const& flipped,
            std::vector<std::pair<csl::Index, csl::Index>> const& indices)
        const;

    csl::vector_expr simplifyChain(csl::vector_expr const& tensors) const;

    std::vector<csl::Index> applyInvolution(
            csl::vector_expr& tensors,
            csl::Expr            & factor) const;

    void applyChiralityProp(
            std::vector<csl::Index>& tensors,
            csl::Expr                   & factor) const;

    void simplifySquares(
            std::vector<csl::Index>& tensors,
            csl::Expr                   & factor) const;

    csl::Expr applyRecursion(
            std::vector<csl::Index>& tensors,
            csl::Expr                   & factor) const;

    csl::Expr applyStandardRecursion(
            std::vector<csl::Index> const& tensors,
            bool                           first = true) const;

    csl::Expr applyChiralRecursion(
            std::vector<csl::Index> const& tensors,
            bool                           first = true) const;

    csl::Expr compute(csl::vector_expr const& tensors) const;

    static
    size_t getSpinorDimension(size_t spaceTimeDim);

    size_t countGammaMult(csl::vector_expr const& tensors) const;

    size_t countGammaMult(std::vector<csl::Index> const& tensors) const;

    std::vector<csl::Index> exprToIndex(
            std::vector<csl::Expr> const& tensors,
            std::vector<size_t>&     cuts) const;

    csl::Expr indexToExpr(csl::Index const& spaceTimeIndex,
                     csl::Index const& first,
                     csl::Index const& second) const;

    bool isDelta(csl::Expr const& tensor) const;

    bool isGammaTensor(csl::Expr const& tensor) const;

    bool isGammaMu(csl::Expr const& tensor) const;

    bool isSigma(csl::Expr const& tensor) const;

    bool isGammaChir(csl::Expr const& tensor) const;

    bool isP_L(csl::Expr const& tensor) const;

    bool isP_R(csl::Expr const& tensor) const;

    bool isProjector(csl::Expr const& tensor) const;

    bool isCMatrix(csl::Expr const &tensor) const;

    bool isSymmetric(csl::Expr const& tensor) const;

    bool isDelta(csl::Expr_info tensor) const;

    bool isGammaTensor(csl::Expr_info tensor) const;

    bool isGammaMu(csl::Expr_info tensor) const;

    bool isSigma(csl::Expr_info tensor) const;

    bool isGammaChir(csl::Expr_info tensor) const;

    bool isP_L(csl::Expr_info tensor) const;

    bool isP_R(csl::Expr_info tensor) const;

    bool isProjector(csl::Expr_info tensor) const;

    bool isCMatrix(csl::Expr_info tensor) const;

    bool isSymmetric(csl::Expr_info tensor) const;

    bool isDelta(csl::Index const& tensor) const;

    bool isGammaMu(csl::Index const& tensor) const;

    bool isGammaChir(csl::Index const& tensor) const;

    bool isP_L(csl::Index const& tensor) const;

    bool isP_R(csl::Index const& tensor) const;

    bool isProjector(csl::Index const& tensor) const;

    bool isCMatrix(csl::Index const& tensor) const;

    void flipChirality(csl::Index& tensor) const;
    csl::Index flippedChirality(csl::Index const& tensor) const;

    csl::Index getSpaceTimeIndex(csl::Expr const& tensor) const;

    csl::Index getFirstIndex(csl::Expr const& tensor) const;

    csl::Index getSecondIndex(csl::Expr const& tensor) const;
};

class ConjugationSimplifier {

public:

    ConjugationSimplifier(mty::DiracSpace const *t_diracSpace)
        :diracSpace(t_diracSpace)
    {}

    bool hasContractionProperty(
            csl::Expr_info self, 
            csl::Expr_info other
            ) const;

    csl::Expr contraction(
            csl::Expr_info self,
            csl::Expr_info other
            ) const;

private:

    bool commutation(
            csl::Index &first,
            csl::Index &second,
            csl::Expr       &tensor
            ) const;

    bool commutationWithTensor(
            csl::Index &first,
            csl::Index &second,
            csl::Expr       &tensor
            ) const;
    bool commutationWithIntegral(
            csl::Index &first,
            csl::Index &second,
            csl::Expr       &tensor
            ) const;
    bool commutationWithProduct(
            csl::Index &first,
            csl::Index &second,
            csl::Expr       &prod
            ) const;
    bool commutationWithSum(
            csl::Index &first,
            csl::Index &second,
            csl::Expr       &sum
            ) const;
private:

    mty::DiracSpace const *diracSpace;
};

inline
const DiracSpace dirac4(&csl::Minkowski);

csl::Expr slashed_s(csl::Tensor     p,
              const csl::Index& alpha,
              const csl::Index& beta,
              const DiracSpace* space = &dirac4);

csl::Expr bar_s(const csl::Expr& tensor,
          const DiracSpace* space = &dirac4);

} // End of namespace mty

#endif /* DIRACOLOGY_H */
