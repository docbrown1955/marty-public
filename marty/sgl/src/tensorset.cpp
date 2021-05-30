// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

 #include "tensorset.h"

namespace sgl {

    TensorSet buildTensorSet(
            csl::Space const *minkoSpace,
            csl::Space const *diracSpace
            )
    {
        static std::map<
            std::pair<csl::Space const*, csl::Space const*>, 
            TensorSet
                > builtSets;
        auto pos = builtSets.find(std::make_pair(minkoSpace, diracSpace));
        if (pos != builtSets.end()) 
            return pos->second;
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
        builtSets[std::make_pair(minkoSpace, diracSpace)] = res;
        return res;
    }

}
