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

#include "marty.h"
#include "marty/models/sm.h"

using namespace csl;
using namespace mty;
using namespace std;
using namespace sm_input;

std::vector<size_t> concat(
        std::vector<std::vector<size_t>> const &vecs
        )
{
    std::vector<size_t> res;
    res.reserve(50);
    for (const auto& v : vecs)
        for (size_t s : v)
            res.push_back(s);
    return res;
}

std::vector<size_t> ranges(size_t mini, size_t maxi)
{
    std::vector<size_t> res(maxi - mini + 1);
    std::iota(res.begin(), res.end(), mini);
    return res;
}

std::vector<size_t> elements(std::initializer_list<size_t> l)
{
    return l;
}

void select(
        Amplitude &ampl,
        std::vector<size_t> const &indices
        )
{
    std::vector<FeynmanDiagram> diagrams;
    diagrams.reserve(ampl.size());
    for (size_t i : indices)
        diagrams.push_back(ampl.getDiagrams()[i]);
    ampl.getDiagrams() = diagrams;
}

csl::Expr getOperatorFactor()
{
    //auto expr = res.expressions[0];
    Expr V_ts_star = csl::GetComplexConjugate(sm_input::V_ts);
    Expr V_tb = sm_input::V_tb;
    Expr mb = sm_input::m_b;
    Expr ms = sm_input::m_s;
    Expr M_W = sm_input::M_W;
    Expr theta_W = sm_input::theta_W;
    Expr e_em = sm_input::e_em;
    Expr s2_theta_W = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr factorOperator = -csl::pow_s(e_em, 4) * V_ts_star * V_tb 
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2_theta_W);

    return factorOperator;
}

csl::Expr C1, C2, C3;

enum Process { Wtt, Gtt, tWW, tGG, tWG, tGW, bW, sW, bG, sG };

std::vector<size_t> indicesOf(Process p)
{
    switch (p) {
        case Wtt: return elements({6, 10, 62, 66});
        case Gtt: return elements({7,8,9, 11,12,13, 63,64,65, 67,68,69});
        case tWW: return elements({0, 1});
        case tGG: return ranges(34, 41);
        case tWG: return elements({4, 5});
        case tGW: return elements({2, 3});
        case bW: return elements({24, 29, 52, 57});
        case sW: return elements({14, 19, 42, 43});
        case bG: return concat({ranges(25,28), ranges(30,33), ranges(53,56), ranges(58,61)});
        case sG: return concat({ranges(15,18), ranges(20,23), ranges(44,51), ranges(53,56)});
    }
    return {};
}

void select(Amplitude &ampl, Process p)
{
    select(ampl, indicesOf(p));
}

auto getC9BZ(Model &model, mty::Library &lib)
{
    mty::option::keepOnlyFirstMassInLoop = true;
    FeynOptions options;
    options.addFilter(filter::disableParticle("A"));
    options.setWilsonOperatorCoefficient(getOperatorFactor());
    options.setFermionOrder({1, 0, 2, 3});
    auto C_bsmumu = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), 
            Outgoing("s"), 
            Outgoing("mu"),
            Outgoing(AntiPart("mu"))},
            options
            );

    Display(C_bsmumu);
    // Show(C_bsmumu);

    //options.setWilsonOperatorBasis(OperatorBasis::Standard);
    auto wil = C_bsmumu;
    csl::Expr C9 = getWilsonCoefficient(
            wil,
            dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::V)
            );

    Display(wil);
    C1 = C9;
    lib.addFunction("C9_BZ", C9);
}

auto getC9A(Model &model, mty::Library &lib)
{
    mty::option::keepOnlyFirstMassInLoop = false;
    FeynOptions options;
    options.addFilter(filter::forceParticle("A"));
    options.setWilsonOperatorCoefficient(getOperatorFactor());
    options.setFermionOrder({1, 0, 2, 3});
    auto C_bsmumu = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), 
            Outgoing("s"), 
            Outgoing("mu"),
            Outgoing(AntiPart("mu"))},
            options
            );

    Display(C_bsmumu);
    // Show(C_bsmumu);

    //options.setWilsonOperatorBasis(OperatorBasis::Standard);
    auto wil = C_bsmumu;

    mty::option::displayAbbreviations = false;
    Display(wil);
    auto C = dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::V);
    Display(WilsonSet{C.begin(), C.end()});
    csl::Expr C9 = getWilsonCoefficient(
            wil,
            dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::V)
            );

    Display(wil);
    C2 = C9;
    lib.addFunction("C9_A", C9);
}

auto getC9Tot(Model &model, mty::Library &lib)
{
    mty::option::keepOnlyFirstMassInLoop = false;
    FeynOptions options;
    options.setWilsonOperatorCoefficient(getOperatorFactor());
    options.setFermionOrder({1, 0, 2, 3});
    auto C_bsmumu = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), 
            Outgoing("s"), 
            Outgoing("mu"),
            Outgoing(AntiPart("mu"))},
            options
            );

    Display(C_bsmumu);
    // Show(C_bsmumu);

    //options.setWilsonOperatorBasis(OperatorBasis::Standard);
    auto wil = C_bsmumu;

    csl::Expr C9 = getWilsonCoefficient(
            wil,
            dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::V)
            );

    Display(wil);
    C3 = C9;
    lib.addFunction("C9_Tot", C9);
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);
    SM_Model SM;
    std::cout << SM << std::endl;

    using namespace sm_input;
    for (auto &value : {M_Z, M_W, theta_W, e_em, alpha_em, m_b, m_s, m_t, m_mu})
        value->setValue(CSL_UNDEF);

    SM.getParticle("h0")->setEnabledInDiagrams(false);
    SM.getParticle("u")->setEnabledInDiagrams(false);
    SM.getParticle("c")->setEnabledInDiagrams(false);


    Library::setQuadruplePrecision(true);
    Library lib("C9_SM");
    getC9BZ(SM, lib);
    getC9A(SM, lib);
    getC9Tot(SM, lib);
    lib.cleanExistingSources();
    lib.print();

    [[maybe_unused]] int sysres 
        = system("cp libscripts/example_c9_sm.cpp C9_SM/script");
    sysres = system("cd C9_SM; rm data.txt; make >/dev/null && bin/example_c9_sm.x > /dev/null");
    sysres = system("bin/comparedata.x data/C9_SM.txt C9_SM/data.txt 1e-5");

    // std::cout << "CBZ = " <<  csl::Evaluated(C1, csl::eval::abbreviation) << '\n';
    // std::cout << "CA = " <<   csl::Evaluated(C2, csl::eval::abbreviation) << '\n';
    // std::cout << "CTot = " << csl::Evaluated(C3, csl::eval::abbreviation) << '\n';

    return sysres;
}
