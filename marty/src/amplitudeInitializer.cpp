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

 #include "amplitudeInitializer.h"
#include "feynmanDiagram.h"
#include "diracology.h"
#include "wick.h"
#include "amplitudeSimplification.h"
#include "feynruleMomentum.h"
#include "polarization.h"

namespace mty {

    AmplitudeInitializer::AmplitudeInitializer(
            mty::Model                       const *t_model,
            mty::FeynOptions                 const &t_options,
            Kinematics                       const &t_kinematics,
            std::vector<mty::QuantumField>   const &t_insertions,
            std::vector<mty::Lagrangian::TermType> &t_lagrangian
            )
        :model(t_model),
        options(t_options),
        insertions(t_insertions),
        kinematics(t_kinematics),
        feynRuleMode(false),
        lagrangian(t_lagrangian),
        feynmanRules(defaultFR)
    {

    }

    AmplitudeInitializer::AmplitudeInitializer(
            mty::Model                       const *t_model,
            mty::FeynOptions                 const &t_options,
            Kinematics                       const &t_kinematics,
            std::vector<mty::QuantumField>   const &t_insertions,
            std::vector<mty::FeynmanRule const*>   &t_feynmanRules
            )
        :model(t_model),
        options(t_options),
        insertions(t_insertions),
        kinematics(t_kinematics),
        feynRuleMode(true),
        lagrangian(defaultL),
        feynmanRules(t_feynmanRules)
    {

    }

    Amplitude AmplitudeInitializer::getAmplitude(
            std::vector<std::vector<size_t>> const &terms
            )
    {
        csl::ScopedProperty p1(&mty::option::excludeTadpoles,
                mty::option::excludeTadpoles 
                    || !(options.getTopology() & Topology::Tadpole));
        csl::ScopedProperty p2(&mty::option::excludeMassCorrections,
                mty::option::excludeMassCorrections 
                    || !(options.getTopology() & Topology::Mass));
        csl::ScopedProperty p3(&mty::option::excludeTriangles,
                mty::option::excludeTriangles 
                    || !(options.getTopology() & Topology::Triangle));
        csl::ScopedProperty p4(&mty::option::excludeBoxes,
                mty::option::excludeBoxes 
                    || !(options.getTopology() & Topology::Box));
        csl::ScopedProperty p5(&mty::option::excludePentagons,
                mty::option::excludePentagons 
                    || !(options.getTopology() & Topology::Pentagon));

        Amplitude res = (feynRuleMode) ? 
            ruledCalculation(terms) : fullQuantumCalculation(terms);
        options.applyFilters(res.diagrams);
        return res;
    }

    Amplitude AmplitudeInitializer::fullQuantumCalculation(
            std::vector<std::vector<size_t>> const &terms
            )
    {
        csl::Expr LSZInsertion = getLSZInsertions(
                insertions,
                kinematics.getMomenta(),
                true
                );
        if (options.orderInsertions)
            orderInsertions(insertions, kinematics);
            
        csl::PseudoIntegral integral;
        integral.addTerm(LSZInsertion);

        std::vector<csl::Expr> fieldVertices(insertions.size());
        for (size_t i = 0; i != insertions.size(); ++i) {
            fieldVertices[i] = insertions[i].copy();
        }
        Amplitude res { options, kinematics };
        for (size_t index = 0; index != terms.size(); ++index) {
            const auto &termPos = terms[index];
            std::vector<csl::Tensor> vertices = getVertices(termPos.size());
            for (size_t i = 0; i != termPos.size(); ++i) {
                lagrangian[termPos[i]]->setPoint(vertices[i]);
            }
            std::map<csl::Tensor, size_t> vertexIds;
            auto cpyFieldVertices = fieldVertices;
            cpyFieldVertices.resize(fieldVertices.size() + termPos.size());
            csl::PseudoIntegral integral_cpy = integral;
            for (size_t i = 0; i != termPos.size(); ++i) {
                integral_cpy.addTerm(csl::vectorintegral_s(vertices[i]));
                vertexIds[vertices[i]] = i;
                cpyFieldVertices[insertions.size() + i] 
                    = lagrangian[termPos[i]]->getFieldProduct();
                simpli::applyDerivativesInStructure(
                        cpyFieldVertices[insertions.size()+i]
                        );
            }

            cpyFieldVertices.push_back(
                    csl::pow_s(CSL_I, csl::int_s(termPos.size())));
            auto fieldProd = mty::wick_s(csl::prod_s(cpyFieldVertices, true));
            std::vector<std::vector<size_t>> externalSym
                = getExternalSymmetries(fieldProd);
            std::vector<FeynmanDiagram> amplitude 
                = wick::WickCalculator::getDiagrams(
                    model,
                    fieldProd,
                    vertexIds
                    );
            for (size_t i = 0; i != amplitude.size(); ++i) {
                simplifyFullQuantumCalculation(
                        amplitude[i],
                        integral_cpy,
                        termPos,
                        externalSym
                        );
            }
            removeZeroDiagrams(amplitude);
            res.add(amplitude);
        }

        return res;
    }

    Amplitude AmplitudeInitializer::ruledCalculation(
            std::vector<std::vector<size_t>> const &terms
            )
    {
        csl::Expr LSZInsertion = getLSZInsertions(
                insertions, 
                kinematics.getMomenta(),
                false);
        if (options.orderInsertions)
            orderInsertions(insertions, kinematics);

        csl::PseudoIntegral integral;
        integral.addTerm(LSZInsertion);

        std::vector<csl::Expr> fieldVertices(insertions.size());
        for (size_t i = 0; i != insertions.size(); ++i) {
            fieldVertices[i] = insertions[i].copy();
        }
        Amplitude res { options, kinematics };
        for (size_t index = 0; index != terms.size(); ++index) {
            ///////////////////////////////:
            // This line is important as it disables all index contractions
            // This allows to disbable index replacements from deltas and keep
            // all fields with their initial indices, which is needed to
            // recognize a field between the initial Feynman rules and the 
            // initial non-simplified amplitude (in particular for 
            // initMomentumVertices() and applyMomentumVertices())
            const bool selfContractionEnabled = csl::option::applySelfContractions;
            csl::option::applySelfContractions = false;
            ///////////////////////////////:
            
            const auto &termPos = terms[index];
            std::vector<FeynmanRule> localRules;
            localRules.reserve(termPos.size());
            for (size_t i : termPos) {
                localRules.push_back(*feynmanRules[i]);
            }
            std::vector<csl::Tensor> vertices = getVertices(termPos.size());
            std::map<csl::Tensor, size_t> vertexIds;
            std::vector<csl::Tensor> witnessVertices;
            auto fieldVertices_cpy = fieldVertices;
            auto integral_cpy = integral;
            initMomentumVertices(
                    localRules,
                    vertexIds, 
                    vertices, 
                    witnessVertices, 
                    integral_cpy, 
                    fieldVertices_cpy);
            std::vector<FeynruleMomentum> momentumMapping
                = {FeynruleMomentum(localRules, vertices)};
            auto fieldProd = mty::wick_s(csl::prod_s(fieldVertices_cpy, true));
            if (fieldProd == CSL_0) {
                continue;
            }
            std::vector<mty::FeynmanDiagram> amplitude 
                = wick::WickCalculator::getDiagrams(
                    model,
                    fieldProd,
                    vertexIds,
                    momentumMapping,
                    true,
                    false);
            options.applyFilters(amplitude);

            ////////////////////////////////////////
            // Setting this option to its old value
            csl::option::applySelfContractions = selfContractionEnabled;
            ////////////////////////////////////////
            for (size_t i = 0; i != amplitude.size(); ++i) {
                simplifyRuledCalculation(
                        amplitude[i],
                        integral_cpy,
                        witnessVertices,
                        momentumMapping[i]
                        );
            }
            options.applyFilters(amplitude);
            removeZeroDiagrams(amplitude);
            if (!amplitude.empty() && options.verboseAmplitude) {
                const auto plural = (amplitude.size() == 1) ? "" : "s";
                std::cout << amplitude.size() << " new particle amplitude"
                    << plural << " found" << " (set of vertices " << index+1 
                    << " / " << terms.size() << ")\n";
            } 
            res.add(amplitude);
        }
        if (options.verboseAmplitude) {
            std::cout << res.size() << " total particle amplitudes found.\n";
        }
        return res;
    }

    void AmplitudeInitializer::simplifyFullQuantumCalculation(
                mty::FeynmanDiagram            &diagram,
                csl::PseudoIntegral      const &integral,
                std::vector<size_t>      const &posTerms,
                std::vector<std::vector<size_t>> &externalSym
                )
    {
        csl::Expr res = integral.applyOn(diagram.getExpression());
        csl::ScopedProperty p(&csl::option::checkCommutations, false);
        if (res != CSL_0) {
            csl::Index ind = csl::Minkowski.generateIndex();
            csl::Expr PSum = mty::getMomentumSum(
                    insertions,
                    kinematics.getMomenta(),
                    ind);
            res = res->suppressTerm(csl::diracdelta_s(PSum).get());
            if (mty::option::applyFactorsAndSymmetries) {
                for (size_t i : posTerms) {
                    res = lagrangian[i]->applyFactorAndSymmetriesOn(res);
                }
            }
            Evaluate(res);
            if (res != CSL_0) {
                csl::vector_expr appliedSym = applyAllExternalSymmetries(
                            {res},
                            externalSym.begin(),
                            externalSym.end()
                            );
                res = sum_s(appliedSym);
                if (res->getType() == csl::Type::Sum) {
                    csl::Comparator::setDummyVecIntComparisonActive(true);
                    res = csl::Refreshed(res);
                    csl::Comparator::setDummyVecIntComparisonActive(false);
                }
                csl::Expr cpy = csl::DeepCopy(res);
                simpli::simplify(
                        cpy,
                        insertions,
                        kinematics.getMomenta(),
                        options,
                        simpli::FeynmanRule
                        );
                // simpli::expandGammaMatrices(res);
                // simpli::simplifyConjugationMatrix(res);
                // simpli::simplifyFermionChains(res);
                res = cpy;
            }
        }
        diagram.getExpression() = res;
    }

    void AmplitudeInitializer::simplifyRuledCalculation(
                mty::FeynmanDiagram            &diagram,
                csl::PseudoIntegral      const &integral,
                std::vector<csl::Tensor> const &witnessVertices,
                FeynruleMomentum               &momentumMapping
            )
    {
        applyMomentumVertices(
                witnessVertices, 
                momentumMapping, 
                diagram.getExpression()
                );
        csl::Expr res = 1 
             / csl::int_s(diagram.getDiagram()->getTotalDegeneracyFactor())
             * integral.applyOn(diagram.getExpression());
        csl::ScopedProperty p(&csl::option::checkCommutations, false);
        DeepRefresh(res);
        if (res != CSL_0) {
            csl::Index ind = csl::Minkowski.generateIndex();
            csl::Expr PSum = mty::getMomentumSum(
                    insertions,
                    kinematics.getMomenta(),
                    ind);
            simpli::suppressDiracDelta(res, PSum);
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

            if (res != CSL_0) {
                HEPAssert(csl::TestIndexSanity(res),
                        mty::error::RuntimeError,
                        "Bad index property for " + toString(res))
                if (options.simplifyAmplitudes)
                    simpli::simplify(
                            res, 
                            insertions, 
                            kinematics.getMomenta(), 
                            options,
                            simpli::Amplitude
                            );
                HEPAssert(csl::TestIndexSanity(res),
                        mty::error::RuntimeError,
                        "Bad index property for " + toString(res))
            }
        }
        diagram.getExpression() = res;
    }

    void AmplitudeInitializer::removeZeroDiagrams(
            std::vector<FeynmanDiagram> &diagrams
            )
    {
        auto last = std::remove_if(begin(diagrams), end(diagrams),
                [&](FeynmanDiagram const &diag) {
                    return diag.isZero();
                });
        diagrams.erase(last, end(diagrams));
    }

    csl::Expr AmplitudeInitializer::getLSZInsertions(
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &momenta,
            bool                                  feynRuleCalculation
            )
    {
        HEPAssert(insertions.size() == momenta.size(),
                mty::error::ValueError,
                "Expecting the same number of fields and momenta, " + 
                std::to_string(insertions.size()) + " and " +
                std::to_string(momenta.size()) + " found.")
        // Gathering LSZ insertions of fields
        std::vector<csl::Expr> terms(insertions.size());
        bool firstFermionFound = false;
        for (size_t i = 0; i != insertions.size(); ++i) {
            auto& field = insertions[i];
            if (!firstFermionFound && field.isFermionic() && !field.isSelfConjugate()) {
                firstFermionFound = true;
                terms[i] = field.getLSZInsertion(
                        momenta[i], feynRuleCalculation, true);
            }
            else {
                terms[i] = field.getLSZInsertion(
                        momenta[i], feynRuleCalculation);
            }
        }
        csl::Expr res = csl::prod_s(terms);
        if (!feynRuleCalculation)
            mty::simpli::findExternalAbbreviation(res);
        return res;
    }

    std::vector<csl::Tensor> AmplitudeInitializer::getVertices(size_t N)
    {
        std::vector<csl::Tensor> vertices(N);
        for (size_t i = 0; i != N; ++i) {
            vertices[i] = csl::Tensor("V_" + std::to_string(i), &csl::Minkowski);
        }
        return vertices;
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

    void AmplitudeInitializer::orderInsertions(
            std::vector<mty::QuantumField> &insertions,
            Kinematics                     &kinematics
            )
    {
        std::vector<size_t> indices(kinematics.size());
        std::iota(begin(indices), end(indices), 0);
        for (size_t i = 0; i != insertions.size(); ++i)  {
            size_t mini = i;
            for (size_t j = i+1; j < insertions.size(); ++j)  {
                if (compareFields(insertions[j], insertions[mini], i))
                    mini = j;
            }
            if (mini != i) {
                std::swap(insertions[i], insertions[mini]);
                std::swap(indices[i], indices[mini]);
            }
        }
        kinematics.applyPermutation(indices);
    }


    void AmplitudeInitializer::initMomentumVertices(
            std::vector<FeynmanRule>       &localRules,
            std::map<csl::Tensor, size_t>  &vertexIds,
            std::vector<csl::Tensor> const &vertices,
            std::vector<csl::Tensor>       &witnessVertices,
            csl::PseudoIntegral            &integral,
            std::vector<csl::Expr>         &fieldVertices
            )
    {
        size_t index = 0;
        for (size_t i = 0; i != localRules.size(); ++i) {
            for (size_t j = 0; j != localRules[i].getSize(); ++j)
                witnessVertices.push_back(csl::tensor_s(
                            "W_"+toString(index++),
                            &csl::Minkowski));
        }
        auto firstWitness = witnessVertices.begin();
        fieldVertices.resize(fieldVertices.size() + localRules.size());
        for (size_t i = 0; i != localRules.size(); ++i) {
            auto lastWitness = firstWitness + localRules[i].getSize();
            integral.addTerm(csl::vectorintegral_s(vertices[i]));
            vertexIds[vertices[i]] = i;
            fieldVertices[insertions.size() + i] 
                = localRules[i].getFieldProduct(
                        vertices[i],
                        firstWitness,
                        lastWitness);
        }
    }

    std::vector<std::vector<size_t>> AmplitudeInitializer::getExternalSymmetries(
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
        if (insertions.size() < 3)
            return res;

        auto isFirst = [&](QuantumField const& field)
        {
            return wick::areDegenerate(field, firstField)
                and field.getPoint().get() == firstField.getPoint().get();
        };
        std::vector<size_t> indicesLeft = csl::range(insertions.size());
        for (size_t i = 0; i < indicesLeft.size()-1; ++i) {
            std::vector<size_t> intermediate;
            QuantumField field = insertions[indicesLeft[i]];
            if (not isFirst(field))
                intermediate.push_back(indicesLeft[i]);
            for (size_t j = i+1; j < indicesLeft.size(); ++j) {
                if (wick::areDegenerate(field,
                            insertions[indicesLeft[j]])) {
                    if (not isFirst(insertions[indicesLeft[j]]))
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


    std::vector<csl::Expr> AmplitudeInitializer::applyExternalSymmetries(
            csl::Expr    const& res,
            std::vector<size_t> perm
            ) const
    {
        const size_t nIndices = insertions[perm[0]].
            getIndexStructureView().size();

        std::vector<mty::QuantumField> deg_insertions(perm.size());
        std::vector<csl::Tensor>       deg_momentum(perm.size());
        std::vector<csl::Index>        deg_indices(perm.size() * nIndices);

        for (size_t i = 0; i != perm.size(); ++i) {
            deg_momentum[i] = kinematics.getMomenta()[perm[i]];
            deg_insertions[i] = insertions[perm[i]];
            for (size_t j = 0; j != nIndices; ++j) {
                deg_indices[nIndices*i + j] =
                    deg_insertions[i].getIndexStructureView()[j];
            }
        }

        size_t count = 0;
        bool fermionic = not insertions[perm[0]].isBosonic();
        int sign = -1;
        std::vector<csl::Expr> terms(1, DeepRefreshed(res));

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
                        csl::Index alpha = deg_indices[(k+1)*nIndices - 1]; // Last = Dirac
                        csl::Index beta  = deg_indices[(perm[k]+1)*nIndices - 1];
                        csl::Index gam   = alpha.rename();
                        csl::Index del   = beta .rename();
                        Replace(copyRes, alpha, gam);
                        Replace(copyRes, beta,  del);
                        csl::Tensor C = mty::dirac4.C_matrix;
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
                if (not mty::IsOfType<mty::PolarizationField>(el)
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

    std::vector<csl::Expr> AmplitudeInitializer::applyAllExternalSymmetries(
            std::vector<csl::Expr>             const& init,
            std::vector<std::vector<size_t>>::iterator first,
            std::vector<std::vector<size_t>>::iterator last
            ) const
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


    void AmplitudeInitializer::applyMomentumVertices(
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

} // namespace mty
