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

#ifndef MARTY_API_MODELING_FEYNMANRULES_H
#define MARTY_API_MODELING_FEYNMANRULES_H

#include "../../../csl/api/expr.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace mty::api::modeling {

namespace detail {
struct ModelState;
}

/**
 * @brief Read-only view on the Feynman rules of a model.
 *
 * Nothing is computed when the view is created: the core computes the rules
 * on the first query, and every query runs under a Session.
 */
class FeynmanRules {
  public:
    /// Number of Feynman rules of the model.
    std::size_t size() const;
    /// Printed form of the rule at index (LookupError if out of range).
    std::string str(std::size_t index) const;
    /// LaTeX block of the rule at index (LookupError if out of range).
    std::string latex(std::size_t index) const;
    /// LaTeX document with one block per rule.
    std::string latexDocument() const;
    /// Names of the fields of the vertex at index, in product order.
    std::vector<std::string> vertexParticles(std::size_t index) const;
    /// Expression of the vertex at index (LookupError if out of range).
    csl::Expr vertexExpression(std::size_t index) const;

  private:
    friend class Model;
    explicit FeynmanRules(std::shared_ptr<detail::ModelState> state);

    std::shared_ptr<detail::ModelState> m_state;
};

} // namespace mty::api::modeling

#endif
