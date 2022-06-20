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
#ifndef HEPOPTIONS_H_INCLUDED
#define HEPOPTIONS_H_INCLUDED

#include "../sgl/sgloptions.h"

namespace mty::option {

///////////////////////////////////////////////////
// General options for computations
///////////////////////////////////////////////////

inline bool showDiagrams         = false;
inline bool amputateExternalLegs = false;

///////////////////////////////////////////////////
// Options on amplitude computation
///////////////////////////////////////////////////

inline bool simplifyAmplitudes        = true;
inline bool orderExternalFermions     = false;
inline bool discardLowerOrders        = true;
inline bool evaluateFermionTraces     = true;
inline bool excludeTadpoles           = true;
inline bool excludeMassCorrections    = false;
inline bool excludeTriangles          = false;
inline bool excludeBoxes              = false;
inline bool excludePentagons          = false;
inline bool computeFirstIntegral      = true;
inline bool dimensionalRegularization = true;
inline bool searchAbreviations        = true;
inline bool expandAbbreviations       = false;
inline bool applyFermionChain         = false;
inline bool abbreviateColorStructures = true;
inline bool decomposeInOperators      = false;
inline bool decomposeInLocalOperator  = true;
inline bool applyEquationsOfMotion    = true;
inline bool addLocalTerms             = true;
inline bool verboseAmplitude          = true;
inline bool testRepresentations       = true;
inline bool keepOnlyFirstMassInLoop   = false;
inline bool useMassiveSimplifications = true;

// This one is a reference ! Only to have this option available in mty::option
inline bool &keepEvanescentOperators = sgl::option::keepEvanescentOperators;

///////////////////////////////////////////////////
// Other options
///////////////////////////////////////////////////

inline bool displayAbbreviations    = true;
inline bool displayIntegralArgs     = false;
inline bool diagonalizeSymbolically = false;

///////////////////////////////////////////////////
// More deep features, the user should probably
// not need them
///////////////////////////////////////////////////

inline bool applyFactorsAndSymmetries = true;
inline bool applyMomentumConservation = true;
inline bool applyInsertionOrdering    = true;
inline bool keepExternScalarLeg       = true;
inline bool enableAntiChiralProps     = true;
} // namespace mty::option

#endif
