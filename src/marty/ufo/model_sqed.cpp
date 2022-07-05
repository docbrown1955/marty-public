#include "model_sqed.h"

namespace mty::experimental::ufo
{

    Model_SQED::Model_SQED(bool do_initialize)
    {
        if (do_initialize)
        {
            initialize();
        }
    }

    void Model_SQED::initialize()
    {
        // Gauge initialization
        addGaugedGroup(group::Type::U1, "em");
        Model::init();

        renameParticle("A_em", "A");

        // Scalar definition, charge +1 for the "em" group
        Particle phi = scalarboson_s("phi ; \\phi", *this);
        phi->setGroupRep("em", 1);
        addParticle(phi);
    }

} // namespace mty::experimental::ufo
