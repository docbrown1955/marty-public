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
#ifndef INTERACTION_TERM_H_INCLUDED
#define INTERACTION_TERM_H_INCLUDED

#include <csl.h>
#include "quantumField.h"
#include "scalarField.h"
#include "vectorField.h"
#include "fermionicField.h"

namespace mty {


class InteractionTerm {

    public:

    using permutation = std::vector<std::pair<csl::Index, csl::Index>>;

    using TermType = std::shared_ptr<InteractionTerm>;

    inline static bool abbreviateFactors = false;

    public:

    InteractionTerm(InteractionTerm const&);  

    InteractionTerm& operator=(InteractionTerm const&);

    InteractionTerm()                             = default;

    InteractionTerm(InteractionTerm&&) = default;

    InteractionTerm& operator=(InteractionTerm&&) = default;

    InteractionTerm copy() const;

    InteractionTerm(csl::Expr                      const& term,
                    std::vector<QuantumField> const& t_content);

    InteractionTerm(csl::Expr const& term,
                    bool        copy = true);

    public:

    static void fillContent(
        csl::Expr const &prod,
        std::vector<mty::QuantumField> &fields
        );

    static std::vector<TermType>
        createAndDispatch(csl::Expr const& expression);

    bool hasSameContent(InteractionTerm const& other) const;

    bool hasSameContent(std::vector<mty::QuantumField> const &fields) const;

    size_t size() const;

    size_t nTerms() const;

    void clear();

    csl::Expr getTerm() const;

    csl::Expr getGlobalFactor() const;

    csl::Expr getTotalFactor() const;

    std::vector<QuantumField>& getContent();

    std::vector<QuantumField> const& getContent() const;

    std::vector<Particle> getParticles() const;
    
    std::vector<QuantumFieldParent*> getParticlesInfo() const;

    size_t count(QuantumFieldParent* particle) const;

    size_t getExponent(csl::Expr const& parameter) const;

    csl::Expr getFieldProduct(bool applyDerivatives = false) const;

    csl::Expr getFullExpression() const;

    csl::Expr applyFactorAndSymmetriesOn(csl::Expr const& init) const;

    csl::Tensor getPoint() const;

    void setPoint(csl::Tensor const& newPoint);

    bool contains(const QuantumFieldParent* p) const;

    bool contains(mty::Particle const &p) const {
        return contains(p.get());
    }

    bool containsExactly(const QuantumFieldParent* p) const;

    bool containsExactly(mty::Particle const &p) const {
        return containsExactly(p.get());
    }

    bool operator==(const InteractionTerm& other) const;

    bool operator==(const csl::Expr& other) const;

    friend
    std::ostream& operator<<(std::ostream         & out,
                             InteractionTerm const& term);

    private:

    bool gatherFieldContent(csl::Expr               const& expr,   
                            size_t                    multiplicity,
                            csl::IndexStructure const& derivativeStructure);

    void gatherFieldContent();

    static
    bool containsQuantumField(csl::Expr const& expr);

    std::pair<csl::Expr, csl::IndexStructure> buildContent(
            QuantumField              expr,
            csl::IndexStructure const& structure);

    static
    void recallIndices(permutation             & perm,
                       csl::IndexStructure const& old,
                       csl::IndexStructure const& structure);

    void gatherFactorsAndSymmetries();

    void gatherSymmetries(csl::Expr const& sum);

    static 
    bool areSimilarField(QuantumField const& A,
                         QuantumField const& B);

    static 
    bool areSimilarContent(
            std::vector<QuantumField> const &A,
            std::vector<QuantumField> const &B
            );

    static
    csl::Expr applyPermutation(permutation const& perm,
                          csl::Expr        const& init,
                          std::map<csl::Index, csl::Index>& constraints);

    private:

    csl::Expr term;

    bool forcedContent = false;

    std::vector<QuantumField> content;

    csl::Expr globalFactor = CSL_1;

    permutation globalPermutation;

    csl::vector_expr factors;

    std::vector<permutation> permutations;
};

bool hardComparison(csl::Expr const&, csl::Expr const&);

}

#endif
