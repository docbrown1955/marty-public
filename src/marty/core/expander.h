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
 * @file expander.h
 * @brief Contains the Expander class that expands the Lagrangian in
 * perturbation to find all Feynman diagrams for a given process.
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2021-05-06
 */
#pragma once

#include "feynOptions.h"
#include "insertion.h"
#include "lagrangian.h"

namespace mty {

class Model;
class FeynmanDiagram;

enum Order { TreeLevel, OneLoop };

class Expander {

  public:
    Expander(FeynOptions const                            &t_options,
             std::vector<mty::Lagrangian::TermType> const &lagrangian,
             std::vector<mty::QuantumField> const         &t_insertions);

    std::vector<std::vector<size_t>> findNonZeroDiagrams();

    template <class... Args>
    static std::vector<std::vector<size_t>> getDiagrams(Args &&...args)
    {
        Expander exp(std::forward<Args>(args)...);
        return exp.findNonZeroDiagrams();
    }

  private:
    bool contractionPossible(std::vector<size_t> const       &fieldPos,
                             std::vector<bool> const         &insertionsPaired,
                             std::vector<QuantumField> const &newFields,
                             std::vector<bool>               &newPairing,
                             int                              order,
                             int                              maxOrder,
                             size_t                           nLoops,
                             size_t maxInteractionLegs);

    void addVertexCarefully(size_t                               iTerm,
                            std::vector<size_t> const           &terms,
                            std::vector<std::vector<size_t>>    &numbers,
                            std::vector<std::vector<bool>>      &newPairings,
                            std::vector<bool> const             &pairing,
                            std::map<std::vector<size_t>, bool> &vertexMap);

  private:
    FeynOptions const                     &options;
    std::vector<mty::Lagrangian::TermType> effectiveLagrangian;
    std::vector<mty::QuantumField>         insertions;
};

} // namespace mty
