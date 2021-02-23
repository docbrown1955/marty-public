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

/*! \file polarization.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Contains PolarizationField, class representing polarization vectors
 * in equations. Allows to compute polarization sums in cross-sections.
 */
#ifndef POLARIZATION_H_INCLUDED
#define POLARIZATION_H_INCLUDED

#include <csl.h>
#include "quantumField.h"

namespace mty {
class QuantumFieldParent;

/*!
 * 
 * \brief Representing polarization vectors in equations. Allows to compute
 * polarization sums in cross-sections. Is derived from TensorFieldElement: possess
 * at least one csl::Index (polarization) and one "space-time point" that is the 
 * impulsion of the particle.
 */
class PolarizationField: public csl::TensorFieldElement {

    public:

    PolarizationField();

    PolarizationField(csl::Tensor const& impulsion,
                      csl::Parent const&  t_parent);

    PolarizationField(csl::Tensor const&            impulsion,
                      csl::Parent const&             t_parent,
                      const std::vector<csl::Index>& indices);

    PolarizationField(csl::Tensor const&        impulsion,
                      csl::Parent const&         t_parent,
                      const csl::IndexStructure& indices);

    PolarizationField(PolarizationField const&) = default;

    ~PolarizationField(){};

    bool getCommutable() const override;

    bool isOnShell() const;

    std::optional<csl::Expr> getComplexConjugate() const override;

    bool hasContractionProperty(csl::Expr_info other) const override;

    csl::Expr contraction(csl::Expr_info other) const override;

    csl::unique_Expr copy_unique() const override;

    void printCode(
            int mode = 0,
            std::ostream &out = std::cout
            ) const override;

    void printProp(std::ostream& out = std::cout) const override;

    /*!
     * \brief Returns a pointer to the QuantumFieldParent of the field.
     * \details As QuantumField inherits from csl::TensorFieldElement, its parent is
     * in the form of a csl::Parent. This function then makes a conversion to 
     * a mty::QuantumFieldParent before returning the pointer.
     * \return A pointer to the QuantumFieldParent of the field.
     */
    inline
    QuantumFieldParent* getQuantumParent() const {
        return static_cast<QuantumFieldParent*>(parent.get());
    }

    /*!
     * \brief Returns a mty::Particle, pointer to the parent of the field.
     * \details As QuantumField inherits from csl::TensorFieldElement, its parent is
     * in the form of a csl::Parent. This function then makes a conversion to 
     * a mty::Particle before returning the pointer.
     * \return A shared pointer to the QuantumFieldParent (i.e. mty::Particle) 
     * of the field.
     */
    inline 
    Particle getParticle() const {
        return std::static_pointer_cast<QuantumFieldParent>(parent);
    }

    bool isConjugationLocked() const {
        return lockConjugation;
    }

    void setConjugationLock(bool t_lock) {
        lockConjugation = t_lock;
    }

    csl::Expr getMass() const;

    void setParticle(bool t_particle);

    void setIncoming(bool t_incoming);

    void setOnShell(bool t_onShell);

    bool isSelfConjugate() const;

    bool isParticle() const {
        return particle;
    }

    bool isAntiParticle() const {
        return !isParticle();
    }

    bool isOutgoing() const {
        return !isIncoming();
    }

    bool isIncoming() const {
        return incoming;
    }

    bool isIncomingParticle() const;

    bool isOutgoingParticle() const;

    bool isIncomingAntiParticle() const;

    bool isOutgoingAntiParticle() const;

    friend
    std::ostream& operator<<(std::ostream& fout, const PolarizationField& pol);

    private:

    bool spaceIndexContraction(csl::Expr_info other) const;
    bool spinIndexContraction(csl::Expr_info other) const;

    bool isContractibleWith(csl::Expr_info otherPolar) const;

    csl::Expr sumPolarization(csl::Expr_info other) const;

    csl::Expr chargeConjugation(csl::Expr_info other) const;

    void updateComplexConjugation();

    private:

    bool particle;

    bool incoming;

    bool onShell;

    bool lockConjugation;
};

} // End of namespace mty

#endif
