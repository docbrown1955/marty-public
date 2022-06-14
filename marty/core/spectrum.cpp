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

#include "spectrum.h"
#include "modelBuilder.h"
#include "diracology.h"
#include "lagrangian.h"

///////////////////////////////////////////////////
/*************************************************/
// Spectrum class                                //
/*************************************************/
///////////////////////////////////////////////////

namespace mty {

Spectrum::Spectrum(
        std::vector<mty::Particle>     const &t_particles,
        std::vector<mty::Particle>     const &t_newParticles,
        std::vector<csl::Expr>              const &terms,
        std::vector<std::vector<csl::Expr>> const &t_mixing
        )
    :bidiagonalization(false),
    replaceMasses(true),
    replaceMixings(true),
    fields(t_particles),
    newFields(t_newParticles)
{
    setBlock(terms, t_mixing);
}

Spectrum::Spectrum(
        std::vector<mty::Particle>     const &t_newParticles,
        std::vector<std::vector<csl::Expr>> const &t_mass,
        std::vector<std::vector<csl::Expr>> const &t_mixing,
        std::vector<std::vector<csl::Expr>> const &t_mixing2
        )
    :bidiagonalization(!t_mixing2.empty()),
    replaceMasses(true),
    replaceMixings(true),
    newFields(t_newParticles),
    mass(
            t_mass.size(),
            (t_mass.empty()) ? 0 : t_mass[0].size()
          ),
    mixing(
            t_mixing.size(),
            (t_mixing.empty()) ? 0 : t_mixing[0].size()
          ),
    mixing2(
            t_mixing2.size(),
            (t_mixing2.empty()) ? 0 : t_mixing2[0].size()
          )
{
    for (size_t i = 0; i != t_mass.size(); ++i)
        for (size_t j = 0; j != t_mass[i].size(); ++j) {
            mass(i, j)    = t_mass   [i][j];
            mixing(i, j)  = t_mixing [i][j];
            if (bidiagonalization)
                mixing2(i, j) = t_mixing2[i][j];
        }
}

Spectrum::Spectrum(
        std::vector<mty::Particle> const &partA,
        std::vector<mty::Particle> const &partB,
        std::vector<mty::Particle> const &newPartA,
        std::vector<mty::Particle> const &newPartB,
        std::vector<csl::Expr>          const &terms,
        std::vector<std::vector<csl::Expr>> const &mixingA,
        std::vector<std::vector<csl::Expr>> const &mixingB
        )
    :bidiagonalization(true),
    replaceMasses(true),
    replaceMixings(true)
{
    fields.reserve(partA.size() + partB.size());
    fields.insert(fields.end(), partA.begin(), partA.end());
    fields.insert(fields.end(), partB.begin(), partB.end());

    newFields.reserve(partA.size() + partB.size());
    newFields.insert(newFields.end(), newPartA.begin(), newPartA.end());
    newFields.insert(newFields.end(), newPartB.begin(), newPartB.end());

    setBlock(terms, mixingA, mixingB);
}

bool Spectrum::isDiagonalized() const 
{
    return (!bidiagonalization and diagonal and transfer)
        or ( bidiagonalization and diagonal and transfer and transfer2);
}

void Spectrum::setBlock(
        std::vector<csl::Expr>              const &terms,
        std::vector<std::vector<csl::Expr>> const &t_mixing
        )
{
    mass.resize(fields.size(), fields.size());
    mixing.resize(fields.size(), fields.size());
    for (size_t i = 0; i != mass.size1(); ++i)
        for (size_t j = 0; j != mass.size2(); ++j)
            mass(i, j) = 0;
    massData = csl::identity_s(fields.size());
    for (const auto &term : terms) {
        if (term != CSL_0)
            addMassTerm(getMassFromTerm(term));
    }

    for (size_t i = 0; i != t_mixing.size(); ++i) {
        for (size_t j = 0; j != t_mixing[i].size(); ++j) {
            mixing(i, j) = t_mixing[i][j];
        }
    }
}

void Spectrum::setBlock(
        std::vector<csl::Expr>              const &terms,
        std::vector<std::vector<csl::Expr>> const &mixingA,
        std::vector<std::vector<csl::Expr>> const &mixingB
        )
{
    mass.resize(fields.size()/2, fields.size()/2);
    mixing .resize(fields.size()/2, fields.size()/2);
    mixing2.resize(fields.size()/2, fields.size()/2);
    for (size_t i = 0; i != mass.size1(); ++i)
        for (size_t j = 0; j != mass.size2(); ++j)
            mass(i, j) = 0;
    massData = csl::identity_s(fields.size()/2);
    for (const auto &term : terms)
        addMassTerm(getMassFromTerm(term));

    for (size_t i = 0; i != mixingA.size(); ++i)
        for (size_t j = 0; j != mixingA[i].size(); ++j) {
            mixing(i, j) = mixingA[i][j];
        }
    for (size_t i = 0; i != mixingB.size(); ++i)
        for (size_t j = 0; j != mixingB[i].size(); ++j) {
            mixing2(i, j) = mixingB[i][j];
        }
}

void Spectrum::updateData()
{
    // std::cout << "UPDATE ! " << std::endl;

    // std::cout << massData << std::endl;
    bool numerical = true;
    const size_t size = (bidiagonalization) ? fields.size()/2 : fields.size();
    for (size_t i = 0; i != size; ++i) {
        for (size_t j = i; j < size; ++j) {
            // std::cout << mass(i, j) << std::endl;
            massData[i][j] 
                = csl::Evaluated(
                        mass(i, j) / ((i != j) ? 2 : 1), 
                        csl::eval::literal
                        | csl::eval::numerical
                        );
            if (!csl::IsNumerical(massData[i][j]))
                numerical = false;
            if (i != j)
                massData[j][i] = massData[i][j];
        }
    }

    // std::cout << massData << std::endl;

    if (numerical) {
        (bidiagonalization) ? bidiagonalize() : diagonalize();
    }
}

void Spectrum::replace(
        csl::Expr                    &expr,
        std::vector<csl::Expr> const &oldExpr,
        std::vector<csl::Expr> const &newExpr
        )
{
    csl::ForEachLeaf(expr, [&](csl::Expr &sub)
    {
        if (csl::IsLiteral(sub) or csl::IsIndicialTensor(sub)) {
            if (auto parent = csl::Abbrev::find_opt(sub); parent) {
                csl::Expr abbrev = parent->getEncapsulated();
                replace(abbrev, oldExpr, newExpr);
                parent->setEncapsulated(abbrev);
                return;
            }
        }
    });
    csl::Replace(expr, oldExpr, newExpr);
}

void Spectrum::applyOn(csl::Expr &expr) const
{
    // std::cout << "HERE" << std::endl;
    if (!isDiagonalized())
        return;
    (bidiagonalization) ? 
        applyBidiagonalizationOn(expr) 
        :applyDiagonalizationOn(expr);
}

void Spectrum::applyOn(std::vector<csl::Expr> &expr) const
{
    if (!isDiagonalized())
        return;
    for (auto &e : expr)
        applyOn(e);
}

size_t Spectrum::getFieldPos(
        QuantumField const &field
        ) const
{
    for (size_t i = 0; i != fields.size(); ++i)  {
        if (fields[i].get() == field.getParent_info())
            return (bidiagonalization and i >= fields.size() / 2) ?
                i - fields.size() / 2 : i;
    }
    std::cerr << "Searching " << field << " in \n";
    for (size_t i = 0; i != fields.size(); ++i)  
        std::cerr << fields[i]->getName() << " ";
    CallHEPError(mty::error::TypeError,
            "Field " + toString(field.copy()) + " does not correspond to "
            " mass block.")
    return size_t(-1);
}


Spectrum::MatrixEl Spectrum::getMassFromTerm(
        csl::Expr const &term
        ) const
{
    if (csl::IsSum(term)) {
        HEPAssert(term->size() > 0,
                mty::error::RuntimeError,
                "Got unexpected empty mass term " + toString(term))
        MatrixEl element = getMassFromTerm(term[0]);
        for (size_t i = 1; i != term->size(); ++i) {
            element.term += getMassFromTerm(term[i]).term;
        }
        return element;
    }
    csl::Expr expression = csl::DeepCopy(term);
    std::vector<QuantumField> massFields;
    massFields.reserve(2);
    csl::ForEachLeaf(expression, [&](csl::Expr &sub)
    {
        if (csl::IsPow(sub) 
                && IsOfType<QuantumField>(sub[0])
                && csl::IsInteger(sub[1])) {
            size_t n = sub[1]->evaluateScalar();
            for (size_t i = 0; i != n; ++i) 
                massFields.push_back(ConvertTo<QuantumField>(sub[0]));
            sub = CSL_1;
        }
    });
    csl::ForEachLeaf(expression, [&](csl::Expr &sub)
    {
        if (IsOfType<mty::QuantumField>(sub)) {
            massFields.push_back(ConvertTo<QuantumField>(sub));
            sub = CSL_1;
        }
    });

    HEPAssert(massFields.size() >= 1,
            mty::error::RuntimeError,
            "Term " + toString(term) + " contains less than two fields !")
    HEPAssert(massFields.size() <= 2,
            mty::error::RuntimeError,
            "Term " + toString(term) + " contains more than two fields !")
    bool fermionSign = massFields[0].isFermionic()
            && !massFields[0].isComplexConjugate()
            && massFields[1].isComplexConjugate();
    if (fermionSign) {
        bool indexContraction = 
            massFields[0].getIndexStructureView().back()
            == massFields[1].getIndexStructureView().back();
        HEPAssert(indexContraction,
                mty::error::TypeError,
                "Mass term " + toString(term) + " is not supported. "
                "Fermions should be re-ordered with psi^* psi.")
        expression *= CSL_M_1;
    }
    if (massFields.size() == 1)
        // Assuming phi(X)^2 here.
        massFields.push_back(massFields[0]);
    csl::ForEachLeaf(expression, [&](csl::Expr &sub)
    {
        if (csl::IsIndicialTensor(sub) 
                && sub->getParent_info() == mty::dirac4.C_matrix.get()) {
            if (sub->getIndexStructureView()[0] 
                    == massFields[0].getIndexStructureView().back()) {
            // psi C psi
                sub = CSL_1;
            }
            else {
            // psi C^dagger psi = -psi C psi
                sub = CSL_M_1;
            }
        }
    });
    csl::DeepRefresh(expression);
    csl::Expand(expression);
    csl::Factor(expression);

    size_t i = getFieldPos(massFields[0]);
    size_t j = getFieldPos(massFields[1]);
    if (bidiagonalization and i != j) {
        for (size_t k = 0; k != fields.size() / 2; ++k) {
            if (fields[k].get() == massFields[0].getParent_info()) {
                std::swap(i, j);
                break;
            }
        }
    }
    return Spectrum::MatrixEl({
            i,
            j,
            expression
            });
}

void Spectrum::addMassTerm(MatrixEl &&matrixEl)
{
    if (!bidiagonalization and matrixEl.i > matrixEl.j)
        std::swap(matrixEl.i, matrixEl.j);

    mass(matrixEl.i, matrixEl.j) += matrixEl.term;
}

void Spectrum::diagonalize()
{
    csl::Diagonalizer diagonalizer(massData);
    diagonalizer.diagonalize();
    diagonal = diagonalizer.getDiagonal();
    transfer = diagonalizer.getTransform();
}

void Spectrum::bidiagonalize()
{
    csl::Expr M  = massData;
    csl::Expr Mc = csl::GetComplexConjugate(massData->transpose());

    csl::Expr MMc = M->dot(Mc);
    csl::Expr McM = Mc->dot(M);

    csl::Diagonalizer diagonalizer(MMc);
    diagonalizer.diagonalize();
    transfer = diagonalizer.getTransform();
    csl::Expr D2 = diagonalizer.getDiagonal();
    diagonal = csl::DeepCopy(D2);
    for (size_t i = 0; i != csl::Size(diagonal); ++i)
        diagonal[i][i] = csl::Evaluated(csl::sqrt_s(diagonal[i][i]),
                                        csl::eval::numerical);

    diagonalizer = csl::Diagonalizer(McM);
    diagonalizer.diagonalize();
    transfer2 = diagonalizer.getTransform();
    csl::Evaluate(transfer,  csl::eval::numerical);
    csl::Evaluate(transfer2, csl::eval::numerical);
    HEPAssert(D2 == diagonalizer.getDiagonal(),
            mty::error::RuntimeError,
            "Bidiagonalization found two different diagonal matrices, "
            + toString(D2) + " and " + toString(diagonalizer.getDiagonal()))
}

void Spectrum::applyDiagonalizationOn(csl::Expr &expr) const
{
    // std::cout << "HERE" << std::endl;
    const size_t size = fields.size();
    std::vector<csl::Expr> oldMasses(size);
    std::vector<csl::Expr> newMasses(size);
    for (size_t i = 0; i != size; ++i) {
        oldMasses[i] = newFields[i]->getMass();
        newMasses[i] = diagonal[i][i];
        oldMasses[i]->setValue(newMasses[i]);
        newFields[i]->setMass(newMasses[i]);
        // std::cout << "Replacing" << std::endl;
        // std::cout << oldMasses[i] << "  " << newMasses[i] << std::endl;
    }
    if (replaceMasses)
        replace(expr, oldMasses, newMasses);

    std::vector<csl::Expr> oldMixing(size * size);
    std::vector<csl::Expr> newMixing(size * size);
    for (size_t i = 0; i != size; ++i)
        for (size_t j = 0; j != size; ++j) {
            size_t index = size * i + j;
            oldMixing[index] = mixing(i, j);
            newMixing[index] = transfer[i][j];
            oldMixing[index]->setValue(newMixing[index]);
            // std::cout << "Replacing" << std::endl;
            // std::cout << oldMixing[index] << "  " << newMixing[index] << std::endl;
        }
    if (replaceMixings)
        replace(expr, oldMixing, newMixing);
}

void Spectrum::applyBidiagonalizationOn(csl::Expr &expr) const
{
    const size_t size = fields.size() / 2;
    std::vector<csl::Expr> oldMasses(size);
    std::vector<csl::Expr> newMasses(size);
    for (size_t i = 0; i != size; ++i) {
        oldMasses[i] = newFields[i]->getMass();
        newMasses[i] = diagonal[i][i];
        oldMasses[i]->setValue(newMasses[i]);
    }
    if (replaceMasses)
        replace(expr, oldMasses, newMasses);

    std::vector<csl::Expr> oldMixing(2 * size*size);
    std::vector<csl::Expr> newMixing(2 * size*size);
    for (size_t i = 0; i != size; ++i)
        for (size_t j = 0; j != size; ++j) {
            const size_t index = size * i + j;
            oldMixing[2*index] = mixing(i, j);
            newMixing[2*index] = transfer[i][j];
            oldMixing[2*index]->setValue(newMixing[2*index]);
            oldMixing[2*index + 1] = mixing2(i, j);
            newMixing[2*index + 1] = transfer2[i][j];
            oldMixing[2*index + 1]->setValue(newMixing[2*index + 1]);
        }
    if (replaceMixings)
        replace(expr, oldMixing, newMixing);
}

} // End of namespace mty
