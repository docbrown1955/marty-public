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

#include "sort.h"

namespace csl {

using iterator = std::vector<Expr>::iterator;

void selectionSort(
    iterator first,
    iterator last
    )
{
    auto i = first;
    while (i != last) {
        auto simpler = i;
        auto j = i;
        while (++j != last) {
            if (*j < *simpler)
                simpler = j;
        }
        if (simpler != i)
            std::swap(*simpler, *i);
        ++i;
    }
}
void selectionSort(std::vector<Expr> &argument)
{
    selectionSort(argument.begin(), argument.end());
}

static void insert(
    iterator a1_first,
    iterator a1_last,
    iterator a2_first,
    iterator a2_last,
    iterator b_first
    )
{
    while (a1_first != a1_last and a2_first != a2_last) {
        if (*a1_first < *a2_first) {
            *b_first++ = *a1_first++;
        }
        else {
            *b_first++ = *a2_first++;
        }
    }
    while (a1_first != a1_last) {
        *b_first++ = *a1_first++;
    }
    while (a2_first != a2_last) {
        *b_first++ = *a2_first++;
    }
}

static void mergeSort(
    iterator a_first,
    iterator a_last,
    iterator b_first,
    iterator b_last
    )
{
    if (static_cast<size_t>(a_last - a_first) <= minMergeSize) {
        selectionSort(a_first, a_last);
    }
    else {
        iterator a_mid = a_first + (a_last - a_first) / 2;
        iterator b_mid = b_first + (a_mid - a_first);
        mergeSort(a_first, a_mid,  b_first, b_mid);
        mergeSort(a_mid,   a_last, b_mid,   b_last);
        insert(a_first, a_mid, a_mid, a_last, b_first);
        std::copy(b_first, b_last, a_first);
    }
}

void sort(std::vector<Expr> &argument)
{
    sort(argument.begin(), argument.end());
}

void sort(
        iterator first,
        iterator last
        )
{
    std::vector<Expr> copy(first, last);
    mergeSort(first, last, copy.begin(), copy.end());
    if (static_cast<size_t>(last - first) > minMergeSize)
        std::copy(copy.begin(), copy.end(), first);
}

void mergeSort(std::vector<Expr> &argument)
{
    mergeSort(argument.begin(), argument.end());
}

void mergeSort(
        iterator first,
        iterator last
        )
{
    sort(first, last);
}

} // End of namespace csl
