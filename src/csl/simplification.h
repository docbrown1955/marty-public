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

/*! \fn printVector(const csl::vector_expr& vector)
 * \brief Display a vector of expressions. Useful in debug.
 * \param vector std::vector of expressions to display.
 */
void printVector(const csl::vector_expr &vector);

/*! \fn addAlternateForm(csl::vector_expr& alternateForms,
 *                       const Expr& newAlternate,
 *                       bool add_factor_expand)
 * \brief Tried to add \b newAlternate in the set \b alternateForms. We simply
 * test if the alternate is already present in the set \b newAlternate and add
 * it if it is not the case. If \b add_factor_expand is set to \b true, we try
 * to add the expanded and the factored forms of \b newAlternate.
 * \param alternateForms std::vector of expressions in which we add \b
 * newAlternate. \param newAlternate Expression to add. \param
 * add_factor_expand Boolean determining if we want to add also the factored
 * and expanded versions of \b newAlternate.
 */
void addAlternateForm(csl::vector_expr &alternateForms,
                      const Expr &      newAlternate,
                      bool              add_factor_expand = true);

/*! \fn reduceAlternate(csl::vector_expr& alternateForms)
 * \brief Reduces the number of elements in \b alternateForms to
 * MAX_ALTERNATE_FORMS. \param alternateForms std::vector of expressions to
 * reduce.
 */
void reduceAlternate(csl::vector_expr &alternateForms);

/*! \fn clearRedundancyAlternate(csl::vector_expr& alternateForms)
 * \brief Search and remove redundancies (equal alternate forms) in \b
 * alternateForms. \param alternateForms std::vector of expressions to reduce.
 */
void clearRedundancyAlternate(csl::vector_expr &alternateForms);

/*! \fn getRecursiveAlternateForms(const Expr& expr, int depth=-1)
 * \brief Returns the alternate forms of \b expr by applying recursively
 * internalRecursiveAlternateForms() MAX_RECURSION_ALTERNATE times: take
 * alternates, then alternates of the alternates etc.
 * \param expr Expression from which we search alternates.
 * \param depth Recursion depth transmitted to
 * internalRecursiveAlternateForms(). \return a std::vector of expressions
 * containing alternates of \b expr.
 */
csl::vector_expr getRecursiveAlternateForms(const Expr &expr, int depth = -1);

/*! \fn internalRecursiveAlternateForms(const Expr& expr, int depth=-1)
 * \brief Calculates and return all alternate forms of \b expr, by getting
 * (once) alternate forms of the possible arguments of \b expr, and then the
 * specific alternates of expr.
 * \param expr Expression from which we get alternate forms.
 * \param depth Recursion depth (default = -1, we take the alternate forms of
 * all the recursion tree).
 * \return A std::vector of expressions containing the alternate forms of \b
 * expr.
 */
csl::vector_expr internalRecursiveAlternateForms(const Expr &expr,
                                                 int         depth = -1);

/*! \fn Expr Simplify(const Expr& expr, int depth=-1)
 * \brief \b Simplifies \b expr depending on its type.
 * \param expr Expression to reduce.
 * \param depth Depth of simplification. Default=1: **max depth**.
 * \return The simplified expression.
 */
Expr Simplify(const Expr &expr, int depth = -1);

int numberOfMutatingNodes(const Expr &expr);

bool insertSortMutant(csl::vector_expr &vec, const Expr &newExpr);

bool addMutants(csl::vector_expr &      individuals,
                const csl::vector_expr &mutants);

csl::vector_expr getRandomMutation(const Expr &expr);

void naturalSelection(csl::vector_expr &individuals);

Expr evolve(const Expr &baseIndividual);
} // End of namespace csl

#endif
