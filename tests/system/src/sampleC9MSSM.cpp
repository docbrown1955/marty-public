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
#include "marty/models/pmssm_lem.h"

using namespace csl;
using namespace mty;
using namespace std;
using namespace sm_input;

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

std::initializer_list<std::string_view> forced 
    = {"st_1", "st_2", "su_L", "su_R", "sc_L", "sc_R"};

auto getC9Tot(Model &model, mty::Library &lib)
{
    FeynOptions options;
    options.addFilter(filter::forceParticles(forced));
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

    csl::Expr C9 = getWilsonCoefficient(
            C_bsmumu,
            dimension6Operator(model, C_bsmumu, DiracCoupling::VL, DiracCoupling::V)
            );
    csl::Expr C10 = getWilsonCoefficient(
            C_bsmumu,
            dimension6Operator(model, C_bsmumu, DiracCoupling::VL, DiracCoupling::A)
            );

    Display(C_bsmumu);
    std::cout << C9 << '\n';
    std::cout << C10 << '\n';
    lib.addFunction("C9_Tot", C9);
    lib.addFunction("C10_Tot", C10);
}


auto getC9_easy(std::string const &name, int topology, Model &model, mty::Library &lib)
{
    FeynOptions options;
    options.addFilter(filter::disableParticle("A"));
    if (name == "C9_B")
        options.addFilter(filter::disableParticle("Z"));
    options.addFilter(filter::forceParticles(forced));
    options.setWilsonOperatorCoefficient(getOperatorFactor());
    options.setTopology(topology);
    options.setFermionOrder({1, 0, 2, 3});
    auto C_bsmumu = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), 
            Outgoing("s"), 
            Outgoing("mu"),
            Outgoing(AntiPart("mu"))},
            options
            );

    csl::Expr C9 = getWilsonCoefficient(
            C_bsmumu,
            dimension6Operator(model, C_bsmumu, DiracCoupling::VL, DiracCoupling::V)
            );

    DisplayAbbreviations();
    Display(C_bsmumu);
    std::cout << name << " = " << C9 << '\n';
    lib.addFunction(name, C9);
}

auto getC9BZ(Model &model, mty::Library &lib)
{
    getC9_easy("C9_Z", Topology::Triangle | Topology::Mass, model, lib);
    getC9_easy("C9_B", Topology::Box, model, lib);
}

auto getC9A_old(Model &model, mty::Library &lib)
{
    FeynOptions options;
    mty::option::keepOnlyFirstMassInLoop = false;
    options.addFilter(filter::disableParticle("Z"));
    options.addFilter(filter::forceParticles(forced));
    options.setWilsonOperatorCoefficient(getOperatorFactor());
    // options.setWilsonOperatorBasis(OperatorBasis::Chiral);
    options.setTopology(Topology::Triangle);
    //options.setFermionOrder({1, 0, 2, 3});
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
    
    //Show(C_bsmumu);
    std::cout << "Photon contrib" << '\n';

    auto wil = C_bsmumu;
    Display(wil);

    csl::Expr C9 = getWilsonCoefficient(
            wil,
            dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::V)
            );
    lib.addFunction("C9_A", C9);
}

int main() {

    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);
    PMSSM_LEM SM;

    mty::option::displayAbbreviations = false;
    using namespace sm_input;
    for (auto &value : {M_Z, M_W, theta_W, e_em, alpha_em, m_b, m_s, m_t, m_mu,
            V_ud, V_us, V_ub, V_ub_mod, delta_wolf, V_cd, V_cb, V_cs, V_td, V_ts, V_tb})
        value->setValue(CSL_UNDEF);

    SM.getParticle("h0")->setEnabledInDiagrams(false);
    SM.getParticle("H0")->setEnabledInDiagrams(false);
    SM.getParticle("Hp")->setEnabledInDiagrams(false);
    SM.getParticle("W")->setEnabledInDiagrams(false);
    SM.getParticle("Gp")->setEnabledInDiagrams(false);
    SM.getParticle("A0")->setEnabledInDiagrams(false);


    Library::setQuadruplePrecision(true);
    Library lib("C9_MSSM");
    lib.generateSpectrum(SM);
    lib.print();
    //getC9A(SM, lib);
    getC9A_old(SM, lib);
    getC9BZ(SM, lib);
    //getC9_Zfull(SM, lib);
    getC9Tot(SM, lib);
    //lib.cleanExistingSources();
    lib.print();

    [[maybe_unused]] int sysres 
        = system("cp libscripts/example_c9bis_mssm.cpp C9_MSSM/script");
    sysres = system("cd C9_MSSM; rm C9_marty_tanb50_ss.txt");
    sysres = system("cp libscripts/example_c9bis_mssm.cpp C9_MSSM/script");
    sysres = system("cp libscripts/kinematics.h C9_MSSM/include");
    sysres = system("cp libscripts/read_spectrum.h C9_MSSM/include");
    sysres = system("cp libscripts/lha*.h C9_MSSM/include");
    sysres = system("cp libscripts/lha*.cpp C9_MSSM/src");
    sysres = system("cp libscripts/ckm.h C9_MSSM/include");
    sysres = system("cp libscripts/ckm.cpp C9_MSSM/src");
    sysres = system("cd C9_MSSM; sed -i 's/-O2/-O1/g' Makefile");
    sysres = system("cd C9_MSSM; make clean; make -j 4 >/dev/null && "
                    " bin/example_c9bis_mssm.x > /dev/null");
    sysres = system("bin/comparedata.x data/C9_marty_tanb50_ss.txt "
                    "C9_MSSM/C9_marty_tanb50_ss.txt 1e-5");

    return sysres;
}
