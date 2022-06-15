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
#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#include "allocator.h"
#include <memory>
#include <vector>

namespace csl {

class Abstract;
class AbstractParent;
class TensorParent;
class Parent;
class Tensor;
class Expr;
typedef const csl::Abstract *Expr_info;

// typedef std::shared_ptr<csl::Abstract>       Expr;
typedef std::shared_ptr<const csl::Abstract> Expr_c;

typedef std::unique_ptr<csl::Abstract>       unique_Expr;
typedef std::unique_ptr<const csl::Abstract> unique_Expr_c;

typedef std::weak_ptr<csl::Abstract>       weak_Expr;
typedef std::weak_ptr<const csl::Abstract> weak_Expr_c;

typedef const csl::AbstractParent *Parent_info;
// typedef std::shared_ptr<csl::AbstractParent> Parent;
typedef const csl::TensorParent *IParent_info;
// typedef std::shared_ptr<csl::TensorParent> Tensor;

using Vector_alloc = std::allocator<Expr>;
// using vector_expr = std::vector<Expr, Vector_alloc>;
using vector_expr = std::vector<Expr>;

inline Vector_alloc alloc_expr;
} // namespace csl

#endif
