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

#include "quantumField.h"
#include "kinematics.h"
#include "insertion.h"
#include "mrtError.h"

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
        momenta(t_momenta)
    {
        HEPAssert(insertions.size() == momenta.size(),
                mty::error::ValueError,
                "Expecting same number of insertions and momenta, got "
                + std::to_string(insertions.size()) + " and "
                + std::to_string(momenta.size()) + ".")
        initMomentaSquared(defaultIndices(insertions.size()));
    }

    Kinematics::Kinematics(
            std::vector<mty::Insertion> const &t_insertions,
            std::vector<size_t>         const &indices
            )
        :insertions(t_insertions)
    {
        momenta.resize(insertions.size());
        std::generate(begin(momenta), end(momenta), [&, i = 0]() mutable {
            auto p = csl::Tensor(
                    "p_" + std::to_string(indices[i]+1),
                    &csl::Minkowski);
            ++i;
            return p;
        });
        initMomentaSquared(indices);
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
                    || A.getField()->isSelfConjugate());
    }

    csl::Expr Kinematics::getDegreesOfFreedomFactor() const
    {
        csl::Expr factor = CSL_1;
        std::vector<size_t> incomingIndices;
        incomingIndices.reserve(insertions.size());
        for (size_t i = 0; i != insertions.size(); ++i) {
            if (insertions[i].isIncoming())
                factor *= insertions[i].getField()->getNDegreesOfFreedom();
            else
                incomingIndices.push_back(i);
        }

        while (!incomingIndices.empty()) {
            const size_t i = incomingIndices[0];
            std::vector<size_t> identicalFields;
            identicalFields.reserve(insertions.size());
            identicalFields.push_back(0);
            for (size_t index = 1; index != incomingIndices.size(); ++index) {
                const size_t j = incomingIndices[index];
                if (areIdenticalFields(insertions[i], insertions[j])) {
                    identicalFields.push_back(index);
                }
            }
            int combi = std::tgamma(static_cast<int>(1+identicalFields.size()));
            if (combi != 1)
                factor *= combi;
            removeIndices(incomingIndices, identicalFields);
        }
        return factor;
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
            for (size_t j = i; j != momenta.size(); ++j) {
                csl::Expr prod = momenta[i](mu) * momenta[j](+mu);
                if (prod->getIndexStructure().size() > 0) {
                    prod = csl::constant_s(
                            "s_" + std::to_string(10*(indices[i]+1) + indices[j]+1)
                            );
                    addContraction(momenta[i], momenta[j], prod);
                }
                setSquaredMomenta(i, j, prod);
            }
        }
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

    void Kinematics::setSquaredMomenta(
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
        res.momenta.reserve(sz);
        res.squaredMomenta.reserve(sz * sz);

        for (const size_t i : pos) {
            res.insertions.push_back(insertions[i]);
            res.momenta.push_back(momenta[i]);
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

    Kinematics Kinematics::applyMap(
            std::vector<size_t> const &mapping
            ) const
    {
        return Kinematics{insertions, mapping};
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
        csl::Replace(expr, k1.getMomenta(),        k2.getMomenta());
        csl::Replace(expr, k1.getSquaredMomenta(), k2.getSquaredMomenta());
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
