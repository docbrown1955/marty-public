#include "wilson.h"
#include "insertion.h"
#include "mrtError.h"
#include "diracology.h"
#include "amplitude.h"
#include "mrtInterface.h"
#include "propagator.h"

namespace mty {

    static void insertionError()
    {
        CallHEPError(mty::error::TypeError,
                "Insertions are not compatible with the penguin patch, "
                "2 fermions and one massless vector boson are required.")
    }

    static bool checkInsertionsForPatch(
            std::vector<mty::Insertion> const &insertions,
            bool                               raiseError
            )
    {
        if (insertions.size() != 3) {
            if (raiseError)
                insertionError();
            return false;
        }
        std::size_t nF = std::count_if(insertions.begin(), insertions.end(), 
                [&](mty::Insertion const &part) {
                    return part.getField()->isFermionic();
                });
        std::size_t nmB = std::count_if(insertions.begin(), insertions.end(), 
                [&](mty::Insertion const &part) {
                    return part.getField()->isBosonic()
                        && part.getField()->getMass() == CSL_0; 
                });
        if (nF != 2 || nmB != 1) {
            if (raiseError)
                insertionError();
            return false;
        }
        return true;
    }

    static std::pair<csl::Expr, csl::Expr> getFermions(
            csl::Expr const &op
            )
    {
        std::pair<csl::Expr, csl::Expr> fermions;
        HEPAssert(op->getType() == csl::Type::Prod,
                mty::error::TypeError,
                "Expected product as operator, got " + toString(op) + ".")
        for (const auto &arg : op) {
            if (auto field = dynamic_cast<QuantumField const*>(arg.get());
                    field) {
                if (field->isFermionic()) {
                    if (field->isComplexConjugate())
                        fermions.first = arg;
                    else
                        fermions.second = arg;
                }
            }
        }
        HEPAssert(fermions.first && fermions.second,
                mty::error::TypeError,
                "Expected one conjugate and one not conjugate fermions in "
                "operator.")
        return fermions;
    }

    static std::pair<std::size_t, std::size_t> checkOperators(
            std::vector<Wilson>      const &wilsons,
            std::vector<csl::Tensor> const &momenta
            )
    {
        constexpr std::size_t npos = -1;
        std::pair<std::size_t, std::size_t> posFermions { npos, npos };
        for (const auto& wil : wilsons) {
            auto [psic, psi] = getFermions(wil.op.getOp());
            std::size_t ic = -1, i = -1;
            for (std::size_t k = 0; k != momenta.size(); ++k) {
                if (momenta[k] == psi->getPoint()) {
                    i = k;
                }
                if (momenta[k] == psic->getPoint()) {
                    ic = k;
                }
            }
            HEPAssert((ic == posFermions.first || posFermions.first == npos)
                    && (i == posFermions.second || posFermions.second == npos),
                    mty::error::RuntimeError,
                    "Inconsistent fermion alignement, consider fermion ordering.")
            posFermions = {ic, i};
        }
        HEPAssert(posFermions.first != npos && posFermions.second != npos,
                mty::error::RuntimeError,
                "Inconsistent fermion alignement, consider fermion ordering.")
        return posFermions;
    }

    struct PenguinPatchData {
        csl::Expr m1, m2;   // fermion masses
        csl::Tensor p1, p2; // fermion momenta
        csl::Expr s_12;     // s_12 = (p1.p2)
        int s1, s2, s3;     // +1 for incoming, -1 for outgoing
        int eta1, eta2;     // +1 for particle, -1 for anti-particle
    };

    static PenguinPatchData getPenguinData(
            std::vector<mty::Insertion> const &insertions,
            std::size_t iConj,
            std::size_t iReg
            )
    {
        auto boolsign = [](bool b) { return b ? 1 : -1; };
        PenguinPatchData data;
        auto const &psi  = insertions[iReg];
        auto const &psic = insertions[iConj];
        auto const &boson = insertions[3 - iReg - iConj];
        data.m1 = psi .getField()->getMass();
        data.m2 = psic.getField()->getMass();
        data.s1 = boolsign(psi .isIncoming());
        data.s2 = boolsign(psic.isIncoming());
        data.s3 = boolsign(boson.isIncoming());
        data.eta1 = boolsign(psi .isParticle());
        data.eta2 = boolsign(psic.isParticle());
        return data;
    }

    static void orderOperators(
            std::vector<Wilson> &wilsons,
            csl::Tensor   const &p1,
            csl::Tensor   const &p2
            )
    {
        // Order F1, F1c, F2, F2c, F3, F3c
        auto contains = [&](csl::Expr const &full, csl::Tensor const &p) {
            return csl::AnyOfLeafs(full, [&](csl::Expr const &sub) {
               return csl::IsIndicialTensor(sub) && sub->getParent() == p;
            });
        };
        std::vector<Wilson> res(6, {CSL_0, CSL_0});
        for (const auto &wil : wilsons) {
            auto const &op = wil.op.getOp();
            bool hasP1 = contains(op, p1);
            bool hasP2 = contains(op, p2);
            bool hasGamma5 = op->dependsExplicitlyOn(mty::dirac4.gamma_chir.get());
            if (hasP1) {
                res[hasGamma5] = wil;
            }
            else if (hasP2) {
                res[2+hasGamma5] = wil;
            }
            else {
                res[4+hasGamma5] = wil;
            }
        }
        wilsons = std::move(res);
    }

    static void ensureOperator(
            Wilson const &model,
            Wilson       &target,
            csl::Tensor const &p
            )
    {
        csl::Expr op = csl::DeepCopy(model.op.getOp());
        csl::Index mu;
        csl::Expr *momentum = nullptr;
        for (auto &arg : op) {
            if (csl::IsIndicialTensor(arg) && arg->getParent() == p) {
                mu = arg->getIndexStructureView().back();
                momentum = &arg;
                break;
            }
        }
        HEPAssert(momentum,
                mty::error::RuntimeError,
                "Momentum not found in " + toString(op) + ".")
        for (auto &arg : op) {
            if (auto field = dynamic_cast<QuantumField*>(arg.get());
                    field && field->isFermionic() && field->isComplexConjugate()) {
                auto &alpha = field->getIndexStructureView().back();
                csl::Index beta = alpha.rename();
                *momentum = dirac4.gamma({mu, beta, alpha});
                alpha = beta;
                break;
            }
        }
        target.op.setOp(csl::Refreshed(op));
    }

    static void ensureOperators(
            std::vector<Wilson> &wilsons,
            csl::Tensor const &p1,
            csl::Tensor const &p2
            )
    {
        if (wilsons[0].op.getOp() != CSL_0)
            ensureOperator(wilsons[0], wilsons[4], p1);
        else if (wilsons[2].op.getOp() != CSL_0)
            ensureOperator(wilsons[2], wilsons[4], p2);
        if (wilsons[1].op.getOp() != CSL_0)
            ensureOperator(wilsons[1], wilsons[5], p1);
        else if (wilsons[3].op.getOp() != CSL_0)
            ensureOperator(wilsons[3], wilsons[5], p2);
    }

    static void applyPenguinPatch_implementation(
            std::vector<Wilson>    &wilsons,
            PenguinPatchData const &data
            )
    {
        csl::Expr eval_m1 = csl::Evaluated(data.m1, csl::eval::all);
        csl::Expr eval_m2 = csl::Evaluated(data.m2, csl::eval::all);
        csl::Expr F1 = wilsons[0].coef.getCoefficient();
        csl::Expr F1c = wilsons[1].coef.getCoefficient();
        csl::Expr F2 = wilsons[2].coef.getCoefficient();
        csl::Expr F2c = wilsons[3].coef.getCoefficient();
        csl::Expr F = (data.s1*F1 + data.s2*F2)/2;
        csl::Expr Fc = (data.s1*F1c + data.s2*F2c)/2;
        csl::Expr Lambda1 = data.s1*csl::pow_s(data.m1, 2)
            + data.s2*data.s_12;
        csl::Expr Lambda2 = data.s2*csl::pow_s(data.m2, 2)
            + data.s1*data.s_12;
        csl::Expr qSquared = data.s1*Lambda1 + data.s2*Lambda2 + Propagator::Peps;
        csl::Expr m1_tilde = data.eta1*data.s1 * data.m1;
        csl::Expr m2_tilde = data.eta2*data.s2 * data.m2;
        Wilson &O3 = wilsons[4];
        Wilson &O3c = wilsons[5];
        if (eval_m1 == CSL_0 && eval_m2 == CSL_0) {
            O3 .coef.setCoefficient(CSL_0);
            O3c.coef.setCoefficient(CSL_0);
        }
        if (eval_m1 == eval_m2) {
            if (data.eta1*data.s1 == data.eta2*data.s2) {
                O3 .coef.setCoefficient(
                        -F*qSquared / (2*m1_tilde)
                        );
                O3c.coef.setCoefficient(CSL_0);
            }
            else {
                O3 .coef.setCoefficient(CSL_0);
                O3c.coef.setCoefficient(
                        Fc*qSquared / (2*m1_tilde)
                        );
            }
        }
        else {
            O3 .coef.setCoefficient(
                    -F*qSquared/(m1_tilde + m2_tilde));
            O3c.coef.setCoefficient(
                    Fc*qSquared/(m1_tilde - m2_tilde));
        }
    }

    bool requiresPenguinPatch(
            Amplitude const &amplitude
            )
    {
        if (amplitude.empty() || amplitude.getDiagrams()[0].getNLoops() == 0)
            return false;
        auto const &insertions = amplitude.getKinematics().getInsertions();
        return checkInsertionsForPatch(insertions, false);
    }

    void applyPenguinPatch(
            std::vector<Wilson> &wilsons,
            Kinematics    const &kinematics
            )
    {
        if (wilsons.empty())
            return;
        auto const &insertions = kinematics.getInsertions();
        checkInsertionsForPatch(insertions, true);
        auto [iconj, ireg] = checkOperators(wilsons, kinematics.getMomenta());
        auto p1 = kinematics.momentum(ireg);
        auto p2 = kinematics.momentum(iconj);
        auto data = getPenguinData(insertions, iconj, ireg);
        csl::Index mu = csl::Minkowski.generateIndex();
        data.p1 = p1;
        data.p2 = p2;
        data.s_12 = p1(mu)*p2(+mu);
        orderOperators(wilsons, p1, p2);
        ensureOperators(wilsons, p1, p2);
        applyPenguinPatch_implementation(wilsons, data);
    }
}
