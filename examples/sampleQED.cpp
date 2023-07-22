/*
 * Simple QED model
 *
 * Create a QED model with a single electron (Dirac fermion) of charge -1
 * and compute several amplitudes in this model.
 */
#include <marty.h>
#include <marty/models/qed.h>

using namespace csl;
using namespace mty;

int main()
{
    QED_Model qed;
    std::cout << qed << std::endl;

    auto rules = ComputeFeynmanRules(qed);
    Display(rules);
    Show(rules);

    auto compton = qed.computeAmplitude(Order::TreeLevel,
                                        {Incoming("e"),
                                         Incoming("A"),
                                         Outgoing("e"),
                                         Outgoing("A")});
    Display(compton);
    Show(compton);

    auto diff = qed.computeAmplitude(Order::TreeLevel,
                                     {Incoming("e"),
                                      Incoming("e"),
                                      Outgoing("e"),
                                      Outgoing("e")});
    Display(diff);
    Show(diff);
    DisplayAbbreviations();

    return 0;
}
