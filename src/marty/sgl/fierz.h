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
 * @file fierz.h
 * @brief Utilities to apply general Fierz identities in fermion chains.
 * @author Grégoire Uhlrich
 * @version
 * @date 2021-05-06
 */
#pragma once

namespace sgl {

class GExpr;
class IndexChain;

void fixFierz(GExpr &expr);
bool areCanonical(IndexChain const &A, IndexChain const &B);
void applyGeneralFierz(GExpr &expr, bool twice = false);
void applyGeneralFierzTwice(GExpr &expr);
} // namespace sgl
