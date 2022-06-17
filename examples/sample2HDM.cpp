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

#include <marty.h>
#include <marty/models/2HDM.h>

using namespace mty;

int main()
{

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between
    // 1 and 4 (inclusive)
    // Type 2 is the MSSM-like 2HDM
    ///////////////////////////////////////////////////

    constexpr int type = 2;

    mty::TwoHDM_Model<type> THDM;
    std::cout << THDM << std::endl;

    FeynOptions options;
    options.setTopology(Topology::Box);
    options.addFilter(filter::forceParticleCombination({"W", "t"}));
    options.addFilter(filter::disableParticles({"u_L", "c_L", "H^+", "G^+"}));
    options.setFermionOrder({3, 0, 1, 2});
    mty::option::keepOnlyFirstMassInLoop = true;
    auto ampl                            = THDM.computeAmplitude(OneLoop,
                                      {Incoming("d"),
                                       Incoming(AntiPart("s")),
                                       Outgoing(AntiPart("d")),
                                       Outgoing("s")},
                                      options);
    Display(ampl);
    Show(ampl);

    auto wilsons = THDM.getWilsonCoefficients(ampl);
    Display(wilsons);

    return 0;
}
