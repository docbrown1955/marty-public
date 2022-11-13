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

/**
 * @file modelBuilder.h
 * @brief File containing the ModelBuilder class.
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2020-08-14
 */

#ifndef MODEL_BUILDER_H_INCLUDED
#define MODEL_BUILDER_H_INCLUDED

#include "modelData.h"
#include "spectrum.h"

namespace mty {

class Amplitude;

/**
 * @brief Helper struct to store mass block data when diagonalizing.
 */
struct MassBlock {
    std::vector<size_t>               positions;
    std::vector<Particle>             particles;
    std::vector<Lagrangian::TermType> terms;
};

/**
 * @brief Class containing all model building features of MARTY.
 *
 * @details This class inherits ModelData that can store all the data
 * concerning a model. ModelBuilder implements all modifications capabilities
 * of MARTY. Gauge and flavor symmetry breaking, particle replacement /
 * rotation, mass diagonalization, renaming etc. It is further inherited by the
 * Model class that implements all calculation features of MARTY.
 *
 * @sa ModelData, Model
 */
class ModelBuilder : public ModelData {

  public:
    ///////////////////////////////////////////////////
    // Constructors
    ///////////////////////////////////////////////////

  public:
    /**
     * @brief Constructor.
     *
     * @details This constructor is a generic constructor that forwards any
     * argument it gets (can have any number of arguments) to ModelData
     * constructors.
     *
     * @tparam ...Args Any argument types that ModelData takes in constructors.
     * @param ...args Any arguments that ModelData takes in constructors.
     *
     * @sa ModelData
     */
    template <class... Args>
    explicit ModelBuilder(Args &&... args)
        : ModelData(std::forward<Args>(args)...)
    {
    }

    /**
     * @brief Default destructor.
     */
    ~ModelBuilder() override{};

    /**
     * @brief Default move constructor.
     *
     * @param other
     */
    ModelBuilder(ModelBuilder &&other) = default;

    /**
     * @brief Default move assignement operator.
     *
     * @param other
     *
     * @return
     */
    ModelBuilder &operator=(ModelBuilder &&other) = default;

    /**
     * @brief Defauly copy constructor.
     *
     * @param other
     */
    ModelBuilder(ModelBuilder const &other) = delete;

    /**
     * @brief Default copy assignement operator.
     *
     * @param other
     *
     * @return
     */
    ModelBuilder &operator=(ModelBuilder const &other) = delete;

    /**
     * @return #abbreviatedMassExpressions
     */
    std::vector<csl::Expr> const &getAbbreviatedMassExpressions() const
    {
        return abbreviatedMassExpressions;
    }

    csl::Expr abbreviateMassExpression(
        std::string   const &abbrevPrefix,
        mty::Particle const &particle,
        csl::Expr     const &mass);

    csl::Expr abbreviateMassExpression(
        std::string             const &abbrevPrefix,
        mty::QuantumFieldParent const *particle,
        csl::Expr               const &mass);

    ///////////////////////////////////////////////////
    // Replacements
    ///////////////////////////////////////////////////

  public:
    void replace(csl::Expr const &oldExpression,
                 csl::Expr const &newExpression);

    void replace(csl::Tensor &oldExpression, csl::Expr const &newExpression);

    void replace(
        csl::Tensor &                          oldExpression,
        csl::Tensor &                          newExpression,
        std::function<bool(csl::Expr const &)> condition
        = [](csl::Expr const &) { return true; });

    void replace(
        mty::Particle const &                  particle,
        csl::Expr                              newExpression,
        std::function<bool(csl::Expr const &)> condition
        = [](csl::Expr const &) { return true; });

    void rotateFields(
        mty::Particle &                        fields,
        csl::Tensor &                          unitaryMatrix,
        std::function<bool(csl::Expr const &)> condition
        = [](csl::Expr const &) { return true; });

    void rotateFields(
        std::string const &                    fields,
        csl::Tensor &                          unitaryMatrix,
        std::function<bool(csl::Expr const &)> condition
        = [](csl::Expr const &) { return true; })
    {
        auto part = getParticle(fields);
        rotateFields(part, unitaryMatrix, condition);
    }

    void rotateFields(std::vector<mty::Particle> const &         fields,
                      std::vector<mty::Particle> const &         newFields,
                      std::vector<std::vector<csl::Expr>> const &rotation,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    void rotateFields(std::vector<mty::Particle> const &         fields1,
                      std::vector<mty::Particle> const &         newFields1,
                      std::vector<std::vector<csl::Expr>> const &rotation1,
                      std::vector<mty::Particle> const &         fields2,
                      std::vector<mty::Particle> const &         newFields2,
                      std::vector<std::vector<csl::Expr>> const &rotation2,
                      int nMassLessFields = 0);

    void rotateFields(std::vector<mty::Particle> const &fields,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    void birotateFields(std::vector<mty::Particle> const &fields1,
                        std::vector<mty::Particle> const &fields2,
                        int                               nMassLessFields = 0);

    void rotateFields(std::vector<std::string> const &           fields,
                      std::vector<std::string> const &           newFields,
                      std::vector<std::vector<csl::Expr>> const &rotation,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    void rotateFields(std::vector<std::string> const &           fields1,
                      std::vector<std::string> const &           newFields1,
                      std::vector<std::vector<csl::Expr>> const &rotation1,
                      std::vector<std::string> const &           fields2,
                      std::vector<std::string> const &           newFields2,
                      std::vector<std::vector<csl::Expr>> const &rotation2,
                      int nMassLessFields = 0);

    void rotateFields(std::initializer_list<std::string> const & fields,
                      std::initializer_list<std::string> const & newFields,
                      std::vector<std::vector<csl::Expr>> const &rotation,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0)
    {
        return rotateFields(std::vector<std::string>(fields),
                            std::vector<std::string>(newFields),
                            rotation,
                            diagonalizeMasses,
                            nMassLessFields);
    }

    void rotateFields(std::initializer_list<std::string> const & fields1,
                      std::initializer_list<std::string> const & newFields1,
                      std::vector<std::vector<csl::Expr>> const &rotation1,
                      std::initializer_list<std::string> const & fields2,
                      std::initializer_list<std::string> const & newFields2,
                      std::vector<std::vector<csl::Expr>> const &rotation2,
                      int nMassLessFields = 0)
    {
        return rotateFields(std::vector<std::string>(fields1),
                            std::vector<std::string>(newFields1),
                            rotation1,
                            std::vector<std::string>(fields2),
                            std::vector<std::string>(newFields2),
                            rotation2,
                            nMassLessFields);
    }

    void rotateFields(std::vector<std::string> const &fields,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0);

    void birotateFields(std::vector<std::string> const &fields1,
                        std::vector<std::string> const &fields2,
                        int                             nMassLessFields = 0);

    void rotateFields(std::initializer_list<std::string> const &fields,
                      bool diagonalizeMasses = false,
                      int  nMassLessFields   = 0)
    {
        return rotateFields(std::vector<std::string>(fields),
                            diagonalizeMasses,
                            nMassLessFields);
    }

    void birotateFields(std::initializer_list<std::string> const &fields1,
                        std::initializer_list<std::string> const &fields2,
                        int nMassLessFields = 0)
    {
        return birotateFields(std::vector<std::string>(fields1),
                              std::vector<std::string>(fields2),
                              nMassLessFields);
    }

    void
    applyUnitaryCondition(std::vector<std::vector<csl::Expr>> const &unitary);

    void saveModel(std::ostream &out, int indent = 4) override;

    ///////////////////////////////////////////////////
    // Pure model building utilities
    ///////////////////////////////////////////////////

  public:
    template <class GroupType>
    void setGaugeChoice(GroupType &&group, gauge::Type choice);

    template <class FieldType1, class FieldType2>
    void promoteToGoldstone(FieldType1 &&goldstone, FieldType2 &&gaugeBoson);

    template <class FieldType1, class FieldType2>
    void promoteToGhost(FieldType1 &&ghost, FieldType2 &&gaugeBoson);

    template <class FieldType, class... Args>
    void promoteToMajorana(FieldType &&weylFermion, Args &&... args);

    static void findAbreviation(csl::Expr &expr);
    void        integrateOutParticle(mty::Particle const &  particle,
                                     std::vector<csl::Expr> newInteractions);

    void diracFermionEmbedding(std::string const &leftName,
                               std::string const &rightName);

    void diracFermionEmbedding(std::shared_ptr<mty::WeylFermion> leftWeyl,
                               std::shared_ptr<mty::WeylFermion> rightWeyl);
    void applyDiracFermionEmbedding(
        std::shared_ptr<mty::DiracFermion> const &diracFermion,
        std::shared_ptr<mty::WeylFermion>         leftWeyl,
        std::shared_ptr<mty::WeylFermion>         rightWeyl,
        std::vector<mty::Lagrangian::TermType> &  interaction);

    void gatherMass(mty::Particle const &part);
    void gatherMass(std::string const &name);

    void gatherMasses();

    void refresh();

    void breakU1GaugeSymmetry(std::string const &groupName);

    void breakGaugeSymmetry(std::string const &brokenGroup);

    void
    breakGaugeSymmetry(std::string const &               brokenGroup,
                       std::vector<mty::Particle> const &brokenFields,
                       std::vector<std::vector<std::string>> const &newNames);

    void
    breakGaugeSymmetry(std::string const &             brokenGroup,
                       std::vector<std::string> const &brokenFields,
                       std::vector<std::vector<std::string>> const &newNames);

    void breakGaugeSymmetry(std::string const &             brokenGroup,
                            std::vector<std::string> const &brokenFields);

    void breakGaugeSymmetry(std::string const &               brokenGroup,
                            std::vector<mty::Particle> const &brokenFields);

    void
    breakGaugeSymmetry(Group *                           brokenGroup,
                       std::vector<mty::Particle> const &brokenFields,
                       std::vector<std::vector<std::string>> const &newNames);

    void breakFlavorSymmetry(std::string const &nameGroup);

    void breakFlavorSymmetry(std::string const &        flavorName,
                             std::vector<size_t> const &subGroups,
                             std::vector<std::string>   newFlavorNames
                             = std::vector<std::string>());

    void
    breakFlavorSymmetry(std::string const &               flavorName,
                        std::vector<mty::Particle> const &brokenFields,
                        std::vector<std::vector<std::string>> const &newNames);

    void
    breakFlavorSymmetry(std::string const &               flavorName,
                        std::vector<size_t> const &       subGroups,
                        std::vector<mty::Particle> const &brokenFields,
                        std::vector<std::vector<std::string>> const &newNames,
                        std::vector<std::string> const &newFlavorNames);

    void diagonalizeMassMatrices();

    template <class FieldType>
    bool diagonalizeSymbolically(FieldType &&field, bool forceDetZero = false);

    void diagonalizeYukawa(std::string const &             nameYukawa,
                           std::vector<std::string> const &nameMass,
                           csl::Expr const &globalFactor = CSL_1);
    void diagonalizeYukawa(std::string const &             nameYukawa,
                           std::vector<std::string> const &nameMass,
                           csl::Expr const &               globalFactor,
                           csl::Tensor &                   mixing,
                           std::vector<mty::Particle>      mixed);
    void diagonalizeYukawa(std::string const &           nameYukawa,
                           std::vector<csl::Expr> const &diagonal,
                           csl::Tensor                   mixing,
                           std::vector<mty::Particle>    mixed);

    void addSpectrum(std::vector<mty::Particle> const &         particles,
                     std::vector<std::vector<csl::Expr>> const &mass,
                     std::vector<std::vector<csl::Expr>> const &mix,
                     std::vector<std::vector<csl::Expr>> const &mix2
                     = std::vector<std::vector<csl::Expr>>());

    void updateDiagonalizationData();

    void applyDiagonalizationData(csl::Expr &expr) const;
    void applyDiagonalizationData(std::vector<csl::Expr> &expr) const;
    void applyDiagonalizationData(csl::LibraryGenerator &lib) const;
    void applyDiagonalizationData(csl::LibraryGenerator &lib,
                                  mty::Amplitude const & amplitudes) const;
    void applyDiagonalizationData(
        csl::LibraryGenerator &                           lib,
        std::function<bool(mty::Spectrum const &)> const &condition) const;
    void addMassAbbreviations(csl::LibraryGenerator &lib);

    void abbreviateBigTerms(size_t maxLeafs = 30);

    void checksRotation(std::vector<mty::Particle> const &         fields,
                        std::vector<mty::Particle> const &         newFields,
                        std::vector<std::vector<csl::Expr>> const &rotation);

    std::vector<csl::Expr> getRotationTerms(
        std::vector<mty::Particle> const &         newFields,
        std::vector<std::vector<csl::Expr>> const &rotation) const;

    std::vector<csl::Expr>
    getFullMassMatrix(std::vector<mty::Particle> const &fields) const;

    void fillDependenciesForRotation(std::vector<csl::Expr> &kinetic,
                                     std::vector<csl::Expr> &interaction,
                                     mty::Particle const &   field);

    void fillDependenciesForRotation(std::vector<csl::Expr> &kinetic,
                                     std::vector<csl::Expr> &interaction,
                                     std::vector<mty::Particle> const &fields);

    void applyRotation(mty::Particle const &field,
                       mty::Particle const &newField,
                       csl::Expr const &    rotation);

    void applyRotation(std::vector<mty::Particle> const &fields,
                       std::vector<mty::Particle> const &newFields,
                       std::vector<csl::Expr> const &    rotations);

    void
    diagonalizeWithSpectrum(std::vector<mty::Particle> const &fields,
                            std::vector<mty::Particle> const &newFields,
                            std::vector<std::vector<csl::Expr>> const &mixing,
                            std::vector<csl::Expr> const &massMatrix);

    void bidiagonalizeWithSpectrum(
        std::vector<mty::Particle> const &         fields1,
        std::vector<mty::Particle> const &         newFields1,
        std::vector<std::vector<csl::Expr>> const &mixing1,
        std::vector<mty::Particle> const &         fields2,
        std::vector<mty::Particle> const &         newFields2,
        std::vector<std::vector<csl::Expr>> const &mixing2,
        std::vector<csl::Expr> const &             massMatrix);

    std::vector<std::vector<mty::Particle>> const &getParticleFamilies() const
    {
        return particleFamilies;
    }

    void addParticleFamily(std::vector<mty::Particle> const &families);
    void removeParticleFamily(mty::Particle const &particle);

    void addParticleFamily(std::vector<std::string> const &familyNames);
    void removeParticleFamily(std::string const &particleName);

  protected:
    void replaceTermInLagrangian(std::vector<Lagrangian::TermType> &lagrangian,
                                 size_t &                           i,
                                 csl::vector_expr &                 newTerms);

    void fillDependencies(
        std::vector<csl::Expr> &                          kinetic,
        std::vector<csl::Expr> &                          mass,
        std::vector<csl::Expr> &                          interaction,
        std::function<bool(Lagrangian::TermType const &)> dependencyFunc);

    std::vector<csl::Expr> clearDependencies(
        std::function<bool(Lagrangian::TermType const &)> dependencyFunc);

    std::vector<csl::Expr> clearDependencies(
        std::vector<Lagrangian::TermType> &               terms,
        std::function<bool(Lagrangian::TermType const &)> dependencyFunc);

    void doSetGaugeChoice(mty::Particle vectorBoson, gauge::Type choice);

    void doPromoteToGoldstone(mty::Particle &goldstone,
                              mty::Particle &gaugeBoson);

    void doPromoteToGhost(mty::Particle &ghost, mty::Particle &gaugeBoson);

    void clearProjectorsInMass();

    void doPromoteToMajorana(mty::Particle &    weylFermion,
                             std::string const &newParticleName = "");

    bool doDiagonalizeSymbolically(mty::Particle const &field,
                                   bool                 forceDetZero = false);

    void breakLagrangian(mty::Particle const &init,
                         csl::Space const *   brokenSpace);
    void breakLagrangian(mty::Particle const &                  init,
                         csl::Space const *                     brokenSpace,
                         std::vector<csl::Space const *> const &newSpace);

    static bool isValidMassTerm(mty::InteractionTerm const &term);

    static std::vector<mty::Particle>
    uniqueContent(mty::InteractionTerm const &term);

    static std::vector<mty::Particle>::const_iterator
                findInContent(std::vector<mty::Particle> const &content,
                              mty::Particle const &             field);
    static bool fieldInContent(std::vector<mty::Particle> const &content,
                               mty::Particle const &             field);

    std::vector<MassBlock> getMassBlocks() const;

    static csl::Expr getMassMatrixOf(MassBlock const &block);

    bool diagonalizeExplicitely(MassBlock const &           block,
                                std::vector<mty::Particle> &newFields,
                                bool forceDetZero = false);

    void writeMatrix(Spectrum::matrix<csl::Expr> const &m,
                     std::ostream &                     out,
                     std::string const &                indent) const;

    void writeSpectrum(std::ostream &out, int indentSize) const;

  protected:
    /**
     * @brief Vector of Spectrum objects, remembering symbolic diagonalizations
     * to, later on, diagonalize numerically mass matrices and update mixing
     * terms.
     */
    std::vector<Spectrum> spectra;

    /**
     * @brief Vector of abbreviated masses in the model, determined by the
     * ModelBuilder::gatherMasses() function and written to libraries by the
     * mty::Library::generateSpectrum() function.
     */
    std::vector<csl::Expr> abbreviatedMassExpressions;

    std::vector<std::vector<mty::Particle>> particleFamilies;
};

///////////////////////////////////////////////////
// Template specializations
///////////////////////////////////////////////////

template <class FieldType>
void ModelBuilder::setGaugeChoice(FieldType &&field, gauge::Type choice)
{
    constexpr bool group_valid
        = std::is_convertible<FieldType, mty::Particle>::value;

    if constexpr (group_valid)
        doSetGaugeChoice(field, choice);
    else
        setGaugeChoice(getParticle(std::forward<FieldType>(field)), choice);
}

template <class FieldType1, class FieldType2>
void ModelBuilder::promoteToGoldstone(FieldType1 &&goldstone,
                                      FieldType2 &&gaugeBoson)
{
    constexpr bool field1_valid
        = std::is_convertible<FieldType1, mty::Particle const &>::value;
    constexpr bool field2_valid
        = std::is_convertible<FieldType2, mty::Particle const &>::value;

    if constexpr (field1_valid) {
        if constexpr (field2_valid)
            return doPromoteToGoldstone(goldstone, gaugeBoson);
        else
            return promoteToGoldstone(
                goldstone, getParticle(std::forward<FieldType2>(gaugeBoson)));
    }
    else if constexpr (field2_valid)
        return promoteToGoldstone(
            getParticle(std::forward<FieldType1>(goldstone)), gaugeBoson);
    else
        return promoteToGoldstone(
            getParticle(std::forward<FieldType1>(goldstone)),
            getParticle(std::forward<FieldType2>(gaugeBoson)));
}

template <class FieldType1, class FieldType2>
void ModelBuilder::promoteToGhost(FieldType1 &&ghost, FieldType2 &&gaugeBoson)
{
    constexpr bool field1_valid
        = std::is_convertible<FieldType1, mty::Particle const &>::value;
    constexpr bool field2_valid
        = std::is_convertible<FieldType2, mty::Particle const &>::value;

    if constexpr (field1_valid) {
        if constexpr (field2_valid)
            return doPromoteToGhost(ghost, gaugeBoson);
        else
            return promoteToGhost(
                ghost, getParticle(std::forward<FieldType2>(gaugeBoson)));
    }
    else if constexpr (field2_valid)
        return promoteToGhost(getParticle(std::forward<FieldType1>(ghost)),
                              gaugeBoson);
    else
        return promoteToGhost(
            getParticle(std::forward<FieldType1>(ghost)),
            getParticle(std::forward<FieldType2>(gaugeBoson)));
}

template <class FieldType, class... Args>
void ModelBuilder::promoteToMajorana(FieldType &&weylFermion, Args &&... args)
{
    constexpr bool field_valid
        = std::is_convertible<FieldType, mty::Particle const &>::value;

    if constexpr (field_valid) {
        return doPromoteToMajorana(weylFermion, std::forward<Args>(args)...);
    }
    else
        return promoteToMajorana(
            getParticle(std::forward<FieldType>(weylFermion)),
            std::forward<Args>(args)...);
}

template <class FieldType>
bool ModelBuilder::diagonalizeSymbolically(FieldType &&field,
                                           bool        forceDetZero)
{
    constexpr bool field_valid
        = std::is_convertible<FieldType, mty::Particle const &>::value;

    if constexpr (field_valid)
        return doDiagonalizeSymbolically(field, forceDetZero);
    else
        return doDiagonalizeSymbolically(
            getParticle(std::forward<FieldType>(field)), forceDetZero);
}

} // End of namespace mty

#endif
