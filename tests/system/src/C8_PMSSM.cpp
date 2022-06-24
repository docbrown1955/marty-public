#include "comparedata.h"
#include "marty/models/pmssm_lem.h"
#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

int calculate(Model &model)
{
    Expr V_ts_star      = csl::GetComplexConjugate(sm_input::V_ts);
    Expr V_tb           = sm_input::V_tb;
    Expr mb             = sm_input::m_b;
    Expr ms             = sm_input::m_s;
    Expr M_W            = sm_input::M_W;
    Expr e_em           = sm_input::e_em;
    Expr g_s            = sm_input::g_s;
    Expr s2_theta_W     = sm_input::s2_theta_W;
    Expr factorOperator = -csl::pow_s(e_em, 2) * g_s * V_ts_star * V_tb * mb
                          / (32 * CSL_PI * CSL_PI * M_W * M_W * s2_theta_W);

    Display(model.getFeynmanRules());

    mty::sm_input::M_W->setValue(CSL_UNDEF);
    mty::sm_input::m_b->setValue(CSL_UNDEF);
    mty::sm_input::m_s->setValue(CSL_UNDEF);
    mty::sm_input::m_t->setValue(CSL_UNDEF);

    mty::option::decomposeInOperators    = false;
    mty::option::displayAbbreviations    = false;
    mty::option::keepOnlyFirstMassInLoop = false;
    FeynOptions options;
    options.setWilsonOperatorCoefficient(factorOperator);
    options.addFilters(mty::filter::forceParticles({"C_1", "C_2"}),
                       mty::filter::forceParticles({"st_1", "st_2"}),
                       mty::filter::disableParticles({"W", "Gp", "Hp"}));
    auto res
        = model.computeAmplitude(Order::OneLoop,
                                 {Incoming("b"), Outgoing("s"), Outgoing("G")},
                                 options);

    DisplayAbbreviations();
    Display(res);
    std::cout << e_em << '\n';
    std::cout << g_s << '\n';
    std::cout << V_ts_star << '\n';
    std::cout << V_tb << '\n';
    std::cout << mb << '\n';
    std::cout << "FACTOR = " << factorOperator << '\n';

    auto wilsonC8 = model.getWilsonCoefficients(res);
    Display(wilsonC8);

    Expr CC8 = getWilsonCoefficient(
        wilsonC8, chromoMagneticOperator(model, wilsonC8, DiracCoupling::R));
    Expr CC8p = getWilsonCoefficient(
        wilsonC8, chromoMagneticOperator(model, wilsonC8, DiracCoupling::L));
    std::cout << CC8 << std::endl;
    std::cout << CC8p << std::endl;
    Display(csl::DeepExpanded(csl::Evaluated(CC8, csl::eval::abbreviation))
                ->getVectorArgument());

    [[maybe_unused]] int sysres = system("rm -rf libs/C8_PMSSM");
    mty::Library::setQuadruplePrecision(true);
    mty::Library wilsonLib("C8_PMSSM", "libs");
    wilsonLib.addFunction("C8", CC8);
    wilsonLib.addFunction("C8_p", CC8p);
    wilsonLib.generateSpectrum(model);
    wilsonLib.print();

    sysres = system("cp libsrc/example_c8_pmssm.cpp libs/C8_PMSSM/script");
    sysres = system("cd libs/C8_PMSSM; make && bin/example_c8_pmssm.x");
    return assert_equal("data/output/C8_marty_tanb50_ss.txt",
                        "libs/C8_PMSSM/C8_marty_tanb50_ss.txt",
                        1e-5);
}

int main()
{

    auto pmssm_model = mty::PMSSM_LEM();

    return calculate(pmssm_model);
}
