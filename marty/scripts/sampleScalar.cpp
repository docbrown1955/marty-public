#include <marty>

using namespace csl;
using namespace mty;

int main() {

/******* Field and model definitions **********/
    Model p3Model; 
    Particle phi = scalarboson_s("\\phi", p3Model); 
    SetMass(phi, "m");
    SetSelfConjugate(phi, true);
    AddParticle(p3Model, phi);
                                 
/******* phi^3 term definition       **********/
    Tensor X = MinkowskiVector("X");                
    csl::Expr lambda = constant_s("\\lambda");            
    AddTerm(p3Model, -lambda/6 * pow_s(phi(X), 3));

    std::cout << p3Model << std::endl;

    auto rules = ComputeFeynmanRules(p3Model);
    Display(rules);

/******* Computation                 **********/
    mty::option::computeFirstIntegral = false;
    auto res = p3Model.computeAmplitude(
            Order::OneLoop,
            { Incoming(phi(X)), Outgoing(phi(X)), Outgoing(phi(X)) });

/******* Displaying results          **********/
    Display(res);
    Show(res);

    return 0;
}
