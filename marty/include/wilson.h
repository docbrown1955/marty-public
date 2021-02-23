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
 
 * \brief
 */
#ifndef WILSON_H_INCLUDED
#define WILSON_H_INCLUDED

#include <csl.h>

namespace mty {

class Model;
class FermionCurrent;

class WilsonCoefficient {

    public:

    WilsonCoefficient(csl::Expr const& t_coefficient);
    WilsonCoefficient() = default;
    WilsonCoefficient(WilsonCoefficient const&) = default;
    WilsonCoefficient(WilsonCoefficient&&) = default;
    WilsonCoefficient& operator=(WilsonCoefficient const& other) = default;
    WilsonCoefficient& operator=(WilsonCoefficient&& other) = default;
    ~WilsonCoefficient() = default;

    csl::Expr getCoefficient() const;
    void setCoefficient(csl::Expr const& t_coefficient);

    private:

    csl::Expr coefficient;
};

class WilsonOperator {

    public:

    WilsonOperator(csl::Expr const& t_op,
                   csl::Expr const& t_factor = CSL_1,
                   bool        localTerm = false);
    WilsonOperator() = default;
    WilsonOperator(WilsonOperator const&) = default;
    WilsonOperator(WilsonOperator&&) = default;
    WilsonOperator& operator=(WilsonOperator const& other) = default;
    WilsonOperator& operator=(WilsonOperator&& other) = default;
    ~WilsonOperator() = default;

    csl::Expr getOp() const;
    void setOp(csl::Expr const& t_op);

    csl::Expr getFactor() const;
    void setFactor(csl::Expr const& t_factor);

    csl::Tensor getPoint() const;
    void setPoint(csl::Tensor const& Y);

    csl::Expr getExpression() const;
    void setExpression(csl::Expr const& t_expression,
                       bool        localTerm);

    static bool hardComparison(csl::Expr const& A,
                               csl::Expr const& B);

    bool operator==(WilsonOperator const& other) const;

    private:

    csl::Tensor X;
    csl::Expr factor;
    csl::Expr op;
};

struct Wilson {

    WilsonCoefficient coef;
    WilsonOperator    op;

    bool isNull() const {
        return coef.getCoefficient() == CSL_0 
            or op.getExpression()    == CSL_0;
    }

    void print(std::ostream& out = std::cout) const {
        out << "Operator : " << std::endl;
        out << "O = ";
        op.getExpression()->print(0, out);
        out << "C = ";
        coef.getCoefficient()->print(0, out);
    }

    friend
    std::ostream& operator<<(std::ostream& out,
                             Wilson const& wil) {
        wil.print(out);
        return out;
    }
};

std::vector<csl::Expr> parseStructures(
        csl::Expr& product
        );

std::vector<Wilson> parseExpression(
        csl::Expr const& product,
        csl::Expr const& operatorFactor = CSL_1,
        bool        localTerm = false
        );

std::vector<Wilson> parseSum(
        csl::Expr const& sum,
        csl::Expr const& operatorFactor = CSL_1,
        bool        localTerm = false
        );

}

#endif
