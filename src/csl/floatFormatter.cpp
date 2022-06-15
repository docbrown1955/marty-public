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

#include "floatFormatter.h"
#include <sstream>

using namespace std;

namespace csl {

std::optional<format> Formatter::f = nullopt;

format::format(string t_format)
{
    if (t_format.size() == 1) {
        if (t_format[0] == 'e')
            scientific = true;
        else if (t_format[1] == 'f')
            scientific = false;
    }
    else {
        if (t_format[t_format.size() - 1] == 'e')
            scientific = true;
        else if (t_format[t_format.size() - 1] == 'f')
            scientific = false;
        t_format.erase(t_format.end() - 1);
        if (t_format.size() > 0 and t_format[0] == '.')
            t_format.erase(t_format.begin());
        if (t_format.size() > 0) {
            istringstream sin(t_format);
            sin >> maxDigits;
            if (maxDigits < 0)
                maxDigits = 15;
        }
    }
}

bool format::isScientific() const
{
    return scientific;
}

int format::getMaxDigits() const
{
    return maxDigits;
}

format Formatter::getFormat()
{
    if (f) {
        format copy = f.value();
        f           = nullopt;
        return copy;
    }
    return format();
}

void Formatter::setFormat(format const &t_f)
{
    f = t_f;
}
} // namespace csl
