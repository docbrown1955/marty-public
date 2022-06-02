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

#include "modelBuilder.h"
#include "amplitude.h"
#include "diracology.h"
#include "ghostField.h"
#include "goldstoneField.h"
#include "mrtInterface.h"
#include "mrtOptions.h"
#include "quantumFieldTheory.h"
#include "vectorField.h"
#include <numeric>

namespace mty {

///////////////////////////////////////////////////
// Replacement functions
///////////////////////////////////////////////////

void ModelBuilder::addAbbreviatedMassExpression(csl::Expr const &abbreviation)
{
    auto pos
        = std::find_if(abbreviatedMassExpressions.begin(),
                       abbreviatedMassExpressions.end(),
                       [&](csl::Expr const &expr) {
                           return expr->getName() == abbreviation->getName();
                       });
    if (pos != abbreviatedMassExpressions.end()) {
        *pos == abbreviation;
    }
    else {
        abbreviatedMassExpressions.push_back(abbreviation);
    }
}

void ModelBuilder::replace(csl::Expr const &oldExpression,
                           csl::Expr const &newExpression)
{
    Particle init;
    if (IsOfType<QuantumField>(oldExpression)
        && !oldExpression->isComplexConjugate())
        init = ConvertToPtr<QuantumField>(oldExpression)->getParticle();

    csl::VisitEachLeaf(newExpression, [&](csl::Expr const &sub) {
        if (IsOfType<mty::QuantumField>(sub)) {
            auto parent = sub->getParent();
            auto part = std::dynamic_pointer_cast<QuantumFieldParent>(parent);
            if (init and init.get() == part.get())
                init = nullptr;
            else if (not findParticle(part))
                addParticle(part, false);
        }
    });
    csl::vector_expr terms
        = clearDependencies([&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(oldExpression.get());
          });

    for (auto &term : terms) {
        csl::Replace(term, oldExpression, newExpression);
        csl::Expand(term);
        L.push_back(term);
    }
    csl::Abbrev::replace(oldExpression, newExpression);
    if (init)
        removeParticle(init);

    // Abbreviated mass expressions check
    if (!oldExpression->isBuildingBlock())
        return;
    auto pos
        = std::find_if(abbreviatedMassExpressions.begin(),
                       abbreviatedMassExpressions.end(),
                       [&](csl::Expr const &expr) {
                           return expr->getName() == oldExpression->getName();
                       });
    if (pos != abbreviatedMassExpressions.end()) {
        if (csl::Abbrev::isAnAbbreviation(newExpression)) {
            *pos = newExpression;
        }
        else {
            abbreviatedMassExpressions.erase(pos);
        }
    }
}

void ModelBuilder::replace(csl::Tensor     &oldExpression,
                           csl::Expr const &newExpression)
{
    csl::vector_expr terms
        = clearDependencies([&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(oldExpression.get());
          });

    for (auto &term : terms) {
        csl::Replace(term, oldExpression, newExpression);
        csl::Expand(term);
        L.push_back(term);
    }
    csl::Abbrev::replace(oldExpression, newExpression);
}

void ModelBuilder::replace(csl::Tensor &oldExpression,
                           csl::Tensor &newExpression,
                           std::function<bool(csl::Expr const &)> condition)
{
    csl::vector_expr terms
        = clearDependencies([&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(oldExpression.get());
          });

    for (auto &term : terms) {
        if (condition(term)) {
            csl::Replace(term, oldExpression, newExpression);
            csl::Expand(term);
        }
        L.push_back(term);
    }
    csl::Abbrev::replace(oldExpression, newExpression);
}

void ModelBuilder::replace(mty::Particle const &part,
                           csl::Expr            newExpression,
                           std::function<bool(csl::Expr const &)> condition)
{
    csl::Replace(newExpression, newExpression->getPoint(), L.getPoint());
    bool particleInFinalState = false;
    csl::VisitEachLeaf(newExpression, [&](csl::Expr const &sub) {
        if (IsOfType<mty::QuantumField>(sub)) {
            auto parent = sub->getParent();
            auto localPart
                = std::dynamic_pointer_cast<QuantumFieldParent>(parent);
            if (part.get() == localPart.get())
                particleInFinalState = true;
            else if (not findParticle(localPart))
                addParticle(localPart, false);
        }
    });

    csl::vector_expr terms
        = clearDependencies([&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(part.get());
          });
    for (auto &term : terms) {
        if (condition(term)) {
            csl::Replace(term, part, newExpression);
            csl::Expand(term);
        }
        L.push_back(term);
    }
    // if (!particleInFinalState)
    //     removeParticle(part);
}

void ModelBuilder::rotateFields(
    mty::Particle                         &fields,
    csl::Tensor                           &U,
    std::function<bool(csl::Expr const &)> condition)
{
    auto uSpaces = U->getSpace();
    HEPAssert(uSpaces.size() == 2 and uSpaces[0] == uSpaces[1],
              mty::error::TypeError,
              "Expecting unitary matrix, dimensions do not match.");

    csl::Space const       *space   = uSpaces[0];
    csl::Tensor             X       = L.getPoint();
    std::vector<csl::Index> indices = fields->getFullSetOfIndices();
    csl::Index              i;
    csl::Index              j              = space->generateIndex();
    csl::Expr               old_expression = fields(indices, X);
    for (auto &i_field : indices)
        if (i_field.getSpace() == space) {
            i       = i_field;
            i_field = j;
        }
    csl::Expr new_expression = U({i, j}) * fields(indices, X);
    replace(fields, new_expression, condition);
}

void ModelBuilder::checksRotation(
    std::vector<mty::Particle> const          &fields,
    std::vector<mty::Particle> const          &newFields,
    std::vector<std::vector<csl::Expr>> const &rotation)
{
    HEPAssert(fields.size() > 1,
              mty::error::ValueError,
              "Should rotate at least two fields.");
    HEPAssert(fields.size() == newFields.size(),
              mty::error::ValueError,
              "Please provide the same number of fields before and "
              "after rotation.");
    HEPAssert(rotation.size() == fields.size(),
              mty::error::ValueError,
              "Rotation matrix must be a square matrix of size the number of "
              "fields.");
    for (const auto &row : rotation)
        HEPAssert(row.size() == fields.size(),
                  mty::error::ValueError,
                  "Rotation matrix must be a square matrix of size the number "
                  "of "
                  "fields.");

    for (size_t i = 0; i != fields.size(); ++i) {
        HEPAssert(fields[i]->getSpinDimension()
                          == newFields[i]->getSpinDimension()
                      and fields[i]->getGaugeIrrep()
                              == newFields[i]->getGaugeIrrep(),
                  mty::error::ValueError,
                  "Fields in mixing must have the same spin and gauge "
                  "representation.");
        for (size_t j = i + 1; j < fields.size(); ++j)
            HEPAssert(fields[i]->getSpinDimension()
                              == fields[j]->getSpinDimension()
                          and fields[i]->getGaugeIrrep()
                                  == fields[j]->getGaugeIrrep(),
                      mty::error::ValueError,
                      "Fields in mixing must have the same spin and gauge "
                      "representation.");
    }
}

void ModelBuilder::fillDependencies(
    std::vector<csl::Expr>                           &kinetic,
    std::vector<csl::Expr>                           &mass,
    std::vector<csl::Expr>                           &interaction,
    std::function<bool(Lagrangian::TermType const &)> dependencyFunc)
{
    kinetic     = clearDependencies(L.kinetic, dependencyFunc);
    mass        = clearDependencies(L.mass, dependencyFunc);
    interaction = clearDependencies(L.interaction, dependencyFunc);
}

std::vector<csl::Expr> ModelBuilder::getRotationTerms(
    std::vector<mty::Particle> const          &newFields,
    std::vector<std::vector<csl::Expr>> const &rotation) const
{
    std::vector<csl::Index> indices = newFields[0]->getFullSetOfIndices();
    std::vector<csl::Expr>  newExpressions(newFields.size());
    for (size_t i = 0; i != newFields.size(); ++i) {
        std::vector<csl::Expr> terms;
        for (size_t j = 0; j != newFields.size(); ++j)
            if (rotation[i][j] != CSL_0) {
                mty::Particle newField = newFields[j];
                terms.push_back(rotation[i][j]
                                * newField(indices, L.getPoint()));
            }
        newExpressions[i] = csl::sum_s(terms);
    }

    return newExpressions;
}

std::vector<csl::Expr>
ModelBuilder::getFullMassMatrix(std::vector<mty::Particle> const &fields) const
{
    std::vector<csl::Expr> fullMassMatrix;
    csl::Expr factor = (fields[0]->getSpinDimension() == 3) ? CSL_1 : CSL_M_1;
    for (const auto &mass : L.mass) {
        bool found = false;
        for (const auto &f : fields) {
            for (const auto &c : mass->getContent())
                if (f.get() == c.getParent_info()
                    and (c.isComplexConjugate() or c.isSelfConjugate())) {
                    fullMassMatrix.push_back(factor * mass->getTerm());
                    found = true;
                    break;
                }
            if (found)
                break;
        }
    }
    // Merging
    std::vector<Lagrangian::TermType> terms
        = InteractionTerm::createAndDispatch(csl::sum_s(fullMassMatrix));
    fullMassMatrix.clear();
    bool realField = (fields[0]->isSelfConjugate() && fields[0]->isBosonic());
    for (const auto &t : terms) {
        fullMassMatrix.push_back((realField) ? 2 * t->getTerm()
                                             : t->getTerm());
    }

    return fullMassMatrix;
}

void ModelBuilder::fillDependenciesForRotation(
    std::vector<csl::Expr> &kinetic,
    std::vector<csl::Expr> &interaction,
    mty::Particle const    &field)
{
    std::vector<csl::Expr> mass;
    fillDependencies(
        kinetic, mass, interaction, [&](Lagrangian::TermType const &term) {
            for (const auto &content : term->getContent())
                if (field.get() == content.getQuantumParent())
                    return true;
            return false;
        });
    interaction.insert(interaction.end(),
                       std::make_move_iterator(mass.begin()),
                       std::make_move_iterator(mass.end()));
}

void ModelBuilder::fillDependenciesForRotation(
    std::vector<csl::Expr>           &kinetic,
    std::vector<csl::Expr>           &interaction,
    std::vector<mty::Particle> const &fields)
{
    std::vector<csl::Expr> mass;
    fillDependencies(
        kinetic, mass, interaction, [&](Lagrangian::TermType const &term) {
            for (const auto &content : term->getContent())
                for (const auto &f : fields)
                    if (f.get() == content.getQuantumParent())
                        return true;
            return false;
        });
    interaction.insert(interaction.end(),
                       std::make_move_iterator(mass.begin()),
                       std::make_move_iterator(mass.end()));
}

void ModelBuilder::applyRotation(mty::Particle const &field,
                                 mty::Particle const &newField,
                                 csl::Expr const     &rotation)
{
    std::vector<csl::Expr> kineticTerms;
    std::vector<csl::Expr> interactionTerms;
    fillDependenciesForRotation(kineticTerms, interactionTerms, field);
    csl::ProgressBar bar(interactionTerms.size() + kineticTerms.size(),
                         "Rotating fields in Lagrangian");
    size_t           index = 0;
    for (auto &term : interactionTerms) {
        bar.progress(index++);
        term = csl::Replaced(term, field, rotation);
    }
    for (auto &term : kineticTerms) {
        bar.progress(index++);
        csl::Replace(term, field, newField);
    }
    bar.reset(interactionTerms.size() + kineticTerms.size(),
              "Adding new terms in Lagrangian");
    index = 0;
    for (auto &term : interactionTerms) {
        bar.progress(index++);
        L.push_back(term);
    }
    for (auto &term : kineticTerms) {
        bar.progress(index++);
        L.push_back(term);
    }
    L.mergeTerms();
}

void ModelBuilder::applyRotation(std::vector<mty::Particle> const &fields,
                                 std::vector<mty::Particle> const &newFields,
                                 std::vector<csl::Expr> const     &rotations)
{
    std::vector<csl::Expr> kineticTerms;
    std::vector<csl::Expr> interactionTerms;
    fillDependenciesForRotation(kineticTerms, interactionTerms, fields);
    csl::ProgressBar bar(interactionTerms.size() + kineticTerms.size(),
                         "Rotating fields in Lagrangian");
    size_t           index = 0;
    for (auto &term : interactionTerms) {
        bar.progress(index++);
        csl::Replace(term, fields, rotations);
    }
    std::vector<csl::Parent> fieldParent(fields.size());
    std::vector<csl::Parent> newFieldParent(fields.size());
    for (size_t i = 0; i != fieldParent.size(); ++i) {
        fieldParent[i]    = fields[i];
        newFieldParent[i] = newFields[i];
    }
    for (auto &term : kineticTerms) {
        bar.progress(index++);
        csl::Replace(term, fieldParent, newFieldParent);
    }
    bar.reset(interactionTerms.size() + kineticTerms.size(),
              "Adding new terms in Lagrangian");
    index = 0;
    for (auto &term : interactionTerms) {
        bar.progress(index++);
        L.push_back(term);
    }
    for (const auto &term : kineticTerms) {
        bar.progress(index++);
        L.push_back(term);
    }
    L.mergeTerms();
}

void ModelBuilder::diagonalizeWithSpectrum(
    std::vector<mty::Particle> const          &fields,
    std::vector<mty::Particle> const          &newFields,
    std::vector<std::vector<csl::Expr>> const &mixing,
    std::vector<csl::Expr> const              &massMatrix)
{
    [[maybe_unused]] std::vector<csl::Expr> terms
        = clearDependencies(L.mass, [&](Lagrangian::TermType const &term) {
              for (const auto &field : term->getContent())
                  for (const auto &f : fields)
                      if (f.get() == field.getParent_info())
                          return true;
              return false;
          });
    for (const auto &f : newFields) {
        csl::Expr mass = csl::constant_s("m_" + std::string(f->getName()));
        f->setMass(mass);
        if (f->isBosonic())
            addBosonicMass(f, mass);
        else
            addFermionicMass(f, mass);
    }
    for (const auto &row : mixing)
        for (const auto &el : row)
            if (el->size() > 0)
                return;
    addParticleFamily(newFields);
    spectra.push_back({fields, newFields, massMatrix, mixing});
}

void ModelBuilder::bidiagonalizeWithSpectrum(
    std::vector<mty::Particle> const          &fields1,
    std::vector<mty::Particle> const          &newFields1,
    std::vector<std::vector<csl::Expr>> const &mixing1,
    std::vector<mty::Particle> const          &fields2,
    std::vector<mty::Particle> const          &newFields2,
    std::vector<std::vector<csl::Expr>> const &mixing2,
    std::vector<csl::Expr> const              &massMatrix)
{
    auto l = clearDependencies(L.mass, [&](Lagrangian::TermType const &term) {
        for (const auto &field : term->getContent()) {
            for (const auto &f : newFields1)
                if (f.get() == field.getParent_info())
                    return true;
            for (const auto &f : newFields2)
                if (f.get() == field.getParent_info())
                    return true;
        }
        return false;
    });
    for (size_t i = 0; i != newFields1.size(); ++i) {
        const auto &f1   = newFields1[i];
        const auto &f2   = newFields2[i];
        csl::Expr   mass = csl::constant_s("m_" + std::string(f1->getName()));
        fields1[i]->setMass(mass);
        fields2[i]->setMass(mass);
        f1->setMass(mass);
        f2->setMass(mass);
        addLagrangianTerm(mty::MassTerm(mass, f1.get(), f2.get()), false);
    }
    for (const auto &row : mixing1)
        for (const auto &el : row)
            if (el->size() > 0)
                return;
    for (const auto &row : mixing2)
        for (const auto &el : row)
            if (el->size() > 0)
                return;
    addParticleFamily(newFields1);
    addParticleFamily(newFields2);
    spectra.push_back({fields1,
                       fields2,
                       newFields1,
                       newFields2,
                       massMatrix,
                       mixing1,
                       mixing2});
}

void ModelBuilder::rotateFields(
    std::vector<mty::Particle> const          &fields,
    std::vector<mty::Particle> const          &newFields,
    std::vector<std::vector<csl::Expr>> const &rotation,
    bool                                       diagonalizeMasses,
    int                                        nMassLessFields)
{
    if (!fields.empty() && fields[0]->getFieldStrength()) {
        std::vector<mty::Particle> fs1(fields.size());
        std::vector<mty::Particle> fs2(fields.size());
        for (size_t i = 0; i != fs1.size(); ++i) {
            fs1[i] = fields[i]->getFieldStrength();
            fs2[i] = newFields[i]->getFieldStrength();
        }
        rotateFields(fs1, fs2, rotation, false);
    }
    checksRotation(fields, newFields, rotation);

    std::vector<csl::Expr> newExpressions
        = getRotationTerms(newFields, rotation);
    std::vector<csl::Expr> fullMassMatrix;
    if (diagonalizeMasses)
        fullMassMatrix = getFullMassMatrix(fields);

    if (fields.size() >= 3) {
        for (size_t i = 0; i < fields.size(); ++i) {
            std::cout << "Replacing " << fields[i]->getName() << " ..."
                      << std::endl;
            applyRotation(fields[i], newFields[i], newExpressions[i]);
        }
    }
    else {
        applyRotation(fields, newFields, newExpressions);
    }
    removeParticles(fields);
    addParticles(newFields, false);

    if (diagonalizeMasses) {
        diagonalizeWithSpectrum(fields, newFields, rotation, fullMassMatrix);
        for (int i = 0; i < nMassLessFields; ++i) {
            newFields[i]->setMass(CSL_0);
            clearDependencies(L.mass, [&](Lagrangian::TermType const &t) {
                return t->containsExactly(newFields[i].get());
            });
        }
    }
    applyUnitaryCondition(rotation);
}

void ModelBuilder::rotateFields(
    std::vector<mty::Particle> const          &fields1,
    std::vector<mty::Particle> const          &newFields1,
    std::vector<std::vector<csl::Expr>> const &rotation1,
    std::vector<mty::Particle> const          &fields2,
    std::vector<mty::Particle> const          &newFields2,
    std::vector<std::vector<csl::Expr>> const &rotation2,
    int                                        nMassLessFields)
{
    checksRotation(fields1, newFields1, rotation1);
    checksRotation(fields2, newFields2, rotation2);
    addParticles(newFields1, false);
    addParticles(newFields2, false);

    std::vector<csl::Expr> newExpressions1
        = getRotationTerms(newFields1, rotation1);
    std::vector<csl::Expr> newExpressions2
        = getRotationTerms(newFields2, rotation2);
    std::vector<csl::Expr> fullMassMatrix = getFullMassMatrix(fields1);

    if (fields1.size() >= 3) {
        for (size_t i = 0; i < fields1.size(); ++i) {
            std::cout << "Replacing " << fields1[i]->getName() << " ..."
                      << std::endl;
            applyRotation(fields1[i], newFields1[i], newExpressions1[i]);
            applyRotation(fields2[i], newFields2[i], newExpressions2[i]);
        }
    }
    else {
        applyRotation(fields1, newFields1, newExpressions1);
        applyRotation(fields2, newFields2, newExpressions2);
    }
    removeParticles(fields1);
    removeParticles(fields2);

    bidiagonalizeWithSpectrum(fields1,
                              newFields1,
                              rotation1,
                              fields2,
                              newFields2,
                              rotation2,
                              fullMassMatrix);
    for (int i = 0; i < nMassLessFields; ++i) {
        newFields1[i]->setMass(CSL_0);
        newFields2[i]->setMass(CSL_0);
        clearDependencies(L.mass, [&](Lagrangian::TermType const &t) {
            return t->containsExactly(newFields1[i].get())
                   || t->containsExactly(newFields2[i].get());
        });
    }
}

void ModelBuilder::rotateFields(std::vector<mty::Particle> const &fields,
                                bool diagonalizeMasses,
                                int  nMassLessFields)
{
    std::vector<mty::Particle> newFields(fields.size());
    for (size_t i = 0; i != fields.size(); ++i)
        newFields[i] = fields[i]->generateSimilar(
            std::string(fields[i]->getName()) + " ; "
            + std::string(fields[i]->getLatexName()));
    std::vector<std::vector<csl::Expr>> mixing(fields.size());
    std::string                         baseName = "U";
    std::string                         firstLetters;
    bool                                valid = false;
    for (char c : fields[0]->getName()) {
        if ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z')) {
            firstLetters += c;
            valid = true;
        }
        else if (valid)
            break;
    }
    for (size_t i = 0; i != fields.size(); ++i) {
        mixing[i] = std::vector<csl::Expr>(fields.size());
        for (size_t j = 0; j != fields.size(); ++j) {
            mixing[i][j] = csl::constant_s(baseName + "_" + firstLetters + "_"
                                               + toString(i) + toString(j),
                                           csl::ComplexProperty::Complex);
        }
    }
    rotateFields(
        fields, newFields, mixing, diagonalizeMasses, nMassLessFields);
    for (size_t i = 0; i != newFields.size(); ++i) {
        if (auto fs = newFields[i]->getFieldStrength(); fs) {
            replace(fs, fields[i]->getFieldStrength()->getInstance());
        }
        replace(newFields[i], fields[i]->getInstance());
        removeParticle(newFields[i]);
        if (diagonalizeMasses)
            fields[i]->setMass(newFields[i]->getMass());
    }
}

void ModelBuilder::birotateFields(std::vector<mty::Particle> const &fields1,
                                  std::vector<mty::Particle> const &fields2,
                                  int nMassLessFields)
{
    std::vector<mty::Particle> newFields1(fields1.size());
    std::vector<mty::Particle> newFields2(fields2.size());
    for (size_t i = 0; i != fields1.size(); ++i) {
        newFields1[i] = fields1[i]->generateSimilar(
            std::string(fields1[i]->getName()) + " ; "
            + std::string(fields1[i]->getLatexName()));
        newFields2[i] = fields2[i]->generateSimilar(
            std::string(fields2[i]->getName()) + " ; "
            + std::string(fields2[i]->getLatexName()));
    }
    std::vector<std::vector<csl::Expr>> mixing1(fields1.size());
    std::vector<std::vector<csl::Expr>> mixing2(fields2.size());
    std::string                         baseName1 = "U";
    std::string                         baseName2 = "V";
    std::string                         firstLetters;
    bool                                valid = false;
    for (char c : fields1[0]->getName()) {
        if ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z')) {
            firstLetters += c;
            valid = true;
        }
        else if (valid)
            break;
    }
    for (size_t i = 0; i != fields1.size(); ++i) {
        mixing1[i] = std::vector<csl::Expr>(fields1.size());
        mixing2[i] = std::vector<csl::Expr>(fields2.size());
        for (size_t j = 0; j != fields1.size(); ++j) {
            mixing1[i][j]
                = csl::constant_s(baseName1 + "_" + firstLetters + "_"
                                      + toString(i) + toString(j),
                                  csl::ComplexProperty::Complex);
            mixing2[i][j]
                = csl::constant_s(baseName2 + "_" + firstLetters + "_"
                                      + toString(i) + toString(j),
                                  csl::ComplexProperty::Complex);
        }
    }
    rotateFields(fields1,
                 newFields1,
                 mixing1,
                 fields2,
                 newFields2,
                 mixing2,
                 nMassLessFields);
    applyUnitaryCondition(mixing1);
    applyUnitaryCondition(mixing2);
    for (size_t i = 0; i != newFields1.size(); ++i) {
        if (auto fs = newFields1[i]->getFieldStrength(); fs) {
            replace(fs, fields1[i]->getFieldStrength()->getInstance());
        }
        replace(newFields1[i], fields1[i]->getInstance());
        fields1[i]->setMass(newFields1[i]->getMass());

        if (auto fs = newFields2[i]->getFieldStrength(); fs) {
            replace(fs, fields2[i]->getFieldStrength()->getInstance());
        }
        replace(newFields2[i], fields2[i]->getInstance());
        fields2[i]->setMass(newFields2[i]->getMass());
        removeParticles({newFields1[i], newFields2[i]});
    }
}

void ModelBuilder::rotateFields(
    std::vector<std::string> const            &fields,
    std::vector<std::string> const            &newFields,
    std::vector<std::vector<csl::Expr>> const &rotation,
    bool                                       diagonalizeMasses,
    int                                        nMassLessFields)
{
    if (fields.size() == 0)
        return;
    std::vector<mty::Particle> fields_p;
    fields_p.reserve(fields.size());
    std::vector<mty::Particle> newFields_p;
    newFields_p.reserve(newFields.size());
    for (const auto &name : fields)
        fields_p.push_back(getParticle(name));
    for (const auto &name : newFields)
        newFields_p.push_back(fields_p[0]->generateSimilar(name));

    rotateFields(
        fields_p, newFields_p, rotation, diagonalizeMasses, nMassLessFields);
}

void ModelBuilder::rotateFields(
    std::vector<std::string> const            &fields1,
    std::vector<std::string> const            &newFields1,
    std::vector<std::vector<csl::Expr>> const &rotation1,
    std::vector<std::string> const            &fields2,
    std::vector<std::string> const            &newFields2,
    std::vector<std::vector<csl::Expr>> const &rotation2,
    int                                        nMassLessFields)
{
    if (fields1.size() == 0 or fields2.size() == 0)
        return;
    std::vector<mty::Particle> fields1_p;
    fields1_p.reserve(fields1.size());
    std::vector<mty::Particle> newFields1_p;
    newFields1_p.reserve(newFields1.size());
    for (const auto &name : fields1)
        fields1_p.push_back(getParticle(name));
    for (const auto &name : newFields1)
        newFields1_p.push_back(fields1_p[0]->generateSimilar(name));

    std::vector<mty::Particle> fields2_p(fields2.size());
    std::vector<mty::Particle> newFields2_p(newFields2.size());
    for (const auto &name : fields2)
        fields2_p.push_back(getParticle(name));
    for (const auto &name : newFields2)
        newFields2_p.push_back(fields2_p[0]->generateSimilar(name));

    rotateFields(fields1_p,
                 newFields1_p,
                 rotation1,
                 fields2_p,
                 newFields2_p,
                 rotation2,
                 nMassLessFields);
}

void ModelBuilder::rotateFields(std::vector<std::string> const &fields,
                                bool diagonalizeMasses,
                                int  nMassLessFields)
{
    std::vector<mty::Particle> fields_p;
    fields_p.reserve(fields.size());
    for (const auto &name : fields)
        fields_p.push_back(getParticle(name));

    rotateFields(fields_p, diagonalizeMasses, nMassLessFields);
}

void ModelBuilder::birotateFields(std::vector<std::string> const &fields1,
                                  std::vector<std::string> const &fields2,
                                  int nMassLessFields)
{
    std::vector<mty::Particle> fields1_p;
    fields1_p.reserve(fields1.size());
    for (const auto &name : fields1)
        fields1_p.push_back(getParticle(name));

    std::vector<mty::Particle> fields2_p;
    fields2_p.reserve(fields2.size());
    for (const auto &name : fields2)
        fields2_p.push_back(getParticle(name));

    birotateFields(fields1_p, fields2_p, nMassLessFields);
}

void ModelBuilder::applyUnitaryCondition(
    std::vector<std::vector<csl::Expr>> const &unitary)
{
    auto dependsOnMixing = [&](csl::Expr const &expr) {
        for (const auto &row : unitary)
            for (const auto &el : row)
                if (expr->dependsExplicitlyOn(el.get()))
                    return true;
        return false;
    };
    std::vector<csl::Expr> terms = clearDependencies(
        L.interaction, [&](Lagrangian::TermType const &term) {
            return dependsOnMixing(term->getTerm());
        });
    for (auto &term : terms) {
        bool applied = false;
        for (size_t i = 0; i != unitary.size(); ++i) {
            for (size_t j = 0; j != unitary.size(); ++j) {
                csl::Expr lhs = GetComplexConjugate(unitary[0][i]);
                csl::Expr rhs = CSL_0;
                for (size_t k = 1; k != unitary.size(); ++k)
                    rhs += csl::GetComplexConjugate(unitary[k][i])
                           * unitary[k][j];
                csl::Expr remnant  = (i == j) ? 1 : 0;
                rhs                = (remnant - rhs) / unitary[0][j];
                csl::Expr replaced = csl::Replaced(term, lhs, rhs);
                csl::DeepExpand(replaced);
                csl::DeepHardFactor(replaced);
                if (!dependsOnMixing(replaced)) {
                    applied = true;
                    term    = replaced;
                    break;
                }
            }
            if (applied)
                break;
        }
        L.push_back(term);
    }
    // for (size_t i = 0; i != unitary.size(); ++i)
    //     for (size_t j = 0; j != unitary.size(); ++j) {
    //         csl::Expr expr = CSL_0;
    //         for (size_t k = 0; k != unitary.size(); ++k)
    //             expr +=
    //             csl::GetComplexConjugate(unitary[k][i])*unitary[k][j];
    //         replace(expr, (i == j) ? CSL_1 : CSL_0);
    //         // replace(csl::GetComplexConjugate(expr), (i == j) ? CSL_1 :
    //         CSL_0);
    //     }
}

void ModelBuilder::addParticleFamily(std::vector<mty::Particle> const &family)
{
    for (const auto &oldFamily : particleFamilies) {
        for (const auto &part : family) {
            HEPAssert(std::find(oldFamily.begin(), oldFamily.end(), part)
                          == oldFamily.end(),
                      mty::error::ValueError,
                      "Particle \"" + part->getName()
                          + "\" already belongs to a "
                            "family !")
        }
    }
    particleFamilies.push_back(family);
}

void ModelBuilder::addParticleFamily(std::vector<std::string> const &family)
{
    std::vector<mty::Particle> particles(family.size());
    for (size_t i = 0; i != particles.size(); ++i)
        particles[i] = getParticle(family[i]);
    return addParticleFamily(particles);
}

void ModelBuilder::removeParticleFamily(mty::Particle const &particle)
{
    auto last = std::remove_if(
        particleFamilies.begin(),
        particleFamilies.end(),
        [&](std::vector<mty::Particle> const &family) {
            return std::count(family.begin(), family.end(), particle) > 0;
        });
    particleFamilies.erase(last, particleFamilies.end());
}

void ModelBuilder::removeParticleFamily(std::string const &particle)
{
    return removeParticleFamily(getParticle(particle));
}

void ModelBuilder::saveModel(std::ostream &out, int indentSize)
{
    ModelData::saveModel(out, indentSize);
    writeSpectrum(out, indentSize);
}

///////////////////////////////////////////////////
// Model building utilities
///////////////////////////////////////////////////

void ModelBuilder::doSetGaugeChoice(mty::Particle particle, gauge::Type choice)
{
    particle->setGaugeChoice(choice);
}

void ModelBuilder::doPromoteToGoldstone(mty::Particle &t_goldstone,
                                        mty::Particle &gaugeBoson)
{
    HEPAssert(dynamic_cast<mty::VectorBoson const *>(gaugeBoson.get()),
              mty::error::TypeError,
              "Expecting a vector boson, " + std::string(gaugeBoson->getName())
                  + " given.");
    HEPAssert(gaugeBoson->getGaugeIrrep() == t_goldstone->getGaugeIrrep(),
              mty::error::TypeError,
              "A goldstone boson must have the exact same representation "
              "(here "
                  + toString(t_goldstone->getName()) + " has rep "
                  + toString(t_goldstone->getGaugeIrrep())
                  + ") as "
                    "its corresponding gauge boson (here "
                  + toString(gaugeBoson->getName())
                  + toString(gaugeBoson->getGaugeIrrep()) + ").");
    HEPAssert(t_goldstone->getSpinDimension() == 1,
              mty::error::TypeError,
              "A golstone boson must be of spin 0 ("
                  + toString(t_goldstone->getName()) + ")");

    std::shared_ptr<mty::GoldstoneBoson> goldstone
        = std::make_shared<mty::GoldstoneBoson>(
            std::string(t_goldstone->getName()),
            std::dynamic_pointer_cast<mty::VectorBoson>(gaugeBoson));
    gaugeBoson->setGoldstoneBoson(goldstone);
    addParticle(goldstone, false);
    std::vector<csl::Index> indices = goldstone->getFullSetOfIndices();
    replace(t_goldstone, (*goldstone)(indices));
    removeParticle(t_goldstone);

    csl::Index mu  = MinkowskiIndex();
    auto       _mu = indices;
    _mu.push_back(mu);
    csl::Index nu  = MinkowskiIndex();
    auto       _nu = indices;
    _nu.push_back(nu);

    csl::Tensor X      = L.getPoint();
    csl::Expr   xi     = gaugeBoson->getXiGauge();
    csl::Expr   m      = gaugeBoson->getMass();
    csl::Expr   factor = (gaugeBoson->isSelfConjugate()) ? CSL_HALF : CSL_1;
    addLagrangianTerm(-factor / xi
                      * csl::GetComplexConjugate(mty::partialMinko(+mu, X)
                                                 * gaugeBoson(_mu, X))
                      * (mty::partialMinko(+nu, X) * gaugeBoson(_nu, X)));
    // Done by the addBosonicMass() below
    // addLagrangianTerm(
    //         -factor * m*m*xi
    //         * csl::GetComplexConjugate((*goldstone)(indices, X))
    //         * (*goldstone)(indices, X));
    addLagrangianTerm(
        factor * m * csl::GetComplexConjugate(gaugeBoson(_mu, X))
            * (mty::partialMinko(+mu, X) * (*goldstone)(indices, X)),
        true);
    addBosonicMass(goldstone, m * csl::sqrt_s(xi));
}

void ModelBuilder::doPromoteToGhost(mty::Particle &t_ghost,
                                    mty::Particle &gaugeBoson)
{
    HEPAssert(dynamic_cast<mty::VectorBoson const *>(gaugeBoson.get()),
              mty::error::TypeError,
              "Expecting a vector boson, " + std::string(gaugeBoson->getName())
                  + " given.");
    HEPAssert(dynamic_cast<mty::GhostBoson const *>(t_ghost.get()),
              mty::error::TypeError,
              "Expecting a ghost boson, " + std::string(t_ghost->getName())
                  + " given.");
    HEPAssert(gaugeBoson->getGaugeIrrep() == t_ghost->getGaugeIrrep(),
              mty::error::TypeError,
              "A ghost boson must have the exact same representation "
              "(here "
                  + toString(t_ghost->getName()) + " has rep "
                  + toString(t_ghost->getGaugeIrrep())
                  + ") as "
                    "its corresponding gauge boson (here "
                  + toString(gaugeBoson->getName())
                  + toString(gaugeBoson->getGaugeIrrep()) + ").");

    gaugeBoson->setGhostBoson(t_ghost);
}

bool isProjected(csl::Expr const &prod, csl::Expr const &field)
{
    csl::Index a = field->getIndexStructureView().back();
    for (const auto &arg : prod) {
        if (csl::IsIndicialTensor(arg)) {
            auto const &tensor
                = csl::IsTensorialDerivative(arg) ? arg[0] : arg;
            if (tensor.get() != field.get()) {
                auto parent = tensor->getParent_info();
                if (parent == mty::dirac4.P_L.get()
                    || parent == mty::dirac4.P_R.get()) {
                    return true;
                }
                auto qfield = dynamic_cast<mty::WeylFermion const *>(parent);
                if (qfield)
                    return true;
            }
        }
    }
    return false;
}

bool isMassTerm(csl::Expr const &prod, csl::Expr const &field)
{
    csl::Index a = field->getIndexStructureView().back();
    for (const auto &arg : prod) {
        if (csl::IsIndicialTensor(arg)) {
            auto const &tensor
                = csl::IsTensorialDerivative(arg) ? arg[0] : arg;
            if (tensor->getParent() == field->getParent()) {
                return tensor->getIndexStructureView().back()
                       == field->getIndexStructureView().back();
            }
        }
    }
    return false;
}

auto replaceMajorana(csl::Expr           &expr,
                     mty::Particle const &xi,
                     mty::Particle        Lambda)
{
    HEPAssert(csl::IsProd(expr),
              mty::error::TypeError,
              "Expected a product as interaction term, " + toString(expr)
                  + " given.") const size_t sz
        = expr->size();
    bool left = false;
    auto d    = dirac4.getDelta();
    auto P_1  = dirac4.P_L;
    auto P_2  = dirac4.P_R;
    if (xi->getChirality() == Chirality::Right) {
        std::swap(P_1, P_2);
    }
    auto cc           = csl::GetComplexConjugate;
    auto C            = dirac4.C_matrix;
    bool leftMajorana = false;
    for (size_t i = 0; i != sz; ++i) {
        auto &arg
            = (csl::IsTensorialDerivative(expr[i])) ? expr[i][0] : expr[i];
        if (IsOfType<QuantumField>(arg)) {
            mty::QuantumFieldParent const *qparent
                = ConvertToPtr<QuantumField>(arg)->getQuantumParent();
            if (qparent->isFermionic())
                left = !left;
            if (qparent == xi.get()) {
                auto indices   = arg->getIndexStructureView().getIndex();
                auto a         = indices.back();
                auto b         = a.rename();
                indices.back() = b;
                bool conjug    = arg->isComplexConjugate();
                bool alreadyProjected = isProjected(expr, arg);
                bool massTerm         = isMassTerm(expr, arg);
                bool disableProjector = alreadyProjected || massTerm;
                bool half   = leftMajorana || (massTerm && !alreadyProjected);
                csl::Expr f = (half) ? CSL_HALF : CSL_1;
                if (left && conjug) {
                    const auto P
                        = (disableProjector) ? f * d({b, a}) : P_2({b, a});
                    arg = cc(Lambda(indices)) * P;
                }
                else if (left && !conjug) {
                    auto c         = b.rename();
                    indices.back() = c;
                    const auto P
                        = (disableProjector) ? f * d({c, b}) : P_1({c, b});
                    arg = -cc(Lambda(indices)) * P * C({b, a});
                }
                else if (conjug) {
                    auto c         = b.rename();
                    indices.back() = c;
                    const auto P
                        = (disableProjector) ? f * d({b, c}) : P_2({b, c});
                    arg = -C({a, b}) * P * Lambda(indices);
                }
                else {
                    const auto P
                        = (disableProjector) ? f * d({a, b}) : P_1({a, b});
                    arg = P * Lambda(indices);
                }
            }
        }
    }
}

void ModelBuilder::clearProjectorsInMass()
{
    csl::Tensor            P_L = dirac4.P_L;
    csl::Tensor            P_R = dirac4.P_R;
    std::vector<csl::Expr> terms
        = clearDependencies(L.mass, [&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(P_L.get())
                     || term->getTerm()->dependsExplicitlyOn(P_R.get());
          });
    csl::Index  a  = DiracIndex();
    csl::Index  b  = DiracIndex();
    csl::Tensor id = dirac4.getDelta();
    for (auto &term : terms) {
        L.push_back(
            csl::Replaced(term, P_L({a, b}), id({a, b}) - P_R({a, b})));
    }
}

void ModelBuilder::doPromoteToMajorana(mty::Particle     &particle,
                                       std::string const &newParticleName)
{
    std::shared_ptr<mty::WeylFermion> weylFermion
        = std::dynamic_pointer_cast<mty::WeylFermion>(particle);
    HEPAssert(weylFermion,
              mty::error::TypeError,
              "Expecting a vector boson, "
                  + std::string(weylFermion->getName()) + " given.");
    HEPAssert(weylFermion->isSelfConjugate(),
              mty::error::PhysicsError,
              "Cannot promote a non self-conjugate Weyl fermion to a Majorana "
              "fermion ("
                  + weylFermion->getName() + " given).")

        mty::Particle majorana
        = mty::diracfermion_s(newParticleName.empty() ? weylFermion->getName()
                                                      : newParticleName,
                              weylFermion->getGaugeIrrep(),
                              weylFermion->getFlavorIrrep());
    majorana->setSelfConjugate(true);
    std::vector<csl::Expr> terms
        = clearDependencies([&](Lagrangian::TermType const &term) {
              return term->getTerm()->dependsExplicitlyOn(particle.get());
          });
    for (auto &term : terms) {
        csl::ForEachNode(term, [&](csl::Expr &prod) {
            if (csl::IsProd(prod)) {
                replaceMajorana(prod, particle, majorana);
            }
        });
        L.push_back(csl::DeepRefreshed(term));
    }
    clearProjectorsInMass();
    removeParticle(particle);
    addParticle(majorana, false);
}

void ModelBuilder::findAbreviation(csl::Expr &product)
{
    csl::IndexStructure quantumStructure;
    for (const auto &arg : *product) {
        if (IsOfType<mty::QuantumField>(arg))
            quantumStructure += arg->getIndexStructureView();
        else if (arg->getType() == csl::Type::TDerivativeElement
                 and IsOfType<mty::QuantumField>(arg->getOperand()))
            quantumStructure += arg->getOperand()->getIndexStructureView();
    }
    csl::vector_expr inWilson;
    for (auto &arg : *product) {
        if (IsOfType<mty::QuantumField>(arg))
            continue;
        csl::IndexStructure structure = arg->getIndexStructure();
        bool                connected = false;
        for (csl::Index i1 : structure) {
            for (csl::Index i2 : quantumStructure)
                if (i1 == i2) {
                    connected = true;
                    break;
                }
            if (connected)
                break;
        }
        if (not connected) {
            inWilson.push_back(arg);
            arg = CSL_1;
        }
    }

    if (not inWilson.empty()) {
        product = csl::Abbrev::makeAbbreviation(
                      "C", csl::Refreshed(csl::prod_s(inWilson)))
                  * csl::Refreshed(product);
    }
}

void ModelBuilder::integrateOutParticle(mty::Particle const   &particle,
                                        std::vector<csl::Expr> newInteractions)
{
    replace(particle, CSL_0);
    for (auto &interaction : newInteractions) {
        if (interaction->getType() == csl::Type::Sum) {
            for (auto &term : *interaction) {
                findAbreviation(term);
                L.push_back(term);
            }
        }
        else if (interaction != CSL_0) {
            findAbreviation(interaction);
            L.push_back(interaction);
        }
    }
    L.mergeTerms();
}

void ModelBuilder::diracFermionEmbedding(std::string const &leftName,
                                         std::string const &rightName)
{
    Particle left   = getParticle(leftName);
    Particle right  = getParticle(rightName);
    auto     leftW  = std::dynamic_pointer_cast<WeylFermion>(left);
    auto     rightW = std::dynamic_pointer_cast<WeylFermion>(right);
    HEPAssert(leftW,
              mty::error::TypeError,
              "Particle " + left->getName() + " is not a Weyl fermion !")
        HEPAssert(rightW,
                  mty::error::TypeError,
                  "Particle " + right->getName() + " is not a Weyl fermion !")
            diracFermionEmbedding(leftW, rightW);
}

void ModelBuilder::diracFermionEmbedding(
    std::shared_ptr<WeylFermion> leftWeyl,
    std::shared_ptr<WeylFermion> rightWeyl)
{
    if (leftWeyl->getDiracParent() or rightWeyl->getDiracParent())
        return;
    auto diracFermion = std::dynamic_pointer_cast<DiracFermion>(
        diracfermion_s(leftWeyl, rightWeyl));
    applyDiracFermionEmbedding(diracFermion, leftWeyl, rightWeyl, L.kinetic);
    applyDiracFermionEmbedding(diracFermion, leftWeyl, rightWeyl, L.mass);
    applyDiracFermionEmbedding(
        diracFermion, leftWeyl, rightWeyl, L.interaction);
    addParticle(diracFermion, false);
}

void ModelBuilder::applyDiracFermionEmbedding(
    std::shared_ptr<mty::DiracFermion> const & /*diracFermion*/,
    std::shared_ptr<mty::WeylFermion>       leftWeyl,
    std::shared_ptr<mty::WeylFermion>       rightWeyl,
    std::vector<mty::Lagrangian::TermType> &interaction)
{
    std::vector<csl::Expr> interactionTerms(interaction.size());
    std::vector<csl::Expr> minusInteractionTerms(interaction.size());
    for (size_t i = 0; i != interaction.size(); ++i) {
        interactionTerms[i]      = interaction[i]->getFullExpression();
        minusInteractionTerms[i] = -interactionTerms[i];
    }
    for (size_t i = 0; i != interaction.size(); ++i) {
        if (not interaction[i]->containsExactly(leftWeyl.get())
            and not interaction[i]->containsExactly(rightWeyl.get()))
            continue;
        csl::Expr term = csl::DeepCopy(interactionTerms[i]);
        csl::ForEachLeaf(term, [&](csl::Expr &expr) {
            if (IsOfType<mty::QuantumField>(expr)) {
                mty::QuantumField *field
                    = ConvertToPtr<mty::QuantumField>(expr);
                if (field->getParent_info() != leftWeyl.get()
                    and field->getParent_info() != rightWeyl.get())
                    return;
                field->setChirality(!field->getChirality());
            }
        });
        if (term == CSL_0)
            continue;
        for (size_t j = i + 1; j < interaction.size(); ++j) {
            if (interactionTerms[j] == term) {
                csl::ForEachNode(term, [&](csl::Expr &expr) {
                    if (IsOfType<mty::QuantumField>(expr)) {
                        mty::QuantumField *field
                            = ConvertToPtr<mty::QuantumField>(expr);
                        if (field->getParent_info() != leftWeyl.get()
                            and field->getParent_info() != rightWeyl.get())
                            return;
                        field->setChirality(Chirality::None);
                    }
                });
                std::vector<mty::Lagrangian::TermType> terms
                    = mty::InteractionTerm::createAndDispatch(term);
                HEPAssert(terms.size() == 1,
                          mty::error::RuntimeError,
                          "Got wrong number of interaction terms from "
                              + toString(term) + " (should be 1).");
                interaction[i]      = std::move(terms[0]);
                interactionTerms[i] = interaction[i]->getTerm();
                interaction.erase(interaction.begin() + j);
                interactionTerms.erase(interactionTerms.begin() + j);
                minusInteractionTerms.erase(minusInteractionTerms.begin() + j);
                break;
            }
            else if (minusInteractionTerms[j] == term) {
                csl::Transform(term, [&](csl::Expr &expr) {
                    if (IsOfType<mty::QuantumField>(expr)) {
                        mty::QuantumField *field
                            = ConvertToPtr<mty::QuantumField>(expr);
                        if (field->isBosonic())
                            return false;
                        if (field->getParent_info() != leftWeyl.get()
                            and field->getParent_info() != rightWeyl.get())
                            return false;
                        // Chiralities have been flipped, so Right-handed
                        // is indeed to one that has a minus sign.
                        // gamma5 = PR - PL, with flipped chiralities L<->R
                        csl::Expr factor
                            = (field->getChirality() == Chirality::Right)
                                  ? CSL_M_1
                                  : CSL_1;
                        field->setChirality(Chirality::None);
                        if (not expr->isComplexConjugate()) {
                            auto &structure  = field->getIndexStructureView();
                            csl::Index alpha = structure[structure.size() - 1];
                            csl::Index beta  = dirac4.generateIndex();
                            structure[structure.size() - 1] = beta;
                            expr = factor * dirac4.gamma_chir({alpha, beta})
                                   * expr;
                        }
                        return true;
                    }
                    return false;
                });
                std::vector<mty::Lagrangian::TermType> terms
                    = mty::InteractionTerm::createAndDispatch(term);
                HEPAssert(terms.size() == 1,
                          mty::error::RuntimeError,
                          "Got wrong number of interaction terms from "
                              + toString(term) + " (should be 1).");
                interaction[i]      = std::move(terms[0]);
                interactionTerms[i] = interaction[i]->getTerm();
                interaction.erase(interaction.begin() + j);
                interactionTerms.erase(interactionTerms.begin() + j);
                minusInteractionTerms.erase(minusInteractionTerms.begin() + j);
                break;
            }
        }
    }
}

void ModelBuilder::gatherMass(std::string const &name)
{
    Particle part = getParticle(name);
    gatherMass(part);
}

void ModelBuilder::gatherMass(Particle const &part)
{
    std::vector<size_t> massTerms;
    bool                sameContent = true;
    for (size_t i = 0; i != L.mass.size(); ++i)
        if (L.mass[i]->containsExactly(part.get())
            and isValidMassTerm(*L.mass[i])) {
            for (size_t pos : massTerms)
                if (sameContent and !L.mass[pos]->hasSameContent(*L.mass[i]))
                    sameContent = false;
            massTerms.push_back(i);
        }
    if (massTerms.empty() or massTerms.size() > 2) {
        if (part->getParticleType() != ParticleType::WeylFermion
            or !part->getDiracParent()) {
            part->setMass(CSL_0);
            if (massTerms.size() > 2)
                std::cerr << "Warning: mixings in mass terms for "
                          << part->getName() << " not taken into account.\n";
        }
    }
    else {
        for (size_t i = 1; i != massTerms.size(); ++i)
            if (L.mass[massTerms[0]]->getContent().size()
                < L.mass[massTerms[i]]->getContent().size()) {
                std::swap(massTerms[0], massTerms[i]);
            }
        csl::Expr mass = L.mass[massTerms[0]]->getMass();
        if (!mass->isBuildingBlock()) {
            std::string name = "m_" + std::string(part->getName());
            csl::Expr   abbreviatedMass
                = csl::Abbrev::makeAbbreviation(name, mass);
            mass = csl::constant_s(name);
            addAbbreviatedMassExpression(abbreviatedMass);
        }
        part->setMass(mass);
        std::vector<mty::QuantumField> content
            = L.mass[massTerms[0]]->getContent();
        HEPAssert(content.size() == 2,
                  mty::error::TypeError,
                  "Mass term with more or less than two fields encountered: "
                      + toString(L.mass[massTerms[0]]->getTerm()));
        if (content[0].getParent() == content[1].getParent())
            content.erase(content.begin() + 1);
        if (content.size() == 1) {
            if (!sameContent)
                return;
            part->setMass(mass);
            auto newMassTerms = mty::InteractionTerm::createAndDispatch(
                MassTerm(mass, part.get()));
            HEPAssert(massTerms.size() >= newMassTerms.size(),
                      mty::error::RuntimeError,
                      "Invalid mass term encountered for "
                          + std::string(part->getName())
                          + ".") for (size_t i = 0; i != massTerms.size(); ++i)
            {
                if (i < newMassTerms.size())
                    L.mass[massTerms[i]] = std::move(newMassTerms[i]);
                else
                    L.mass.erase(L.mass.begin() + massTerms[i]
                                 - (i - newMassTerms.size()));
            }
        }
        else if (auto const &pointed = *content[0].getParent();
                 typeid(WeylFermion) == typeid(pointed)) {
            bool massTerm = (massTerms.size() == 2);
            for (size_t i = 0; i != L.mass.size(); ++i) {
                auto pos = std::find(massTerms.begin(), massTerms.end(), i);
                if (pos == massTerms.end()) {
                    if (L.mass[i]->containsWeakly(
                            content[0].getQuantumParent())
                        || L.mass[i]->containsWeakly(
                            content[1].getQuantumParent())) {
                        massTerm = false;
                        break;
                    }
                }
            }
            if (!massTerm) {
                std::cerr << "Warning: mixings in mass terms for "
                          << part->getName() << " not taken into account.\n";
                return;
            }
            for (auto &c : content)
                c.getQuantumParent()->setMass(mass);
            if (content[1].isComplexConjugate())
                std::swap(content[0], content[1]);
            content[0] = *std::dynamic_pointer_cast<mty::QuantumField>(
                csl::GetComplexConjugate(content[0].copy()));
            std::cout << "Dirac fermion embedding for " << content[0]
                      << " and " << content[1] << std::endl;
            diracFermionEmbedding(std::dynamic_pointer_cast<mty::WeylFermion>(
                                      content[0].getParent()),
                                  std::dynamic_pointer_cast<mty::WeylFermion>(
                                      content[1].getParent()));
        }
    }
}

void ModelBuilder::gatherMasses()
{
    const size_t size = particles.size();
    for (size_t i = 0; i != size; ++i) {
        gatherMass(particles[i]);
    }
}

void ModelBuilder::refresh()
{
    L.mergeTerms();
    for (size_t i = 0; i != particles.size(); ++i) {
        if (IsOfType<FieldStrength>(particles[i])) {
            particles.erase(particles.begin() + i);
            --i;
        }
        else {
            bool found = false;
            for (const auto &k : L.kinetic)
                if (k->contains(particles[i].get())) {
                    found = true;
                    break;
                }
            for (const auto &m : L.mass)
                if (m->contains(particles[i].get())) {
                    found = true;
                    break;
                }
            for (const auto &in : L.interaction)
                if (in->contains(particles[i].get())) {
                    found = true;
                    break;
                }
            if (!found) {
                particles.erase(particles.begin() + i);
                --i;
            }
        }
    }
    gatherMasses();
}

///////////////////////////////////////////////////
// Symmetry breaking utilities
///////////////////////////////////////////////////

void ModelBuilder::breakU1GaugeSymmetry(std::string const &groupName)
{
    Group *brokenGroup = getGroup(groupName);
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i] == brokenGroup)
            gauge->getGaugedGroup(i)->setBroken(true);
}

void ModelBuilder::breakGaugeSymmetry(std::string const &brokenGroup)
{
    std::vector<mty::Particle> brokenFields;
    auto                       group = getGroup(brokenGroup);
    for (const auto &part : particles)
        if (getGroupIrrep(part, group) != group->getTrivialRep()
            and group->getType() != group::Type::U1)
            brokenFields.push_back(part);
    breakGaugeSymmetry(brokenGroup, brokenFields);
}

void ModelBuilder::breakGaugeSymmetry(
    std::string const                           &brokenGroup,
    std::vector<std::string> const              &brokenFields,
    std::vector<std::vector<std::string>> const &newNames)
{
    std::vector<mty::Particle> particles(brokenFields.size());
    auto                       part = particles.begin();
    for (auto iter = brokenFields.begin(); iter != brokenFields.end();
         ++iter, ++part)
        *part = getParticle(*iter);
    breakGaugeSymmetry(getGroup(brokenGroup), particles, newNames);
}

void ModelBuilder::breakGaugeSymmetry(
    std::string const                &brokenGroup,
    std::vector<mty::Particle> const &brokenFields)
{
    std::vector<std::vector<std::string>> newNames(brokenFields.size());
    for (size_t i = 0; i != brokenFields.size(); ++i) {
        const size_t size
            = getGroupIrrep(brokenFields[i], brokenGroup).getDim();
        newNames[i] = std::vector<std::string>(size);
        for (size_t j = 0; j != size; ++j) {
            newNames[i][j] = brokenFields[i]->getName();
            newNames[i][j] += "_" + toString(j + 1);
        }
    }
    breakGaugeSymmetry(brokenGroup, brokenFields, newNames);
}

void ModelBuilder::breakGaugeSymmetry(
    std::string const              &brokenGroup,
    std::vector<std::string> const &brokenFields)
{
    std::vector<mty::Particle> parts;
    parts.reserve(brokenFields.size());
    for (const auto &name : brokenFields)
        parts.push_back(getParticle(name));
    breakGaugeSymmetry(brokenGroup, parts);
}

void ModelBuilder::breakGaugeSymmetry(
    std::string const                           &brokenGroup,
    std::vector<mty::Particle> const            &brokenFields,
    std::vector<std::vector<std::string>> const &newNames)
{
    breakGaugeSymmetry(getGroup(brokenGroup), brokenFields, newNames);
}

void ModelBuilder::breakGaugeSymmetry(
    Group                                       *brokenGroup,
    std::vector<mty::Particle> const            &brokenFields,
    std::vector<std::vector<std::string>> const &newNames)
{
    for (const auto &p : brokenFields)
        HEPAssert(p->getGroupIrrep(brokenGroup).getDim() > 1,
                  mty::error::ValueError,
                  "Field " + toString(p->getName())
                      + " has no representation "
                        "for group "
                      + toString(brokenGroup->getName())
                      + " and can "
                        "thus not be broken !") for (size_t i = 0;
                                                     i != brokenFields.size();
                                                     ++i)
        {
            brokenFields[i]->breakParticle(brokenGroup, newNames[i]);
        }
    for (size_t i = 0; i != brokenFields.size(); ++i) {
        csl::Space const *vectorSpace = brokenGroup->getVectorSpace(
            brokenFields[i]->getGroupIrrep(brokenGroup));
        breakLagrangian(brokenFields[i], vectorSpace);
        if (brokenFields[i]->getFieldStrength()) {
            auto ff = brokenFields[i]->getFieldStrength();
            breakLagrangian(ff, vectorSpace);
        }
        removeParticle(brokenFields[i]);
    }
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i] == brokenGroup)
            gauge->getGaugedGroup(i)->setBroken(true);
}

void ModelBuilder::breakFlavorSymmetry(std::string const &brokenGroup)
{
    std::vector<mty::Particle> brokenFields;
    auto                       group = getGroup(brokenGroup);
    for (const auto &part : particles)
        if (getGroupIrrep(part, group) != group->getTrivialRep())
            brokenFields.push_back(part);
    std::vector<std::vector<std::string>> newNames(brokenFields.size());
    for (size_t i = 0; i != brokenFields.size(); ++i) {
        const size_t size
            = getGroupIrrep(brokenFields[i], brokenGroup).getDim();
        newNames[i] = std::vector<std::string>(size);
        for (size_t j = 0; j != size; ++j) {
            newNames[i][j] = brokenFields[i]->getName();
            newNames[i][j] += "_" + toString(j + 1);
        }
    }
    breakFlavorSymmetry(brokenGroup, brokenFields, newNames);
}

void ModelBuilder::breakFlavorSymmetry(std::string const         &brokenGroup,
                                       std::vector<size_t> const &subGroups,
                                       std::vector<std::string>   newFlavors)
{
    if (newFlavors.empty()) {
        newFlavors.reserve(subGroups.size());
        size_t i = 0;
        for (size_t dim : subGroups)
            if (dim > 1)
                newFlavors.push_back(brokenGroup + "_" + toString(++i));
    }
    if (newFlavors.size() == 1)
        newFlavors[0] = brokenGroup;
    std::vector<mty::Particle> brokenFields;
    auto                       group = getGroup(brokenGroup);
    for (const auto &part : particles)
        if (getGroupIrrep(part, group) != group->getTrivialRep())
            brokenFields.push_back(part);
    std::vector<std::vector<std::string>> newNames(brokenFields.size());
    for (size_t i = 0; i != brokenFields.size(); ++i) {
        const size_t size
            = getGroupIrrep(brokenFields[i], brokenGroup).getDim();
        newNames[i] = std::vector<std::string>(size);
        for (size_t j = 0; j != size; ++j) {
            newNames[i][j] = brokenFields[i]->getName();
            newNames[i][j] += "_" + toString(j + 1);
        }
    }
    breakFlavorSymmetry(
        brokenGroup, subGroups, brokenFields, newNames, newFlavors);
}

void ModelBuilder::breakFlavorSymmetry(
    std::string const                           &flavorName,
    std::vector<mty::Particle> const            &brokenFields,
    std::vector<std::vector<std::string>> const &newNames)
{
    if (brokenFields.empty())
        return;
    mty::FlavorGroup *flavor = getFlavorGroup(flavorName);
    size_t            dim
        = flavor->getVectorSpace(brokenFields[0]->getFlavorIrrep(flavor))
              ->getDim();
    breakFlavorSymmetry(flavorName,
                        std::vector<size_t>(dim, 1),
                        brokenFields,
                        newNames,
                        std::vector<std::string>(dim, ""));
}

void ModelBuilder::breakFlavorSymmetry(
    std::string const                           &flavorName,
    std::vector<size_t> const                   &subGroups,
    std::vector<mty::Particle> const            &brokenFields,
    std::vector<std::vector<std::string>> const &newNames,
    std::vector<std::string> const              &newFlavorNames)
{
    size_t dim = std::accumulate(subGroups.begin(), subGroups.end(), 0);
    mty::FlavorGroup *brokenFlavor = nullptr;
    for (const auto &flav : *flavor)
        if (flav->getName() == flavorName) {
            brokenFlavor = flav;
            break;
        }
    HEPAssert(brokenFlavor,
              mty::error::ValueError,
              "Flavor " + flavorName + " not found in model for "
                  + "symmetry breaking.");
    HEPAssert(dim == brokenFlavor->getDim(),
              mty::error::ValueError,
              "Dimension does not match in broken flavor in "
                  + (std::string) "ModelBuilder::breakFlavorSymmetry()");
    for (const auto &p : brokenFields)
        HEPAssert(p->getFlavorIrrep(brokenFlavor).getDim() > 1,
                  mty::error::ValueError,
                  "Field " + toString(p->getName())
                      + " has no representation "
                        "for flavor "
                      + toString(brokenFlavor->getName())
                      + " and can "
                        "thus not be broken !") std::vector<mty::FlavorGroup *>
            newFlavorGroups;
    std::vector<csl::Space const *> newSpaces;
    newFlavorGroups.reserve(subGroups.size());
    newSpaces.reserve(subGroups.size());
    size_t nNonTrivial = 0;
    for (size_t i = 0; i != subGroups.size(); ++i)
        if (subGroups[i] > 1) {
            std::unique_ptr<mty::FlavorGroup> newGroup
                = std::make_unique<mty::FlavorGroup>(
                    newFlavorNames[nNonTrivial++],
                    subGroups[i],
                    brokenFlavor->isComplex());
            newSpaces.push_back(newGroup->getFundamentalSpace());
            newFlavorGroups.push_back(newGroup.get());
            flavor->push_back(std::move(newGroup));
        }
        else {
            newSpaces.push_back(nullptr);
            newFlavorGroups.push_back(nullptr);
        }
    for (auto &part : particles)
        part->adjustFlavorRep(flavor.get());

    for (size_t i = 0; i != brokenFields.size(); ++i) {
        brokenFields[i]->breakParticle(
            brokenFlavor, newFlavorGroups, newNames[i]);
    }
    for (size_t i = 0; i != brokenFields.size(); ++i) {
        breakLagrangian(
            brokenFields[i], brokenFlavor->getFundamentalSpace(), newSpaces);
        if (brokenFields[i]->hasFieldStrength())
            breakLagrangian(brokenFields[i]->getFieldStrength(),
                            brokenFlavor->getFundamentalSpace(),
                            newSpaces);
        removeParticle(brokenFields[i]);
    }
}

void ModelBuilder::replaceTermInLagrangian(
    std::vector<Lagrangian::TermType> &lagrangian,
    size_t                            &i,
    csl::vector_expr                  &newTerms)
{
    std::vector<Lagrangian::TermType> newMass;
    newMass.reserve(newTerms.size());
    for (const auto &term : newTerms) {
        addParticlesIn(term);
        std::vector<Lagrangian::TermType> interactions
            = InteractionTerm::createAndDispatch(term);
        for (const auto &interac : interactions) {
            newMass.push_back(interac);
        }
    }
    lagrangian.erase(lagrangian.begin() + i);
    --i;
    lagrangian.insert(lagrangian.end(), newMass.begin(), newMass.end());
}

void ModelBuilder::breakLagrangian(mty::Particle const &init,
                                   csl::Space const    *brokenSpace)
{
    breakLagrangian(
        init,
        brokenSpace,
        std::vector<csl::Space const *>(brokenSpace->getDim(), nullptr));
}

static std::vector<csl::Expr>
expandBrokenIndices(csl::Expr const                       &init,
                    csl::Space const                      *brokenSpace,
                    std::vector<csl::Space const *> const &newSpace)
{
    // Expanding all broken indices because for now the breakSpace method
    // is only well-defined for expanded expressions.
    csl::Expr interm = csl::DeepExpandedIf(init, [&](csl::Expr const &expr) {
        return csl::AnyOfLeafs(expr, [&](csl::Expr const &sub) {
            if (csl::IsIndicialTensor(sub)) {
                for (const auto &index : sub->getIndexStructureView())
                    if (index.getSpace() == brokenSpace)
                        return true;
            }
            return false;
        });
    });
    return interm->breakSpace(brokenSpace, newSpace);
}

void ModelBuilder::breakLagrangian(
    mty::Particle const                   &init,
    csl::Space const                      *brokenSpace,
    std::vector<csl::Space const *> const &newSpace)
{
    for (size_t i = 0; i != L.kinetic.size(); ++i)
        if (L.kinetic[i]->containsExactly(init.get())) {
            csl::vector_expr broke = expandBrokenIndices(
                L.kinetic[i]->getTerm(), brokenSpace, newSpace);
            replaceTermInLagrangian(L.kinetic, i, broke);
        }

    for (size_t i = 0; i != L.mass.size(); ++i)
        if (L.mass[i]->containsExactly(init.get())) {
            csl::vector_expr broke = expandBrokenIndices(
                L.mass[i]->getTerm(), brokenSpace, newSpace);
            replaceTermInLagrangian(L.mass, i, broke);
        }

    for (size_t i = 0; i != L.interaction.size(); ++i)
        if (L.interaction[i]->containsExactly(init.get())) {
            csl::vector_expr broke = expandBrokenIndices(
                L.interaction[i]->getTerm(), brokenSpace, newSpace);
            replaceTermInLagrangian(L.interaction, i, broke);
        }
}

///////////////////////////////////////////////////
// Diagonalization utilities
///////////////////////////////////////////////////

bool ModelBuilder::isValidMassTerm(mty::InteractionTerm const &term)
{
    csl::Expr factor = csl::DeepCopy(term.getTerm());
    return csl::AllOfNodes(factor, [&](csl::Expr const &sub) {
        return (sub->getType() != csl::Type::TDerivativeElement)
               && (!csl::IsIndicialTensor(sub)
                   || (IsOfType<QuantumField>(sub)
                       || mty::dirac4.isCMatrix(sub)));
    });
}

std::vector<mty::Particle>
ModelBuilder::uniqueContent(InteractionTerm const &term)
{
    auto content = term.getParticles();
    auto last    = std::unique(content.begin(),
                            content.end(),
                            [&](Particle const &A, Particle const &B) {
                                return A.get() == B.get();
                            });
    content.erase(last, content.end());
    return content;
}

std::vector<mty::Particle>::const_iterator
ModelBuilder::findInContent(std::vector<mty::Particle> const &content,
                            mty::Particle const              &field)
{
    return std::find_if(
        content.begin(), content.end(), [&](mty::Particle const &field2) {
            return field.get() == field2.get();
        });
}

bool ModelBuilder::fieldInContent(std::vector<mty::Particle> const &content,
                                  mty::Particle const              &field)
{
    return findInContent(content, field) != content.end();
}

std::vector<MassBlock> ModelBuilder::getMassBlocks() const
{
    std::vector<MassBlock> blocks;
    std::vector<size_t>    indicesLeft;
    indicesLeft.reserve(L.mass.size());
    std::vector<std::vector<Particle>> contents;
    contents.reserve(L.mass.size());
    for (size_t i = 0; i != L.mass.size(); ++i)
        if (isValidMassTerm(*L.mass[i])) {
            indicesLeft.push_back(i);
            contents.push_back(uniqueContent(*L.mass[i]));
        }

    for (size_t i = 0; i < indicesLeft.size(); ++i) {
        if (contents[i].size() == 1)
            continue;
        std::vector<size_t> block(1, indicesLeft[i]);
        for (size_t j = 0; j < indicesLeft.size(); ++j) {
            if (i == j)
                continue;
            bool commonField = false;
            for (const auto &field : contents[i])
                if (fieldInContent(contents[j], field)) {
                    commonField = true;
                    break;
                }
            if (commonField) {
                block.push_back(indicesLeft[j]);
                for (const auto &field : contents[j])
                    if (not fieldInContent(contents[i], field))
                        contents[i].push_back(field);
                indicesLeft.erase(indicesLeft.begin() + j);
                contents.erase(contents.begin() + j);
                if (i > j)
                    --i;
                j = -1;
            }
        }
        if (block.size() > 1) {
            blocks.push_back({block, contents[i], {}});
        }
        contents.erase(contents.begin() + i);
        indicesLeft.erase(indicesLeft.begin() + i);
        --i;
    }
    return blocks;
}

csl::Expr ModelBuilder::getMassMatrixOf(MassBlock const &block)
{
    auto &[pos, content, terms] = block;
    csl::Expr massMatrix        = csl::matrix_s(content.size());
    for (auto &term : terms) {
        std::vector<mty::Particle> termContent = term->getParticles();
        auto last = std::unique(termContent.begin(), termContent.end());
        termContent.erase(last, termContent.end());
        if (termContent.size() == 1) {
            // Diagonal term
            auto posInVector = findInContent(content, termContent[0]);
            int  j           = distance(content.begin(), posInVector);
            massMatrix[j][j] = -term->getGlobalFactor();
        }
        else {
            auto pos1 = findInContent(content, termContent[0]);
            auto pos2 = findInContent(content, termContent[1]);
            int  j1   = distance(content.begin(), pos1);
            int  j2   = distance(content.begin(), pos2);
            massMatrix[j1][j2]
                = massMatrix[j1][j2] - term->getGlobalFactor() / 2;
            massMatrix[j2][j1]
                = massMatrix[j2][j1] - term->getGlobalFactor() / 2;
        }
    }
    return massMatrix;
}

bool ModelBuilder::diagonalizeExplicitely(MassBlock const       &block,
                                          std::vector<Particle> &newFields,
                                          bool                   forceDetZero)
{
    auto &[pos, content, terms] = block;
    csl::Expr massMatrix        = getMassMatrixOf(block);
    csl::Evaluate(massMatrix, csl::eval::abbreviation);
    if (massMatrix->size() != 2) {
        return false;
    }
    if (!forceDetZero) {
        csl::Expr det = massMatrix[0][0] * massMatrix[1][1]
                        - massMatrix[0][1] * massMatrix[1][0];
        csl::DeepExpand(det);
        csl::DeepFactor(det);
        if (det != CSL_0
            and (massMatrix[0][0] != massMatrix[1][1]
                 or massMatrix[0][1] != massMatrix[1][0]))
            return false;
    }
    if (newFields[0]->getSpinDimension() == 3)
        massMatrix = -massMatrix;

    csl::Diagonalizer diagonalizer(massMatrix, forceDetZero);
    diagonalizer.diagonalize();
    csl::Expr transfer = diagonalizer.getInvTransform();
    if (forceDetZero) {
        // If complicated matrix, we abbreviate
        for (size_t i = 0; i != transfer->size(); ++i)
            for (size_t j = 0; j != transfer[i]->size(); ++j)
                transfer[i][j] = transfer[i][j]->getNumericalFactor()
                                 * csl::Abbrev::makeAbbreviation(
                                     csl::GetTerm(transfer[i][j]));
    }
    csl::Expr               newParts = csl::vector_s(newFields.size());
    std::vector<csl::Index> indices  = newFields[0]->getFullSetOfIndices();
    for (size_t i = 0; i != newFields.size(); ++i)
        newParts[i] = newFields[i](indices);

    csl::Expr replacement = transfer->dot(newParts);
    for (size_t i = 0; i != replacement->size(); ++i) {
        replace(content[i], replacement[i]);
    }
    if (newFields[0]->hasFieldStrength()) {
        csl::Expr               newParts = csl::vector_s(newFields.size());
        std::vector<csl::Index> indices
            = newFields[0]->getFieldStrength()->getFullSetOfIndices();
        for (size_t i = 0; i != newFields.size(); ++i)
            newParts[i] = newFields[i]->getFieldStrength()(indices);

        csl::Expr replacement = transfer->dot(newParts);
        for (size_t i = 0; i != replacement->size(); ++i) {
            replace(content[i]->getFieldStrength(), replacement[i]);
        }
    }
    if (newFields[0]->getGhostBoson()) {
        csl::Expr               newParts = csl::vector_s(newFields.size());
        std::vector<csl::Index> indices
            = newFields[0]->getGhostBoson()->getFullSetOfIndices();
        for (size_t i = 0; i != newFields.size(); ++i)
            newParts[i] = newFields[i]->getGhostBoson()(indices);

        csl::Expr replacement = transfer->dot(newParts);
        for (size_t i = 0; i != replacement->size(); ++i) {
            replace(content[i]->getGhostBoson(), replacement[i]);
        }
    }
    clearDependencies(L.mass, [&](Lagrangian::TermType const &term) {
        for (const auto &f : newFields)
            if (term->containsExactly(f.get()))
                return true;
        return false;
    });
    csl::Expr D = diagonalizer.getDiagonal();
    for (size_t i = 0; i != newFields.size(); ++i) {
        if (D[i][i] != CSL_0) {
            csl::Expr mass
                = (newFields[i]->isSelfConjugate()) ? 2 * D[i][i] : D[i][i];
            if (mass == CSL_0)
                continue;
            if (newFields[i]->isBosonic())
                addBosonicMass(newFields[i], csl::sqrt_s(mass));
            else
                addFermionicMass(newFields[i], mass);
        }
    }
    if (forceDetZero) {
        csl::Expr     D = diagonalizer.getDiagonal();
        mty::Particle massLess
            = (D[0][0] == CSL_0) ? newFields[0] : newFields[1];
        for (size_t i = 0; i != L.mass.size(); ++i)
            if (L.mass[i]->containsExactly(massLess.get())) {
                L.mass.erase(L.mass.begin() + i);
                --i;
            }
    }
    for (const auto &p : block.particles)
        removeParticle(p);

    return true;
}

bool ModelBuilder::doDiagonalizeSymbolically(mty::Particle const &field,
                                             bool                 forceDetZero)
{
    if (L.mass.empty())
        return false;
    std::vector<MassBlock> blocks = getMassBlocks();
    MassBlock              massBlock;
    for (auto &block : blocks) {
        if (block.particles.size() == 2) {
            if (block.particles[0] != field and block.particles[1] != field)
                continue;
            massBlock = block;
            massBlock.terms.reserve(massBlock.positions.size());
            for (size_t pos : massBlock.positions)
                massBlock.terms.push_back(L.mass[pos]);
        }
    }
    mty::Particle new1 = massBlock.particles[0].get()->generateSimilar(
        std::string(massBlock.particles[0]->getName()));
    mty::Particle new2 = massBlock.particles[0].get()->generateSimilar(
        std::string(massBlock.particles[1]->getName()));
    std::vector<mty::Particle> newParts = {new1, new2};
    return diagonalizeExplicitely(massBlock, newParts, forceDetZero);
}

void ModelBuilder::diagonalizeMassMatrices()
{
    if (L.mass.empty())
        return;
    L.mergeTerms();
    std::vector<MassBlock> blocks = getMassBlocks();
    for (auto &block : blocks) {
        if (block.particles.size() == 2
            or mty::option::diagonalizeSymbolically) {
            block.terms.reserve(block.positions.size());
            for (size_t pos : block.positions)
                block.terms.push_back(L.mass[pos]);
        }
    }
    for (const auto &block : blocks) {
        if (block.particles.size() == 2
            and block.particles[0]->getSpinDimension() != 2) {
            mty::Particle new1 = block.particles[0].get()->generateSimilar(
                std::string(block.particles[0]->getName()));
            mty::Particle new2 = block.particles[0].get()->generateSimilar(
                std::string(block.particles[1]->getName()));
            std::vector<mty::Particle> newParts = {new1, new2};
            if (diagonalizeExplicitely(block, newParts))
                continue;
        }
        else if (block.particles.size() > 2
                 and mty::option::diagonalizeSymbolically) {
            rotateFields(block.particles, true);
        }
    }
}

void ModelBuilder::diagonalizeYukawa(std::string const &nameYukawa,
                                     std::vector<std::string> const &nameMass,
                                     csl::Expr const                &factor)
{
    csl::vector_expr diagonal(nameMass.size());
    for (size_t i = 0; i != nameMass.size(); ++i) {
        diagonal[i] = factor * csl::constant_s(nameMass[i]);
    }
    std::vector<mty::Particle> empty;
    diagonalizeYukawa(nameYukawa, diagonal, nullptr, empty);
}
void ModelBuilder::diagonalizeYukawa(std::string const &nameYukawa,
                                     std::vector<std::string> const &nameMass,
                                     csl::Expr const                &factor,
                                     csl::Tensor                    &mixing,
                                     std::vector<Particle>           mixed)
{
    csl::vector_expr diagonal(nameMass.size());
    for (size_t i = 0; i != nameMass.size(); ++i) {
        diagonal[i] = factor * csl::constant_s(nameMass[i]);
    }
    diagonalizeYukawa(nameYukawa, diagonal, mixing, mixed);
}
void ModelBuilder::diagonalizeYukawa(std::string const            &nameYukawa,
                                     std::vector<csl::Expr> const &diagonal,
                                     csl::Tensor                   mixing,
                                     std::vector<mty::Particle>    mixed)
{
    csl::Tensor coupling;
    for (const auto &y : tensorCouplings)
        if (y->getName() == nameYukawa) {
            coupling = y;
            break;
        }
    HEPAssert(bool(coupling),
              mty::error::TypeError,
              "Yukawa " + nameYukawa + " not found for diagonalization.");
    csl::Space const *space = coupling->getSpace()[0];
    csl::Tensor newTensor   = csl::tensor_s("M_" + nameYukawa, {space, space});
    newTensor->setFullySymmetric();
    csl::Expr matrix = csl::diagonal_s(diagonal);
    newTensor->setTensor(matrix);

    if (mixing)
        for (auto &field : mixed)
            rotateFields(field, mixing, [&coupling](const csl::Expr &term) {
                return not term->dependsExplicitlyOn(coupling.get());
            });
    replace(coupling, newTensor);
}

void ModelBuilder::addSpectrum(std::vector<mty::Particle> const &parts,
                               std::vector<std::vector<csl::Expr>> const &mass,
                               std::vector<std::vector<csl::Expr>> const &mix,
                               std::vector<std::vector<csl::Expr>> const &mix2)
{
    addParticleFamily(parts);
    spectra.emplace_back(parts, mass, mix, mix2);
}

void ModelBuilder::updateDiagonalizationData()
{
    for (auto &spectrum : spectra)
        spectrum.updateData();
}

void ModelBuilder::applyDiagonalizationData(csl::Expr &expr) const
{
    for (const auto &spectrum : spectra)
        spectrum.applyOn(expr);
}

void ModelBuilder::applyDiagonalizationData(std::vector<csl::Expr> &expr) const
{
    for (auto &ex : expr)
        applyDiagonalizationData(ex);
}

void ModelBuilder::applyDiagonalizationData(csl::LibraryGenerator &lib) const
{
    applyDiagonalizationData(lib, [&](mty::Spectrum const &) { return true; });
}

void ModelBuilder::applyDiagonalizationData(csl::LibraryGenerator &lib,
                                            Amplitude const &amplitude) const
{
    std::vector<std::vector<mty::QuantumField const *>> fields;
    fields.reserve(amplitude.size());
    for (const auto &diag : amplitude.getDiagrams())
        fields.emplace_back(diag.getDiagram()->getFields());
    auto isSpectrumNeeded = [&](Spectrum const &spectrum) {
        for (const auto &p : spectrum.getParticles())
            for (const auto &fieldsDiagram : fields)
                for (const auto &f : fieldsDiagram)
                    if (f->getParent_info() == p.get())
                        return true;
        return false;
    };
    applyDiagonalizationData(lib, isSpectrumNeeded);
}

void ModelBuilder::applyDiagonalizationData(
    csl::LibraryGenerator                            &lib,
    std::function<bool(mty::Spectrum const &)> const &cond) const
{
    for (const auto &s : spectra) {
        if (!cond(s))
            continue;
        std::vector<std::string>           mixings;
        std::vector<std::string>           masses;
        std::vector<std::string>           expressions;
        std::vector<std::string>           dependencies;
        std::vector<mty::Particle> const  &parts    = s.getParticles();
        Spectrum::matrix<csl::Expr> const &initMix  = s.getMixings();
        Spectrum::matrix<csl::Expr> const &initMix2 = s.getMixings2();
        Spectrum::matrix<csl::Expr> const &initMass = s.getMassTerms();
        masses.reserve(parts.size());
        mixings.reserve(initMix.size1() * initMix.size2()
                        + initMix2.size1() * initMix2.size2());
        expressions.reserve(initMass.size1() * initMass.size2());
        for (size_t i = 0; i != initMix.size1(); ++i) {
            const auto &p = parts[i];
            masses.emplace_back(
                csl::LibraryGenerator::regularName(p->getMass()->getName()));
            if (masses.back().empty()) {
                masses.back() = "m_" + p->getName();
            }
        }
        for (size_t i = 0; i != initMix.size1(); ++i)
            for (size_t j = 0; j != initMix.size2(); ++j) {
                mixings.emplace_back(csl::LibraryGenerator::regularName(
                    initMix(i, j)->getName()));
                std::ostringstream sout;
                csl::Expr          massTerm = csl::DeepCopy(initMass(i, j));
                csl::Evaluate(massTerm,
                              csl::eval::numerical | csl::eval::literal);
                csl::ForEachLeaf(massTerm, [&](csl::Expr &sub) {
                    if (sub == CSL_I)
                        sub = csl::LibraryGenerator::imaginary;
                    else if (csl::Size(sub) == 0 and !csl::IsNumerical(sub)) {
                        bool conjug = sub->isComplexConjugate();
                        sub         = csl::constant_s(
                            csl::LibraryGenerator::regularName(sub->getName()),
                            csl::ComplexProperty::Complex);
                        if (conjug)
                            sub = csl::GetComplexConjugate(sub);
                        dependencies.emplace_back(sub->getName());
                    }
                });
                massTerm->print(1, sout, csl::LibraryMode::CppLib);
                expressions.emplace_back(sout.str());
            }
        for (size_t i = 0; i != initMix2.size1(); ++i)
            for (size_t j = 0; j != initMix2.size2(); ++j) {
                mixings.emplace_back(csl::LibraryGenerator::regularName(
                    initMix2(i, j)->getName()));
            }

        lib.addDiagonalization(mixings,
                               masses,
                               expressions,
                               dependencies,
                               parts[0]->getSpinDimension() != 2);
    }
}

void ModelBuilder::writeMatrix(Spectrum::matrix<csl::Expr> const &m,
                               std::ostream                      &out,
                               std::string const                 &indent) const
{
    out << indent << "{\n";
    for (size_t i1 = 0; i1 != m.size1(); ++i1) {
        out << indent << "{";
        for (size_t i2 = 0; i2 != m.size2(); ++i2) {
            m(i1, i2)->printCode(1, out);
            if (i2 + 1 != m.size2())
                out << ", ";
        }
        out << "},\n";
    }
    out << indent << "}";
}

void ModelBuilder::writeSpectrum(std::ostream &out, int indentSize) const
{
    std::string indent  = std::string(indentSize, ' ');
    std::string indent2 = indent + indent;
    for (const auto &s : spectra) {
        std::vector<mty::Particle> const  &parts    = s.getParticles();
        Spectrum::matrix<csl::Expr> const &initMix  = s.getMixings();
        Spectrum::matrix<csl::Expr> const &initMix2 = s.getMixings2();
        Spectrum::matrix<csl::Expr> const &initMass = s.getMassTerms();

        out << indent << "model->addSpectrum(\n";
        out << indent2 << "{";
        for (const auto &p : parts)
            out << csl::Abstract::regularName(p->getName()) << ", ";
        out << "},\n";
        writeMatrix(initMass, out, indent2);
        out << ",\n";
        writeMatrix(initMix, out, indent2);
        if (initMix2.size1() > 0) {
            out << ",\n";
            writeMatrix(initMix2, out, indent2);
        }
        out << '\n';
        out << indent << ");\n";
    }
}

void ModelBuilder::addMassAbbreviations(csl::LibraryGenerator &lib)
{
    for (const auto &p : particles) {
        auto m = p->getMass();
        if (auto parent = csl::Abbrev::find_opt(m->getName()); parent) {
            csl::Expr encaps = parent->getEncapsulated();
            lib.addFunction(
                csl::LibraryGenerator::regularName(m->getName()), encaps, "");
        }
    }
}

void abbreviate(csl::Expr &prod)
{
    std::vector<csl::Expr> factors;
    csl::Expr              term = csl::DeepCopy(prod);
    for (size_t i = 0; i != term->size(); ++i) {
        if (term[i]->isIndexed())
            break;
        if (csl::AnyOfLeafs(term, [&](csl::Expr const &sub) {
                if (IsOfType<QuantumField>(sub))
                    return true;
                return false;
            }))
            break;
        factors.push_back(term[i]);
        term[i] = CSL_1;
    }
    csl::Expr factor = csl::prod_s(factors, true);
    factor           = factor->getNumericalFactor()
             * csl::Abbrev::makeAbbreviation(csl::GetTerm(factor));
    csl::Expr newTerm = factor * csl::Refreshed(term);
    prod              = newTerm;
}

void ModelBuilder::abbreviateBigTerms(size_t maxLeafs)
{
    std::cout << "Abbreviation complex interaction terms ..." << std::endl;
    csl::ProgressBar bar(L.interaction.size());
    for (size_t i = 0; i != L.interaction.size(); ++i) {
        bar.progress(i);
        csl::Expr term = L.interaction[i]->getTerm();
        if (csl::CountLeafs(term) < maxLeafs) {
            continue;
        }
        if (csl::IsSum(term)) {
            for (size_t i = 0; i != term->size(); ++i)
                abbreviate(term[i]);
            csl::Refresh(term);
        }
        else
            abbreviate(term);
        auto interactions = InteractionTerm::createAndDispatch(term);
        HEPAssert(interactions.size() <= 1,
                  mty::error::RuntimeError,
                  "Should get only one term in abbreviation, "
                      + toString(interactions.size()) + " found.");
        if (interactions.empty()) {
            L.interaction.erase(L.interaction.begin() + i);
            --i;
        }
        else
            L.interaction[i] = interactions[0];
    }
}

///////////////////////////////////////////////////
// Other protected functions
///////////////////////////////////////////////////

std::vector<csl::Expr> ModelBuilder::clearDependencies(
    std::function<bool(Lagrangian::TermType const &)> dependencyFunc)
{
    std::vector<csl::Expr> terms;
    terms.reserve(L.fullSize());
    for (size_t i = 0; i != L.kinetic.size(); ++i)
        if (dependencyFunc(L.kinetic[i])) {
            terms.push_back(L.kinetic[i]->getTerm());
            L.kinetic.erase(L.kinetic.begin() + i);
            --i;
        }
    for (size_t i = 0; i != L.mass.size(); ++i)
        if (dependencyFunc(L.mass[i])) {
            terms.push_back(L.mass[i]->getTerm());
            L.mass.erase(L.mass.begin() + i);
            --i;
        }
    for (size_t i = 0; i != L.interaction.size(); ++i)
        if (dependencyFunc(L.interaction[i])) {
            terms.push_back(L.interaction[i]->getTerm());
            L.interaction.erase(L.interaction.begin() + i);
            --i;
        }

    return terms;
}

std::vector<csl::Expr> ModelBuilder::clearDependencies(
    std::vector<Lagrangian::TermType>                &terms,
    std::function<bool(Lagrangian::TermType const &)> dependencyFunc)
{
    std::vector<csl::Expr> res;
    res.reserve(L.fullSize());
    for (size_t i = 0; i != terms.size(); ++i)
        if (dependencyFunc(terms[i])) {
            res.push_back(terms[i]->getTerm());
            terms.erase(terms.begin() + i);
            --i;
        }

    return res;
}

} // End of namespace mty
