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
#ifndef MTY_WILSONOPERATOR_H_INCLUDED
#define MTY_WILSONOPERATOR_H_INCLUDED 

#include "wilson.h"
#include "diracology.h"
#include "polarization.h"

namespace mty {

enum class OperatorType {

    Magnetic,
    ChromoMagnetic,
};

std::ostream &operator<<(
        std::ostream &out,
        OperatorType  type
        );

namespace OperatorParser {

    inline mty::DiracSpace const *diracSpace = &mty::dirac4;
    inline csl::Space      const *spaceTime  = &csl::Minkowski;

    struct Requirement {

        int spin;
        int conjugation;
    };

    struct Insertion {
        mty::QuantumField field;
        csl::Tensor       momentum;

        bool verifies(Requirement const &req) const {
            return field.getQuantumParent()->getSpinDimension() - 1 == req.spin
                and (field.isComplexConjugate() == req.conjugation
                        or req.conjugation == -1);
        }
    };

    csl::Expr getMagneticCoefficient(
            std::vector<Wilson> const &coefs,
            Chirality                  chirality
            );

    csl::Expr get_V_x_V_Coefficient(
            std::vector<Wilson> const &coefs
            );

    csl::Expr build_V_x_V(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            );

    csl::Expr get_V_x_V5_Coefficient(
            std::vector<Wilson> const &coefs
            );

    csl::Expr build_V_x_V5(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            );

    csl::Expr get_V5_x_V_Coefficient(
            std::vector<Wilson> const &coefs
            );

    csl::Expr build_V5_x_V(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            );

    csl::Expr get_V5_x_V5_Coefficient(
            std::vector<Wilson> const &coefs
            );

    csl::Expr build_V5_x_V5(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            );

    csl::Expr buildMagneticOperator(
            Insertion const &incomingFermion,
            Insertion const &outgoingFermion,
            Insertion const &vectorBoson,
            csl::Tensor      momentum,
            Chirality        chirality
            );

    std::optional<Insertion> getInsertion(
            std::vector<Requirement> &requirements,
            csl::Expr               const &qField
            );

    std::vector<Insertion> getInsertions(
            std::vector<Requirement> const &requirements,
            WilsonOperator           const &op
            );

    void sortInsertions(
            std::vector<Insertion>         &insertions,
            std::vector<Requirement> const &requirements
            );

} // End of namespace OperatorParser

} // End of namespace mty

#endif /* ifndef MTY_WILSONOPERATOR_H_INCLUDED */
