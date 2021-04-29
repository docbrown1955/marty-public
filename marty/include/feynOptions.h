#pragma once

#include <functional>
#include "lagrangian.h"
#include "mrtOptions.h"

namespace mty {

    class InteractionTerm;
    class FeynmanDiagram;
    class FeynmanRule;

    class FeynOptions {

    public:

        using LagrangianFilter 
            = std::function<bool(mty::InteractionTerm const&)>;
        using DiagramFilter 
            = std::function<bool(mty::FeynmanDiagram const&)>;

        FeynOptions();

        bool getFeynRuleCalculation() const { return feynRuleCalculation; }

        int getLoopOrder()      const { return *loopOrder; }

        int getExpansionOrder() const { return expansionOrder; }

        void setFeynmanRuleMode();

        void setLoopOrder(int t_nLoops, int nExternalLegs);

        void applyFilters(
                std::vector<Lagrangian::TermType> &lagrangian
                ) const;

        std::vector<FeynmanRule const*> applyFilters(
                std::vector<FeynmanRule> const &lagrangian
                ) const;

        void applyFilters(
                std::vector<FeynmanDiagram> &diagrams
                ) const;

        void addLagrangianFilter(LagrangianFilter const &filter) {
            addLagrangianFilters({filter});
        }
        void addDiagramFilter(DiagramFilter const &filter) {
            addDiagramFilters({filter});
        }

        template<class Filter>
            void addFilter(Filter &&filter) {
                if constexpr (std::is_constructible_v<LagrangianFilter, Filter>) 
                {
                    addLagrangianFilter(std::forward<Filter>(filter));
                }
                else 
                {
                    addDiagramFilter(std::forward<Filter>(filter));
                }
            }

        void addFilters() {}

        template<class First, class ...Next>
            void addFilters(First &&first, Next &&...next)
            {
                addFilter(std::forward<First>(first));
                addFilters(std::forward<Next>(next)...);
            }

        void resetFilters();

        void addLagrangianFilters(
                std::initializer_list<LagrangianFilter> filters
                );
        void addDiagramFilters(
                std::initializer_list<DiagramFilter> filters
                );

    private:

        template<class Container, class Predicate>
            void applyFilters(
                    Container       &container,
                    Predicate const &predicate
                    ) const
            {
                auto last = std::remove_if(
                        begin(container), end(container), predicate);
                container.erase(last, end(container));
            }

        void initDefaultFilters();

    public:

        bool orderInsertions       { mty::option::applyInsertionOrdering };
        bool orderExternalFermions { mty::option::orderExternalFermions  };
        bool applyDerivatives      { mty::option::applyDerivatives       };
        bool simplifyAmplitudes    { mty::option::simplifyAmplitudes     };
        bool discardLowerOrders    { mty::option::discardLowerOrders     };
        bool verboseAmplitude      { mty::option::verboseAmplitude       };

    private:

        bool                          feynRuleCalculation { false };
        std::shared_ptr<int>          loopOrder;
        int                           expansionOrder;
        std::vector<LagrangianFilter> lfilters;
        std::vector<DiagramFilter>    dfilters;
    };

} // namespace mty
