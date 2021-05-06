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

 #include "abstractgammasym.h"
#include "multifunction.h"
#include "exceptions.h"
#include "typecast.h"
#include "operations.h"
#include "cslexpr.h"
#include "gammaindex.h"
#include "metricindex.h"
#include "epsilonindex.h"
#include "commutation.h"
#include "contraction.h"
#include "simplify.h"
#include "debuglog.h"
#include "algo.h"
#include "fierz.h"
#include "indexchain.h"
#include "interface.h"
#include "tensorset.h"
#include "cslconverter.h"

#include "tracer.h"
