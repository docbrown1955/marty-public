#include "vectorlike.h"
#include "lookparams.h"
#include <map>

using namespace mty;

auto cc(csl::Expr const &expr) { return csl::GetComplexConjugate(expr); }

enum class BosonFilter {
    None,
    SMExtension,
    NoBoson
};

void addBosonFilter(
        BosonFilter  filter,
        FeynOptions &options
        )
{
    switch (filter) {
        case BosonFilter::SMExtension:
            options.addFilter(filter::forceParticles({"h0", "G0", "Gp"}));
            break;
        case BosonFilter::NoBoson:
            options.addFilter(filter::disableParticles({"W", "Z", "A", "G"}));
            break;
        default:
            break;
    }
}

void calculateMixingCoefficients(
        Model       &model,
        Library     &lib,
        char const  *q1,
        char const  *q2,
        BosonFilter filter
        )
{
    FeynOptions options;
    options.setTopology(Topology::Box);
    options.setWilsonOperatorCoefficient(
            1 / (16 * M_PI*M_PI)
            );
    options.addFilter([&](FeynmanDiagram const &diagram) {
                return !(diagram.isInLoop("u") || diagram.isInLoop("c")
                        || diagram.isInLoop("t") || diagram.isInLoop("d")
                        || diagram.isInLoop("s") || diagram.isInLoop("b"));
            });
    addBosonFilter(filter, options);
    options.setFermionOrder({1, 0, 2, 3});
    auto wil = model.computeWilsonCoefficients(
            OneLoop,
            {Incoming(q1), Incoming(AntiPart(q2)),
            Outgoing(q2),  Outgoing(AntiPart(q1))},
            options
            );

    Display(wil);

    auto O_VLL = dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::VL);
    auto O_VRR = dimension6Operator(model, wil, DiracCoupling::VR, DiracCoupling::VR);
    // The factor 2 takes into account the operators with crossed color couplings (ij)(ji)
    // could be added also by taking the coefficients of operators with
    // ...DiracCoupling::VL, DiracCoupling::VL, {"SU3c", ColorCoupling::Crossed});
    auto C_VLL = 2*getWilsonCoefficient(wil, O_VLL);
    auto C_VRR = 2*getWilsonCoefficient(wil, O_VRR);

    auto O_VLR = dimension6Operator(model, wil, DiracCoupling::VL, DiracCoupling::VR);
    auto O_VRL = dimension6Operator(model, wil, DiracCoupling::VR, DiracCoupling::VL);
    // The factor 4 takes into account the factor 2 defined above and the two possible
    // operators (L)(R) and (R)(L). Otherwise, one can make the sum of C_VLR and C_VRL
    // directly. Both have been calculated (and multiplied by 2) for testing reasons
    auto C_VLR = 4*getWilsonCoefficient(wil, O_VLR);
    auto C_VRL = 4*getWilsonCoefficient(wil, O_VRL);

    Display(wil);

    std::cout << "Results:\n";
    std::cout << "C_VLL = " << C_VLL << '\n';
    std::cout << "C_VRR = " << C_VRR << '\n';
    std::cout << "C_VLR = " << C_VLR << '\n';
    std::cout << "C_VRL = " << C_VRL << '\n';

    std::cout << "Adding functions to the library" << '\n';
    lib.addFunction("Cmix_VLL", C_VLL);
    lib.addFunction("Cmix_VRR", C_VRR);
    lib.addFunction("Cmix_VLR", C_VLR);
    lib.addFunction("Cmix_VRL", C_VRL);
}

void undefineSMVariables()
{
    using namespace sm_input;
    for (auto &var : {e_em, V_ud, V_us, V_ub, delta_wolf,
            V_cd, V_cs, V_cb, V_td, V_ts, V_tb, m_u, m_d, m_s, m_c, m_b,
            m_t, theta_W, M_W, M_Z, g_s}) {
        var->setValue(CSL_UNDEF);
    }
}

int main() {

    vl::VectorLike_Model::Model model_params;
    model_params.model5  = 0;
    model_params.model6  = 1;
    model_params.model7  = 0;
    model_params.model8  = 0;
    model_params.model9  = 0;
    model_params.model10 = 0;
    model_params.model11 = 0;
    auto masses = vl::VectorLike_Model::getVLQMasses(model_params);
    vl::VectorLike_Model model(model_params);
    std::cout << model << '\n';
    Display(model.getFeynmanRules());
    undefineSMVariables();

    Library lib("testlib");
    lib.cleanExistingSources();
    lib.generateSpectrum(model);
    calculateMixingCoefficients(model, lib, "d", "s", BosonFilter::NoBoson);
    lib.print();

    vl::setupLibrary("testlib", masses);

    return 0;
}
