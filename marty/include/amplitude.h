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
 
 * \brief Contains utilities for amplitude calculation and simplification.
 */
#ifndef AMPLITUDE_H_INCLUDED
#define AMPLITUDE_H_INCLUDED

#include "model.h"
#include "colorSpace.h"

namespace mty {

    struct FermionEOMData {

        csl::Tensor p;
        csl::Expr        m;
        csl::Index  alpha;
        bool        sign;
    };

    /**
     * @brief Contains the result of an amplitude calculation.
     *
     * @details Contains in particular all the expressions and diagrams 
     * resulting from an amplitude calculation.
     */
    struct Amplitude {

        /**
         * @brief Type for the diagram objects.
         */
        using Diagram = std::shared_ptr<wick::Graph>;

        /**
         * @brief Different terms of the amplitude.
         *
         * @details The final result would be the sum of all terms
         * \code
         *      csl::Expr amplitude = csl::sum_s(expressions);
         * \endcode
         */
        std::vector<csl::Expr>    expressions;
        /**
         * @brief All the diagrams of the process.
         */
        std::vector<Diagram> diagrams;
        /**
         * @brief Initial field insertions.
         */
        std::vector<mty::QuantumField> insertions;
        /**
         * @brief List of momenta used in the calculation for external legs.
         */
        std::vector<csl::Tensor>       momenta;

        /**
         * @return The number of expressions, i.e. 
         * \code 
         *      expressions.size()
         * \endcode
         */
        size_t size() const {
            return expressions.size();
        }

        /**
         * @return \b True  if there is no expression (0 amplitude).
         * @return \b False else.
         */
        bool empty() const {
            return expressions.empty();
        }

        /**
         * @brief Adds a pair of expression / diagram to the result.
         *
         * @param el Pair of an expression and a Feynman diagram to append to 
         * the result.
         */
        void push_back(std::pair<csl::Expr, Diagram> const &el) {
            expressions.push_back(el.first);
            diagrams.push_back(el.second);
        }

        /**
         * @brief Adds a pair of expression / diagram to the result.
         *
         * @param el Pair of an expression and a Feynman diagram to append to 
         * the result.
         */
        void push_back(std::pair<csl::Expr, Diagram> &&el) {
            expressions.push_back(std::move(el.first));
            diagrams.push_back(std::move(el.second));
        }
    };

/**
 * @brief Contains all the procedures that simplify amplitudes.
 */
class AmplitudeCalculator {

    public:

    enum Mode {
        Ampl,
        Square,
        Coef
    };

    using TermType = Lagrangian::TermType;

    AmplitudeCalculator(
            csl::vector_expr const& fieldInsertions,
            bool                    initMomentum = true
            );

    AmplitudeCalculator(
            csl::vector_expr         const& fieldInsertions,
            std::vector<csl::Tensor> const& momentum
            );
    AmplitudeCalculator(
            std::vector<QuantumField> const& fieldInsertions,
            std::vector<csl::Tensor>  const& momentum
            );

    AmplitudeCalculator(
            csl::vector_expr      const& fieldInsertions,
            std::vector<TermType> const& terms,
            bool                          initMomentum = true);

    AmplitudeCalculator(
            csl::vector_expr         const& fieldInsertions,
            std::vector<TermType>    const& terms,
            std::vector<csl::Tensor> const& momentum
            );

    AmplitudeCalculator(
            std::vector<mty::QuantumField> const& fieldInsertions,
            std::vector<TermType>          const& terms,
            std::vector<csl::Tensor>       const& momentum
            );

    AmplitudeCalculator(
            std::vector<mty::QuantumField> const& fieldInsertions,
            std::vector<FeynmanRule>       const& terms,
            std::vector<csl::Tensor>       const& momentum
            );

    AmplitudeCalculator(
            csl::vector_expr         const& fieldInsertions,
            std::vector<TermType>    const& terms,
            csl::Expr                     const& LSZInsertions,
            std::vector<csl::Tensor> const& momentum
            );

    void setRuleMode(bool t_ruleMode);

    void setInteractionTerms(
            std::vector<TermType> const& terms);

    void setFeynmanRules(
            std::vector<FeynmanRule> const& rules);

    csl::Expr getResult();

    Amplitude getDiagrams();

    static bool findScalars(csl::Expr& init);

    void simplifyImpulsions(csl::Expr& init);

    void tryMomentumConservation(csl::Expr& ampl,
                                 csl::Expr const& momentumSum);

    static
    void applyDimensionalRegularization(csl::Expr& res);

    /**
     * @brief Applies the Dirac equation on a product containing expanded gamma
     * chains (with explicit external spinors).
     *
     * @param prod  Product containing gamma amtrices and external spinors
     * @param p     Momentum to simplify
     * @param m     Mass of the particle 
     * @param alpha Index of the external spinor
     * @param sign  True if the particle is a particle, false if it is an
     * anti-particle.
     *
     * @return 1 if the Dirac equation has been applied.
     * @return 0 if the momentum has been found in the \f$ \slashed{p} \f$ form
     * but could not be simplified (requires Fierz identity).
     * @return -1 else.
     */
    int doApplyEOM(
            csl::Expr              &prod,
            csl::Tensor       &p,
            csl::Expr        const &m,
            csl::Index         alpha,
            bool               sign
            );

    void applyEOM(csl::Expr &res);

    void applyEOM(
            csl::Expr                   &res,
            std::vector<FermionEOMData> &onShellFermions,
            bool                         fierzBackwards = false
            );

    void simplify(csl::Expr& ampl, Mode mode = Ampl);
        bool expandColorIndices(csl::Expr &res);
        bool simplifyColorWeights(csl::Expr &res);
        bool findColorAbbreviation(csl::Expr& res);
        bool expandMinkoMetric(csl::Expr &res);
        bool simplifyEpsilon(csl::Expr &res);
        bool simplifyEpsilonInProd(
                csl::Expr &prod,
                csl::IndexStructure &indices
                );
        bool expandGammaMatrices(csl::Expr &res) const;
        static bool simplifyConjugationMatrix(csl::Expr &res);
        bool simplifyFermionChains(csl::Expr &res);
        void reduceTensorIntegrals(csl::Expr &res);
        void expandMomenta(csl::Expr &res);
        void expandMomentaExperimental(csl::Expr &res);
        static void expandMomenta(
                csl::Expr &res,
                std::vector<csl::Tensor>             &momentum,
                std::vector<mty::QuantumField> const &insertions
                );
        static void doExpandMomenta(
                csl::Expr &res,
                std::vector<csl::Tensor>             &momentum,
                std::vector<mty::QuantumField> const &insertions
                );
        void addLocalTerms(csl::Expr &res);
        static
        bool factorIndicial(csl::Expr &res);
        void factor(csl::Expr &res);
        static void abbreviateIntegral(csl::Expr &res);
        static void abbreviateAll(csl::Expr &res);

    static
    bool findAbreviations(csl::Expr& res);

    private:

    void initInteractionTerms(
            std::vector<TermType> const& terms);

    void initFieldInsertions(csl::vector_expr fieldInsertions);

    void initVertices();

    void initLSZInsertion();

    void orderInsertions();

    void initMomentumVertices(
            std::map<csl::Tensor, size_t> &vertexIds,
            std::vector<csl::Tensor>      &witnessVertices,
            csl::PseudoIntegral           &integral,
            std::vector<csl::Expr>             &fieldVertices
            );

    void applyMomentumVertices(
            std::vector<csl::Tensor> const &witnessVertices,
            FeynruleMomentum               &momentumMapping,
            csl::Expr                           &amplitude
            );

    csl::IndexStructure momentumStructure(csl::Expr const &node);
    csl::IndexStructure colorStructure(
            csl::Expr const &node,
            color::ColorSpace const *color
            );
    bool expandInProd(
            csl::Expr &prod,
            std::function<csl::IndexStructure(csl::Expr const&)> const &f
            );

    void suppressDiracDelta(
            csl::Expr &amplitude,
            csl::Expr const &PSum
            );

    std::vector<std::vector<size_t>> getExternalSymmetries(
            csl::Expr &fieldProd
            ) const;

    void applyDerivativesInStructure(csl::Expr& expression);

    csl::vector_expr applyExternalSymmetries(
            csl::Expr const& result,
            std::vector<size_t> degenerate) const;

    csl::vector_expr applyAllExternalSymmetries(
            csl::vector_expr const& init,
            std::vector<std::vector<size_t>>::iterator first,
            std::vector<std::vector<size_t>>::iterator last) const;

    void calculateResult();

    void calculateResultWithRules();

    void computeDiracTraces(csl::Expr& res);

    static 
    bool findIntegralAbbreviation(csl::Expr& res);

    static 
    void findExternalAbbreviation(csl::Expr& res);

    private:

    bool ruleMode = false;

    std::optional<csl::Expr> result;

    Amplitude amplitude;

    std::optional<csl::Expr> LSZInsertions;

    std::vector<mty::QuantumField> fieldInsertions;

    std::vector<csl::Tensor> momentum;

    std::vector<csl::Tensor> vertices;

    std::vector<TermType> interactionTerms;

    std::vector<mty::FeynmanRule> feynmanRules;
};

} // End of namespace mty

#endif
