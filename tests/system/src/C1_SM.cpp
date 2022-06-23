#include "marty.h"
#include "marty/models/sm.h"

using namespace csl;
using namespace mty;
using namespace sm_input;

int main() {

    SM_Model sm;

    FeynOptions options;
    options.addFilter(filter::forceParticle("G"));
    options.setWilsonOperatorCoefficient(
            -4 * (e_em*e_em / (8 * pow_s(M_W*sin_s(theta_W), 2))) 
                * V_cb * GetComplexConjugate(V_cs)
            );
    auto wil = sm.computeWilsonCoefficients(
            OneLoop,
            {Incoming("b"), Outgoing("c"), Outgoing(AntiPart("c")), Outgoing("s")},
            options
            );
    Display(wil);

    csl::Expr CLL = getWilsonCoefficient(
            wil,
            dimension6Operator(sm, wil, DiracCoupling::VL, DiracCoupling::VL)
            );

    std::cout << CLL << '\n';

    Library lib("C1_SM");
    lib.addFunction("CLL", CLL);
    lib.print();

    return 0;
}
