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
 * @version 1.0
 
 * \brief Contains the FeynmanRule object for Feynman rules.
 */
#ifndef FEYNMANRULE_H_INCLUDED
#define FEYNMANRULE_H_INCLUDED

#include <csl.h>
#include "graph.h"
#include "interactionTerm.h"

namespace mty {

class Model;

/**
 * @brief Represents a Feynman rule.
 *
 * @details This object lauches the vertex calculation in its constructor to get
 * the expression of the Feynman rule. It may, mater on, give any kind of 
 * information about the vertex, including of course the fields inside and the
 * expression that will enter amplitude calculations.
 */
class FeynmanRule {

    public:

    using TermType = InteractionTerm::TermType;

    FeynmanRule(mty::Model                     &model,
                TermType                 const &interactionTerm,
                std::vector<csl::Tensor> impulsions);

    FeynmanRule();
    FeynmanRule(FeynmanRule const& other);
    FeynmanRule& operator=(FeynmanRule const& other);
    FeynmanRule(FeynmanRule && other) = default;
    FeynmanRule& operator=(FeynmanRule && other) = default;
    ~FeynmanRule() = default;

    std::vector<QuantumField>& getFieldProduct();
    std::vector<QuantumField> const& getFieldProduct() const;
    csl::Expr getFieldProduct(
            csl::Tensor const& point,
            std::vector<csl::Tensor>::iterator &first,
            std::vector<csl::Tensor>::iterator last);
    TermType& getInteractionTerm();
    TermType const& getInteractionTerm() const;
    std::shared_ptr<wick::Graph>& getDiagram();
    std::shared_ptr<wick::Graph> const& getDiagram() const;
    csl::Expr getExpr() const;
    bool contains(mty::QuantumFieldParent *parent) const;
    size_t count(mty::QuantumFieldParent *parent) const;

    void setFieldProduct(std::vector<QuantumField> const& fieldProduct);
    void setInteractionTerm(TermType const& term);
    void setDiagram(std::shared_ptr<wick::Graph> const& diagram);
    void setExpr(csl::Expr const& expr);

    bool isZero() const;
    bool isEmpty() const;
    bool isSame(FeynmanRule const& other) const;
    size_t getSize() const;

    bool operator==(FeynmanRule const& other) const;
    bool operator!=(FeynmanRule const& other) const;
    bool operator<(FeynmanRule const& other) const;
    bool operator>(FeynmanRule const& other) const;
    bool operator<=(FeynmanRule const& other) const;
    bool operator>=(FeynmanRule const& other) const;

    friend
    std::ostream& operator<<(std::ostream& out,
                             FeynmanRule const& rule);

    private:

    void directCMatrix(csl::Expr &rule);

    private:

    std::vector<QuantumField> fieldProduct;
    TermType term;
    std::shared_ptr<wick::Graph>  diagram;
    csl::Expr            expr;
};

}


#endif
