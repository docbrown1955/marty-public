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
 * @file feynOptions.h
 *
 * @brief Contains the FeynOptions object used by the mty::Model class to
 * set calculation options.
 *
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2021-05-05
 */
#pragma once

#include "lagrangian.h"
#include "mrtOptions.h"
#include "operatorBasis.h"
#include "topology.h"
#include <functional>

namespace mty {

class InteractionTerm;
class FeynmanDiagram;
class FeynmanRule;

/**
 * @brief Instances of this class can be given to mty::Model when launching
 * a calculation to customize the output.
 */
class FeynOptions {

  public:
    /**
     * @brief Type definition for a Lagrangian filter.
     *
     * @details A Lagrangian filter is a boolean predicate (function,
     * lambda, ...) taking a mty::InteractionTerm as parameter (const
     * reference) and returning true if the vertex must be kept.
     */
    using LagrangianFilter = std::function<bool(mty::InteractionTerm const &)>;
    /**
     * @brief Type definition for a Feynman diagram.
     *
     * @details A Feynman diagram filter is a boolean predicate (function,
     * lambda, ...) taking a mty::FeynmanDiagram as parameter (const
     * reference) and returning true if the diagram must be kept.
     */
    using DiagramFilter = std::function<bool(mty::FeynmanDiagram const &)>;

    /**
     * @brief Default constructor, only constructor of this class (expect
     * copy and move).
     *
     * @details This constructor initializes default Lagrangian and
     * Feynman diagram filters used in all calculations.
     */
    FeynOptions();

    /**
     * @return #feynRuleCalculation
     */
    bool getFeynRuleCalculation() const
    {
        return feynRuleCalculation;
    }

    /**
     * @return The value pointed by #loopOrder
     */
    int getLoopOrder() const
    {
        return *loopOrder;
    }

    /**
     * @return #expansionOrder
     */
    int getExpansionOrder() const
    {
        return expansionOrder;
    }

    /**
     * @return #topology
     */
    int getTopology() const
    {
        return topology;
    }

    /**
     * @return #fermionOrder
     */
    std::vector<int> const &getFermionOrder() const
    {
        return fermionOrder;
    }

    /**
     * @return #lfilters
     */
    std::vector<LagrangianFilter> const &getLagrangianFilters() const
    {
        return lfilters;
    }

    /**
     * @return #dfilters
     */
    std::vector<DiagramFilter> const &getDiagramFilters() const
    {
        return dfilters;
    }

    /**
     * @return #wilsonOperatorCoefficient
     */
    csl::Expr getWilsonOperatorCoefficient() const
    {
        return wilsonOperatorCoefficient;
    }

    /**
     * @return #wilsonOperatorBasis
     */
    OperatorBasis getWilsonOperatorBasis() const
    {
        return wilsonOperatorBasis;
    }

    /**
     * @brief Sets the value of #feynRuleCalculation to true (the default
     * value of this member is false).
     *
     * @details This function will probably not be useful for user-level
     * manipulations.
     */
    void setFeynmanRuleMode();

    /**
     * @brief Sets the number of loops for the calculation.
     *
     * @details This function needs not only the loop order, but also the
     * number of external legs in the process as it will determine, together
     * with the number of loop, the Taylor expansion order of the
     * Lagrangian exponential.
     *
     * @param t_nLoops      Number of loops wanted (basically 0 or 1).
     * @param nExternalLegs Number of external legs of the process.
     */
    void setLoopOrder(int t_nLoops, int nExternalLegs);

    /**
     * @brief Sets the topology for the calculation.
     *
     * @param t_topology Topologies allowed.
     *
     * @sa mty::Topology
     */
    void setTopology(int t_topology);

    /**
     * @brief Sets the fermion order used to express the amplitude.
     *
     * @param order Fermion order to apply, range of integers from \f$ 0 \f$
     * to \f$ N-1 \f$ for \f$ N \f$ external fermions in the process.
     *
     * @sa #fermionOrder
     */
    void setFermionOrder(std::vector<int> const &order);

    /**
     * @brief Sets the Wilson operator coefficient
     * #wilsonOperatorCoefficient.
     *
     * @param factor Factor used in Wilson coefficient decomposition.
     */
    void setWilsonOperatorCoefficient(csl::Expr const &factor);

    /**
     * @brief Tells if a diagram passes all diagram filters.
     *
     * @param diagram
     *
     * @return
     */
    bool passFilters(FeynmanDiagram const &diagram) const;

    /**
     * @brief Applies the Lagrangian filters on a vector of vertices.
     *
     * @details The vector must contain mty::Lagrangian::TermType objects
     * even if the filters themselves take mty::InteractionTerm. This
     * function removes from the vector all interaction terms for which
     * one of the filters returns false (filter out).
     *
     * @param lagrangian Lagrangian Set of interaction terms to filter.
     *
     * @sa #lfilters, addFilter(), addFilters()
     */
    void applyFilters(std::vector<Lagrangian::TermType> &lagrangian) const;

    /**
     * @brief Filters a set a Feynman rules using the lagrangian filters.
     *
     * @details This function uses the interaction term represented by the
     * different rules and applies the Lagrangian filters. Unlike the other
     * filtering functions, this one does not remove the elements inside the
     * initial vector but returns a newly constructed vector containing
     * pointers to the rules that passed all filters (i.e. for which all
     * filters returned true). This allows to avoid any copy of a
     * FeynmanRule object that is a high cost operation.
     *
     * @param lagrangian Set of Feynman rules (interpreted here as a
     * lagrangian with respect to filters).
     *
     * @return A vector containing all pointers to FeynmanRule objects that
     * passed the filters.
     *
     * @sa #lfilters, addFilter(), addFilters()
     */
    std::vector<FeynmanRule const *>
    applyFilters(std::vector<FeynmanRule> const &lagrangian) const;

    /**
     * @brief Applies the diagram filters on a vector of Feynman diagrams.
     *
     * @details This function removes from the vector all diagrams
     * for which one of the filters returns false (filter out).
     *
     * @param diagrams     Set of diagrams to filter.
     * @param forceFilters Boolean that ensures (if true) that the filters
     * are applied whatever are the other options (#partialCalculation in
     * particular).
     *
     * @sa #dfilters, addFilter(), addFilters()
     */
    void applyFilters(std::vector<FeynmanDiagram> &diagrams,
                      bool                         forceFilters = false) const;

    /**
     * @brief Adds one filter (LagrangianFilter or DiagramFilter) to the
     * options.
     *
     * @details Depending on the type (deduced at compile-time) of the
     * filter, this function delegates to addLagrangianFilter() or
     * addDiagramFilter().
     *
     * @tparam Filter Type of the filter given as parameter.
     * @param filter  User-defined predicate respecting the requirements
     * of LagrangianFilter or DiagramFilter types.
     *
     * @sa addFilters()
     */
    template <class Filter>
    void addFilter(Filter &&filter)
    {
        if constexpr (std::is_constructible_v<LagrangianFilter, Filter>) {
            addLagrangianFilter(std::forward<Filter>(filter));
        }
        else {
            addDiagramFilter(std::forward<Filter>(filter));
        }
    }

    /**
     * @brief Overloads to add 0 filter (this function does nothing).
     *
     * @details This function represents the end-point of the compile-time
     * recursion of the templated overload.
     */
    void addFilters()
    {
    }

    /**
     * @brief Adds an arbitrary number of filters (Lagrangian or diagram
     * filters) to the options.
     *
     * @details This function accepts an arbitrary number of arguments
     * provided that each of them is either a LagrangianFilter or a
     * DiagramFilter. The compiler deduces automatically the types and the
     * function can be used as simply as
     * \code
     *      FeynOptions options;
     *      options.addFilter(
     *          // diagram filter:
     *          mty::filters::forceParticle("t"), // only top quark contrib
     *          // Lagrangian filter:
     *          mty::filters::disableParticle("Z"), // no Z boson
     *          // Another diagram filter:
     *          mty::filters::disableParticleCombination({"e", "mu"}) //
     * Disables combinations of e and mu in the same diagram
     *      );
     * \endcode
     *
     * @tparam First   Type of the first filter given.
     * @tparam ...Next Types of the next filters.
     * @param first    First filter.
     * @param ...next  Next filters.
     */
    template <class First, class... Next>
    void addFilters(First &&first, Next &&...next)
    {
        addFilter(std::forward<First>(first));
        addFilters(std::forward<Next>(next)...);
    }

    /**
     * @brief Resets the filters (for lagrangian and diagrams) to the
     * default ones, i.e. removing all user-defined ones.
     */
    void resetFilters();

  protected:
    /**
     * @brief Adds one Lagrangian filter to the options.
     *
     * @details This function delegates to the other overload that takes
     * an arbitrary number of filters.
     *
     * @param filter Unique filter to add.
     */
    void addLagrangianFilter(LagrangianFilter const &filter)
    {
        addLagrangianFilters({filter});
    }
    /**
     * @brief Adds one diagram filter to the options.
     *
     * @details This function delegates to the other overload that takes
     * an arbitrary number of filters.
     *
     * @param filter Unique filter to add.
     */
    void addDiagramFilter(DiagramFilter const &filter)
    {
        addDiagramFilters({filter});
    }
    /**
     * @brief Adds a list of Lagrangian filters to the options.
     *
     * @param filters List of filters.
     */
    void addLagrangianFilters(std::initializer_list<LagrangianFilter> filters);
    /**
     * @brief Adds a list of diagram filters to the options.
     *
     * @param filters List of filters.
     */
    void addDiagramFilters(std::initializer_list<DiagramFilter> filters);

  private:
    /**
     * @brief Helper private class applying a set of filters on a range.
     *
     * @tparam Container Type of the container to filter.
     * @tparam Predicate Predicate type to apply.
     * @param container  Container on which the filter is applied.
     * @param predicate  Inversed filter, must return true if the object
     * must be filtered out.
     */
    template <class Container, class Predicate>
    void applyFilters(Container &container, Predicate const &predicate) const
    {
        auto last
            = std::remove_if(begin(container), end(container), predicate);
        container.erase(last, end(container));
    }

    /**
     * @brief Initializes the default filters.
     */
    void initDefaultFilters();

    /**
     * @brief Sets the Wilson operator basis #wilsonOperatorBasis.
     *
     * @note This method is private because the operator basis is for now
     * fixed to the standard basis, more suitable for identification.
     *
     * @param basis Basis used in Wilson coefficient decomposition.
     */
    void setWilsonOperatorBasis(OperatorBasis basis);

  public:
    /**
     * @brief This option (default = true) implies that fermionic insertions
     * are ordered using a given rule.
     *
     * @details This only affects the absolute signs
     * of different diagrams in a process and is then physically irrelevant.
     */
    bool orderInsertions{mty::option::applyInsertionOrdering};

    bool excludeExternalLegCorrections{
        mty::option::excludeExternalLegsCorrections};

    /**
     * @brief Boolean telling if external fermions must be ordered in the
     * result.
     *
     * @details This option is false by default and should not be set to
     * true except for Wilson coefficient calculations because it requires
     * more calculations and it not relevant when calculating a squared
     * amplitude.
     */
    bool orderExternalFermions{mty::option::orderExternalFermions};

    /**
     * @brief Set this option to false if you want to disable all
     * simplifications of amplitudes.
     *
     * @details Using this option, the result is then much closer to the
     * bare use of Feynman rules and propagators.
     */
    bool simplifyAmplitudes{mty::option::simplifyAmplitudes};

    /**
     * @brief Set this option to false if you want the tree-level diagrams
     * to appear in the same amplitude as the one-loop calculation.
     */
    bool discardLowerOrders{mty::option::discardLowerOrders};

    /**
     * @brief Set this options to false to disable all prints during
     * calculations.
     */
    bool verboseAmplitude{mty::option::verboseAmplitude};

    /**
     * @brief Set this option to true to disable the DiagramFilters if the
     * calculation is only a part of the full calculation.
     */
    bool partialCalculation{false};

  private:
    /**
     * @brief Mode of an amplitude calculation.
     *
     * @details If \b true, the calculation is done to derive Feynman rules,
     * false otherwise. The derivation of Feynman rules is a more involved
     * calculation and is used only in that purpose. Otherwise, Feynman
     * rules are used. This parameter is handled automatically by MARTY when
     * calculating Feynman rules, it should not be set by the user a priori.
     */
    bool feynRuleCalculation{false};

    /**
     * @brief Loop order (0 for tree-level, 1 for one-loop).
     *
     * @details This variable is stored in a shared pointer for a technical
     * reason, but the interface (getter and setter functions) use a
     * standard integer.
     */
    std::shared_ptr<int> loopOrder;

    /**
     * @brief Taylor expansion order for an amplitude calculation (expansion
     * of the lagrangian exponential).
     */
    int expansionOrder;

    /**
     * @brief Topology allowed for the calculation. The default is
     * mty::Topology::Any.
     */
    int topology = mty::Topology::Any;

    /**
     * @brief Order for fermions in bilinears.
     *
     * @details The order of fermions in the result in fixed, useful for
     * Wilson coefficient extraction. For example, a process with for
     * external fermions \f$ \psi_0,\ \psi_1,\ \psi_2 \f$ and \f$ \psi_3 \f$
     * can be expressed with the order \f$ (0123) \f$ with terms of the type
     * \f[
     *     \left(\bar{\psi}_0\Gamma^A\psi_1\right)
     *          \left(\bar{\psi}_1\Gamma^B\psi_2\right,
     * \f]
     * while an order \f$ (2013) \f$ corresponds to something such as
     * \f[
     *     \left(\bar{\psi}_2\Gamma^C\psi_0\right)
     *          \left(\bar{\psi}_1\Gamma^D\psi_3\right,
     * \f]
     * with \f$ \Gamma^{A,B,C,D} \f$ arbitrary \f$ \gamma \f$-matrix
     * combinations.
     */
    std::vector<int> fermionOrder;

    /**
     * @brief List of lagrangian filters.
     *
     * @details Those filters are applied by MARTY before any calculation
     * is done by discarding unwanted interaction terms. This type of filter
     * must be preferred to diagram filters if possible.
     *
     * @sa filters.h for built-in filters
     */
    std::vector<LagrangianFilter> lfilters;

    /**
     * @brief List of diagram filters.
     *
     * @details Those filters are applied by MARTY once the Wick theorem has
     * been applied, i.e. that all possible diagrams have been found. They
     * save less calculation than LagrangianFilter but are more general as
     * one can filter diagrams on more complex conditions.
     *
     * @sa filters.h for built-in filters
     */
    std::vector<DiagramFilter> dfilters;

    /**
     * @brief Prefactor used for Wilson coefficients.
     *
     * @details An amplitude is decomposed on pairs of Wilson coefficients
     * and operators the following way:
     * \f[
     *      i\mathcal{M} \equiv -if\sum _i C_i \hat{\mathcal{O}}_i,
     * \f]
     * with \f$ C_i \f$ the Wilson coefficients, \f$ \hat{\mathcal{O}}_i \f$
     * the operators, and $f$ a convention-dependent factor. This member
     * corresponds to the \f$ f \f$used by MARTY when calculating the
     * \f$ C_i \f$.
     */
    csl::Expr wilsonOperatorCoefficient{CSL_1};

    /**
     * @brief Operator basis for Wilson coefficients.
     *
     * @details The chiral basis uses projectors \f$ P_L,P_R \f$ whereas
     * the standard one uses \f$ \1,\gamma^5 \f$.
     */
    OperatorBasis wilsonOperatorBasis = OperatorBasis::Chiral;

    constexpr static size_t nDefaultDiagramFilters = 1;
};

} // namespace mty
