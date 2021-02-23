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
 
 * \brief
 */
#ifndef EFFMODEL_H_INCLUDED
#define EFFMODEL_H_INCLUDED

#include <vector>
#include <initializer_list>
#include <string>
#include <csl.h>

namespace mty::wick {
class Graph;
}

namespace mty {

class Model;
struct Amplitude;
class Particle;
struct Wilson;

class EffModel {

    public:

    EffModel(mty::Model& t_model,
             std::vector<mty::Particle> const& t_integrated);

    EffModel(mty::Model& t_model,
             std::vector<std::string> const& t_integrated);

    EffModel(mty::Model& t_model,
             std::initializer_list<std::string> const& t_integrated);

    mty::Model* getBase() const;

    std::vector<Particle>& getIntegrated();
    std::vector<Particle> const& getIntegrated() const;

    std::vector<Wilson>& getWilsons();
    std::vector<Wilson> const& getWilsons() const;

    Amplitude computeAmplitude(
            int                              order,
            const csl::vector_expr&          insertions,
            std::vector<csl::Tensor>& impulsions,
            bool                             ruleMode = false) const;

    Amplitude computeAmplitude(
            int                     order,
            const csl::vector_expr& insertions,
            bool                    ruleMode = false) const;

    csl::Expr computeSquaredAmplitude(
            int                              order,
            const csl::vector_expr&          insertions,
            std::vector<csl::Tensor>& impulsions) const;

    std::vector<Wilson> match(
            std::vector<csl::Expr>& effectiveAmplitudes,
            std::vector<csl::Expr>& fullAmplitudes,
            csl::Expr        const& operatorFactor = CSL_1) const;

    csl::Expr getCoefficientForOperator(csl::Expr const& expr) const;

    protected:

    void highMassApproximation(Wilson& wilson) const;

    void highMassApproximation(csl::Expr& init) const;

    void highMassApproximation(csl::Expr      & init,
                               csl::Expr const& M) const;

    void checkInsertions(csl::vector_expr const& insertions) const;

    void mergeDiagramVertices(
            std::vector<std::shared_ptr<wick::Graph>> &diagrams
            ) const;

    void mergeDiagramVertices(
            std::shared_ptr<wick::Graph> &diagram
            ) const;

    void integrateOut(bool value) const;

    public:

    static void addWilson(
            Wilson const& wil,
            std::vector<Wilson> &wilsons,
            bool                 merge = true
            );

    static void mergeWilsons(
            std::vector<Wilson> &wilsons
            );
    
    protected:

    mty::Model *base;
    mutable std::vector<mty::Particle> integrated;
    mutable std::vector<Wilson>        wilsons;
};

}

#endif
