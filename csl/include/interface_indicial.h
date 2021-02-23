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
#ifndef INTERFACE_INDICIAL_H_INCLUDED
#define INTERFACE_INDICIAL_H_INCLUDED

#include <string>
#include <vector>
#include "abstract.h"

namespace csl {

class Tensor;
class Space;
class Index;

void AddProperty(csl::Tensor & tensor,
                 Expr    const& A,
                 Expr    const& B,
                 Expr    const& res);

void AddProperty(csl::Tensor           & tensor,
                 std::vector<Expr> const& product,
                 Expr              const& res);

const csl::Space* GetSpace(csl::Tensor const& tensor,
                           int pos = 0);

csl::Index GetIndex(csl::Tensor const& tensor,
                    int pos = 0);

csl::Index GetIndex(csl::Space const* space);

std::vector<csl::Index> GetIndices(
        size_t            N,
        csl::Space const* space);

csl::Index GetIndex(csl::Space const& space);

std::vector<csl::Index> GetIndices(
        size_t            N,
        csl::Space const& space);

std::vector<csl::Index> GetIndices(
        size_t             N,
        csl::Tensor const &tensor,
        int                pos = 0
        );

std::vector<csl::Index> GetFullSetOfIndicesFor(
        csl::Tensor const& tensor);

Tensor Unitary(std::string const& name,
                csl::Space   const* space);

bool TestIndexSanity(
        Expr const &expr,
        bool        verbose = false
        );

}


#endif
