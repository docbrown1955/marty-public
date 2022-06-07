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
#ifndef MTY_MATRIX_H_INCLUDED
#define MTY_MATRIX_H_INCLUDED

#ifdef USE_BOOST
#include <boost/numeric/ublas/matrix.hpp>
#else

#include <vector>
#include <iostream>

namespace boost::numeric::ublas {

template<class T>
class matrix {

public:
    matrix() 
        :m_size1(0), m_size2(0)
    {}

    matrix(std::size_t s1, std::size_t s2)
	:m_size1(s1), m_size2(s2),
	data(std::vector<T>(s1*s2, 0))
    {}

    matrix(matrix const &) = default;
    matrix(matrix &&) = default;
    matrix &operator=(matrix const &) = default;
    matrix &operator=(matrix &&) = default;

    ~matrix() {}

    void resize(std::size_t s1, std::size_t s2) {
        m_size1 = s1;
        m_size2 = s2;
        data = std::vector<T>(s1*s2, 0);
    }

    std::size_t size1() const { return m_size1; }
    std::size_t size2() const { return m_size2; }

    T const &operator()(std::size_t i, std::size_t j) const {
	return data[i*m_size1 + j];
    }
    T &operator()(std::size_t i, std::size_t j) {
	return data[i*m_size1 + j];
    }

protected:
    std::size_t         m_size1;
    std::size_t         m_size2;
    std::vector<T> data;
};

template<class T>
class identity_matrix: public matrix<T> {

public:
    identity_matrix(std::size_t s)
         :matrix<T>(s, s)
    {
        for (std::size_t i = 0; i != this->m_size1; ++i)
	    (*this)(i, i) = 1;
    }

    identity_matrix(identity_matrix<T> const &) = default;
    identity_matrix(identity_matrix<T> &&) = default;
    identity_matrix &operator=(identity_matrix<T> const &) = default;
    identity_matrix &operator=(identity_matrix<T> &&) = default;
};

template<class T>
std::ostream &operator<<(
        std::ostream &out,
        matrix<T> const &m
        )
{
    for (std::size_t i = 0; i < m.size1(); ++i) {
        for (std::size_t j = 0; j != m.size2(); ++j) {
            out << m(i, j) << ", ";
        }
        out << ",\n";
    }
    return out;
}

} // End of namespace boost::numeric::ublas
#endif // #ifdef USE_BOOST

#endif // #ifndef MTY_MATRIX_H_INCLUDED
