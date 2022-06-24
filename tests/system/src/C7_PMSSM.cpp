#include "comparedata.h"
#include "marty/models/pmssm_lem.h"
#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

int main()
{

    mty::sm_input::M_W->setValue(CSL_UNDEF);
    mty::sm_input::m_b->setValue(CSL_UNDEF);
    mty::sm_input::m_s->setValue(CSL_UNDEF);
    mty::sm_input::m_t->setValue(CSL_UNDEF);

    auto pmssm_model = mty::PMSSM_LEM();

    ComputeFeynmanRules(pmssm_model);
    Display(pmssm_model.getFeynmanRules());

    mty::option::keepOnlyFirstMassInLoop = false;
    mty::option::decomposeInOperators    = false;

    FeynOptions options;
    options.setTopology(Topology::Triangle);
    options.addFilters(mty::filter::forceParticles({"C_1", "C_2"}),
                       mty::filter::forceParticles({"st_1", "st_2"}),
                       mty::filter::disableParticles({"W", "Gp", "Hp"}));
    auto res = pmssm_model.computeAmplitude(
        Order::OneLoop,
        {Incoming("b"), Outgoing("s"), Outgoing("A")},
        options);

    Expr V_ts_star      = csl::GetComplexConjugate(sm_input::V_ts);
    Expr V_tb           = sm_input::V_tb;
    Expr mb             = GetParticle("b_L")->getMass();
    Expr ms             = GetParticle("s_L")->getMass();
    Expr M_W            = sm_input::M_W;
    Expr e_em           = sm_input::e_em;
    Expr s2_theta_W     = sm_input::s2_theta_W;
    Expr factorOperator = -csl::pow_s(e_em, 3) * V_ts_star * V_tb * mb
                          / (32 * CSL_PI * CSL_PI * M_W * M_W * s2_theta_W);

    options.setWilsonOperatorCoefficient(factorOperator);
    auto wilsonC7 = pmssm_model.getWilsonCoefficients(res, options);
    Display(wilsonC7);
    DisplayAbbreviations();

    Expr CC7 = getWilsonCoefficient(
        wilsonC7,
        chromoMagneticOperator(pmssm_model, wilsonC7, DiracCoupling::R));
    Expr CC7p = getWilsonCoefficient(
        wilsonC7,
        chromoMagneticOperator(pmssm_model, wilsonC7, DiracCoupling::L));

    [[maybe_unused]] int sysres = system("rm -r libs/C7_PMSSM");
    mty::Library::setQuadruplePrecision(true);
    mty::Library wilsonLib("C7_PMSSM", "libs");
    wilsonLib.addFunction("C7", CC7);
    wilsonLib.addFunction("C7_p", CC7p);
    wilsonLib.generateSpectrum(pmssm_model);
    wilsonLib.print();

    sysres = system("cp libsrc/example_c7_pmssm.cpp libs/C7_PMSSM/script");
    sysres = system("cd libs/C7_PMSSM; make && bin/example_c7_pmssm.x");
    return assert_equal("data/output/C7_marty_tanb50_ss.txt",
                        "libs/C7_PMSSM/C7_marty_tanb50_ss.txt",
                        1e-5);
}
