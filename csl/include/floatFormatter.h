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
#ifndef FLOATFORMATTER_H_INCLUDED
#define FLOATFORMATTER_H_INCLUDED

#include <iostream>
#include <optional>
#include <string>
#include <map>

namespace csl {

class format {

    public:

    format() {}

    format(std::string t_format);

    bool isScientific() const;

    int getMaxDigits() const;

    private:

    bool scientific = true;

    int  maxDigits = 15;
};

class Formatter {

    public:

    Formatter() = delete;

    static format getFormat();

    static void setFormat(format const& t_f);

    private:

    static std::optional<format> f;
};

}

#endif
