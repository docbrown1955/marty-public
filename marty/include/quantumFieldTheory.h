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
 * @version 1.0
 
 * \brief
 */
#ifndef QUANTUMFIELDTHEORY_H_INCLUDED
#define QUANTUMFIELDTHEORY_H_INCLUDED

#include <csl.h>
#include <vector>
#include "diracology.h"

namespace mty {

class QuantumFieldParent;
class QuantumField;
struct PartnerShip;

csl::Expr ScalarKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X);

csl::Expr FermionKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X);

csl::Expr VectorKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X);

csl::Expr GhostKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X);

csl::Expr ReplaceXiGauge(
        csl::Expr const &initalTerm
        );

csl::Expr ExponentialFactor(csl::Tensor & X,
                       csl::Tensor & Y,
                       csl::Tensor & P);

csl::Expr StandardDenominator(csl::Tensor    & P,
                         csl::Expr      const& mass,
                         csl::Expr      const& width,
                         bool              external = false);

csl::Expr StandardDenominator(csl::Tensor    & P,
                         csl::Expr       const& mass,
                         bool              external = false);

csl::Expr NullPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr ScalarPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr FermionPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

// These functions are not used anymore, see fermionFlow.h
// csl::Expr MajoranaPropagator(
//         QuantumField const& A,
//         QuantumField const& B,
//         csl::Tensor      & P,
//         bool                external);
// 
// csl::Expr MajoranaConjugatedPropagator(
//         QuantumField const& A,
//         QuantumField const& B,
//         csl::Tensor      & P,
//         bool                external);

csl::Expr VectorPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr FieldStrengthPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr FieldStrengthSquaredPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr IntegratedScalarPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr IntegratedFermionPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr IntegratedVectorPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr IntegratedFieldStrengthPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr IntegratedFieldStrengthSquaredPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external);

csl::Expr MajoranaMassTerm(
        csl::Expr const &mass,
        QuantumFieldParent* field
        );

csl::Expr MajoranaMassTerm(
        csl::Expr const &mass,
        QuantumFieldParent* fieldL,
        QuantumFieldParent* fieldR
        );

csl::Expr MassTerm(csl::Expr const&         mass,
              QuantumFieldParent* field);

csl::Expr MassTerm(csl::Expr const&         mass,
              QuantumFieldParent* fieldL,
              QuantumFieldParent* fieldR);

csl::Expr ExternalLeg(
        QuantumField       const& field, 
        const csl::Tensor&        impulsion,
        bool                      ruleMode = false,
        bool                      lockConjugation = false
        );

csl::Expr ExternalLeg(
        QuantumFieldParent     & field,
        csl::Tensor              impulsion,
        csl::Tensor              point,
        std::vector<csl::Index>   indices,
        bool                     particle,
        bool                     incoming,
        bool                     onshell,
        PartnerShip       const &partnerShip,
        bool                     ruleMode = false,
        bool                     lockConjugation = false
        );



}

#endif
