#pragma once

#include "abstractgammasym.h"

namespace sgl {

    struct TensorSet {
        csl::Tensor gamma_chir;
        csl::Tensor C;
        csl::Tensor P_L;
        csl::Tensor P_R;
        std::map<size_t, csl::Tensor> gamma;
    };

    TensorSet buildTensorSet(
            csl::Space const *minkoSpace,
            csl::Space const *diracSpace
            );
}
