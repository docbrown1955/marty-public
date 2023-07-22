#include "comparedata.h"
#include "testutility.h"
#include "marty.h"
#include "marty/models/sm.h"

using namespace mty;

auto cc(csl::Expr const &expr)
{
    return GetComplexConjugate(expr);
}

std::pair<csl::Expr, csl::Expr> calculate(Model &model)
{
    using namespace sm_input;
    FeynOptions options;
    options.setTopology(Topology::Box);
    options.addFilters(filter::disableParticles({"u", "c"}));
    options.setFermionOrder({1, 0, 2, 3});
    auto GF
        = csl::sqrt_s(2) * csl::pow_s(e_em / (M_W * sin_s(theta_W)), 2) / 8;
    options.setWilsonOperatorCoefficient(
        pow_s(GF * V_tb * cc(V_ts) / (4 * CSL_PI), 2));
    auto wil = model.computeWilsonCoefficients(OneLoop,
                                               {Incoming("b"),
                                                Incoming(AntiPart("s")),
                                                Outgoing("s"),
                                                Outgoing(AntiPart("b"))},
                                               options);
    Display(wil);

    auto O_VLL
        = dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::VL);
    csl::Expr C1 = getWilsonCoefficient(wil, O_VLL);
    csl::Expr C2 = getWilsonCoefficient(
        wil,
        dimension6Operator(model,
                           wil,
                           DiracCoupling::VL,
                           DiracCoupling::VL,
                           {"C", ColorCoupling::Crossed}));

    return {C1, C2};
}

int main()
{

    mty::sm_input::redefineNumericalValues(); // for compatibility
    std::cout.setstate(std::ios_base::failbit);
    std::cerr.setstate(std::ios_base::failbit);

    mty::option::displayAbbreviations    = false;
    mty::option::keepOnlyFirstMassInLoop = true;

    SM_Model sm;
    sm_input::m_t->setValue(CSL_UNDEF);

    auto [C1, C2] = calculate(sm);

    [[maybe_unused]] int sysres = system("rm -rf libs/BsMixingSM");
    mty::Library         lib("BsMixingSM", "libs");
    lib.addFunction("C1", C1);
    lib.addFunction("C2", C2);
    defineLibPath(lib);
    lib.print();

    sysres = system("cp libsrc/example_bsmixingsm.cpp libs/BsMixingSM/script");
    sysres = system("cd libs/BsMixingSM; make && bin/example_bsmixingsm.x");
    return assert_equal(
        "data/output/BsMixingSM.txt", "libs/BsMixingSM/data.txt", 1e-8);
}
