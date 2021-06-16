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

#include "wilson.h"
#include "polarization.h"
#include "diracology.h"
#include "fermionChain.h"
#include "interactionTerm.h"
#include "quantumField.h"
#include "model.h"
#include "fermionCurrent.h"
#include "mrtOptions.h"
#include "sgl.h"
#include "propagator.h"

using namespace csl;
namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class WilsonCoefficient                       //
/*************************************************/
///////////////////////////////////////////////////

WilsonCoefficient::WilsonCoefficient(csl::Expr const& t_coefficient)
    :coefficient(t_coefficient)
{

}

csl::Expr WilsonCoefficient::getCoefficient() const
{
    return coefficient;
}

void WilsonCoefficient::setCoefficient(csl::Expr const& t_coefficient)
{
    HEPAssert(csl::Abbrev::getFreeStructure(t_coefficient->getIndexStructure())
                .size() == 0,
                mty::error::RuntimeError,
                "Non empty index structure encountered for coef " 
                + toString(t_coefficient))
    // coefficient = DeepRefreshed(t_coefficient);
    coefficient = csl::Copy(t_coefficient);
}

///////////////////////////////////////////////////
/*************************************************/
// Class WilsonOperator                          //
/*************************************************/
///////////////////////////////////////////////////

WilsonOperator::WilsonOperator(csl::Expr const& t_op,
                               csl::Expr const& t_factor)
    :factor(t_factor)
{
    setExpression(t_op);
}

csl::Expr WilsonOperator::getOp() const
{
    return op;
}

void WilsonOperator::setOp(csl::Expr const& t_op)
{
    op = t_op;
}

csl::Expr WilsonOperator::getFactor() const
{
    return factor;
}

void WilsonOperator::setFactor(csl::Expr const& t_factor)
{
    factor = t_factor;
}

csl::Expr WilsonOperator::getExpression() const
{
    return factor * op;
}

void WilsonOperator::setExpression(csl::Expr const& t_expression)
{
    csl::ScopedProperty p(&csl::option::checkCommutations, false);
    op = DeepCopy(t_expression);
    csl::ForEachLeaf(op, [&](csl::Expr& expr)
    {
        if (IsOfType<PolarizationField>(expr)) {
            std::shared_ptr<PolarizationField> field 
                = ConvertToShared<PolarizationField>(expr);
            Particle diracParent = field->getQuantumParent()->getDiracParent();
            if (diracParent) {
                field->setParent(diracParent);
            }
            if (!mty::option::decomposeInLocalOperator) 
                return;
            csl::IndexStructure& structure = field->getIndexStructureView();
            std::vector<csl::Index> indices = structure.getIndex();
            csl::Index polar = indices.front();
            indices.erase(indices.begin());
            if (!mty::option::decomposeInLocalOperator) {
                expr = (*field->getQuantumParent())(polar, indices, field->getPoint());
                ConvertToShared<PolarizationField>(expr)->setIncoming(
                        field->isIncoming()
                        );
                ConvertToShared<PolarizationField>(expr)->setConjugated(
                        field->isComplexConjugate()
                        );
            }
            else {
                expr = field->getParent()(indices, field->getPoint());
                ConvertToShared<QuantumField>(expr)->setIncoming(
                        field->isIncoming()
                        );
                ConvertToShared<QuantumField>(expr)->setConjugated(
                        field->isComplexConjugate()
                        );
            }
        }
        else if (IsOfType<QuantumField>(expr)) {
            QuantumField *field = ConvertToPtr<QuantumField>(expr);
            Particle diracParent = field->getQuantumParent()->getDiracParent();
            if (diracParent)
                field->setParent(diracParent);
        }
    });
    op = Refreshed(op);
}

bool WilsonOperator::hardComparison(csl::Expr const& A,
                                    csl::Expr const& B)
{
    if (A == CSL_1 or B == CSL_1)
        return A == CSL_1 and B == CSL_1;
    return mty::hardComparison(A, B);
}

bool WilsonOperator::operator<(WilsonOperator const& other) const
{
    return hardOrdering(DeepRefreshed(op), 
                        DeepRefreshed(other.op));
}

bool WilsonOperator::operator==(WilsonOperator const& other) const
{
    return hardComparison(DeepRefreshed(op), 
                          DeepRefreshed(other.op));
}


///////////////////////////////////////////////////
/*************************************************/
// WilsonSet Class                               //
/*************************************************/
///////////////////////////////////////////////////

void WilsonSet::merge()
{
    WilsonSet other;
    for (const auto &wil : *this)
        addWilson(wil, other);
    *this = std::move(other);
    auto last = std::remove_if(begin(), end(), [&](Wilson const &wil) {
        return wil.coef.getCoefficient() == CSL_0;
    });
    erase(last, end());
}

void WilsonSet::sort()
{
    std::sort(begin(), end(), [&](Wilson const &A, Wilson const &B) {
        return A.op < B.op;
    });
}

///////////////////////////////////////////////////
/*************************************************/
// Other functions                               //
/*************************************************/
///////////////////////////////////////////////////

void parseStructures(
        csl::Expr              &arg,
        std::vector<csl::Expr> &inOperator,
        csl::IndexStructure    &quantumStructure
        )
{
    if (IsOfType<PolarizationField>(arg) or IsOfType<QuantumField>(arg)) {
        quantumStructure += arg->getIndexStructureView();
        inOperator.push_back(arg);
        arg = CSL_1;
    }
    else if (arg->getType() == csl::Type::TDerivativeElement
            and (IsOfType<PolarizationField>(arg->getOperand())
                or IsOfType<QuantumField>(arg->getOperand()))) {
        inOperator.push_back(arg->getOperand());
        quantumStructure += arg->getOperand()->getIndexStructureView();
        arg = CSL_1;
    }
    else if (csl::IsIndicialTensor(arg) 
            && arg->getFreeIndexStructure().size() > 0) {
        quantumStructure += arg->getIndexStructureView();
        inOperator.push_back(arg);
        arg = CSL_1;
    }
}

std::vector<csl::Expr> parseStructures(
        csl::Expr &prod
        )
{
    csl::IndexStructure quantumStructure;
    csl::vector_expr inOperator;
    if (!csl::IsProd(prod)) {
        parseStructures(prod, inOperator, quantumStructure);
        return inOperator;
    }
    for (auto& arg : prod) {
        parseStructures(arg, inOperator, quantumStructure);
    }
    for (size_t k = 0; k != prod->size(); ++k) {
        csl::Expr &arg = prod[k];
        if (arg == CSL_1)
            continue;
        csl::IndexStructure structure = arg->getIndexStructure();
        bool connected = false;
        for (size_t i = 0; i != structure.size(); ++i) {
            for (size_t j = 0; j != quantumStructure.size(); ++j) {
                if (structure[i] == quantumStructure[j]) {
                    structure.erase(structure.begin() + i);
                    quantumStructure.erase(quantumStructure.begin() + j);
                    quantumStructure.insert(
                            quantumStructure.end(),
                            std::make_move_iterator(structure.begin()),
                            std::make_move_iterator(structure.end())
                            );
                    connected = true;
                    k = -1;
                    break;
                }
            }
            if (connected)
                break;
        }
        if (connected) {
            inOperator.push_back(arg);
            arg = CSL_1;
        }
    }

    return inOperator;
}

std::vector<Wilson> parseSum(
        csl::Expr const& sum,
        csl::Expr const& operatorFactor,
        bool        standardBasis,
        bool        recursive
        )
{
    std::vector<Wilson> wilsons;
    wilsons.reserve(csl::Size(sum));
    for (const auto &term : sum) {
        std::vector<Wilson> interm = 
            parseExpression(
                    term,
                    operatorFactor,
                    standardBasis,
                    recursive);
        wilsons.insert(
                wilsons.end(),
                std::make_move_iterator(interm.begin()),
                std::make_move_iterator(interm.end())
                );
    }
    return wilsons;
}

std::vector<Wilson> parseExpression(
        csl::Expr const& expr,
        csl::Expr const& operatorFactor,
        bool        standardBasis,
        bool        recursive
        )
{
    if (csl::IsSum(expr)) {
        return parseSum(expr, operatorFactor, standardBasis, recursive);
    }
    if (expr == CSL_0)
        return {};
    csl::Expr product = DeepCopy(expr);
    mty::FermionChain f(&mty::dirac4);
    f.applyOn(product, false);
    if (product == CSL_0)
        return {};
    HEPAssert(operatorFactor->getFreeIndexStructure().size() == 0,
            mty::error::TypeError,
            "Operator factor in Wilson operator single parsing should have"
            + std::string(" no free index."));

    Wilson res = {WilsonCoefficient(CSL_1), WilsonOperator(CSL_0)};
    std::vector<csl::Expr> inOperator = parseStructures(product);
    if (!inOperator.empty()) {
        res.coef = csl::DeepRefreshed(product);
        csl::Expr op = csl::DeepExpanded(csl::prod_s(inOperator));
        if (csl::IsSum(op)) {
            return parseSum(
                    csl::Expanded(res.coef.getCoefficient() * op),
                    operatorFactor,
                    standardBasis,
                    recursive);
        }
        else if (csl::IsProd(op)){
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
            //std::cout << "HERE" << std::endl;
            //std::cout << op << std::endl;
            if (!recursive){
                op = sgl::sgl_to_csl(sgl::Simplified(
                            sgl::csl_to_sgl(op, tset), !recursive
                            ), tset);
                auto d = dirac4.getDelta();
                auto g = dirac4.gamma_chir;
                auto a = dirac4.generateIndex();
                auto b = dirac4.generateIndex();
                csl::DeepRefresh(op);
                if (standardBasis) {
                    Model::projectOnBasis(op, OperatorBasis::Standard);
                }
            }
            csl::Expand(op);
            if (csl::IsSum(op)) {
                return parseSum(
                        csl::Expanded(res.coef.getCoefficient() * op),
                        operatorFactor,
                        standardBasis,
                        true);
            }
            res.coef.setCoefficient(
                    res.coef.getCoefficient() / operatorFactor);
            res.op = WilsonOperator(op, operatorFactor);
            return {res};
        }
        else {
            res.op = WilsonOperator(op, operatorFactor);
            return {res};
        }
    }
    return {Wilson{WilsonCoefficient(product), WilsonOperator(CSL_1)}};
}

void addWilson(
        Wilson        const &wil,
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
std::vector<Wilson> match(
        std::vector<csl::Expr> &fullAmplitudes,
        csl::Expr        const &operatorFactor,
        bool                    standardBasis,
        bool                    squaredAfter
        )
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
    }
    csl::Abbrev::disableGenericEvaluation("Fc");
    csl::Abbrev::disableGenericEvaluation("EXT");

    std::vector<Wilson> fullWilson;
    for (size_t i = 0; i != fullAmplitudes.size(); ++i) {
        std::vector<Wilson> newWilsons = parseExpression(
                DeepCopy(fullAmplitudes[i]),
                operatorFactor,
                standardBasis,
                squaredAfter
                );
        for (auto &w : newWilsons) {
            w.coef.setCoefficient(
                    csl::Factored(w.coef.getCoefficient())
                    );
            addWilson(w, fullWilson);
        }
    }

    return fullWilson;
}


}
