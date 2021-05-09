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

/**
 * @file exceptions.h
 * @brief Exceptions utilities.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include <iostream>

namespace sgl {

    enum class Exception {
        AbstractCall,
        MathError,
        TypeError,
        IndexError,
        ValueError,
    };

    inline std::ostream &operator<<(
            std::ostream &out,
            Exception     ex
            )
    {
        switch (ex) {
            case Exception::AbstractCall: out << "AbstractCall"; break;
            case Exception::MathError:    out << "MathError";    break;
            case Exception::TypeError:    out << "TypeError";    break;
            case Exception::IndexError:   out << "IndexError";   break;
            case Exception::ValueError:   out << "ValueError";   break;
        }
        return out;
    }
}
