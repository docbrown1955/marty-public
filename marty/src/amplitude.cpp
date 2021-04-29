#include "amplitude.h"

namespace mty {

    Amplitude::Amplitude(
            FeynOptions const &t_options,
            Kinematics  const &t_kinematics
            )
        :options(t_options),
        kinematics(t_kinematics)
    {

    }

    Amplitude::Amplitude(
            FeynOptions                      const &t_options,
            std::vector<mty::FeynmanDiagram> const &t_diagrams,
            mty::Kinematics                  const &t_kinematics
            )
        :options(t_options),
        diagrams(t_diagrams),
        kinematics(t_kinematics)
    {

    }

    Amplitude::Amplitude(
            FeynOptions                 const &t_options,
            std::vector<mty::FeynmanDiagram> &&t_diagrams,
            mty::Kinematics             const &t_kinematics
            )
        :options(t_options),
        diagrams(std::move(t_diagrams)),
        kinematics(t_kinematics)
    {

    }

    std::vector<csl::Expr> Amplitude::obtainExpressions() const
    {
        std::vector<csl::Expr> res(size());
        for (size_t i = 0; i != size(); ++i) {
            res[i] = diagrams[i].getExpression();
        }
        return res;
    }

    std::vector<std::shared_ptr<mty::wick::Graph>> 
        Amplitude::obtainGraphs() const
    {
        std::vector<std::shared_ptr<mty::wick::Graph>> res(size());
        for (size_t i = 0; i != size(); ++i) {
            res[i] = diagrams[i].getDiagram();
        }
        return res;
    }


    void Amplitude::add(std::vector<mty::FeynmanDiagram> const &t_diagrams)
    {
        add(begin(t_diagrams), end(t_diagrams));
    }

    void Amplitude::add(std::vector<mty::FeynmanDiagram> &&t_diagrams)
    {
        add(
            std::make_move_iterator(begin(t_diagrams)),
            std::make_move_iterator(end(t_diagrams))
           );
    }

    Amplitude Amplitude::select(
            std::function<bool(mty::FeynmanDiagram const&)> selection
            ) const
    {
        std::vector<mty::FeynmanDiagram> selected;
        selected.reserve(diagrams.size());
        std::for_each(
                begin(diagrams), end(diagrams), 
                [&](FeynmanDiagram const& diag) {
                    if (selection(diag))
                        selected.push_back(diag);
                });
        return {options, std::move(selected), kinematics};
    }

    csl::Expr Amplitude::getSum() const 
    {
        std::vector<csl::Expr> terms(diagrams.size());
        for (size_t i = 0; i != terms.size(); ++i) 
            terms[i] = diagrams[i].getExpression();
        return csl::sum_s(terms);
    }

} // namespace mty
