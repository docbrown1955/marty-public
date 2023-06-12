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

 * \brief
 */
#ifndef COMPLEX_H_INCLUDED
#define COMPLEX_H_INCLUDED

#include "operator.h"
#include "scalarFunc.h"

namespace csl {

class RealPart : public Operator<AbstractFunc> {

  public:
    RealPart();

    RealPart(const Expr &t_argument);

    ~RealPart(){};

    csl::Type getType() const override;

    bool getCommutable() const override
    {
        return true;
    }

    bool operatorAppliesOn(Expr_info arg) const override;

    Expr getOperand() const override;

    void setOperand(const Expr &t_operand) override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printLaTeX(int mode = 0, std::ostream &out = std::cout) const override;

    LibDependency getLibDependency() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getComplexConjugate() const override;

    bool operator==(Expr_info other) const override;
};

class ImaginaryPart : public Operator<AbstractFunc> {

  public:
    ImaginaryPart();

    ImaginaryPart(const Expr &t_argument);

    ~ImaginaryPart(){};

    bool getCommutable() const override
    {
        return true;
    }

    csl::Type getType() const override;

    bool operatorAppliesOn(Expr_info arg) const override;

    Expr getOperand() const override;

    void setOperand(const Expr &t_operand) override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printLaTeX(int mode = 0, std::ostream &out = std::cout) const override;

    LibDependency getLibDependency() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getComplexConjugate() const override;

    bool operator==(Expr_info other) const override;
};

Expr real_s(const Expr &expr);

Expr imaginary_s(const Expr &expr);

Expr cconjugate_(const Expr &expr);
} // End of namespace csl

#endif
