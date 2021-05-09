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
 * @version 1.3
 
 * \brief
 */
#ifndef BUILDINGBLOCK_H_INCLUDED
#define BUILDINGBLOCK_H_INCLUDED

#include "abstract.h"

namespace csl {

/*! 
 * \brief Abstract class from which derive all building blocks of exprs,
 * i.e. objects not function of further exprs: the leafs of the recursive tree. 
 */
class AbstractBuildingBlock: public Abstract {
    public:

    AbstractBuildingBlock();

    bool isBuildingBlock() const override { return true;}

    virtual bool commutesWith(Expr_info expr, int sign=-1) const override;

    std::optional<Expr> expand(bool full=false,
                               bool inplace=false) const override;

    std::optional<Expr> factor(bool full=false) const override;

    std::optional<Expr> factor(Expr_info expr, bool full=false) const override;

    std::optional<Expr> getTerm() const override;

    std::optional<Expr> getRealPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getPolynomialTerm(
            Expr_info expr, int order) const override;

    std::optional<Expr> findSubExpression(
            Expr_info   subExpression,
            const Expr& newExpression) const override;
};

/*************************************************/
// Inline functions (non virtual and short)      //
/*************************************************/
inline AbstractBuildingBlock::AbstractBuildingBlock(): Abstract(){}

class Complexified: public AbstractBuildingBlock {

    public:

    Complexified();

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    bool isComplexConjugate() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexConjugate() const override;

    void setConjugated(bool t_conjugated) override;

    void printProp(std::ostream& fout = std::cout) const override;

    csl::ComplexProperty getComplexProperty() const override;

    void setComplexProperty(csl::ComplexProperty prop) override;

    bool operator==(Expr_info other) const override;

    protected:

    void applyComplexPropertiesOn(const Expr& expr) const;

    void applyComplexPropertiesOn(unique_Expr& expr) const;

    protected:

    bool conjugated = false;
};

} // End of namespace csl

#endif
