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

#include "effModel.h"
#include "mrtInterface.h"
#include "model.h"
#include "quantumField.h"
#include "graph.h"
#include "wilson.h"
#include "propagator.h"
#include "SM.h"
#include "amplitude.h"
#include "mrtOptions.h"

using namespace csl;
namespace mty {

EffModel::EffModel(mty::Model& t_model,
                   std::vector<mty::Particle> const& t_integrated)
    :base(&t_model),
    integrated(t_integrated)
{

}

EffModel::EffModel(mty::Model& t_model,
                   std::vector<std::string> const& t_integrated)
    :base(&t_model)
{
    integrated.reserve(t_integrated.size());
    for (const auto& name : t_integrated)
        integrated.push_back(base->getParticle(name));
}

EffModel::EffModel(mty::Model& t_model,
                   std::initializer_list<std::string> const& t_integrated)
    :EffModel(t_model, std::vector<std::string>(t_integrated))
{
    
}

mty::Model* EffModel::getBase() const
{
    return base;
}

std::vector<Particle>& EffModel::getIntegrated() 
{
    return integrated;
}

std::vector<Particle> const& EffModel::getIntegrated() const
{
    return integrated;
}

std::vector<Wilson>& EffModel::getWilsons() 
{
    return wilsons;
}

std::vector<Wilson> const& EffModel::getWilsons() const
{
    return wilsons;
}

Amplitude EffModel::computeAmplitude(
            int                              order,
            csl::vector_expr const&          insertions,
            std::vector<csl::Tensor>& impulsions,
            bool                             ruleMode) const
{
    checkInsertions(insertions);
    integrateOut(true);
    auto res = base->computeAmplitude(
            order, insertions, impulsions, ruleMode);
    mergeDiagramVertices(res.diagrams);
    integrateOut(false);
    return res;
}

Amplitude EffModel::computeAmplitude(
            int                              order,
            csl::vector_expr const&          insertions,
            bool                             ruleMode) const
{
    checkInsertions(insertions);
    integrateOut(true);
    auto res = base->computeAmplitude(
            order, insertions, ruleMode);
    mergeDiagramVertices(res.diagrams);
    integrateOut(false);
    return res;
}

void EffModel::mergeDiagramVertices(
        std::vector<std::shared_ptr<wick::Graph>> &diagrams
        ) const
{
    for (auto &diag : diagrams)
        mergeDiagramVertices(diag);
}

void EffModel::mergeDiagramVertices(
        std::shared_ptr<wick::Graph> &diagram
        ) const
{
    for (const auto &particle : integrated)
        diagram->contractHighMass(particle.get());
}

csl::Expr EffModel::computeSquaredAmplitude(
        int                              order,
        const csl::vector_expr&          insertions,
        std::vector<csl::Tensor>& impulsions) const
{
    checkInsertions(insertions);
    integrateOut(true);
    auto res = base->computeSquaredAmplitude(order, insertions, impulsions);
    integrateOut(false);
    return res;
}

void EffModel::highMassApproximation(Wilson& wilson) const
{
    csl::Expr coef = wilson.coef.getCoefficient();
    highMassApproximation(coef);
    wilson.coef.setCoefficient(coef);
}

void EffModel::highMassApproximation(csl::Expr& init) const
{
    for (const auto& particle : integrated)
        if (init->dependsExplicitlyOn(particle->getMass().get())) {
            highMassApproximation(init, particle->getMass());
        }
}

void EffModel::highMassApproximation(csl::Expr      & init,
                                     csl::Expr const& M) const
{
    std::vector<csl::Expr> expansion
        = csl::InverseTaylorExpand(init, M, 2);
    init = DeepRefreshed(sum_s(expansion));
}

void EffModel::checkInsertions(csl::vector_expr const& insertions) const
{
    auto check = [&](csl::Expr const& tensor)
    {
        for (const auto& integ : integrated)
            HEPAssert(tensor->getParent_info() != integ.get(),
                    mty::error::TypeError,
                    "Integrated particle encountered in insertion: "
                    + std::string(integ->getName()) + ".");
    };

    for (const auto& expr : insertions)
        csl::VisitEachLeaf(expr, [&](csl::Expr const& el)
        {
            if (IsIndicialTensor(el))
                check(el);
        });
}

void EffModel::integrateOut(bool value) const
{
    for (auto& particle : integrated)
        particle->integrateOut(value);
}

void EffModel::addWilson(
        Wilson const& wil,
        std::vector<Wilson> &wilsons,
        bool                 merge
        )
{
    csl::Expr C = wil.coef.getCoefficient();
    if (merge) {
        for (auto& w : wilsons) {
            if (w.op == wil.op) {
                HEPAssert(w.op.getFactor() != CSL_0,
                        mty::error::ValueError,
                        "Zero encountered in operator factor !")
                csl::Expr newCoef = 
                        w.coef.getCoefficient()
                        + (wil.op.getFactor() / w.op.getFactor()) * C;
                csl::Factor(newCoef, true);
                w.coef.setCoefficient(newCoef);
                return;
            }
        }
    }
    wilsons.push_back({WilsonCoefficient(C), wil.op});
}

void EffModel::mergeWilsons(
        std::vector<Wilson> &wilsons
        )
{
    std::vector<size_t> indicesLeft(wilsons.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i;

    std::vector<Wilson> newWilsons;
    newWilsons.reserve(wilsons.size());
    while (!indicesLeft.empty()) {
        size_t i = indicesLeft[0];
        indicesLeft.erase(indicesLeft.begin());
        auto op     = wilsons[i].op;
        auto factor = wilsons[i].op.getFactor();
        std::vector<csl::Expr> coefs;
        coefs.reserve(std::min(size_t(50), wilsons.size()));
        coefs.push_back(wilsons[i].coef.getCoefficient());
        for (size_t j_left = 0; j_left != indicesLeft.size(); ++j_left) {
            size_t j = indicesLeft[j_left];
            if (wilsons[j].op == op) {
                if (factor != wilsons[j].op.getFactor())
                    coefs.push_back(wilsons[j].op.getFactor()/factor 
                            * wilsons[j].coef.getCoefficient());
                else 
                    coefs.push_back(wilsons[j].coef.getCoefficient());
                indicesLeft.erase(indicesLeft.begin() + j_left);
                --j_left;
            }
        }
        csl::Expr C = (coefs.size() == 1) ? 
            coefs[0] : csl::Factored(csl::sum_s(coefs));
        newWilsons.push_back({ C, op });
    }
    wilsons = std::move(newWilsons);
}

std::vector<Wilson> EffModel::match(
        std::vector<csl::Expr>&,
        std::vector<csl::Expr>& fullAmplitudes,
        csl::Expr        const& operatorFactor) const
{
    csl::Abbrev::enableGenericEvaluation("Fc");
    csl::Abbrev::enableGenericEvaluation("EXT");
    for (auto& ampl : fullAmplitudes) {
        csl::ForEachNode(ampl, [&](csl::Expr &expr)
        {
            if (IsOfType<Propagator>(expr)) {
                csl::Expr m = expr[1];
                expr[1] = CSL_0;
                expr = Evaluated(expr, csl::eval::abbreviation);
                RenameIndices(expr);
                expr = 1 / (1 / expr - m*m);
                csl::DeepExpand(expr);
                HEPAssert(expr != CSL_INF,
                        mty::error::ValueError,
                        "Pole encountered of mass " + toString(m))
            }
        });
        Evaluate(ampl);
        // highMassApproximation(ampl);
    }
    csl::Abbrev::disableGenericEvaluation("Fc");
    csl::Abbrev::disableGenericEvaluation("EXT");

    std::vector<Wilson> fullWilson;
    for (size_t i = 0; i != fullAmplitudes.size(); ++i) {
        std::vector<Wilson> newWilsons = parseExpression(
                DeepCopy(fullAmplitudes[i]),
                operatorFactor
                );
        for (auto &w : newWilsons) {
            // highMassApproximation(w);
            w.coef.setCoefficient(
                    csl::Factored(w.coef.getCoefficient())
                    );
            addWilson(w, fullWilson);
        }
    }

    return fullWilson;
}

csl::Expr EffModel::getCoefficientForOperator(csl::Expr const& expr) const
{
    std::vector<Wilson> w1 = parseExpression(expr);
    HEPAssert(w1.size() == 1,
            mty::error::RuntimeError,
            "Expecting a unique operator to get its coefficient, "
            + toString(expr) + " given.")
    for (const auto& w2 : wilsons)
        if (w1[0].op == w2.op)
            return w2.coef.getCoefficient();
    return CSL_0;
}

}
