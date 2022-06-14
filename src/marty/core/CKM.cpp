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

#include "CKM.h"
#include "SM.h"
namespace mty {

csl::Tensor buildCKM(const csl::Space* flavorSpace) {

    using namespace sm_input;
    csl::Expr CKM_matrix = csl::matrix_s({{V_ud, V_us, V_ub},
                                   {V_cd, V_cs, V_cb},
                                   {V_td, V_ts, V_tb}});

    V_CKM = csl::Unitary("V_CKM", flavorSpace);
    V_CKM->setTensor(CKM_matrix);
    return V_CKM;
}

} // End of namespace mty
