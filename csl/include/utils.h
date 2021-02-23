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
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <functional>
#include "abstract.h"

namespace csl {

std::vector<Expr> InverseTaylorExpand(
        Expr const& init,
        Expr const& big,
        size_t order);

std::vector<Expr> TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order);

std::vector<Expr> internal_TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order);

/*! \fn Expr Copy(const Abstract* expr)
 * \brief See Copy(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be 
 * removed soon.
 */
Expr Copy(const Abstract* expr);

Expr CopySelf(Abstract* expr);

/*! \fn Expr Copy(const Expr& expr)
 * \brief \b Copy an Abstract to depth 1.
 * \details \b Copy the depth 0 structure. For example the copy of cos(x+exp(y)) 
 * creates another cos function but take a reference to x+exp(y). Note that copy
 *  a \b Variable will create another with the same name. It could create 
 * misunderstanding in the following operations.
 * \param expr Abstract to copy.
 * \return The copy.
 */
Expr Copy(const Expr& expr);

/*! \fn Expr DeepCopy(const Abstract* expr)
 * \brief See DeepCopy(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be 
 * removed soon.
 */
Expr DeepCopy(const Abstract* expr);

/*! \fn Expr DeepCopy(const Expr& expr)
 * \brief \b Copy an Abstract to the \b maximum depth.
 * \details \b Copy \b recursively the entire Abstract.
 * \param expr The Abstract to copy.
 * \return The deepCopy.
 */
Expr DeepCopy(const Expr& expr);

/*! \fn Expr Refreshed(const Abstract* expr)
 * \brief See Refreshed(const Expr& expr).
 * \note With the apparition of shared_from_this(), this function should be 
 * removed soon.
 */
Expr Refreshed(const Abstract* expr);

/*! \fn Expr Refreshed(const Expr& expr)
 * \brief \b Refreshed an Abstract and apply basic simplifications.
 * \details Apply all simplifications that take place normally at the creation
 * of an Abstract. For example, a sum with only one term gives just the term in
 * question.
 * \param expr The Abstract to refresh.
 * \return The refreshed Abstract.
 */
Expr Refreshed(const Expr& expr);

/*! \fn Expr DeepRefreshed(const Expr& expr) 
 * \brief \b Refreshed recursively an Abstract and apply basic simplifications.
 * \details Apply all simplifications that take place normally at the creation
 * of an Abstract. For example, a sum with only one term gives just the term in
 * question. The refresh is resursive.
 * \param expr The Abstract to refresh.
 * \return The refreshed Abstract.
 */
Expr DeepRefreshed(const Expr& expr);

inline void Refresh(Expr& expr)
{
    expr = Refreshed(expr);
}

inline void DeepRefresh(Expr& expr)
{
    expr = DeepRefreshed(expr);
}
        
/*! \fn Expr Replaced(const Expr& expr,
                     const Expr& old_abstract,
                     const Expr& new_abstract)
 * \brief \b Replaced an Abstract by another in an expr.
 * \details This function allows to replace \a old_abstract by \a new_abstract 
 * in \a expr. For example Replaced(cos(x+x^2),x,y-1) = cos(y-1+(y-1)^2). 
 * It leaves the original Abstract \b invariant and returns another Abstract.
 * \param expr Abstract in which the replacement takes place.
 * \param old_abstract Abstract to search and replace.
 * \param new_abstract Abstract by which we replace.
 * \return A \b new Abstract with the replacement done.
 */
Expr Replaced(const Expr& expr,
              const Expr& old_abstract,
              const Expr& new_abstract);

Expr internal_Replaced(
        Expr const &expr,
        Expr const &old_abstract,
        Expr const &new_abstract
        );

inline void Replace(
        Expr       &expr,
        Expr const &old_abstract,
        Expr const &new_abstract)
{
    expr = Replaced(expr, old_abstract, new_abstract);
}

Expr Replaced(
        const Expr& expr,
        std::vector<Expr> const& old_abstract,
        std::vector<Expr> const& new_abstract
        );

Expr internal_Replaced(
        Expr const &expr,
        std::vector<Expr> const &old_abstract,
        std::vector<Expr> const &new_abstract
        );

void Replace(
        Expr       &expr,
        std::vector<Expr> const &old_abstract,
        std::vector<Expr> const &new_abstract);

void ResetDummyIndices(Expr& expr);

void ApplyIndices(Expr&           expr,
                  const IndexStructure& freeStructure);

void ApplyIndices(Expr&           expr,
                  const IndexStructure& oldStructure,
                  const IndexStructure& freeStructure);

Expr Replaced(const Expr&     expr,
              const Parent&   old_abstract,
              const Expr&     new_abstract);

inline void Replace(
        Expr &expr,
        Parent const &old_abstract,
        Expr   const &new_abstract
        )
{
    expr = Replaced(expr, old_abstract, new_abstract);
}

void internal_Replace(Expr&           expr,
                      Parent const&  old_abstract,
                      Expr const&     new_abstract);

Expr Replaced(const Expr&     expr,
              std::vector<Parent> const&   old_abstract,
              std::vector<Expr> const&     new_abstract);

inline void Replace(
        Expr &expr,
        std::vector<Parent> const &old_abstract,
        std::vector<Expr>   const &new_abstract
        )
{
    expr = Replaced(expr, old_abstract, new_abstract);
}

void internal_Replace(Expr&           expr,
                      std::vector<Parent> const&  old_abstract,
                      std::vector<Expr> const&     new_abstract);

template<
    class ParentType>
Expr Replaced(const Expr&     expr,
              std::vector<ParentType> const&   t_old_abstract,
              std::vector<Expr> const&     new_abstract)
{
    std::vector<Parent> old_abstract(t_old_abstract.size());
    for (size_t i = 0; i != old_abstract.size(); ++i) 
        old_abstract[i] = t_old_abstract[i];
    return Replaced(expr, old_abstract, new_abstract);
}

template<
    class ParentType,
    typename = typename std::enable_if<
            std::is_convertible<ParentType, Parent>::value
            >::value
    >
inline void Replace(
        Expr &expr,
        std::vector<ParentType> const &old_abstract,
        std::vector<Expr>   const &new_abstract
        )
{
    expr = Replaced(expr, old_abstract, new_abstract);
}

Index internal_RenameIndex(const Index& index,
                  std::map<Index, Index>& mapping);

void RenameIndices(Expr& expr);

inline Expr RenamedIndices(Expr const& expr)
{
    Expr res = DeepCopy(expr);
    RenameIndices(res);
    return res;
}

void internal_RenameIndices(Expr& expr,
                            std::map<Index, Index>& mapping);

/*! \fn Expr Replaced(const Expr& expr,
                     const Expr& old_parent,
                     const Expr& new_parent)
 * \brief \b Replaced an TensorParent by another in an expr.
 * \details This function allows to replace \a old_parent by \a new_parent 
 * in \a expr. For example Replaced(cos(x+x^2),x,y-1) = cos(y-1+(y-1)^2). 
 * It leaves the original TensorParent \b invariant and returns another
 * TensorParent.
 * \param expr TensorParent in which the replacement takes place.
 * \param old_parent TensorParent to search and replace.
 * \param new_parent TensorParent by which we replace.
 * \return A \b new TensorParent with the replacement done.
 */
Expr Replaced(const Expr& expr,
             const Parent& old_parent,
             const Parent& new_parent,
             bool refresh = true);

inline void Replace(
        Expr& expr,
        const Parent& old_parent,
        const Parent& new_parent,
        bool refresh = true)
{
    expr = Replaced(expr, old_parent, new_parent, refresh);
}

Expr Replaced(const Expr& expr,
             std::vector<Parent> const& old_parent,
             std::vector<Parent> const& new_parent,
             bool refresh = true);

inline void Replace(
        Expr& expr,
        std::vector<Parent> const& old_parent,
        std::vector<Parent> const& new_parent,
        bool refresh = true)
{
    expr = Replaced(expr, old_parent, new_parent, refresh);
}

template<
    class ParentType,
    typename = typename std::enable_if<
            std::is_convertible<ParentType, Parent>::value
            >::value
    >
Expr Replaced(const Expr& expr,
             std::vector<ParentType> const& t_old_parent,
             std::vector<ParentType> const& t_new_parent,
             bool refresh = true)
{
    std::vector<Parent> old_parent(t_old_parent.size());
    std::vector<Parent> new_parent(t_new_parent.size());
    for (size_t i = 0; i != old_parent.size(); ++i)
        old_parent[i] = t_old_parent[i];
    for (size_t i = 0; i != new_parent.size(); ++i)
        new_parent[i] = t_new_parent[i];

    return Replaced(expr, old_parent, new_parent, refresh);
}

template<
    class ParentType,
    typename = typename std::enable_if<
            std::is_convertible<ParentType, Parent>::value
            >::value
    >
inline void Replace(
        Expr& expr,
        std::vector<ParentType> const& old_parent,
        std::vector<ParentType> const& new_parent,
        bool refresh = true)
{
    expr = Replaced(expr, old_parent, new_parent, refresh);
}


template<class T, class U>
Expr Tried(const Expr& expr,
           const T&    oldExpr,
           const U&    newExpr)
{
    Expr trial = Replaced(expr, oldExpr, newExpr);
    return (*trial < expr.get()) ? trial : expr;
}

template<class T, class U>
void Try(Expr& expr,
         const T&    oldExpr,
         const U&    newExpr)
{
    Expr trial = Replaced(expr, oldExpr, newExpr);
    if (trial < expr)
        expr = trial;
}

csl::vector_expr applyOnAll(const csl::vector_expr& vec,
        std::function<Expr(Expr)> func);

template<class T>
T chooseOptional(std::optional<T> const& opt,
                 T                const& other)
{
    if (opt)
        return opt.value();
    return other;
}


} // End of namespace csl

#endif
