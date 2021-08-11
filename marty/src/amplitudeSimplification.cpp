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

#include "amplitudeSimplification.h"
#include "polarization.h"
#include "mrtInterface.h"
#include "colorSpace.h"
#include "diracology.h"
#include "fermionChain.h"
#include "feynmanIntegral.h"
#include "feynOptions.h"
#include "momentumConservation.h"
#include "propagator.h"
#include "mrtOptions.h"
#include "mrtError.h"
#include "dimensionalRegularization.h"
#include <sgl.h>

namespace mty::simpli {

    bool mayBeSimplified(csl::Expr const &expr)
    {
        if (IsOfType<mty::Propagator>(expr))
            return false;
        if (csl::IsPow(expr)
                and IsOfType<mty::Propagator>(expr[0])
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

    void findExternalAbbreviation(csl::Expr &expr) 
    {
        csl::ForEachNode(expr, [&](csl::Expr &sub) {
            if (csl::IsProd(sub)) {
                std::vector<csl::Expr> external;
                for (size_t i = 0; i != sub->size(); ++i) {
                    if (mty::IsOfType<mty::PolarizationField>(sub[i])) {
                        external.push_back(sub[i]);
                        sub[i] = CSL_1;
                    }
                }
                if (!external.empty()) {
                    sub = prod_s(csl::Abbrev::makeAbbreviation(
                                "EXT", prod_s(external)), csl::Refreshed(sub));
                }
            }
        });
    }

    void abbreviateIntegral(csl::Expr &res)
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

    static bool isMomentum(csl::Expr &tensor)
    {
        if (!csl::IsIndicialTensor(tensor))
            return false;
        const auto& spaces = tensor->getParent_info()->getSpace();
        if (spaces.size() != 1 || spaces[0] != &csl::Minkowski)
            return false;
        const auto &name = tensor->getName();
        if (name[0] != 'p' && name[0] != 'k' && name[0] != 'q')
            return false;
        return true;
    }

    void abbreviateAll(csl::Expr &res)
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

    bool findAbbreviations(csl::Expr& res)
    {
        if (res->getType() == csl::Type::Sum) {
            for (auto& arg : *res)
                findAbbreviations(arg);
            return false;
        }
        if (res->getType() != csl::Type::Prod)
            return false;
        for (auto& arg : *res)
            findAbbreviations(arg);

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

    void applyDerivativesInStructure(
            csl::Expr& expression
            )
    {
        csl::ForEachNode(
            expression,
            [&](csl::Expr& el) {
              if (el->getType() == csl::Type::TDerivativeElement
                      and IsOfType<mty::QuantumField>(el->getOperand())) {
                mty::QuantumField field = ConvertTo<mty::QuantumField>(
                        el->getOperand());
                field.addDerivative(el->getIndexStructureView()[0]);
                el = field.copy();
              }
            });
    }

    void suppressDiracDelta(
            csl::Expr       &expr,
            csl::Expr const &PSum
            )
    {
        csl::Expr diracDelta = csl::diracdelta_s(PSum);
        csl::ForEachNode(expr, [&](csl::Expr &sub)
        {
            if (*sub == diracDelta) {
                sub = CSL_1;
            }
        });
    }

    ///////////////////////////////////////////////////
    /*************************************************/
    // Simplification functions for color structures //
    /*************************************************/
    ///////////////////////////////////////////////////

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

    color::ColorSpace const *isColorStructure(csl::Expr const &expr)
    {
        if (csl::AnyOfLeafs(expr, [&](csl::Expr const &sub) {
                return IsOfType<PolarizationField>(sub)
                    or sub->getName() == "EXT";
                }))
        {
            return nullptr;
        }
        if (csl::IsIndicialTensor(expr)) {
            return isColorStructure(expr->getIndexStructureView());
        }
        color::ColorSpace const *color = nullptr;
        csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
        {
            if (csl::IsIndicialTensor(sub)) {
                if (auto space = isColorStructure(sub->getIndexStructureView()))
                    color = space;
            }
        });
        return color;
    }

    csl::IndexStructure colorStructure(
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

    bool factorIndicial(csl::Expr &res)
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
            factorIndicial(toAbbreviate);
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

    bool commonIndex(
            std::vector<size_t> const &A,
            std::vector<size_t> const &B
            )
    {
        return std::any_of(begin(A), end(A), [&](size_t i) {
            return end(B) != std::find(begin(B), end(B), i);
        });
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

    bool expandInProd(
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


    bool expandColorIndices(csl::Expr &res)
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
                    expanded = expandInProd(prod, [c](csl::Expr const &sub) {
                        return colorStructure(sub, c);
                    }) || expanded;
            });
        }

        return expanded;
    }


    bool simplifyColorWeights(csl::Expr &expr)
    {
        auto hasColor = csl::AnyOfLeafs(expr, [&](csl::Expr const &sub)
        {
            return csl::IsIndicialTensor(sub) && isColorStructure(sub);
        });
        if (!hasColor)
            return false;
        csl::Expr newRes = CalculateColorTrace(expr);
        bool simplified = (expr != newRes);
        expr = newRes;

        return simplified;
    }

    bool findColorAbbreviation(csl::Expr& expr)
    {
        bool simplified = false;
        csl::ForEachNodeCut(expr, [&](csl::Expr &sub)
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
        csl::Refresh(expr);

        return simplified;
    }

    ///////////////////////////////////////////////////
    /*************************************************/
    // Simplification functions for gamma / Minkowski//
    /*************************************************/
    ///////////////////////////////////////////////////

    static bool hasCommonIndex(
            csl::Expr  const &A,
            csl::Expr  const &B,
            csl::Space const *space = nullptr
            )
    {
        csl::IndexStructure const &Aindex = 
            csl::IsIndicialTensor(A) ?
            A->getIndexStructureView()
            :A->getIndexStructure();
        csl::IndexStructure const &Bindex =
            csl::IsIndicialTensor(B) ?
            B->getIndexStructureView()
            :B->getIndexStructure();
        for (const auto &i : Aindex) {
            if (space && i.getSpace() != space)
                continue;
            for (const auto &j : Bindex) {
                if (i == j) {
                    return true;
                }
            }
        }
        return false;
    }

    bool expandMinkoStructures(csl::Expr &expr)
    {
        const bool exp1 = expandMinkoMetric(expr);
        const bool exp2 = expandMinkoEpsilon(expr);
        return exp1 || exp2;
    }
        
    bool expandMinkoMetric(csl::Expr &expr)
    {
        csl::Tensor g = csl::Minkowski.getMetric();
        csl::Tensor d = csl::Minkowski.getDelta();
        if (!expr->dependsOn(g.get()) && !expr->dependsOn(d.get()))
            return false;
        auto init { expr };
        auto stopCondition = [&](csl::Expr const &expr) {
            return !csl::AnyOfLeafs(expr, [&](csl::Expr const &emitter) {
                        return csl::IsIndicialTensor(emitter)
                            and (emitter->getParent_info() == g.get()
                                    or emitter->getParent_info() == d.get())
                            and emitter->getFreeIndexStructure().size() < 2;
                    });
        };
        auto isEmitter = [&](csl::Expr const &emitter) {
            return csl::IsIndicialTensor(emitter)
                && (emitter->getParent_info() == g.get()
                        || emitter->getParent_info() == d.get())
                && emitter->getFreeIndexStructure().size() < 2;
        };
        auto isReceiver = [&](csl::Expr const &emitter, csl::Expr const &receiver) {
            return hasCommonIndex(emitter, receiver, &csl::Minkowski);
        };

        do {
            csl::DeepPartialExpand(expr, isEmitter, isReceiver);
        } while (!stopCondition(expr));

        csl::DeepFactor(expr);

        return (init != expr);
    }

    bool expandMinkoEpsilon(csl::Expr &expr)
    {
        csl::Tensor e = csl::Minkowski.getEpsilon();
        if (!expr->dependsOn(e.get()))
            return false;
        auto init { expr };
        auto isEmitter = [&](csl::Expr const &emitter) {
            return csl::IsIndicialTensor(emitter)
                && emitter->getParent_info() == e.get();
        };
        auto isReceiver = [&](csl::Expr const &emitter, csl::Expr const &receiver) {
            return hasCommonIndex(emitter, receiver, &csl::Minkowski);
        };

        csl::DeepPartialExpand(expr, isEmitter, isReceiver);
        csl::DeepFactor(expr);

        return (init != expr);
    }

    bool simplifyEpsilonInProd(
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


    bool simplifyEpsilon(csl::Expr &expr)
    {
        auto isEpsilon = [&](csl::Expr const &expr) {
            return csl::IsIndicialTensor(expr)
                and expr->getParent_info() == csl::Minkowski.getEpsilon().get();
        };
        bool simplified = false;
        csl::ForEachNode(expr, [&](csl::Expr &prod) {
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

    bool expandGammaMatrices(csl::Expr &expr)
    {
        csl::Expr init = expr;
        auto predicate = [&](csl::Expr const &expr)
        {
            return csl::IsIndicialTensor(expr)
                and (dirac4.isGammaTensor(expr)
                  or dirac4.getDelta().get() == expr->getParent_info()
                  or dirac4.C_matrix.get() == expr->getParent_info());
        };
        bool toExpand = csl::AnyOfNodes(expr, [&](csl::Expr const &sub) {
            return csl::IsSum(sub)
                and sub.get() != expr.get()
                and csl::AnyOfLeafs(sub, [&](csl::Expr const &leaf)
                        {return predicate(leaf);});
        });
        if (!toExpand)
            return false;
        csl::DeepExpandIf_lock(expr, predicate);
        // csl::Factor(expr);
        return init != expr;
    }

    bool simplifyFermionChains(csl::Expr &expr)
    {
        bool gamma = csl::AnyOfLeafs(expr, [&](csl::Expr const &sub)
        {
            return csl::IsIndicialTensor(sub)
                && dirac4.isGammaTensor(sub);
        });
        if (!gamma)
            return false;
        bool simplified = false;
        FermionChain chain(&dirac4);
        csl::ForEachNode(expr, [&](csl::Expr& el)
        {
            if (el->getType() == csl::Type::Prod) {
                simplified = chain.applyOn(el) || simplified;
            }
        });
        if (simplified)
            csl::DeepRefresh(expr);

        return simplified;
    }

    void expandForFermionOrdering(csl::Expr &expr)
    {
        auto isEmitter = [&](csl::Expr const &emitter) {
            if (csl::IsIndicialTensor(emitter)) {
                auto const &indices = emitter->getIndexStructureView();
                return std::any_of(indices.begin(), indices.end(),
                        [&](csl::Index const &index) {
                            return index.getSpace() == &mty::dirac4;
                        });
            }
            return false;
        };
        auto isReceiver = [&](csl::Expr const &emitter, csl::Expr const &receiver) {
            auto const &emitterIndices  = emitter->getIndexStructureView();
            auto const &receiverIndices = receiver->getIndexStructure();
            for (const auto &i : emitterIndices)
                for (const auto &j : receiverIndices)
                    if (i == j) 
                        return true;
            return false;
        };
        csl::DeepPartialExpand(expr, isEmitter, isReceiver);
    }

    void expandMomentaExperimental(
            csl::Expr &res,
            std::vector<csl::Tensor> const &momenta
            )
    {
        auto init = res;
        auto isEmitter = [&](csl::Expr const &emitter) {
            if (!csl::IsIndicialTensor(emitter))
                return false;
            const auto parent = emitter->getParent_info();
            auto const &spaces = parent->getSpace();
            if (spaces.size() != 1 || spaces[0] != &csl::Minkowski)
                return false;
            for (const auto &p : momenta)
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
    }

    void reduceTensorIntegrals(csl::Expr &expr)
    {
        csl::ForEachNode(expr, [&](csl::Expr& node)
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

    static bool compareFields(QuantumField const &A, QuantumField const &B)
    {
        if (A.isBosonic() != B.isBosonic())
            return A.isBosonic();
        if (A.isBosonic())
            return A.getSpinDimension() < B.getSpinDimension();
        return (!A.isOnShell() && B.isOnShell());
    }

    std::pair<csl::Expr, csl::Expr> getMomentumReplacement(
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            size_t                                posReplaced
            )
    {
        csl::Expr momentumSum = CSL_0;
        csl::Index mu = csl::Minkowski.generateIndex();
        for (size_t i = 0; i != insertions.size(); ++i) {
            if (i != posReplaced)
                momentumSum += (insertions[i].isIncoming()) ? 
                    csl::Tensor(momenta[i])(mu)
                    : -csl::Tensor(momenta[i])(mu);
        }
        if (insertions[posReplaced].isIncoming())
            momentumSum *= CSL_M_1;
        return {
            csl::Tensor(momenta[posReplaced])(mu),
            momentumSum
        };
    }

    void replaceMomentum(
            csl::Expr                            &init,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            size_t                                posReplaced
            )
    {
        auto [p, momentumEquivalence] = getMomentumReplacement(
                insertions, momenta, posReplaced
                );
        csl::Replace(init, p, momentumEquivalence);
    }

    void simplifyImpulsions(
            csl::Expr                            &init,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta
            )
    {
        size_t mini = 0;
        for (size_t i = 1; i != insertions.size(); ++i) {
            if (compareFields(insertions[i], insertions[mini]))
                mini = i;
        }
        replaceMomentum(init, insertions, momenta, mini);
    }

    void applyEOM(
        csl::Expr                   &ampl,
        std::vector<FermionEOMData> &onShellFermions
        )
    {
        csl::DeepExpandIf_lock(ampl, [&](csl::Expr const &expr) {
            for (const auto &eom : onShellFermions)
                if (csl::IsIndicialTensor(expr)
                    and (expr->getParent_info() == eom.p.get()
                            or mty::dirac4.isGammaTensor(expr)))
                    return true;
            return false;
        });
        csl::ForEachNode(ampl, [&](csl::Expr &sub)
        {
            if (csl::IsProd(sub)) {
                auto sub_copy = csl::DeepCopy(sub);
                sgl::TensorSet tset {
                    dirac4.gamma_chir,
                    dirac4.C_matrix,
                    dirac4.P_L,
                    dirac4.P_R,
                    {}
                };
                tset.gamma[0] = dirac4.getDelta();
                tset.gamma[1] = dirac4.gamma;
                tset.gamma[2] = dirac4.sigma;
                sgl::GExpr init = sgl::csl_to_sgl(sub, tset);
                sgl::GExpr sglTest = sgl::Simplified(init);
                sub = sgl::sgl_to_csl(sglTest, tset);
            }
        });
    }

    void applyEOM(
            csl::Expr                            &ampl,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta
            )
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
        for (const auto &field : insertions)
            if (field.isFermionic() && field.isOnShell())
                ++nOnShellFermions;
        if (nOnShellFermions == 2 && insertions.size() > 2) {
            simplifyImpulsions(ampl, insertions, momenta);
        }

        applyEOM(ampl, onShellFermions);
    }


    void simplify(
            csl::Expr                            &expr,
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            mty::FeynOptions               const &options,
            Mode                                  mode
            )
    {
        csl::ScopedProperty commut(&csl::option::checkCommutations, false);
        csl::Refresh(expr);
        if (mode != WilsonCoefficient && mode != FeynmanRule) {
            //MomentumConservater cons(insertions, momenta);
            //cons.apply(expr);
        }
        std::vector<csl::Expr> factors;
        if (csl::IsProd(expr)) {
            for (size_t i = 0; i != expr->size(); ++i) {
                if (!mayBeSimplified(expr[i])) {
                    factors.push_back(expr[i]);
                    expr[i] = CSL_1;
                }
            }
            if (!factors.empty()) {
                csl::Refresh(expr);
            }
        }
        csl::ScopedProperty prop(&mty::option::applyFermionChain, false);

        ///////////////////////

        // Warning here: The two following lines are important to ensure that
        // 1: no sub-expression is shared with another (DeepRefreshed())
        // 2: no index (name + id) is shared between two different tensors
        // (except of course two tensors contracted with each other)
        csl::DeepRefresh(expr);
        csl::RenameIndices(expr);

        ///////////////////////

        reduceTensorIntegrals(expr);
        //if (mode != SquaredAmplitude && mode != FeynmanRule)
        //    simplifyImpulsions(expr, insertions, momenta);
        expandMomentaExperimental(expr, momenta);
        size_t maxLoop = 10;
        bool simplified;
        csl::Abbrev::enableGenericEvaluation("EXT");
        csl::Evaluate(expr);
        csl::Abbrev::disableGenericEvaluation("EXT");
        size_t loop = 0;
        do {
            simplified = expandMinkoStructures(expr);
            simplified = simplifyEpsilon(expr) || simplified;
            simplified = expandGammaMatrices(expr) || simplified;
            if (loop == 0 
                    && mode != FeynmanRule 
                    && mode != SquaredAmplitude 
                    && options.orderExternalFermions)
                expandForFermionOrdering(expr);
            simplified = simplifyFermionChains(expr) || simplified;
        } while (simplified && loop++ != maxLoop);

        loop = 0;
        do {
            simplified = expandColorIndices(expr);
            simplified = simplifyColorWeights(expr) || simplified;
        } while (simplified and loop++ != maxLoop);
        if (mode != FeynmanRule && option::abbreviateColorStructures)
            findColorAbbreviation(expr);

        expandMomentaExperimental(expr, momenta);
        if (mode == Amplitude and option::applyEquationsOfMotion) {
            applyEOM(expr, insertions, momenta);
        }
        if (mode != FeynmanRule)
            findExternalAbbreviation(expr);

        ///////////////////////

        // Warning here: for now the addLocalTerms() method needs the integrals to be 
        // NOT FACTORED. Beware not to call factor() before addLocalTerms() then !
        // Otherwise local terms contributions will be numerically wrong

        if (mode != FeynmanRule) {
            addLocalTerms(expr);
            csl::DeepFactor(expr);
        }
        // std::cout << "Before Abbrevs" << '\n';
        // std::cout << expr << '\n';

        //////////////////////
        
        if (option::searchAbreviations) {
            findAbbreviations(expr);
        }
        expandMomentaExperimental(expr, momenta);
        if (mode != Amplitude)
            csl::DeepHardFactor(expr);
        else
            csl::DeepFactor(expr);
        if (!factors.empty()) {
            csl::Expr factor = csl::prod_s(factors);
            csl::ForEachNode(factor, [&](csl::Expr &sub) {
                if (IsOfType<Propagator>(sub))
                    sub = csl::Factored(csl::Expanded(csl::Evaluated(
                                    sub, 
                                    csl::eval::abbreviation
                                    )));
            });
            if (mty::option::searchAbreviations 
                    and mty::option::decomposeInOperators)
                expr *= csl::Abbrev::makeAbbreviation(factor);
            else
                expr *= factor;
        }
        csl::DeepRefresh(expr);
        // std::cout << "Factors = " << prod_s(factors) << '\n';

        if (mode == Amplitude) {
            abbreviateIntegral(expr);
            if (!mty::option::decomposeInOperators)
                abbreviateAll(expr);
            csl::DeepRefresh(expr);
        }
        // std::cout << "RES = " << '\n';
        // std::cout << expr << '\n';
    }

} // namespace mty
