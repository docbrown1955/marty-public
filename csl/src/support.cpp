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

#include <iostream>
#include "support.h"

using namespace std;

namespace csl {

int PGCD(double a, double b)
{
    return PGCD((long long int)round(a), (long long int)round(b));
}

int PGCD(int a, int b)
{
    return (int)PGCD((long long)a, (long long)b);
}

long long int PGCD(long long int a, long long int b)
{
    if (a < 0) a *= -1;
    if (b < 0) b *= -1;
    if (a < b)
    {
        int foo = a;
        a = b;
        b = foo;
    }
    //if (a > 1000000) return 1; // too big number
    if (b == 0) return 1;
    return internal_PGCD(a,b);
}

int internal_PGCD(long long int a, long long int b)
{
    int r = a % b;
    if (r == 0) return b;
    if (r == 1) return 1;
    return internal_PGCD(b,r);
}

long long int sgn(long long int a)
{
    return (a>0) - (a<0);
}
int sgn(int a)
{
    return (int)sgn((long long)a);
}
int sgn(double a)
{
    return (a>0) - (a<0);
}

double factorial(int n)
{
    if (n > 200) n = 200;
    if (n < 0) return 1;
    if (n == 0) return 1;
    return internal_factorial(n);
}

long long int internal_factorial(long long int n)
{
    if (n <= 2) return n;
    return n*internal_factorial(n-1);
}

int compare(string_view a, string_view b)
    // return 1 if  a > b
    // return 0 if  a = b
    // return -1 if a < b
{
    const std::size_t sizeA = a.size();
    const std::size_t sizeB = b.size();
    if (sizeA > sizeB)
        return 1;
    else if (sizeA < sizeB)
        return -1;
    auto iterA = a.begin();
    auto iterB = b.begin();
    const auto endA = a.end();
    const auto endB = b.end();
    for (; iterA != endA and iterB != endB; ++iterA, ++iterB) {
        if (*iterA > *iterB)
            return 1;
        else if (*iterA < *iterB)
            return -1;
    }
    return 0;
}
    
bool operator>(const string& a, const string& b)
{
    return (compare(a, b) == 1);
}
    
bool operator<(const string& a, const string& b)
{
    return (compare(a, b) == -1);
}

bool operator>=(const string& a, const string& b)
{
    return (compare(a, b) >= 0);
}

bool operator<=(const string& a, const string& b)
{
    return (compare(a, b) <= 0);
}

vector<std::size_t> range(std::size_t n)
{
    return range(0, n, 1);
}

vector<std::size_t> range(std::size_t i, std::size_t n)
{
    return range(i, n, 1);
}

vector<std::size_t> range(std::size_t i, std::size_t n, std::size_t step)
{
    vector<std::size_t> rangeVector((n-i)/step);
    for (auto& element : rangeVector) {
        element = i;
        i += step;
    }

    return rangeVector;
}

} // End of namespace csl
