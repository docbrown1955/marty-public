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

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace sgl {

class [[nodiscard]] Exception : public std::exception {
  public:
    Exception() = default;

    template <class... Args>
    explicit Exception(Args &&...args)
    {
        std::ostringstream sout;
        (sout << ... << std::forward<Args>(args));
        _msg = sout.str();
    }

    char const *what() const noexcept override
    {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

class AbstractCallError : public Exception {
    using Exception::Exception;
};
class MathError : public Exception {
    using Exception::Exception;
};
class TypeError : public Exception {
    using Exception::Exception;
};
class IndexError : public Exception {
    using Exception::Exception;
};
class ValueError : public Exception {
    using Exception::Exception;
};

} // namespace sgl
