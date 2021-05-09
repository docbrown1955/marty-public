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

#include <algorithm>
#include "feynOptions.h"
#include "feynmanDiagram.h"
#include "feynmanRule.h"

namespace mty {

    FeynOptions::FeynOptions()
    {
        loopOrder = std::make_shared<int>();
        initDefaultFilters();
    }

    void FeynOptions::setLoopOrder(int t_loopOrder, int nExternalLegs) 
    {
        *loopOrder = t_loopOrder;
        expansionOrder = 2 * (*loopOrder - 1) + nExternalLegs;
    }

    void FeynOptions::setFeynmanRuleMode()
    {
        *loopOrder     = 0;
        expansionOrder = 1;
        feynRuleCalculation = true;
        verboseAmplitude    = false;
    }

    std::vector<FeynmanRule const*> FeynOptions::applyFilters(
            std::vector<FeynmanRule> const &lagrangian
            ) const
    {
        std::vector<FeynmanRule const*> filtered(lagrangian.size());
        for (size_t i = 0; i != lagrangian.size(); ++i) 
            filtered[i] = &lagrangian[i];
        applyFilters(
                filtered,
                [=](FeynmanRule const *term) {
                    return std::any_of(
                            begin(lfilters), end(lfilters), 
                            [&](LagrangianFilter const &f) {
                                return !f(*term->getInteractionTerm());
                            });
                });
        return filtered;
    }

    void FeynOptions::applyFilters(
            std::vector<Lagrangian::TermType> &lagrangian
            ) const
    {
        applyFilters(
                lagrangian,
                [=](Lagrangian::TermType const &term) {
                    return std::any_of(
                            begin(lfilters), end(lfilters), 
                            [&](LagrangianFilter const &f) {
                                return !f(*term);
                            });
                });
    }

    void FeynOptions::applyFilters(
            std::vector<FeynmanDiagram> &diagrams
            ) const
    {
        applyFilters(
                diagrams,
                [=](FeynmanDiagram const &diagram) {
                return std::any_of(
                        begin(dfilters), end(dfilters),
                        [&](DiagramFilter const &d) {
                            return !d(diagram);
                        });
                });
    }

    void FeynOptions::resetFilters()
    {
        lfilters.clear();
        dfilters.clear();
        initDefaultFilters();
    }

    void FeynOptions::addLagrangianFilters(
            std::initializer_list<LagrangianFilter> filters
            )
    {
        for (auto const &f : filters) {
            lfilters.push_back(f);
        }
    }

    void FeynOptions::addDiagramFilters(
            std::initializer_list<DiagramFilter> filters
            )
    {
        for (auto const &f : filters) {
            dfilters.push_back(f);
        }
    }

    void FeynOptions::initDefaultFilters()
    {
        addLagrangianFilter([](InteractionTerm const &term) {
            for (const auto &field : term.getContent())
                if (!field.getQuantumParent()->isEnabledInDiagrams())
                    return false;
            return true;
        });
        addDiagramFilter([=](FeynmanDiagram const &diagram) {
            if (diagram.getNLoops() != *loopOrder) {
                return !(discardLowerOrders || *loopOrder < diagram.getNLoops());
            }
            return true;
        });
    }

} // namespace mty
