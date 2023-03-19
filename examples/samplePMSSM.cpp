/*
 * pMSSM example, simply loads the model and compute the Feynman rules.
 */
#include <marty.h>
#include <marty/models/pmssm.h>

using namespace std;
using namespace csl;
using namespace mty;

int main()
{

    PMSSM_Model pmssm_model;
    // Or (directly the full Lagrangian without model building)
    // PMSSM_LEM pmssm_model;
    std::cout << pmssm_model << std::endl;

    Display(pmssm_model.getFeynmanRules());
}
