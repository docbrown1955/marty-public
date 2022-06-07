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

    csl::Expr &Amplitude::expression(std::size_t pos)
    {
        HEPAssert(pos < diagrams.size(),
                mty::error::IndexError,
                "Index " + std::to_string(pos) + " out of bounds of amplitude "
                "of size " + std::to_string(size()) + ".")
        return diagrams[pos].getExpression();
    }

    csl::Expr const &Amplitude::expression(std::size_t pos) const
    {
        HEPAssert(pos < diagrams.size(),
                mty::error::IndexError,
                "Index " + std::to_string(pos) + " out of bounds of amplitude "
                "of size " + std::to_string(size()) + ".")
        return diagrams[pos].getExpression();
    }

    FeynmanDiagram::diagram_t &Amplitude::diagram(std::size_t pos)
    {
        HEPAssert(pos < diagrams.size(),
                mty::error::IndexError,
                "Index " + std::to_string(pos) + " out of bounds of amplitude "
                "of size " + std::to_string(size()) + ".")
        return diagrams[pos].getDiagram();
    }

    FeynmanDiagram::diagram_t const &Amplitude::diagram(std::size_t pos) const
    {
        HEPAssert(pos < diagrams.size(),
                mty::error::IndexError,
                "Index " + std::to_string(pos) + " out of bounds of amplitude "
                "of size " + std::to_string(size()) + ".")
        return diagrams[pos].getDiagram();
    }

    std::vector<csl::Expr> Amplitude::obtainExpressions() const
    {
        std::vector<csl::Expr> res(size());
        for (std::size_t i = 0; i != size(); ++i) {
            res[i] = diagrams[i].getExpression();
        }
        return res;
    }

    std::vector<std::shared_ptr<mty::wick::Graph>> 
        Amplitude::obtainGraphs() const
    {
        std::vector<std::shared_ptr<mty::wick::Graph>> res(size());
        for (std::size_t i = 0; i != size(); ++i) {
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

    Amplitude Amplitude::filterOut(
            std::function<bool(mty::FeynmanDiagram const&)> filter
            ) const
    {
        std::vector<mty::FeynmanDiagram> selected;
        selected.reserve(diagrams.size());
        std::for_each(
                begin(diagrams), end(diagrams), 
                [&](FeynmanDiagram const& diag) {
                    if (filter(diag))
                        selected.push_back(diag);
                });
        return {options, std::move(selected), kinematics};
    }

    csl::Expr Amplitude::getSum() const 
    {
        std::vector<csl::Expr> terms(diagrams.size());
        for (std::size_t i = 0; i != terms.size(); ++i) 
            terms[i] = diagrams[i].getExpression();
        return csl::sum_s(terms);
    }

    Amplitude Amplitude::copy() const
    {
        std::vector<FeynmanDiagram> diagramCopy;
        diagramCopy.reserve(diagrams.size());
        for (std::size_t i = 0; i != diagrams.size(); ++i) {
            diagramCopy.emplace_back(diagrams[i].copy());
        }
        return Amplitude { options, diagramCopy, kinematics };
    }

    void Amplitude::setKinematics(Kinematics const &t_kinematics, bool replace)
    {
        if (replace) {
            for (auto &diag : diagrams) {
                Kinematics::replace(diag.getExpression(), kinematics, t_kinematics);
            }
        }
        kinematics = t_kinematics;
    }

} // namespace mty
