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
#pragma once

#include <iostream>
#include <vector>
#include <cassert>

namespace drawer {

template<typename T1, typename T2 = T1>
class Base_iterable {

    public:

    using Type      = T1;
    using PType     = T2;
    using size_type = size_t;
    using iterator        = typename std::vector<Type>::iterator;
    using const_iterator  = typename std::vector<Type>::const_iterator;

    Base_iterable()
        :elements(std::vector<Type>())
    {}

    explicit
    Base_iterable(size_t N)
        :elements(std::vector<Type>(N, Type()))
    {

    }

    explicit
    Base_iterable(std::vector<Type> const& t_elements)
        :elements(t_elements)
    {}

    explicit
    Base_iterable(std::vector<Type>&& t_elements)
        :elements(std::move(t_elements))
    {}

    virtual
    ~Base_iterable()
    {}

    Base_iterable(Base_iterable<T1, T2> const&) = default;
    Base_iterable(Base_iterable<T1, T2>&&)      = default;
    Base_iterable& operator=(Base_iterable<T1, T2> const&) = default;
    Base_iterable& operator=(Base_iterable<T1, T2>&&)      = default;

    void clear() {
        elements.clear();
    }

    bool empty() const {
        return elements.empty();
    }

    size_t size() const {
        return elements.size();
    }

    iterator begin() {
        return elements.begin();
    }

    const_iterator begin() const {
        return elements.begin();
    }

    iterator end() {
        return elements.end();
    }

    const_iterator end() const {
        return elements.end();
    }

    Type& operator[](size_t pos) {
        return elements[pos];
    }

    Type operator[](size_t pos) const {
        return elements[pos];
    }

    protected:

    std::vector<Type> elements;
};

template<typename T1, typename T2 = T1>
class Vector: public Base_iterable<T1, T2> {

    public:

    using Type      = typename Base_iterable<T1, T2>::Type;
    using size_type = typename Base_iterable<T1, T2>::size_type;
    using iterator       = typename Base_iterable<T1, T2>::iterator;
    using const_iterator = typename Base_iterable<T1, T2>::const_iterator;

    Vector(size_t N)
        :Base_iterable<T1, T2>(N)
    {}

    Vector(std::vector<Type> const& t_containor)
        :Base_iterable<T1, T2>(t_containor)
    {}

    Vector(std::vector<Type>&& t_containor)
        :Base_iterable<T1, T2>(std::move(t_containor))
    {}

    ~Vector()
    {}

    Vector(Vector<T1, T2> const&) = default;
    Vector(Vector<T1, T2>&&)      = default;
    Vector& operator=(Vector<T1, T2> const&) = default;
    Vector& operator=(Vector<T1, T2>&&)      = default;

    int find(T1 const el) const {
        for (auto iter = Base_iterable<T1, T2>::begin(); 
                iter != Base_iterable<T1, T2>::end();
                ++iter)
            if (*iter == el)
                return std::distance(iter, Base_iterable<T1, T2>::begin());
        return -1;
    }

    Type& operator()(size_t i) {
        return this->elements[i];
    }

    Type operator()(size_t i) const {
        return this->elements[i];
    }

    template<class U1, class U2>
    friend
    std::ostream& operator<<(std::ostream& out,
                             Vector<U1, U2> const& vec);
};

template<class U1, class U2>
std::ostream& operator<<(std::ostream& out,
                         Vector<U1, U2> const& vec)
{
    for (const auto& el : vec)
        if constexpr (std::is_same<U1, U2>::value)
            out << el << '\n';
        else
            out << U2(el) << '\n';
    return out;
}

template<typename T1, typename T2 = T1>
class Matrix: public Base_iterable<T1, T2> {

    public:

    using Type      = typename Base_iterable<T1, T2>::Type;
    using size_type = typename Base_iterable<T1, T2>::size_type;
    using iterator       = typename Base_iterable<T1, T2>::iterator;
    using const_iterator = typename Base_iterable<T1, T2>::const_iterator;

    Matrix(size_t M)
        :Base_iterable<T1, T2>(M * M),
        N(M)
    {}

    Matrix(std::vector<Type> const& t_containor)
        :Base_iterable<T1, T2>(t_containor),
        N(t_containor.size())
    {}

    Matrix(std::vector<Type>&& t_containor)
        :Base_iterable<T1, T2>(std::move(t_containor)),
        N(t_containor.size())
    {}

    ~Matrix()
    {}

    Matrix(Matrix<T1, T2> const&) = default;
    Matrix(Matrix<T1, T2>&&)      = default;
    Matrix& operator=(Matrix<T1, T2> const&) = default;
    Matrix& operator=(Matrix<T1, T2>&&)      = default;

    void clear() {
        Base_iterable<T1, T2>::clear();
        N = 0;
    }

    size_type size() const {
        return N;
    }

    Type& operator()(size_t i, size_t j) {
        return this->elements[this->size()*i + j];
    }

    Type operator()(size_t i, size_t j) const {
        return this->elements[this->size()*i + j];
    }

    template<class U1, typename U2>
    friend
    std::ostream& operator<<(std::ostream& out,
                             Matrix<U1, U2> const& mat);

    private:

    size_type N;
};

template<class U1, typename U2>
std::ostream& operator<<(std::ostream& out,
                         Matrix<U1, U2> const& mat)
{
    for (size_t i = 0; i != mat.size(); ++i) {
        for (size_t j = 0; j != mat.size(); ++j)
            if constexpr (std::is_same<U1, U2>::value)
                out << mat(i, j) << "  ";
            else
                out << U2(mat(i, j)) << "  ";
        out << '\n';
    }
    return out;
}

template<class T1, class T2, class T3>
Vector<T1, T2> operator+(Vector<T1, T2> const& vec, 
                         T3                    value)
{
    Vector<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Vector<T1, T2> operator+(T3                    value,
                         Vector<T1, T2> const& vec)
{
    Vector<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Vector<T1, T2> operator*(Vector<T1, T2> const& vec, 
                         T3                    value)
{
    Vector<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Vector<T1, T2> operator*(T3                    value,
                         Vector<T1, T2> const& vec)
{
    Vector<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}


template<class T1, class T2, class T3>
Matrix<T1, T2> operator+(Matrix<T1, T2> const& vec, 
                         T3                    value)
{
    Matrix<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Matrix<T1, T2> operator+(T3                    value,
                         Matrix<T1, T2> const& vec)
{
    Matrix<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Matrix<T1, T2> operator*(Matrix<T1, T2> const& vec, 
                         T3                    value)
{
    Matrix<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3>
Matrix<T1, T2> operator*(T3                    value,
                         Matrix<T1, T2> const& vec)
{
    Matrix<T1, T2> copy(vec);
    for (auto icopy = copy.begin(), ivec = vec.begin();
            icopy != copy.end();
            ++icopy, ++ivec)
        icopy *= value;

    return copy;
}

template<class T1, class T2, class T3, class T4>
T1 operator*(Vector<T1, T2> const& left, 
             Vector<T3, T4> const& right)
{
    assert(left.size() == right.size());
    T1 init(0);
    for (auto ileft = left.begin(), iright = right.begin();
            ileft != left.end();
            ++ileft, ++iright) 
        init += (*ileft) * (*iright);

    return init;
}

template<class T1, class T2, class T3, class T4>
Vector<T1, T2> operator*(Vector<T1, T2> const& left, 
                         Matrix<T3, T4> const& right)
{
    assert(left.size() == right.size());
    Vector<T1, T2> init(left.size());
    auto imat = right.begin();

    for (auto ivec = left.begin(); ivec != left.end(); ++ivec)
        for (auto ires = init.begin(); ires != init.end(); ++ires, ++imat) 
            *ires += (*imat) * (*ivec);

    return init;
}

template<class T1, class T2, class T3, class T4>
Vector<T1, T2> operator*(Matrix<T1, T2> const& left, 
                         Vector<T3, T4> const& right)
{
    assert(left.size() == right.size());
    Vector<T1, T2> init(right.size());
    auto imat = left.begin();

    for (auto ires = init.begin(); ires != init.end(); ++ires) 
        for (auto ivec = right.begin(); ivec != right.end(); ++ivec, ++imat)
            *ires += (*imat) * (*ivec);

    return init;
}

template<class U1, class U2>
Matrix<U1, U2> operator*(Matrix<U1, U2> const& A,
                         Matrix<U1, U2> const& B)
{
    assert(A.size() == B.size());
    const size_t s = A.size();
    Matrix<U1, U2> C(s);
    for (size_t i = 0; i != s; ++i)
        for (size_t j = 0; j != s; ++j)
            for (size_t k = 0; k != s; ++k)
                C(i, j) += A(i, k) * B(k, j);

    return C;
}

template<class U1, class U2>
Matrix<U1, U2>& operator*=(Matrix<U1, U2>&       A,
                           Matrix<U1, U2> const& B)
{
    return A = std::move(A * B);
}

template<class U1, class U2, class Int_type>
Matrix<U1, U2> pow(Matrix<U1, U2> const& mat, Int_type value)
{
    assert(value > 0);
    Matrix<U1, U2> res = mat;
    for (size_t i = 1; i < value; ++i)
        res = res * mat;
    return res;
}


}
