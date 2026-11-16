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

/*! \file latexChain.h
 * \brief Printing of contracted vectors and matrices of a product as matrix
 * chains, e.g. y^{T} M x or \mathrm{Tr}\left(M_{1} M_{2}\right).
 */
#ifndef LATEXCHAIN_H_INCLUDED
#define LATEXCHAIN_H_INCLUDED

#include "typedef.h"
#include <string>
#include <vector>

namespace csl {

/*! \brief Implicit matrix chain of a product, e.g. y_i M_{ij} x_j, printed as
 * a matrix product y^{T} M x.
 */
struct LatexMatrixChain {
    std::vector<size_t> factors; //!< Positions of the factors in the product.
    std::string         latex;   //!< LaTeX text of the chain.
};

/*! \brief Finds the matrix chains of a product.
 * \details A chain is a connected set of factors whose matrix-printing
 * indices are contracted with each other: an open chain (two vector ends and
 * matrices in between) or a closed trace (matrices only). Only spaces with
 * matrix printing enabled are considered. A component is returned only if its
 * non-commuting factors can be printed in the order of the product, possibly
 * using transposed factors; otherwise its factors are printed in index
 * notation.
 */
std::vector<LatexMatrixChain> getMatrixChains(vector_expr const &factors);

} // namespace csl

#endif
