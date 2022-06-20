#pragma once

namespace csl {

/**
 * @brief Template dichotomy algorithm using a comparator.
 *
 * @details For the insertion of an element e, the comparator given must take
 * one argument (of the same type as the range's elements) and return +1 if
 * the element to insert is **simpler** than the argument, -1 if it is
 * **less simple**, and 0 otherwise.
 *
 * @tparam Iterator   Iterator type.
 * @tparam Comparator Comparator type.
 * @param first First iterator in the range.
 * @param last  Last iterator in the range.
 * @param f     Comparator function.
 *
 * @return The iterator where the element compared with $$f$$ must be inserted.
 */
template <class Iterator, class Comparator>
Iterator dichotomyFindIf(Iterator first, Iterator last, Comparator &&f)
{
    while (last != first) {
        const auto diff = last - first;
        Iterator   mid  = first + diff / 2;
        // auto const &midExpr = v[mid]->getEncapsulated();
        int comp = f(*mid);
        if (comp == 1)
            last = mid;
        else if (comp == -1) {
            if (mid == first)
                ++first;
            else
                first = mid;
        }
        else
            return mid;
        if (first + 1 == mid) {
            return (f(*first) == 1) ? first : mid;
        }
    }
    return first;
}

} // namespace csl
