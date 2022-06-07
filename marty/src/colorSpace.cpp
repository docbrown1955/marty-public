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

#include "colorSpace.h"
#include "gaugedGroup.h"
#include "groupIndices.h"
#include "traceIdentities.h"
#include "bernoulli.h"
#include "mrtUtils.h"

using namespace std;
using namespace csl;

namespace color {

template<class Container>
std::size_t cycleIndex(std::size_t i, Container const &container)
{
    return (i < container.size()) ? i : i - container.size();
}

template<class Container>
std::size_t distance(std::size_t i, std::size_t j, Container const &container)
{
    if (j < i)
        return distance(j, i, container);
    if (i == j)
        return 0;
    return std::min(j - i, i + container.size() - j);
}

template<class Container>
bool direction(std::size_t from, std::size_t to, Container const &container)
{
    if (from < to)
        return (to - from) <= distance(from, to, container);
    return (to + container.size() - from) <= distance(from, to, container);
}

std::ostream &operator<<(
        std::ostream &out,
        TraceIndices const &trace)
{
    out << trace.factor << " * [ ";
    for (std::size_t i = 0; i != trace.size(); ++i) {
        if (trace.symmetry == i)
            out << "| ";
        out << trace[i] << " ";
    }
    out << "]";

    return out;
}

ColorSpace::ColorSpace(const mty::SemiSimpleGroup*  t_group,
                       const string&                t_name,
                       int                          t_dim,
                       bool                         adjoint)
    :csl::Space(t_name, t_dim),
    group(t_group),
    algebra(t_group->getAlgebra())
{
    if (adjoint)
        defaultIndexNames = {"A", "B", "C", "D", "E", "F", "G", "H"};
    else
        defaultIndexNames = {"a", "b", "c", "d", "e", "f", "g", "h"};
    defaultName = defaultIndexNames.begin();

    mty::group::Type type = t_group->getType();
    if (type == mty::group::Type::Sp
            or type == mty::group::Type::E7) {
        metric = csl::make_shared<MetricParent>(this, "f");
        inverseMetric = csl::make_shared<MetricParent>(
                this,
                "f"
                );
        metric->setFullyAntiSymmetric();
        inverseMetric->setFullyAntiSymmetric();
    }

    // keepCycles = true;
}

std::string ColorSpace::getName() const
{
    std::string colorName = csl::Abstract::regularName(csl::Space::getName());
    std::vector<int> labels = group->getHighestWeight(this);
    for (int l : labels)
        colorName += "_" + toString(l);

    return colorName;
}

void ColorSpace::printCode(
        std::ostream &out,
        int           indentSize
        ) const
{
    std::string indent(indentSize, ' ');
    out << indent << getName();
}

void ColorSpace::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    auto keyword = (header) ? "inline " : "";
    out << indent << keyword << "csl::Space const *" << getName();
    out << " = " << csl::Abstract::regularName(group->getName())
        << "->getGroup()->getVectorSpace(";
    out << csl::Abstract::regularName(group->getName())
        << "->getGroup()->highestWeightRep({";
    std::vector<int> labels = group->getHighestWeight(this);
    for (int l : labels)
        out << l << ", ";
    out << "}));\n";
}


csl::Tensor ColorSpace::getF() const
{
    auto gauged = group->getGaugedGroup();
    if (gauged)
        return gauged->getF();
    return nullptr;
}

csl::Tensor ColorSpace::getD(std::size_t p) const
{
    auto gauged = group->getGaugedGroup();
    if (gauged)
        return gauged->getD(p);
    return nullptr;
}

csl::Tensor ColorSpace::getDTilde() const
{
    auto gauged = group->getGaugedGroup();
    if (gauged)
        return gauged->getDTilde();
    return nullptr;
}

bool ColorSpace::hasSpecialTraceProperty(
        const csl::vector_expr& tensors) const
{
    if (tensors.size() < 3 or tensors.size() > 12)
        return false;
    for (const auto& tensor : tensors)
        if (!isInstance(tensor))
            return false;

    return true;
}

csl::Expr ColorSpace::calculateTrace(csl::vector_expr tensors) const
{
    HEPAssert(tensors.size() <= 12,
            mty::error::RuntimeError,
            "Should not encounter more than 12 tensors in a trace.");
    // tensors.size() >= 3
    mty::Irrep irrep = group->highestWeightRep(group->getHighestWeight(this));
    std::vector<csl::Index> indices = exprToIndices(tensors);
    std::vector<TraceIndices> contracted = contractIndices(indices, irrep);
    mergeTraces(contracted);
    applyModifier(contracted, [&](TraceIndices const &init)
    {
        return symmetrize(init);
    });
    mergeTraces(contracted);
    std::vector<csl::Expr> terms;
    terms.reserve(contracted.size());
    for (const auto &trace : contracted) {
        terms.push_back(symmetrizedTrace(trace));
    }
    return csl::Expanded(csl::sum_s(terms), true);
}

void ColorSpace::applyE(
        std::size_t                     j,
        std::vector<TraceIndices> &terms
        ) const
{
    csl::Tensor f = getF();
    if (terms.empty())
        return;
    if (j == 0) {
        for (auto &term : terms)
            ++term.symmetry;
        return;
    }
    std::size_t n = terms[0].symmetry;
    std::vector<TraceIndices> res;
    res.reserve(n * terms.size());
    for (std::size_t i = 0; i < n; ++i) {
        for (const auto &term : terms) {
            TraceIndices newTerm(term);
            csl::Index A = newTerm[i];
            csl::Index B = newTerm[n];
            csl::Index C = A.rename();
            newTerm[n] = C;
            newTerm.erase(newTerm.begin() + i);
            newTerm.factor *= CSL_I * f({A, B, C});
            --newTerm.symmetry;
            res.push_back(std::move(newTerm));
        }
    }
    
    if (j > 0)
        applyE(j - 1, res);
    terms = std::move(res);
}

std::vector<TraceIndices> ColorSpace::symmetrize(
        TraceIndices const &init
        ) const
{
    if (init.isSymmetrized()) {
        return {};
    }
    // std::cout << "Symmetrizing" << std::endl;
    // std::cout << init << std::endl;
    std::size_t n = init.symmetry;
    std::vector<TraceIndices> res;
    res.push_back(init);
    ++res[0].symmetry;
    for (std::size_t j = 1; j <= n; ++j) {
        csl::Expr factor = 
            csl::pow_s(-1, j) / csl::factorial(j) * mty::bernoulliNumber(j);
        if (factor == CSL_0)
            continue;
        std::vector<TraceIndices> EApplied({init});
        EApplied[0].factor *= factor;
        applyE(j, EApplied);
        //std::cout << "AFTER E_" << j << " applied: " << std::endl;
        //for (const auto &t : EApplied)
        //    std::cout << t << std::endl;
        //std::cout << "END" << std::endl;
        res.insert(
                res.end(),
                std::make_move_iterator(EApplied.begin()),
                std::make_move_iterator(EApplied.end()));
    }

    return res;
}

csl::Expr ColorSpace::applySingleTraceIdentity(
        TraceIndices const &trace,
        mty::TraceIdentity const &id
        ) const
{
    std::vector<csl::Expr> terms;
    terms.reserve(id.size());
    for (const auto &tensorProduct : id) {
        std::vector<csl::Expr> factors;
        factors.reserve(2 + tensorProduct.size());
        factors.push_back(trace.factor);
        factors.push_back(id.factor);
        for (const auto &tensor : tensorProduct) {
            std::vector<csl::Index> indices(tensor.size());
            for (std::size_t i = 0; i != indices.size(); ++i)
                indices[i] = trace.indices[tensor.indices[i]];
            factors.push_back(symmetrizedTrace({CSL_1, indices}));
        }
        terms.push_back(csl::prod_s(factors));
    }
    return csl::Factored(csl::sum_s(terms));
}

csl::Expr ColorSpace::applyTraceIdentity(
        TraceIndices const &trace,
        std::vector<mty::TraceIdentity> const &identities
        ) const
{
    std::vector<csl::Expr> terms;
    terms.reserve(identities.size());
    for (const auto &id : identities)
        terms.push_back(applySingleTraceIdentity(trace, id));
    return csl::sum_s(terms);
}

csl::Expr ColorSpace::symmetrizedTrace(
        TraceIndices const &trace) const
{
    mty::Irrep irrep = group->highestWeightRep(group->getHighestWeight(this));
    csl::Expr index = group->getAlgebra()->getIndex(
            irrep,
            trace.indices.size());
    if (index != CSL_0) {
        csl::Tensor d = getD(trace.indices.size()); 
            // = getSymmetricTensor(irrep, indices.size())
        if (algebra->getType() == mty::algebra::Type::D
                and (int)(trace.size()) == algebra->getOrderL() / 2) {
            csl::Tensor d_tilde = getDTilde();
            csl::Expr i_tilde = getLastEvenSOIndex(
                    irrep.getHighestWeight(),
                    trace.indices.size()
                    );
            return csl::prod_s({trace.factor, index,  d(trace.indices)})
                + csl::prod_s({trace.factor, i_tilde, d_tilde(trace.indices)});
        }
        return csl::prod_s({trace.factor, index,  d(trace.indices)});
    }
    else {
        std::vector<mty::TraceIdentity> traceIdentity
            = mty::traceIdentity(
                    algebra->getType(), 
                    algebra->getOrderL(), 
                    trace.indices.size()
                    );
        if (traceIdentity.empty())
            return CSL_0;
        return applyTraceIdentity(trace, traceIdentity);
    }
    return CSL_0;
}

bool areSameTraces(
        TraceIndices const &A,
        TraceIndices const &B
        )
{
    if (A.size() != B.size())
        return false;
    for (std::size_t i = 0; i != A.size(); ++i)
        for (std::size_t j = 0; j != B.size(); ++j) {
            bool same = true;
            for (std::size_t k = 0; k != A.size(); ++k)
                if (A[cycleIndex(i + k, A.indices)]
                        != B[cycleIndex(j + k, B.indices)]) {
                    same = false;
                    break;
                }
            if (same)
                return true;
        }
    return false;
}

void ColorSpace::mergeTraces(std::vector<TraceIndices> &traces) const
{
    for (std::size_t i = 0; i != traces.size(); ++i)
        for (std::size_t j = i+1; j < traces.size(); ++j)
            if (areSameTraces(traces[i], traces[j])) {
                traces[i].factor += traces[j].factor;
                traces.erase(traces.begin() + j);
                --j;
            }
}

bool ColorSpace::contractCloseIndices(
        TraceIndices &indices,
        csl::Expr   const &CR,
        csl::Expr   const &CA
        ) const
{
    bool contracted = false;
    for (std::size_t i = 0; i != indices.size(); ++i) {
        std::size_t j = cycleIndex(i+1, indices);
        if (indices[i] == indices[j]) {
            indices.factor *= CR;
            indices.erase(indices.begin() + std::max(i, j));
            indices.erase(indices.begin() + std::min(i, j));
            i = -1;
            contracted = true;
            continue;
        }
        j = cycleIndex(i+2, indices);
        if (indices[i] == indices[j]) {
            indices.factor *= CR - CA / 2;
            indices.erase(indices.begin() + std::max(i, j));
            indices.erase(indices.begin() + std::min(i, j));
            i = -1;
            contracted = true;
            continue;
        }
    }
    return contracted;
}

std::vector<TraceIndices> ColorSpace::commuteTo(
        TraceIndices const &indices,
        std::size_t              initPos,
        std::size_t              targetPos
        ) const
{
    csl::Tensor f = getF();// = group->getAdjointRep()
    csl::Index B = indices[initPos];
    csl::Index C = group->getVectorSpace(group->getAdjointRep())
        ->generateIndex();
    std::size_t n = distance(initPos, targetPos, indices) - 1;
    if (n == 0)
        return {indices};
    bool dir = direction(initPos, targetPos, indices);
    int increment = (dir) ? 1 : -1;
    csl::Expr factor = (dir) ? -CSL_I : CSL_I;
    std::vector<TraceIndices> newIndices;
    newIndices.reserve(1 + n);
    newIndices.emplace_back(indices);
    std::swap(newIndices[0][initPos],
              newIndices[0][cycleIndex(targetPos-increment, indices)]);
    for (std::size_t i = 0; i != n; ++i) {
        std::size_t pos = cycleIndex(initPos + (i+1)*increment, indices);
        newIndices.emplace_back(indices);
        csl::Index A = indices[pos];
        newIndices.back().factor *= factor * f({A, B, C});
        newIndices.erase(newIndices.begin() + std::max(initPos, pos));
        newIndices.erase(newIndices.begin() + std::min(initPos, pos));
    }

    return newIndices;
}

bool ColorSpace::applyModifier(
        std::vector<TraceIndices> &indices,
        Modifier            const &f
        ) const
{
    bool contracted = false;
    std::vector<TraceIndices> res;
    res.reserve(indices.size());
    do {
        std::vector<TraceIndices> newIndices;
        newIndices.reserve(indices.size());
        for (TraceIndices &index : indices) {
            std::vector<TraceIndices> interm = f(index);
            if (interm.empty())
                res.push_back(std::move(index));
            else {
                contracted = true;
                newIndices.insert(
                        newIndices.end(),
                        std::make_move_iterator(interm.begin()),
                        std::make_move_iterator(interm.end())
                        );
            }
        }
        indices = std::move(newIndices);
    } while (!indices.empty());

    indices = std::move(res);
    return contracted;
}

std::vector<TraceIndices> ColorSpace::contractFarIndices(
        TraceIndices const &indices,
        csl::Expr         const &CR,
        csl::Expr         const &CA) const
{
    for (std::size_t i = 0; i != indices.size(); ++i) 
        for (std::size_t j = i+3; j < indices.size(); ++j) 
            if (i == j)  {
                std::vector<TraceIndices> newIndices
                    = commuteTo(indices, j, i);
                contractCloseIndices(newIndices[0], CR, CA);
                return newIndices;
            }
    return {};
}

std::vector<TraceIndices> ColorSpace::contractStructureConstants(
        TraceIndices const &indices,
        csl::Expr         const &,
        csl::Expr         const &CA) const
{
    csl::Index A, B, C;
    csl::Tensor f = getF();//
    csl::VisitEachLeaf(indices.factor, [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub)
                and sub->getParent_info() == f.get()) {
            A = sub->getIndexStructureView()[0];
            B = sub->getIndexStructureView()[1];
            C = sub->getIndexStructureView()[2];
        }
    });
    for (std::size_t i = 0; i != indices.size(); ++i)  {
        csl::Index first;
        csl::Index second;
        if (indices[i] == A) {
            first  = B;
            second = C;
        }
        else if (indices[i] == B) {
            first  = C;
            second = A;
        }
        else if (indices[i] == C) {
            first  = A;
            second = B;
        }
        for (std::size_t j = i+1; j < indices.size(); ++j) 
            if (indices[j] == first or indices[j] == second)  {
                bool dir = direction(j, i, indices);
                std::vector<TraceIndices> newIndices
                    = commuteTo(indices, j, i);
                csl::Expr factor;
                if (!dir) {
                    if (indices[j] == first)
                        factor = CSL_I * CA / 2;
                    else
                        factor = -CSL_I * CA / 2;
                }
                else {
                    if (indices[j] == first)
                        factor = -CSL_I * CA / 2;
                    else
                        factor = CSL_I * CA / 2;
                }
                newIndices[0] = indices;
                if (indices[j] == first)
                    newIndices[0][i] = second;
                if (indices[j] == second)
                    newIndices[0][i] = first;
                newIndices[0].erase(newIndices[0].begin() + j);
                bool transformed = false;
                // Replacing fABC by +- iCA/2 in the factor
                csl::Transform(newIndices[0].factor, [&](csl::Expr &sub)
                {
                    if (not transformed
                            and csl::IsIndicialTensor(sub)
                            and sub->getParent_info() == f.get()
                            and sub->getIndexStructureView()[0] == A
                            and sub->getIndexStructureView()[0] == B
                            and sub->getIndexStructureView()[0] == C) {
                        transformed = true;
                        sub = factor;
                        return true;
                    }
                    return false;
                });
                return newIndices;
            }
    }
    return {};
}

bool ColorSpace::contractFarIndices(
        std::vector<TraceIndices> &indices,
        csl::Expr                const &CR,
        csl::Expr                const &CA) const
{
    return applyModifier(
            indices,
            [&](TraceIndices const &index) {
                return contractFarIndices(
                    index, CR, CA
                    );
            });

}

bool ColorSpace::contractStructureConstants(
        std::vector<TraceIndices> &indices,
        csl::Expr                const &CR,
        csl::Expr                const &CA) const
{
    return applyModifier(
            indices,
            [&](TraceIndices const &index) {
                return contractStructureConstants(
                    index, CR, CA
                    );
            });
}

std::vector<TraceIndices> ColorSpace::contractIndices(
        std::vector<csl::Index> const &t_indices,
        mty::Irrep              const &irrep
        ) const
{
    csl::Expr CR = group->getAlgebra()->getQuadraticCasimir(
            irrep.getHighestWeight());
    csl::Expr CA = group->getAlgebra()->getQuadraticCasimir(
            group->getAdjointRep().getHighestWeight());
    std::vector<TraceIndices> indices(1, {CSL_1, t_indices});
    return indices;
    // A priori no need for the following as projectors are applied
    // bool transformed;
    // do {
    //     transformed = contractCloseIndices(indices[0], CR, CA);
    //     if (contractFarIndices(indices, CR, CA)) {
    //         transformed = true;
    //         contractStructureConstants(indices, CR, CA);
    //     }
    // } while (transformed);

    // return indices;
}

bool ColorSpace::isInstance(csl::Expr const &tensor) const
{
    if (!csl::IsIndicialTensor(tensor))
        return false;
    auto spaces = tensor->getParent()->getSpace();
    return spaces.size() == 3
        and spaces[1] == this
        and spaces[2] == this;
}

std::vector<csl::Index> ColorSpace::exprToIndices(
        std::vector<csl::Expr> const &tensors
        ) const
{
    // All tensors should be instances (isInstance() returns true)
    auto index = [&](csl::Expr const &tensor, std::size_t i)
    {
        return tensor->getIndexStructureView()[i];
    };
    std::vector<csl::Index> indices;
    indices.reserve(tensors.size());
    indices.push_back(index(tensors[0], 0));
    csl::Index second = index(tensors[0], 2);
    std::vector<std::size_t> indicesLeft(tensors.size() - 1);
    for (std::size_t i = 0; i != indicesLeft.size(); ++i)
        indicesLeft[i] = i+1;
    for (std::size_t i = 0; i != indicesLeft.size(); ++i) {
        if (index(tensors[indicesLeft[i]], 1) == second) {
            second = index(tensors[indicesLeft[i]], 2);
            indices.push_back(index(tensors[indicesLeft[i]], 0));
            indicesLeft.erase(indicesLeft.begin() + i);
            i = -1;
        }
    }
    HEPAssert(indicesLeft.empty(),
            mty::error::RuntimeError,
            "Could not properly recognize generator chain for trace.")
    return indices;
}

tuple<csl::Index, csl::Index, int> ColorSpace::getFreeIndices(
        const csl::Expr& tensorA,
        const csl::Expr& tensorB
        ) const
{
    const csl::IndexStructure& structA = tensorA->getIndexStructureView();
    if (structA[0].getSpace() == structA[1].getSpace())
        return getFreeIndicesAdjoint(tensorA, tensorB);
    const csl::IndexStructure& structB = tensorB->getIndexStructureView();

    int freeIndexA = -1;
    int freeIndexB = -1;
    for (std::size_t i = 0; i != structA.size(); ++i) 
        if (structA[i].getSpace() != this) {
            freeIndexA = i;
            break;
        }

    for (std::size_t i = 0; i != structB.size(); ++i) 
        if (structB[i].getSpace() != this) {
            freeIndexB = i;
            break;
        }

    return make_tuple(structA[freeIndexA], structB[freeIndexB], 1);
}

tuple<csl::Index, csl::Index, int> ColorSpace::getFreeIndicesAdjoint(
        const csl::Expr& tensorA,
        const csl::Expr& tensorB
        ) const
{
    csl::IndexStructure structA = tensorA->getIndexStructure();
    csl::IndexStructure structB = tensorB->getIndexStructure();
    vector<std::size_t> left(structB.size());
    for (std::size_t i = 0; i != left.size(); ++i)
        left[i] = i;

    int posIndexA = -1;
    for (std::size_t i = 0; i != structA.size(); ++i) {
        bool matched = false;
        for (std::size_t j = 0; j != left.size(); ++j) {
            if (structA[i] == structB[left[j]]) {
                left.erase(left.begin() + j);
                matched = true;
                break;
            }
        }
        if (not matched)
            posIndexA = i;
    }

    int posIndexB;
    if (left.empty()) {
        posIndexA = 0;
        posIndexB = 0;
    }
    else
        posIndexB = left[0];

    int sign = 1;
    if (posIndexA != 0) {
        swap(structA[0], structA[posIndexA]);
        sign *= -1;
    }
    if (posIndexB != 0) {
        swap(structB[0], structB[posIndexB]);
        sign *= -1;
    }
    if (structA[1] == structB[1])
        sign *= -1;

    return make_tuple(structA[0], structB[0], sign);
}

}
