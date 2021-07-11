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
#include "csl.h"
#include "sgl.h"
#include "graph.h"
#include "drawer.h"
#include "amplitude.h"
#include "amplitudeInitializer.h"
#include "amplitudeSimplification.h"
#include "topology.h"
#include "kinematics.h"
#include "modelData.h"
#include "model.h"
#include "quantumField.h"
#include "polarization.h"
#include "quantumFieldTheory.h"
#include "scalarField.h"
#include "fermionicField.h"
#include "vectorField.h"
#include "goldstoneField.h"
#include "ghostField.h"
#include "mrtInterface.h"
#include "mrtInterfaceCurrent.h"
#include "mrtOptions.h"
#include "mrtError.h"
#include "diracology.h"
#include "CKM.h"
#include "colorSpace.h"
#include "insertion.h"
#include "expander.h"
#include "gauge.h"
#include "group.h"
#include "jsonToPhysics.h"
#include "flha.h"
#include "flhaReader.h"
#include "mtylibrary.h"
#include "lhaData.h"
#include "lha.h"
#include "lhaBlocks.h"
#include "lhaBuiltIn.h"
#include "model.h"
#include "QED.h"
#include "SM.h"
#include "2HDM.h"
#include "MSSM.h"
#include "PMSSM.h"
#include "NMFV.h"
#include "wilson.h"
#include "wilsonOperator.h"
#include "propagator.h"
#include "mrtUtils.h"
#include "wick.h"
#include "feynOptions.h"
#include "feynruleMomentum.h"
#include "feynmanRule.h"
#include "fermionChain.h"
#include "feynmanIntegral.h"
#include "feynmanDiagram.h"
#include "fermionOrder.h"
#include "filters.h"
#include "bernoulli.h"
#include "groupIndices.h"
#include "traceIdentities.h"
#include "doc_brown_link.h"
