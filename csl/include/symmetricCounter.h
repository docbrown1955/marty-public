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
#ifndef SYMMETRICCOUNTER_H_INCLUDED
#define SYMMETRICCOUNTER_H_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include "std_vector_implementation.h"

template<class T>
class symmetricCounter {

    IMPLEMENTS_STD_VECTOR(T, counter);

    public:

    static constexpr char separator = '\'';

    symmetricCounter(size_t N)
        :symmetricCounter(N, N) 
    {

    }

    symmetricCounter(size_t N, size_t t_max)
        :max(t_max),
        counter(N, 0)
    {

    }

    symmetricCounter<T> operator++(int)
    {
        symmetricCounter<T> other(*this);
        ++(*this);
        return other;
    }

    symmetricCounter<T>& operator++()
    {
        if (size() > 0 and increment(0)) {
            adjust();
        }

        return *this;
    }

    static size_t combinatorial(size_t N, size_t m)
    {
        size_t num = 1;
        size_t denom = 1;
        for (size_t i = 1; i <= N; ++i) {
            if (i > m)
                num *= i;
            if (i <= N - m)
                denom *= i;
        }

        return num / denom;
    }

    size_t factor() const
    {
        auto copy = counter;
        auto last = std::unique(copy.begin(), copy.end());
        size_t factor = 1;
        size_t N = counter.size();
        for (auto el = copy.begin(); el != last; ++el) {
            size_t m = std::count(counter.begin(), counter.end(), *el);
            factor *= combinatorial(N, m);
            N -= m;
        }

        return factor;
    }

    operator bool() const 
    {
        for (const auto& value : counter)
            if (value != 0)
                return true;
        return false;
    }

    friend
    std::ostream& operator<<(std::ostream& out, symmetricCounter<T> const& c)
    {
        out << c.factor();
        out << ": ";
        for (const auto& value : c)
            out << value << symmetricCounter<T>::separator;
        return out;
    }


    protected:

    bool increment(size_t pos)
    {
        ++counter[pos];
        if (counter[pos] == max) {
            counter[pos] = 0;
            if (++pos != counter.size())
                increment(pos);
            return true;
        }
        return false;
    }

    void adjust() 
    {
        if (counter.size() < 2)
            return;
        T min = counter.back();
        for (auto iter = counter.rbegin()+1; iter != counter.rend(); ++iter) {
            if (*iter < min)
                *iter = min;
            else if (*iter > min)
                min = *iter;
        }
    }

    private:

    size_t max;

    std::vector<T> counter;
};

#endif
