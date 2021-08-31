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

#include "feynmanIntegral.h"
#include "mrtError.h"
#include "looptools_extension.h"
#include "mrtInterface.h"
#include "localTerms.h"
#include "clooptools.h"
#include "propagator.h"
#include "mrtUtils.h"
#include "mrtOptions.h"
using namespace csl;
namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Utility Functions                             //
/*************************************************/
///////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, 
                         IntegralType  type)
{
    switch(type) {
        case IntegralType::None: out << "None"; break;
        case IntegralType::A: out << "A";       break;
        case IntegralType::B: out << "B";       break;
        case IntegralType::C: out << "C";       break;
        case IntegralType::D: out << "D";       break;
        case IntegralType::E: out << "E";       break;
    }
    return out;
}

void printLooptoolsId_A(int id,
                        std::ostream& out)
{
    switch(id) {
        case aa0: out << "A0"; break;
        case aa00: out << "A00"; break;
        default : out << "A_"; break;
    }
}

void printLooptoolsId_B(int id,
                        std::ostream& out)
{
    switch(id) {
        case bb0: out << "B0"; break;
        case bb1: out << "B1"; break;
        case bb00: out << "B00"; break;
        case bb11: out << "B11"; break;
        case bb001: out << "B001"; break;
        case bb111: out << "B111"; break;
        case dbb0: out << "DB0"; break;
        case dbb1: out << "DB1"; break;
        case dbb00: out << "DB00"; break;
        case dbb11: out << "DB11"; break;
        case dbb001: out << "DB001"; break;
        default: out << "B_"; break;
    }
}

void printLooptoolsId_C(int id,
                        std::ostream& out)
{
    switch(id) {
        case cc0: out << "C0"; break; 
        case cc1: out << "C1"; break; 
        case cc2: out << "C2"; break; 
        case cc00: out << "C00"; break;
        case cc11: out << "C11"; break;
        case cc12: out << "C12"; break;
        case cc22: out << "C22"; break;
        case cc001: out << "C001"; break;
        case cc002: out << "C002"; break;
        case cc111: out << "C111"; break;
        case cc112: out << "C112"; break;
        case cc122: out << "C122"; break;
        case cc222: out << "C222"; break;
        case cc0000: out << "C0000"; break;
        case cc0011: out << "C0011"; break;
        case cc0012: out << "C0012"; break;
        case cc0022: out << "C0022"; break;
        case cc1111: out << "C1111"; break;
        case cc1112: out << "C1112"; break;
        case cc1122: out << "C1122"; break;
        case cc1222: out << "C1222"; break;
        case cc2222: out << "C2222"; break;
        default: out << "C_"; break;
    }
}

void printLooptoolsId_D(int id,
                        std::ostream& out)
{
    switch(id) {
        case dd0: out << "D0"; break;
        case dd1: out << "D1"; break;
        case dd2: out << "D2"; break;
        case dd3: out << "D3"; break;
        case dd00: out << "D00"; break;
        case dd11: out << "D11"; break;
        case dd12: out << "D12"; break;
        case dd13: out << "D13"; break;
        case dd22: out << "D22"; break;
        case dd23: out << "D23"; break;
        case dd33: out << "D33"; break;
        case dd001: out << "D001"; break;
        case dd002: out << "D002"; break;
        case dd003: out << "D003"; break;
        case dd111: out << "D111"; break;
        case dd112: out << "D112"; break;
        case dd113: out << "D113"; break;
        case dd122: out << "D122"; break;
        case dd123: out << "D123"; break;
        case dd133: out << "D133"; break;
        case dd222: out << "D222"; break;
        case dd223: out << "D223"; break;
        case dd233: out << "D233"; break;
        case dd333: out << "D333"; break;
        case dd0000: out << "D0000"; break;
        case dd0011: out << "D0011"; break;
        case dd0012: out << "D0012"; break;
        case dd0013: out << "D0013"; break;
        case dd0022: out << "D0022"; break;
        case dd0023: out << "D0023"; break;
        case dd0033: out << "D0033"; break;
        case dd1111: out << "D1111"; break;
        case dd1112: out << "D1112"; break;
        case dd1113: out << "D1113"; break;
        case dd1122: out << "D1122"; break;
        case dd1123: out << "D1123"; break;
        case dd1133: out << "D1133"; break;
        case dd1222: out << "D1222"; break;
        case dd1223: out << "D1223"; break;
        case dd1233: out << "D1233"; break;
        case dd1333: out << "D1333"; break;
        case dd2222: out << "D2222"; break;
        case dd2223: out << "D2223"; break;
        case dd2233: out << "D2233"; break;
        case dd2333: out << "D2333"; break;
        case dd3333: out << "D3333"; break;
        case dd00001: out << "D00001"; break;
        case dd00002: out << "D00002"; break;
        case dd00003: out << "D00003"; break;
        case dd00111: out << "D00111"; break;
        case dd00112: out << "D00112"; break;
        case dd00113: out << "D00113"; break;
        case dd00122: out << "D00122"; break;
        case dd00123: out << "D00123"; break;
        case dd00133: out << "D00133"; break;
        case dd00222: out << "D00222"; break;
        case dd00223: out << "D00223"; break;
        case dd00233: out << "D00233"; break;
        case dd00333: out << "D00333"; break;
        case dd11111: out << "D11111"; break;
        case dd11112: out << "D11112"; break;
        case dd11113: out << "D11113"; break;
        case dd11122: out << "D11122"; break;
        case dd11123: out << "D11123"; break;
        case dd11133: out << "D11133"; break;
        case dd11222: out << "D11222"; break;
        case dd11223: out << "D11223"; break;
        case dd11233: out << "D11233"; break;
        case dd11333: out << "D11333"; break;
        case dd12222: out << "D12222"; break;
        case dd12223: out << "D12223"; break;
        case dd12233: out << "D12233"; break;
        case dd12333: out << "D12333"; break;
        case dd13333: out << "D13333"; break;
        case dd22222: out << "D22222"; break;
        case dd22223: out << "D22223"; break;
        case dd22233: out << "D22233"; break;
        case dd22333: out << "D22333"; break;
        case dd23333: out << "D23333"; break;
        case dd33333: out << "D33333"; break;
        default: out << "D_"; break;
    }
}

void printLooptoolsId_E(int id,
                        std::ostream& out)
{
    switch(id) {
        case ee0: out << "E0"; break;
        case ee1: out << "E1"; break;
        case ee2: out << "E2"; break;
        case ee3: out << "E3"; break;
        case ee4: out << "E4"; break;
        case ee00: out << "E00"; break;
        case ee11: out << "E11"; break;
        case ee12: out << "E12"; break;
        case ee13: out << "E13"; break;
        case ee14: out << "E14"; break;
        case ee22: out << "E22"; break;
        case ee23: out << "E23"; break;
        case ee24: out << "E24"; break;
        case ee33: out << "E33"; break;
        case ee34: out << "E34"; break;
        case ee44: out << "E44"; break;
        case ee001: out << "E001"; break;
        case ee002: out << "E002"; break;
        case ee003: out << "E003"; break;
        case ee004: out << "E004"; break;
        case ee111: out << "E111"; break;
        case ee112: out << "E112"; break;
        case ee113: out << "E113"; break;
        case ee114: out << "E114"; break;
        case ee122: out << "E122"; break;
        case ee123: out << "E123"; break;
        case ee124: out << "E124"; break;
        case ee133: out << "E133"; break;
        case ee134: out << "E134"; break;
        case ee144: out << "E144"; break;
        case ee222: out << "E222"; break;
        case ee223: out << "E223"; break;
        case ee224: out << "E224"; break;
        case ee233: out << "E233"; break;
        case ee234: out << "E234"; break;
        case ee244: out << "E244"; break;
        case ee333: out << "E333"; break;
        case ee334: out << "E334"; break;
        case ee344: out << "E344"; break;
        case ee444: out << "E444"; break;
        case ee0000: out << "E0000"; break;
        case ee0011: out << "E0011"; break;
        case ee0012: out << "E0012"; break;
        case ee0013: out << "E0013"; break;
        case ee0014: out << "E0014"; break;
        case ee0022: out << "E0022"; break;
        case ee0023: out << "E0023"; break;
        case ee0024: out << "E0024"; break;
        case ee0033: out << "E0033"; break;
        case ee0034: out << "E0034"; break;
        case ee0044: out << "E0044"; break;
        case ee1111: out << "E1111"; break;
        case ee1112: out << "E1112"; break;
        case ee1113: out << "E1113"; break;
        case ee1114: out << "E1114"; break;
        case ee1122: out << "E1122"; break;
        case ee1123: out << "E1123"; break;
        case ee1124: out << "E1124"; break;
        case ee1133: out << "E1133"; break;
        case ee1134: out << "E1134"; break;
        case ee1144: out << "E1144"; break;
        case ee1222: out << "E1222"; break;
        case ee1223: out << "E1223"; break;
        case ee1224: out << "E1224"; break;
        case ee1233: out << "E1233"; break;
        case ee1234: out << "E1234"; break;
        case ee1244: out << "E1244"; break;
        case ee1333: out << "E1333"; break;
        case ee1334: out << "E1334"; break;
        case ee1344: out << "E1344"; break;
        case ee1444: out << "E1444"; break;
        case ee2222: out << "E2222"; break;
        case ee2223: out << "E2223"; break;
        case ee2224: out << "E2224"; break;
        case ee2233: out << "E2233"; break;
        case ee2234: out << "E2234"; break;
        case ee2244: out << "E2244"; break;
        case ee2333: out << "E2333"; break;
        case ee2334: out << "E2334"; break;
        case ee2344: out << "E2344"; break;
        case ee2444: out << "E2444"; break;
        case ee3333: out << "E3333"; break;
        case ee3334: out << "E3334"; break;
        case ee3344: out << "E3344"; break;
        case ee3444: out << "E3444"; break;
        case ee4444: out << "E4444"; break;
        // // From here: non looptools functions: rank 5 5-pt function. See PJFry.
        // case Nee+0:   out << "E00001"; break;
        // case Nee+3:   out << "E00002"; break;
        // case Nee+6:   out << "E00003"; break;
        // case Nee+9:   out << "E00004"; break;
        // case Nee+12:  out << "E00111"; break;
        // case Nee+15:  out << "E00112"; break;
        // case Nee+18:  out << "E00113"; break;
        // case Nee+21:  out << "E00114"; break;
        // case Nee+24:  out << "E00122"; break;
        // case Nee+27:  out << "E00123"; break;
        // case Nee+30:  out << "E00124"; break;
        // case Nee+33:  out << "E00133"; break;
        // case Nee+36:  out << "E00134"; break;
        // case Nee+39:  out << "E00144"; break;
        // case Nee+42:  out << "E00222"; break;
        // case Nee+45:  out << "E00223"; break;
        // case Nee+48:  out << "E00224"; break;
        // case Nee+51:  out << "E00233"; break;
        // case Nee+54:  out << "E00234"; break;
        // case Nee+57:  out << "E00244"; break;
        // case Nee+60:  out << "E00333"; break;
        // case Nee+63:  out << "E00334"; break;
        // case Nee+66:  out << "E00344"; break;
        // case Nee+69:  out << "E00444"; break;
        // case Nee+72:  out << "E11111"; break;
        // case Nee+75:  out << "E11112"; break;
        // case Nee+78:  out << "E11113"; break;
        // case Nee+81:  out << "E11114"; break;
        // case Nee+84:  out << "E11122"; break;
        // case Nee+87:  out << "E11123"; break;
        // case Nee+90:  out << "E11124"; break;
        // case Nee+93:  out << "E11133"; break;
        // case Nee+96:  out << "E11134"; break;
        // case Nee+99:  out << "E11144"; break;
        // case Nee+102: out << "E11222"; break;
        // case Nee+105: out << "E11223"; break;
        // case Nee+108: out << "E11224"; break;
        // case Nee+111: out << "E11233"; break;
        // case Nee+114: out << "E11234"; break;
        // case Nee+117: out << "E11244"; break;
        // case Nee+120: out << "E11333"; break;
        // case Nee+123: out << "E11334"; break;
        // case Nee+126: out << "E11344"; break;
        // case Nee+129: out << "E11444"; break;
        // case Nee+132: out << "E12222"; break;
        // case Nee+135: out << "E12223"; break;
        // case Nee+138: out << "E12224"; break;
        // case Nee+141: out << "E12233"; break;
        // case Nee+144: out << "E12234"; break;
        // case Nee+147: out << "E12244"; break;
        // case Nee+150: out << "E12333"; break;
        // case Nee+153: out << "E12334"; break;
        // case Nee+156: out << "E12344"; break;
        // case Nee+159: out << "E12444"; break;
        // case Nee+162: out << "E13333"; break;
        // case Nee+165: out << "E13334"; break;
        // case Nee+168: out << "E13344"; break;
        // case Nee+171: out << "E13444"; break;
        // case Nee+174: out << "E14444"; break;
        // case Nee+177: out << "E22222"; break;
        // case Nee+180: out << "E22223"; break;
        // case Nee+183: out << "E22224"; break;
        // case Nee+186: out << "E22233"; break;
        // case Nee+189: out << "E22234"; break;
        // case Nee+192: out << "E22244"; break;
        // case Nee+195: out << "E22333"; break;
        // case Nee+198: out << "E22334"; break;
        // case Nee+201: out << "E22344"; break;
        // case Nee+204: out << "E22444"; break;
        // case Nee+207: out << "E23333"; break;
        // case Nee+210: out << "E23334"; break;
        // case Nee+213: out << "E23344"; break;
        // case Nee+216: out << "E23444"; break;
        // case Nee+219: out << "E24444"; break;
        // case Nee+222: out << "E33333"; break;
        // case Nee+225: out << "E33334"; break;
        // case Nee+228: out << "E33344"; break;
        // case Nee+231: out << "E33444"; break;
        // case Nee+234: out << "E34444"; break;
        // case Nee+237: out << "E44444"; break;
        default: out << "E_"; break;
    }
}

void printLooptoolsId(IntegralType type, 
                      int id, 
                      std::ostream& out)
{
    switch(type) {
        case IntegralType::A: printLooptoolsId_A(id, out); break;
        case IntegralType::B: printLooptoolsId_B(id, out); break;
        case IntegralType::C: printLooptoolsId_C(id, out); break;
        case IntegralType::D: printLooptoolsId_D(id, out); break;
        case IntegralType::E: printLooptoolsId_E(id, out); break;
        default:
            return;
    }
}

int loopToolsIdOf(
        IntegralType               type,
        std::vector<size_t> const &indices
        )
{
    switch(type) {
        case IntegralType::A: return loopToolsIdOf_A(indices);
        case IntegralType::B: return loopToolsIdOf_B(indices);
        case IntegralType::C: return loopToolsIdOf_C(indices);
        case IntegralType::D: return loopToolsIdOf_D(indices);
        default:
            break;
    }
    CallHEPError(mty::error::TypeError,
            "Type " + toString(type) + " not taken into account.")
    return -1;
}

int loopToolsIdOf_A(
        std::vector<size_t> const &indices
        )
{
    if (indices.size() > 2)
        return 0;
    if (indices.size() == 1
            and indices[0] == 0)
        return aa0;
    if (indices.size() == 2
            and indices[0] == 0
            and indices[1] == 0)
        return aa00;
    return -1;
}
#define EQUAL(A, B) \
    std::equal(A.begin(), A.end(), std::initializer_list<int>B.begin())

int loopToolsIdOf_B(
        std::vector<size_t> const &indices
        )
{
    if (indices.size() > 3)
        return 0;
    if (indices.size() == 1) {
        if (indices[0] == 0)
            return bb0;
        return bb1;
    }
    if (indices.size() == 2) {
        if (EQUAL(indices, ({0, 0})))
            return bb00;
        if (EQUAL(indices, ({1, 1})))
            return bb11;
    }
    if (indices.size() == 3) {
        if (EQUAL(indices, ({0, 0, 1})))
            return bb001;
        if (EQUAL(indices, ({1, 1, 1})))
            return bb111;
    }

    return -1;
}

int loopToolsIdOf_C(
        std::vector<size_t> const &indices
        )
{
    if (indices.size() > 4)
        return 0;
    if (indices.size() == 1) {
        if (indices[0] == 0)
            return cc0;
        if (indices[0] == 1)
            return cc1;
        return cc2;
    }
    if (indices.size() == 2) {
        if (EQUAL(indices, ({0, 0})))
            return cc00;
        if (EQUAL(indices, ({1, 1})))
            return cc11;
        if (EQUAL(indices, ({1, 2})))
            return cc12;
        if (EQUAL(indices, ({2, 2})))
            return cc22;
    }
    if (indices.size() == 3) {
        if (EQUAL(indices, ({0, 0, 1})))
            return cc001;
        if (EQUAL(indices, ({0, 0, 2})))
            return cc002;
        if (EQUAL(indices, ({1, 1, 1})))
            return cc111;
        if (EQUAL(indices, ({1, 1, 2})))
            return cc112;
        if (EQUAL(indices, ({1, 2, 2})))
            return cc122;
        if (EQUAL(indices, ({2, 2, 2})))
            return cc222;
    }
    if (indices.size() == 4) {
        if (EQUAL(indices, ({0, 0, 0, 0})))
            return cc0000;
        if (EQUAL(indices, ({0, 0, 1, 1})))
            return cc0011;
        if (EQUAL(indices, ({0, 0, 1, 2})))
            return cc0012;
        if (EQUAL(indices, ({0, 0, 2, 2})))
            return cc0022;
        if (EQUAL(indices, ({1, 1, 1, 1})))
            return cc1111;
        if (EQUAL(indices, ({1, 1, 1, 2})))
            return cc1112;
        if (EQUAL(indices, ({1, 1, 2, 2})))
            return cc1122;
        if (EQUAL(indices, ({1, 2, 2, 2})))
            return cc1222;
        if (EQUAL(indices, ({2, 2, 2, 2})))
            return cc2222;
    }

    return -1;
}

int loopToolsIdOf_D(
        std::vector<size_t> const &indices
        )
{
    if (indices.size() > 5)
        return -1;
    switch(indices.size()) {
        case 1:
            if (EQUAL(indices, ({0})))
                return dd0;
            if (EQUAL(indices, ({1})))
                return dd1;
            if (EQUAL(indices, ({2})))
                return dd2;
            if (EQUAL(indices, ({3})))
                return dd3;
            break;
        case 2:
            if (EQUAL(indices, ({0, 0})))
                return dd00;
            if (EQUAL(indices, ({1, 1})))
                return dd11;
            if (EQUAL(indices, ({1, 2})))
                return dd12;
            if (EQUAL(indices, ({1, 3})))
                return dd13;
            if (EQUAL(indices, ({2, 2})))
                return dd22;
            if (EQUAL(indices, ({2, 3})))
                return dd23;
            if (EQUAL(indices, ({3, 3})))
                return dd33;
            break;
        case 3:
            if (EQUAL(indices, ({0, 0, 1})))
                return dd001;
            if (EQUAL(indices, ({0, 0, 2})))
                return dd002;
            if (EQUAL(indices, ({0, 0, 3})))
                return dd003;
            if (EQUAL(indices, ({1, 1, 1})))
                return dd111;
            if (EQUAL(indices, ({1, 1, 2})))
                return dd112;
            if (EQUAL(indices, ({1, 1, 3})))
                return dd113;
            if (EQUAL(indices, ({1, 2, 2})))
                return dd122;
            if (EQUAL(indices, ({1, 2, 3})))
                return dd123;
            if (EQUAL(indices, ({1, 3, 3})))
                return dd133;
            if (EQUAL(indices, ({2, 2, 2})))
                return dd222;
            if (EQUAL(indices, ({2, 2, 3})))
                return dd223;
            if (EQUAL(indices, ({2, 3, 3})))
                return dd233;
            if (EQUAL(indices, ({3, 3, 3})))
                return dd333;
            break;
        case 4:
            if (EQUAL(indices, ({0, 0, 0, 0})))
                return dd0000;
            if (EQUAL(indices, ({0, 0, 1, 1})))
                return dd0011;
            if (EQUAL(indices, ({0, 0, 1, 2})))
                return dd0012;
            if (EQUAL(indices, ({0, 0, 1, 3})))
                return dd0013;
            if (EQUAL(indices, ({0, 0, 2, 2})))
                return dd0022;
            if (EQUAL(indices, ({0, 0, 2, 3})))
                return dd0023;
            if (EQUAL(indices, ({0, 0, 3, 3})))
                return dd0033;
            if (EQUAL(indices, ({1, 1, 1, 1})))
                return dd1111;
            if (EQUAL(indices, ({1, 1, 1, 2})))
                return dd1112;
            if (EQUAL(indices, ({1, 1, 1, 3})))
                return dd1113;
            if (EQUAL(indices, ({1, 1, 2, 2})))
                return dd1122;
            if (EQUAL(indices, ({1, 1, 2, 3})))
                return dd1123;
            if (EQUAL(indices, ({1, 1, 3, 3})))
                return dd1133;
            if (EQUAL(indices, ({1, 2, 2, 2})))
                return dd1222;
            if (EQUAL(indices, ({1, 2, 2, 3})))
                return dd1223;
            if (EQUAL(indices, ({1, 2, 3, 3})))
                return dd1233;
            if (EQUAL(indices, ({1, 3, 3, 3})))
                return dd1333;
            if (EQUAL(indices, ({2, 2, 2, 2})))
                return dd2222;
            if (EQUAL(indices, ({2, 2, 2, 3})))
                return dd2223;
            if (EQUAL(indices, ({2, 2, 3, 3})))
                return dd2233;
            if (EQUAL(indices, ({2, 3, 3, 3})))
                return dd2333;
            if (EQUAL(indices, ({3, 3, 3, 3})))
                return dd3333;
            if (EQUAL(indices, ({0, 0, 0, 0, 1})))
                return dd00001;
            if (EQUAL(indices, ({0, 0, 0, 0, 2})))
                return dd00002;
            if (EQUAL(indices, ({0, 0, 0, 0, 3})))
                return dd00003;
            if (EQUAL(indices, ({0, 0, 1, 1, 1})))
                return dd00111;
            if (EQUAL(indices, ({0, 0, 1, 1, 2})))
                return dd00112;
            if (EQUAL(indices, ({0, 0, 1, 1, 3})))
                return dd00113;
            if (EQUAL(indices, ({0, 0, 1, 2, 2})))
                return dd00122;
            if (EQUAL(indices, ({0, 0, 1, 2, 3})))
                return dd00123;
            if (EQUAL(indices, ({0, 0, 1, 3, 3})))
                return dd00133;
            if (EQUAL(indices, ({0, 0, 2, 2, 2})))
                return dd00222;
            if (EQUAL(indices, ({0, 0, 2, 2, 3})))
                return dd00223;
            if (EQUAL(indices, ({0, 0, 2, 3, 3})))
                return dd00233;
            if (EQUAL(indices, ({0, 0, 3, 3, 3})))
                return dd00333;
            if (EQUAL(indices, ({1, 1, 1, 1, 1})))
                return dd11111;
            if (EQUAL(indices, ({1, 1, 1, 1, 2})))
                return dd11112;
            if (EQUAL(indices, ({1, 1, 1, 1, 3})))
                return dd11113;
            if (EQUAL(indices, ({1, 1, 1, 2, 2})))
                return dd11122;
            if (EQUAL(indices, ({1, 1, 1, 2, 3})))
                return dd11123;
            if (EQUAL(indices, ({1, 1, 1, 3, 3})))
                return dd11133;
            if (EQUAL(indices, ({1, 1, 2, 2, 2})))
                return dd11222;
            if (EQUAL(indices, ({1, 1, 2, 2, 3})))
                return dd11223;
            if (EQUAL(indices, ({1, 1, 2, 3, 3})))
                return dd11233;
            if (EQUAL(indices, ({1, 1, 3, 3, 3})))
                return dd11333;
            if (EQUAL(indices, ({1, 2, 2, 2, 2})))
                return dd12222;
            if (EQUAL(indices, ({1, 2, 2, 2, 3})))
                return dd12223;
            if (EQUAL(indices, ({1, 2, 2, 3, 3})))
                return dd12233;
            if (EQUAL(indices, ({1, 2, 3, 3, 3})))
                return dd12333;
            if (EQUAL(indices, ({1, 3, 3, 3, 3})))
                return dd13333;
            if (EQUAL(indices, ({2, 2, 2, 2, 2})))
                return dd22222;
            if (EQUAL(indices, ({2, 2, 2, 2, 3})))
                return dd22223;
            if (EQUAL(indices, ({2, 2, 2, 3, 3})))
                return dd22233;
            if (EQUAL(indices, ({2, 2, 3, 3, 3})))
                return dd22333;
            if (EQUAL(indices, ({2, 3, 3, 3, 3})))
                return dd23333;
            if (EQUAL(indices, ({3, 3, 3, 3, 3})))
                return dd33333;
            break;

    }

    return -1;
}

#undef EQUAL

std::array<int, 5> getRank5Indices(int loopToolsId)
{
    switch(loopToolsId) {
        case Nee+0:   return {0, 0, 0, 0, 1};
        case Nee+3:   return {0, 0, 0, 0, 2};
        case Nee+6:   return {0, 0, 0, 0, 3};
        case Nee+9:   return {0, 0, 0, 0, 4};
        case Nee+12:  return {0, 0, 1, 1, 1};
        case Nee+15:  return {0, 0, 1, 1, 2};
        case Nee+18:  return {0, 0, 1, 1, 3};
        case Nee+21:  return {0, 0, 1, 1, 4};
        case Nee+24:  return {0, 0, 1, 2, 2};
        case Nee+27:  return {0, 0, 1, 2, 3};
        case Nee+30:  return {0, 0, 1, 2, 4};
        case Nee+33:  return {0, 0, 1, 3, 3};
        case Nee+36:  return {0, 0, 1, 3, 4};
        case Nee+39:  return {0, 0, 1, 4, 4};
        case Nee+42:  return {0, 0, 2, 2, 2};
        case Nee+45:  return {0, 0, 2, 2, 3};
        case Nee+48:  return {0, 0, 2, 2, 4};
        case Nee+51:  return {0, 0, 2, 3, 3};
        case Nee+54:  return {0, 0, 2, 3, 4};
        case Nee+57:  return {0, 0, 2, 4, 4};
        case Nee+60:  return {0, 0, 3, 3, 3};
        case Nee+63:  return {0, 0, 3, 3, 4};
        case Nee+66:  return {0, 0, 3, 4, 4};
        case Nee+69:  return {0, 0, 4, 4, 4};
        case Nee+72:  return {1, 1, 1, 1, 1};
        case Nee+75:  return {1, 1, 1, 1, 2};
        case Nee+78:  return {1, 1, 1, 1, 3};
        case Nee+81:  return {1, 1, 1, 1, 4};
        case Nee+84:  return {1, 1, 1, 2, 2};
        case Nee+87:  return {1, 1, 1, 2, 3};
        case Nee+90:  return {1, 1, 1, 2, 4};
        case Nee+93:  return {1, 1, 1, 3, 3};
        case Nee+96:  return {1, 1, 1, 3, 4};
        case Nee+99:  return {1, 1, 1, 4, 4};
        case Nee+102: return {1, 1, 2, 2, 2};
        case Nee+105: return {1, 1, 2, 2, 3};
        case Nee+108: return {1, 1, 2, 2, 4};
        case Nee+111: return {1, 1, 2, 3, 3};
        case Nee+114: return {1, 1, 2, 3, 4};
        case Nee+117: return {1, 1, 2, 4, 4};
        case Nee+120: return {1, 1, 3, 3, 3};
        case Nee+123: return {1, 1, 3, 3, 4};
        case Nee+126: return {1, 1, 3, 4, 4};
        case Nee+129: return {1, 1, 4, 4, 4};
        case Nee+132: return {1, 2, 2, 2, 2};
        case Nee+135: return {1, 2, 2, 2, 3};
        case Nee+138: return {1, 2, 2, 2, 4};
        case Nee+141: return {1, 2, 2, 3, 3};
        case Nee+144: return {1, 2, 2, 3, 4};
        case Nee+147: return {1, 2, 2, 4, 4};
        case Nee+150: return {1, 2, 3, 3, 3};
        case Nee+153: return {1, 2, 3, 3, 4};
        case Nee+156: return {1, 2, 3, 4, 4};
        case Nee+159: return {1, 2, 4, 4, 4};
        case Nee+162: return {1, 3, 3, 3, 3};
        case Nee+165: return {1, 3, 3, 3, 4};
        case Nee+168: return {1, 3, 3, 4, 4};
        case Nee+171: return {1, 3, 4, 4, 4};
        case Nee+174: return {1, 4, 4, 4, 4};
        case Nee+177: return {2, 2, 2, 2, 2};
        case Nee+180: return {2, 2, 2, 2, 3};
        case Nee+183: return {2, 2, 2, 2, 4};
        case Nee+186: return {2, 2, 2, 3, 3};
        case Nee+189: return {2, 2, 2, 3, 4};
        case Nee+192: return {2, 2, 2, 4, 4};
        case Nee+195: return {2, 2, 3, 3, 3};
        case Nee+198: return {2, 2, 3, 3, 4};
        case Nee+201: return {2, 2, 3, 4, 4};
        case Nee+204: return {2, 2, 4, 4, 4};
        case Nee+207: return {2, 3, 3, 3, 3};
        case Nee+210: return {2, 3, 3, 3, 4};
        case Nee+213: return {2, 3, 3, 4, 4};
        case Nee+216: return {2, 3, 4, 4, 4};
        case Nee+219: return {2, 4, 4, 4, 4};
        case Nee+222: return {3, 3, 3, 3, 3};
        case Nee+225: return {3, 3, 3, 3, 4};
        case Nee+228: return {3, 3, 3, 4, 4};
        case Nee+231: return {3, 3, 4, 4, 4};
        case Nee+234: return {3, 4, 4, 4, 4};
        case Nee+237: return {4, 4, 4, 4, 4};
    }
    CallHEPError(mty::error::RuntimeError,
            "ID " + toString(loopToolsId) + " unrecognized for rank 5 5-pt"
            " function. Should be between " + toString(Nee) + " and "
            + toString(Nee+237) + " with step 3.");

    return {0, 0, 0, 0, 0};
}

int loopToolsBegin(IntegralType type,
                   size_t       nIndices)
{
    switch(type) {
        case IntegralType::A:
            switch(nIndices) {
                case 0: return aa0;
                case 2: return aa00;
            }
        break;

        case IntegralType::B:
            switch(nIndices) {
                case 0: return bb0;
                case 1: return bb1;
                case 2: return bb00;
                case 3: return bb001;
            }
        break;

        case IntegralType::C:
            switch(nIndices) {
                case 0: return cc0;
                case 1: return cc1;
                case 2: return cc00;
                case 3: return cc001;
                case 4: return cc0000;
            }
        break;

        case IntegralType::D:
            switch(nIndices) {
                case 0: return dd0;
                case 1: return dd1;
                case 2: return dd00;
                case 3: return dd001;
                case 4: return dd0000;
                case 5: return dd00001;
            }
        break;

        case IntegralType::E:
            switch(nIndices) {
                case 0: return ee0;
                case 1: return ee1;
                case 2: return ee00;
                case 3: return ee001;
                case 4: return ee0000;
                case 5: return Nee;
            }
        break;

        default: break;
    }

    return Nee;
    // CallHEPError(mty::error::RuntimeError,
    //         "Type " + toString(type) + " for " + toString(nIndices) 
    //         + " indices is not possible within looptools extension");

    // return -1;
}

size_t nPropagatorForIntegral(IntegralType type)
{
    switch(type) {
        case IntegralType::None:
        case IntegralType::A: return 1;
        case IntegralType::B: return 2;
        case IntegralType::C: return 3;
        case IntegralType::D: return 4;
        case IntegralType::E: return 5;
    }
    return 0;
}

size_t nIndicesForIntegral(IntegralType type)
{
    switch(type) {
        case IntegralType::None:
        case IntegralType::A: return 2;
        case IntegralType::B: return 3;
        case IntegralType::C: return 4;
        case IntegralType::D: return 5;
        case IntegralType::E: return 5;
    }
    return 0;
}

size_t nArgumentsForIntegral(IntegralType type)
{
    switch(type) {
        case IntegralType::None:
        case IntegralType::A: return 1;
        case IntegralType::B: return 3;
        case IntegralType::C: return 6;
        case IntegralType::D: return 10;
        case IntegralType::E: return 15;
    }
    return 0;
}

csl::Expr getDivergentFactor(IntegralType type,
                        int          integral_id,
                        std::vector<csl::Expr> const& arguments)
{
    HEPAssert(arguments.size() == nArgumentsForIntegral(type),
            mty::error::TypeError,
            "Expecting " + toString(nArgumentsForIntegral(type)) + " for "
            "integral of type " + toString(type) + ", " 
            + toString(arguments.size()) + " given.");
    switch(type) {
        case IntegralType::A: 
            if (integral_id == aa0) return -2*arguments[0];
            break;
        case IntegralType::B:
            if (integral_id == bb0) return CSL_M_2;
            if (integral_id == bb1) return CSL_1;
            if (integral_id == bb00) return CSL_1 / 6 * 
                (arguments[0]
                -3 * arguments[1]
                -3 * arguments[2]
                );
            if (integral_id == bb11) return 2*CSL_M_THIRD;
            if (integral_id == bb001) return CSL_M_1 / 12 * 
                (arguments[0]
                -2 * arguments[1]
                -4 * arguments[2]
                );
            if (integral_id == bb111) return CSL_1 / 2;
            break;
        case IntegralType::C:
            if (integral_id == cc00) return CSL_M_1 / 2;
            if (integral_id == cc001
                    or integral_id == cc002) 
                return CSL_1 / 6;
            if (integral_id == cc0000) 
                return CSL_1 / 48 * (arguments[0]+arguments[1]+arguments[2])
                    - CSL_1 / 12 * (arguments[3]+arguments[4]+arguments[5]);
            if (integral_id == cc0011
                    or integral_id == cc0022)
                return - CSL_1 / 12;
            if (integral_id == cc0012)
                return -CSL_1 / 24;
            break;
        case IntegralType::D:
            if (integral_id == dd0000) return CSL_M_1 / 12;
        default:
            break;
    }

    return CSL_0;
}

///////////////////////////////////////////////////
/*************************************************/
// Class FeynmanIntegral                         //
/*************************************************/
///////////////////////////////////////////////////

csl::Expr FeynmanIntegral::replaceIntegral(csl::Expr const& expr)
{
    csl::Expr integral;
    csl::Expr factor = CSL_1;
    if (expr->getType() == csl::Type::VectorIntegral)
        integral = expr;
    else if (expr->getType() == csl::Type::Prod) {
        std::vector<csl::Expr> terms;
        terms.reserve(expr->size());
        for (const auto &arg : expr)
            if (arg->getType() == csl::Type::VectorIntegral)
                integral = arg;
            else
                terms.push_back(arg);
        factor = csl::prod_s(terms, true);
    }
    HEPAssert(integral && integral->getType() == csl::Type::VectorIntegral,
            mty::error::TypeError,
            "Expeting VectorIntegral, " + 
            ((integral) ? toString(integral->getType()) : std::string("NULL"))
            + " given.");

    csl::Parent variable = integral->getParent();
    csl::Expr res = csl::DeepExpandedIf(integral->getOperand(),
    [&](csl::Expr const& el)
    {
        return el->dependsExplicitlyOn(variable.get());
    });

    if (res->getType() == csl::Type::Sum) {
        std::vector<csl::Expr> terms;
        terms.reserve(res->size());
        for (const auto& arg : *res) {
            terms.push_back(replaceIntegral(csl::Refreshed(arg), variable));
        }
        return factor * csl::sum_s(terms);
    }

    res = replaceIntegral(csl::Refreshed(res), variable);
    return factor * res;
}

csl::Expr FeynmanIntegral::replaceIntegral(
        csl::Expr        const& argument,
        csl::Parent      const& variable
        )
{
    if (argument == CSL_0)
        return CSL_0;
    std::vector<csl::Expr> factor;
    std::vector<csl::Index> indices;
    std::vector<csl::Expr> momentum;
    std::vector<csl::Expr> mass;

    csl::Expr firstTerm;
    auto parse = [&](csl::Expr const& el)
    {
        csl::Expr term;
        csl::Expr exponent = CSL_1;
        if (csl::IsPow(el) && csl::IsInteger(el[1]) 
                && IsOfType<Propagator>(el[0])) {
            term = el[0];
            exponent = el[1];
        }
        else {
            term = el;
        }
        if (IsOfType<Propagator>(term)) {
            csl::Expr mom = DeepCopy(term->getArgument(0));
            csl::Expr fact = CSL_1;
            csl::ForEachNode(mom, [&](csl::Expr& node)
            {
                csl::Expr tensor = csl::Refreshed(node->getTerm().value_or(node));
                if (tensor->getType() == csl::Type::TensorElement
                        and tensor->getParent_info() == variable.get()) {
                    fact = node->getNumericalFactor();
                    node = CSL_0;
                    return true;
                }
                return false;
            });
            int exp = std::abs(static_cast<int>(
                        std::round(exponent->evaluateScalar())));
            for (int i = 0; i != exp; ++i) {
                momentum.push_back(DeepRefreshed(Expanded(fact * mom, true)));
                mass.push_back(DeepCopy(term->getArgument(1)));
            }
            if (momentum.back() == CSL_0)
                firstTerm = term;
        }
        else if (term->getType() == csl::Type::TensorElement
                and term->getParent_info() == variable.get()) {
            indices.push_back(term->getIndexStructureView()[0]);
        }
        else
            factor.push_back(DeepCopy(term));
    };
    if (argument->getType() == csl::Type::Prod) {
        for (const auto& arg : *argument)
            parse(arg);
    }
    else
        parse(argument);
    const int Ncrit = std::max(0, 2*static_cast<int>(mass.size()) - 3);
    if (option::keepOnlyFirstMassInLoop 
            && static_cast<int>(indices.size()) < Ncrit) {
        // Also check that non divergent integral
        std::fill(momentum.begin(), momentum.end(), CSL_0);
    }

    size_t firstZero = size_t(-1);
    for (size_t i = 0; i != momentum.size(); ++i)
        if (momentum[i] == CSL_0) {
            firstZero = i;
            break;
        }
    HEPAssert(firstZero != size_t(-1) or momentum.empty(),
            mty::error::RuntimeError,
            "No standard q momentum found in integral. Should get at least"
            " one propagator with momentum q (q beeing integration variable), "
            " the operand given is : " + toString(argument) + ".");
    if (momentum.empty())
        firstZero = 0;
    if (firstZero != 0) {
        std::swap(momentum[firstZero], momentum[0]);
        std::swap(mass[firstZero], mass[0]);
    }
    if (!momentum.empty())
        momentum.erase(momentum.begin());

    applyIndices(momentum);
    //for (size_t i = 0; i != indices.size(); ++i) 
    //    for (size_t j = i+1; j < indices.size(); ++j) 
    //        if (indices[i] == indices[j]) {
    //            return applyQSquared(
    //                    indices[i],
    //                    argument,
    //                    variable,
    //                    factor,
    //                    momentum,
    //                    mass,
    //                    indices,
    //                    firstTerm
    //                    );
    //        } 

    //std::cout << "Rank " << indices.size() << " " << 
        //mass.size() << "-point function."<< std::endl;
    return replaceIntegral(
            factor,
            momentum,
            mass,
            indices
            );
}
csl::Expr FeynmanIntegral::replaceIntegral(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices
        )
{
    csl::Expr res;
    switch(momentum.size()) {
        case 0: res = replaceIntegral_A(factor, momentum, mass, indices);
                break;
        case 1: res = replaceIntegral_B(factor, momentum, mass, indices);
                break;
        case 2: res = replaceIntegral_C(factor, momentum, mass, indices);
                break;
        case 3: res = replaceIntegral_D(factor, momentum, mass, indices);
                break;
        case 4: res = replaceIntegral_E(factor, momentum, mass, indices);
                break;
    }

    return res;
}

csl::Expr FeynmanIntegral::applyQSquared(
        csl::Index              const &squaredIndex,
        csl::Expr                    const &argument,
        csl::Parent             const &variable,
        std::vector<csl::Expr>       const &factor,
        std::vector<csl::Expr>       const &momentum,
        std::vector<csl::Expr>       const &mass,
        std::vector<csl::Index> const &indices,
        csl::Expr                    const &firstTerm
        )
{
    csl::Expr p1;
    size_t imin = 0;
    for (size_t k = 0; k != momentum.size(); ++k) 
        if (momentum[k]->size() < momentum[imin]->size())
            imin = k;
    p1 = momentum[imin];
    auto structure = p1->getIndexStructure();
    auto mu = (structure.size() == 0) ? MinkowskiIndex() : structure[0];
    csl::Expr newArgument = csl::DeepCopy(argument);
    csl::FirstOfNode(newArgument, [&](csl::Expr &sub) {
        if (sub == firstTerm) {
            sub = CSL_1;
            return true;
        }
        return false;
    });
    csl::ForEachNodeCut(newArgument, [&](csl::Expr &sub) {
        if (IsOfType<Propagator>(sub))
            return true;
        if (sub == (*variable)(squaredIndex))
            sub = CSL_1;
        else if (sub == (*variable)(squaredIndex.getFlipped()))
            sub = CSL_1;
        return false;
    });
    auto freeStruct = newArgument->getFreeIndexStructure();
    if (p1 != CSL_0)
        csl::Replace(newArgument, variable, (*variable)(mu) - p1);
    // bool denominatorSimpli = csl::FirstOfNode(newArgument, [&](csl::Expr &sub)
    // {
    //     if (sub == propagator_s((*variable)(mu), CSL_0)) {
    //         sub = CSL_1;
    //         return true;
    //     }
    //     return false;
    // });
    // if (denominatorSimpli) {
    //     return replaceIntegral(vectorintegral_s(firstTerm*newArgument, variable));
    // }
    csl::ForEachLeaf(newArgument, [&](csl::Expr &sub)
    {
        if (IsIndicialTensor(sub)) {
            for (auto &index : sub->getIndexStructureView()) {
                auto pos = std::find(freeStruct.begin(), freeStruct.end(), index);
                if (pos == freeStruct.end())
                    index.setFree(false);
            }
        }
    });
    std::vector<csl::Index> newIndices(indices);
    for (size_t i = 0; i != newIndices.size(); ++i) 
        if (newIndices[i] == squaredIndex) {
            newIndices.erase(newIndices.begin() + i);
            --i;
        }
    csl::Expr term1 = replaceIntegral(vectorintegral_s(newArgument, variable));
    csl::Expr term2 = pow_s(mass[0], 2) * replaceIntegral(
                factor,
                momentum,
                mass,
                newIndices
                );
    return term1 + term2;
}

void testError(IntegralType type,
               std::vector<csl::Expr> const& momentum,
               size_t                   nMomentum,
               std::vector<csl::Expr> const& mass,
               size_t                   nMass,
               std::vector<csl::Index> const&,
               size_t)
{
    auto mom_string = [&]() 
    {
        std::ostringstream sout;
        for (const auto& m : momentum)
            sout << m << " ";
        return sout.str();
    };
    auto mass_string = [&]() 
    {
        std::ostringstream sout;
        for (const auto& m : mass)
            sout << m << " ";
        return sout.str();
    };
    // auto index_string = [&]() 
    // {
    //     std::ostringstream sout;
    //     for (const auto& i : indices)
    //         sout << i << " ";
    //     return sout.str();
    // };
    HEPAssert(momentum.size() == nMomentum,
            mty::error::RuntimeError,
            "Integral of type " + toString(type) + " encountered an unvalid "
            "instance for momentum: " + mom_string() + " are invalid, expected "
            + toString(nMomentum) + ".");
    HEPAssert(mass.size() == nMass,
            mty::error::RuntimeError,
            "Integral of type " + toString(type) + " encountered an unvalid "
            "instance for mass: " + mass_string() + " are invalid, expected "
            + toString(nMass) + ".");
    // HEPAssert(indices.size() <= maxIndices,
    //         mty::error::RuntimeError,
    //         "Integral of type " + toString(type) + " encountered an unvalid "
    //         "instance for index: " + index_string() + " are invalid, expected "
    //         + toString(maxIndices) + " at most.");
}

csl::Expr psquared(csl::Expr const& init)
{
    csl::Expr flipped = DeepCopy(init);
    csl::IndexStructure structure = flipped->getIndexStructure();
    for (const csl::Index& index : structure)
        if (index.getSpace()->getSignedIndex())
            flipped = Replaced(flipped, index, index.getFlipped(), false);
    return init * flipped;
}

csl::Expr psquared(csl::Expr const &A,
             csl::Expr const &B)
{
    csl::Expr flipped = DeepCopy(B);
    csl::IndexStructure Astructure = A->getIndexStructure();
    csl::IndexStructure Bstructure = B->getIndexStructure();
    for (size_t i = 0; i != Astructure.size(); ++i) 
        if (Astructure[i].getSpace()->getSignedIndex())
            flipped = Replaced(
                    flipped, 
                    Bstructure[i], 
                    Astructure[i].getFlipped(), 
                    false);
    return A * flipped;
}

csl::Expr psum(csl::Expr const &A,
         csl::Expr const &B)
{
    csl::Expr flipped = csl::DeepCopy(B);
    csl::IndexStructure Astructure = A->getIndexStructure();
    csl::IndexStructure Bstructure = B->getIndexStructure();
    for (size_t i = 0; i != Astructure.size(); ++i) 
        if (Astructure[i].getSpace()->getSignedIndex())
            flipped = Replaced(
                    flipped, 
                    Bstructure[i], 
                    Astructure[i], 
                    false);
    return A + flipped;
}

csl::Expr apply(csl::Expr       const& init, 
           csl::Index const& index)
{
    csl::IndexStructure structure = init->getIndexStructure();
    for (const auto& structure_index : structure)
        if (index.getSpace() == structure_index.getSpace()) {
            return Replaced(init, structure_index, index);
        }

    if (csl::DeepRefreshed(init) == CSL_0)
        return CSL_0;
    CallHEPError(mty::error::RuntimeError,
            "Unable to apply index " + toString(index) + " to expression "
            + toString(init) + ".");
    return DeepCopy(init);
}

void FeynmanIntegral::applyIndices(std::vector<csl::Expr>& momentum)
{
    if (momentum.size() < 2)
        return;
    csl::IndexStructure structure;
    size_t first;
    for (first = 0; first != momentum.size(); ++first)
        if (momentum[first] != CSL_0) {
            structure = momentum[first]->getIndexStructure();
        }
    for (size_t i = 0; i < momentum.size(); ++i) {
        if (i == first)
            continue;
        csl::IndexStructure other_struct = momentum[i]->getIndexStructure();
        HEPAssert(momentum[i] == CSL_0 
                or structure.size() == other_struct.size(),
                mty::error::RuntimeError,
                "Momentum indices do not match between " 
                + toString(momentum[first]) + " and " + toString(momentum[i]) 
                + ".");
        for (size_t j = 0; j != structure.size(); ++j)
            if (momentum[i] != CSL_0)
                momentum[i] = Replaced(
                        momentum[i], 
                        other_struct[j], 
                        structure[j], 
                        false);
    }
}

std::vector<std::vector<size_t>> getMetricIndices(
        std::vector<size_t> indices
        )
{
    size_t n = indices.size();
    HEPAssert(n > 0 and n % 2 == 0,
            mty::error::ValueError,
            "Cannot get metric term for " + toString(n) + " indices.")

    if (n == 2)
        return {{ indices[0], indices[1] }};
    std::function<size_t(size_t)> N; // N(n) = (n-1)!!
    N = [&](size_t n) { return (n == 2) ? 1 : (n-1)*N(n-2); };
    std::vector<std::vector<size_t>> res;
    res.reserve(N(n));
    std::vector<size_t> newIndices(indices.size()-2);
    for (size_t i = 1; i != indices.size(); ++i) {
        for (size_t j = 1; j != indices.size(); ++j)
            if (j < i)
                newIndices[j-1] = indices[j];
            else if (j > i)
                newIndices[j-2] = indices[j];
        std::vector<std::vector<size_t>> interm = getMetricIndices(newIndices);
        auto toInsert = { indices[0], indices[i] };
        for (auto &indexSet : interm) {
            res.push_back(std::move(indexSet));
            res.back().insert(
                    res.back().begin(),
                    toInsert.begin(),
                    toInsert.end());
        }
    }
    return res;
}

std::vector<std::vector<size_t>> getMetricIndices(
        size_t n
        )
{
    std::vector<size_t> indices(n);
    for (size_t i = 0; i != n; ++i)
        indices[i] = i;
    return getMetricIndices(indices);
}

csl::Expr FeynmanIntegral::metricTerm(
        csl::Space              const *space,
        std::vector<csl::Index> const &indices
        )
{
    HEPAssert(indices.size() % 2 == 0,
            mty::error::ValueError,
            "Expecting even number of indices in metric term, " 
            + toString(indices.size()) + " given.");
    if (indices.empty())
        return CSL_1;
    csl::Tensor g = space->getMetric();
    if (indices.size() == 2) {
        auto mu    = indices[0];
        auto nu    = indices[1];
        return g({mu, nu});
    }
    else if (indices.size() == 4) {
        auto mu    = indices[0];
        auto nu    = indices[1];
        auto rho   = indices[2];
        auto sigma = indices[3];
        return g({mu, nu}) * g({rho, sigma})
            + g({mu, rho}) * g({nu, sigma})
            + g({mu, sigma}) * g({nu, rho});
    }

    std::vector<std::vector<size_t>> indicesPos 
        = getMetricIndices(indices.size());
    std::vector<csl::Expr> terms;
    terms.reserve(indicesPos.size());
    for (const auto &pos : indicesPos) {
        std::vector<csl::Expr> args;
        args.reserve(pos.size() / 2);
        for (size_t i = 0; i != pos.size(); i += 2)
            args.push_back(g({indices[pos[i]], indices[pos[i+1]]}));
        terms.push_back(csl::prod_s(args));
    }
    return csl::sum_s(terms);
}

csl::Expr FeynmanIntegral::applyIndicesToTensors(
        csl::Space              const* space,
        std::vector<csl::Expr>       const& momentum,
        std::vector<csl::Index> const& indices,
        std::vector<size_t>            tensorPos)
{
    std::vector<csl::Expr> res;
    std::sort(tensorPos.begin(), tensorPos.end());
    do {
        std::vector<csl::Expr> prod;
        std::vector<csl::Index> metricIndices;
        for (size_t i = 0; i != tensorPos.size(); ++i) {
            if (tensorPos[i] > 0) {
                if (momentum[tensorPos[i]-1] == CSL_0) {
                    prod = {CSL_0};
                    break;
                }
                else
                    prod.push_back(apply(momentum[tensorPos[i]-1], indices[i]));
            }
            else
                metricIndices.push_back(indices[i]);
        }
        if (prod.size() == 1 and prod[0] == CSL_0)
            continue;
        prod.push_back(metricTerm(space, metricIndices));
        if (prod.size() > 1)
            res.push_back(prod_s(prod));
        else
            res.push_back(prod[0]);
    } while (std::next_permutation(tensorPos.begin(), tensorPos.end()));
    return sum_s(res);
}

csl::Expr FeynmanIntegral::computeFinalIntegralDecomposition(
        IntegralType                   type,
        std::vector<csl::Expr>       const& momentum,
        std::vector<csl::Index> const& indices,
        std::vector<csl::Expr>       const& arguments)
{
    std::vector<csl::Expr> sum_terms;
    int integral_id = loopToolsBegin(type, indices.size());
    for (int nMetric = indices.size() / 2; 
             nMetric >= 0; 
           --nMetric) {
        size_t nMomentumIndices = indices.size() - 2*nMetric;
        if (nMomentumIndices > 0) {
            symmetricCounter<size_t> counter(
                    nMomentumIndices, momentum.size());
            std::vector<csl::Expr> sum_terms_intermediate;
            do {
                std::vector<size_t> tensorPos(indices.size(), 0);
                for (size_t i = 2*nMetric; i != indices.size(); ++i)
                    tensorPos[i] = 1 + counter[i - 2*nMetric];
                csl::Expr integral = feynmanintegral_s(type,
                                                  integral_id,
                                                  arguments,
                                                  tensorPos);
                sum_terms_intermediate.push_back(
                        integral*
                        applyIndicesToTensors(
                            &csl::Minkowski,
                            momentum,
                            indices,
                            tensorPos));
                integral_id += loopToolsStep;
            } while (++counter);
            integral_id -= loopToolsStep;
            sum_terms.insert(
                    sum_terms.end(),
                    std::make_move_iterator(sum_terms_intermediate.begin()),
                    std::make_move_iterator(sum_terms_intermediate.end())
                    );
            //sum_terms.push_back(
            //            sum_s(sum_terms_intermediate),
            //            true));
        }
        else {
            csl::Expr integral = feynmanintegral_s(type,
                                              integral_id,
                                              arguments,
                                              std::vector<size_t>(nMetric, 0));
            sum_terms.push_back(integral
                    * applyIndicesToTensors(
                        &csl::Minkowski,
                        momentum,
                        indices,
                        std::vector<size_t>(indices.size(), 0)));
        }
        integral_id += loopToolsStep;
    }
    csl::Expr res = csl::sum_s(sum_terms);
    return res;
}

csl::Expr FeynmanIntegral::replaceIntegral_A(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices)
{
    std::vector<csl::Expr> terms(factor);
    testError(IntegralType::A, momentum, 0, mass, 1, indices, 2);
    if (indices.size() % 2 == 1)
        return CSL_0;

    if (indices.empty())
        terms.push_back(feynmanintegral_s(IntegralType::A,
                                          aa0,
                                          {mass[0]*mass[0]}));
    else if (indices.size() == 2) {
        terms.push_back(csl::Minkowski.getMetric()({indices[0], indices[1]}));
        terms.push_back(feynmanintegral_s(IntegralType::A,
                                          aa00,
                                          {mass[0]*mass[0]}));
    }
    else {
        // arxiv hep-ph/0509141.pdf
        size_t n = indices.size() / 2;
        csl::Expr factor = csl::pow_s(mass[0], 2*n) 
            / (std::pow(2, n) * csl::factorial(n+1));
        csl::Expr offset = CSL_0;
        for (size_t i = 1; i <= n; ++i)
            offset += CSL_1 / (i + 1);
        return factor * metricTerm(&csl::Minkowski, indices) * (
                feynmanintegral_s(
                    IntegralType::A,
                    aa0,
                    {mass[0]*mass[0]}
                    )
                + mass[0]*mass[0]*offset
                );
    }

    return prod_s(terms);
}

csl::Expr FeynmanIntegral::replaceIntegral_B(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices)
{
    std::vector<csl::Expr> terms(factor);
    testError(IntegralType::B, momentum, 1, mass, 2, indices, 3);

    csl::Tensor g = csl::Minkowski.getMetric();
    csl::Expr const&  p = momentum[0];
    std::vector<csl::Expr> arguments = {psquared(p),
                                   mass[0]*mass[0],
                                   mass[1]*mass[1]};
    switch(indices.size()) {
        case 0:
        terms.push_back(feynmanintegral_s(IntegralType::B,
                                          bb0,
                                          arguments));
        break;

        case 1:
        terms.push_back(apply(p, indices[0])
                * feynmanintegral_s(IntegralType::B,
                                    bb1,
                                    arguments));
        break;

        case 2:
        terms.push_back(
            prod_s(
                g({indices[0], indices[1]}),
                feynmanintegral_s(
                    IntegralType::B,
                    bb00,
                    arguments),
                true)
            + prod_s(
                {apply(p, indices[0]),
                apply(p, indices[1]),
                feynmanintegral_s(
                        IntegralType::B,
                        bb11,
                        arguments)},
                true)
            );
        break;

        case 3:
        terms.push_back(
            prod_s({
                apply(p, indices[0]),
                apply(p, indices[1]),
                apply(p, indices[2]),
                feynmanintegral_s(
                        IntegralType::B,
                        bb111,
                        arguments)},
                true)
            +

            (g({indices[0], indices[1]}) * apply(p, indices[2])
             + g({indices[0], indices[2]}) * apply(p, indices[1])
             + g({indices[1], indices[2]}) * apply(p, indices[0]))
            * feynmanintegral_s(IntegralType::B,
                                bb001,
                                arguments)
        );
        break;

        default:
        terms.push_back(computeFinalIntegralDecomposition(
                    IntegralType::B,
                    momentum,
                    indices,
                    arguments));
    }
    return prod_s(terms);
}

csl::Expr mom(csl::Expr init)
{
    auto structure = csl::Abbrev::getFreeStructure(init->getIndexStructure());
    csl::ForEachLeaf(init, [&](csl::Expr &sub) 
    {
        if (csl::IsIndicialTensor(sub)) {
            auto &subStructure = sub->getIndexStructureView();
            for (auto &i : subStructure)
                if (i.getSpace() == &csl::Minkowski) 
                    for (const auto &j : structure) 
                        if (i == j) {
                            i.setFree(false);
                            i.setSign(false);
                        }
        }
    });
    return init;
}

csl::Expr FeynmanIntegral::replaceIntegral_C(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices)
{
    std::vector<csl::Expr> terms(factor);
    testError(IntegralType::C, momentum, 2, mass, 3, indices, 4);

    csl::Tensor g = csl::Minkowski.getMetric();
    csl::Expr const&  p1 = mom(momentum[0]);
    csl::Expr const&  p2 = mom(momentum[1]);
    std::vector<csl::Expr> arguments = {psquared(p1),
                                   psquared(p2-p1),
                                   psquared(p2),
                                   mass[0]*mass[0],
                                   mass[1]*mass[1],
                                   mass[2]*mass[2]};

    if (indices.empty())
        terms.push_back(feynmanintegral_s(IntegralType::C, cc0, arguments));
    else
        terms.push_back(computeFinalIntegralDecomposition(
                    IntegralType::C,
                    momentum,
                    indices,
                    arguments));

    return prod_s(terms);
}

csl::Expr FeynmanIntegral::replaceIntegral_D(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices)
{
    std::vector<csl::Expr> terms(factor);
    testError(IntegralType::D, momentum, 3, mass, 4, indices, 5);

    csl::Tensor g = csl::Minkowski.getMetric();
    csl::Expr const&  p1 = mom(momentum[0]);
    csl::Expr const&  p2 = mom(momentum[1]);
    csl::Expr const&  p3 = mom(momentum[2]);
    std::vector<csl::Expr> arguments = {psquared(p1),
                                   psquared(p2-p1),
                                   psquared(p3-p2),
                                   psquared(p3),
                                   psquared(p2),
                                   psquared(p3-p1),
                                   mass[0]*mass[0],
                                   mass[1]*mass[1],
                                   mass[2]*mass[2],
                                   mass[3]*mass[3]};

    if (indices.empty())
        terms.push_back(feynmanintegral_s(IntegralType::D, dd0, arguments));
    else
        terms.push_back(computeFinalIntegralDecomposition(
                    IntegralType::D,
                    momentum,
                    indices,
                    arguments));

    return prod_s(terms);
}

csl::Expr FeynmanIntegral::replaceIntegral_E(
        std::vector<csl::Expr> const& factor,
        std::vector<csl::Expr> const& momentum,
        std::vector<csl::Expr> const& mass,
        std::vector<csl::Index> const& indices)
{
    std::vector<csl::Expr> terms(factor);
    testError(IntegralType::E, momentum, 4, mass, 5, indices, 5);

    csl::Tensor g = csl::Minkowski.getMetric();
    csl::Expr p1 = mom(momentum[0]);
    csl::Expr p2 = mom(momentum[1]);
    csl::Expr p3 = mom(momentum[2]);
    csl::Expr p4 = mom(momentum[3]);
    std::vector<csl::Expr> arguments(15);
    arguments[0] = psquared(p1);
    arguments[1] = psquared(p2-p1);
    arguments[2] = psquared(p3-p2);
    arguments[3] = psquared(p4-p3);
    arguments[4] = psquared(p4);
    arguments[5] = psquared(p2);
    arguments[6] = psquared(p3-p1);
    arguments[7] = psquared(p4-p2);
    arguments[8] = psquared(p3);
    arguments[9] = psquared(p1-p4);
    arguments[10] = mass[0]*mass[0];
    arguments[11] = mass[1]*mass[1];
    arguments[12] = mass[2]*mass[2];
    arguments[13] = mass[3]*mass[3];
    arguments[14] = mass[4]*mass[4];

    if (indices.empty())
        terms.push_back(feynmanintegral_s(IntegralType::E, ee0, arguments));
    else
        terms.push_back(computeFinalIntegralDecomposition(
                    IntegralType::E,
                    momentum,
                    indices,
                    arguments));

    return prod_s(terms);
}

FeynmanIntegral::FeynmanIntegral(IntegralType             t_type,
                                 int                      t_looptoolsId,
                                 std::vector<csl::Expr> const& t_argument)
    :type(t_type),
    loopToolsId(t_looptoolsId)
{
    HEPAssert(t_argument.size() == nArgumentsForIntegral(type),
            mty::error::ValueError,
            "Expected " + toString(nArgumentsForIntegral(type)) + " arguments"
            " for integral of type " + toString(type) + ", " 
            + toString(t_argument.size()) + " given.");
    argument = t_argument;
}

FeynmanIntegral::FeynmanIntegral(IntegralType               t_type,
                                 int                        t_looptoolsId,
                                 std::vector<csl::Expr>   const &t_argument,
                                 std::vector<size_t> const &t_indices)
    :FeynmanIntegral(t_type, t_looptoolsId, t_argument)
{
    integralIndices = t_indices;
}

std::pair<csl::Expr, csl::Expr> FeynmanIntegral::getPair(size_t i) const
{
    return std::make_pair(
            argument[i], 
            argument[(argument.size()+1)/2 + i]
            );
}

bool FeynmanIntegral::compare(size_t i, size_t j) const
{
    auto A = getPair(i);
    auto B = getPair(j);
    if (A.first < B.first)
        return true;
    else if (!(B.first < A.first))
        return A.second < B.second;
    return false;
}

void FeynmanIntegral::sortArgument()
{
    if (type == IntegralType::A
            or type == IntegralType::B)
        return;
    const size_t n = argument.size() / 2;

    for (size_t i = 0; i != n; ++i) {
        size_t mini = i;
        for (size_t j = i+1; j < n; ++j) {
            if (compare(j, mini))
                mini = j;
        } 
        if (mini != i) {
            size_t offset = (argument.size()+1) / 2;
            std::swap(argument[i], argument[mini]);
            std::swap(argument[i+offset], argument[mini+offset]);
        }
    }
}

void FeynmanIntegral::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (lib) {
        printLib(mode, out);
        return;
    }
    if (loopToolsId != -1)
        printLooptoolsId(type, loopToolsId, out);
    else {
        out << type;
        for (size_t i = 0; i != integralIndices.size(); ++i) 
            if (integralIndices[i] == 0)
                out << '0';
            else {
                for (size_t j = integralIndices.size(); j --> i ;)
                    out << integralIndices[j];
                break;
            }

    }
    if (mty::option::displayIntegralArgs) {
        out << "(";
        for (size_t i = 0; i != argument.size(); ++i)  {
            out << argument[i];
            if (i+1 != argument.size())
                out << ", ";
        }
        out << ")";
    }
    if (mode == 0)
        out << '\n';
}

std::string FeynmanIntegral::printLaTeX(int) const
{
    std::ostringstream sout;
    printLooptoolsId(type, loopToolsId, sout);
    if (mty::option::displayIntegralArgs) {
        sout << "(";
        for (const auto& arg : argument)
            sout << arg << ", ";
        sout << ")";
    }

    return sout.str();
}

void FeynmanIntegral::printLib(int mode,
                               std::ostream& out) const
{
    // if (type == IntegralType::E and loopToolsId >= Nee) {
    //     auto integers = getRank5Indices(loopToolsId);
    //     out << "PJFry::E0v5(" ;
    //     for (int index : integers)
    //         out << index << ", ";
    // }
    // else {
    if (int(loopToolsId) == -1) {
        std::cerr << "Warning: Integral " << copy() << " set to 0 because not "
            << "supported.\n";
        out << "0";
        if (mode == 0)
            out << "\n";
        return;
    }
    out << "mty::lt::" << type << "0iC(" << int(loopToolsId) << ", ";
    // }
    for (size_t i = 0; i != argument.size(); ++i)  {
        argument[i]->print(mode, out, true);
        out << ", ";
    }
    out << "mty::lt::reg_int";
    out << ")";
    if (mode == 0)
        out << '\n';
}

csl::LibDependency FeynmanIntegral::getLibDependency() const
{
    csl::LibDependency dependencies;
    // if (type == IntegralType::E and loopToolsId >= Nee) {
    //     dependencies.addInclude("pjfry.h");
    //     dependencies.addLib("-lpjfry");
    //     dependencies.addLib("-lavh_olo");
    // }
    // else {
    dependencies.addInclude("marty/looptools_init.h");
    dependencies.addInclude("marty/looptools_interface.h");
    dependencies.addLib("-lgfortran");
    dependencies.addInclude("clooptools.h");
    if (csl::LibraryGenerator::isQuadruplePrecision()) {
        dependencies.addLib("-looptools-quad");
    }
    else {
        dependencies.addLib("-looptools");
    }
    // }

    return dependencies;
}

std::optional<csl::Expr> FeynmanIntegral::evaluate(
        csl::eval::mode user_mode) const
{
    if (not evaluateIntegrals) {
        csl::Expr copyExpr = copy();
        bool evaluated = false;
        for (size_t i = 0; i != argument.size(); ++i) {
            std::optional<csl::Expr> arg = argument[i]->evaluate(user_mode);
            if (arg) {
                copyExpr->setArgument(arg.value(), i);
                evaluated = true;
            }
        }
        if (evaluated)
            return copyExpr;
        return std::nullopt;
    }
    return complexToExpr(evaluateIntegral());
}

csl::unique_Expr FeynmanIntegral::copy_unique() const
{
    return std::make_unique<FeynmanIntegral>(
            type, loopToolsId, argument, integralIndices);
}

csl::Expr FeynmanIntegral::deepCopy() const
{
    std::vector<csl::Expr> copyArg(argument.size());
    for (size_t i = 0; i != argument.size(); ++i) 
        copyArg[i] = DeepCopy(argument[i]);

    return std::make_shared<FeynmanIntegral>(
            type, loopToolsId, copyArg, integralIndices);
}

csl::Expr FeynmanIntegral::deepRefresh() const
{
    return deepCopy();
}

bool FeynmanIntegral::operator==(csl::Expr_info other) const
{
    auto other_int = dynamic_cast<FeynmanIntegral const*>(other);
    if (!other_int)
        return false;
    if (type != other_int->type
            or loopToolsId != other_int->loopToolsId)
        return false;
    if (loopToolsId == -1)
        if (integralIndices != other_int->integralIndices)
            return false;

    for (size_t i = 0; i != argument.size(); ++i)
        if (!argument[i]->compareWithDummy(other_int->argument[i].get()))
            return false;

    return true;
}

bool FeynmanIntegral::operator<(csl::Expr_info other) const
{
    if (typeid(*other) != typeid(FeynmanIntegral))
        return AbstractMultiFunc::operator<(other);

    auto other_int = dynamic_cast<FeynmanIntegral const*>(other);
    if (type <  other_int->type)
        return true;
    else if (type > other_int->type)
        return false;
    if (loopToolsId < other_int->loopToolsId)
        return true;
    else if (loopToolsId > other_int->loopToolsId)
        return false;
    if (loopToolsId == -1) {
        if (integralIndices.size() < other_int->integralIndices.size())
            return true;
        else if (integralIndices.size() > other_int->integralIndices.size())
            return false;
        for (size_t i = 0; i != integralIndices.size(); ++i)
            if (integralIndices[i] < other_int->integralIndices[i])
                return true;
            else if (integralIndices[i] > other_int->integralIndices[i])
                return false;
    }
    return AbstractMultiFunc::operator<(other);
}

csl::Expr FeynmanIntegral::getDivergentFactor() const
{
    if (loopToolsId == -1) {
        return mty::FiniteFlag * getLocalTerm(
                type, integralIndices, getMomenta(), getMasses()
                );
    }
    return mty::FiniteFlag * mty::getDivergentFactor(
            type, loopToolsId, argument
            );
}

std::vector<csl::Expr> FeynmanIntegral::getMomenta() const
{
    switch(type) {
        case IntegralType::A: return {};
        case IntegralType::B: return {argument[0]};
        case IntegralType::C: return 
                              {
                                argument[0], 
                                argument[2],
                                argument[1]
                              };
        case IntegralType::D: return 
                              {
                                argument[0], 
                                argument[4],
                                argument[3]
                              };
        case IntegralType::E: return 
                              {
                                argument[0], 
                                argument[5],
                                argument[8],
                                argument[4]
                              };
        default: return {};
    }
}

std::vector<csl::Expr> FeynmanIntegral::getMasses() const
{
    switch(type) {
        case IntegralType::A: return {argument[0]};
        case IntegralType::B: return {argument[1], argument[2]};
        case IntegralType::C: return {argument[3], argument[4], argument[5]};
        case IntegralType::D: return {
                                      argument[6], 
                                      argument[7], 
                                      argument[8], 
                                      argument[9]
                                     };
        case IntegralType::E: return {
                                      argument[10], 
                                      argument[11], 
                                      argument[12], 
                                      argument[13],
                                      argument[14]
                                     };
        default: return {};
    }
}

ComplexType FeynmanIntegral::evaluateIntegral() const
{
    switch (type) {
        case IntegralType::A: return A0i_runtime(loopToolsId, argument);
        case IntegralType::B: return B0i_runtime(loopToolsId, argument);
        case IntegralType::C: return C0i_runtime(loopToolsId, argument);
        case IntegralType::D: return D0i_runtime(loopToolsId, argument);
        case IntegralType::E: return E0i_runtime(loopToolsId, argument);
            // else {
            //     // Rank 5 5-pt function
            //     std::array<int, 5> indices = getRank5Indices(loopToolsId);
            //     std::vector<double> doubleArg = exprToRealType(argument);
            //     return PJFry::E0v5(indices[0],
            //                        indices[1],
            //                        indices[2],
            //                        indices[3],
            //                        indices[4],
            //                        doubleArg[0],
            //                        doubleArg[1],
            //                        doubleArg[2],
            //                        doubleArg[3],
            //                        doubleArg[4],
            //                        doubleArg[5],
            //                        doubleArg[6],
            //                        doubleArg[7],
            //                        doubleArg[8],
            //                        doubleArg[9],
            //                        doubleArg[10],
            //                        doubleArg[11],
            //                        doubleArg[12],
            //                        doubleArg[13],
            //                        doubleArg[14]);
            // }
        default:
            return {1, 0};
    }
}

csl::Expr feynmanintegral_s(IntegralType               type,
                       int                        looptoolsId,
                       std::vector<csl::Expr>   const &argument,
                       std::vector<size_t> const &indices)
{
    return csl::make_shared<FeynmanIntegral>(
            type,
            (looptoolsId >= Nee and type != IntegralType::E) ? -1 : looptoolsId,
            argument,
            indices
            );
}

csl::Expr feynmanintegral_s(IntegralType             t_type,
                       int                      t_looptoolsId,
                       std::vector<csl::Expr> const& t_argument)
{
    return csl::make_shared<FeynmanIntegral>(t_type,
                                             t_looptoolsId,
                                             t_argument);
}

}
