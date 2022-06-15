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
 * @version 1.3

 * \brief
 */
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "abstract.h"
#include <functional>

namespace csl {

std::vector<Expr>
InverseTaylorExpand(Expr const &init, Expr const &big, size_t order);

std::vector<Expr>
TaylorExpand(Expr const &init, Expr const &eps, size_t order);

std::vector<Expr>
internal_TaylorExpand(Expr const &init, Expr const &eps, size_t order);

/*! \fn Expr Copy(const Abstract* expr)
 * \brief See Copy(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be
 * removed soon.
 */
[[nodiscard]] Expr Copy(const Abstract *expr);

[[nodiscard]] Expr CopySelf(Abstract *expr);

/*! \fn Expr Copy(const Expr& expr)
 * \brief \b Copy an Abstract to depth 1.
 * \details \b Copy the depth 0 structure. For example the copy of
 * cos(x+exp(y)) creates another cos function but take a reference to x+exp(y).
 * Note that copy a \b Variable will create another with the same name. It
 * could create misunderstanding in the following operations. \param expr
 * Abstract to copy. \return The copy.
 */
[[nodiscard]] Expr Copy(const Expr &expr);

/*! \fn Expr DeepCopy(const Abstract* expr)
 * \brief See DeepCopy(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be
 * removed soon.
 */
[[nodiscard]] Expr DeepCopy(const Abstract *expr);

/*! \fn Expr DeepCopy(const Expr& expr)
 * \brief \b Copy an Abstract to the \b maximum depth.
 * \details \b Copy \b recursively the entire Abstract.
 * \param expr The Abstract to copy.
 * \return The deepCopy.
 */
[[nodiscard]] Expr DeepCopy(const Expr &expr);

void WeakDeepCopy(Expr &expr);

/*! \fn Expr Refreshed(const Abstract* expr)
 * \brief See Refreshed(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be
 * removed soon.
 */
[[nodiscard]] Expr Refreshed(const Abstract *expr);

/*! \fn Expr Refreshed(const Expr& expr)
 * \brief \b Refreshed an Abstract and apply basic simplifications.
 * \details Apply all simplifications that take place normally at the creation
 * of an Abstract. For example, a sum with only one term gives just the term in
 * question.
 * \param expr The Abstract to refresh.
 * \return The refreshed Abstract.
 */
[[nodiscard]] Expr Refreshed(const Expr &expr);

/*! \fn Expr DeepRefreshed(const Expr& expr)
 * \brief \b Refreshed recursively an Abstract and apply basic simplifications.
 * \details Apply all simplifications that take place normally at the creation
 * of an Abstract. For example, a sum with only one term gives just the term in
 * question. The refresh is resursive.
 * \param expr The Abstract to refresh.
 * \return The refreshed Abstract.
 */
[[nodiscard]] Expr DeepRefreshed(const Expr &expr);

inline void Refresh(Expr &expr)
{
    expr = Refreshed(expr);
}

inline void DeepRefresh(Expr &expr)
{
    expr = DeepRefreshed(expr);
}

} // End of namespace csl

#endif
