#include <marty>

using namespace csl;
using namespace mty;
using namespace std;
using namespace sm_input;

int main() {

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between 
    // 1 and 4
    ///////////////////////////////////////////////////

    const int type = 2;

    ///////////////////////////////////////////////////
    // Load the model in 2HDM.json
    ///////////////////////////////////////////////////

    TwoHDM_Model<type> _2HDM;
    std::cout << _2HDM << std::endl;

    auto rules = ComputeFeynmanRules(_2HDM);
    Display(rules);
    std::cin.get();
    // Show(rules);

    std::cout << "The model is now complete." << std::endl;
    std::cout << "Press enter to launch the computation of ";
    std::cout << "b->cdu^* wilson coefficients at tree-level" << std::endl;
    // std::cin.get();

    EffModel Eff_2HDM(_2HDM, {"W"});
    Particle b = GetParticle(_2HDM, "b");
    Particle c = GetParticle(_2HDM, "c");
    Particle u = GetParticle(_2HDM, "u");
    Particle d = GetParticle(_2HDM, "d");

    Timer timer;
    Particle bL = GetParticle(_2HDM, "b");
    Particle sL = GetParticle(_2HDM, "s");
    Particle muL = GetParticle(_2HDM, "\\mu_L");
    Particle photon = GetParticle(_2HDM, "A");
    Particle W = GetParticle(_2HDM, "W");
    Particle gluon = GetParticle(_2HDM, "g");
    GetParticle(_2HDM, "H^+")->setMass(csl::constant_s("m_Hp"));
    GetParticle(_2HDM, "G^+")->setMass(GetParticle(_2HDM, "W")->getMass());
    std::cout << "Searching possible vertices in contractions ..." << std::endl;
    DisableParticle(_2HDM, "u");
    DisableParticle(_2HDM, "c");
    // DisableParticle(_2HDM, "W");
    // DisableParticle(_2HDM, "G^+");
    DisableParticle(_2HDM, "H^+");
    // mty::option::applyEquationsOfMotion = false;
    // mty::option::simplifyAmplitudes     = false;
    mty::option::decomposeInLocalOperator = false;
    mty::option::decomposeInOperators     = true;
    mty::option::applyFermionChain = false;

    auto res = ComputeAmplitude(
                Order::OneLoop,
                _2HDM,
                {Incoming("b"), 
                Outgoing("s"), 
                Outgoing("\\mu"),
                Outgoing(AntiPart("\\mu"))});

    // Expr squared = ComputeSquaredAmplitude(_2HDM, res);

    Display(res);
    Show(res);

    csl::Expr V_ts_star = csl::GetComplexConjugate(sm_input::V_ts);
    csl::Expr V_tb = sm_input::V_tb;
    csl::Expr mb = b->getMass();
    csl::Expr ms = GetParticle(_2HDM, "s")->getMass();
    csl::Expr M_W = sm_input::M_W;
    csl::Expr theta_W = sm_input::theta_W;
    csl::Expr e_em = sm_input::e;
    csl::Expr s2_theta_W = csl::pow_s(csl::sin_s(theta_W), 2);
    csl::Expr factorOperator = -csl::pow_s(e_em, 3) * V_ts_star * V_tb * mb
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2_theta_W);

    return 0;
}
