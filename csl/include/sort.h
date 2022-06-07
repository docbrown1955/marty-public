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

/**
 * @file sort.h
 * @brief File containing custom sorting algorithms for expressions.
 * @details The std::sort algorithm is in principle more efficient but needs a
 * perfect total ordering between elements, otherwise it might (and will 
 * eventually) crash. Ordering between expressions is indeed well-formed, but 
 * only when expressions are cannonical (typically after a csl::DeepRefresh()
 * call). If an expression is not canonical, the ordering may be ill-formed and
 * std::sort() function crashes. We then define here algorithms (selection and 
 * merge sorts) that will not crash. For a canonical expression, these sorting
 * algorithms will have the same effect as std::sort(), may be slightly less
 * efficient. For non canonical ones, the sort is ill-defined but may still
 * be performed.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-20
 */
#ifndef CSL_SORT_H_INCLUDED
#define CSL_SORT_H_INCLUDED

#include "abstract.h"

namespace csl {

    /**
     * @brief Minimum size for a container to be sorted with mergeSort().
     *
     * @details For smaller containers, selectionSort() is called instead, even
     * trying to use specifically mergeSort().
     */
    inline std::size_t minMergeSize = 10;

    /**
     * @brief Sorts a container using mergeSort().
     *
     * @param argument Container to sort.
     */
    void sort(std::vector<Expr> &argument);
    /**
     * @brief Sorts a container using mergeSort().
     *
     * @param first Iterator pointing to the first element (.begin()).
     * @param last  Iterator pointing to the last element (.end()).
     */
    void sort(
        std::vector<Expr>::iterator first,
        std::vector<Expr>::iterator last
        );


    /**
     * @brief Applies the selection sort algorithm \f$ \mathcal{O}(N^2) \f$
     * on a container.
     *
     * @param argument Container to sort.
     */
    void selectionSort(std::vector<Expr> &argument);
    /**
     * @brief Applies the selection sort algorithm \f$ \mathcal{O}(N^2) \f$
     * on a container.
     *
     * @param first Iterator pointing to the first element (.begin()).
     * @param last  Iterator pointing to the last element (.end()).
     */
    void selectionSort(
            std::vector<Expr>::iterator first,
            std::vector<Expr>::iterator last
            );

    /**
     * @brief Applies the merge sort algorithm \f$ \mathcal{O}(N\log N) \f$
     * on a container.
     *
     * @details This function is recursive, calling a merge sort on parts of 
     * the container. Once the size of these parts are bellow #minMergeSize, 
     * selectionSort() is called instead. For std::shared_ptr (the case for 
     * the Expr type) the optimal size is around 10. This means that with less
     * than 10 elements, selectionSort() is still faster that mergeSort(). 
     * Above, the \f$ \mathcal{O}(N\log N) \f$ complexity of mergeSort() is 
     * of course better and better with large \f$ N \f$.
     *
     * @param argument Container to sort.
     */
    void mergeSort(std::vector<Expr> &argument);
    /**
     * @brief Applies the merge sort algorithm \f$ \mathcal{O}(N\log N) \f$
     * on a container.
     *
     * @details This function is recursive, calling a merge sort on parts of 
     * the container. Once the size of these parts are bellow #minMergeSize, 
     * selectionSort() is called instead. For std::shared_ptr (the case for 
     * the Expr type) the optimal size is around 10. This means that with less
     * than 10 elements, selectionSort() is still faster that mergeSort(). 
     * Above, the \f$ \mathcal{O}(N\log N) \f$ complexity of mergeSort() is 
     * of course better and better with large \f$ N \f$.
     *
     * @param first Iterator pointing to the first element (.begin()).
     * @param last  Iterator pointing to the last element (.end()).
     */
    void mergeSort(
            std::vector<Expr>::iterator first,
            std::vector<Expr>::iterator last
            );

} // End of namespace csl

#endif
