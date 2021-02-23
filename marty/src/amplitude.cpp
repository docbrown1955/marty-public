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
#include "scalarDot.h"
#include "colorSpace.h"
#include "propagator.h"
#include "mrtOptions.h"
#include "graph.h"
#include "polarization.h"
#include "wick.h"
#include "mrtlog.h"
#include "feynruleMomentum.h"
#include "fermionChain.h"
#include "diracology.h"
#include "feynmanIntegral.h"
#include "momentumConservation.h"
#include "mrtInterface.h"
#include "wilson.h"
#include "fierz.h"

using namespace csl;
namespace mty {


///////////////////////////////////////////////////
/*************************************************/
// Class AmplitudeCalculator                     //
/*************************************************/
///////////////////////////////////////////////////

AmplitudeCalculator::AmplitudeCalculator(
        csl::vector_expr const& t_fieldInsertions,
        bool                    initMomentum)
    :result(std::nullopt),
    LSZInsertions(std::nullopt)
{
    initFieldInsertions(t_fieldInsertions);
    initVertices();

    if (initMomentum) {
        momentum.reserve(fieldInsertions.size());
        for (size_t i = 0; i != fieldInsertions.size(); ++i)
            momentum.push_back(csl::tensor_s("p_" + toString(i),
                                             &csl::Minkowski));
    }
}

AmplitudeCalculator::AmplitudeCalculator(
        csl::vector_expr          const& t_fieldInsertions,
        std::vector<csl::Tensor> const& t_momentum)
    :AmplitudeCalculator(t_fieldInsertions, false)
{
    momentum = t_momentum;
}

AmplitudeCalculator::AmplitudeCalculator(
        std::vector<QuantumField> const& t_fieldInsertions,
        std::vector<csl::Tensor>  const& t_momentum)
    :fieldInsertions(t_fieldInsertions),
    momentum(t_momentum)
{

}

AmplitudeCalculator::AmplitudeCalculator(
        csl::vector_expr                   const& t_fieldInsertions,
        std::vector<TermType> const& t_interactionTerms,
        bool                                      initMomentum)
    :result(std::nullopt),
    LSZInsertions(std::nullopt)
{
    initFieldInsertions(t_fieldInsertions);
    initInteractionTerms(t_interactionTerms);
    initVertices();

    if (initMomentum) {
        momentum.reserve(fieldInsertions.size());
        for (size_t i = 0; i != fieldInsertions.size(); ++i)
            momentum.push_back(csl::tensor_s("p_" + toString(i),
                                             &csl::Minkowski));
    }
}

AmplitudeCalculator::AmplitudeCalculator(
        std::vector<mty::QuantumField>     const& t_fieldInsertions,
        std::vector<TermType> const& t_interactionTerms,
        std::vector<csl::Tensor>          const& t_momentum)
    :result(std::nullopt),
    LSZInsertions(std::nullopt),
    fieldInsertions(t_fieldInsertions)
{
    initInteractionTerms(t_interactionTerms);
    initVertices();
    momentum = t_momentum;
}

AmplitudeCalculator::AmplitudeCalculator(
        std::vector<mty::QuantumField> const& t_fieldInsertions,
        std::vector<FeynmanRule>       const& t_interactionTerms,
        std::vector<csl::Tensor>      const& t_momentum)
    :result(std::nullopt),
    LSZInsertions(std::nullopt),
    fieldInsertions(t_fieldInsertions)
{
    setFeynmanRules(t_interactionTerms);
    momentum = t_momentum;
}


AmplitudeCalculator::AmplitudeCalculator(
        csl::vector_expr                  const& t_fieldInsertions,
        std::vector<TermType> const& t_interactionTerms,
        std::vector<csl::Tensor>          const& t_momentum)
    :AmplitudeCalculator(t_fieldInsertions, t_interactionTerms, false)
{
    momentum = t_momentum;
}

AmplitudeCalculator::AmplitudeCalculator(
        csl::vector_expr                  const& t_fieldInsertions,
        std::vector<TermType> const& t_interactionTerms,
        csl::Expr                               const& t_LSZInsertion,
        std::vector<csl::Tensor>          const& t_momentum)
    :AmplitudeCalculator(t_fieldInsertions, t_interactionTerms, false)
{
    LSZInsertions = t_LSZInsertion;
    momentum = t_momentum;
}

void AmplitudeCalculator::setRuleMode(bool t_ruleMode)
{
    result = std::nullopt;
    ruleMode = t_ruleMode;
}

void AmplitudeCalculator::setInteractionTerms(
        std::vector<TermType> const& t_terms)
{
    result = std::nullopt;
    initInteractionTerms(t_terms);
    initVertices();
}

void AmplitudeCalculator::setFeynmanRules(
        std::vector<FeynmanRule> const& rules)
{
    result = std::nullopt;
    feynmanRules = rules;
    initVertices();
}

csl::Expr AmplitudeCalculator::getResult()
{
    if (not result)
        calculateResult();

    return result.value();
}

Amplitude AmplitudeCalculator::getDiagrams()
{
    if (not result)
        calculateResult();

    return amplitude;
}

std::optional<csl::Index> getUniqueMinkoIndex(csl::Expr const& expr)
{
    if (expr->getType() != csl::Type::TDerivativeElement) {
        std::optional<csl::Index> index;
        bool found = false;
        csl::IndexStructure structIndex = expr->getIndexStructure();
        if (structIndex.size() > 1 
                and not IsOfType<QuantumField>(expr)
                and not IsOfType<PolarizationField>(expr))
            return std::nullopt;
        for (size_t s = 0; s != structIndex.size(); ++s)
            if (structIndex[s].getSpace() == &csl::Minkowski) {
                if (found) 
                    return std::nullopt;
                found = true;
                index = structIndex[s];
            }
        return index;
    }
    return std::nullopt;
}

bool AmplitudeCalculator::findScalars(csl::Expr& init)
{
    bool found = false;
    if (init->getType() == csl::Type::Prod) {
        const size_t size = init->size();
        for (size_t i = 0; i+1 < size; ++i) {
            csl::Expr& arg_i = (*init)[i];
            std::optional<csl::Index> index_i = getUniqueMinkoIndex(arg_i);
            if (not index_i or not arg_i->getCommutable())
                continue;
            for (size_t j = i+1; j < size; ++j) {
                csl::Expr& arg_j = (*init)[j];
                std::optional<csl::Index> index_j = getUniqueMinkoIndex(arg_j);
                if (not index_j or not arg_j->getCommutable())
                    continue;
                if (index_i.value() == index_j.value()) {
                    arg_i = scalardot_s(arg_i, arg_j);
                    arg_j = CSL_1;
                    found = true;
                    break;
                }
            }
        }
    }
    return found;
}

auto findWard(csl::Expr& init,
              csl::AbstractParent const* momentum,
              csl::AbstractParent const *polar)
{

    if (IsOfType<ScalarDot>(init)) {
        auto removeMomentum = [&](csl::Expr& expr) {
          if (expr->getType() == csl::Type::TensorElement
                  and expr->getParent_info() == momentum) {
             expr = CSL_0;
             return true;
          }
          return false;
        };
        csl::Expr &arg0 = (*init)[0];
        csl::Expr &arg1 = (*init)[1];
        bool found = false;
        if (IsOfType<PolarizationField>(arg0)
                and polar == arg0->getParent_info()
                and arg0->getPoint().get() == momentum) {
            if (csl::Transform(arg1, removeMomentum))
                found = true;
        }
        if (IsOfType<PolarizationField>(arg1)
                and polar == arg1->getParent_info()
                and arg1->getPoint().get() == momentum) {
            if (csl::Transform(arg0, removeMomentum))
                found = true;
        }
        return found;
    }
    return false;
}

static bool compareFields(QuantumField const &A, QuantumField const &B)
{
    if (A.isBosonic() != B.isBosonic())
        return A.isBosonic();
    if (A.isBosonic())
        return A.getSpinDimension() < B.getSpinDimension();
    return (!A.isOnShell() && B.isOnShell());
}

void AmplitudeCalculator::simplifyImpulsions(csl::Expr& init)
{
    csl::Expr momentumSum = CSL_0;
    csl::Index mu = csl::Minkowski.generateIndex();
    size_t mini = 0;
    for (size_t i = 1; i != fieldInsertions.size(); ++i) {
        if (compareFields(fieldInsertions[i], fieldInsertions[mini]))
            mini = i;
    }
    for (size_t i = 0; i != fieldInsertions.size(); ++i) {
        if (i != mini)
            momentumSum += (fieldInsertions[i].isIncoming()) ? 
                momentum[i](mu)
                : -momentum[i](mu);
    }
    if (fieldInsertions[mini].isIncoming())
        momentumSum *= CSL_M_1;
    csl::Replace(
            init,
            momentum[mini],
            momentumSum);
    // Applying eps.p = 0
    // for (size_t i = 0; i != fieldInsertions.size(); ++i) {
    //     if (fieldInsertions[i].getSpinDimension() == 2 
    //             and fieldInsertions[i].getIndexStructureView().size() == 1) {
    //         csl::AbstractParent const*P   = momentum[i].get();
    //         csl::AbstractParent const*eps = fieldInsertions[i].getParent_info();
    //         csl::Transform(
    //                 init, 
    //                 [=](csl::Expr& init) {
    //                     return findWard(init, P, eps);
    //                 });
    //     }
    // }
}

void AmplitudeCalculator::tryMomentumConservation(
        csl::Expr&       init,
        csl::Expr const& momentumSum)
{
    auto tryApply = [&](csl::Expr& init)
    {
        if (IsOfType<ScalarDot>(init)) {
            csl::Expr& arg0 = (*init)[0];
            csl::Expr& arg1 = (*init)[1];
            csl::IndexStructure struct0 = arg0->getIndexStructure();
            csl::IndexStructure struct1 = arg1->getIndexStructure();
            bool simplified = false;

            if (struct0.size() == 1 
                    and struct0[0].getSpace() == &csl::Minkowski) {
                auto sum = Replaced(momentumSum,
                                   momentumSum->getIndexStructure()[0],
                                   struct0[0]);
                if (auto trial = Expanded(arg0) + sum;
                        trial < arg0) {
                    simplified = true;
                    arg0 = trial;
                }
            }
            if (struct1.size() == 1
                    and struct1[0].getSpace() == &csl::Minkowski) {
                auto sum = Replaced(momentumSum,
                                   momentumSum->getIndexStructure()[0],
                                   struct1[0]);
                if (auto trial = Expanded(arg1) + sum;
                        trial < arg1) {
                    simplified = true;
                    arg1 = trial;
                }
            }
            return simplified;
        }
        return false;
    };
    csl::ForEachNode(init, tryApply);
}

void AmplitudeCalculator::applyDimensionalRegularization(csl::Expr& res)
{
    csl::Expr eps = csl::constant_s("eps");
    csl::DeepRefresh(res);
    csl::Replace(res, csl::DMinko, 4 - 2 * eps);
    ExpandIf(res, [&](csl::Expr const& el)
    {
        return el->dependsOn(eps.get());
    },
    true);
    csl::Distribute(res, eps.get(), true);

    csl::Transform(res, [&](csl::Expr& node)
    {
        if (node->getType() == csl::Type::Prod) {
            int posEps = -1;
            int posIntegral = -1;
            for (size_t i = 0; i != node->size(); ++i) {
                if ((*node)[i] == eps) {
                    posEps = int(i);
                    (*node)[i] = CSL_1;
                }
                else if (IsOfType<FeynmanIntegral>((*node)[i]))
                    posIntegral = int(i);
            }
            if (posEps >= 0 and posIntegral == -1) {
                node = CSL_0;
                return true;
            }
            if (posEps >= 0) {
                auto integral = ConvertToPtr<FeynmanIntegral>(
                        (*node)[posIntegral]);
                csl::Expr factor = integral->getDivergentFactor();
                if (factor == CSL_0)
                    node = CSL_0;
                else
                    (*node)[posIntegral] = -factor;
                return true;
            }
        }
        return false;
    });
}

int AmplitudeCalculator::doApplyEOM(
        csl::Expr         &prod,
        csl::Tensor       &p,
        csl::Expr   const &m,
        csl::Index        alpha,
        bool              sign
        )
{
    // prod = csl::DeepCopy(prod);
    HEPAssert(prod->getType() == csl::Type::Prod,
            mty::error::TypeError,
            "Expecting a product, " + toString(prod) + " given.")

    csl::Index mu;
    int posP = -1;
    int posGamma = -1;
    csl::Index a1, a2;
    for (size_t i = 0; i != prod->size(); ++i) 
        if (csl::IsIndicialTensor(prod[i])
                and prod[i]->getParent_info() == p.get()) {
            posP = i;
            mu = prod[i]->getIndexStructureView()[0];
            for (size_t i = 0; i != prod->size(); ++i) 
                if (csl::IsIndicialTensor(prod[i])
                        and mty::dirac4.isGammaTensor(prod[i])
                        and prod[i]->getIndexStructureView()[0] == mu) {
                    posGamma = i;
                    a1 = prod[i]->getIndexStructureView()[1];
                    a2 = prod[i]->getIndexStructureView()[2];
                    break;
                }
            if (posGamma != -1)
                break;
        }
    if (posP == -1 or posGamma == -1) {
        //std::cout << prod << std::endl;
        return -1; // Nothing was found
    }

    // Here we have gamma ^mu _{a1, a2} * p_mu * [u/v](p)_alpha
    if (a2 == alpha or a1 == alpha) {
        // slashed{p}.u or bar{u}.slashed{p} -> m
        // slashed{p}.v or bar{v}.slashed{p} -> -m
        prod[posP] = (sign) ? m : -m;
        prod[posGamma] = csl::Copy(prod[posGamma]);
        csl::IndexStructure &structure 
            = prod[posGamma]->getIndexStructureView();
        structure.erase(structure.begin());
        prod[posGamma]->setParent(dirac4.getDelta());
        //csl::DeepRefresh(prod);
        return 1;
    }

    // Parsing all the product to have the entire chain
    csl::Index beta = alpha;
    std::vector<csl::Expr> gammaMatrices;
    gammaMatrices.reserve(prod->size());
    int firstArg = -1;
    while (beta != a1 and beta != a2) {
        bool betaChanged = false;
        for (size_t i = 0; i != prod->size(); ++i)  {
            if (csl::IsIndicialTensor(prod[i])
                    and mty::dirac4.isGammaTensor(prod[i])
                    and prod[i]->getParent()->getDim(&mty::dirac4) == 2) {
                csl::IndexStructure const &structure 
                    = prod[i]->getIndexStructureView();
                size_t n = structure.size();
                if (structure[n-1] == beta) {
                    gammaMatrices.push_back(csl::Copy(prod[i]));
                    firstArg = i;
                    beta = structure[n-2];
                    prod[i] = CSL_1;
                    betaChanged = true;
                    break;
                }
                if (structure[n-2] == beta) {
                    gammaMatrices.push_back(csl::Copy(prod[i]));
                    firstArg = i;
                    beta = structure[n-1];
                    prod[i] = CSL_1;
                    betaChanged = true;
                    break;
                }
            }
        }
        if (not betaChanged) {
            if (not gammaMatrices.empty())
                prod[firstArg] = csl::prod_s(gammaMatrices);
            //std::cout << prod << std::endl;
            return 0;
        }
    }

    // Chain has been found, commuting slashed(p) to u(p)
    prod[posP] = CSL_1;
    prod[posGamma] = csl::Copy(prod[posGamma]);
    prod[posGamma]->setParent(mty::dirac4.getDelta());
    prod[posGamma]->getIndexStructureView().erase(
            prod[posGamma]->getIndexStructureView().begin()
            );
    csl::Expr factor = CSL_1;
    std::vector<csl::Expr> res;
    res.reserve(gammaMatrices.size());
    size_t index = gammaMatrices.size();
    for (auto iter = gammaMatrices.rbegin(); 
            iter != gammaMatrices.rend();
            ++iter) {
        --index;
        if (mty::dirac4.isGammaChir(*iter))
            factor *= CSL_M_1;
        else if (mty::dirac4.isP_L(*iter)) {
            *iter = csl::Copy(*iter);
            (**iter).setParent(mty::dirac4.P_R);
        }
        else if (mty::dirac4.isP_R(*iter)) {
            *iter = csl::Copy(*iter);
            (**iter).setParent(mty::dirac4.P_L);
        }
        else {
            HEPAssert(mty::dirac4.isGammaMu(*iter),
                    mty::error::TypeError,
                    "Tensor " + toString(*iter) + " not recognized for dirac"
                    " equation.");
            std::vector<csl::Expr> args;
            args.reserve(prod->size());
            for (const auto &arg : prod)
                if (arg != CSL_1)
                    args.push_back(csl::DeepCopy(arg));
                else
                    args.push_back(CSL_1);
            std::vector<csl::Expr> modifiedChain;
            modifiedChain.reserve(gammaMatrices.size());
            for (const auto &gam : gammaMatrices)
                modifiedChain.push_back(csl::DeepCopy(gam));
            csl::IndexStructure &structure
                = modifiedChain[index]->getIndexStructureView();
            csl::Expr mom = p(structure[0]);
            modifiedChain[index]->setParent(mty::dirac4.getDelta());
            structure.erase(structure.begin());
            modifiedChain.push_back(2*factor * mom);
            args[firstArg] = csl::prod_s(modifiedChain);
            res.push_back(csl::prod_s(args));
            factor *= -1;
        }
    }
    std::vector<csl::Expr> args = prod->getVectorArgument();
    gammaMatrices.push_back(
            (sign) ? factor * m : -factor * m
            );
    args[firstArg] = csl::prod_s(gammaMatrices);
    res.push_back(csl::prod_s(args));
    prod = csl::sum_s(res);

    return 1;
}

struct RecursiveCounter {
    size_t n { 0 };
};

struct RecursiveWitness {
    RecursiveWitness(RecursiveCounter &t_counter): counter(t_counter)
    { ++counter.n; 
        std::cout << counter.n << std::endl; 
    }
    ~RecursiveWitness() { --counter.n; std::cout << "count : " << counter.n << std::endl;}
    RecursiveCounter &counter;
};


void AmplitudeCalculator::applyEOM(
        csl::Expr                   &ampl,
        std::vector<FermionEOMData> &onShellFermions,
        bool                         //fierzBackwards
        )
{
    bool transformed;
    csl::DeepExpandIf_lock(ampl, [&](csl::Expr const &expr) {
        for (const auto &eom : onShellFermions)
            if (csl::IsIndicialTensor(expr)
                and (expr->getParent_info() == eom.p.get()
                        or mty::dirac4.isGammaTensor(expr)))
                return true;
        return false;
    });
    do {
        transformed = false;
        for (auto &fermion : onShellFermions) {
            auto p = fermion.p;
            auto m = fermion.m;
            auto alpha = fermion.alpha;
            auto sign = fermion.sign;
            alpha.setFree(false);
            csl::ForEachNodeCut(ampl, [&](csl::Expr &sub)
            {
                if (csl::IsProd(sub)) {
                    auto sub_copy = csl::DeepCopy(sub);
                    int res = doApplyEOM(sub, p, m, alpha, sign);
                    if (res == 1) {
                        transformed = true;
                        return true;
                    }
                }
                return false;
            });
            if (transformed) {
                csl::DeepRefresh(ampl);
                break;
            }
        }
    } while (transformed);
    // do {
    //     transformed = false;
    //     for (auto &fermion : onShellFermions) {
    //         auto p = fermion.p;
    //         auto m = fermion.m;
    //         auto alpha = fermion.alpha;
    //         auto sign = fermion.sign;
    //         alpha.setFree(false);
    //         csl::ForEachNodeCut(ampl, [&](csl::Expr &sub)
    //         {
    //             if (csl::IsProd(sub)) {
    //                 int res = doApplyEOM(sub, p, m, alpha, sign);
    //                 if (fierzBackwards) {
    //                     fierz::applyFierz(sub, fierz::Right);
    //                     applyEOM(sub, onShellFermions, false);
    //                     transformed = true;
    //                     return true;
    //                 }
    //                 if (res == 1) {
    //                     transformed = true;
    //                     return true;
    //                 }
    //                 else if (res == 0) {
    //                     fierz::applyFierz(sub, fierz::Left);
    //                     applyEOM(sub, onShellFermions, true);
    //                     transformed = true;
    //                     return true;
    //                 }
    //             }
    //             return false;
    //         });
    //         if (transformed) {
    //             csl::DeepRefresh(ampl);
    //             break;
    //         }
    //     }
    // } while (transformed);
}

void AmplitudeCalculator::applyEOM(csl::Expr &ampl)
{
    std::vector<FermionEOMData> onShellFermions;
    csl::ForEachLeaf(ampl, [&](csl::Expr const &sub)
    {
        if (!IsOfType<PolarizationField>(sub))
            return;
        PolarizationField const &field = ConvertTo<PolarizationField>(sub);
        if (field.getQuantumParent()->getSpinDimension() == 2 and field.isOnShell()) {
            onShellFermions.push_back({
                field.getPoint(),
                field.getMass(),
                field.getIndexStructureView().back(),
                field.isIncomingParticle()
                    or field.isOutgoingParticle()
            });
        }
    });
    size_t nOnShellFermions = 0;
    for (const auto &field : fieldInsertions)
        if (field.isFermionic() && field.isOnShell())
            ++nOnShellFermions;
    if (nOnShellFermions == 2 && fieldInsertions.size() > 2) {
        simplifyImpulsions(ampl);
    }

    applyEOM(ampl, onShellFermions);
}

bool mayBeSimplified(csl::Expr const &expr)
{
    if (IsOfType<Propagator>(expr))
        return false;
    if (csl::IsPow(expr)
            and IsOfType<Propagator>(expr[0])
            and csl::IsNumerical(expr[1]))
        return false;
    if (csl::IsVectorIntegral(expr))
        return true;
    return csl::AnyOfLeafs(expr, [&](csl::Expr const &sub) {
        if (csl::IsIndexed(sub))
            return true;
        if (csl::IsVariable(sub)) 
            return true;
        return !(csl::IsLiteral(sub) or csl::IsNumerical(sub));
    });       
}

void AmplitudeCalculator::simplify(csl::Expr& ampl, Mode mode)
{
    csl::ScopedProperty commut(&csl::option::checkCommutations, false);
    csl::Refresh(ampl);
    if (mode != Coef) {
        MomentumConservater cons(fieldInsertions, momentum);
        cons.apply(ampl);
    }
    std::vector<csl::Expr> factors;
    if (csl::IsProd(ampl)) {
        for (size_t i = 0; i != ampl->size(); ++i)
            if (!mayBeSimplified(ampl[i])) {
                factors.push_back(ampl[i]);
                ampl[i] = CSL_1;
            }
        if (!factors.empty())
            csl::Refresh(ampl);
    }
    csl::ScopedProperty prop(&mty::option::applyFermionChain, false);
    csl::DeepRefresh(ampl);
    reduceTensorIntegrals(ampl);
    if (option::searchAbreviations) {
        findAbreviations(ampl);
    }
    // if (!xsec)
    //     simplifyImpulsions(ampl);
    expandMomentaExperimental(ampl);
    size_t maxLoop = 10;
    bool simplified;
    csl::Abbrev::enableGenericEvaluation("EXT");
    csl::Evaluate(ampl);
    csl::Abbrev::disableGenericEvaluation("EXT");
    size_t loop = 0;
    do {
        simplified = expandMinkoMetric(ampl);
        simplified = simplifyEpsilon(ampl) || simplified;
        simplified = expandGammaMatrices(ampl) || simplified;
        simplified = simplifyConjugationMatrix(ampl) || simplified;
        simplified = simplifyFermionChains(ampl) || simplified;
    } while (simplified and loop++ != maxLoop);

    loop = 0;
    do {
        simplified = expandColorIndices(ampl);
        simplified = simplifyColorWeights(ampl) || simplified;
    } while (simplified and loop++ != maxLoop);
    if (option::abbreviateColorStructures)
        findColorAbbreviation(ampl);
    // if (0 && !xsec and option::computeFirstIntegral) {
    //     loop = 0;
    //     do {
    //         simplified = expandMinkoMetric(ampl);
    //         simplified = simplifyEpsilon(ampl) || simplified;
    //         simplified = expandGammaMatrices(ampl) || simplified;
    //         simplified = simplifyConjugationMatrix(ampl) || simplified;
    //         simplified = simplifyFermionChains(ampl) || simplified;
    //     } while (simplified and loop++ != maxLoop);
    // }
    expandMomentaExperimental(ampl);
    if (mode == Ampl and option::applyEquationsOfMotion) {
        applyEOM(ampl);
    }
    findExternalAbbreviation(ampl);
    factor(ampl);
    addLocalTerms(ampl);
    expandMomentaExperimental(ampl);
    // HEPAssert(
    //         !csl::DependsOn(ampl, csl::DMinko),
    //         mty::error::RuntimeError,
    //         "D (Minkowski dimension) dependence found after local terms added,"
    //         " in " + toString(ampl))
    // csl::DeepHardFactor(ampl);
    if (mode != Ampl)
        csl::DeepHardFactor(ampl);
    else
        factor(ampl);
    if (!factors.empty()) {
        csl::Expr factor = csl::prod_s(factors);
        csl::ForEachNode(factor, [&](csl::Expr &sub) {
            if (IsOfType<Propagator>(sub))
                sub = csl::Factored(csl::Expanded(csl::Evaluated(
                                sub, 
                                csl::eval::abbreviation
                                )));
        });
        // , csl::eval::abbreviation);
        // csl::DeepExpand(factor);
        // csl::Factor(factor);
        if (mty::option::searchAbreviations 
                and mty::option::decomposeInOperators)
            ampl *= csl::Abbrev::makeAbbreviation(factor);
        else
            ampl *= factor;
    }
    csl::DeepRefresh(ampl);

    if (mode == Ampl) {
        abbreviateIntegral(ampl);
        if (!mty::option::decomposeInOperators)
            abbreviateAll(ampl);
        csl::DeepRefresh(ampl);
    }
}

color::ColorSpace const *inColorSpace(csl::Index const& index)
{
    return dynamic_cast<color::ColorSpace const*>(index.getSpace());
}

color::ColorSpace const *isColorStructure(
        csl::IndexStructure const& structure)
{
    if (structure.size() < 2)
        return nullptr;
    color::ColorSpace const *color = inColorSpace(structure[0]);
    for (const auto &index : structure) {
        if (!inColorSpace(index))
            return nullptr;
    }
    return color;
}

color::ColorSpace const *isColorStructure(csl::Expr const &tensor)
{
    if (csl::AnyOfLeafs(tensor, [&](csl::Expr const &sub) {
            return IsOfType<PolarizationField>(sub) 
                or sub->getName() == "EXT";
            }))
    {
        return nullptr;
    }
    if (csl::IsIndicialTensor(tensor)) {
        return isColorStructure(tensor->getIndexStructureView());
    }
    color::ColorSpace const *color = nullptr;
    csl::VisitEachLeaf(tensor, [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub)) {
            if (auto space = isColorStructure(sub->getIndexStructureView()))
                color = space;
        }
    });
    return color;
}

csl::IndexStructure AmplitudeCalculator::colorStructure(
        csl::Expr const &node,
        color::ColorSpace const *color
        )
{
    csl::IndexStructure res;
    csl::VisitEachLeaf(node, [&](csl::Expr const &sub)
    {
        if (!csl::IsIndicialTensor(sub))
            return;
        if (color != isColorStructure(sub))
            return;
        for (const auto &i : sub->getIndexStructureView()) {
            if (i.getSpace() != color)
                continue;
            auto pos = std::find(res.begin(), res.end(), i);
            if (pos == res.end())
                res.push_back(i);
        }
    });

    return res;
}

csl::Expr colorAbbreviation(
        std::vector<csl::Expr> const &tensors,
        std::vector<color::ColorSpace const*> const &spaces
        )
{
    std::vector<size_t> indicesLeft(tensors.size());
    for (size_t i = 0; i != indicesLeft.size(); ++i) {
        indicesLeft[i] = i;
    }

    csl::Expr res = CSL_1;
    while (!indicesLeft.empty()) {
        auto color = spaces[indicesLeft[0]];
        std::vector<csl::Expr> subArg;
        subArg.reserve(tensors.size());
        subArg.push_back(tensors[indicesLeft[0]]);
        indicesLeft.erase(indicesLeft.begin());
        for (size_t j = 0; j != indicesLeft.size(); ++j) {
            if (spaces[indicesLeft[j]]->getGroup() == color->getGroup()) {
                subArg.push_back(tensors[indicesLeft[j]]);
                indicesLeft.erase(indicesLeft.begin() + j);
                --j;
            }
        }
        csl::Expr init = csl::prod_s(subArg);
        csl::Expr toAbbreviate;
        while (init != (toAbbreviate = csl::DeepExpanded(init)))
            init = toAbbreviate;
        AmplitudeCalculator::factorIndicial(toAbbreviate);
        toAbbreviate = CalculateColorTrace(toAbbreviate);
        if (not csl::IsProd(toAbbreviate)) {
            if (toAbbreviate->isIndexed())
                res *= csl::Abbrev::makeAbbreviation(
                        "Color",
                        toAbbreviate
                        );
            else
                res *= toAbbreviate;
        }
        else {
            std::vector<csl::Expr> color;
            color.reserve(csl::Size(toAbbreviate));
            for (auto &arg : toAbbreviate)
                if (arg->isIndexed()) {
                    color.push_back(arg);
                    arg = CSL_1;
                }
            res *= toAbbreviate;
            res *= csl::Abbrev::makeAbbreviation(
                    "Color",
                    csl::prod_s(color)
                    );
        }
    }
    return res;
}

bool AmplitudeCalculator::expandColorIndices(csl::Expr &res)
{
    std::set<color::ColorSpace const*> colors;
    csl::VisitEachLeaf(res, [&](csl::Expr const &sub)
    {
        if (auto c = isColorStructure(sub))
            colors.insert(c);
    });
    bool expanded = false;
    for (auto c : colors) {
        csl::ForEachNode(res, [&](csl::Expr &prod) {
            if (csl::IsProd(prod))
                expanded = expandInProd(prod, [c, this](csl::Expr const &sub) {
                    return this->colorStructure(sub, c);
                }) || expanded;
        });
    }
    // csl::DeepExpandIf(res,
    // [&](csl::Expr const &expr)
    // {
    //     bool dependence = isColorStructure(expr);
    //     if (dependence)
    //         expanded = true;
    //     return dependence;
    // });

    return expanded;
}

bool AmplitudeCalculator::simplifyColorWeights(csl::Expr &res)
{
    auto hasColor = csl::AnyOfLeafs(res, [&](csl::Expr const &sub)
    {
        return csl::IsIndicialTensor(sub) && isColorStructure(sub);
    });
    if (!hasColor)
        return false;
    csl::Expr newRes = CalculateColorTrace(res);
    bool simplified = (res != newRes);
    res = newRes;

    return simplified;
}

bool AmplitudeCalculator::findColorAbbreviation(csl::Expr& res)
{
    bool simplified = false;
    csl::ForEachNodeCut(res, [&](csl::Expr &sub)
    {
        if (not csl::IsProd(sub))
            return false;
        std::vector<csl::Expr> colorArgs;
        std::vector<color::ColorSpace const*> colorSpaces;
        colorArgs.reserve(sub->size());
        colorSpaces.reserve(sub->size());
        for (auto &arg : sub)
            if (auto space = isColorStructure(arg);
                    space) {
                colorArgs.push_back(arg);
                colorSpaces.push_back(space);
                arg = CSL_1;
            }
        if (colorArgs.empty())
            return false;
        sub *= colorAbbreviation(colorArgs, colorSpaces);
        simplified = true;

        return true;
    });
    csl::Refresh(res);

    return simplified;
}

bool AmplitudeCalculator::expandMinkoMetric(csl::Expr &res)
{
    csl::Tensor g = csl::Minkowski.getMetric();
    csl::Tensor d = csl::Minkowski.getDelta();
    csl::Tensor e = csl::Minkowski.getEpsilon();
    auto init { res };
    // auto predicate = [&](csl::Expr const &expr)
    // {
    //     return csl::IsIndicialTensor(expr)
    //         and (expr->getParent_info() == g.get()
    //                 or expr->getParent_info() == d.get()
    //                 or expr->getParent_info() == e.get());
    // };
    // csl::DeepExpandIf_lock(res, predicate);
    auto isEmitter = [&](csl::Expr const &emitter) {
        return csl::IsIndicialTensor(emitter)
            and (emitter->getParent_info() == g.get()
                    or emitter->getParent_info() == d.get()
                    or emitter->getParent_info() == e.get());
    };
    auto isReceiver = [&](csl::Expr const &emitter, csl::Expr const &receiver) {
        csl::IndexStructure const &eindex = emitter->getIndexStructureView();
        csl::IndexStructure const &rindex = 
            csl::IsIndicialTensor(receiver) ? 
            receiver->getIndexStructureView()
            :receiver->getIndexStructure();
        for (const auto &ri : rindex) {
            if (ri.getSpace() == &csl::Minkowski) {
                for (const auto &ei : eindex)
                    if (ri == ei)
                        return true;
            }
        }
        return false;
    };

    csl::DeepPartialExpand(res, isEmitter, isReceiver);
    csl::DeepFactor(res);

    return (init != res);
}

bool AmplitudeCalculator::simplifyEpsilon(csl::Expr &res)
{
    auto isEpsilon = [&](csl::Expr const &expr) {
        return csl::IsIndicialTensor(expr)
            and expr->getParent_info() == csl::Minkowski.getEpsilon().get();
    };
    bool simplified = false;
    csl::ForEachNode(res, [&](csl::Expr &prod) {
        if (csl::IsProd(prod)) {
            for (size_t i = 0; i != prod->size(); ++i) 
                if (isEpsilon(prod[i])) {
                    std::vector<csl::Expr> args = prod->getVectorArgument();
                    args[i] = CSL_1;
                    csl::Expr cpy = csl::prod_s(args, true);
                    csl::IndexStructure epsStruct 
                        = prod[i]->getIndexStructure();
                    if (simplifyEpsilonInProd(cpy, epsStruct))
                        simplified = true;
                    prod = prod[i] * cpy;
                    return;
                }
        }
    });

    return simplified;
}

bool AmplitudeCalculator::simplifyEpsilonInProd(
        csl::Expr &prod,
        csl::IndexStructure &indices
        )
{
    csl::IndexStructure prodStruct = prod->getIndexStructure();
    for (size_t i = 0; i != indices.size(); ++i) {
        bool found = false;
        for (const auto &index : prodStruct)
            if (index == indices[i]) {
                found = true;
                break;
            }
        if (!found) {
            indices.erase(indices.begin() + i);
            --i;
        }
    }

    csl::Lock::lock(prod, [&](csl::Expr const &expr) {
        if (!expr->isIndexed())
            return false;
        const csl::IndexStructure structure = expr->getIndexStructure();
        for (const auto &index : structure)
            if (index.getSpace() == &csl::Minkowski)
                return true;
        return false;
    });
    for (size_t i = 0; i != indices.size(); ++i) 
        for (size_t j = i+1; j < indices.size(); ++j) {
            csl::Expr replaced = csl::Swapped(
                    prod,
                    indices[i],
                    indices[j]
                    );
            if (replaced == prod) {
                prod = CSL_0;
                return true;
            }
        }
    csl::Lock::unlock(prod);
    return false;
}

bool AmplitudeCalculator::expandGammaMatrices(csl::Expr &res) const
{
    auto predicate = [&](csl::Expr const &expr)
    {
        return csl::IsIndicialTensor(expr)
            and (dirac4.isGammaTensor(expr) 
              or dirac4.getDelta().get() == expr->getParent_info()
              or dirac4.C_matrix.get() == expr->getParent_info());
    };
    bool toExpand = csl::AnyOfNodes(res, [&](csl::Expr const &sub) {
        return csl::IsSum(sub)
            and sub.get() != res.get()
            and csl::AnyOfLeafs(sub, [&](csl::Expr const &leaf)
                    {return predicate(leaf);});
    });
    if (!toExpand)
        return false;
    csl::DeepExpandIf_lock(res, predicate);
    // csl::Factor(res);
    return false;
}

bool AmplitudeCalculator::simplifyConjugationMatrix(csl::Expr &res)
{
    if (not res->dependsExplicitlyOn(mty::dirac4.C_matrix.get()))
        return false;
    bool dependence = false;
    csl::ForEachLeaf(res, [&](csl::Expr &sub)
    {
        if (csl::IsITensor(sub) 
                and sub->getParent_info() == mty::dirac4.C_matrix.get()) {
            dependence = true;
        }
    });
    if (!dependence)
        return false;

    mty::ConjugationSimplifier simplifier(&mty::dirac4);
    bool simplified = false;
    csl::ForEachNodeCut(res, [&](csl::Expr &sub)
    {
        if (not csl::IsProd(sub))
            return false;
        for (size_t i = 0; i < csl::Size(sub); ++i)
            if (csl::IsITensor(sub[i]) 
                    and sub[i]->getParent_info() == mty::dirac4.C_matrix.get()
                    and !sub[i]->getIndexStructureView()[1].getFree()) {
                csl::Expr C = Copy(sub[i]);
                sub[i] = 1;
                csl::Refresh(sub);
                if (simplifier.hasContractionProperty(C.get(), sub.get())) {
                    sub = simplifier.contraction(C.get(), sub.get());
                    simplified = true;
                    return true;
                }
                else {
                    std::cout << C << std::endl;
                    CallHEPError(mty::error::RuntimeError,
                            "Expression " + toString(sub) + " has no "
                            "appropriate property for conjugation matrix.")
                }
            }
        return false;
    });
    if (simplified) {
        simplifyConjugationMatrix(res);
        return true;
    }
    return false;
}

bool AmplitudeCalculator::simplifyFermionChains(csl::Expr &res)
{
    bool gamma = csl::AnyOfLeafs(res, [&](csl::Expr const &sub)
    {
        return csl::IsIndicialTensor(sub)
            && dirac4.isGammaTensor(sub);
    });
    if (!gamma)
        return false;
    bool simplified = false;
    auto traced = mty::dirac4.calculateTrace(res);
    if (traced != res)
        simplified = true;
    res = traced;
    FermionChain chain(&dirac4);
    csl::ForEachNode(res, [&](csl::Expr& el)
    {
        if (el->getType() == csl::Type::Prod)
            simplified = chain.applyOn(el) || simplified;
    });
    if (simplified)
        csl::DeepRefresh(res);

    return simplified;
}

void AmplitudeCalculator::reduceTensorIntegrals(csl::Expr &res)
{
    csl::ForEachNode(res, [&](csl::Expr& node)
    {
      if (node->getType() == csl::Type::Prod) {
        for (auto &arg : node)
            if (arg->getType() == csl::Type::VectorIntegral) {
                arg = FeynmanIntegral::replaceIntegral(arg);
                Refresh(node);
                break;
            }
      }
      else if (node->getType() == csl::Type::VectorIntegral) {
           node = FeynmanIntegral::replaceIntegral(node);
           Refresh(node);
       }
    });
}

csl::IndexStructure AmplitudeCalculator::momentumStructure(csl::Expr const &node)
{
    csl::IndexStructure res;
    csl::VisitEachLeaf(node, [&](csl::Expr const &sub)
    {
        if (!csl::IsIndicialTensor(sub))
            return false;
        if (sub->getIndexStructureView().size() != 1
             or sub->getIndexStructureView()[0].getSpace() != &csl::Minkowski)
            return false;
        const auto parent = sub->getParent_info();
        for (const auto &m : momentum)
            if (m.get() == parent) {
                auto pos = std::find(res.begin(), res.end(), 
                        sub->getIndexStructureView()[0]);
                if (pos == res.end())
                    res.push_back(sub->getIndexStructureView()[0]);
            }
        return false;
    });

    return res;
}

bool commonIndex(
        std::vector<size_t> const &A,
        std::vector<size_t> const &B
        )
{
    auto iterA = A.begin();
    while (iterA != A.end()) {
        auto iterB = B.begin();
        while (iterB != B.end()) {
            if (*iterA == *iterB)
                return true;
            ++iterB;
        }
        ++iterA;
    }
    return false;
}

void mergeIn(
        std::vector<size_t>       &target,
        std::vector<size_t> const &toCopy
        )
{
    target.insert(target.end(), toCopy.begin(), toCopy.end());
    std::sort(target.begin(), target.end());
    auto last = std::unique(target.begin(), target.end());
    target.erase(last, target.end());
}

std::vector<std::vector<size_t>> findStructureMatches(
        std::vector<csl::IndexStructure> const &structures
        )
{
    std::vector<std::vector<size_t>> res;
    for (size_t i = 0; i != structures.size(); ++i) {
        res.push_back({i});
        for (size_t j = i+1; j < structures.size(); ++j) {
            bool match = false;
            for (const auto& ind : structures[i]) {
                for (const auto &jnd : structures[j])
                    if (ind == jnd) {
                        match = true;
                        break;
                    }
                if (match)
                    break;
            }
            if (match)
                res.back().push_back(j);
        }
        if (res.back().size() < 2)
            res.erase(res.end() - 1);
    }

    for (size_t i = 0; i != res.size(); ++i) {
        for (size_t j = i+1; j < res.size(); ++j) {
            if (commonIndex(res[i], res[j])) {
                mergeIn(res[i], res[j]);
                res.erase(res.begin() + j);
                --j;
            }
        }
    }

    return res;
}

bool AmplitudeCalculator::expandInProd(
        csl::Expr &prod,
        std::function<csl::IndexStructure(csl::Expr const&)> const &structureGetter
        )
{
    std::vector<size_t> pos;
    std::vector<csl::IndexStructure> structures;
    structures.reserve(prod->size());
    for (size_t i = 0; i != prod->size(); ++i) {
        if (auto structure = structureGetter(prod[i]);
                !structure.empty()) {
            pos.push_back(i);
            structures.push_back(structure);
        }
    }
    std::vector<std::vector<size_t>> toExpand
        = findStructureMatches(structures);
    if (toExpand.empty()) {
        return false;
    }
    // prod = csl::Copy(prod);
    for (size_t i = 0; i != toExpand.size(); ++i) {
        std::vector<size_t> const &positions = toExpand[i];
        std::vector<csl::Expr> args;
        args.reserve(positions.size());
        for (size_t k : positions) {
            args.push_back(prod[pos[k]]);
            prod[pos[k]] = CSL_1;
        }
        HEPAssert(prod[pos[positions[0]]] == CSL_1,
                mty::error::RuntimeError,
                "Argument " + toString(prod[pos[positions[0]]]) + " should be "
                "1..")
        prod[pos[positions[0]]] = csl::Expanded(csl::prod_s(args));
    }
    csl::Refresh(prod);
    return true;
}

void AmplitudeCalculator::expandMomentaExperimental(csl::Expr &res)
{
    auto init = res;
    auto isEmitter = [&](csl::Expr const &emitter) {
        if (!csl::IsIndicialTensor(emitter))
            return false;
        const auto parent = emitter->getParent_info();
        auto const &spaces = parent->getSpace();
        if (spaces.size() != 1 || spaces[0] != &csl::Minkowski)
            return false;
        for (const auto &p : momentum)
            if (p.get() == parent)
                return true;
        return false;
    };
    auto isReceiver = [&](csl::Expr const &p, csl::Expr const &receiver) {
        if (!isEmitter(receiver))
            return false;
        return p->getIndexStructureView()[0] 
            == receiver->getIndexStructureView()[0];
    };
    csl::DeepPartialExpand(res, isEmitter, isReceiver);

    // return init != res;
    // bool expanded = false;
    // csl::ForEachNode(res, [&](csl::Expr &node)
    // {
    //     if (csl::IsProd(node))
    //     if(expandInProd(node, 
    //                 [this](csl::Expr const &e) {
    //                     return this->momentumStructure(e);
    //                 }))
    //     {
    //         expanded = true;
    //     }
    // });
    // if (expanded)
    //     expandMomentaExperimental(res);
}

void AmplitudeCalculator::expandMomenta(csl::Expr &res)
{
    expandMomenta(res, momentum, fieldInsertions);
}

void AmplitudeCalculator::doExpandMomenta(
        csl::Expr                                 &res,
        std::vector<csl::Tensor>             &momentum,
        std::vector<mty::QuantumField> const &fieldInsertions)
{
    csl::Index mu = mty::MinkowskiIndex();
    std::vector<csl::Expr> squaredMomenta;
    for (size_t i = 0; i != fieldInsertions.size(); ++i) 
        for (size_t j = i; j != fieldInsertions.size(); ++j) {
            csl::Expr squared = momentum[i](mu)*momentum[j](+mu);
            if (!squared->isIndexed()) {
                if (csl::IsPow(squared))
                    squared = squared[0];
                squaredMomenta.push_back(squared);
            }
        }
    auto isMomentum = [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub)) {
            for (const auto &mom : momentum)
                if (mom.get() == sub->getParent_info())
                    return true;
            return false;
        }
        return false;
    };
    auto predicate = [&](csl::Expr const &sub) {
        return isMomentum(sub);
    };
    auto expandif = [&](csl::Expr const &sub) {
        return csl::AnyOfLeafs(sub, [&](csl::Expr const &leaf) {
            return isMomentum(leaf);
        });
    };
    csl::DeepFactor(res);
    csl::Lock::lock(res, predicate);
    csl::DeepExpandIf(res, expandif);
    csl::Lock::unlock(res);
    csl::DeepRefresh(res);
    csl::DeepCollect(res, squaredMomenta);
}

void AmplitudeCalculator::expandMomenta(
        csl::Expr                                 &res,
        std::vector<csl::Tensor>             &momentum,
        std::vector<mty::QuantumField> const &fieldInsertions)
{
    doExpandMomenta(res, momentum, fieldInsertions);
    // auto isMomentum = [&](csl::Expr const &sub)
    // {
    //     if (csl::IsIndicialTensor(sub)) {
    //         for (const auto &mom : momentum)
    //             if (mom.get() == sub->getParent_info())
    //                 return true;
    //         return false;
    //     }
    //     return false;
    // };
    // csl::ForEachNode(res, [&](csl::Expr &prod)
    // {
    //     if (csl::IsProd(prod)) {
    //         for (auto const &arg : prod) {
    //             if (isMomentum(arg)) {
    //                 doExpandMomenta(prod, momentum, fieldInsertions);
    //                 return;
    //             }
    //         }
    //     }
    // });
}

struct D_info {

    inline static const csl::Expr Dm4 = csl::constant_s("Dm4");

    csl::Expr constantTerm;
    csl::Expr Dm4Term;

    bool operator==(D_info const &other) const {
        return constantTerm == other.constantTerm
            && Dm4Term == other.Dm4Term;
    }

    bool operator!=(D_info const &other) const {
        return !(*this == other);
    }

    D_info &operator+=(D_info const &other) {
        constantTerm += other.constantTerm;
        Dm4Term      += other.Dm4Term;
        return *this;
    }

    D_info &operator*=(D_info const &other) {
        if (Dm4Term == CSL_0 and other.constantTerm != CSL_0) {
            return *this = other;
        }
        Dm4Term = constantTerm * other.Dm4Term 
                + Dm4Term * other.constantTerm;
        constantTerm *= other.constantTerm;
        return *this;
    }
};

D_info parseDInfoSum(csl::Expr&, D_info init);
D_info parseDInfoProd(csl::Expr&, D_info init);
D_info parseDInfoPow(csl::Expr&, D_info init);
D_info parseDInfo(
        csl::Expr &expr,
        D_info init = {0, 0})
{
    //std::cout << "PARSING " << expr << std::endl;
    D_info res {0, 0};
    if (csl::IsSum(expr))
        res = parseDInfoSum(expr, init);
    if (csl::IsProd(expr))
        res = parseDInfoProd(expr, init);
    if (csl::IsPow(expr))
        res = parseDInfoPow(expr, init);
    if (csl::IsNumerical(expr))
        res = {expr, 0};
    if (expr == D_info::Dm4)
        res = {0, 1};
    if (IsOfType<FeynmanIntegral>(expr)
            and init.Dm4Term != CSL_0) {
        //std::cout << "HERE" << std::endl;
        csl::Expr factor = ConvertToPtr<FeynmanIntegral>(expr)->getDivergentFactor();
        if (factor != CSL_0) {
            expr = Copy(expr) + init.Dm4Term / init.constantTerm * factor;
            // std::cout << expr << std::endl;
        }
        //std::cout << expr << std::endl;
    }

    //std::cout << "(" << res.constantTerm << " , " << res.Dm4Term << ")" << std::endl;
    //std::cin.get();
    return res;
}
D_info parseDInfoSum(
        csl::Expr &sum,
        D_info init)
{
    D_info info {0, 0};
    for (auto &arg : sum)
        info += parseDInfo(arg, init);

    if (info.Dm4Term == CSL_0)
        return {0, 0};
    return info;
}
D_info parseDInfoProd(
        csl::Expr &prod,
        D_info init)
{
    D_info info {0, 0};
    for (auto &arg : prod) {
        if (csl::AnyOfNodes(arg, [&](csl::Expr const &sub) 
            { return IsOfType<FeynmanIntegral>(sub); }))
            continue;
        D_info infoArg = parseDInfo(arg, init);
        if (info == D_info({0, 0}))
            info = infoArg;
        else if (infoArg.Dm4Term != CSL_0)
            info *= infoArg;
    }
    if (init == D_info({0, 0}))
        init = info;
    else
        init *= info;
    for (auto &arg : prod)
        if (csl::AnyOfNodes(arg, [&](csl::Expr const &sub) 
            { return IsOfType<FeynmanIntegral>(sub); }))
            parseDInfo(arg, init);

    return info;
}
D_info parseDInfoPow(
        csl::Expr &pow,
        D_info /*init*/)
{
    D_info info = parseDInfo(pow->getArgument());
    if (info.Dm4Term != CSL_0
            and csl::IsNumerical(pow->getArgument(1))) {
        info.constantTerm = csl::pow_s(info.constantTerm, pow->getArgument(1));
        info.Dm4Term *= pow->getArgument(1) * info.Dm4Term;
        return info;
    }
    return {0, 0};
}

void AmplitudeCalculator::addLocalTerms(csl::Expr &res)
{
    bool integral = csl::AnyOfNodes(res, [&](csl::Expr const &sub)
    {
        return IsOfType<FeynmanIntegral>(sub);
    });
    if (!option::addLocalTerms || !integral) {
        csl::Replace(res, csl::DMinko, csl::int_s(4));
        return;
    }
    csl::Expr Dm4 = D_info::Dm4;
    csl::DeepRefresh(res);
    res = csl::Replaced(res, csl::DMinko, 4 + Dm4);
    parseDInfo(res);
    csl::DeepRefresh(res);
    csl::Replace(res, Dm4, CSL_0);
    HEPAssert(!csl::DependsOn(res, Dm4),
            mty::error::RuntimeError,
            "Epsilon dependence in " + toString(res))
}

csl::Expr getLiteralFactor(csl::Expr const &el)
{
    if (csl::IsProd(el)) {
        std::vector<csl::Expr> factors;
        factors.reserve(el->size());
        for (size_t i = 0; i != el->size(); ++i) {
            if (el[i]->isIndexed()) 
                break;
            factors.push_back(el[i]);
        }
        return csl::prod_s(factors, true);
    }
    else
        return (!el->isIndexed()) ? el : CSL_1;
}

void mergeLiteralsSum(csl::Expr &sum)
{
    bool found = false;
    for (size_t i = 0; i != sum->size(); ++i) {
        auto &arg_i = sum[i];
        if (arg_i == CSL_0)
            continue;
        auto fact_i = getLiteralFactor(arg_i);
        std::vector<csl::Expr> factors{fact_i};
        auto term_i = arg_i / fact_i;
        for (size_t j = i+1; j < sum->size(); ++j) {
            auto &arg_j = sum[j];
            if (arg_j == CSL_0)
                continue;
            auto fact_j = getLiteralFactor(arg_j);
            auto term_j = arg_j / fact_j;
            if (term_i == term_j) {
                found = true;
                factors.push_back(fact_j);
                arg_j = CSL_0;
            }
        }
        if (factors.size() > 1) {
            arg_i = (arg_i / fact_i) * csl::sum_s(factors);
        }
    }
    if (found)
        csl::Refresh(sum);
}

void mergeLiterals(csl::Expr &expr)
{
    csl::ForEachNode(expr, [&](csl::Expr &sum)
    {
        if (csl::IsSum(sum))
            mergeLiteralsSum(sum);
    });
}

bool AmplitudeCalculator::factorIndicial(csl::Expr &res)
{
    auto termAndFactor = 
    [](csl::Expr const &expr) -> std::pair<csl::Expr, csl::Expr> 
    {
        if (csl::IsProd(expr)) {
            std::vector<csl::Expr> terms;
            std::vector<csl::Expr> factors;
            terms.reserve(expr->size());
            factors.reserve(expr->size());
            for (const auto &arg : expr) {
                if (arg->isIndexed())
                    terms.push_back(arg);
                else
                    factors.push_back(arg);
            }
            return {prod_s(factors, true), prod_s(terms, true)};
        }
        else
            return (expr->isIndexed()) ? 
                std::make_pair(CSL_1, expr) 
                :std::make_pair(expr, CSL_1);
    };
    std::vector<std::pair<csl::Expr, csl::Expr>> termsAndFactors;
    bool simplified = false;
    csl::ForEachNode(res, [&](csl::Expr &sum)
    {
        if (!csl::IsSum(sum))
            return;
        size_t hasIndex = 0;
        for (const auto &arg : sum)
            if (arg->isIndexed()) {
                ++hasIndex;
                if (hasIndex == 2)
                    break;
        }
        if (hasIndex < 2)
            return;
        termsAndFactors.reserve(sum->size());
        for (const auto &arg : sum) {
            termsAndFactors.push_back(termAndFactor(arg));
        }
        bool merged = false;
        for (size_t i = 0; i != termsAndFactors.size(); ++i) {
            if (termsAndFactors[i].second == CSL_1)
                return;
            for (size_t j = i+1; j != termsAndFactors.size(); ++j) {
                if (termsAndFactors[j].first != CSL_0
                        && termsAndFactors[j].second == termsAndFactors[i].second) {
                    termsAndFactors[i].first += termsAndFactors[j].first;
                    termsAndFactors.erase(termsAndFactors.begin() + j);
                    --j;
                    merged = true;
                    simplified = true;
                }
            }
        }
        if (merged) {
            std::vector<csl::Expr> newArgs;
            newArgs.reserve(termsAndFactors.size());
            for (const auto &[factor, term] : termsAndFactors)
                newArgs.push_back(prod_s(factor, term, true));
            sum = sum_s(newArgs, true);
        }
    });
    return simplified;
}

void AmplitudeCalculator::factor(csl::Expr &res)
{
    csl::DeepFactor(res);
    // mergeLiterals(res);
}

bool isMomentum(csl::Expr &tensor)
{
    if (!csl::IsIndicialTensor(tensor))
        return false;
    if (tensor->getName()[0] != 'p')
        return false;
    std::vector<csl::Space const*> spaces = tensor->getParent()->getSpace();
    return spaces.size() == 1 and spaces[0] == &csl::Minkowski;
}

void AmplitudeCalculator::abbreviateIntegral(csl::Expr &res)
{
    if (csl::IsSum(res)) {
        bool integral = true;
        for (const auto &arg : res) {
            const size_t size = arg->size();
            if (size == 0
                    and !csl::IsNumerical(arg)
                    and !IsOfType<FeynmanIntegral>(arg)) {
                integral = false;
                break;
            }
            else if (size > 0
                    and !IsOfType<FeynmanIntegral>(arg)) {
                for (size_t i = 0; i != size; ++i) 
                    if (!csl::IsNumerical(arg[i])
                            and !IsOfType<FeynmanIntegral>(arg[i])) {
                        integral = false;
                        break;
                    }
            }
            if (!integral)
                break;
        }
        if (integral) {
            res = csl::Abbrev::makeAbbreviation(
                    "INT",
                    res
                    );
        }
        else
            for (size_t i = 0; i != res->size(); ++i)
                abbreviateIntegral(res[i]);
    }
    else
        for (size_t i = 0; i != res->size(); ++i)
            abbreviateIntegral(res[i]);
}

void AmplitudeCalculator::abbreviateAll(csl::Expr &res)
{
    if (!mty::option::searchAbreviations)
        return;
    if (csl::IsSum(res)) {
        for (auto &term : res)
            abbreviateAll(term);
    }
    else if (csl::IsProd(res)){
        std::vector<csl::Expr> externalLegs;
        std::vector<csl::Expr> momenta;
        std::vector<csl::Expr> factors;
        for (auto &arg : res) {
            if (isMomentum(arg)) {
                momenta.push_back(arg);
                arg = CSL_1;
            }
            else if (IsOfType<PolarizationField>(arg)) {
                externalLegs.push_back(arg);
                arg = CSL_1;
            }
            else if (
                    csl::Abbrev::getFreeStructure(
                        arg->getIndexStructure()).size() == 0
                    and !csl::IsNumerical(arg)
                    and arg != csl::DMinko) {
                factors.push_back(arg);
                arg = CSL_1;
            }
        }
        csl::Refresh(res);
        csl::Expr ext = (externalLegs.size() > 0) ? 
            csl::Abbrev::makeAbbreviation(
                    "EXT", 
                    csl::prod_s(externalLegs, true)
                    )
            : CSL_1;
        csl::Expr mom = (momenta.size() > 0) ? 
            csl::Abbrev::makeAbbreviation(
                    "P", 
                    csl::prod_s(momenta, true)
                    )
            : CSL_1;
        csl::Expr factor;
        if (factors.size() > 1 
                or (factors.size() == 1 and ! csl::IsLiteral(factors[0]))) 
            factor = csl::Abbrev::makeAbbreviation(csl::prod_s(factors, true));
        else
            factor = (factors.empty()) ? CSL_1 : factors[0];

        mty::FermionChain chain(&mty::dirac4);
        csl::ForEachNode(res, [&](csl::Expr &prod)
        {
            if (csl::IsProd(prod))
                chain.applyOn(prod);
        });

        res = csl::prod_s({factor, mom, res, ext});
    }
}

void AmplitudeCalculator::initInteractionTerms(
        std::vector<TermType> const& t_interactionTerms)
{
    interactionTerms.clear(),
    interactionTerms.shrink_to_fit();
    interactionTerms.reserve(t_interactionTerms.size());
    for (const auto& term_ptr : t_interactionTerms)
        interactionTerms.push_back(
                std::make_shared<InteractionTerm>(*term_ptr));
}

void AmplitudeCalculator::initFieldInsertions(csl::vector_expr t_fieldInsertions)
{
    fieldInsertions.reserve(t_fieldInsertions.size());
    for (auto& insertion : t_fieldInsertions) {
        applyDerivativesInStructure(insertion);
        HEPAssert(IsOfType<QuantumField>(insertion),
                mty::error::TypeError,
                "Expected quantumField in insertion in"
                + static_cast<std::string>(" mty::option::calculateResult()."));
        mty::QuantumField* field = ConvertToPtr<QuantumField>(insertion);
        field->setExternal(true);
        fieldInsertions.push_back(*field);
    }
}

void AmplitudeCalculator::initVertices()
{
    vertices.clear();
    vertices.shrink_to_fit();
    const size_t size = (not ruleMode) ? 
        feynmanRules.size() : interactionTerms.size();
    vertices.reserve(size);
    for (size_t i = 0; i != size; ++i) {
        csl::Tensor V = csl::tensor_s("V_" + toString(i),
                                       &csl::Minkowski);
        vertices.push_back(V);
        if (ruleMode)
            interactionTerms[i]->setPoint(V);
    }
}

void AmplitudeCalculator::initLSZInsertion()
{
    if (LSZInsertions)
        return;

    // Gathering LSZ insertions of fields
    LSZInsertions = CSL_1;
    bool firstFermionFound = false;
    for (size_t i = 0; i != fieldInsertions.size(); ++i) {
        auto& field = fieldInsertions[i];
        if (!firstFermionFound && field.isFermionic() && !field.isSelfConjugate()) {
            firstFermionFound = true;
            LSZInsertions = csl::prod_s(LSZInsertions.value(),
                                       field.getLSZInsertion(momentum[i], ruleMode, true),
                                       true);
        }
        else
            LSZInsertions = csl::prod_s(LSZInsertions.value(),
                                       field.getLSZInsertion(momentum[i], ruleMode),
                                       true);
    }
    if (not ruleMode)
        findExternalAbbreviation(LSZInsertions.value());
}

static bool compareFields(
        QuantumField const &A,
        QuantumField const &B,
        [[maybe_unused]] size_t pos
        )
{
    if (A.isComplexConjugate() != B.isComplexConjugate())
        return !A.isComplexConjugate();
    if (A.getName() != B.getName())
        return A.getName() < B.getName();
    return A.getPoint()->getName() < B.getPoint()->getName();
}
void AmplitudeCalculator::orderInsertions()
{
    for (size_t i = 0; i != fieldInsertions.size(); ++i)  {
        size_t mini = i;
        for (size_t j = i+1; j < fieldInsertions.size(); ++j)  {
            if (compareFields(fieldInsertions[j], fieldInsertions[mini], i))
                mini = j;
        }
        if (mini != i) {
            std::swap(fieldInsertions[i], fieldInsertions[mini]);
            std::swap(momentum[i], momentum[mini]);
        }
    }
    // bool conjugated = false;
    //for (size_t i = 0; i != fieldInsertions.size(); ++i)
    //    if (fieldInsertions[i].isFermionic()) {
    //        if (fieldInsertions[i].isComplexConjugate() == conjugated) {
    //            conjugated = !conjugated;
    //            continue;
    //        }
    //        for (size_t j = i+1; j < fieldInsertions.size(); ++j)
    //            if (fieldInsertions[j].isFermionic()
    //                    and fieldInsertions[j].isComplexConjugate()
    //                            == conjugated) {
    //                QuantumField save = fieldInsertions[j];
    //                fieldInsertions.erase(fieldInsertions.begin() + j);
    //                fieldInsertions.insert(fieldInsertions.begin()+i, save);
    //                csl::Tensor mom = momentum[j];
    //                momentum.erase(momentum.begin() + j);
    //                momentum.insert(momentum.begin() + i, mom);
    //                conjugated = !conjugated;
    //                break;
    //            }
    //    }
    // std::cout << std::endl;
    // for (const auto& f : fieldInsertions)
    //     f.print();
}

void AmplitudeCalculator::initMomentumVertices(
        std::map<csl::Tensor, size_t> &vertexIds,
        std::vector<csl::Tensor>      &witnessVertices,
        csl::PseudoIntegral           &integral,
        std::vector<csl::Expr>             &fieldVertices
        )
{
    size_t index = 0;
    for (size_t i = 0; i != feynmanRules.size(); ++i) {
        for (size_t j = 0; j != feynmanRules[i].getSize(); ++j)
            witnessVertices.push_back(csl::tensor_s(
                        "W_"+toString(index++),
                        &csl::Minkowski));
    }
    auto firstWitness = witnessVertices.begin();
    for (size_t i = 0; i != feynmanRules.size(); ++i) {
        auto lastWitness = firstWitness + feynmanRules[i].getSize();
        integral.addTerm(csl::vectorintegral_s(vertices[i]));
        vertexIds[vertices[i]] = i;
        fieldVertices[fieldInsertions.size() + i] 
            = feynmanRules[i].getFieldProduct(vertices[i],
                                              firstWitness,
                                              lastWitness);
    }
}

void AmplitudeCalculator::applyMomentumVertices(
        std::vector<csl::Tensor> const &witnessVertices,
        FeynruleMomentum               &momentumMapping,
        csl::Expr                           &amplitude
        )
{
    for (const auto& w : witnessVertices) {
        auto optReplacement = momentumMapping.pop(w);
        if (optReplacement) {
            csl::Tensor momentum = optReplacement->momentum;
            csl::Expr factor = optReplacement->factor;
            csl::ForEachLeaf(amplitude, [&](csl::Expr& el)
            {
                if (el->getType() == csl::Type::TensorElement
                        and el->getParent_info() == w.get()) {
                    el->setParent(momentum);
                    if (factor != CSL_1)
                        el = factor * el;
                    return true;
                }
                return false;
            });
        }
        HEPAssert(
                !csl::DependsExplicitlyOn(amplitude, w),
                mty::error::RuntimeError,
                "Mapping of momentum " + toString(w->getName()) 
                + " failed.");
    }
}

void AmplitudeCalculator::suppressDiracDelta(
        csl::Expr &amplitude,
        csl::Expr const &PSum
        )
{
    csl::Expr diracDelta = csl::diracdelta_s(PSum);
    csl::ForEachNode(amplitude, [&](csl::Expr &sub)
    {
        if (*sub == diracDelta) {
            sub = CSL_1;
        }
    });
}

std::vector<std::vector<size_t>> AmplitudeCalculator::getExternalSymmetries(
        csl::Expr &fieldProd
        ) const
{
    csl::Expr firstFieldExpr;
    csl::FirstOfLeaf(fieldProd,
    [&](csl::Expr const& expr) {
      if (IsOfType<QuantumField>(expr) and
              ConvertToPtr<QuantumField>(expr)->isExternal()) {
        firstFieldExpr = expr;
        return true;
      }
      return false;
    });
    if (!firstFieldExpr)
        return {};
    mty::QuantumField firstField = ConvertTo<QuantumField>(firstFieldExpr);

    std::vector<std::vector<size_t>> res;
    if (fieldInsertions.size() < 3)
        return res;

    auto isFirst = [&](QuantumField const& field)
    {
        return wick::areDegenerate(field, firstField)
            and field.getPoint().get() == firstField.getPoint().get();
    };
    std::vector<size_t> indicesLeft = csl::range(fieldInsertions.size());
    for (size_t i = 0; i < indicesLeft.size()-1; ++i) {
        std::vector<size_t> intermediate;
        QuantumField field = fieldInsertions[indicesLeft[i]];
        if (not isFirst(field))
            intermediate.push_back(indicesLeft[i]);
        for (size_t j = i+1; j < indicesLeft.size(); ++j) {
            if (wick::areDegenerate(field,
                        fieldInsertions[indicesLeft[j]])) {
                if (not isFirst(fieldInsertions[indicesLeft[j]]))
                    intermediate.push_back(indicesLeft[j]);
                indicesLeft.erase(indicesLeft.begin() + j);
                --j;
            }
        }
        if (intermediate.size() > 1)
            res.push_back(std::move(intermediate));
    }

    return res;
}

void AmplitudeCalculator::applyDerivativesInStructure(csl::Expr& expression)
{
    csl::ForEachNode(
        expression,
        [](csl::Expr& el) {
          if (el->getType() == csl::Type::TDerivativeElement
                  and IsOfType<mty::QuantumField>(el->getOperand())) {
            mty::QuantumField field = ConvertTo<mty::QuantumField>(
                    el->getOperand());
            if (mty::option::applyDerivatives) {
                field.addDerivative(el->getIndexStructureView()[0]);
            }
            el = field.copy();
          }
          else if (not mty::option::applyDerivatives 
                  and IsOfType<mty::QuantumField>(el)) {
            mty::QuantumField* field = ConvertToPtr<mty::QuantumField>(el);
            field->setDerivativeStructure(csl::IndexStructure());
          }
        });
}

csl::vector_expr AmplitudeCalculator::applyExternalSymmetries(
        csl::Expr const& res,
        std::vector<size_t> perm) const
{
    const size_t nIndices = fieldInsertions[perm[0]].
        getIndexStructureView().size();

    std::vector<mty::QuantumField> deg_insertions(perm.size());
    std::vector<csl::Tensor>      deg_momentum(perm.size());
    std::vector<csl::Index>        deg_indices(perm.size() * nIndices); 

    for (size_t i = 0; i != perm.size(); ++i) {
        deg_momentum[i] = momentum[perm[i]];
        deg_insertions[i] = fieldInsertions[perm[i]];
        if (ruleMode) {
            for (size_t j = 0; j != nIndices; ++j) {
                deg_indices[nIndices*i + j] = 
                    deg_insertions[i].getIndexStructureView()[j];
            }
        }
        else {
            csl::Expr polarTensor = CSL_0;
            csl::AnyOfLeafs(res, [&](csl::Expr const& expr)
            {
                if (not IsOfType<PolarizationField>(expr))
                    return false;
                if (expr->getPoint().get() != deg_momentum[i].get())
                    return false;
                polarTensor = expr;
                return true;
            });
            HEPAssert(polarTensor != CSL_0 
                        or deg_insertions[i].getSpinDimension() == 1,
                    mty::error::RuntimeError,
                    "Polar tensor for external leg " 
                    + std::string(deg_insertions[i].getName()) 
                    + " not found in expression " + toString(res));
            if (polarTensor != CSL_0) {
                for (size_t j = 0; j != nIndices; ++j) {
                    deg_indices[nIndices*i + j] = 
                        polarTensor->getIndexStructureView()[1+j].getFlipped();
                }
            }
        }
    }

    size_t count = 0;
    bool fermionic = not fieldInsertions[perm[0]].isBosonic();
    int sign = -1;
    csl::vector_expr terms(1, DeepRefreshed(res));

    perm = csl::range(perm.size());
    while (std::next_permutation(perm.begin(), perm.end())) {
        csl::Expr copyRes = DeepCopy(terms[0]);
        csl::ForEachLeaf(copyRes, [&](csl::Expr& el)
        {
            if (el->isIndexed()) {
                for (size_t i = 0; i != deg_momentum.size(); ++i)
                    if (el->getParent_info() == deg_momentum[i].get()) {
                        el->setParent(deg_momentum[perm[i]]);
                        if (deg_insertions[i].isIncoming() xor 
                                deg_insertions[perm[i]].isIncoming())
                            el = -el;
                        break;
                    }
            }
        });
        if (fermionic) {
            for (size_t k = 0; k != perm.size(); ++k) {
                if (k < perm[k]) {
                    // Majorana symmetry, conjugation matrix to introduce
                    Index alpha = deg_indices[(k+1)*nIndices - 1]; // Last = Dirac
                    Index beta  = deg_indices[(perm[k]+1)*nIndices - 1]; 
                    Index gam   = alpha.rename();
                    Index del   = beta .rename();
                    Replace(copyRes, alpha, gam);
                    Replace(copyRes, beta,  del);
                    csl::Tensor C = dirac4.C_matrix;
                    csl::Expr C1 = C({gam,  alpha});
                    csl::Expr C2 = C({beta, del});
                    csl::Expr sign = (deg_insertions[k].isComplexConjugate()
                        == deg_insertions[perm[k]].isComplexConjugate()) ?
                        CSL_M_1 : CSL_1;
                    copyRes *= csl::prod_s({sign, C1, C2});
                    csl::Refresh(copyRes);
                }
            }
        }

        csl::ForEachLeaf(copyRes, [&](csl::Expr& el)
        {
            if (not el->isIndexed())
                return;
            if (not mty::IsOfType<PolarizationField>(el)
                    or el->getParent_info()
                        != deg_insertions[perm[0]].getParent_info()) {
                csl::IndexStructure& structure
                    = el->getIndexStructureView();
                csl::Expr init = DeepCopy(el);
                for (auto& index : structure)
                    for (size_t n = 0; n != nIndices; ++n) {
                        bool breakValue = false;
                        for (size_t i = 0; i != perm.size(); ++i)  {
                            if (index == deg_indices[i*nIndices + n]
                                    and deg_indices[i*nIndices+n] 
                                        != deg_indices[perm[i]*nIndices+n]) {
                                // bool sign = index.getSign();
                                index = deg_indices[perm[i]*nIndices + n];
                                // index.setSign(sign);
                                breakValue = true;
                                break;
                            }
                        }
                        if (breakValue)
                            break;
                    }
            }
        });
        if (fermionic) {
            copyRes = csl::int_s(sign) * copyRes;
            ++count;
            if (count % 2 == 0)
                sign *= -1;
        }
        terms.push_back(DeepRefreshed(copyRes));
    }

    return terms;
}

csl::vector_expr AmplitudeCalculator::applyAllExternalSymmetries(
        csl::vector_expr const& init,
        std::vector<std::vector<size_t>>::iterator first,
        std::vector<std::vector<size_t>>::iterator last) const
{
    if (first == last)
        return init;
    csl::vector_expr res;
    res.reserve(init.size()*first->size());
    auto inserter = std::back_inserter(res);
    for (const auto& expr : init) {
        csl::vector_expr applied = applyExternalSymmetries(expr, *first);
        for (const auto& newExpr : applied)
            *inserter = newExpr;
    }
    return applyAllExternalSymmetries(res, ++first, last);
}

void AmplitudeCalculator::calculateResult()
{
    if (not ruleMode) {
        calculateResultWithRules();
        return;
    }

    // std::cout << std::endl;
    // std::cout << std::endl;
    initLSZInsertion();
    if (mty::option::applyInsertionOrdering)
        orderInsertions();
    // std::cout << "LSZ = " << std::endl;
    // std::cout << *LSZInsertions << std::endl;
        
    csl::PseudoIntegral integral;
    integral.addTerm(LSZInsertions.value());

    csl::vector_expr fieldVertices(fieldInsertions.size()
                                  + interactionTerms.size());
    // std::cout << "VERTICES " << std::endl;
    for (size_t i = 0; i != fieldInsertions.size(); ++i) {
        if (not mty::option::applyDerivatives)
            fieldInsertions[i].setDerivativeStructure(csl::IndexStructure());
        fieldVertices[i] = fieldInsertions[i].copy();
        // std::cout << fieldVertices[i] << std::endl;
    }
    std::map<csl::Tensor, size_t> vertexIds;
    for (size_t i = 0; i != interactionTerms.size(); ++i) {
        integral.addTerm(csl::vectorintegral_s(vertices[i]));
        vertexIds[vertices[i]] = i;
        fieldVertices[fieldInsertions.size() + i] 
            = interactionTerms[i]->getFieldProduct();
        applyDerivativesInStructure(fieldVertices[fieldInsertions.size()+i]);
        // std::cout << fieldVertices[fieldInsertions.size() + i] << std::endl;
    }

    fieldVertices.insert(                         
            fieldVertices.begin(),
            csl::pow_s(CSL_I, csl::int_s(interactionTerms.size())));
    // std::cout << "i FACTOR" << std::endl;
    // std::cout << csl::pow_s(CSL_I, interactionTerms.size()) << std::endl;
    auto fieldProd = mty::wick_s(csl::prod_s(fieldVertices, true));
    amplitude = wick::WickCalculator::getDiagrams(
            fieldProd,
            vertexIds
            );
    amplitude.insertions = fieldInsertions;

    csl::vector_expr results;
    std::vector<std::vector<size_t>> externalSym 
        = getExternalSymmetries(fieldProd);
    CHECK_POINT_BUILD(FRCalc, csl::Expr)
    for (size_t i = 0; i != amplitude.size(); ++i) {
        csl::Expr res = integral.applyOn(amplitude.expressions[i]);
        csl::ScopedProperty p(&csl::option::checkCommutations, false);
        if (res != CSL_0) {
            csl::Index ind = csl::Minkowski.generateIndex();
            csl::Expr PSum = mty::getMomentumSum(fieldInsertions,
                                             momentum,
                                             ind);
            res = res->suppressTerm(csl::diracdelta_s(PSum).get());
            if (mty::option::applyFactorsAndSymmetries) {
                for (const auto& interac : interactionTerms) {
                    res = interac->applyFactorAndSymmetriesOn(res);
                }
            }
            Evaluate(res);
            if (res != CSL_0) {
                if (not ruleMode and mty::option::searchAbreviations)
                    findAbreviations(res);
                if (ruleMode) {
                    csl::vector_expr appliedSym 
                        = applyAllExternalSymmetries({res}, 
                                                     externalSym.begin(),
                                                     externalSym.end());
                    res = sum_s(appliedSym);
                }
                if (res->getType() == csl::Type::Sum) {
                    csl::Comparator::setDummyVecIntComparisonActive(true);
                    res = csl::Refreshed(res);
                    csl::Comparator::setDummyVecIntComparisonActive(false);
                }
                CHECK_POINT_ADD_TO(FRCalc, res)
                expandGammaMatrices(res);
                simplifyConjugationMatrix(res);
                simplifyFermionChains(res);
                results.push_back(res);
            }
        }
        amplitude.expressions[i] = res;
    }
    // std::cin.get();

    result = sum_s(results);
}

void AmplitudeCalculator::calculateResultWithRules()
{
    initLSZInsertion();
    if (mty::option::applyInsertionOrdering)
        orderInsertions();

    csl::PseudoIntegral integral;
    integral.addTerm(LSZInsertions.value());

    csl::vector_expr fieldVertices(fieldInsertions.size()
                                  + feynmanRules.size());
    for (size_t i = 0; i != fieldInsertions.size(); ++i) {
        fieldVertices[i] = fieldInsertions[i].copy();
    }
    std::map<csl::Tensor, size_t> vertexIds;
    std::vector<csl::Tensor> witnessVertices;
    initMomentumVertices(vertexIds, witnessVertices, integral, fieldVertices);
    std::vector<FeynruleMomentum> momentumMapping
        = {FeynruleMomentum(feynmanRules, vertices)};
    auto fieldProd = mty::wick_s(csl::prod_s(fieldVertices, true));
    if (fieldProd == CSL_0) {
        result = CSL_0;
        return;
    }
    amplitude = wick::WickCalculator::getDiagrams(
            fieldProd,
            vertexIds,
            momentumMapping,
            true,
            false);
    amplitude.insertions = fieldInsertions;
    csl::Expr firstField;
    csl::FirstOfLeaf(fieldProd,
    [&](csl::Expr const& expr) {
      if (IsOfType<QuantumField>(expr) and
              ConvertToPtr<QuantumField>(expr)->isExternal()) {
        firstField = expr;
        return true;
      }
      return false;
    });

    csl::vector_expr results;
    std::vector<std::vector<size_t>> externalSym 
        = getExternalSymmetries(fieldProd);

    for (size_t i = 0; i != amplitude.size(); ++i) {
        applyMomentumVertices(
                witnessVertices, 
                momentumMapping[i], 
                amplitude.expressions[i]
                );
        csl::Expr res = 1 
             / csl::int_s(amplitude.diagrams[i]->getTotalDegeneracyFactor())
             * integral.applyOn(amplitude.expressions[i]);
        csl::ScopedProperty p(&csl::option::checkCommutations, false);
        DeepRefresh(res);
        if (res != CSL_0) {
            csl::Index ind = csl::Minkowski.generateIndex();
            csl::Expr PSum = mty::getMomentumSum(fieldInsertions,
                                             momentum,
                                             ind);
            csl::Expr diracDelta = csl::diracdelta_s(PSum);
            csl::ForEachNode(res, [&](csl::Expr& el)
            {
                if (*el == diracDelta)
                    el = CSL_1;
            });
            //simplifyImpulsions(res);
            csl::ForEachNodeCut(res, [&](csl::Expr& el)
            {
                if (csl::IsProd(el)) {
                    for (const auto &arg : el)
                        if (csl::IsVectorIntegral(arg)) {
                            el *= CSL_I * CSL_PI * CSL_PI;
                            return true;
                        }
                }
                return false;
            });

            suppressDiracDelta(
                    res,
                    mty::getMomentumSum(fieldInsertions,
                                        momentum,
                                        ind));
            if (res != CSL_0) {
                HEPAssert(csl::TestIndexSanity(res),
                        mty::error::RuntimeError,
                        "Bad index property for " + toString(res))
                if (mty::option::simplifyAmplitudes)
                    simplify(res);
                HEPAssert(csl::TestIndexSanity(res),
                        mty::error::RuntimeError,
                        "Bad index property for " + toString(res))
                results.push_back(res);
            }
        }
        amplitude.expressions[i] = res;
    }

    result = sum_s(results);
}

bool AmplitudeCalculator::findAbreviations(csl::Expr& res)
{
    if (res->getType() == csl::Type::Sum) {
        for (auto& arg : *res)
            findAbreviations(arg);
        return false;
    }
    if (res->getType() != csl::Type::Prod)
        return false;
    for (auto& arg : *res)
        findAbreviations(arg);

    std::vector<size_t> constants;
    for (size_t i = 0; i != res->size(); ++i) {
        if ((*res)[i]->getPrimaryType() != csl::PrimaryType::Numerical
                and (*res)[i] != CSL_I
                and csl::AllOfLeafs((*res)[i], [](const csl::Expr& el) 
                    {
                        return (el->getPrimaryType() == csl::PrimaryType::Numerical
                                or (el->getPrimaryType() 
                                    == csl::PrimaryType::Literal
                                and el->getType() != csl::Type::Variable
                                and el != csl::DMinko));
                    }))
        constants.push_back(i);
    }

    if (constants.empty())
        return false;
    if (constants.size() > 1
            or res->getArgument(constants[0])->size() > 0) {
        csl::vector_expr args;
        args.reserve(constants.size());
        for (size_t pos : constants) {
            args.push_back((*res)[pos]);
            (*res)[pos] = CSL_1;
        }
        csl::Expr ab = csl::Abbrev::makeAbbreviation(prod_s(args, true));
        (*res)[constants[0]] = ab;
        res = csl::Refreshed(res);
    }
    return true;
}

void AmplitudeCalculator::computeDiracTraces(csl::Expr& res)
{
    csl::DeepExpandIf_lock(res, [&](csl::Expr const& expr) {
        if (not expr->isIndexed())
            return false;
        return (expr->getParent_info()->getDim(&mty::dirac4) == 2);
    });

    res = mty::dirac4.calculateTrace(res);
}

bool AmplitudeCalculator::findIntegralAbbreviation(csl::Expr& res)
{
    csl::ForEachNode(res, [&](csl::Expr& node)
    {
      if (node->getType() == csl::Type::Prod) {
        for (auto &arg : node)
            if (arg->getType() == csl::Type::VectorIntegral) {
                if (mty::option::computeFirstIntegral)
                    arg = FeynmanIntegral::replaceIntegral(arg);
                else 
                    arg = csl::Abbrev::makeAbbreviation("INT", arg);
                Refresh(node);
                break;
            }
      }
    });
    return true;
}

void AmplitudeCalculator::findExternalAbbreviation(csl::Expr& res)
{
    if (res->size() > 0) {
        for (size_t i = 0; i != res->size(); ++i) 
            findExternalAbbreviation(res[i]);
        if (csl::IsProd(res)) {
            csl::vector_expr external;
            for (size_t i = 0; i != res->size(); ++i) {
                if (mty::IsOfType<mty::PolarizationField>(res[i])) {
                  external.push_back(res[i]);
                  res[i] = CSL_1;
                }
            }
            if (not external.empty()) {
                res = prod_s(csl::Abbrev::makeAbbreviation("EXT", prod_s(external)),
                            csl::Refreshed(res));
            }
        }
    }
}

} // End of namespace mty
