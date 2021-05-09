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
#ifndef LOCALTERMS_H_INCLUDED
#define LOCALTERMS_H_INCLUDED

#include "feynmanIntegral.h"

namespace mty {

inline const csl::Expr FiniteFlag = csl::constant_s("Finite");

csl::Expr getLocalTerm(
        IntegralType               type,
        std::vector<size_t> const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

csl::Expr getLocalATerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

csl::Expr getLocalBTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

csl::Expr getLocalCTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

csl::Expr getLocalDTerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

csl::Expr getLocalETerm(
        std::string       const &indices,
        std::vector<csl::Expr> const &momentum,
        std::vector<csl::Expr> const &masses
        );

} // End of namespace mty

#endif
