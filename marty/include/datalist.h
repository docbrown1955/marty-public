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

 #pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <type_traits>
#include "csl/std_vector_implementation.h"

namespace mty::doc {

template<class Printable>
struct DefaultPrinter {
    void operator()(std::ostream &out, Printable const &printable) {
        out << printable;
    }
};

template<class Comparable>
struct DefaultComparator
    :public std::equal_to<std::remove_reference_t<std::remove_cv_t<Comparable>>> 
{};

template<class Comparable>
struct DefaultHardComparator {
    bool operator()(Comparable const &A, Comparable const &B) const {
        return false;
    }
};

template<class ValueType, class Comparator, class HardComparator, class Printer>
struct DataDiff;

template<
    class ValueType, 
    class Comparator     = doc::DefaultComparator<ValueType>,
    class HardComparator = doc::DefaultHardComparator<ValueType>,
    class Printer        = doc::DefaultPrinter<ValueType>
    >
class DataList {

    IMPLEMENTS_STD_VECTOR_NO_PB(ValueType, data)

public:

    static std::vector<size_t> range(size_t N) {
        std::vector<size_t> indices(N);
        for (size_t i = 0; i != N; ++i) 
            indices[i] = i;
        return indices;
    }
        
    template<class ...Args>
    DataList(Args &&...args): data(std::forward<Args>(args)...) {}

    template<class t_Comparator, class t_HardComparator, class t_Printer>
    DataList(DataList<
            ValueType, t_Comparator, t_HardComparator, t_Printer
            > const &other)
        :DataList(other.data)
    {}

    void push(ValueType const &value) { data.push_back(value); }
    void push(ValueType &&value)      { data.push_back(value); }
    
    [[nodiscard]]
    bool compare(DataList<
            ValueType, Comparator, HardComparator, Printer
            > const &other) const;
    [[nodiscard]]
    bool operator==(DataList<
            ValueType, Comparator, HardComparator, Printer
            > const &other) const 
    {
        return compare(other);
    }

    [[nodiscard]]
    DataDiff<ValueType, Comparator, HardComparator, Printer> getDiff(
            DataList<
                ValueType, Comparator, HardComparator, Printer
                > const &other
            ) const;

    [[nodiscard]]
    DataDiff<ValueType, Comparator, HardComparator, Printer> getHardDiff(
            DataList<
                ValueType, Comparator, HardComparator, Printer
                > const &other
            ) const;

private:

    std::vector<ValueType> data;
};

template<class ValueType, class Comparator, class HardComparator, class Printer>
struct DataDiff {
    DataList<ValueType, Comparator, HardComparator, Printer> lhs;
    DataList<ValueType, Comparator, HardComparator, Printer> rhs;

    bool  empty() const { return lhs.empty() && rhs.empty(); }
    size_t size() const { return lhs.size()  +  rhs.size();  }
};

template<class ValueType, class Comparator, class HardComparator, class Printer>
std::ostream &operator<<(
        std::ostream &out,
        DataDiff<ValueType, Comparator, HardComparator, Printer> const &diff
        )
{
    out << "Diff of size " << diff.size() << '\n';

    out << "LHS : \n";
    auto printer = Printer();
    for (const auto &data : diff.lhs) {
        printer(out, data);
        out << '\n';
    }

    out << "RHS : \n";
    for (const auto &data : diff.rhs) {
        printer(out, data);
        out << '\n';
    }
    out.flush();

    return out;
}

template<class ValueType, class Comparator, class HardComparator, class Printer>
bool DataList<ValueType, Comparator, HardComparator, Printer>::compare(
        DataList<ValueType, Comparator, HardComparator, Printer> const &other
        ) const
{
    if (size() != other.size())
        return false;
    std::vector<size_t> indices = range(size());

    auto comparator = Comparator();
    auto iter = begin();
    while (!indices.empty()) {
        bool matched = false;
        for (size_t k = 0; k != indices.size(); ++k) {
            if (comparator(*iter, other[indices[k]])) {
                matched = true;
                indices.erase(indices.begin() + k);
                break;
            }
        }
        if (!matched)
            return false;
        ++iter;
    }
    return true;
}

template<class ValueType, class Comparator, class HardComparator, class Printer>
DataDiff<ValueType, Comparator, HardComparator, Printer> 
    DataList<ValueType, Comparator, HardComparator, Printer>::getDiff(
        DataList<ValueType, Comparator, HardComparator, Printer> const &other
        ) const
{
    std::vector<size_t> indicesLhs = range(size());
    std::vector<size_t> indicesRhs = range(other.size());
    DataDiff<ValueType, Comparator, HardComparator, Printer> diff;
    auto comparator = Comparator();

    while (!indicesLhs.empty() && !indicesRhs.empty()) {
        const size_t i = indicesLhs[0];
        bool matched = false;
        for (size_t k = 0; k != indicesRhs.size(); ++k) {
            const size_t j = indicesRhs[k];
            if (comparator(data[i], other[j])) {
                matched = true;
                indicesRhs.erase(indicesRhs.begin() + k);
                break;
            }
        }
        if (!matched)
            diff.lhs.push(data[i]);
        indicesLhs.erase(indicesLhs.begin());
    }
    if (!indicesLhs.empty()) {
        for (size_t i : indicesLhs)
            diff.lhs.push(data[i]);
    }
    if (!indicesRhs.empty()) {
        for (size_t i : indicesRhs)
            diff.rhs.push(other[i]);
    }

    return diff;
}

template<class ValueType, class Comparator, class HardComparator, class Printer>
DataDiff<ValueType, Comparator, HardComparator, Printer> 
    DataList<ValueType, Comparator, HardComparator, Printer>::getHardDiff(
        DataList<ValueType, Comparator, HardComparator, Printer> const &other
        ) const
{
    auto diff = getDiff(other);
    if (diff.lhs.empty() or diff.rhs.empty())
        return diff;

    using HardList = typename doc::DataList<
        ValueType, HardComparator, HardComparator, Printer
        >;
    return HardList(std::move(diff.lhs)).getDiff(HardList(std::move(diff.rhs)));
}

}
