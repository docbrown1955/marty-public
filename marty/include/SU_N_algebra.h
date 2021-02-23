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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#ifndef SU_N_ALGEBRA_H_INCLUDED
#define SU_N_ALGEBRA_H_INCLUDED

#include <csl.h>

namespace pauli {

inline
csl::Expr sigma1 = csl::matrix_s({{CSL_0, CSL_1},
                           {CSL_1, CSL_0}});

inline
csl::Expr sigma2 = csl::matrix_s({{CSL_0, -CSL_I},
                           {CSL_I, CSL_0}});

inline
csl::Expr sigma3 = csl::matrix_s({{CSL_1, CSL_0},
                           {CSL_0, -CSL_1}});

inline
csl::Expr f = csl::highdtensor_s({{{CSL_0, CSL_0, CSL_0},
                            {CSL_0, CSL_0, CSL_1},
                            {CSL_0, -CSL_1, CSL_0}},

                           {{CSL_0, CSL_0, -CSL_1},
                            {CSL_0, CSL_0, CSL_0},
                            {CSL_1, CSL_0, CSL_0}},

                           {{CSL_0, CSL_1, CSL_0},
                            {-CSL_1, CSL_0, CSL_0},
                            {CSL_0, CSL_0, CSL_0}}});

}

namespace gell_mann {

inline
csl::Expr lambda1 = csl::matrix_s({{CSL_0, CSL_1, CSL_0},
                            {CSL_1, CSL_0, CSL_0},
                            {CSL_0, CSL_0, CSL_0}});

inline
csl::Expr lambda2 = csl::matrix_s({{CSL_0, -CSL_I, CSL_0},
                            {CSL_I, CSL_0, CSL_0},
                            {CSL_0, CSL_0, CSL_0}});

inline
csl::Expr lambda3 = csl::matrix_s({{CSL_1, CSL_0, CSL_0},
                            {CSL_0, -CSL_1, CSL_0},
                            {CSL_0, CSL_0, CSL_0}});

inline
csl::Expr lambda4 = csl::matrix_s({{CSL_0, CSL_0, CSL_1},
                            {CSL_0, CSL_0, CSL_0},
                            {CSL_1, CSL_0, CSL_0}});

inline
csl::Expr lambda5 = csl::matrix_s({{CSL_0, CSL_0, -CSL_I},
                            {CSL_0, CSL_0, CSL_0},
                            {CSL_I, CSL_0, CSL_0}});

inline
csl::Expr lambda6 = csl::matrix_s({{CSL_0, CSL_0, CSL_0},
                            {CSL_0, CSL_0, CSL_1},
                            {CSL_0, CSL_1, CSL_0}});

inline
csl::Expr lambda7 = csl::matrix_s({{CSL_0, CSL_0, CSL_0},
                            {CSL_0, CSL_0, -CSL_I},
                            {CSL_0, CSL_I, CSL_0}});

inline
csl::Expr lambda8 = 1/csl::sqrt_s(csl::int_s(3)) 
              * csl::matrix_s({{CSL_1, CSL_0, CSL_0},
                             {CSL_0, CSL_1, CSL_0},
                             {CSL_0, CSL_0, -CSL_2}});

csl::Expr generate_f_ABC();

csl::Expr generate_d_ABC();


inline
csl::Expr f = generate_f_ABC();

inline 
csl::Expr d3 = generate_d_ABC();

}

#endif
