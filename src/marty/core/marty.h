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
#include "../../csl/csl.h"
#include "../sgl/sgl.h"
#include "2HDM.h"
#include "CKM.h"
#include "GTHDM.h"
#include "MSSM.h"
#include "NMFV.h"
#include "PMSSM.h"
#include "QED.h"
#include "SM.h"
#include "amplitude.h"
#include "amplitudeInitializer.h"
#include "amplitudeSimplification.h"
#include "bernoulli.h"
#include "builtinOperators.h"
#include "colorSimplification.h"
#include "colorSpace.h"
#include "diracology.h"
#include "doc_brown_link.h"
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
#include "lha.h"
#include "lhaBlocks.h"
#include "lhaBuiltIn.h"
#include "lhaData.h"
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
