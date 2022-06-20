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
 * @file sgloperations.h
 * @brief Contains sgl::Prod and sgl::Sum for SGL.
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2021-05-06
 */
#pragma once

#include "multifunction.h"

namespace sgl {

class Sum : public AbstractMultiFunction {
  public:
    using AbstractMultiFunction::AbstractMultiFunction;

    bool isZero() const override;

    GExpr copy() const override;
    GExpr refresh() const override;

    bool contains(csl::Index const &index) const override;
    void replace(csl::Index const &i, csl::Index const &j) override;

    csl::Expr toCSL(TensorSet const &) const override;

    void merge();
    void print(std::ostream &out) const override;
};

class Prod : public AbstractMultiFunction {
  public:
    using AbstractMultiFunction::AbstractMultiFunction;

    bool isZero() const override;

    csl::Expr getFactor() const override;
    GExpr     getTerm() const override;

    GExpr copy() const override;
    GExpr refresh() const override;

    bool contains(csl::Index const &index) const override;
    void replace(csl::Index const &i, csl::Index const &j) override;

    csl::Expr toCSL(TensorSet const &) const override;

    void print(std::ostream &out) const override;

    void  merge();
    void  applyProperties();
    GExpr expand();
};

GExpr sum_s(std::vector<GExpr> args);
GExpr prod_s(std::vector<GExpr> args);
} // namespace sgl
