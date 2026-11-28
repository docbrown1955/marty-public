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

/*! \file simplification.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Automatic simplification framework. Sources in simplification.cpp and
 * alternateForms.cpp.
 */
#ifndef SIMPLIFICATION_H_INCLUDED
#define SIMPLIFICATION_H_INCLUDED

#include "abstract.h"

namespace csl {

extern bool RANDOM_SEED;

extern const int NUMBER_OF_MUTATIONS;

extern const int NATURAL_SELECTION;

extern const int NUMBER_OF_GENERATION;

extern const int GENERATION_BEFORE_SELECTION;

/*! \var SIMPLIFICATION_METHOD
 * \brief Determines which simplification method is applied. Ther used to be 2,
 * now there is only one method. It may be useful in the future.
 */
extern const int SIMPLIFICATION_METHOD;

/*! \var MAX_ALTERNATE_FORMS
 * \brief Maximum number of alternate forms given by
 * getRecursiveAlternateForms(). When getting alternate forms of an expression,
 * we keep only the MAX_ALTERNATE_FORMS simpler.
 */
extern const int MAX_ALTERNATE_FORMS;

/*! \var MAX_RECURSION_ALTERNATE
 * \brief When getting alternateForms, we iterate MAX_RECURSION_ALTERNATE times
 * (getting alternate forms of the alternate forms) in order to allow
 * alternates that need several steps of Transformation.
 */
extern const int MAX_RECURSION_ALTERNATE;

} // End of namespace csl

#endif
