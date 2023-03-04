#include "comparedata.h"
#include "testutility.h"
#include "marty/models/sm.h"
#include <marty.h>


using namespace csl;
using namespace mty;
using namespace std;
using namespace sm_input;

int calculate(Model &model, gauge::Type gauge)
{
    using namespace mty::sm_input;
    for (auto &expr : {e_em, m_b, m_t, m_s, theta_W, alpha_s, alpha_em, g_s})
        expr->setValue(CSL_UNDEF);

    model.getParticle("W")->setGaugeChoice(gauge);

    mty::FeynOptions options;
    options.addFilters(mty::filter::forceParticle("t"));
    auto res = model.computeAmplitude(
        OneLoop, {Incoming("b"), Outgoing("s"), Outgoing("A")}, options);
    Display(res);

    Expr V_ts_star      = csl::GetComplexConjugate(sm_input::V_ts);
    Expr s2_theta_W     = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr factorOperator = -csl::pow_s(e_em, 3) * V_ts_star * V_tb * m_b
                          / (32 * CSL_PI * CSL_PI * M_W * M_W * s2_theta_W);

    options.setWilsonOperatorCoefficient(factorOperator);
    auto wilsonC7 = model.getWilsonCoefficients(res, options);
    Display(wilsonC7);
    Expr CC7 = getWilsonCoefficient(
        wilsonC7, chromoMagneticOperator(model, wilsonC7, DiracCoupling::R));
    Expr CC7p = getWilsonCoefficient(
        wilsonC7, chromoMagneticOperator(model, wilsonC7, DiracCoupling::L));
    std::cout << CC7 << std::endl;
    std::cout << CC7p << '\n';

    [[maybe_unused]] int sysres = system("rm -rf libs/C7_SM");
    mty::Library         wilsonLib("C7_SM", "libs");
    wilsonLib.cleanExistingSources();
    wilsonLib.addFunction("C7", CC7);
    wilsonLib.addFunction("C7_p", CC7p);
    defineLibPath(wilsonLib);
    wilsonLib.print();

    sysres = system("cp libsrc/example_c7_sm.cpp libs/C7_SM/script");
    sysres = system("cd libs/C7_SM; make && bin/example_c7_sm.x");
    std::cout.clear();
    return assert_equal("data/output/C7_SM.txt",
                        "libs/C7_SM/C7_SM.txt",
                        (gauge == gauge::Unitary) ? 1e-3 : 1e-5);
}

int main()
{

    SM_Model sm;
    std::cout << sm << std::endl;
    sm.computeFeynmanRules();
    Display(sm.getFeynmanRules());

    // calculate(sm, gauge::Type::Unitary);
    // calculate(sm, gauge::Type::Lorenz);
    return calculate(sm, gauge::Type::Feynman);
}
