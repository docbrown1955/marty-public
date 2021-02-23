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

/*! \file algo.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Contains algorithms that look over (and possibly modify on the go)
 * expressions for you, given a certain function to apply to nodes in the tree
 * expression.
 * \details All algorithms work the same way. The user provides an initial 
 * expression and a function. The expression will be browsed entirely by the 
 * algorithm, and the user function will be called on each node or leaf of the
 * expression tree. The function may be equivalently an inline lambda 
 * expression, a lambda stored in a variable or a standard function pointer.
 * Here is an example:
 * \code
 * void standardFunc(Expr const &node)
 * {
 *      doStuff();
 * }
 *
 * int main() {
 *
 *      auto lambdaVar = [&](Expr const &node)
 *      {
 *          doStuff();
 *      }
 *
 *      Expr myBigExpression = bigExpression();
 *
 *      // First possibility
 *      csl::AlgoOnNodes(myBigExpression, &standardFunc);
 *
 *      // Second possibility
 *      csl::AlgoOnNodes(myBigExpression, lambdaVar);
 *
 *      // Third possibility
 *      csl::AlgoOnNodes(myBigExpression,
 *      [&](Expr const &node)
 *      {
 *          doStuff();
 *      });
 * }
 * \endcode
 * In this example, consider the expression \f$ A\left(1 + \cos \frac{2\pi t}{T}
 * \right) \f$. If you call a node algorithm on it, the function you pass will 
 * be called with successively:
 *      -  \f$ A\left(1 + \cos \frac{2\pi t}{T}\right) \f$
 *      -  \f$ A \f$
 *      -  \f$ 1 + \cos \frac{2\pi t}{T} \f$
 *      -  \f$ 1 \f$
 *      -  \f$ \cos \frac{2\pi t}{T} \f$
 *      -  \f$ \frac{2\pi t}{T} \f$
 *      -  \f$ 2 \f$
 *      -  \f$ \pi \f$
 *      -  \f$ t \f$
 *      -  \f$ T^{-1} \f$
 *      -  \f$ T \f$
 *      -  \f$ -1 \f$
 *
 * whereas with a leaf algorithm, the function will be called on:
 *      -  \f$ A \f$
 *      -  \f$ 1 \f$
 *      -  \f$ 2 \f$
 *      -  \f$ \pi \f$
 *      -  \f$ t \f$
 *      -  \f$ T \f$
 *      -  \f$ -1 \f$
 * \note I say here that all nodes / leafs are seen, but depending on the 
 * algorithm and user function this might not be the case. It is however always
 * wanted by the user that wrote good code.
 * \note The order of call is reversed for the Transform() algorithm that begins
 * with the leafs of the tree.
 * \note When giving a function that concerns only leafs, use a leaf algorithm.
 * It may be considerably faster if both the initial expression and the given
 * function are complicated.
 */
#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

#include "abstract.h"
#include <functional>

namespace csl {

    /*!
     * \brief Searches and returns an expression in another.
     * \details If one of the leafs of \b init compares to \b value successfully
     * the function returns it. Else it returns nullptr. Beware that in this 
     * case the result is invalid.
     * \param init  Expression in which the function searches.
     * \param value Sub-expression to find.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return The first leaf to compare to \b value if found.
     * \return nullptr else.
     * \sa FindNode(), FindIfLeaf(), FindIfNode().
     */
    Expr FindLeaf(Expr const& init,
                  Expr const& value,
                  size_t      depth = -1);

    /*!
     * \brief Searches and returns an expression in another.
     * \details If one of the nodes of \b init compares to \b value successfully
     * the function returns it. Else it returns nullptr. Beware that in this 
     * case the result is invalid.
     * \param init  Expression in which the function searches.
     * \param value Sub-expression to find.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return The first node to compare to \b value if found.
     * \return nullptr else.
     * \sa FindLeaf(), FindIfNode(), FindIfLeaf().
     */
    Expr FindNode(Expr const& init,
                  Expr const& node,
                  size_t      depth = -1);

    /*!
     * \brief Searches and returns an expression in another.
     * \details If one of the leafs of \b init compares to \b value successfully
     * using the condition \b f given by the user the function returns it. Else 
     * it returns nullptr. Beware that in this case the result is invalid.
     * \param init  Expression in which the function searches.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return The first leaf to respect the condition if found.
     * \return nullptr else.
     * \sa FindNode(), FindLeaf(), FindIfNode().
     */
    Expr FindIfLeaf(Expr const&                             init,
                    std::function<bool(Expr const&)> const& f,
                    size_t                                  depth = -1);

    /*!
     * \brief Searches and returns an expression in another.
     * \details If one of the nodes of \b init compares to \b value successfully
     * using the condition \b f given by the user the function returns it. Else 
     * it returns nullptr. Beware that in this case the result is invalid.
     * \param init  Expression in which the function searches.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return The first node to respect the condition if found.
     * \return nullptr else.
     * \sa FindNode(), FindLeaf(), FindIfLeaf().
     */
    Expr FindIfNode(Expr const&                             init,
                    std::function<bool(Expr const&)> const& f,
                    size_t                                  depth = -1);

    /*!
     * \brief Tells if any of the leafs of an expression respect a certain 
     * condition given by the user.
     * \details If one of the leafs of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return \b True  if any of the leafs of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa AllOfLeafs(), AnyOfNodes(), AllOfNodes().
     */
    bool AnyOfLeafs(Expr                             const& init,
                    std::function<bool(Expr const&)> const& f,
                    int                                     depth = -1);
    bool AnyOfLeafs(Expr_info                             init,
                    std::function<bool(Expr_info)> const& f,
                    int                                   depth = -1);

    /*!
     * \brief Tells if all of the leafs of an expression respect a certain 
     * condition given by the user.
     * \details If one of the leafs of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return \b True  if all of the leafs of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa AllOfLeafs(), AnyOfNodes(), AllOfNodes().
     */
    bool AllOfLeafs(Expr                             const& init,
                    std::function<bool(Expr const&)> const& f,
                    int                                     depth = -1);
    bool AllOfLeafs(Expr_info                             init,
                    std::function<bool(Expr_info)> const& f,
                    int                                   depth = -1);

    /*!
     * \brief Tells if any of the nodes of an expression respect a certain 
     * condition given by the user.
     * \details If one of the nodes of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return \b True  if any of the nodes of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa AllOfLeafs(), AnyOfLeafs(), AllOfNodes().
     */
    bool AnyOfNodes(Expr                             const& init,
                    std::function<bool(Expr const&)> const& f,
                    int                                     depth = -1);
    bool AnyOfNodes(Expr_info                             init,
                    std::function<bool(Expr_info)> const& f,
                    int                                   depth = -1);

    /*!
     * \brief Tells if all of the nodes of an expression respect a certain 
     * condition given by the user.
     * \details If one of the nodes of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning a boolean. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \return \b True  if all of the nodes of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa AllOfLeafs(), AnyOfNodes(), AllOfNodes().
     */
    bool AllOfNodes(Expr                             const& init,
                    std::function<bool(Expr const&)> const& f,
                    int                                     depth = -1);
    bool AllOfNodes(Expr_info                             init,
                    std::function<bool(Expr_info)> const& f,
                    int                                   depth = -1);

    /*!
     * \brief Visits all nodes of an expression, applying a function on it 
     * without modifying it.
     * \details This algorithm browses the whole expression applying \b f to 
     * each node. 
     * \param init Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning nothing. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa VisitEachLeaf(), VisitEachNodeCut().
     */
    void VisitEachNode(Expr                             const& init,
                       std::function<void(Expr const&)> const& f,
                       int                                     depth = -1);
    void VisitEachNodeReversed(Expr                     const &init,
                       std::function<void(Expr const&)> const& f,
                       int                                   depth = -1);
    void VisitEachNode(Expr_info                             init,
                       std::function<void(Expr_info)> const& f,
                       int                                   depth = -1);

    /*!
     * \brief Visits all leafs of an expression, applying a function on it 
     * without modifying it.
     * \details This algorithm browses all the leafs of \b init applying \b f 
     * each time. 
     * \param init Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning nothing. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa VisitEachNode(), VisitEachNodeCut().
     */
    void VisitEachLeaf(Expr                             const& init,
                       std::function<void(Expr const&)> const& f,
                       int                                     depth = -1);
    void VisitEachLeaf(Expr_info                             init,
                       std::function<void(Expr_info)> const& f,
                       int                                   depth = -1);

    /*!
     * \brief Visits all nodes of an expression, applying a function on it 
     * without modifying it. When the user function returns true on one node,
     * the underlying branch (if there is) is not explored.
     * \details This algorithm browses the whole expression applying \b f to 
     * each node. The user function must return a boolean. If true, the 
     * algorithm stops on the node (other branches are still explored).
     * \param init Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr const& as parameter and returning a boolean telling if the 
     * algorithm must stop on that branch. 
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa VisitEachLeaf(), VisitEachNode().
     */
    void VisitEachNodeCut(
            Expr                             const& init,
            std::function<bool(Expr const&)> const& f,
            int                                     depth = -1);
    void VisitEachNodeCut(
            Expr_info                             init,
            std::function<bool(Expr_info)> const& f,
            int                                   depth = -1);


    size_t CountNodes(Expr const &init);
    size_t CountLeafs(Expr const &init);

    /*!
     * \brief Applies a user function on each node of an expression. The 
     * expression may be modified.
     * \details This algorithm browses the whole expression applying \b f to 
     * each node. If the user function \b f does not modify the nodes, consider 
     * using VisitEachNode() instead.
     * \param init  Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& (may or may not modify it) as parameter.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa ForEachLeaf(), ForEachNodeCut(), Transform().
     */
    void ForEachNode(Expr                            & init,
                     std::function<void(Expr&)> const& f,
                     int                               depth = -1);
    void ForEachNodeReversed(Expr                            & init,
                     std::function<void(Expr&)> const& f,
                     int                               depth = -1);

    /*!
     * \brief Applies a user function on each leaf of an expression. The 
     * expression may be modified.
     * \details This algorithm browses the whole expression applying \b f to 
     * each leaf. If the user function \b f does not modify the leafs, consider 
     * using VisitEachLeaf() instead.
     * \param init  Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& (may or may not modify it) as parameter.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa ForEachNode(), ForEachNodeCut(), Transform().
     */
    void ForEachLeaf(Expr                            & init,
                     std::function<void(Expr&)> const& f,
                     int                               depth = -1);

    /*!
     * \brief Applies a user function on each node of an expression. The 
     * expression may be modified.
     * \details This algorithm browses the whole expression applying \b f to 
     * each node. If the user function \b f does not modify the nodes, consider 
     * using VisitEachNodeCut() instead. The user function must return a boolean. 
     * If true, the algorithm stops on the node (other branches are still 
     * explored).
     * \param init  Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& (may or may not modify it) as parameter. Must return a boolean that
     * tells if the algorithm must stop to search in that branch, if that 
     * boolean is true.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa ForEachLeaf(), ForEachNode(), VisitEachNodeCut().
     */
    void ForEachNodeCut(Expr                            & init,
                        std::function<bool(Expr&)> const& f,
                        int                               depth = -1);

    /*!
     * \brief Tells if any of the nodes of an expression respect a certain 
     * condition given by the user. The condition function may apply on the go
     * on the expression found.
     * \details If one of the nodes of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false. Once one
     * node has been found, the function stops completely.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& as parameter and returning a boolean. May modify the content of the
     * expression.
     * \return \b True  if any of the nodes of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa FirstOfLeaf().
     */
    bool FirstOfNode(Expr& init,
                     std::function<bool(Expr&)> const& f);

    /*!
     * \brief Tells if any of the leafs of an expression respect a certain 
     * condition given by the user. The condition function may apply on the go
     * on the expression found.
     * \details If one of the leafs of \b init respects the condition \b f given 
     * by the user the function returns \b true. Else it returns false. Once one
     * node has been found, the function stops completely.
     * \param init  Expression in which the function searches recursively.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& as parameter and returning a boolean. May modify the content of the
     * expression.
     * \return \b True  if any of the leafs of \b init respects the condition \b
     * f.
     * \return \b False else.
     * \sa FirstOfLeaf().
     */
    bool FirstOfLeaf(Expr& init,
                     std::function<bool(Expr&)> const& f);

    bool VisitFirstOfNode(Expr                       const& init,
                     std::function<bool(Expr const&)> const& f);
    bool VisitFirstOfLeaf(Expr                       const& init,
                     std::function<bool(Expr const&)> const& f);

    /*!
     * \brief Applies a user function on each node of an expression. The 
     * expression may be modified. If it is, the expression is refreshed.
     * \details This algorithm browses the whole expression applying \b f to 
     * each node. The user function must return true if the expression has to
     * be refreshed after the transformation, false else. If you do not want 
     * to refresh the expression (powerful but heavy) consider using 
     * ForEachNode() instead.
     * \param init  Expression to browse.
     * \param f     std::function (can be a c++ lambda expression) taking an 
     * Expr& (may or may not modify it) as parameter. Must return a boolean, 
     * true if the expression requires a refresh after the call, false else.
     * \param depth Depth of recursion in the search (default = -1, full 
     * recursion).
     * \sa ForEachNode(), Refreshed(), DeepRefreshed().
     * */
    bool Transform(Expr                            & init,
                   std::function<bool(Expr&)> const& f,
                   int                               depth = -1);

} // End of namespace csl

#endif
