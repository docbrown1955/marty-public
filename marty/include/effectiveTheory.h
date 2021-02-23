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
#ifndef EFFECTIVE_THEORY_H_INCLUDED
#define EFFECTIVE_THEORY_H_INCLUDED

#include "model.h"

namespace mty {

////////////////////////////////////////////////////
//*************************************************/
//  Class EffOp                                   //
//*************************************************/
////////////////////////////////////////////////////

class EffOp {

    public:

    EffOp() = delete;

    EffOp(mty::Model*          model,
          mty::Particle const& particle,
          size_t               maxDim);

    EffOp(EffOp const& other) = default;

    EffOp& operator=(EffOp const& other) = default;

    std::optional<EffOp> addTerm(
            mty::FeynmanRule const& term,
            size_t                  posTerm);

    std::vector<EffOp> applyExternal(const size_t maxLoop) const;

    csl::vector_expr applyDerivatives() const;

    bool empty() const;

    bool isZero() const;

    csl::Expr getDimension() const;

    bool operator==(EffOp const& other) const;

    friend
    std::ostream& operator<<(std::ostream& fout,
                             EffOp const&  op);

    private:

    static
    int computeNLoops(size_t nIntegrated,
                      size_t nVertices);

    static 
    std::vector<mty::QuantumField> getContractibleFields(
            std::vector<mty::QuantumField> const& external);

    static
    void removeContractiblePair(
            std::vector<mty::QuantumField> & fields,
            mty::QuantumField         const& toRemove);

    static 
    void updateExternal(
            std::vector<std::vector<mty::QuantumField>>& external);

    static
    void clearAmplitude(csl::Expr& amplitude);

    int getMomentumNumber(csl::Expr const& expr) const;

    void replaceMomentumAndExternalLegs(
            csl::Expr& amplitude) const;

    void replaceExternalLegs(
            csl::Expr                            & amplitude, 
            std::map<int, csl::vector_expr>& derivatives) const;

    private:

    mty::Model*      model;

    mty::Particle    particle;

    size_t           maxDimension = 0;

    size_t           nLoops = 0;

    csl::Expr             dimension    = CSL_0;

    size_t           nVertices    = 0;

    std::vector<size_t> termsInOp;

    std::vector<mty::QuantumField> externalLegs;
};


///////////////////////////////////////////////////
/*************************************************/
// Class EffectiveTheory                         //
/*************************************************/
///////////////////////////////////////////////////

class EffectiveTheory {

    public:

    EffectiveTheory() = delete;

    static
    void integrateOutParticle(mty::Model&    model,
                              mty::Particle& particle,
                              size_t         maxDim,
                              size_t         nLoops,
                              size_t         maxDimOperator = 0);

    private:

    static
    void clear();

    static
    void integrateOutParticle();

    static
    void findOperators();

    static 
    void applyExternalLegs();

    static
    void cutHigherDimensionalOperators();

    static 
    csl::vector_expr applyDerivatives();

    static
    csl::Expr getDimension(csl::Expr const& field);

    private:

    static inline
    mty::Model*    model;

    static inline
    mty::Particle  particle;

    static inline
    size_t         maxDim;

    static inline
    size_t         maxDimOperator;

    static inline
    size_t         nLoops;

    static inline
    std::vector<EffOp> operators;

    static inline
    std::vector<size_t> placeInLagrangian;
};

} // End of namespace mty

#endif
