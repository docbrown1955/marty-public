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

#ifndef MARTY_VERSION_STR
#define MARTY_VERSION_STR "2.0"
#endif

#include "../../csl/csl.h"
#include "../sgl/sgl.h"
#include "amplitude.h"
#include "amplitudeInitializer.h"
#include "amplitudeSimplification.h"
#include "bernoulli.h"
#include "builtinOperators.h"
#include "ckm.h"
#include "colorSimplification.h"
#include "colorSpace.h"
#include "diracology.h"
#include "drawer.h"
#include "expander.h"
#include "fermionChain.h"
#include "fermionOrder.h"
#include "fermionicField.h"
#include "feynOptions.h"
#include "feynmanDiagram.h"
#include "feynmanIntegral.h"
#include "feynmanRule.h"
#include "feynruleMomentum.h"
#include "filters.h"
#include "gauge.h"
#include "ghostField.h"
#include "goldstoneField.h"
#include "graph.h"
#include "group.h"
#include "groupIndices.h"
#include "insertion.h"
#include "jsonToPhysics.h"
#include "kinematics.h"
#include "lhaBuiltIn.h"
#include "model.h"
#include "modelData.h"
#include "mrtError.h"
#include "mrtInterface.h"
#include "mrtInterfaceCurrent.h"
#include "mrtOptions.h"
#include "mrtUtils.h"
#include "mtylibrary.h"
#include "polarization.h"
#include "propagator.h"
#include "quantumField.h"
#include "quantumFieldTheory.h"
#include "scalarField.h"
#include "topology.h"
#include "traceIdentities.h"
#include "vectorField.h"
#include "wick.h"
#include "wilson.h"
#include "wilsonOperator.h"
