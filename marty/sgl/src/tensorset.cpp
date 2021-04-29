#include "tensorset.h"

namespace sgl {

    TensorSet buildTensorSet(
            csl::Space const *minkoSpace,
            csl::Space const *diracSpace
            )
    {
        TensorSet res;
        res.gamma_chir = csl::Tensor(
                "gamma5",
                {diracSpace, diracSpace}
                );
        res.P_L = csl::Tensor(
                "P_L",
                {diracSpace, diracSpace}
                );
        res.P_R = csl::Tensor(
                "P_R",
                {diracSpace, diracSpace}
                );
        res.C = csl::Tensor(
                "C",
                {diracSpace, diracSpace}
                );
        res.gamma[0] = diracSpace->getDelta();
        res.gamma[1] = csl::Tensor(
                "gamma",
                {minkoSpace, diracSpace, diracSpace}
                );
        res.gamma[2] = csl::Tensor(
                "sigma",
                {minkoSpace, minkoSpace, diracSpace, diracSpace}
                );
        return res;
    }

}
