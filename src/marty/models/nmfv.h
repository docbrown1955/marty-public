#pragma once

#include "pmssm.h"

namespace mty {

class NMFV_Model : public PMSSM_Model {

  public:
    enum class Mixing { QL, UR, DR, LL, ER, Au, Ad, Ae };

    struct MixingInfo {
        Mixing              mixing;
        std::vector<size_t> mixedParticles;
    };

    NMFV_Model(std::vector<MixingInfo> const &mixings,
               std::string const             &saveFile = "");

    NMFV_Model(std::initializer_list<MixingInfo> mixings,
               std::string const                &saveFile = "")
        : NMFV_Model(std::vector<MixingInfo>(mixings), saveFile)
    {
    }

    void getToLowEnergyLagrangian();

    void addPairMixing(Mixing mix, size_t i, size_t j);

    void addMixing(MixingInfo mix);
};

std::ostream &operator<<(std::ostream &out, NMFV_Model::Mixing mixing);

} // namespace mty
