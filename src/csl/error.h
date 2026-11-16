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

/*! \file error.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief File containing functions that are called by the program
 * when something wrong happened: determines the behaviour in unexpected
 * cases.
 */
#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "options.h"
#include <csignal>
#include <iostream>
#include <string_view>

enum class CSLError {

    AbstractCallError, // Raised when an abstract (not implemented) fucntion is
                       // called.
    AttributeError,    // Raised when attribute assignment or reference fails.
    EOFError, // Raised when the input() functions hits end-of-file condition.
    FloatingPointError, // Raised when a floating point operation fails.
    GeneratorExit,      // Raise when a generator's close() method is called.
    IndexError,         // Raised when index of a sequence is out of range.
    IOError,            // Raised when a file is not found.
    KeyError,           // Raised when a key is not found in a dictionary.
    MathError,
    NameError, // Raised when a variable is not found in local or global scope.
    NotImplementedError, // Raised by abstract methods.
    OverflowError, // Raised when result of an arithmetic operation is too
                   // large to be represented.
    RuntimeError,  // Raised when an error does not fall under any other
                   // category.
    SyntaxError,   // Raised by parser when syntax error is encountered.
    TypeError,  // Raised when a function or operation is applied to an object
                // of incorrect type.
    ValueError, // Raised when a function gets argument of correct type but
                // improper value.
    ZeroDivisionError // Raised when second operand of division or modulo
                      // operation is zero.
};

std::ostream &operator<<(std::ostream &fout, CSLError error);

void CallErrorImpl(CSLError         error,
                   std::string_view func,
                   std::string_view file,
                   int              line);

void CallErrorImpl(CSLError         error,
                   std::string_view func,
                   std::string_view file,
                   int              line,
                   std::string_view message);

#define CSL_ASSERT(cond, error)                             \
    if (not(cond)) {                                        \
        CallErrorImpl(error, __func__, __FILE__, __LINE__); \
    }
#define CSL_ASSERT_SPEC(cond, error, message)                        \
    if (not(cond)) {                                                 \
        CallErrorImpl(error, __func__, __FILE__, __LINE__, message); \
    }

#define CALL_SMERROR(error) CallErrorImpl(error, __func__, __FILE__, __LINE__)

#define CALL_SMERROR_SPEC(error, message) \
    CallErrorImpl(error, __func__, __FILE__, __LINE__, message)

#endif
