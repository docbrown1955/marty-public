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
 * @version 2.0

 * \brief
 */
#ifndef DIRACOLOGY_H
#define DIRACOLOGY_H

#include "../../csl/csl.h"
#include "quantumField.h"

namespace mty {

inline std::shared_ptr<csl::TDerivativeParent> partialMinko_shared
    = csl::tderivative_s("d", &csl::Minkowski);

inline csl::TDerivativeParent &partialMinko = *partialMinko_shared;

class DiracSpace : public csl::Space {

  private:
    csl::Space const *spaceTime;

  public:
    mutable csl::Tensor gamma;
    mutable csl::Tensor sigma;
    mutable csl::Tensor gamma_chir;
    mutable csl::Tensor P_L;
    mutable csl::Tensor P_R;
    mutable csl::Tensor C_matrix;

  public:
    DiracSpace(csl::Space const *t_spaceTime);

    ~DiracSpace();

    csl::Space const *getSpaceTime() const;

    csl::Index index() const;

    csl::Index st_index() const;

    friend void setDiracTensor4(const DiracSpace *self);

    void initProperties();

    Chirality getChirality(size_t pos, size_t cut, csl::Index proj) const;

    csl::vector_expr simplifyChain(csl::vector_expr const &tensors) const;

    static size_t getSpinorDimension(size_t spaceTimeDim);

    bool isDelta(csl::Expr const &tensor) const;

    bool isGammaTensor(csl::Expr const &tensor) const;

    bool isGammaMu(csl::Expr const &tensor) const;

    bool isSigma(csl::Expr const &tensor) const;

    bool isGammaChir(csl::Expr const &tensor) const;

    bool isP_L(csl::Expr const &tensor) const;

    bool isP_R(csl::Expr const &tensor) const;

    bool isProjector(csl::Expr const &tensor) const;

    bool isCMatrix(csl::Expr const &tensor) const;

    bool isSymmetric(csl::Expr const &tensor) const;

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

    bool isDelta(csl::Index const &tensor) const;

    bool isGammaMu(csl::Index const &tensor) const;

    bool isGammaChir(csl::Index const &tensor) const;

    bool isP_L(csl::Index const &tensor) const;

    bool isP_R(csl::Index const &tensor) const;

    bool isProjector(csl::Index const &tensor) const;

    bool isCMatrix(csl::Index const &tensor) const;

    csl::Index getSpaceTimeIndex(csl::Expr const &tensor) const;

    csl::Index getFirstIndex(csl::Expr const &tensor) const;

    csl::Index getSecondIndex(csl::Expr const &tensor) const;
};

inline const DiracSpace dirac4(&csl::Minkowski);

csl::Expr slashed_s(csl::Tensor       p,
                    const csl::Index &alpha,
                    const csl::Index &beta,
                    const DiracSpace *space = &dirac4);

csl::Expr bar_s(const csl::Expr &tensor, const DiracSpace *space = &dirac4);

} // End of namespace mty

#endif /* DIRACOLOGY_H */
