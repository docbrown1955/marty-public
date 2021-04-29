#pragma once

#include "feynmanDiagram.h"
#include "kinematics.h"

namespace mty {

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

        FeynOptions                      options;
        std::vector<mty::FeynmanDiagram> diagrams;
        mty::Kinematics                  kinematics;
    };

} // namespace mty
