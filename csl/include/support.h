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

/*! \file support.h
 * \author Grégoire Uhlrich
 * \version 1.0
 * \brief Some useful functions.
 */
#ifndef SUPPORT_H_INCLUDED
#define SUPPORT_H_INCLUDED

#include <cmath>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <functional>


template<class T>
std::string toString(T obj)
{
    std::ostringstream sout;
    sout << obj;
    return sout.str();
}

/*!
 * \brief Namespace for csl library
 */
namespace csl {

/*! 
 * \brief Returns the PGCD of a and b.
 * \details If \a a and \a b are integers ( \a a == floor(\a a) and also for \a b)
 * this function calls PGCD(int a, int b).
 * \param a
 * \param b
 * \return PGCD(a,b) if \a a and \a b are integers.
 * \return \b 1 else.
 */
int PGCD(double a, double b);

/*! 
 * \brief Returns the PGCD of a and b.
 * \details Proceeds to verifications on \a a and \a b (sign, non zero, etc) and 
 * calls internal_PGCD(int a, int b) if everythiong is Ok.
 * \param a
 * \param b
 * \return PGCD(a,b)
 */
long long int PGCD(long long int a, long long int b);

int PGCD(int a, int b);

/*! 
 * \brief Returns the PGCD of a and b.
 * \details This function is called only by PGCD(int a, int b) that
 * do the nexessary checks on a and b before the calculation. Then the
 * algorithm implemented in internal_PGCD() is Euclid's (recursive). The
 * separation of the cjeck function PGCD() and the calculation function
 * internal_PGCD() allows to check only once \a a and \a b since the 
 * algorithm is recursive.
 * \param a
 * \param b
 * \return PGCD(a,b)
 */
int internal_PGCD(long long int a, long long int b);

/*! 
 * \brief Returns the sign of a.
 * \param a
 * \return sign(a)
 */
long long int sgn(long long int a); 

int sgn(int a); 

/*! 
 * \brief Returns the sign of a.
 * \param a
 * \return sign(a)
 */
int sgn(double a); 

/*! 
 * \brief Returns the factorial of a.
 * \param a
 * \return \b a!
 * \bug The factorial function returns a double for now and so is quickly
 * out of bounds for exact numerical precision. The limit is between 20! 
 * and 25!.
 */
double factorial(int n);

/*!
 * \brief Returns the factorial of a.
 * \param a
 * \return \b a!
 */
long long int internal_factorial(long long int n);

int compare(std::string_view a, std::string_view b);

bool operator>(const std::string& a, const std::string& b);

bool operator<(const std::string& a, const std::string& b);

bool operator>=(const std::string& a, const std::string& b);

bool operator<=(const std::string& a, const std::string& b);

std::vector<size_t> range(size_t n);

std::vector<size_t> range(size_t i, size_t n);

std::vector<size_t> range(size_t i, size_t n, size_t step);

/*!
 * \brief Template function that compares the elements in two vectors \b A and
 * \b B, independently on their order.
 *
 * \tparam T Type of data, must have a well-defined operator==.
 * \param A Left hand side.
 * \param B Right hand side.
 *
 * \return \b True if all elements in \b A (no more no less) are present in \B,
 * independently on their order.
 */
template <typename T>
bool comparePlaceIndependant(std::vector<T> A,
                             std::vector<T> B)
{
    const size_t size = A.size();
    if (size != B.size())
        return false;
    std::vector<int> indicesLeft(size);
    for (size_t i = 0; i != size; ++i) 
        indicesLeft[i] = i;

    for (size_t iA = 0; iA != size; ++iA) {
        bool matched = false;
        for (size_t iLeft = 0; iLeft != indicesLeft.size(); ++iLeft)  {
            size_t iB = indicesLeft[iLeft];
            if (A[iA] == B[iB]) {
                matched = true;
                indicesLeft.erase(indicesLeft.begin()+iLeft);
                break;
            }
        }
        if (not matched)
            return false;
    }

    return true;
}
    
/*!
 * \brief Template function that compares the elements in two vectors \b A and
 * \b B, independently on their order, \b A can be bigger, this function 
 * determines if all elements in \b B are present in \b A.
 *
 * \tparam T Type of data, must have a well-defined operator==.
 * \param A Left hand side.
 * \param B Right hand side.
 *
 * \return \b True if all elements in \b B (and possibly more) are present in 
 * \b A independently on their order.
 */
template <typename T>
bool partialComparePlaceIndependant(std::vector<T> A, std::vector<T> B)
{
    if (A.size() < B.size())
        return false;
    std::vector<int> indicesLeft(B.size());
    for (size_t i = 0; i != B.size(); ++i) 
        indicesLeft[i] = i;

    for (size_t iA = 0; iA != A.size(); ++iA) {
        for (size_t iLeft = 0; iLeft != indicesLeft.size(); ++iLeft)  {
            size_t iB = indicesLeft[iLeft];
            if (A[iA] == B[iB]) {
                indicesLeft.erase(indicesLeft.begin()+iLeft);
                break;
            }
        }
    }

    return (indicesLeft.empty());
}

} // End of namespace csl

#endif
