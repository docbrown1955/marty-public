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
#ifndef PRECISION_H_INCLUDED
#define PRECISION_H_INCLUDED

#include <vector>
#include <iostream>
#include <cmath>
#include <csignal>

namespace csl {

extern std::size_t PRECISION;

extern bool WARN_OVERFLOW;

void fillDigits(const std::vector<short>& digits,
                std::vector<short>&       receiver);

template<const std::size_t base>
long double log(long double x) 
{
#include "math.h"
    return log(x)/log((long double)base);
}

char convertDigit(short digit);

template<std::size_t base = 10>
class float_sap;

template<std::size_t base = 10>
class int_ap{

    template<std::size_t t_base>
    friend class float_sap;

    public:

    bool minusSign = false;

    std::vector<short> digits;

    public:

    int_ap();

    explicit
    int_ap(long long int value);

    explicit
    int_ap(const std::vector<short>& t_digits);

    explicit
    int_ap(bool                      t_minusSign,
           const std::vector<short>& t_digits);

    int_ap(const int_ap& other) = default;

    ~int_ap(){};

    int_ap<base>& operator=(long long int value);

    int_ap<base>& operator=(const int_ap<base>& value);

    template<std::size_t targetBase>
    int_ap<targetBase> convert() const
    {
        int_ap<targetBase> res(0);
        int_ap<base> target_ap(targetBase);
        int_ap<targetBase> targetBasePower(1);
        int_ap<base> X = *this;

        bool sign = false;
        if (X.minusSign) {
            sign = true;
            X.setMinusSign(false);
        }
        while (X != 0) {
            int_ap<base> divisor, rest;
            divide(X, target_ap, divisor, rest);
            X = divisor;
            res += int_ap<targetBase>(rest.toCInteger()) * targetBasePower;
            targetBasePower.shiftLeft();
        }
        res.setMinusSign(sign);

        return res;
    }

    std::size_t size() const;

    bool empty() const;

    short operator[](std::size_t i) const;

    bool getMinusSign() const;

    void setMinusSign(bool t_minusSign);

    void flipSign();

    void flip(std::size_t i);

    int_ap flip() const;

    int_ap all() const;

    void shiftLeft(std::size_t n = 1);

    void shiftRight(std::size_t n = 1);

    void cut();

    void cutRight();

    long long int toCInteger() const;

    friend
    std::ostream& operator<<(std::ostream& fout, const int_ap<base>& integer)
    {
        if (integer.minusSign)
            fout << "-";
        if (base == 16)
            fout << "0x";
        for (auto digit = integer.digits.rbegin();
                  digit != integer.digits.rend();
                  ++digit)
            fout << convertDigit(*digit);

        return fout;
    }

    friend
    int_ap<base> operator+(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)+b;
    }

    friend
    int_ap<base> operator+(const int_ap<base>& a, long long int b)
    {
        return a+int_ap<base>(b);
    }

    friend
    int_ap<base> operator+(const int_ap<base>& a, const int_ap<base>& b)
    {
        int_ap<base> c = a;
        return c += b;
    }

    friend
    int_ap<base>& operator+=(int_ap<base>& a, int_ap<base> b)
    {
        if (a.minusSign xor b.minusSign) {
            if (b.minusSign) {
                return a -= (-b);
            }
            else {
                a.setMinusSign(false);
                a -= b;
                a.flipSign();
                return a;
            }
        }
        if (a.minusSign) {
            a.setMinusSign(false);
            a += (-b);
            a = -a;
            return a;
        }
        if (a.size() < b.size()) {
            int_ap<base> c = std::move(a);
            a = b;
            return a += c;
        }

        short retenue = 0;
        const int n = b.digits.size();

        for (int i = 0; i < n; ++i) {
            short sum = retenue + a.digits[i] + b.digits[i];
            if (sum > (int)base-1) {
                retenue = 1;
                sum = sum % base;
            }
            else
                retenue = 0;
            a.digits[i] = sum;
        }
        for (int i = n; i < (int)a.digits.size(); ++i) {
            short sum = retenue + a.digits[i];
            if (sum > (int)base-1) {
                retenue = 1;
                sum = sum % base;
            }
            else 
                retenue = 0;
            a.digits[i] = sum;
        }
        if (retenue != 0)
            a.digits.push_back(retenue);

        return a;
    }

    friend
    int_ap<base>& operator+=(int_ap<base>& a, long long int b)
    {
        return a += int_ap<base>(b);
    }

    friend
    int_ap<base> operator-(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a) - b;
    }

    friend
    int_ap<base> operator-(const int_ap<base>& a, long long int b)
    {
        return a - int_ap<base>(b);
    }

    friend
    int_ap<base> operator-(const int_ap<base>& a, const int_ap<base>& b)
    {
        int_ap<base> c = a;
        return c -= b;
    }

    friend
    int_ap<base>& operator-=(int_ap<base>& a, int_ap<base> b)
    {
        if (a.minusSign xor b.minusSign) {
            if (a.minusSign) {
                a.setMinusSign(false);
                a += b;
                a.flipSign();
                return a;
            }
            else {
                return a += (-b);
            }
        }
        if (a.minusSign) {
            a.setMinusSign(false);
            a -= (-b);
            a.flipSign();
            return a;
        }
        if (not (a > b)) {
            if (a == b) {
                a = 0;
                return a;
            }
            int_ap<base> c = std::move(a);
            a = b;
            a -= c;
            a.flipSign();
            return a;
        }
        // Here a > 0, b > 0 and a > b
        short retenue = 0;
        for (std::size_t i = 0; i != b.size(); ++i) {
            short res = base + a[i] - (b[i] + retenue);
            a.digits[i] = res % base;
            retenue = res < (int)base;
        }
        if (a.size() > b.size()) {
            for (std::size_t j = b.size(); j != a.size(); ++j) {
                if (retenue) {
                    if (a[j] == 0)
                        a.digits[j] = base - 1;
                    else {
                        a.digits[j] = a[j] - 1;
                        retenue = 0;
                    }
                }
            }
        }
        a.cut();

        return a;
    }

    friend
    int_ap<base>& operator-=(int_ap<base>& a, long long int b)
    {
        return a -= int_ap<base>(b);
    }

    friend
    int_ap<base> operator++(int_ap<base>& a, int)
    {
        int_ap<base> b = a;
        a += 1;
        return b;
    }

    friend
    int_ap<base>& operator++(int_ap<base>& a)
    {
        a += 1;
        return a;
    }

    friend
    int_ap<base> operator--(int_ap<base>& a, int)
    {
        int_ap<base> b = a;
        a -= 1;
        return b;
    }

    friend
    int_ap<base>& operator--(int_ap<base>& a)
    {
        a -= 1;
        return a;
    }

    friend
    int_ap<base> operator-(const int_ap<base>& a)
    {
        return int_ap<base>(not a.minusSign or a == 0,
                      a.digits);
    }

    friend
    int_ap<base> operator*(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)*b;
    }

    friend
    int_ap<base> operator*(const int_ap<base>& a, long long int b)
    {
        return a*int_ap<base>(b);
    }

    friend
    int_ap<base> operator*(const int_ap<base>& a, const int_ap<base>& b)
    {
        int_ap<base> c = a;
        return c *= b;
    }

    friend
    int_ap<base>& operator*=(int_ap<base>& a, long long int b)
    {
        return a *= int_ap<base>(b);
    }

    friend
    int_ap<base>& operator*=(int_ap<base>& a, int_ap<base> b)
    {
        std::vector<short> newDigits(0);
        int_ap<base> res;
        for (std::size_t i = 0; i != a.digits.size(); ++i) {
            std::vector<short> intermediateDigits(i,0);
            short retenue = 0;
            for (std::size_t j = 0; j != b.digits.size(); ++j) {
                int product = retenue + a.digits[i]*b.digits[j];
                if (product > (int)base-1) {
                    retenue = product / base;
                    product = product % base;
                }
                else 
                    retenue = 0;
                intermediateDigits.push_back(product);
            }
            if (retenue != 0)
                intermediateDigits.push_back(retenue);

            res += int_ap<base>(intermediateDigits);
        }
        a.digits = res.digits;
        a.setMinusSign(a.minusSign xor b.minusSign);

        return a;
    }

    friend
    int_ap<base> operator/(const int_ap<base>& a, const int_ap<base>& b)
    {
        int_ap<base> res, rest;
        divide(a, b, res, rest);
        return res;
    }

    friend 
    void divide(int_ap<base>  a,
                int_ap<base>  b,
                int_ap<base>& result,
                int_ap<base>& rest)
    {
        if (abs(a) < abs(b)) {
            result = int_ap<base>(0);
            rest   = a;
            return;
        }
        if (b == 0) {
            std::cout << "Division by 0 encountered in int_ap<" 
                      << base << ">.\n";
            std::raise(SIGFPE);
        }
        bool sign  = a.minusSign xor b.minusSign;
        bool signA = a.minusSign;
        a.minusSign = false;
        b.minusSign = false;

        std::vector<short> a_digits;
        b.shiftLeft();
        int nDigits = 0;
        while (a > b) {
            a_digits.push_back(a.digits[0]);
            a.shiftRight();
            ++nDigits;
        }
        b.shiftRight();

        int_ap<base> prod(0);
        int_ap<base> divisor(0);
        for (std::size_t i = 0; i != base; ++i) {
            int_ap<base> foo = prod + b;
            if (foo > a)
                break;
            prod = foo;
            ++divisor;
        }
        rest = a - prod;

        rest.shiftLeft(nDigits);
        divisor.shiftLeft(nDigits);

        if (a_digits.empty()) {
            result = divisor;
        }
        else {
            rest += int_ap(a_digits);
            divide(rest, b, result, rest);
            result += divisor;
        }
        result.setMinusSign(sign);
        rest  .setMinusSign(signA);
    }

    friend 
    void dividePrecision(int_ap<base>  a,
                         int_ap<base>  b,
                         int_ap<base>& result,
                         int_ap<base>& exponent)
    {
        result   = int_ap<base>(0);
        int_ap<base> rest;
        // std::cout << "b = " << b << std::endl;
        divide(a, b, result, rest);
        exponent = int_ap<base>(result.size());
        while (rest != 0 and result.digits.size()+1 < PRECISION) {
            --exponent;
            result.shiftLeft();
            rest.shiftLeft();
            int_ap<base> intermediate;
            divide(rest, b, intermediate, rest);
            result += intermediate;
        }
    }

    friend
    int_ap<base> operator/(const int_ap<base>& a, long long int b)
    {
        return a / int_ap<base>(b);
    }

    friend 
    int_ap<base> operator/(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a) / b;
    }

    friend
    int_ap<base>& operator/=(int_ap<base>& a, long long int b)
    {
        return a /= int_ap<base>(b);
    }

    friend
    int_ap<base>& operator/=(int_ap<base>& a, const int_ap<base>& b)
    {
        return a = (a / b);
    }

    friend
    int_ap<base> operator%(const int_ap<base>& a, const int_ap<base>& b)
    {
        int_ap<base> divisor, rest;
        divide(a, b, divisor, rest);

        return rest;
    }

    friend
    int_ap<base> operator%(const int_ap<base>& a, long long int b)
    {
        return a % int_ap<base>(b);
    }

    friend
    int_ap<base> operator%(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a) % b;
    }

    friend
    int_ap<base>& operator%=(int_ap<base>& a, long long int b)
    {
        return a %= int_ap<base>(b);
    }

    friend 
    int_ap<base>& operator%=(int_ap<base>& a, const int_ap<base>& b)
    {
        return a = (a % b);
    }

    friend 
    int_ap<base> pow(long long int a, const int_ap<base>& b)
    {
        return pow(int_ap<base>(a), b);
    }

    friend 
    int_ap<base> pow(const int_ap<base>& a, long long int b)
    {
        return pow(a, int_ap<base>(b));
    }

    friend 
    int_ap<base> pow(const int_ap<base>& a, const int_ap<base>& b)
    {
        if (b <= 0) {
            if (a != 0 and b == 0)
                return int_ap<base>(1);
            return int_ap<base>(0);
        }
        if (b == 1)
            return a;
        int_ap<base> res = a;
        int_ap<base> totalPow(1);
        while (true) {
            if (totalPow*2 <= b)  {
                res      *= res;
                totalPow *= 2;
            }
            else if (totalPow == b)
                return res;
            else {
                return res *= pow(a, b-totalPow);
            }
        }
    }

    friend
    int_ap<base> abs(const int_ap<base>& a)
    {
        int_ap<base> b(a);
        b.minusSign = false;
        return b;
    }

    friend
    bool operator==(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)==b;
    }

    friend
    bool operator==(const int_ap<base>& a, long long int b)
    {
        return a==int_ap<base>(b);
    }

    friend
    bool operator==(const int_ap<base>& a, const int_ap<base>& b)
    {
        if (a.minusSign != b.minusSign)
            return false;
        if (a.digits.size() != b.digits.size())
            return false;

        for (std::size_t i = 0; i != a.digits.size(); ++i)
            if (a.digits[i] != b.digits[i])
                return false;
        return true;
    }

    friend
    bool operator!=(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)!=b;
    }

    friend
    bool operator!=(const int_ap<base>& a, long long int b)
    {
        return a!=int_ap<base>(b);
    }

    friend
    bool operator!=(const int_ap<base>& a, const int_ap<base>& b)
    {
        return not (a == b);
    }

    friend
    bool operator<(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)<b;
    }

    friend
    bool operator<(const int_ap<base>& a, long long int b)
    {
        return a<int_ap<base>(b);
    }

    friend
    bool operator<(const int_ap<base>& a, const int_ap<base>& b)
    {
        if (a.minusSign != b.minusSign)
            return a.minusSign;
        if (a.minusSign)
            return -a > -b;
        if (a.digits.size() < b.digits.size())
            return true;
        if (b.digits.size() < a.digits.size())
            return false;
        for (std::size_t i = a.size(); i --> 0 ;) {
            if (a.digits[i] < b.digits[i])
                return true;
            else if (b.digits[i] != a.digits[i])
                return false;
        }
        return false;
    }

    friend
    bool operator<=(long long int a, const int_ap<base>& b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator<=(const int_ap<base>& a, long long int b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator<=(const int_ap<base>& a, const int_ap<base>& b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator>(long long int a, const int_ap<base>& b)
    {
        return int_ap<base>(a)>b;
    }

    friend
    bool operator>(const int_ap<base>& a, long long int b)
    {
        return a>int_ap<base>(b);
    }

    friend
    bool operator>(const int_ap<base>& a, const int_ap<base>& b)
    {
        return b < a;
    }

    friend
    bool operator>=(long long int a, const int_ap<base>& b)
    {
        return (a>b or a==b);
    }

    friend
    bool operator>=(const int_ap<base>& a, long long int b)
    {
        return (a>b or a==b);
    }

    friend
    bool operator>=(const int_ap<base>& a, const int_ap<base>& b)
    {
        return (a>b or a==b);
    }

};

template<std::size_t base>
int_ap<base>::int_ap(): digits(std::vector<short>(1,0)){}

template<std::size_t base>
int_ap<base>::int_ap(long long int value)
{
    *this = value;
    cut();
}

template<std::size_t base>
int_ap<base>::int_ap(const std::vector<short>& t_digits)
{
    fillDigits(t_digits, digits);
    cut();
}

template<std::size_t base>
int_ap<base>::int_ap(bool                      t_minusSign,
                       const std::vector<short>& t_digits)
{
    fillDigits(t_digits, digits);
    cut();
    setMinusSign(t_minusSign);
}

template<std::size_t base>
std::size_t int_ap<base>::size() const
{
    return digits.size();
}

template<std::size_t base>
bool int_ap<base>::empty() const
{
    return digits.empty();
}

template<std::size_t base>
short int_ap<base>::operator[](std::size_t i) const
{
    return digits[i];
}

template<std::size_t base>
bool int_ap<base>::getMinusSign() const
{
    return minusSign;
}

template<std::size_t base>
void int_ap<base>::setMinusSign(bool t_minusSign)
{
    minusSign = t_minusSign;
    if (minusSign and digits.size() == 1 and digits[0] == 0)
        // -0 = 0
        minusSign = false;
}

template<std::size_t base>
void int_ap<base>::flipSign()
{
    setMinusSign(not minusSign);
}

template<std::size_t base>
void int_ap<base>::flip(std::size_t i) 
{
    digits[i] = base - digits[i];
}

template<std::size_t base>
int_ap<base> int_ap<base>::flip() const
{
    std::vector<short> newDigits(digits.size());
    for (std::size_t i = 0; i != digits.size(); ++i)
        newDigits[i] = base - digits[i];
    return int_ap<base>(minusSign, newDigits);
}

template<std::size_t base>
int_ap<base> int_ap<base>::all() const
{
    return int_ap<base>(minusSign, std::vector<short>(digits.size(), base-1));
}

template<std::size_t base>
void int_ap<base>::cut()
{
    for (std::size_t i = digits.size(); i --> 0 ;)
        if (digits[i] != 0) {
            if (i+1 <= digits.size()-1)
                digits.erase(digits.begin()+i+1,digits.end());
            return;
        }
    digits = std::vector<short>(1, 0);
}

template<std::size_t base>
void int_ap<base>::cutRight()
{
    for (std::size_t i = 0; i != digits.size(); ++i)
        if (digits[i] != 0) {
            if (i > 0)
                digits.erase(digits.begin(), digits.begin()+i);
            return;
        }
    digits = std::vector<short>(1, 0);
}

template<std::size_t base>
void int_ap<base>::shiftLeft(std::size_t n)
{
    if (*this == 0)
        return;
    std::vector<short> newDigits = std::vector<short>(n ,0);
    digits.insert(digits.begin(), newDigits.begin(), newDigits.end());
}

template<std::size_t base>
void int_ap<base>::shiftRight(std::size_t n)
{
    if (n > digits.size())
        n = digits.size();
    digits.erase(digits.begin(), digits.begin()+n);
}

template<std::size_t base>
int_ap<base>& int_ap<base>::operator=(long long int value)
{
    digits.clear();
    minusSign = false;
    if (value < 0) {
        value = -value;
        minusSign = true;
    }
    if (value < (int)base)
        digits = std::vector<short>(1);
    else if (value != 0)
        digits = std::vector<short>(1 + floor(log<base>((value))));
    for (std::size_t i = 0; i != digits.size(); ++i) {
        int digit = value % base;
        digits[i] = digit;
        value -= digit;
        value /= base;
    }

    return *this;
}

template<std::size_t base>
int_ap<base>& int_ap<base>::operator=(const int_ap<base>& value)
{
    minusSign = value.minusSign;
    digits = value.digits;
    return *this;
}

template<std::size_t base>
int_ap<base> factorial(const int_ap<base>& number)
{
    int_ap<base> res(1);
    for (int_ap<base> i(2); i <= number; i = i+1) 
        res = i*res;

    return res;
}

template<std::size_t base>
long long int int_ap<base>::toCInteger() const
{
    if (abs(*this) > pow(int_ap<base>(2), int_ap<base>(63))) {
        std::cout << "Error: getting C++ integer for a number bigger than"
                  << " 2^63.";
        std::raise(SIGTERM);
    }
    long long int res = 0;
    for (auto iter = digits.rbegin(); iter != digits.rend(); ++iter)
        res = base * res + *iter;

    return res;
}

template<>
class int_ap<0> {
    public:
    int_ap<0>() = delete;
};

template<>
class int_ap<1> {
    public:
    int_ap<1>() = delete;
};

} // End of namespace csl

#endif
