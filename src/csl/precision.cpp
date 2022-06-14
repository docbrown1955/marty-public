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

#include "precision_int.h"
#include "precision_float.h"
#include <float.h>
#include <math.h>

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Global variables and functions                //
/*************************************************/
///////////////////////////////////////////////////

size_t PRECISION      = 100;

bool WARN_OVERFLOW = false;

void fillDigits(const vector<short>& digits,
                vector<short>&       receiver)
{
    if (digits.size() > PRECISION) {
        if (WARN_OVERFLOW) 
            cout << "Warning: overflow in calculation of precision "
                 << PRECISION
                 << " digits.\n";

        receiver = vector<short>(digits.begin(),
                                 digits.begin()+PRECISION);
    }
    else
        receiver = vector<short>(digits.begin(),
                                 digits.end());
}

char convertDigit(short digit)
{
    if (digit > 9)
        return char(digit + 97 - 10);
    else
        return char(digit + 48);
}

std::pair<int_ap<2>, int> decomposeLongFloat(long double number)
{
    int         exponent;
    long double mantissa    = frexpl(number, &exponent);
    int64_t     mantissaInt = pow(2, LDBL_MANT_DIG-1) * mantissa;
    int_ap<2> mantissa_ap(mantissaInt);
    mantissa_ap.cutRight();

    return std::make_pair(mantissa_ap, exponent);
}

} // End of namespace csl
