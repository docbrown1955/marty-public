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
#ifndef PRECISION_FLOAT_H_INCLUDED
#define PRECISION_FLOAT_H_INCLUDED

#include "precision_int.h"
#include "floatFormatter.h"
#include <algorithm>

namespace csl {

const int maxMantisseSize = 64;

std::pair<int_ap<2>, int> decomposeLongFloat(long double number);

template<size_t base>
float_sap<base> decomposeLongFloat(long double number)
{
    std::pair<int_ap<2>, int> res = decomposeLongFloat(number);
    int_ap<base> convertedMantissa = res.first.convert<base>();
    int exponent = res.second - res.first.size();

    float_sap<base> baseFactor;
    int_ap<base>   baseNum(1), baseDenom(1);
    if (exponent > 0) {
        baseDenom = int_ap(1);
        baseNum   = pow(int_ap<base>(2), int_ap<base>(exponent));
    }
    else if (exponent < 0){
        baseNum   = int_ap(1);
        baseDenom = pow(int_ap<base>(2), int_ap<base>(-exponent));
    }
    int_ap<base> baseMantissa, baseExponent;
    dividePrecision(baseNum, baseDenom, baseMantissa, baseExponent);
    baseFactor = float_sap<base>(baseMantissa, baseExponent);
    float_sap<base> result(convertedMantissa,
                          int_ap<base>(convertedMantissa.size())-1);
    result *= baseFactor;
    return result;
}

template<size_t base>
class float_sap {

    public:

    int_ap<base> mantissa;
    
    int_ap<base> exponent;

    public:

    float_sap();

    explicit
    float_sap(long double value);

    explicit
    float_sap(const std::vector<short>& t_digits);

    float_sap(bool                      t_minusSign,
             const std::vector<short>& t_digits,
             const int_ap<base>&       t_exponent);

    float_sap(const int_ap<base>& t_mantissa,
             const int_ap<base>& t_exponent);

    float_sap(const int_ap<base>& other);

    ~float_sap(){};

    float_sap<base>& operator=(long double value);

    float_sap<base>& operator=(const float_sap<base>& value);

    float_sap<base>& format(std::string const& f);

    bool getMinusSign() const;

    void setMinusSign(bool t_sign);

    void adjustOnExponent(const float_sap<base>& number);

    void adjustOnExponent(const int_ap<base>& t_exponent);

    private:

    int_ap<base> getFloatingPointPos() const;

    friend
    float_sap<base> abs(const float_sap<base>& a)
    {
        float_sap<base> copy = a;
        copy.setMinusSign(false);
        return copy;
    }

    friend 
    float_sap<base> floor(const float_sap<base>& a)
    {
        if (a.exponent < 1) {
            if (a.getMinusSign())
                return -float_sap<base>(1);
            return float_sap<base>(0);
        }
        int_ap<base> nDigits = a.exponent;
        if (a.exponent > a.mantissa.size())
            nDigits = a.mantissa.size();
        std::vector<short> digits(a.mantissa.digits.rend()-nDigits,
                                  a.mantissa.digits.rend());
        if (a.getMinusSign()) {
            return int_ap<base>(true, digits) - 1;
        }
        else
            return int_ap<base>(digits);
    }

    friend 
    float_sap<base> ceil(const float_sap<base>& a)
    {
        if (a.exponent >= a.mantissa.size()) {
            std::vector<short> digits(a.exponent.toCInteger(), 0);
            for (int i = 0; i != a.mantissa.size(); ++i)
                digits[i] = a.mantissa[i];
            return int_ap<base>(a.getMinusSign(), digits);
        }
        return 1 + floor(a);
    }

    friend
    float_sap<base> round(const float_sap<base>& a)
    {
        if (a.exponent >= a.mantissa.size())
            return floor(a);
        short firstDigit = a.mantissa[a.exponent.toCInteger()];
        if (firstDigit > base/2)
            return ceil(a);
        return floor(a);
    }

    friend
    std::ostream& operator<<(std::ostream& fout, const float_sap<base>& floating)
    {
        csl::format f = csl::Formatter::getFormat();
        if (floating.getMinusSign())
            fout << "-";
        if (base == 16)
            fout << "0x";
        if (f.isScientific()) {
            fout << convertDigit(floating.mantissa[
                    floating.mantissa.size()-1]) << ".";
            for (size_t i = 1; i != floating.mantissa.size(); ++i)
                if (i > f.getMaxDigits())
                    break;
                else
                    fout << floating.mantissa[
                        floating.mantissa.size()-i-1];
            fout << " x " << base << "^" << floating.exponent - 1;

            return fout;
        }
        if (floating.exponent <= 0) {
            fout << "0.";
            int nDigits = 0;
            for (int i = 0; i != abs(floating.exponent.toCInteger()); ++i) {
                if (i > f.getMaxDigits())
                    break;
                fout << "0";
                ++nDigits;
            }
            if (nDigits > f.getMaxDigits())
                return fout;
            for (size_t i = 0; i != floating.mantissa.size(); ++i)
                if (i + nDigits > f.getMaxDigits())
                    break;
                else
                    fout << convertDigit(floating.mantissa[
                            floating.mantissa.size()-i-1]);
            return fout;
        }
        int nDigits = 0;
        bool floatingPoint = false;
        for (size_t i = 0; i != floating.mantissa.size(); ++i) {
            if (i == floating.exponent) {
                floatingPoint = true;
                fout << ".";
            }
            if (floatingPoint) {
                if (nDigits > f.getMaxDigits())
                    return fout;
                ++nDigits;
                fout << convertDigit(floating.mantissa[
                        floating.mantissa.size()-i-1]);
            }
            else
                fout << convertDigit(floating.mantissa[
                        floating.mantissa.size()-1-i]);
        }

        return fout;
    }

    friend
    float_sap<base> operator+(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)+b;
    }

    friend
    float_sap<base> operator+(const float_sap<base>& a, long long int b)
    {
        return a+float_sap<base>(b);
    }

    friend
    float_sap<base> operator+(const float_sap<base>& a, const float_sap<base>& b)
    {
        float_sap<base> c = a;
        return c += b;
    }

    friend
    float_sap<base>& operator+=(float_sap<base>& a, float_sap<base> b)
    {
        int diff;
        if (a.exponent > b.exponent) {
            diff = (a.exponent - b.exponent 
                - (a.mantissa.size() - b.mantissa.size())).toCInteger();
        }
        else if (b.exponent > a.exponent) {
            diff = (b.exponent - a.exponent 
                - (b.mantissa.size() - a.mantissa.size())).toCInteger();
        }
        else
            diff = 0;
        if (diff > 0) 
            a.mantissa.shiftLeft(diff);
        else
            b.mantissa.shiftLeft(-diff);
        int_ap<base> newMantissa = a.mantissa + b.mantissa;
        a.exponent += (newMantissa.size() - a.mantissa.size());
        a.mantissa  = newMantissa;
        return a;
    }

    friend
    float_sap<base>& operator+=(float_sap<base>& a, long long int b)
    {
        return a += float_sap<base>(b);
    }

    friend
    float_sap<base> operator-(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a) - b;
    }

    friend
    float_sap<base> operator-(const float_sap<base>& a, long long int b)
    {
        return a - float_sap<base>(b);
    }

    friend
    float_sap<base> operator-(const float_sap<base>& a, const float_sap<base>& b)
    {
        float_sap<base> c = a;
        return c -= b;
    }

    friend
    float_sap<base>& operator-=(float_sap<base>& a, float_sap<base> b)
    {
        return a += -b;
    }

    friend
    float_sap<base>& operator-=(float_sap<base>& a, long long int b)
    {
        return a -= float_sap<base>(b);
    }

    friend
    float_sap<base> operator++(float_sap<base>& a, int)
    {
        float_sap<base> b = a;
        a += 1;
        return b;
    }

    friend
    float_sap<base>& operator++(float_sap<base>& a)
    {
        a += 1;
        return a;
    }

    friend
    float_sap<base> operator--(float_sap<base>& a, int)
    {
        float_sap<base> b = a;
        a -= 1;
        return b;
    }

    friend
    float_sap<base>& operator--(float_sap<base>& a)
    {
        a -= 1;
        return a;
    }

    friend
    float_sap<base> operator-(const float_sap<base>& a)
    {
        float_sap<base> b = a;
        b.setMinusSign(not a.getMinusSign());
        return b;
    }

    friend
    float_sap<base> operator*(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)*b;
    }

    friend
    float_sap<base> operator*(const float_sap<base>& a, long long int b)
    {
        return a*float_sap<base>(b);
    }

    friend
    float_sap<base> operator*(const float_sap<base>& a, const float_sap<base>& b)
    {
        float_sap<base> c = a;
        return c *= b;
    }

    friend
    float_sap<base>& operator*=(float_sap<base>& a, long long int b)
    {
        return a *= float_sap<base>(b);
    }

    friend
    float_sap<base>& operator*=(float_sap<base>& a, float_sap<base> b)
    {
        int_ap<base> newMantissa = a.mantissa * b.mantissa;
        a.exponent = a.exponent + b.exponent 
                        + (newMantissa.size() - a.mantissa.size());
        a.mantissa = std::move(newMantissa);

        return a;
    }

    friend
    float_sap<base> operator/(const float_sap<base>& a, const float_sap<base>& b)
    {
        if (b == 0) {
            std::cout << "Division by 0 encountered in float_sap<" 
                      << base << ">.\n";
            std::raise(SIGFPE);
        }
        int_ap<base> divisor, rest;
        divide(a.mantissa, b.mantissa, divisor, rest);
        int_ap<base> newExponent = a.exponent - b.exponent;
        float_sap<base> result(std::move(divisor), newExponent);
        size_t nDigits = result.mantissa.size();
        while (rest != 0 and nDigits < PRECISION) {
            rest *= base;
            divide(rest, b.mantissa, divisor, rest);
            float_sap<base> floatRest(rest, 1-b.exponent);
            result += floatRest;
            ++nDigits;
        }
        return result;
    }

    friend
    float_sap<base> operator/(const float_sap<base>& a, long long int b)
    {
        return a / float_sap<base>(b);
    }

    friend 
    float_sap<base> operator/(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a) / b;
    }

    friend
    float_sap<base>& operator/=(float_sap<base>& a, long long int b)
    {
        return a /= float_sap<base>(b);
    }

    friend
    float_sap<base>& operator/=(float_sap<base>& a, const float_sap<base>& b)
    {
        return a = (a / b);
    }

    friend
    float_sap<base> operator%(const float_sap<base>& a, const float_sap<base>& b)
    {
        float_sap<base> divisor, rest;
        divide(a, b, divisor, rest);

        return rest;
    }

    friend
    float_sap<base> operator%(const float_sap<base>& a, long long int b)
    {
        return a % float_sap<base>(b);
    }

    friend
    float_sap<base> operator%(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a) % b;
    }

    friend
    float_sap<base>& operator%=(float_sap<base>& a, long long int b)
    {
        return a %= float_sap<base>(b);
    }

    friend 
    float_sap<base>& operator%=(float_sap<base>& a, const float_sap<base>& b)
    {
        return a = (a % b);
    }

    friend 
    float_sap<base> pow(long long int a, const float_sap<base>& b)
    {
        return pow(float_sap<base>(a), b);
    }

    friend 
    float_sap<base> pow(const float_sap<base>& a, long long int b)
    {
        return pow(a, float_sap<base>(b));
    }

    friend 
    float_sap<base> pow(const float_sap<base>& a, const float_sap<base>& b)
    {
        if (b <= 0) {
            if (b == 0) {
                if (a != 0)
                    return int_ap<base>(1);
                std::cout << "Error: 0^0 is not defined." << std::endl;
                std::raise(SIGFPE);
            }
            return 1 / (pow(a, -b));
        }
        if (b == 1)
            return a;
        float_sap<base> res = a;
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
    bool operator==(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)==b;
    }

    friend
    bool operator==(const float_sap<base>& a, long long int b)
    {
        return a==float_sap<base>(b);
    }

    friend
    bool operator==(const float_sap<base>& a, const float_sap<base>& b)
    {
        return (a.exponent == b.exponent) and (a.mantissa == b.mantissa);
    }

    friend
    bool operator!=(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)!=b;
    }

    friend
    bool operator!=(const float_sap<base>& a, long long int b)
    {
        return a!=float_sap<base>(b);
    }

    friend
    bool operator!=(const float_sap<base>& a, const float_sap<base>& b)
    {
        return not (a == b);
    }

    friend
    bool operator<(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)<b;
    }

    friend
    bool operator<(const float_sap<base>& a, long long int b)
    {
        return a<float_sap<base>(b);
    }

    friend
    bool operator<(const float_sap<base>& a, const float_sap<base>& b)
    {
        if (a.exponent < b.exponent)
            return true;
        if (b.exponent < a.exponent)
            return false;
        return a.mantissa < b.mantissa;
    }

    friend
    bool operator<=(long long int a, const float_sap<base>& b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator<=(const float_sap<base>& a, long long int b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator<=(const float_sap<base>& a, const float_sap<base>& b)
    {
        return (a<b or a==b);
    }

    friend
    bool operator>(long long int a, const float_sap<base>& b)
    {
        return float_sap<base>(a)>b;
    }

    friend
    bool operator>(const float_sap<base>& a, long long int b)
    {
        return a>float_sap<base>(b);
    }

    friend
    bool operator>(const float_sap<base>& a, const float_sap<base>& b)
    {
        return b < a;
    }

    friend
    bool operator>=(long long int a, const float_sap<base>& b)
    {
        return (a>b or a==b);
    }

    friend
    bool operator>=(const float_sap<base>& a, long long int b)
    {
        return (a>b or a==b);
    }

    friend
    bool operator>=(const float_sap<base>& a, const float_sap<base>& b)
    {
        return (a>b or a==b);
    }
};

template<size_t base>
float_sap<base>::float_sap()
    :mantissa(int_ap<base>(1)),
    exponent(int_ap<base>(0))
{

}

template<size_t base>
float_sap<base>::float_sap(long double other)
{
    *this = other;
}

template<size_t base>
float_sap<base>::float_sap(const int_ap<base>& other)
{
    mantissa = other;
    exponent = mantissa / base + 1;
}

template<size_t base>
float_sap<base>::float_sap(const int_ap<base>& t_mantissa,
                         const int_ap<base>& t_exponent)
    :mantissa(t_mantissa),
    exponent(t_exponent)
{

}

template<size_t base>
bool float_sap<base>::getMinusSign() const
{
    return mantissa.getMinusSign();
}

template<size_t base>
void float_sap<base>::setMinusSign(bool t_sign)
{
    mantissa.setMinusSign(t_sign);
}

template<size_t base>
void float_sap<base>::adjustOnExponent(const float_sap<base>& number)
{
    adjustOnExponent(number.exponent);
}

template<size_t base>
void float_sap<base>::adjustOnExponent(const int_ap<base>& t_exponent)
{
    int diff = (exponent - t_exponent).toCInteger();
    if (diff < 0) {
        std::cout << "Adjusting a float_sap on bigger exponent. No meant to do"
                  << " do that." << std::endl;
        std::raise(SIGTERM);
    }
    mantissa.shiftLeft(diff);
}

template<size_t base>
float_sap<base>& float_sap<base>::operator=(long double other)
{
    if (other == 0)
        return *this = float_sap(0);
    bool sign = false;
    if (other < 0) {
        other = -other;
        sign  = true;
    }
    *this = decomposeLongFloat<base>(other);
    setMinusSign(sign);

    return *this;
}

template<size_t base>
float_sap<base>& float_sap<base>::operator=(const float_sap<base>& other)
{
    mantissa = other.mantissa;
    exponent = other.exponent;
    return *this;
}

template<size_t base>
int_ap<base> float_sap<base>::getFloatingPointPos() const
{
    return exponent;
}

template<size_t base>
float_sap<base>& float_sap<base>::format(std::string const& f)
{
    csl::Formatter::setFormat(f);
    return *this;
}

template<>
class float_sap<0> {
    public:
    float_sap<0>() = delete;
};

template<>
class float_sap<1> {
    public:
    float_sap<1>() = delete;
};

} // End of namespace csl

#endif
