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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * @brief Contains implementations for fermion (Weyl, Dirac, Majorana) for
 * MARTY.
 */
#ifndef FERMIONIC_FIELD_H_INCLUDED
#define FERMIONIC_FIELD_H_INCLUDED

#include "quantumField.h"

namespace mty {

class DiracFermion;
class Model;

/**
 * @brief Weyl fermion, two-component Chiral particle.
 *
 * @details Building a Weyl fermion require to give its chirality (see
 * Chirality) left or right. By default, if not given, chirality will always be
 * set to Chirality::Left.
 */
class WeylFermion : public QuantumFieldParent {

    friend class DiracFermion;

  private:
    /**
     * @brief Chirality of the particle
     */
    Chirality chirality;

    /**
     * @brief Weak pointer to the dirac parent if the fermion is part of a
     * Dirac fermion.
     *
     * @details If the Weyl fermion is alone (left neutrinos in the standard
     * model for example), this pointer is null.
     */
    std::weak_ptr<DiracFermion> diracParent;

  public:
    WeylFermion();

    WeylFermion(WeylFermion const &other) = default;

    WeylFermion(std::weak_ptr<DiracFermion> parent,
                Chirality                   chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                Model const &      t_model,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                Gauge *            t_gauge,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                const GaugeIrrep & irrep,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                Gauge *            t_gauge,
                bool               t_isSelfConjugate,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                const GaugeIrrep & irrep,
                bool               t_isSelfConjugate,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name,
                const GaugeIrrep & irrep,
                const FlavorIrrep &flavorRep,
                Chirality          chirality = Chirality::Left);

    WeylFermion(const std::string &t_name, const WeylFermion *other);

    ~WeylFermion()
    {
    }

    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    ParticleType getParticleType() const override
    {
        return ParticleType::WeylFermion;
    }

    bool isChiral() const override;

    bool isSameSpecies(QuantumFieldParent const *other) const override;

    Chirality getChirality() const override;

    Particle generateSimilar(std::string const &t_name) const override;

    csl::Expr getEnergyDimension() const override;

    int getNDegreesOfFreedom() const override;

    csl::Expr getKineticTerm(csl::Tensor &X) override;

    Particle getChiralityCounterPart() const override;

    Particle getDiracParent() const override;

    void setDiracParent(Particle const &parent) override;

    void breakParticle(mty::Group *                    brokenGroup,
                       std::vector<std::string> const &newNames) override;
    void breakParticle(mty::FlavorGroup *                     brokenFlavor,
                       std::vector<mty::FlavorGroup *> const &subGroups,
                       std::vector<std::string> const &       names) override;

  protected:
    void initPropagator() override;
};

/**
 * @brief Dirac or Majorana fermion in MARTY.
 *
 * @details Majorana fermions are embedded in a Dirac notation, and this class
 * then handles both. By default, a DiracFermion will be a regular 4-component
 * fermion. To build a 2-component Majorana fermion one simply has to set the
 * particle self conjugate by typing for example:
 * \code
 *      Particle maj = mty::diracfermion_s("M", my_model);
 *      maj->setSelfConjugate(true); // dirac to majorana
 * \endcode
 */
class DiracFermion : public QuantumFieldParent {

    friend class WeylFermion;

  private:
    /**
     * @brief Left Weyl sub-part for a Dirac fermion.
     *
     * @details This particle actually also exist for Majorana fermions but
     * should be less interesting for users.
     */
    std::shared_ptr<WeylFermion> leftPart;

    /**
     * @brief Right Weyl sub-part for a Dirac fermion.
     *
     * @details This particle actually also exist for Majorana fermions but
     * should be less interesting for users.
     */
    std::shared_ptr<WeylFermion> rightPart;

  public:
    DiracFermion();

    DiracFermion(DiracFermion const &other) = default;

    DiracFermion(std::shared_ptr<WeylFermion> const &left,
                 std::shared_ptr<WeylFermion> const &right);

    DiracFermion(const std::string &t_name, const Model &model);

    DiracFermion(const std::string &t_name, Gauge *t_gauge);

    DiracFermion(const std::string &t_name, const GaugeIrrep &irrep);

    DiracFermion(const std::string &t_name,
                 Gauge *            t_gauge,
                 bool               t_isSelfConjugate);

    DiracFermion(const std::string &t_name,
                 const GaugeIrrep & irrep,
                 bool               t_isSelfConjugate);

    DiracFermion(const std::string &t_name,
                 const GaugeIrrep & irrep,
                 const FlavorIrrep &flavorRep);

    DiracFermion(const std::string &t_name, const DiracFermion *other);

    ~DiracFermion()
    {
    }

    bool isSameSpecies(QuantumFieldParent const *other) const override
    {
        return this == other || leftPart.get() == other
               || rightPart.get() == other;
    }

    bool contains(QuantumFieldParent const *other) const override
    {
        return isSameSpecies(other);
    }

    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    ParticleType getParticleType() const override
    {
        return ParticleType::DiracFermion;
    }

    void initWeylFermions();

    Particle generateSimilar(std::string const &t_name) const override;

    csl::Expr getEnergyDimension() const override;

    int getNDegreesOfFreedom() const override;

    csl::Expr getKineticTerm(csl::Tensor &X) override;

    Particle getWeylFermion(Chirality chirality) const override;

    void breakParticle(mty::Group *                    brokenGroup,
                       std::vector<std::string> const &newNames) override;
    void breakParticle(mty::FlavorGroup *                     brokenFlavor,
                       std::vector<mty::FlavorGroup *> const &subGroups,
                       std::vector<std::string> const &       names) override;

  private:
    void initPropagator() override;

    void updateChiralBrokenParts(csl::Space const *space);
};

/**
 * @brief Delegates the construction of a Weyl fermion and returns the result.
 *
 * @details This function can take any argument that the constructors
 * WeylFermion::WeylFermion() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created Weyl fermion.
 */
template <class... Args>
inline Particle weylfermion_s(Args &&... args)
{
    auto weyl = csl::make_shared<WeylFermion>(std::forward<Args>(args)...);
    return weyl;
}

/**
 * @brief Delegates the construction of a Dirac fermion and returns the result.
 *
 * @details This function can take any argument that the constructors
 * DiracFermion::DiracFermion() take to forward them to it.
 *
 * @tparam ...Args Variadic template arguments
 * @param ...args  Parameter pack
 *
 * @return The newly created Dirac fermion.
 */
template <class... Args>
inline Particle diracfermion_s(Args &&... args)
{
    auto part = csl::make_shared<DiracFermion>(std::forward<Args>(args)...);
    part->initWeylFermions();
    return part;
}

} // namespace mty

#endif
