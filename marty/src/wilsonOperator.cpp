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

#include "wilsonOperator.h"
#include "model.h"
#include "error.h"

namespace mty {

std::ostream &operator<<(
        std::ostream &out,
        OperatorType  type
        )
{
    switch(type)
    {
        case mty::OperatorType::Magnetic:       out << "Magnetic";       break;
        case mty::OperatorType::ChromoMagnetic: out << "ChromoMagnetic"; break;
        default: out << "Unknown Operator";
    }
    return out;
}

namespace OperatorParser {

    static std::array<csl::Expr, 6> buildMagneticOperators(
            std::vector<Insertion> const &insertions
            )
    {

        csl::Expr op_p1 = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[0].momentum,
                false
                );
        csl::Expr op_p2 = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[1].momentum,
                false
                );
        csl::Expr op_p3 = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[2].momentum,
                false
                );
        csl::Expr op_p1c = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[0].momentum,
                true
                );
        csl::Expr op_p2c = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[1].momentum,
                true
                );
        csl::Expr op_p3c = buildMagneticOperator(
                insertions[0], insertions[1], insertions[2],
                insertions[2].momentum,
                true
                );
        return {op_p1, op_p2, op_p3, op_p1c, op_p2c, op_p3c};
    }

    csl::Expr getMagneticContribution(
            Wilson                   const &wilson,
            std::array<csl::Expr, 6> const &magneticOperators,
            Chirality                       chirality,
            int                             s1,
            int                             s2
            )
    {
        auto const &[op_p1, op_p2, op_p3, op_p1c, op_p2c, op_p3c] 
            = magneticOperators;
        csl::Expr cFactor = (chirality == Chirality::Left) ? -1 : 1; 
        if (wilson.op == WilsonOperator{op_p1}) 
            return CSL_1 / 4 * s1 * wilson.coef.getCoefficient();
        else if (wilson.op == WilsonOperator{op_p2})
            return CSL_1 / 4 * s2 * wilson.coef.getCoefficient();
        else if (wilson.op == WilsonOperator{op_p3})
            return -CSL_1 / 2 * wilson.coef.getCoefficient();
        if (wilson.op == WilsonOperator{op_p1c}) 
            return cFactor / 4 * s1 * wilson.coef.getCoefficient();
        else if (wilson.op == WilsonOperator{op_p2c})
            return cFactor / 4 * s2 * wilson.coef.getCoefficient();
        else if (wilson.op == WilsonOperator{op_p3c})
            return -cFactor / 2 * wilson.coef.getCoefficient();
        return CSL_0;
    }

    csl::Expr getMagneticCoefficient(
            std::vector<Wilson> const &coefs,
            Chirality                  chirality
            )
    {
        std::vector<Requirement> requirements {
            {1, 0}, // Non-conjugated fermion
            {1, 1}, // Conjugated fermion
            {2, -1} // Vector boson, conjugated or not
        };
        std::vector<csl::Expr> res;
        for (const auto &wilson : coefs) {
            const std::vector<Insertion> insertions = getInsertions(
                    requirements,
                    wilson.op
                    );
            if (insertions.empty()) {
                continue;
            }
            int sign_p1 = insertions[2].field.isIncoming() ? -1 : 1;
            if (!insertions[0].field.isIncoming())
                sign_p1 *= -1;
            int sign_p2 = insertions[2].field.isIncoming() ? -1 : 1;
            if (insertions[1].field.isIncoming())
                sign_p2 *= -1;
            std::array magOperators = buildMagneticOperators(insertions);
            csl::Expr contrib = getMagneticContribution(
                    wilson,
                    magOperators,
                    chirality,
                    sign_p1, 
                    sign_p2);
            if (contrib != CSL_0) {
                res.push_back(contrib);
            }
        }

        return csl::sum_s(res);
    }

    csl::Expr get_V_x_V5_Coefficient(
            std::vector<Wilson> const &coefs
            )
    {
        std::vector<Requirement> requirements {
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
        };
        std::vector<csl::Expr> res;
        for (const auto &wilson : coefs) {
            const std::vector<Insertion> insertions = getInsertions(
                    requirements,
                    wilson.op
                    );
            if (insertions.empty()) {
                continue;
            }
            csl::Expr op_1 = build_V_x_V5(
                    insertions[0],
                    insertions[1],
                    insertions[2],
                    insertions[3]
                    );
            csl::Expr op_2 = build_V_x_V5(
                    insertions[0],
                    insertions[3],
                    insertions[2],
                    insertions[1]
                    );
            if (wilson.op == WilsonOperator{op_1})
                res.push_back(wilson.coef.getCoefficient());
            else if (wilson.op == WilsonOperator{op_2})
                res.push_back(wilson.coef.getCoefficient());
        }

        return csl::sum_s(res);
    }

    csl::Expr get_V5_x_V_Coefficient(
            std::vector<Wilson> const &coefs
            )
    {
        std::vector<Requirement> requirements {
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
        };
        std::vector<csl::Expr> res;
        for (const auto &wilson : coefs) {
            const std::vector<Insertion> insertions = getInsertions(
                    requirements,
                    wilson.op
                    );
            if (insertions.empty()) {
                continue;
            }
            csl::Expr op_1 = build_V5_x_V(
                    insertions[0],
                    insertions[1],
                    insertions[2],
                    insertions[3]
                    );
            csl::Expr op_2 = build_V5_x_V(
                    insertions[0],
                    insertions[3],
                    insertions[2],
                    insertions[1]
                    );
            if (wilson.op == WilsonOperator{op_1})
                res.push_back(wilson.coef.getCoefficient());
            else if (wilson.op == WilsonOperator{op_2})
                res.push_back(wilson.coef.getCoefficient());
        }

        return csl::sum_s(res);
    }

    csl::Expr get_V5_x_V5_Coefficient(
            std::vector<Wilson> const &coefs
            )
    {
        std::vector<Requirement> requirements {
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
        };
        std::vector<csl::Expr> res;
        for (const auto &wilson : coefs) {
            const std::vector<Insertion> insertions = getInsertions(
                    requirements,
                    wilson.op
                    );
            if (insertions.empty()) {
                continue;
            }
            csl::Expr op_1 = build_V5_x_V5(
                    insertions[0],
                    insertions[1],
                    insertions[2],
                    insertions[3]
                    );
            csl::Expr op_2 = build_V5_x_V5(
                    insertions[0],
                    insertions[3],
                    insertions[2],
                    insertions[1]
                    );
            if (wilson.op == WilsonOperator{op_1})
                res.push_back(wilson.coef.getCoefficient());
            else if (wilson.op == WilsonOperator{op_2})
                res.push_back(wilson.coef.getCoefficient());
        }

        return csl::sum_s(res);
    }

    csl::Expr get_V_x_V_Coefficient(
            std::vector<Wilson> const &coefs
            )
    {
        std::vector<Requirement> requirements {
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
            {1, 1}, // Conjugated fermion
            {1, 0}, // Non-conjugated fermion
        };
        std::vector<csl::Expr> res;
        for (const auto &wilson : coefs) {
            const std::vector<Insertion> insertions = getInsertions(
                    requirements,
                    wilson.op
                    );
            if (insertions.empty()) {
                continue;
            }
            csl::Expr op_1 = build_V_x_V(
                    insertions[0],
                    insertions[1],
                    insertions[2],
                    insertions[3]
                    );
            csl::Expr op_2 = build_V_x_V(
                    insertions[0],
                    insertions[3],
                    insertions[2],
                    insertions[1]
                    );
            if (wilson.op == WilsonOperator{op_1})
                res.push_back(wilson.coef.getCoefficient());
            else if (wilson.op == WilsonOperator{op_2})
                res.push_back(wilson.coef.getCoefficient());
        }

        return csl::sum_s(res);
    }

    static mty::QuantumField *qf(csl::Expr &expr) 
    {
        return dynamic_cast<mty::QuantumField*>(expr.get());
    }

    static csl::Expr fermionExpr(Insertion const &ins)
    {
        csl::Expr psi = ins.field.getQuantumParent()->getInstance();
        qf(psi)->setIncoming(ins.field.isIncoming());
        qf(psi)->setConjugated(ins.field.isComplexConjugate());
        qf(psi)->setPoint(ins.momentum);
        return psi;
    }

    csl::Expr build_V_x_V(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            )
    {
        csl::Expr p1 = fermionExpr(psi1L);
        csl::Expr p2 = fermionExpr(psi1R);
        csl::Expr p3 = fermionExpr(psi2L);
        csl::Expr p4 = fermionExpr(psi2R);

        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index  alpha = p1->getIndexStructureView().back();
        csl::Index  beta  = p2->getIndexStructureView().back();
        csl::Index  gam   = p3->getIndexStructureView().back();
        csl::Index  delta = p4->getIndexStructureView().back();
        csl::Tensor gamma = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma;

        for (size_t i = 0; i+1 < p1->getIndexStructureView().size(); ++i) 
            p2->getIndexStructureView()[i] = p1->getIndexStructureView()[i];
        for (size_t i = 0; i+1 < p3->getIndexStructureView().size(); ++i) 
            p4->getIndexStructureView()[i] = p3->getIndexStructureView()[i];

        return (p1 * gamma({+mu, alpha, beta}) * p2)
             * (p3 * gamma({mu, gam, delta}) * p4);
    }

    csl::Expr build_V_x_V5(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            )
    {
        csl::Expr p1 = fermionExpr(psi1L);
        csl::Expr p2 = fermionExpr(psi1R);
        csl::Expr p3 = fermionExpr(psi2L);
        csl::Expr p4 = fermionExpr(psi2R);

        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index  alpha = p1->getIndexStructureView().back();
        csl::Index  beta  = p2->getIndexStructureView().back();
        csl::Index  gam   = p3->getIndexStructureView().back();
        csl::Index  eps   = p4->getIndexStructureView().back();
        csl::Index  delta = eps.rename();
        csl::Tensor gamma = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma;
        csl::Tensor gamma5 = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma_chir;

        for (size_t i = 0; i+1 < p1->getIndexStructureView().size(); ++i) 
            p2->getIndexStructureView()[i] = p1->getIndexStructureView()[i];
        for (size_t i = 0; i+1 < p3->getIndexStructureView().size(); ++i) 
            p4->getIndexStructureView()[i] = p3->getIndexStructureView()[i];

        return (p1 * gamma({+mu, alpha, beta}) * p2)
             * (p3 * gamma({mu, gam, delta}) * gamma5({delta, eps}) * p4);
    }


    csl::Expr build_V5_x_V(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            )
    {
        csl::Expr p1 = fermionExpr(psi1L);
        csl::Expr p2 = fermionExpr(psi1R);
        csl::Expr p3 = fermionExpr(psi2L);
        csl::Expr p4 = fermionExpr(psi2R);

        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index  alpha = p1->getIndexStructureView().back();
        csl::Index  eps  = p2->getIndexStructureView().back();
        csl::Index  gam   = p3->getIndexStructureView().back();
        csl::Index  delta   = p4->getIndexStructureView().back();
        csl::Index  beta = eps.rename();
        csl::Tensor gamma = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma;
        csl::Tensor gamma5 = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma_chir;

        for (size_t i = 0; i+1 < p1->getIndexStructureView().size(); ++i) 
            p2->getIndexStructureView()[i] = p1->getIndexStructureView()[i];
        for (size_t i = 0; i+1 < p3->getIndexStructureView().size(); ++i) 
            p4->getIndexStructureView()[i] = p3->getIndexStructureView()[i];

        return (p1 * gamma({+mu, alpha, beta})*gamma5({beta, eps}) * p2)
             * (p3 * gamma({mu, gam, delta})  * p4);
    }

    csl::Expr build_V5_x_V5(
            Insertion const &psi1L,
            Insertion const &psi1R,
            Insertion const &psi2L,
            Insertion const &psi2R
            )
    {
        csl::Expr p1 = fermionExpr(psi1L);
        csl::Expr p2 = fermionExpr(psi1R);
        csl::Expr p3 = fermionExpr(psi2L);
        csl::Expr p4 = fermionExpr(psi2R);

        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index alpha = p1->getIndexStructureView().back();
        csl::Index eps  = p2->getIndexStructureView().back();
        csl::Index gam   = p3->getIndexStructureView().back();
        csl::Index eta   = p4->getIndexStructureView().back();
        csl::Index beta = eps.rename();
        csl::Index delta = eps.rename();
        csl::Tensor gamma = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma;
        csl::Tensor gamma5 = dynamic_cast<DiracSpace const*>(
                alpha.getSpace())->gamma_chir;

        for (size_t i = 0; i+1 < p1->getIndexStructureView().size(); ++i) 
            p2->getIndexStructureView()[i] = p1->getIndexStructureView()[i];
        for (size_t i = 0; i+1 < p3->getIndexStructureView().size(); ++i) 
            p4->getIndexStructureView()[i] = p3->getIndexStructureView()[i];

        return (p1 * gamma({+mu, alpha, beta})*gamma5({beta, eps}) * p2)
             * (p3 * gamma({mu, gam, delta})*gamma5({delta, eta})  * p4);
    }

    csl::Expr buildMagneticOperator(
            Insertion const &incomingFermion,
            Insertion const &outgoingFermion,
            Insertion const &vectorBoson,
            csl::Tensor      momentum,
            bool             chiralOperator
            )
    {
        csl::Expr psi_star = fermionExpr(outgoingFermion);
        csl::Expr psi = fermionExpr(incomingFermion);
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
        csl::Expr generator = CSL_1;
        if (A->getIndexStructureView().size() > 1) {
            // Chromo-magnetic operator
            auto irrep = vectorBoson.field.getQuantumParent()->getGaugeIrrep();
            Model const &model = *mty::Model::current;
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
        mu = mu.getFlipped();

        csl::Expr projector;
        if (chiralOperator)
            projector = diracSpace->gamma_chir({alpha, beta});
        else
            projector = diracSpace->getDelta()({alpha, beta});

        return psi_star * projector * generator * psi * (momentum(mu) * A);
    }

    std::optional<Insertion> getInsertion(
            std::vector<Requirement> &requirements,
            csl::Expr               const &qField
            )
    {
        HEPAssert(IsOfType<QuantumField>(qField),
                mty::error::TypeError,





                "This function expects a quantum field, " + toString(qField)
                + " given.")
        QuantumField const *f = ConvertToPtr<QuantumField>(qField);
        Insertion ins{
            *f, 
            std::dynamic_pointer_cast<csl::TensorParent>(f->getPoint())
        };
        for (auto iter  = requirements.begin();
                  iter != requirements.end();
                  ++iter) {
            if (ins.verifies(*iter)) {
                requirements.erase(iter);
                return ins;
            }
        }
        return std::nullopt;
    }

    std::vector<Insertion> getInsertions(
            std::vector<Requirement> const &requirements,
            WilsonOperator           const &op
            )
    {
        std::vector<Insertion> res;
        auto copy_requirements = requirements;
        csl::VisitEachLeaf(op.getOp(), [&](csl::Expr const &leaf)
        {
            if (IsOfType<QuantumField>(leaf)) {
                if (copy_requirements.empty()) 
                    res.clear();
                else if (const auto opt = getInsertion(copy_requirements, leaf);
                        opt)
                    res.push_back(opt.value());
                else
                    res.clear();
            }
        });

        if (res.size() == requirements.size()) {
            sortInsertions(res, requirements);
            return res;
        }
        return {};
    }

    void sortInsertions(
            std::vector<Insertion>         &insertions,
            std::vector<Requirement> const &requirements
            )
    {
        HEPAssert(insertions.size() == requirements.size(),
                mty::error::ValueError,
                "Insertions and requirements should have the same size, "
                + toString(insertions.size()) + ", " 
                + toString(requirements.size()) + " respectively were given.")
            for (size_t i = 0; i != insertions.size(); ++i) {
                size_t i_right = i;
                for (size_t j = i+1; j < insertions.size(); ++j) {
                    if (insertions[j].verifies(requirements[i])) {
                        i_right = j;
                        break;
                    }
                }
                if (i != i_right) {
                    std::swap(insertions[i], insertions[i_right]);
                }
            }
    }

} // End of namespace mty::OperatorParser
} // End of namespace mty
