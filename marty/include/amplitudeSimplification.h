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
 * @file amplitudeSimplification.h
 *
 * @brief File containing all the simplification routines for MARTY.
 *
 * @details When a calculation is done (amplitude, squared amplitude or Wilson 
 * coefficient) it is simplified by the mty::simpli::simplify() method that 
 * uses directly or indirectly all the other routines in this file. The other
 * rountines can for the most part be called separatly, but for a full successful
 * simplification the order is extremely important and the mty::simpli::simplify()
 * method is a reference.
 *
 * @author Grégoire Uhlrich
 *
 * @version 1.3
 *
 * @date 2021-04-16
 */
#pragma once

#include <csl.h>

namespace color {
    class ColorSpace;
}
namespace mty {
    class QuantumField;
    class FeynOptions;
}

/**
 * @brief Namespace containing the main simplification method for MARTY and all
 * its dependencies.
 */
namespace mty::simpli {

    /**
     * @brief Mode for the main simplification routine specifying what 
     * calculation is done to adapt the simplification procedure.
     */
    enum Mode {
        FeynmanRule,
        Amplitude,
        SquaredAmplitude,
        WilsonCoefficient
    };

    ///////////////////////////////////////////////////
    /*************************************************/
    // Main simplification routine using the other   //
    // routines below                                //
    /*************************************************/
    ///////////////////////////////////////////////////

    /**
     * @brief Main simplification routine of MARTY, used for amplitudes, squared
     * amplitude and Wilson coefficients.
     *
     * @details This function for now is used for all calculations in MARTY with
     * minor differences (handled through the mode given as a parameter). It 
     * calls directly or indirectly all the other functions in this file to 
     * simplify Dirac algebra, Group algebra, Equations of motions, momentum
     * integrals, introducing abbreviations etc.
     *
     * @param expr       Expression to simplify.
     * @param insertions Insertions of the process.
     * @param momenta    External momenta corresponding to the field insertions.
     * @param mode       Simplification mode specifying if the expression is an
     * amplitude, squared amplitude or Wilson coefficient.
     */
    void simplify(
            csl::Expr                            &expr,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            mty::FeynOptions               const &options,
            Mode                                  mode
            );

    ///////////////////////////////////////////////////
    /*************************************************/
    // Simplification functions for color structures //
    /*************************************************/
    ///////////////////////////////////////////////////

    /**
     * @brief Returns the colorspace associated to an index (nullptr if the 
     * index does not live in any ColorSpace).
     *
     * @param index Index form which the space is obtained.
     *
     * @return The pointer to const ColorSpace if the index lives in such a 
     * vector space.
     * @return nullptr else
     */
    color::ColorSpace const *inColorSpace(csl::Index const& index);


    /**
     * @brief Returns the adjoint colorspace for a generator, nullptr for 
     * another structure.o
     *
     * @details All indices of the structure must lie in a color space otherwise
     * it is not a generator and this function returns nullptr. This allows to
     * select only the objects that may want to simplify with each other !
     *
     * @param structure The index structure of the tensor to test.
     *
     * @return The adjoint colorspace if the indexstructure is the one of a
     * group generator.
     * @return nullptr else.
     *
     * @sa inColorSpace()
     */
    color::ColorSpace const *isColorStructure( 
            csl::IndexStructure const& structure
            );

    /**
     * @brief Returns a colorspace if the expression is a color structure.
     *
     * @details This function should not be used for mixed color / non color
     * expressions. In a mixed case, the behaviour is not defined. Otherwise,
     * a colorspace will be returned independently of the complexity of the 
     * expression (if it is a color structure).
     *
     * @param tensor Expression to test.
     *
     * @return The color space associated to the expression if there is one
     * @return nullptr else.
     */
    color::ColorSpace const *isColorStructure(csl::Expr const &expr);

    /**
     * @brief Returns all color indices found in an expression lying in a 
     * particular color space.
     *
     * @param node  Expression to explore.
     * @param color Color space for which this function searches indices.
     *
     * @return The IndexStructure containing all found indices.
     */
    csl::IndexStructure colorStructure(
            csl::Expr const &node,
            color::ColorSpace const *color
            );

    /**
     * @brief Factors indicial expressions in a bigger expression.
     *
     * @details This function is there mostly to compensate the lack of such a
     * general function in CSL, that factors indicial tensors. This function is
     * yet not very general and can probably not be used reliably to this 
     * purpose in other contexts.
     *
     * @param res Expression to factor
     *
     * @return \b True  if something has been changed (factored).
     * @return \b False else.
     */
    bool factorIndicial(csl::Expr &res);

    /**
     * @brief Abbreviates color structure in expressions under the generic
     * name "Color".
     *
     * @details For all different terms this function tries to simplify first
     * the expression before abbreviating it. If after simplification there is 
     * still color indices the result is encapsulated into an abbreviation, 
     * otherwise it is left as is.
     *
     * @param exprs   Different expressions containing color structures.
     * @param spaces  Color spaces corresponding to the different expressions (
     * must be the same size as exprs).
     * )
     *
     * @return The product of all abbreviated expressions in exprs.
     */
    csl::Expr colorAbbreviation(
            std::vector<csl::Expr> const &exprs,
            std::vector<color::ColorSpace const*> const &spaces
            );
    /**
     * @brief Expands indicial expressions in a way avoiding unnecessary terms,
     * in particular not expanding expressions that have no common index.
     *
     * @details This function takes as parameter another function that must 
     * return, for any expression, the index structure relevant for expansion 
     * (e.g. returning only Minkowski indices if the purpose is to expand 
     * Minkowski metric terms). The expansion will be performed between two 
     * sub-expressions if they have at least one common index by this mean.
     *
     * @note Dev note : This function could now probably be replaced by the much
     * more powerful and general csl::DeepPartialExpand() that can be tuned
     * as will. This function is now used only in expandColorIndices().
     *
     * @param prod            Prod in which the expansion must take place.
     * @param structureGetter Function that must return the index structure of
     * an expression relevant for the expansion.
     *
     * @return \b True  if something has been expanded
     * @return \b False else.
     */
    bool expandInProd(
            csl::Expr &prod,
            std::function<csl::IndexStructure(csl::Expr const&)> const &structureGetter
            );

    /**
     * @brief Expands expressions containing color indices to allow all algebra
     * simplifications.
     *
     * @param res Expression in which color indexed structures must be expanded.
     *
     * @return \b True  if something has been expanded.
     * @return \b False else.
     */
    bool expandColorIndices(csl::Expr &res);

    /**
     * @brief Calculates color traces in an expression.
     *
     * @param expr Expression to simplify.
     *
     * @return \b True  if a simplification has taken place.
     * @return \b Fasle else.
     */
    bool simplifyColorWeights(csl::Expr &expr);

    /**
     * @brief Abbreviates color structures in an expression under the generic 
     * name "Color".
     *
     * @details This function gathers all colorstructures of possibly different
     * vector spaces and then calls colorAbbreviation() to effectively determine
     * an abbreviation for each different space.
     *
     * @param expr Expression to abbreviate.
     *
     * @return \b True  if the expression has been abbreviated in some way.
     * @return \b False else.
     */
    bool findColorAbbreviation(csl::Expr& expr);

    ///////////////////////////////////////////////////
    /*************************************************/
    // Simplification functions for gamma / Minkowski//
    /*************************************************/
    ///////////////////////////////////////////////////

    /**
     * @brief Expands Minkowski structures to contract all possible indices.
     *
     * @details This function calls expandMinkoMetric() and expandMinkoEpsilon()
     * successively.
     * 
     * @param expr Expression in which Minkowski structures must be expanded.
     *
     * @return \b True  if something has been expanded.
     * @return \b False else.
     */
    bool expandMinkoStructures(csl::Expr &expr);

    /**
     * @brief Expands Minkowski metrics to contract all possible indices.
     *
     * @details This function only expand sub-terms that allow to contract
     * one metric with another tensor that has a common index. All other terms
     * are let in sums, not expanded.
     *
     * @note Dev note : This is using the emitter / receiver pattern of 
     * csl::DeepPartialExpand(): to use without moderation !
     *
     * @param expr Expression in which Minkowski metric terms must be expanded.
     *
     * @return \b True  if something has been expanded.
     * @return \b False else.
     */
    bool expandMinkoMetric(csl::Expr &expr);

    /**
     * @brief Expands Minkowski epsilon symbols to contract all possible 
     * indices.
     *
     * @details This function only expand sub-terms that allow to contract
     * one epsilon with another tensor that has a common index. All other terms
     * are let in sums, not expanded.
     *
     * @note Dev note : This is using the emitter / receiver pattern of 
     * csl::DeepPartialExpand(): to use without moderation !
     *
     * @param expr Expression in which Minkowski epsilon terms must be expanded.
     *
     * @return \b True  if something has been expanded.
     * @return \b False else.
     */
    bool expandMinkoEpsilon(csl::Expr &expr);

    /**
     * @brief Simplifies the contraction of an epsilon tensor with a symmetric
     * structure index.
     *
     * @details This function is fully general and tries to swap indices that 
     * are contracted with the epsilon tensor to detect a symmetry. If a 
     * symmetry is detected, then the result is zero.
     *
     * @param prod    Product to simplify ni which the epsilon tensor has been 
     * removed.
     * @param indices Indices of the initial epsilon tensor.
     *
     * @return \b True  if a simplification has been found (the result is zero).
     * @return \b False else.
     */
    bool simplifyEpsilonInProd(
            csl::Expr &prod,
            csl::IndexStructure &indices
            );

    /**
     * @brief Simplifies epsilon contractions with symmetric indices in an 
     * expression.
     *
     * @brief This function detects all epsilon tensors in a product and calls
     * simplifyEpsilonInProd() to simplify it if it is contracted with symmetric
     * indices.
     *
     * @param expr Expression to simplify.
     *
     * @return \b True  if a simplification has been found.
     * @return \b False else.
     */
    bool simplifyEpsilon(csl::Expr &expr);

    /**
     * @brief Expands gamma matrices in an expression to be able to simplify
     * fermion chains (and traces).
     *
     * @param expr Expression to simplify.
     *
     * @return \b True  if a simplification has been found.
     * @return \b False else.
     */
    bool expandGammaMatrices(csl::Expr &expr);

    /**
     * @brief Simplifies fermion chains using the mty::FermionChain utility.
     *
     * @param expr Expression to simplify.
     *
     * @return \b True  if a simplification has been found.
     * @return \b False else.
     */
    bool simplifyFermionChains(csl::Expr &expr);

    /**
     * @brief Forces the ordering of external fermions by making sure that 
     * all Dirac structures are expanded.
     *
     * @param expr Expression to expand.
     */
    void expandForFermionOrdering(csl::Expr &expr);

    ///////////////////////////////////////////////////
    /*************************************************/
    // Momenta and one-loop related simplifications  //
    /*************************************************/
    ///////////////////////////////////////////////////

    /**
     * @brief Expands expressions containing momenta to contract all indices,
     * typically replacing \f$ p_i^\mu\cdot p_{j\mu} \mapsto s_{ij} \f$.
     *
     * @param res     Expression to expand.
     * @param momenta Set of momenta involved.
     */
    void expandMomentaExperimental(
            csl::Expr &res,
            std::vector<csl::Tensor> const &momenta
            );
    /**
     * @brief Replaces the one-loop momentum integrals by their reduced form
     * depending on scalar integrals.
     *
     * @details The scalar integrals are later evaluated numerically using 
     * the LoopTools library. 
     *
     * @param expr Expression to simplify.
     */
    void reduceTensorIntegrals(csl::Expr &expr);

    /**
     * @brief Helper structure for the application of Dirac equation
     * (equations of motions for spin 1/2)
     */
    struct FermionEOMData {
        csl::Tensor p;
        csl::Expr   m;
        csl::Index  alpha;
        bool        sign;
    };

    /**
     * @brief Creates the relevant replacement (using momentum conservation) to
     * replace one particular momentum.
     *
     * @details The two elements returned in a pair can be used directly in 
     * a csl replacement function on a particular expression as presented in the
     * following sample code
     * \code
     *      auto [p, momentumEquivalent] = getMomentumReplacement(args...);
     *      csl::Replace(amplitude, p, momentumEquivalent);
     * \endcode
     *
     * @param insertions  Insertions for the process.
     * @param momenta     Set of external momenta of the process.
     * @param posReplaced Position of the momentum (in \b momenta) that must be 
     * replaced by the relevant combination of the others.
     *
     * @return A pair containing the momentum replacement to forward to 
     * csl::Replace().
     */
    std::pair<csl::Expr, csl::Expr> getMomentumReplacement(
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            size_t                                posReplaced
            );
    /**
     * @brief Applies the momentum conservation by replacing one of the external
     * momenta by the combination of the others.
     *
     * @param init        Expression in which the momentum conservation is
     * applied.
     * @param insertions  Insertions for the process.
     * @param momenta     Set of external momenta of the process.
     * @param posReplaced Position of the momentum (in \b momenta) that must be 
     * replaced by the relevant combination of the others.
     *
     * @sa getMomentumReplacement()
     */
    void replaceMomentum(
            csl::Expr                            &init,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            size_t                                posReplaced
            );

    /**
     * @brief Applies the momentum conservation by replacing one of the external
     * momenta by the combination of the others.
     *
     * @details The momentum that is to be replaced is chosen automatically by
     * this function following an ordering rule for the external fields. To 
     * maximize the possible simplifications, the momentum that are kept are 
     * those of on-shell fermions (because they can later be simplified later
     * on through the Dirac equation). This function calls replaceMomentum().
     *
     * @param init       Expression in which the momentum conservation is 
     * applied.
     * @param insertions Insertions for the process.
     * @param momenta    Set of external momenta of the process.
     */
    void simplifyImpulsions(
            csl::Expr                            &init,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta
            );

    /**
     * @brief  Applies the equations of motions in an expression.
     *
     * @details This function uses the SGL (Symbolic Gamma-matrices Library),
     * a module of MARTY specialized in this purpose, to apply the EOM in 
     * fermion currents.
     *
     * @param ampl            Expression in which the EOM must be applied.
     * @param onShellFermions On-shell fermions.
     */
    void applyEOM(
        csl::Expr                   &ampl,
        std::vector<FermionEOMData> &onShellFermions
        );

    /**
     * @brief Applies the equations of motion in an expression.
     *
     * @details This function detects all the on-shell fermions for the process
     * and call the other overload of applyEOM() to effectively apply the Dirac
     * equation.
     *
     * @param ampl       Expression in which the EOM must be applied.
     * @param insertions Field insertions for the process.
     * @param momenta    External momenta corresponding to the field insertions.
     */
    void applyEOM(
            csl::Expr                            &ampl,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta
            );

    /**
     * @brief Adds the local terms in an expression.
     *
     * @details This function is defined in dimensionalRegularization.cpp but
     * the declaration is reminded. Local terms are added when, in dim. reg., 
     * the Minkowki space dimension \f$ D = 4 - 2\epsilon \f$ is multiplied by a 
     * divergent integral \f$ I = \frac{a}{\epsilon}  + b + \mathcal{O}(\epsilon)\f$,
     * as the epsilon terms cancel each other to produce another constant term.
     * In this simple case we obtain (removing the purely divergent term):
     * \f[
     *      D\cdot I = (4 - 2\epsilon)(\frac{a}{\epsilon} + b) = 4b + Local,
     * \f]
     * with
     * \f[
     *      Local = -2a.
     * \f]
     *
     * @warning This uses a simple algorithm to parse the total \f$ D \f$ factors
     * mutiplying integrals but works only for non-factored expressions. This is
     * fine as long as, in the main simplification chain, the amplitude is 
     * factored **after** this function is called.
     *
     * @param res Expression in which local terms are added.
     */
    void addLocalTerms(csl::Expr &res);

    ///////////////////////////////////////////////////
    /*************************************************/
    // Miscellaneous routines                        //
    /*************************************************/
    ///////////////////////////////////////////////////
    
    /**
     * @brief Helper function that tells if an expression may be simplified in
     * amplitudes.
     *
     * @details If this function returns false, the main simplification routines
     * can remove the expression from the amplitude to pass all simplification
     * steps and put it back at the end to save unnecessary calculations.
     *
     * @param expr Expression to check if it may be simplified.
     *
     * @return \b True  if the expression must not be discarded.
     * @return \b False else.
     */
    bool maybeSimplified(csl::Expr const &expr);

    /**
     * @brief Abbreviates external legs in the expression using the generic name
     * "EXT".
     *
     * @note Dev Note : This function works only for external fields of type 
     * PolarizationField. Could probably be extended very easily to
     * QuantumField objects using the following condition
     * \code 
     *      dynamic_cast<mty::QuantumField const*>(sub[i].get())
     * \endcode
     *
     * @param expr Expression ni which the abbreviation will be applied.
     */
    void findExternalAbbreviation(csl::Expr &expr);
    
    /**
     * @brief Abbreviate scalar integrals (or combinations of them) into 
     * abbreviations with the generic name "INT"
     *
     * @param res Expression to abbreviate.
     */
    void abbreviateIntegral(csl::Expr &res);

    /**
     * @brief Abbreviates all the sub-expressions that are relevant.
     *
     * @details This function is more powerful than findAbbreviations() as it
     * will also find external abbreviations ("EXT") and momentum abbreviations
     * ("P")
     *
     * @param res Expression to abbreviate.
     */
    void abbreviateAll(csl::Expr &res);

    /**
     * @brief Abbreviates all the sub-expressions that are relevant considering
     * constant factors only.
     *
     * @details This function is simply searches for combination of factors than
     * can be grouped into a unique abbreviation "Ab".
     * 
     * @param res Expression to abbreviate.
     */
    bool findAbbreviations(csl::Expr& res);

    /**
     * @brief Searches in the expression derived fields and applies the 
     * derivatives in the fields themselves.
     *
     * @details QuantumField objects may carry an additional derivative indicial
     * structure, that is used here to get simple quantum field objects (no 
     * derivative) while keeping the information about the Minkowski derivatives
     * to use in the quantum calculation. This is equivalent to the analytical 
     * replacement / definition:
     * \f[
     *      \partial _\mu \Phi _A(X) \equiv \Phi_{A;\mu}(X)
     * \f]
     *
     * @param expr             Expression in which the the function is applied.
     */
    void applyDerivativesInStructure(
            csl::Expr &expr
            );

    /**
     * @brief Removes a dirac delta sub-expression.
     *
     * @details This function is used at the end of a quantum calculation to 
     * remove the \f$ \delta ^{(4)}(\sum _i p_i) \f$ (with \f$ p_i \f$ external
     * momenta) that appears naturally but does not enter the final amplitude 
     * result. The momentum sum in the dirac delta to remove must be provided.
     *
     * @param expr Expression in which 
     * @param PSum Momentum sum appearing in the dirac delta to remove.
     */
    void suppressDiracDelta(
            csl::Expr       &expr,
            csl::Expr const &PSum
            );

} // namespace mty
