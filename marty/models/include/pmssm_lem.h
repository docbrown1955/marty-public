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
#ifndef DEFINITION_PMSSM_H_INCLUDED
#define DEFINITION_PMSSM_H_INCLUDED

#include "marty.h"
#include "pmssm_data.h"

namespace mty {

class PMSSM_LEM: public mty::Model, public mty::PMSSM_data {

public:

    PMSSM_LEM();
    ~PMSSM_LEM(){};

protected:

    void initContent();
    void initKinetic();
        void initKinetic0();
        void initKinetic1();
        void initKinetic2();
    void initMass();
        void initMass0();
        void initMass1();
    void initInteractions();
        void initInteractions0();
        void initInteractions1();
        void initInteractions2();
        void initInteractions3();
        void initInteractions4();
        void initInteractions5();
        void initInteractions6();
        void initInteractions7();
        void initInteractions8();
        void initInteractions9();
        void initInteractions10();
        void initInteractions11();
        void initInteractions12();
        void initInteractions13();
        void initInteractions14();
        void initInteractions15();
        void initInteractions16();
        void initInteractions17();
        void initInteractions18();
        void initInteractions19();
        void initInteractions20();
        void initInteractions21();
        void initInteractions22();
        void initInteractions23();
        void initInteractions24();
        void initInteractions25();
        void initInteractions26();
        void initInteractions27();
        void initInteractions28();
        void initInteractions29();
        void initInteractions30();
        void initInteractions31();
        void initInteractions32();
        void initInteractions33();
        void initInteractions34();
        void initInteractions35();
        void initInteractions36();
        void initInteractions37();
        void initInteractions38();
        void initInteractions39();
        void initInteractions40();
        void initInteractions41();
        void initInteractions42();
        void initInteractions43();
        void initInteractions44();
        void initInteractions45();
        void initInteractions46();
        void initInteractions47();
        void initInteractions48();
        void initInteractions49();
        void initInteractions50();
        void initInteractions51();
        void initInteractions52();
        void initInteractions53();
        void initInteractions54();
        void initInteractions55();
        void initInteractions56();
        void initInteractions57();
        void initInteractions58();
        void initInteractions59();
        void initInteractions60();
        void initInteractions61();
        void initInteractions62();
        void initInteractions63();
        void initInteractions64();
        void initInteractions65();
        void initInteractions66();
        void initInteractions67();
        void initInteractions68();
        void initInteractions69();
        void initInteractions70();
        void initInteractions71();
        void initInteractions72();
        void initInteractions73();
        void initInteractions74();
        void initInteractions75();
        void initInteractions76();
        void initInteractions77();
        void initInteractions78();
        void initInteractions79();
        void initInteractions80();
        void initInteractions81();
        void initInteractions82();
        void initInteractions83();
        void initInteractions84();
        void initInteractions85();
        void initInteractions86();
        void initInteractions87();
        void initInteractions88();
        void initInteractions89();
        void initInteractions90();
        void initInteractions91();
        void initInteractions92();
        void initInteractions93();
        void initInteractions94();
        void initInteractions95();
        void initInteractions96();
        void initInteractions97();
        void initInteractions98();
        void initInteractions99();
        void initInteractions100();
        void initInteractions101();
        void initInteractions102();
        void initInteractions103();
        void initInteractions104();
        void initInteractions105();
        void initInteractions106();
        void initInteractions107();
        void initInteractions108();
        void initInteractions109();
        void initInteractions110();
        void initInteractions111();
        void initInteractions112();
        void initInteractions113();
        void initInteractions114();
        void initInteractions115();
        void initInteractions116();
        void initInteractions117();
        void initInteractions118();
        void initInteractions119();
        void initInteractions120();
        void initInteractions121();
        void initInteractions122();
        void initInteractions123();
        void initInteractions124();
        void initInteractions125();
        void initInteractions126();
        void initInteractions127();
        void initInteractions128();
        void initInteractions129();
        void initInteractions130();
        void initInteractions131();
        void initInteractions132();
        void initInteractions133();
        void initInteractions134();
        void initInteractions135();
        void initInteractions136();
        void initInteractions137();
        void initInteractions138();
        void initInteractions139();
        void initInteractions140();
        void initInteractions141();
        void initInteractions142();
        void initInteractions143();
        void initInteractions144();
        void initInteractions145();
        void initInteractions146();
        void initInteractions147();
        void initInteractions148();
        void initInteractions149();
        void initInteractions150();
        void initInteractions151();
        void initInteractions152();
        void initInteractions153();
        void initInteractions154();
        void initInteractions155();
        void initInteractions156();
        void initInteractions157();
        void initInteractions158();
        void initInteractions159();
        void initInteractions160();
        void initInteractions161();
        void initInteractions162();
        void initInteractions163();
        void initInteractions164();
        void initInteractions165();
        void initInteractions166();
        void initInteractions167();
        void initInteractions168();
        void initInteractions169();
        void initInteractions170();
        void initInteractions171();
        void initInteractions172();
        void initInteractions173();
        void initInteractions174();
        void initInteractions175();
        void initInteractions176();
        void initInteractions177();
        void initInteractions178();
        void initInteractions179();
        void initInteractions180();
    void initSpectrum();

protected:
    csl::Space const *C_1_1;
    csl::Space const *C_1_0;
    csl::TDerivative d_der;
    csl::Tensor X;
    mty::Generator f_C_1_1;
    mty::Generator T_C_10_C_1_0;
    std::vector<csl::Index> i_dirac;
    std::vector<csl::Index> i_Minko;
    std::vector<csl::Index> i_C_1_1;
    std::vector<csl::Index> i_C_1_0;
};

} // End of namespace mty

#endif
