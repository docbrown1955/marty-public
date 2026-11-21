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
 * @file error.h
 * @brief Typed errors of the CSL API layer (Python neutral).
 *
 * Every API function reports failures with one of these classes. The message
 * is complete and is never printed by the API itself.
 */

#ifndef CSL_API_ERROR_H_INCLUDED
#define CSL_API_ERROR_H_INCLUDED

#include <stdexcept>

namespace csl::api {

/// Base class of all errors thrown by the API layer.
class Error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/// Argument has the right type but an improper value.
class ValueError : public Error {
  public:
    using Error::Error;
};

/// Division by zero, for example zero raised to a negative power.
class ZeroDivisionError : public Error {
  public:
    using Error::Error;
};

/// Argument has an incorrect type.
class TypeError : public Error {
  public:
    using Error::Error;
};

/// A name or key was not found.
class LookupError : public Error {
  public:
    using Error::Error;
};

/// The requested operation is not implemented.
class NotImplementedError : public Error {
  public:
    using Error::Error;
};

/// The operation is not valid in the current state.
class StateError : public Error {
  public:
    using Error::Error;
};

} // namespace csl::api

#endif
