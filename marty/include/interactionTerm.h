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
 * @file interactionTerm.h
 * \brief Class mty::InteractionTerm, general purpose container for Lagrangian
 * terms in MARTY.
 * @author Grégoire Uhlrich
 * @version 1.3
 */
#ifndef INTERACTION_TERM_H_INCLUDED
#define INTERACTION_TERM_H_INCLUDED

#include <csl.h>
#include "quantumField.h"
#include "scalarField.h"
#include "vectorField.h"
#include "fermionicField.h"

namespace mty {


/**
 * @brief Interaction term (in the Lagrangian) in MARTY.
 *
 * @details This class stores an interaction term and decomposes it in a more
 * abstract representation to lighten calculations of Feynman rules (that is the
 * full quantum calculation of tree-level matrix elements). It is used as the 
 * term container in the mty::Lagrangian.
 *
 * This class is also used, at the user interface level, in 
 * Lagrangian filters (mty::FeynOptions::LagrangianFilter), that are predicates
 * taking a mty::InteractionTerm (const reference) as parameter. Functions that
 * are relevant to use as filter conditions are documented (other functions may
 * also be documented) i.e. an undocumented function is probably not relevant 
 * for filtering anyway.
 */
class InteractionTerm {

    public:

    using permutation = std::vector<std::pair<csl::Index, csl::Index>>;

    using TermType = std::shared_ptr<InteractionTerm>;

    /**
     * @brief Options telling if factors (constants etc.) must be abbreviated
     * or not. Default is false.
     *
     * @details If enabled, abbreviations will be introduced for all interaction
     * terms. This can save some calculations but will make the lagrangian much
     * less readable.
     */
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

    /**
     * @brief Returns the number of interacting fields.
     *
     * @return The number of interacting fields.
     */
    std::size_t size() const;

    std::size_t nTerms() const;

    void clear();

    /**
     * @brief Returns the symbolic expression corresponding to the interaction.
     *
     * @return #term
     */
    csl::Expr getTerm() const;

    csl::Expr getGlobalFactor() const;

    csl::Expr getTotalFactor() const;

    csl::Expr getMass() const;

    /**
     * @brief Returns a reference to the field content of the interaction.
     *
     * @return #content
     */
    std::vector<QuantumField>& getContent();

    /**
     * @brief Returns a const reference to the field content of the interaction.
     *
     * @return #content
     */
    std::vector<QuantumField> const& getContent() const;

    /**
     * @brief Returns the set of different interacting particles.
     *
     * @details This function creates the result, looping through the content
     * and isolating different fields. mty::Particle objects are returned, 
     * allowing one to then use them with other interface functions.
     *
     * @return The set of different interacting particles.
     */
    std::vector<Particle> getParticles() const;
    
    /**
     * @brief Returns the set of different interacting particles.
     *
     * @details This function creates the result, looping through the content
     * and isolating different fields. Pointers to mty::QuantumFieldParent are 
     * returned meaning that this function should be use for read-only 
     * operations on the fields. Otherwise use getParticles().
     *
     * @return The set of different interacting particles.
     */
    std::vector<QuantumFieldParent*> getParticlesInfo() const;

    /**
     * @brief Counts, in the content, how many particles are of a given species.
     *
     * @param particle Species.
     *
     * @return The number of interacting particles having exactly \b particle
     * as parent (species).
     */
    std::size_t count(QuantumFieldParent* particle) const;

    /**
     * @brief Returns the exponent associated to a given coupling in the 
     * interaction term.
     *
     * @param parameter Coupling to test.
     *
     * @return 0 if the term does not depend on \b param.
     * @return n if the term is proportional to \f$ param^n \f$.
     */
    std::size_t getExponent(csl::Expr const& parameter) const;

    /**
     * @brief Returns the product of bare fields with only free indices.
     *
     * @details This field product is used for calculations. All factors and 
     * index contraction are removed from the term to obtain a very simple and
     * general expression. For example, interaction terms
     * \f[
     *      eQ A_\mu\bar{psi}_\alpha \gamma^\mu_{\alpha\beta}\psi{\beta},\\
     *      -\frac{g^2}{4}f^{EAB}f^{ECD}G_\mu^A G_\nu^B G^{C\mu}G^{D\nu},
     * \f]
     * have respective field products equal to 
     * \f[
     *      A_\mu \bar{psi}_{alpha}\psi_{\beta},
     *      G^{A\mu}G^{B\nu}G^{c\rho}G^{D\sigma}.
     * \f]
     *
     * @return The field product of the interaction term.
     *
     * @sa applyFactorAndSymmetriesOn()
     */
    csl::Expr getFieldProduct() const;

    /**
     * @brief Returns the full expression of the interaction term.
     *
     * @details This function should be equivalent to the getTerm() function,
     * but does more calculations (this means that one should not use it in 
     * general). This function is most of all a way to check that applying
     * symmetries on the bare field product returned by getFieldProduct() one 
     * recovers an expression equivalent to #term.
     *
     * @note This function is quasi-equivalent to call
     * \code
     *      csl::Expr fullExpression = applyFactorAndSymmetriesOn(getFieldProduct());
     * \endcode
     *
     * @return The full expression corresponding to the interaction term.
     */
    csl::Expr getFullExpression() const;

    /**
     * @brief Applies all factors and symmetries on a field product to recover 
     * the full interaction term properties.
     *
     * @details This function applies all factors and symmetries that were 
     * removed to obtain the result of getFieldProduct(). This allows to do 
     * the calculation with a simple and general field product, and apply
     * this function on the result to recover couplings, index contractions etc.
     *
     * @param init Initial expression on which factors and symmetries must be
     * applied.
     *
     * @return The expression on which factors and symmetries have been applied.
     *
     * @sa getFieldProduct()
     */
    csl::Expr applyFactorAndSymmetriesOn(csl::Expr const& init) const;

    csl::Tensor getPoint() const;

    void setPoint(csl::Tensor const& newPoint);

    /**
     * @brief Tells if the interaction term contains a given field.
     *
     * @details This function searches for a partial match between fields, and
     * takes into account fields that contain others. For example, a Dirac 
     * fermion contains its Weyl fermions and not the contrary. For a Dirac
     * fermion \f$ \psi \equiv \psi_L \oplus \psi_R \f$, one has that 
     *  \f$ \psi \f$ contains \f$ \psi_L \f$ but \f$ \psi_L \f$ does not contain
     *  \f$ \psi \f$. Otherwise, this function is equivalent to 
     *  containsExactly().
     *
     * @param p A const pointer to the parent of the field to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same species included in **p**.
     * @return \b False else.
     */
    bool contains(const QuantumFieldParent* p) const;

    /**
     * @brief Tells if the interaction term contains a given field or only a 
     * part of it.
     *
     * @details This function searches for a partial match between fields, and
     * takes into account fields that contain others. For example, a Dirac 
     * fermion contains its Weyl fermions and not the contrary. This weak 
     * comparison will return true event if the interaction term does not 
     * contain the entire field (a Weyl fermion in particular contains in this
     * weak sense its Dirac parent).
     *
     * @param p A const pointer to the parent of the field to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same species.
     * @return \b False else.
     */
    bool containsWeakly(const QuantumFieldParent* p) const;

    /**
     * @brief Tells if the interaction term contains a given field.
     *
     * @details This function searches for a partial match between fields, and
     * takes into account fields that contain others. For example, a Dirac 
     * fermion contains its Weyl fermions and not the contrary. For a Dirac
     * fermion \f$ \psi \equiv \psi_L \oplus \psi_R \f$, one has that 
     *  \f$ \psi \f$ contains \f$ \psi_L \f$ but \f$ \psi_L \f$ does not contain
     *  \f$ \psi \f$. Otherwise, this function is equivalent to 
     *  containsExactly().
     *
     * @param p The particle to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same species included in **p**.
     * @return \b False else.
     */
    bool contains(mty::Particle const &p) const {
        return contains(p.get());
    }

    /**
     * @brief Tells if the interaction term contains a given field or only a 
     * part of it.
     *
     * @details This function searches for a partial match between fields, and
     * takes into account fields that contain others. For example, a Dirac 
     * fermion contains its Weyl fermions and not the contrary. This weak 
     * comparison will return true event if the interaction term does not 
     * contain the entire field (a Weyl fermion in particular contains in this
     * weak sense its Dirac parent).
     *
     * @param p A const pointer to the parent of the field to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same species.
     * @return \b False else.
     */
    bool containsWeakly(mty::Particle const &p) const {
        return containsWeakly(p.get());
    }

    /**
     * @brief Tells if the interaction term contains exactly a given field.
     *
     * @details This function searches for an exact match between fields, and
     * does not take into account fields that contain others. For example, this
     * function considers that for a Dirac fermion 
     * \f$ \psi = \psi_L\oplus\psi_R \f$, one has that \f$ \psi \f$ and 
     * \f$ \psi_L \f$ are **different** fields (which is strictly true).
     *
     * @param p A const pointer to the parent of the field to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same specied.
     * @return \b False else.
     */
    bool containsExactly(const QuantumFieldParent* p) const;

    /**
     * @brief Tells if the interaction term contains exactly a given field.
     *
     * @details This function searches for an exact match between fields, and
     * does not take into account fields that contain others. For example, this
     * function considers that for a Dirac fermion 
     * \f$ \psi = \psi_L\oplus\psi_R \f$, one has that \f$ \psi \f$ and 
     * \f$ \psi_L \f$ are **different** fields (which is strictly true).
     *
     * @param p The particle to search for.
     *
     * @return \b True  if the interaction term contains \b p or a particle of 
     * the same specied.
     * @return \b False else.
     */
    bool containsExactly(mty::Particle const &p) const {
        return containsExactly(p.get());
    }

    /**
     * @brief Equality operator, compares the symbolic expressions.
     *
     * @param other Other interaction term to compare.
     *
     * @return \b True  if the two interaction terms have the same #term 
     * (csl::Expr comparison is used).
     * \return \b False else.
     */
    bool operator==(const InteractionTerm& other) const;

    /**
     * @brief Equality operator with a csl::Expr.
     *
     * @param other Symbolic expression to compare.
     *
     * @return \b True  if the symbolic expression for the term (#term) is 
     * equal (csl::Expr definition of equality) to \b other.
     * \return \b False else.
     */
    bool operator==(const csl::Expr& other) const;

    friend
    std::ostream& operator<<(std::ostream         & out,
                             InteractionTerm const& term);

    private:

    bool gatherFieldContent(csl::Expr               const& expr,   
                            std::size_t                    multiplicity,
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

    /**
     * @brief Initial expression defining the interaction (symbolic Lagrangian 
     * term).
     */
    csl::Expr term;

    /**
     * @brief Quantum fields in the interactions.
     */
    std::vector<QuantumField> content;

    bool forcedContent = false;

    csl::Expr globalFactor = CSL_1;

    permutation globalPermutation;

    csl::vector_expr factors;

    std::vector<permutation> permutations;
};

}

#endif
