#include <marty.h>
#include "pmssm_lem.h"

using namespace std;
using namespace csl;
using namespace mty;


int main() {

    PMSSM_LEM pmssm_model;
    std::cout << pmssm_model << std::endl;

    Display(pmssm_model.getFeynmanRules());

    auto res = pmssm_model.computeAmplitude(
            Order::TreeLevel,
            {Incoming("C_1"), Outgoing("C_1"), Outgoing("A")}
            );
    for (auto &r : res.expressions)
        csl::Evaluate(r, csl::eval::abbreviation);
    Display(res);
}
