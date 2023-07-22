#include "comparedata.h"
#include "testutility.h"
#include "marty/models/sm.h"
#include <marty.h>

using namespace csl;
using namespace mty;
using namespace std;
using namespace sm_input;

std::vector<size_t> pos;

int calculate(Model &model, gauge::Type gauge)
{
    using namespace mty::sm_input;
    for (auto &expr : {e_em, m_b, m_t, m_s, theta_W, alpha_s, alpha_em, g_s})
        expr->setValue(CSL_UNDEF);
    model.getParticle("W")->setGaugeChoice(gauge);
    std::cout << "Searching possible vertices in contractions ..."
              << std::endl;
    model.computeFeynmanRules();
    Display(model.getFeynmanRules());

#ifndef MARTY_v1_2
    FeynOptions options;
    options.addFilters(mty::filter::forceParticle("t"));
    mty::option::keepOnlyFirstMassInLoop = false;
    auto res
        = model.computeAmplitude(Order::OneLoop,
                                 {Incoming("b"), Outgoing("s"), Outgoing("G")},
                                 options);
#else
    DisableParticle("u");
    DisableParticle("c");
    auto res = model.computeAmplitude(
        Order::OneLoop, {Incoming("b"), Outgoing("s"), Outgoing("G")});
#endif
    Display(res);

    Expr V_ts_star      = csl::GetComplexConjugate(sm_input::V_ts);
    Expr s2_theta_W     = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr factorOperator = -csl::pow_s(e_em, 2) * g_s * V_ts_star * V_tb * m_b
                          / (32 * CSL_PI * CSL_PI * M_W * M_W * s2_theta_W);

    options.setWilsonOperatorCoefficient(factorOperator);
    auto wilsonC8 = model.getWilsonCoefficients(res, options);
    Display(wilsonC8);

    Expr CC8 = getWilsonCoefficient(
        wilsonC8, chromoMagneticOperator(model, wilsonC8, DiracCoupling::R));
    Expr CC8p = getWilsonCoefficient(
        wilsonC8, chromoMagneticOperator(model, wilsonC8, DiracCoupling::L));
    std::cout << CC8 << std::endl;
    std::cout << CC8p << std::endl;

    [[maybe_unused]] int sysres = system("rm -rf libs/C8_SM");
    csl::LibraryGenerator::setQuadruplePrecision(false);
    mty::Library wilsonLib("C8_SM", "libs");
    wilsonLib.addFunction("C8", CC8);
    wilsonLib.addFunction("C8_p", CC8p);
    defineLibPath(wilsonLib);
    wilsonLib.print();

    sysres = system("cp libsrc/example_c8_sm.cpp libs/C8_SM/script");
    sysres = system("cd libs/C8_SM; make && bin/example_c8_sm.x");
    return assert_equal("data/output/C8_SM.txt", "libs/C8_SM/C8_SM.txt", 1e-5);
}

int main()
{
    mty::sm_input::redefineNumericalValues(); // for compatibility
    SM_Model sm;
    return calculate(sm, gauge::Type::Unitary);
}
