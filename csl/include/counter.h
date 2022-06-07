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
#ifndef COUNTER_H_INCLUDED
#define COUNTER_H_INCLUDED

#include <iostream>
#include <memory>
#include <list>
#include <vector>

template<class T>
class counter;

template<class T>
class counter_iterator {

    private:

    counter<T>* ptr;

    public:

    counter_iterator(counter<T>* t_ptr)
        :ptr(t_ptr)
    {

    }

    template<class U>
    counter_iterator(counter_iterator<U> const& other)
        :ptr(other.ptr)
    {

    }

    counter_iterator(counter_iterator<T> const& other)            = default;
    counter_iterator& operator=(counter_iterator<T> const& other) = default;
    counter_iterator(counter_iterator<T>&& other)            = default;
    counter_iterator& operator=(counter_iterator<T>&& other) = default;
    ~counter_iterator() = default;

    counter_iterator& operator++()
    {
        ptr = (*ptr).next;
        return *this;
    }

    counter_iterator operator++(int)
    {
        counter_iterator<T> copy(ptr);
        ++(*this);
        return copy;
    }

    T& operator*() 
    {
        return (*ptr).value;
    }

    T operator*()  const
    {
        return (*ptr).value;
    }

    bool operator==(counter_iterator<T> const& other) const
    {
        return ptr == other.ptr;
    }

    bool operator!=(counter_iterator<T> const& other) const
    {
        return ptr != other.ptr;
    }
};

template<class T>
class counter {

    friend class counter_iterator<T>;

    public:

    using iterator       = counter_iterator<T>;
    using const_iterator = counter_iterator<const T>;

    private:

    constexpr static char     separator = '\'';
    constexpr static counter* end_value = nullptr;

    std::size_t   max   = 0;
    T        value;
    counter* next = end_value;

    public:

    counter()
    {

    }

    counter(std::size_t t_max)
        :max(t_max),
        value(T(0))
    {

    }

    template<class Iterator>
    counter(Iterator iter,
            Iterator end)
    {
        max = *iter;
        if (++iter != end)
            next = new counter(iter, end);
    }

    template<class Iterable>
    counter(Iterable const& containor)
        :counter(containor.begin(), containor.end())
    {

    }

    counter(const counter& other)
        :max(other.max),
        value(other.value)
    {
        if (other.next != end_value)
            next = new counter(*other.next);
    }

    counter& operator=(counter const& other)
    {
        max = other.max;
        value = other.value;
        if (other.next != end_value)
            next = new counter(*other.next);
        else 
            next = end_value;

        return *this;
    }

    counter(counter&& other)            = default;
    counter& operator=(counter&& other) = default;

    ~counter()        
    {
        if (next != end_value)
            delete next;
    }

    std::size_t size() const
    {
        return (next != end_value) ? (1 + next->size()) : 1;
    }

    iterator begin() 
    {
        return counter_iterator<T>(this);
    }

    iterator end()
    {
        return end_value;
    }

    const_iterator begin() const
    {
        return counter_iterator<const T>(this);
    }

    const_iterator end() const 
    {
        return end_value;
    }

    void restart()
    {
        for (auto& value : *this)
            value = 0;
    }

    template<class Iterator>
    void getCount(Iterator iter) const
    {
        *iter = value;
        if (next != end_value)
            next->getCount(++iter);
    }

    template<class Iterator>
    void setCount(Iterator iter)
    {
        value = *iter;
        if (next != end_value)
            next->setCount(++iter);
    }

    void push_back(T max,
                   T value)
    {
        if (next != end_value)
            (*next).push_back(max, value);
        else
            next = new counter<T>(max, value);
    }

    counter& operator++() 
    {
        ++value;
        if (value == max) {
            value = 0;
            if (next != end_value)
                ++(*next);
        }
        return *this;
    }

    counter operator++(int) 
    {
        counter other(*this);
        ++(*this);
        return other;
    }

    T& operator[](std::size_t pos)
    {
        if (pos == 0)
            return value;
        return (*next)[pos-1];
    }

    T operator[](std::size_t pos) const
    {
        if (pos == 0)
            return value;
        return (*next)[pos-1];
    }

    operator bool() const 
    {
        return (value != 0 or (next != end_value and bool(*next)));
    }

    friend
    std::ostream& operator<<(std::ostream& out, counter const& c)
    {
        out << c.value;
        if (c.next != counter::end_value)
            out << counter::separator << *c.next;
        return out;
    }
};


#endif
