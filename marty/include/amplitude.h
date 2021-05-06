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

        Amplitude(
                FeynOptions const &t_options,
                Kinematics  const &t_kinematics
                );

        Amplitude(
                FeynOptions                      const &t_options,
                std::vector<mty::FeynmanDiagram> const &t_diagrams,
                mty::Kinematics                  const &t_kinematics
                );

        Amplitude(
                FeynOptions                 const &t_options,
                std::vector<mty::FeynmanDiagram> &&t_diagrams,
                mty::Kinematics             const &t_kinematics
                );

        bool empty() const {
            return diagrams.empty();
        }

        size_t size() const {
            return diagrams.size();
        }

        std::vector<csl::Expr> obtainExpressions() const;

        std::vector<std::shared_ptr<mty::wick::Graph>> obtainGraphs() const;

        void add(std::vector<mty::FeynmanDiagram> const &t_diagrams);
        void add(std::vector<mty::FeynmanDiagram> &&t_diagrams);

        Amplitude select(
                std::function<bool(mty::FeynmanDiagram const&)> selection
                ) const;

        template<class First, class ...Next,
            typename = std::enable_if_t<(sizeof...(Next) > 1)>
            >
        Amplitude select(
                First &&first,
                Next  &&...next
                ) const
        {
            return select(std::forward<First>(first)).select(
                    std::forward<Next>(next)...
                    );
        }

        FeynOptions const &getOptions() const {
            return options;
        }

        std::vector<mty::FeynmanDiagram> const &getDiagrams() const {
            return diagrams;
        }

        std::vector<mty::FeynmanDiagram>       &getDiagrams()       {
            return diagrams;
        }

        void setDiagrams(std::vector<mty::FeynmanDiagram> const &t_diags) {
            diagrams = t_diags;
        }
        void setDiagrams(std::vector<mty::FeynmanDiagram> &&t_diags) {
            diagrams = std::move(t_diags);
        }

        mty::Kinematics const &getKinematics() const {
            return kinematics;
        }

        csl::Expr getSum() const;

    private:

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
