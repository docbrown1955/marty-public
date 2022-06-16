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
#ifndef EQUATION_H_INCLUDED
#define EQUATION_H_INCLUDED

#include "abstract.h"

namespace cslEquation {

enum Type {

    Regular,
    Indicial,
};
}

namespace csl {

std::ostream &operator<<(std::ostream &fout, cslEquation::Type type);

csl::vector_expr listBuildingBlocks(const Expr &expr);

class Equation {

  protected:
    Expr leftHandSide;
    Expr rightHandSide;

    // Contains all concerned building blocks
    csl::vector_expr buildingBlocks;

  protected:
    void searchBuildingBlocks();
    void isolationStep(const Expr &expr);

  public:
    Equation();
    explicit Equation(const Expr &leftHandSide);
    Equation(const Expr &leftHandSide, const Expr &rightHandSide);
    Equation(const Equation &);
    virtual ~Equation();

    cslEquation::Type       getType() const;
    Expr                    getLHS() const;
    Expr                    getRHS() const;
    const csl::vector_expr &getBuildingBlocks() const;

    void setBuildingBlocks(const csl::vector_expr &t_buildingBlocks);

    void replace(const Expr &oldE, const Expr &newE);
    void simplify();
    void makeLHSimple();
    void isolate(const Expr &expr);

    bool operator==(const Equation &eq) const;
    bool operator!=(const Equation &eq) const;

    friend std::ostream &operator<<(std::ostream &fout, const Equation &eq);
};

class IndicialEquation : public Equation {

  public:
    IndicialEquation();
    explicit IndicialEquation(const Expr &leftHandSide);
    IndicialEquation(const Expr &leftHandSide, const Expr &rightHandSide);
    explicit IndicialEquation(const Equation &);
    IndicialEquation(const IndicialEquation &);
    ~IndicialEquation(){};
};

Equation *equation_(const Expr &leftHandSide);
Equation *equation_(const Expr &leftHandSide, const Expr &rightHandSide);
Equation *equation_(const Equation &eq);
Equation *equation_(Equation *eq);
} // End of namespace csl

#endif
