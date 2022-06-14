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

#include "polarization.h"
#include "quantumField.h"
#include "ghostField.h"
#include "kinematics.h"
#include "insertion.h"
#include "mrtError.h"
#include "graph.h"

namespace mty {

    Kinematics::Kinematics(std::vector<mty::Insertion> const &t_insertions)
        :Kinematics(t_insertions, defaultIndices(t_insertions.size()))
    {

    }

    Kinematics::Kinematics(
            std::vector<mty::Insertion> const &t_insertions,
            std::vector<csl::Tensor>    const &t_momenta
            )
        :insertions(t_insertions),
        momenta(t_momenta),
        indices(defaultIndices(insertions.size()))
    {
        HEPAssert(insertions.size() == momenta.size(),
                mty::error::ValueError,
                "Expecting same number of insertions and momenta, got "
                + std::to_string(insertions.size()) + " and "
                + std::to_string(momenta.size()) + ".")
        initMomentaSquared(indices);
    }

    Kinematics::Kinematics(
            std::vector<mty::Insertion> const &t_insertions,
            std::vector<size_t>         const &t_indices
            )
        :insertions(t_insertions),
        indices(t_indices)
    {
        momenta.resize(insertions.size());
        std::generate(begin(momenta), end(momenta), [&, i = 0]() mutable {
            auto p = csl::Tensor(
                    "p_" + std::to_string(indices[i]),
                    &csl::Minkowski);
            ++i;
            return p;
        });
        initMomentaSquared(indices);
    }

    std::vector<csl::Tensor> Kinematics::getOrderedMomenta() const
    {
        std::vector<csl::Tensor> res(momenta.size());
        for (size_t i = 0; i != momenta.size(); ++i) {
            res[indices[i]-1] = momenta[i];
        }
        return res;
    }

    template<class Type>
    static void removeIndices(
            std::vector<Type>  &data,
            std::vector<size_t> indices
            )
    {
        std::sort(indices.begin(), indices.end(), std::greater<size_t>());
        for (size_t index : indices) {
            data.erase(data.begin() + index);
        }
    }

    static bool areIdenticalFields(
            Insertion const &A,
            Insertion const &B
            )
    {
        return A.getField() == B.getField()
            && A.isIncoming() == B.isIncoming()
            && (A.isParticle() == B.isParticle()
                    || A.getField()->isSelfConjugate()
                    || IsOfType<GhostBoson>(A.getField()));
    }

    csl::Expr Kinematics::getDegreesOfFreedomFactor() const
    {
        csl::Expr factor = CSL_1;
        std::vector<size_t> outgoingIndices;
        outgoingIndices.reserve(insertions.size());
        for (size_t i = 0; i != insertions.size(); ++i) {
            if (insertions[i].isIncoming())
                factor *= insertions[i].getField()->getNDegreesOfFreedom();
            else
                outgoingIndices.push_back(i);
        }

        while (!outgoingIndices.empty()) {
            const size_t i = outgoingIndices[0];
            std::vector<size_t> identicalFields;
            identicalFields.reserve(insertions.size());
            identicalFields.push_back(0);
            for (size_t index = 1; index != outgoingIndices.size(); ++index) {
                const size_t j = outgoingIndices[index];
                if (areIdenticalFields(insertions[i], insertions[j])) {
                    identicalFields.push_back(index);
                }
            }
            int combi = std::tgamma(static_cast<int>(1+identicalFields.size()));
            if (combi != 1)
                factor *= combi;
            removeIndices(outgoingIndices, identicalFields);
        }
        return factor;
    }

    void Kinematics::setExternalSpinTensors(
        std::vector<csl::Expr> const &terms
        )
    {
        HEPAssert(terms.size() == size(),
            mty::error::IndexError,
            "Invalid number of spin tensors (" + std::to_string(terms.size())
            + ") for a Kinematics object of size " + std::to_string(size()) 
            + ".")
        auto isSpinTensor = [](csl::Expr const &tensor) {
            return IsOfType<PolarizationField>(tensor);
        };
        externalSpinTensors = std::vector<csl::Expr>(terms.size());
        for (size_t i = 0; i != terms.size(); ++i) {
            csl::Expr spinTensor = csl::FindIfLeaf(terms[i], isSpinTensor);
            if (!spinTensor) {
                spinTensor = CSL_UNDEF;
            }
            externalSpinTensors[i] = csl::Copy(spinTensor);
        }
    }

    void Kinematics::initMomentaSquared(std::vector<size_t> const &indices)
    {
        for (size_t i = 0; i != insertions.size(); ++i)  {
            if (insertions[i].isOnShell()) {
                auto &p = momenta[i];
                addContraction(p, p, insertions[i].getField()->getSquaredMass());
            }
        }
        squaredMomenta.resize(momenta.size() * momenta.size());
        const csl::Index mu = csl::Minkowski.generateIndex();
        for (size_t i = 0; i != momenta.size(); ++i) {
            if (insertions[i].isMediator())
                continue;
            for (size_t j = i; j != momenta.size(); ++j) {
                if (insertions[j].isMediator())
                    continue;
                csl::Expr prod = momenta[i](mu) * momenta[j](+mu);
                if (prod->getIndexStructure().size() > 0) {
                    prod = csl::constant_s(
                            "s_" + std::to_string(10*(indices[i]) + indices[j])
                            );
                    addContraction(momenta[i], momenta[j], prod);
                }
                setSquaredMomentum(i, j, prod);
            }
        }
        for (auto &s : squaredMomenta)
            if (!s) 
                s = CSL_UNDEF;
    }

    void Kinematics::addContraction(
            csl::Tensor     &p1,
            csl::Tensor     &p2,
            csl::Expr const &res
            )
    {
        const csl::Index mu = csl::Minkowski.generateIndex();
        p1->addSelfContraction(p1(mu), p2(+mu), res);
    }

    void Kinematics::setSquaredMomentum(
            size_t           i, 
            size_t           j,
            csl::Expr const &res)
    {
        squaredMomenta[squaredMomentumIndex(i, j)] = res;
        if (i != j) {
            squaredMomenta[squaredMomentumIndex(j, i)] = res;
        }
    }

    Kinematics Kinematics::subset(std::vector<size_t> pos) const
    {
        const size_t sz = pos.size();

        Kinematics res;
        res.insertions.reserve(sz);
        res.externalSpinTensors.reserve(sz);
        res.momenta.reserve(sz);
        res.indices.reserve(sz);
        res.squaredMomenta.reserve(sz * sz);

        for (const size_t i : pos) {
            res.insertions.push_back(insertions[i]);
            res.momenta.push_back(momenta[i]);
            res.indices.push_back(indices[i]);
            res.externalSpinTensors.push_back(externalSpinTensors[i]);
            for (const size_t j : pos) {
                res.squaredMomenta.push_back(
                        squaredMomenta[squaredMomentumIndex(i, j)]
                        );
            }
        }

        return res;
    }

    void Kinematics::applyPermutation(std::vector<size_t> const &pos)
    {
        HEPAssert(pos.size() == size(),
                mty::error::ValueError,
                "Expecting permutation of the same size as the object "
                + std::to_string(pos.size()) + " given, expected "
                + std::to_string(size()) + ".")
        if (std::is_sorted(begin(pos), end(pos)))
            return;
        *this = subset(pos);
    }

    void Kinematics::sortFromIndices()
    {
        const size_t sz = size();
        std::vector<size_t> minPos;
        minPos.reserve(sz);
        std::vector<size_t> indicesLeft(sz);
        std::iota(indicesLeft.begin(), indicesLeft.end(), 0);
        while (!indicesLeft.empty()) {
            size_t mini = 0;
            for (size_t i = 1; i != indicesLeft.size(); ++i) {
                const size_t index = indicesLeft[i];
                if (indices[index] < indices[indicesLeft[mini]])
                    mini = i;
            }
            minPos.push_back(indicesLeft[mini]);
            indicesLeft.erase(indicesLeft.begin() + mini);
        }
        *this = subset(minPos);
    }

    Kinematics Kinematics::alignedWith(Kinematics const &other) const
    {
        HEPAssert(size() == other.size(),
                mty::error::RuntimeError,
                "Cannot align the following kinematics " 
                + toString(*this) + '\n' + toString(other))
        std::vector<mty::Insertion> alignedInsertions = insertions;
        std::vector<csl::Tensor>    alignedMomenta    = momenta;
        std::vector<csl::Expr>      alignedSpins      = externalSpinTensors;
        bool hasSpins = !alignedSpins.empty();
        for (size_t i = 0; i != size(); ++i) {
            bool match = false;
            for (size_t j = i; j != size(); ++j) {
                if (other.insertions[i] == alignedInsertions[j]) {
                    if (i != j) {
                        std::swap(alignedInsertions[i], alignedInsertions[j]);
                        std::swap(alignedMomenta[i],    alignedMomenta[j]);
                        if (hasSpins)
                          std::swap(alignedSpins[i], alignedSpins[j]);
                    }
                    match = true;
                    break;
                }
            }
            HEPAssert(match,
                    mty::error::RuntimeError,
                    "Cannot align the following kinematics " 
                    + toString(*this) + '\n' + toString(other))
        }
        auto kin = Kinematics { alignedInsertions, alignedMomenta };
        if (hasSpins)
          kin.setExternalSpinTensors(alignedSpins);
        return kin;
    }

    Kinematics Kinematics::applyIndices(
            std::vector<size_t> const &t_indices
            ) const
    {
        auto kin = Kinematics{insertions, t_indices};
        kin.setExternalSpinTensors(externalSpinTensors);
        return kin;
    }

    void Kinematics::replace(
            csl::Expr        &expr,
            Kinematics const &k1,
            Kinematics const &k2
            )
    {
        HEPAssert(k1.size() == k2.size(),
                mty::error::ValueError,
                "Kinematics should have the same size for replacement, got " +
                std::to_string(k1.size()) + " and " + std::to_string(k2.size())
                + ".")
        if (k1.externalSpinTensors.size() == k2.externalSpinTensors.size()) {
            for (size_t i = 0 ; i != k1.externalSpinTensors.size(); ++i) {
                csl::Expr spin1 = k1.getExternalSpinTensors()[i];
                csl::Expr spin2 = k2.getExternalSpinTensors()[i];
                bool isSpin1Def = (spin1 != CSL_UNDEF);
                bool isSpin2Def = (spin2 != CSL_UNDEF);
                HEPAssert(isSpin1Def == isSpin2Def,
                    mty::error::RuntimeError,
                    "Encountered different external spin tensors in "
                    "kinematics replacement.")
                if (isSpin1Def) {
                    csl::Index polarIndex1 = spin1->getIndexStructureView()[0];
                    csl::Index polarIndex2 = spin2->getIndexStructureView()[0];
                    csl::Replace(expr, polarIndex1, polarIndex2);
                }
            }
        }
        csl::Replace(expr, k1.getMomenta(),        k2.getMomenta());
        csl::Replace(expr, k1.getSquaredMomenta(), k2.getSquaredMomenta());
    }

    Kinematics Kinematics::merge(
            Kinematics const &k1,
            Kinematics const &k2
            )
    {
        Kinematics res;
        const size_t s1 = k1.size();
        const size_t s2 = k2.size();
        const size_t sT = s1 + s2;
        res.insertions.reserve(sT);
        res.indices.reserve(sT);
        res.momenta.reserve(sT);
        res.externalSpinTensors.reserve(sT);
        for (size_t i1 = 0; i1 != s1; ++i1) {
            res.insertions.push_back(k1.insertions[i1]);
            res.momenta.push_back(k1.momenta[i1]);
            res.indices.push_back(k1.indices[i1]);
            res.externalSpinTensors.push_back(k1.externalSpinTensors[i1]);
        }
        for (size_t i2 = 0; i2 != s2; ++i2) {
            res.insertions.push_back(k2.insertions[i2]);
            res.momenta.push_back(k2.momenta[i2]);
            res.indices.push_back(k2.indices[i2]);
            res.externalSpinTensors.push_back(k2.externalSpinTensors[i2]);
        }
        res.squaredMomenta.resize(sT*sT);
        res.initMomentaSquared(res.indices);

        std::vector<size_t> subset(res.size());
        std::iota(subset.begin(), subset.end(), 0);
        auto last = std::remove_if(subset.begin(), subset.end(), [&](size_t pos) {
            return res.insertions[pos].isMediator();
        });
        subset.erase(last, subset.end());
        return res.subset(subset);
    }

    std::ostream &operator<<(
            std::ostream     &out,
            Kinematics const &kin
            )
    {
        out << "Kinematics of " << kin.size() << " particles\n";
        out << "Insertions : \n";
        for (size_t i = 0; i != kin.size(); ++i) {
            out << "  --> " << kin.insertions[i].getField()->getName();
            out << " , momentum : " << kin.momenta[i]->getName() << '\n';
        }
        for (size_t i = 0; i != kin.size(); ++i) {
            for (size_t j = 0; j != kin.size(); ++j) {
                out << kin.squaredMomenta[kin.squaredMomentumIndex(i, j)] << ' ';
            }
            out << '\n';
        }
        return out;
    }

} // namespace mty
