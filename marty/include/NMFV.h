#pragma once

#include "PMSSM.h"

namespace mty {

    class NMFV_Model: public PMSSM_Model {

    public:

        enum class Mixing {
            QL, UR, DR, LL, ER, Au, Ad, Ae
        };

        struct MixingInfo {
            Mixing mixing;
            size_t i;
            size_t j;
        };

        NMFV_Model(
                std::vector<MixingInfo> const &mixings,
                std::string const &slhaFile = "",
                std::string const &saveFile = ""
                );

        NMFV_Model(
                std::initializer_list<MixingInfo> mixings,
                std::string const &slhaFile = "",
                std::string const &saveFile = ""
                )
            :NMFV_Model(
                    std::vector<MixingInfo>(mixings),
                    slhaFile,
                    saveFile
                    )
        {

        }

        void addMixing(MixingInfo mix);
    };

    std::ostream &operator<<(
            std::ostream      &out,
            NMFV_Model::Mixing mixing
            );

}
