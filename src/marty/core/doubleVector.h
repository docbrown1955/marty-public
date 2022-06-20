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
 * @version 2.0

 * \brief
 */
#ifndef DOUBLE_VECTOR_H_INCLUDED
#define DOUBLE_VECTOR_H_INCLUDED

#include <iostream>
#include <vector>

namespace mty {

template <typename Type>
class double_vector;
template <typename Type, class double_vector_type>
class double_vector_iterator;

template <typename Type, class double_vector_type>
class double_vector_single_view {
    friend class double_vector<Type>;
    friend class double_vector_iterator<Type, double_vector_type>;

  public:
    using iterator       = typename std::vector<Type>::iterator;
    using const_iterator = typename std::vector<Type>::const_iterator;

    double_vector_single_view(double_vector_type t_data, size_t t_pos)
        : data(t_data), pos(t_pos - 1)
    {
    }

    double_vector_single_view(double_vector_single_view const &other) = delete;
    double_vector_single_view &
    operator=(double_vector_single_view const &other)
    {
        size_t min = std::min(size(), other.size());
        for (size_t i = 0; i != min; ++i)
            (*this)[i] = other[i];
        if (size() == min) {
            for (size_t i = min; i != other.size(); ++i)
                push_back(other[i]);
        }
        else {
            for (size_t i = min; i != size(); ++i)
                erase(begin() + min);
        }

        return *this;
    }

    void empty() const
    {
        return size() == 0;
    }
    size_t size() const
    {
        return data->getCut(pos + 1) - data->getCut(pos);
    }

    void push_back(Type const &el);

    void erase(const_iterator pos);

    iterator begin()
    {
        return data->vecBegin() + data->getCut(pos);
    }
    iterator end()
    {
        return data->vecBegin() + data->getCut(1 + pos);
    }

    const_iterator begin() const
    {
        return data->vecBegin() + data->getCut(pos);
    }

    const_iterator end() const
    {
        return data->vecBegin() + data->getCut(1 + pos);
    }

    Type &      operator[](size_t i);
    Type const &operator[](size_t i) const;

  private:
    double_vector_type data;
    size_t             pos;
};

template <typename Type, class double_vector_type>
class double_vector_iterator {

  public:
    double_vector_iterator(double_vector_type t_data)
        : double_vector_iterator(t_data, 0)
    {
    }

    double_vector_iterator(double_vector_type t_data, size_t pos)
        : data(t_data), current(t_data, pos)
    {
    }

    double_vector_iterator(
        double_vector_iterator<Type, double_vector_type> const &other)
        : data(other.data), current(other.data, other.current.pos + 1)
    {
    }

    double_vector_iterator &
    operator=(double_vector_iterator<Type, double_vector_type> const &other)
    {
        data        = other.data;
        current.pos = other.current.pos;
        return *this;
    }

    bool operator==(double_vector_iterator const &other) const
    {
        return data == other.data and current.pos == other.current.pos;
    }
    bool operator!=(double_vector_iterator const &other) const
    {
        return !(*this == other);
    }

    double_vector_single_view<Type, double_vector_type> &operator*()
    {
        return current;
    }
    double_vector_single_view<Type, double_vector_type> operator*() const
    {
        return current;
    }
    double_vector_single_view<Type, double_vector_type> *operator->()
    {
        return &current;
    }
    double_vector_single_view<Type, double_vector_type> const *
    operator->() const
    {
        return &current;
    }

    double_vector_iterator &operator++()
    {
        ++current.pos;
        return *this;
    }
    double_vector_iterator operator++(int)
    {
        auto copy = *this;
        ++*this;
        return copy;
    }

    double_vector_iterator &operator--()
    {
        --current.pos;
        return *this;
    }
    double_vector_iterator operator--(int)
    {
        auto copy = *this;
        --*this;
        return copy;
    }

    size_t operator-(
        double_vector_iterator<Type, double_vector_type> const &other) const
    {
        return current.pos - other.current.pos;
    }
    double_vector_iterator<Type, double_vector_type>
    operator+(size_t diff) const
    {
        auto copy(*this);
        copy.current.pos += diff;
        return copy;
    }
    double_vector_iterator<Type, double_vector_type>
    operator-(size_t diff) const
    {
        auto copy(*this);
        copy.current.pos -= diff;
        return copy;
    }

  private:
    double_vector_type                                  data;
    double_vector_single_view<Type, double_vector_type> current;
};

template <typename Type>
class double_vector {
    friend class double_vector_single_view<Type, double_vector<Type> *>;
    friend class double_vector_single_view<Type, double_vector<Type> const *>;

  public:
    using iterator = double_vector_iterator<Type, double_vector<Type> *>;
    using const_iterator
        = double_vector_iterator<Type, double_vector<Type> const *>;

    double_vector(std::vector<Type> const &init) : data(init), cut(init.size())
    {
    }
    double_vector(std::initializer_list<Type> init)
        : data(init), cut(init.size())
    {
    }
    double_vector()                           = default;
    double_vector(double_vector const &other) = default;
    double_vector(double_vector &&other)      = default;
    double_vector &operator=(double_vector const &other) = default;
    double_vector &operator=(double_vector &&other) = default;
    ~double_vector()                                = default;

    bool empty() const
    {
        return size() == 0;
    }
    size_t size() const
    {
        return cut.size();
    }

    Type &getElement(size_t i)
    {
        return data[i];
    }
    Type const &getElement(size_t i) const
    {
        return data[i];
    }

    size_t getCut(size_t i) const
    {
        return (i == size_t(-1)) ? 0 : cut[i];
    }

    std::vector<Type> const &getData() const
    {
        return data;
    }
    std::vector<size_t> const &getCuts() const
    {
        return cut;
    }

    void push_back(std::vector<Type> const &row);
    void push_back(std::vector<Type> &&row);

    iterator begin()
    {
        return iterator(this);
    }

    iterator end()
    {
        return iterator(this, cut.size());
    }

    const_iterator begin() const
    {
        return const_iterator(this);
    }

    const_iterator end() const
    {
        return const_iterator(this, cut.size());
    }

    double_vector_single_view<Type, double_vector<Type> *>
    operator[](size_t i);
    const double_vector_single_view<Type, double_vector<Type> const *>
    operator[](size_t i) const;

  private:
    typename std::vector<Type>::iterator vecBegin()
    {
        return data.begin();
    }
    typename std::vector<Type>::const_iterator vecBegin() const
    {
        return data.begin();
    }
    typename std::vector<Type>::iterator vecEnd()
    {
        return data.end();
    }
    typename std::vector<Type>::const_iterator vecEnd() const
    {
        return data.end();
    }

    void push_row(size_t pos, Type const &el);

    void eraseEl(size_t pos, size_t pos_j);

  private:
    std::vector<Type>   data;
    std::vector<size_t> cut;
};

template <typename Type, class double_vector_type>
void double_vector_single_view<Type, double_vector_type>::push_back(
    Type const &el)
{
    data->push_row(pos, el);
}

template <typename Type, class double_vector_type>
void double_vector_single_view<Type, double_vector_type>::erase(
    const_iterator i)
{
    data->eraseEl(data->getCut(pos), i - begin());
}

template <typename Type, class double_vector_type>
Type &double_vector_single_view<Type, double_vector_type>::operator[](size_t i)
{
    return data->getElement(data->getCut(pos) + i);
}

template <typename Type, class double_vector_type>
Type const &
double_vector_single_view<Type, double_vector_type>::operator[](size_t i) const
{
    return data->getElement(data->getCut(pos) + i);
}

template <typename Type>
void double_vector<Type>::push_back(std::vector<Type> const &row)
{
    if (cut.empty())
        cut.push_back(row.size());
    else
        cut.push_back(cut.back() + row.size());
    data.insert(data.end(), row.begin(), row.end());
}

template <typename Type>
void double_vector<Type>::push_back(std::vector<Type> &&row)
{
    if (cut.empty())
        cut.push_back(row.size());
    else
        cut.push_back(cut.back() + row.size());
    data.insert(data.end(),
                std::make_move_iterator(row.begin()),
                std::make_move_iterator(row.end()));
}

template <typename Type>
void double_vector<Type>::eraseEl(size_t pos, size_t pos_j)
{
    data.erase(data.begin() + pos + pos_j);
    for (size_t i = pos + 1; i < cut.size(); ++i)
        --cut[i];
}

template <typename Type>
void double_vector<Type>::push_row(size_t pos, Type const &el)
{
    size_t insertPos = getCut(pos + 1);
    data.insert(data.begin() + insertPos, el);
    for (size_t i = pos + 1; i < cut.size(); ++i)
        ++cut[i];
}

template <typename Type>
double_vector_single_view<Type, double_vector<Type> *>
double_vector<Type>::operator[](size_t i)
{
    return double_vector_single_view<Type, double_vector<Type> *>(this, i);
}

template <typename Type>
const double_vector_single_view<Type, double_vector<Type> const *>
double_vector<Type>::operator[](size_t i) const
{
    return double_vector_single_view<Type, double_vector<Type> const *>(this,
                                                                        i);
}

} // End of namespace mty

#endif
