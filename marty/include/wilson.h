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
#ifndef WILSON_H_INCLUDED
#define WILSON_H_INCLUDED

#include <csl.h>
#include "kinematics.h"
#include "cache.h"
#include "feynOptions.h"

namespace mty {

class Model;
class FermionCurrent;
class QuantumField;

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
                   csl::Expr const& t_factor = CSL_1);
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

    csl::Expr getExpression() const;
    void setExpression(csl::Expr const& t_expression);

    static bool hardComparison(csl::Expr const& A,
                               csl::Expr const& B);

    bool operator< (WilsonOperator const& other) const;
    bool operator==(WilsonOperator const& other) const;

    private:

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

    csl::Expr getExpression() const {
        return csl::prod_s({op.getFactor(), coef.getCoefficient(), op.getOp()});
    }

    friend
    std::ostream& operator<<(std::ostream& out,
                             Wilson const& wil) {
        wil.print(out);
        return out;
    }
};

struct WilsonSet: public std::vector<Wilson> {

    using std::vector<Wilson>::vector;

    WilsonSet(WilsonSet const &) = default;
    WilsonSet(WilsonSet &&) = default;
    WilsonSet &operator=(WilsonSet const &) = default;
    WilsonSet &operator=(WilsonSet &&) = default;

    std::vector<csl::Expr> obtainExpressions() const {
        std::vector<csl::Expr> res(size());
        for (size_t i = 0; i != size(); ++i) {
            res[i] = (*this)[i].getExpression();
        }
        return res;
    }

    void merge(bool sorted = false);
    void sort();
    static void sort(std::vector<Wilson> &wilsons);
    static void mergeSorted(std::vector<Wilson> &wilsons);

    Kinematics  kinematics {};
    FeynOptions options {};
    std::vector<std::shared_ptr<wick::Graph>> graphs;
};

std::vector<Wilson> copyWilsons(std::vector<Wilson> const &wilsons);

inline Cache<
        csl::Expr, // Key type
        csl::Expr  // Result type
        > cachedWilson(
                [](csl::Expr const &A, csl::Expr const &B) {
                    return (A == B) || csl::hardComparison(A, B);
                });

void parseStructures(
        csl::Expr              &arg,
        std::vector<csl::Expr> &inOperator,
        csl::IndexStructure    &quantumStructure
        );

std::vector<csl::Expr> parseStructures(
        csl::Expr& product
        );

std::vector<Wilson> cachedWilsonCalculation(
        csl::Expr const& product,
        csl::Expr const& operatorFactor,
        Wilson           res,
        csl::Expr        op,
        bool             standardBasis,
        bool             recursive
        );

std::vector<Wilson> sglSimplifyForWilson(
        csl::Expr const &op,
        csl::Expr const &operatorFactor,
        Wilson           res,
        bool             standardBasis,
        bool             recursive
        );

std::vector<Wilson> parseExpression(
        csl::Expr const& product,
        csl::Expr const& operatorFactor = CSL_1,
        bool        standardBasis = false,
        bool        recursive = false
        );

std::vector<Wilson> parseSum(
        csl::Expr const& sum,
        csl::Expr const& operatorFactor = CSL_1,
        bool        standardBasis = false,
        bool        recursive = false
        );

void addWilson(
        Wilson        const &wil,
        std::vector<Wilson> &wilsons,
        bool                 merge = true
        );

void addSortedWilson(
        Wilson        const &wil,
        std::vector<Wilson> &wilsons
        );

WilsonSet match(
        std::vector<csl::Expr> &amplitudes,
        csl::Expr        const &operatorFactor,
        bool                    standardBasis = false,
        bool                    squaredAfter = false
        );

}

#endif
