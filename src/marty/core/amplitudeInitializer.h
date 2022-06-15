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
 * @file amplitudeInitializer.h
 * @brief Contains utilities to initialize amplitudes and launch
 * simplifications.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include "amplitude.h"
#include "feynOptions.h"
#include "lagrangian.h"
#include <variant>

namespace mty {

class Model;
class Kinematics;

class AmplitudeInitializer {

  public:
    AmplitudeInitializer(mty::Model const *                      t_model,
                         mty::FeynOptions const &                t_options,
                         Kinematics const &                      t_kinematics,
                         std::vector<mty::QuantumField> const &  t_insertions,
                         std::vector<mty::Lagrangian::TermType> &t_lagrangian);

    AmplitudeInitializer(mty::Model const *                     t_model,
                         mty::FeynOptions const &               t_options,
                         Kinematics const &                     t_kinematics,
                         std::vector<mty::QuantumField> const & t_insertions,
                         std::vector<mty::FeynmanRule const *> &t_lagrangian);

    template <class... Args>
    static Amplitude
    getAmplitude(std::vector<std::vector<size_t>> const &terms,
                 Args &&... args)
    {
        AmplitudeInitializer ampl(std::forward<Args>(args)...);
        return ampl.getAmplitude(terms);
    }

    Amplitude getAmplitude(std::vector<std::vector<size_t>> const &terms);

    void initMomentumVertices(std::vector<FeynmanRule> &      localRules,
                              std::map<csl::Tensor, size_t> & vertexIds,
                              std::vector<csl::Tensor> const &vertices,
                              std::vector<csl::Tensor> &      witnessVertices,
                              csl::PseudoIntegral &           integral,
                              std::vector<csl::Expr> &        fieldVertices);

    std::vector<std::vector<size_t>>
    getExternalSymmetries(csl::Expr &fieldProd) const;

    std::vector<csl::Expr>
    applyExternalSymmetries(csl::Expr const &   res,
                            std::vector<size_t> perm) const;

    std::vector<csl::Expr> applyAllExternalSymmetries(
        std::vector<csl::Expr> const &             init,
        std::vector<std::vector<size_t>>::iterator first,
        std::vector<std::vector<size_t>>::iterator last) const;

    void simplifyFullQuantumCalculation(
        mty::FeynmanDiagram &             diagram,
        csl::PseudoIntegral const &       integral,
        std::vector<size_t> const &       posTerms,
        std::vector<std::vector<size_t>> &externalSym);

    void
    simplifyRuledCalculation(mty::FeynmanDiagram &           diagram,
                             csl::PseudoIntegral const &     integral,
                             std::vector<csl::Tensor> const &witnessVericesx,
                             FeynruleMomentum &              momentumMapping);

    static void removeZeroDiagrams(std::vector<FeynmanDiagram> &diagrams);

    static csl::Expr
    getLSZInsertions(std::vector<mty::QuantumField> const &fields,
                     Kinematics &                          kinematics,
                     bool feynRuleCalculation);

    static std::vector<csl::Tensor> getVertices(size_t N);

    /**
     * @brief Order the field insertions (important for fermion ordering),
     * applying the permutation to the associated kinematics.
     *
     * @param insertions Insertions to order.
     * @param kinematics Kinematics that will follow the same order as the
     * insertions.
     */
    static void orderInsertions(std::vector<mty::QuantumField> &insertions,
                                Kinematics &                    kinematics,
                                FeynOptions &                   options);

    static void
    applyMomentumVertices(std::vector<csl::Tensor> const &witnessVertices,
                          FeynruleMomentum &              momentumMapping,
                          csl::Expr &                     amplitude);

  private:
    Amplitude
    fullQuantumCalculation(std::vector<std::vector<size_t>> const &terms);

    Amplitude ruledCalculation(std::vector<std::vector<size_t>> const &terms);

  private:
    inline static std::vector<mty::Lagrangian::TermType> defaultL{};
    inline static std::vector<mty::FeynmanRule const *>  defaultFR{};

    mty::Model const *             model;
    mty::FeynOptions               options;
    std::vector<mty::QuantumField> insertions;
    Kinematics                     kinematics;

    /**
     * @brief True if the calculation a calculation of Feynman rules (using
     * the Lagrangian), false if the calculation uses Feynman rules.
     *
     * @details Depending on the value of this boolean, one of the two
     * references (**lagrangian** or **feynmanRules**) is a reference to an
     * empty container while only the other is used in the calculation.
     */
    bool feynRuleMode;

    /**
     * @brief Reference to the lagrangian term when the calculation does not
     * use Feynman rules, reference to an empty vector otherwise.
     */
    std::vector<mty::Lagrangian::TermType> &lagrangian;

    /**
     * @brief Reference to the Feynman rules when the calculation does
     * use Feynman rules, reference to an empty vector otherwise.
     */
    std::vector<mty::FeynmanRule const *> &feynmanRules;
};

} // namespace mty
