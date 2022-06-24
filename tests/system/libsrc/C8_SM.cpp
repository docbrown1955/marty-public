#include <marty.h>

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
    DisableParticle(_2HDM, GetParticle(_2HDM, "H^+"));
    GetParticle(_2HDM, "G^+")->setMass(GetParticle(_2HDM, "W")->getMass());
    GetParticle(_2HDM, "W")->setGaugeChoice(gauge::Type::Unitary);
    std::cout << "Searching possible vertices in contractions ..." << std::endl;
    DisableParticle(_2HDM, "u");
    DisableParticle(_2HDM, "c");

    auto res = ComputeAmplitude(
                Order::OneLoop,
                _2HDM,
                {Incoming(bL), 
                Outgoing(sL), 
                Outgoing(gluon)});
    // Show(res);

    Expr V_ts_star = csl::GetComplexConjugate(sm_input::V_ts);
    Expr V_tb = sm_input::V_tb;
    Expr mb = b->getMass();
    Expr ms = GetParticle(_2HDM, "s")->getMass();
    Expr M_W = sm_input::M_W;
    Expr theta_W = csl::GetSymbol("theta_W", csl::Evaluated(res.expressions[0], csl::eval::abbreviation));
    Expr gs = csl::GetSymbol("g_s", csl::Evaluated(res.expressions[0], csl::eval::abbreviation));
    Expr e_em = csl::GetSymbol("e_em", csl::Evaluated(res.expressions[0], csl::eval::abbreviation));
    Expr s2_theta_W = csl::pow_s(csl::sin_s(theta_W), 2);
    Expr factorOperator = -csl::pow_s(e_em, 2) * gs * V_ts_star * V_tb * mb
        / (32 * CSL_PI*CSL_PI * M_W*M_W * s2_theta_W);

    auto wilsonC8 = _2HDM.getWilsonCoefficients(res, factorOperator);

    Expr CC8   = OperatorParser::getMagneticCoefficient(wilsonC8, Chirality::Right);
    Expr CC8p  = OperatorParser::getMagneticCoefficient(wilsonC8, Chirality::Left);

    csl::Library::setQuadruplePrecision(false);
    mty::Library wilsonLib("C8_SM", ".", false);
    wilsonLib.addFunction("C8", CC8);
    wilsonLib.addFunction("C8_p", CC8p);
    wilsonLib.build();

    return 0;
}
