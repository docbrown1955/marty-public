#include "builtinOperators.h"
#include "wilson.h"
#include "model.h"
#include "amplitude.h"
#include "insertion.h"
#include "diracology.h"
#include "kinematics.h"
#include "fermionOrder.h"

namespace mty {

    struct OpInsertion {
        mty::QuantumField field;
        csl::Tensor       momentum;
    };

    static mty::QuantumField *qf(csl::Expr &expr) 
    {
        return dynamic_cast<mty::QuantumField*>(expr.get());
    }

    static mty::QuantumField createField(
            mty::Insertion const &ins,
            csl::Tensor    const &momentum
            )
    {
        csl::Expr psi = ins.getField()->getInstance();
        qf(psi)->setIncoming(ins.isIncoming());
        qf(psi)->setParticle(ins.isParticle());
        qf(psi)->setPoint(momentum);
        return ConvertTo<QuantumField>(psi);
    }

    static csl::Expr fermionExpr(OpInsertion const &ins, bool conj)
    {
        csl::Expr psi = ins.field.getQuantumParent()->getInstance();
        qf(psi)->setIncoming(ins.field.isIncoming());
        qf(psi)->setConjugated(ins.field.isComplexConjugate());
        qf(psi)->setPoint(ins.momentum);
        if (conj != psi->isComplexConjugate())
            psi = GetComplexConjugate(psi);
        return psi;
    }

    static std::vector<OpInsertion> getOpInsertions(
            Kinematics const &kinematics
            )
    {
        std::vector<OpInsertion> res(kinematics.size());
        const auto &insertions = kinematics.getInsertions();
        const auto &momenta    = kinematics.getMomenta();
        for (size_t i = 0; i != kinematics.size(); ++i) {
            res[i] = {createField(insertions[i], momenta[i]), momenta[i]};
        }
        return res;
    }

    struct InsertionRequirement {
        int spin;
        int conjugation;
    };

    static bool requirementsSatisfied(
            std::vector<OpInsertion>          const &insertions,
            std::vector<InsertionRequirement> const &requirements
            )
    {
        if (insertions.size() != requirements.size())
            return false;
        std::vector<size_t> indicesLeft(insertions.size());
        std::iota(indicesLeft.begin(), indicesLeft.end(), 0);
        for (size_t i = 0; i != insertions.size(); ++i) {
            auto const &field = insertions[i].field;
            for (size_t j = 0; j != indicesLeft.size(); ++j) {
                auto const &req = requirements[indicesLeft[j]];
                if (!(field.getSpinDimension() == req.spin))
                    break;
                if (req.conjugation == -1 // no requirement
                        || field.isComplexConjugate() == req.conjugation) {
                    indicesLeft.erase(indicesLeft.begin() + j);
                    break;
                }
            }
        }
        return indicesLeft.empty();
    }

    std::vector<Wilson> getMagneticCoupling(
            DiracCoupling coupling,
            csl::Index       alpha,
            csl::Index       beta
            )
    {
        csl::Tensor id = dirac4.getDelta();
        csl::Tensor g5 = dirac4.gamma_chir;
        switch(coupling) {
            case DiracCoupling::S:
                return {{CSL_1, id({alpha, beta})}};
            case DiracCoupling::P: 
                return {{CSL_1, g5({alpha, beta})}};
            case DiracCoupling::L: 
                return {
                    {CSL_1,   id({alpha, beta})},
                    {CSL_M_1, g5({alpha, beta})}
                };
            case DiracCoupling::R: 
                return {
                    {CSL_1, id({alpha, beta})},
                    {CSL_1, g5({alpha, beta})}
                };
            default:
                CallHEPError(mty::error::ValueError,
                    "Value " + std::to_string(static_cast<int>(coupling))
                    + " not defined for Magnetic couplings.")
        }
        return {};
    }

    csl::Expr getMagneticGenerator(
            mty::Model const &model,
            csl::Expr        &psi_star,
            csl::Expr        &psi,
            csl::Expr        &A
            )
    {
        csl::Expr generator = CSL_1;
        if (A->getIndexStructureView().size() > 1) {
            // Chromo-magnetic operator
            auto irrep = qf(A)->getQuantumParent()->getGaugeIrrep();
            SemiSimpleAlgebra const *algebra = nullptr;
            for (const auto &rep : irrep) {
                if (rep.getDim() > 1) {
                    algebra = rep.getAlgebra();
                    break;
                }
            }
            HEPAssert(algebra,
                    mty::error::TypeError,
                    "Invalid amplitude for magnetic operator.")
            Group const *group = nullptr;
            for (const auto &g : *model.getGauge()) {
                if (g->getAlgebra() == algebra) {
                    group = g.get();
                    break;
                }
            }
            HEPAssert(group,
                    mty::error::TypeError,
                    "Invalid amplitude for magnetic operator.")
            auto T = model.getGenerator(group, psi);
            auto I = T->getSpace()[0]->generateIndex();
            auto space = T->getSpace()[1];
            auto i = space->generateIndex();
            auto j = space->generateIndex();
            A->getIndexStructureView()[0] = I;
            for (size_t k = 0; k != psi->getIndexStructureView().size(); ++k)
                if (psi->getIndexStructureView()[k].getSpace() == space) {
                    psi_star->getIndexStructureView()[k] = i;
                    psi     ->getIndexStructureView()[k] = j;
                    break;
                }
            generator = T({I, i ,j});
        }
        return generator;
    }

    static std::vector<Wilson> buildMagneticOperator(
            Model       const &model,
            OpInsertion const &leftFermion,
            OpInsertion const &rightFermion,
            OpInsertion const &vectorBoson,
            csl::Tensor        momentum,
            DiracCoupling   coupling
            )
    {
        csl::Expr psi_star = fermionExpr(leftFermion, true);
        csl::Expr psi      = fermionExpr(rightFermion, false);

        // Coupling by the identity by default 
        for (size_t i = 0; i != psi->getIndexStructureView().size(); ++i)
            csl::Replace(
                    psi_star, 
                    psi_star->getIndexStructureView()[i],
                    psi->getIndexStructureView()[i]);
        csl::Index alpha = psi_star->getIndexStructureView().back();
        csl::Index beta  = alpha.rename();
        csl::Replace(psi, alpha, beta);
        csl::Expr A = vectorBoson.field
            .getQuantumParent()->getInstance();
        A->setPoint(vectorBoson.momentum);
        csl::Index mu = A->getIndexStructureView().back();
        mu = mu.getFlipped();

        std::vector<Wilson> diracCoupling = getMagneticCoupling(
                coupling, alpha, beta
                );
        csl::Expr generator = getMagneticGenerator(model, psi_star, psi, A);
        std::vector<Wilson> res;
        res.reserve(diracCoupling.size());
        for (const auto &coup : diracCoupling)
            res.push_back({coup.coef.getCoefficient(),
                    coup.op.getExpression() * psi_star * generator * psi
                     * (momentum(mu) * A)});
        return res;
    }

    static bool areSwapped(
            std::vector<int> const &fermionOrder
            )
    {
        bool swapped = false;
        for (int fpos : fermionOrder) {
            if (fpos == 0) {
                break;
            }
            else if (fpos == 1) { 
                swapped = true; 
                break ;
            }
        }
        return swapped;
    }

    static std::array<OpInsertion, 3> getMagneticInsertions(
            std::vector<OpInsertion> const &insertions,
            bool swapped
            )
    {
        OpInsertion leftFermion, rightFermion, vectorBoson;
        bool leftFound = false;
        for (size_t i = 0; i != insertions.size(); ++i) {
            if (insertions[i].field.isFermionic()) {
                if (leftFound) rightFermion = insertions[i];
                else {
                    leftFermion = insertions[i];
                    leftFound = true;
                }
            }
            else {
                vectorBoson = insertions[i];
            }
        }
        if (swapped)
            std::swap(leftFermion, rightFermion);
        return {leftFermion, rightFermion, vectorBoson};
    }

    static void addMagneticContribution(
            std::vector<Wilson> &contributions,
            csl::Expr   const &factor,
            Model       const &model,
            OpInsertion const &leftFermion,
            OpInsertion const &rightFermion,
            OpInsertion const &vectorBoson,
            csl::Tensor        momentum,
            DiracCoupling   coupling
            )
    {
        std::vector<Wilson> newContributions = buildMagneticOperator(
                model,
                leftFermion, rightFermion, vectorBoson,
                momentum, coupling
                );
        for (const auto &contrib : newContributions) {
            contributions.push_back({
                    contrib.coef.getCoefficient()*factor,
                    contrib.op.getExpression()
                    });
        }
    }

    std::vector<Wilson> chromoMagneticOperator(
            Model     const &model,
            WilsonSet const &wilsons,
            DiracCoupling coupling
            )
    {
        auto fermionOrder = wilsons.options.getFermionOrder();
        if (fermionOrder.empty())
            fermionOrder = defaultFermionOrder(
                    wilsons.kinematics.getInsertions());
        bool swapped = areSwapped(fermionOrder);
        std::vector<OpInsertion> insertions = getOpInsertions(
                wilsons.kinematics);
        std::vector<InsertionRequirement> requirements = {
            {2, 1}, // conjugate fermion
            {2, 0}, // non-conjugate fermion
            {3, -1}   // vector boson, no conjugation requirement
        };
        if (!requirementsSatisfied(insertions, requirements))
            return {};
        auto [leftFermion, rightFermion, vectorBoson] = 
            getMagneticInsertions(insertions, swapped);
        csl::Expr vectorFactor = - CSL_1 / 2;
        csl::Expr leftFactor  = CSL_1 / 4;
        csl::Expr rightFactor = CSL_1 / 4;
        if (leftFermion.field.isIncoming() != vectorBoson.field.isIncoming())
            leftFactor *= -1;
        if (!leftFermion.field.isParticle())
            leftFactor *= -1;
        if (rightFermion.field.isIncoming() != vectorBoson.field.isIncoming())
            rightFactor *= -1;
        if (rightFermion.field.isParticle())
            rightFactor *= -1;
        std::vector<Wilson> res;
        res.reserve(6);
        addMagneticContribution(
                res, leftFactor,
                model, leftFermion, rightFermion, vectorBoson,
                leftFermion.momentum, coupling
                );
        addMagneticContribution(
                res, rightFactor,
                model, leftFermion, rightFermion, vectorBoson,
                rightFermion.momentum, coupling
                );
        addMagneticContribution(
                res, vectorFactor,
                model, leftFermion, rightFermion, vectorBoson,
                vectorBoson.momentum, coupling
                );

        return res;
    }

    static void setUpDim6FermionOrder(
            WilsonSet  const &wilsons,
            std::vector<int> &fermionOrder
            )
    {
        if (fermionOrder.empty()
                && wilsons.options.getFermionOrder().size() == 4) {
            fermionOrder = wilsons.options.getFermionOrder();
        }
        else if (fermionOrder.empty()) {
            fermionOrder = defaultFermionOrder(
                    wilsons.kinematics.getInsertions());
        }
        else if (!fermionOrder.empty() && fermionOrder.size() != 4) {
            CallHEPError(mty::error::TypeError,
                    "Expecting a fermion order for 4 fermions, vector of size "
                    + std::to_string(fermionOrder.size()) + " given.")
        }
        else {
            auto order_cpy = fermionOrder;
            std::sort(order_cpy.begin(), order_cpy.end());
            HEPAssert((order_cpy == std::vector<int>{0, 1, 2, 3}),
                    mty::error::ValueError,
                    "Fermion ordering should contain integers between 0 and 3.")
        }
    }

    static std::array<csl::Expr, 4> getExternalDim6Fermions(
            std::vector<OpInsertion> const &insertions, 
            std::vector<int>         const &fermionOrder
            )
    {
        return {
            fermionExpr(insertions[fermionOrder[0]], true),
            fermionExpr(insertions[fermionOrder[1]], false),
            fermionExpr(insertions[fermionOrder[2]], true),
            fermionExpr(insertions[fermionOrder[3]], false)
        };
    }

    static std::vector<Wilson> getDiracCurrent(
            DiracCoupling current,
            csl::Index const &a,
            csl::Index const &b,
            csl::Index const &mu,
            csl::Index const &nu
            )
    {
        csl::Index c = a.rename();
        csl::Tensor delta = dirac4.getDelta();
        csl::Tensor gamma = dirac4.gamma;
        csl::Tensor sigma = dirac4.sigma;
        csl::Tensor gamma5 = dirac4.gamma_chir;
        csl::Tensor PL = dirac4.P_L;
        csl::Tensor PR = dirac4.P_R;
        switch(current) {
            case DiracCoupling::S:
                return {{CSL_1, delta({a, b})}};
            case DiracCoupling::P:
                return {{CSL_1, gamma5({a, b})}};
            case DiracCoupling::L:
                return {
                    {CSL_1,   delta({a, b})},
                    {CSL_M_1, gamma5({a, b})}
                };
            case DiracCoupling::R:
                return {
                    {CSL_1, delta({a, b})},
                    {CSL_1, gamma5({a, b})}
                };
            case DiracCoupling::V:
                return {{CSL_1, gamma({mu, a, b})}};
            case DiracCoupling::A:
                return {{CSL_1, gamma({mu, a, c}) * gamma5({c, b})}};
            case DiracCoupling::VL:
                return {
                    {CSL_1, gamma({mu, a, c}) * delta({c, b})},
                    {CSL_M_1, gamma({mu, a, c}) * gamma5({c, b})}
                };
            case DiracCoupling::VR:
                return {
                    {CSL_1, gamma({mu, a, c}) * delta({c, b})},
                    {CSL_1, gamma({mu, a, c}) * gamma5({c, b})}
                };
            case DiracCoupling::T:
                return {{CSL_1, sigma({mu, nu, a, b})}};
            case DiracCoupling::TA:
                return {{CSL_1, sigma({mu, nu, a, c}) * gamma5({c, b})}};
            case DiracCoupling::TL:
                return {
                    {CSL_1, sigma({mu, nu, a, c}) * delta({c, b})},
                    {CSL_M_1, sigma({mu, nu, a, c}) * gamma5({c, b})}
                };
            case DiracCoupling::TR:
                return {
                    {CSL_1, sigma({mu, nu, a, c}) * delta({c, b})},
                    {CSL_1, sigma({mu, nu, a, c}) * gamma5({c, b})}
                };
        }
        return {};
    }

    static std::vector<Wilson> getDiracCurrent(
            DiracCoupling c1,
            DiracCoupling c2,
            csl::Expr   &psi1,
            csl::Expr   &psi2,
            csl::Expr   &psi3,
            csl::Expr   &psi4
            )
    {
        csl::Index a1 = psi1->getIndexStructureView().back();
        csl::Index a2 = psi2->getIndexStructureView().back();
        csl::Index a3 = psi3->getIndexStructureView().back();
        csl::Index a4 = psi4->getIndexStructureView().back();
        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index nu = csl::Minkowski.generateIndex();

        std::vector<Wilson> w1 = getDiracCurrent(c1, a1, a2, -mu, -nu);
        std::vector<Wilson> w2 = getDiracCurrent(c2, a3, a4, +mu, +nu);
        std::vector<Wilson> res;
        res.reserve(w1.size() * w2.size());
        for (const auto &c1 : w1) {
            for (const auto &c2 : w2) {
                res.push_back({
                        c1.coef.getCoefficient() * c2.coef.getCoefficient(),
                        c1.op.getExpression() * c2.op.getExpression()});
            }
        }
        return res;
    }

    static std::pair<size_t, size_t> findTwoIdenticalReps(
            mty::Irrep const &rep1,
            mty::Irrep const &rep2,
            mty::Irrep const &rep3,
            mty::Irrep const &rep4
            )
    {
        const bool s12 = (rep1 == rep2 && rep1.getDim() > 1);
        const bool s13 = (rep1 == rep3 && rep1.getDim() > 1);
        const bool s14 = (rep1 == rep4 && rep1.getDim() > 1);
        const bool s23 = (rep2 == rep3 && rep2.getDim() > 1);
        const bool s24 = (rep2 == rep4 && rep2.getDim() > 1);
        const bool s34 = (rep3 == rep4 && rep3.getDim() > 1);
        if (s12 && !s13) return {0, 1};
        if (s13 && !s12) return {0, 2};
        if (s14 && !s12) return {0, 3};
        if (s23 && !s12) return {1, 2};
        if (s24 && !s12) return {1, 3};
        if (s34 && !s13) return {2, 3};
        return {-1, -1};
    }

    static bool areIdenticalReps(
            mty::Irrep const &rep1,
            mty::Irrep const &rep2,
            mty::Irrep const &rep3,
            mty::Irrep const &rep4
            )
    {
        return rep1 == rep2 && rep1 == rep3 && rep1 == rep4
            && rep1.getDim() > 1;
    }

    csl::Index &groupIndex(
            Model const &model,
            Group const *group,
            csl::Expr   &psi
            )
    {
        auto vSpace = model.getVectorSpace(group, psi);
        for (auto &index : psi->getIndexStructureView()) {
            if (index.getSpace() == vSpace)
                return index;
        }
        CallHEPError(mty::error::RuntimeError,
                "Index for field " + toString(psi) + " in group "
                + group->getName() + " not found.")
        static csl::Index def;
        return def;
    }

    static csl::Expr getTrivialColorCouplings(
            Model const &model,
            csl::Expr   &psi1,
            csl::Expr   &psi2,
            csl::Expr   &psi3,
            csl::Expr   &psi4
            )
    {
        csl::Expr coupling = CSL_1;
        std::array<csl::Expr*, 4> psi {&psi1, &psi2, &psi3, &psi4};
        for (size_t i = 0; i != model.getGauge()->size(); ++i) {
            auto group = (*model.getGauge())[i];
            if (group->getType() == group::Type::U1)
                continue;
            auto rep1 = qf(psi1)->getQuantumParent()->getGroupIrrep(group);
            auto rep2 = qf(psi2)->getQuantumParent()->getGroupIrrep(group);
            auto rep3 = qf(psi3)->getQuantumParent()->getGroupIrrep(group);
            auto rep4 = qf(psi4)->getQuantumParent()->getGroupIrrep(group);
            auto [k, l] = findTwoIdenticalReps(rep1, rep2, rep3, rep4);
            constexpr size_t npos = -1;
            if (k != npos && l != npos) {
                csl::Index a = groupIndex(model, group, *psi[k]);
                csl::Index b = groupIndex(model, group, *psi[l]);
                coupling *= a.getSpace()->getDelta()({a, b});
            }
        }
        return coupling;
    }

    static ColorCoupling getColorCoupling(
            std::vector<ColorSpec> const &colorCouplings,
            std::string            const &group
            )
    {
        for (const auto &coupling : colorCouplings) {
            if (coupling.groupName == group) {
                return coupling.coupling;
            }
        }
        return ColorCoupling::Id;
    }

    static csl::Expr getColorCoupling(
            ColorCoupling      coupling,
            csl::Index  const &a1,
            csl::Index  const &a2,
            csl::Index  const &a3,
            csl::Index  const &a4,
            csl::Tensor       &T,
            csl::Index  const &A
            )
    {
        csl::Tensor delta = a1.getSpace()->getDelta();
        switch (coupling) {
            case ColorCoupling::Id: 
                return delta({a1, a2}) * delta({a3, a4});
            case ColorCoupling::Crossed: 
                return delta({a1, a4}) * delta({a3, a2});
            case ColorCoupling::InvCrossed: 
                return delta({a1, a3}) * delta({a2, a4});
            case ColorCoupling::Generator:
                return T({A, a1, a2}) * T({A, a3, a4});
        }
        return CSL_0;
    }

    static csl::Expr getGeneralColorCouplings(
            Model                  const &model,
            std::vector<ColorSpec> const &colorCouplings,
            csl::Expr   &psi1,
            csl::Expr   &psi2,
            csl::Expr   &psi3,
            csl::Expr   &psi4
            )
    {
        csl::Expr coupling = CSL_1;
        std::array<csl::Expr*, 4> psi {&psi1, &psi2, &psi3, &psi4};
        for (size_t i = 0; i != model.getGauge()->size(); ++i) {
            auto group = (*model.getGauge())[i];
            auto rep1 = qf(psi1)->getQuantumParent()->getGroupIrrep(group);
            auto rep2 = qf(psi2)->getQuantumParent()->getGroupIrrep(group);
            auto rep3 = qf(psi3)->getQuantumParent()->getGroupIrrep(group);
            auto rep4 = qf(psi4)->getQuantumParent()->getGroupIrrep(group);
            bool identical = areIdenticalReps(rep1, rep2, rep3, rep4);
            if (identical) {
                csl::Tensor T = model.getGenerator(group, *psi[0]);
                csl::Index A = T->getSpace()[0]->generateIndex();
                csl::Index a = groupIndex(model, group, *psi[0]);
                csl::Index b = groupIndex(model, group, *psi[1]);
                csl::Index c = groupIndex(model, group, *psi[2]);
                csl::Index d = groupIndex(model, group, *psi[3]);
                ColorCoupling current = getColorCoupling(
                        colorCouplings, group->getName()
                        );
                coupling *= getColorCoupling(current, a, b, c, d, T, A);
            }
        }
        return coupling;
    }

    static csl::Expr getColorCurrent(
            Model                  const &model,
            std::vector<ColorSpec> const &colorCouplings,
            csl::Expr &psi1,
            csl::Expr &psi2,
            csl::Expr &psi3,
            csl::Expr &psi4
            )
    {
        csl::Expr trivialCouplings = getTrivialColorCouplings(
                model, psi1, psi2, psi3, psi4
                );
        csl::Expr generalCouplings = getGeneralColorCouplings(
                model, colorCouplings, psi1, psi2, psi3, psi4
                );
        return trivialCouplings * generalCouplings;
    }

    std::vector<Wilson> dimension6Operator(
            Model     const &model,
            WilsonSet const &wilsons,
            DiracCoupling     leftCurrent,
            DiracCoupling     rightCurrent,
            std::vector<ColorSpec> const &colorCouplings,
            std::vector<int> fermionOrder
            )
    {
        std::vector<OpInsertion> insertions = getOpInsertions(
                wilsons.kinematics);
        std::vector<InsertionRequirement> requirements = {
            {2, 1}, // conjugate fermion
            {2, 0}, // non-conjugate fermion
            {2, 1}, // conjugate fermion
            {2, 0}  // non-conjugate fermion
        };
        if (!requirementsSatisfied(insertions, requirements))
            return {};
        for (const auto &color : colorCouplings)
            model.getGroup(color.groupName); // Checking that the color exists
        setUpDim6FermionOrder(wilsons, fermionOrder);
        auto [psi1, psi2, psi3, psi4] = getExternalDim6Fermions(
                insertions, fermionOrder);
        std::vector<Wilson> diracCouplings = getDiracCurrent(
                leftCurrent, rightCurrent, psi1, psi2, psi3, psi4
                );
        csl::Expr colorCurrent = getColorCurrent(
                model, colorCouplings, psi1, psi2, psi3, psi4
                );
        csl::Expr remnant = csl::prod_s({psi1, psi2, psi3, psi4, colorCurrent});
        for (auto &coef : diracCouplings)
            coef.op.setExpression(
                    coef.op.getExpression() * csl::DeepCopy(remnant));

        return diracCouplings;
    }

}
