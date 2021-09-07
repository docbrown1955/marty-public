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
 * @file amplitude.h
 *
 * @brief Contains the mty::Amplitude object that stores the data of an 
 * amplitude calculation.
 *
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-05
 */
#pragma once

#include "feynmanDiagram.h"
#include "kinematics.h"

namespace mty {

    /**
     * @brief Interface class containing the result of an amplitude calculation.
     *
     * @details This is the return type of the mty::Model::computeAmplitude()
     * member. It contains the mty::FeynmanDiagram objects corresponding to the
     * amplitude, the options that have been used during the calculation and the
     * mty::Kinematics objects corresponding to the external particles layout.
     */
    class Amplitude {

    public:

        friend class AmplitudeInitializer;

        /**
         * @brief Initializes an amplitude with no diagram (to be added later 
         * using Amplitude::add()).
         *
         * @param t_options    Options used to calculate the amplitude.
         * @param t_diagrams   Range of diagrams.
         * @param t_kinematics Kinematics of the process.
         */
        Amplitude(
                FeynOptions const &t_options,
                Kinematics  const &t_kinematics
                );

        /**
         * @brief Initializes the amplitude completely.
         *
         * @param t_options    Options used to calculate the amplitude.
         * @param t_diagrams   Range of diagrams.
         * @param t_kinematics Kinematics of the process.
         */
        Amplitude(
                FeynOptions                      const &t_options,
                std::vector<mty::FeynmanDiagram> const &t_diagrams,
                mty::Kinematics                  const &t_kinematics
                );

        /**
         * @brief Initializes the amplitude completely.
         *
         * @param t_options    Options used to calculate the amplitude.
         * @param t_diagrams   Set of diagrams, moved range that is invalidated 
         * after the call of this constructor (at call site).
         * @param t_kinematics Kinematics of the process.
         */
        Amplitude(
                FeynOptions                 const &t_options,
                std::vector<mty::FeynmanDiagram> &&t_diagrams,
                mty::Kinematics             const &t_kinematics
                );

        /**
         * @return \b True if the amplitude is empty (equal to zero).
         * @return \b False else.
         */
        bool empty() const {
            return diagrams.empty();
        }

        /**
         * @return The size of the amplitude i.e. the number of diagrams / 
         * expressions.
         */
        size_t size() const {
            return diagrams.size();
        }

        /**
         * @brief Returns a reference to expression of the diagram at a given 
         * position.
         *
         * @details This function returns a reference allowing the caller to 
         * modify directly in place the expression. For example swapping the 
         * sign of the second diagram: 
         * \code
         *      amplitude.expression(1) *= -1;
         * \endcode
         * This function can then be used to apply any necessary modifications
         * to the amplitude before going further, calculating Wilson 
         * coefficients or squaring the amplitude for example.
         *
         * @param pos Position of the diagram.
         *
         * @return The expression of the diagram number \b pos (starts at 0).
         */
        csl::Expr       &expression(size_t pos);

        /**
         * @brief Returns the const expression of the diagram at a given pos.
         *
         * @param pos Position of the diagram.
         *
         * @return The expression of the diagram number \b pos (starts at 0).
         */
        csl::Expr const &expression(size_t pos) const;

        /**
         * @brief Returns a reference to the graph layout of the diagram at a 
         * given position
         *
         * @param pos Position of the diagram.
         *
         * @return The graph layout of the diagram at \b pos.
         */
        FeynmanDiagram::diagram_t       &diagram(size_t pos);

        /**
         * @brief Returns a const reference to the graph layout of the diagram 
         * at a given position
         *
         * @param pos Position of the diagram.
         *
         * @return The graph layout of the diagram at \b pos.
         */
        FeynmanDiagram::diagram_t const &diagram(size_t pos) const;

        /**
         * @brief Creates and return a range containing the expressions of all 
         * diagrams.
         *
         * @return A std::vector containing all diagrams' expressions.
         */
        std::vector<csl::Expr> obtainExpressions() const;

        /**
         * @brief Creates and return a range containing the graph layouts of all 
         * diagrams.
         *
         * @return A std::vector containing all diagrams' graph layouts.
         */
        std::vector<std::shared_ptr<mty::wick::Graph>> obtainGraphs() const;

        /**
         * @brief Adds diagrams to the amplitude.
         *
         * @param t_diagrams Range of diagrams to be added.
         */
        void add(std::vector<mty::FeynmanDiagram> const &t_diagrams);

        /**
         * @brief Adds diagrams to the amplitude.
         *
         * @param t_diagrams Moved range of diagrams to be added. The parameter
         * passed to the function is invalid after the call.
         */
        void add(std::vector<mty::FeynmanDiagram> &&t_diagrams);

        /**
         * @brief Applies a diagram filter (mty::FeynOptions::DiagramFilter
         * requirement) to the amplitude and returns the result.
         *
         * @details This function can be used with built-in filters (see
         * file filters.h) or custom filters. For example, selecting only
         * the contribution where the combination c and b quark appears if one
         * wants to test \f$ V_{cb} \f$:
         * \code
         *      auto ampl_Vcb = amplitude.filter_out(
         *           mty::filter::forceParticleCombination({"b", "c"})
         *      );
         * \endcode
         * The other overload allows one to call this function with an arbitrary
         * number of filters.
         *
         * @note This function is meant to separate contributions from a result
         * that must be used entirely elsewhere. If one wants only one specific 
         * contribution at all, consider using filters, through the 
         * mty::FeynOptions object passed as a parameter to the amplitude 
         * calculation, as this saves unnecessary calculations.
         *
         * @param filter Filter that should return \b true if the diagram must
         * be selected and copied in the new Amplitude.
         *
         * @return A copy of the amplitude containing only the selected 
         * diagrams.
         */
        Amplitude filterOut(
                std::function<bool(mty::FeynmanDiagram const&)> filter
                ) const;

        /**
         * @brief Applies an arbitrary number of filters on an amplitude.
         *
         * @tparam First    First filter type.
         * @tparam ...Next  Next filter types.
         * @tparam typename Constraining that there is at least 2 filters.
         * @param first   First filter.
         * @param ...next Next filters. 
         *
         * @return The filtered amplitude containing only the diagrams that pass
         * all filters.
         */
        template<class First, class ...Next,
            typename = std::enable_if_t<(sizeof...(Next) > 1)>
            >
        Amplitude filterOut(
                First &&first,
                Next  &&...next
                ) const
        {
            return filterOut(std::forward<First>(first)).filterOut(
                    std::forward<Next>(next)...
                    );
        }

        /**
         * @brief Returns the options used to calculate the amplitude;
         *
         * @return #options.
         */
        FeynOptions const &getOptions() const {
            return options;
        }

        /**
         * @brief Returns all the mty::FeynmanDiagram objects as a const 
         * reference to a range.
         *
         * @return #diagrams.
         */
        std::vector<mty::FeynmanDiagram> const &getDiagrams() const {
            return diagrams;
        }

        /**
         * @brief Returns all the mty::FeynmanDiagram objects as a 
         * reference to a range.
         *
         * @return #diagrams.
         */
        std::vector<mty::FeynmanDiagram>       &getDiagrams()       {
            return diagrams;
        }

        /**
         * @brief Returns the kinematics of the process.
         *
         * @return #kinematics.
         */
        mty::Kinematics const &getKinematics() const {
            return kinematics;
        }

        /**
         * @brief Returns the kinematics of the process.
         *
         * @return #kinematics.
         */
        mty::Kinematics &getKinematics() {
            return kinematics;
        }

        /**
         * @brief Computes and returns the sum of all diagrams i.e. the total 
         * amplitude.
         *
         * @return The sum of all the diagrams' expressions.
         */
        csl::Expr getSum() const;

        /**
         * @brief Returns a copy of the amplitude.
         *
         * @details This function creates a deep copy ensuring that no object,
         * symbolic expressions included, is shared between the copy and the
         * initial object. Building blocks are still shared as e.g. momenta and
         * squared momenta.
         *
         * @return The copy of the amplitude.
         */
        Amplitude copy() const;

        /**
         * @brief Replaces one kinematic context by another, replacing the 
         * relevant momenta in expressions.
         *
         * @param t_kinematics New kinematic context for the process.
         */
        void setKinematics(
                Kinematics const &t_kinematics,
                bool              replace = true);

    private:

        /**
         * @brief Template utility to add multiple diagrams at once.
         *
         * @tparam Iterator Iterator type for the range that must be inserted.
         * This type must point to mty::FeynmanDiagram objects.
         * @param first Begin iterator.
         * @param last  End iterator.
         */
        template<class Iterator>
        void add(Iterator first, Iterator last)
        {
            diagrams.insert(end(diagrams), first, last);
        }

    private:

        /**
         * @brief Set of options that have been used for the calculation.
         */
        FeynOptions                      options;

        /**
         * @brief Range of mty::FeynmanDiagram of the process. Each diagram 
         * contains a mathematical expression and the corresponding graph (that
         * can be displayed using GRAFED).
         */
        std::vector<mty::FeynmanDiagram> diagrams;

        /**
         * @brief Kinematics of the process.
         */
        mty::Kinematics                  kinematics;
    };

} // namespace mty
