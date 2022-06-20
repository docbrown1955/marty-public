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
 * @file mrtError.h
 * @brief
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2020-11-05
 */

#ifndef HEPERROR_H_INCLUDED
#define HEPERROR_H_INCLUDED

#include <csignal>
#include <iostream>
#include <sstream>
#include <string>

namespace mty::error {

enum Type {

    AssertionError, // Raised when assert statement fails.
    AttributeError, // Raised when attribute assignment or reference fails.
    EOFError, // Raised when the input() functions hits end-of-file condition.
    FloatingPointError, // Raised when a floating point operation fails.
    GeneratorExit,      // Raise when a generator's close() method is called.
    ImportError,        // Raised when the imported module is not found.
    IndexError,         // Raised when index of a sequence is out of range.
    KeyError,           // Raised when a key is not found in a dictionary.
    KeyboardInterrupt,  // Raised when the user hits interrupt key (Ctrl+c or
                       // delete).
    MemoryError, // Raised when an operation runs out of memory.
    ModelBuildingError,
    NameError, // Raised when a variable is not found in local or global scope.
    NotImplementedError, // Raised by abstract methods.
    OSError,       // Raised when system operation causes system related error.
    OverflowError, // Raised when result of an arithmetic operation is too
                   // large to be represented.
    PhysicsError,
    ReferenceError, // Raised when a weak reference proxy is used to access a
                    // garbage collected referent.
    RuntimeError, // Raised when an error does not fall under any other
                  // category.
    StopIteration, // Raised by next() function to indicate that there is no
                   // further item to be returned by iterator.
    SyntaxError,      // Raised by parser when syntax error is encountered.
    IndentationError, // Raised when there is incorrect indentation.
    TabError, // Raised when indentation consists of inconsistent tabs and
              // spaces.
    SystemError, // Raised when interpreter detects internal error.
    SystemExit,  // Raised by sys.exit() function.
    TypeError,   // Raised when a function or operation is applied to an object
               // of incorrect type.
    UnboundLocalError, // Raised when a reference is made to a local variable
                       // in a function or method, but no value has been bound
                       // to that variable.
    UnicodeError, // Raised when a Unicode-related encoding or decoding error
                  // occurs.
    UnicodeEncodeError, // Raised when a Unicode-related error occurs during
                        // encoding.
    UnicodeDecodeError, // Raised when a Unicode-related error occurs during
                        // decoding.
    UnicodeTranslateError, // Raised when a Unicode-related error occurs during
                           // translating.
    ValueError, // Raised when a function gets argument of correct type but
                // improper value.
    ZeroDivisionError // Raised when second operand of division or modulo
                      // operation is zero.
};

inline std::ostream &operator<<(std::ostream &fout, mty::error::Type error)
{
    switch (error) {
    case mty::error::AssertionError:
        fout << "AssertionError";
        break;
    case mty::error::AttributeError:
        fout << "AttributeError";
        break;
    case mty::error::EOFError:
        fout << "EOFError";
        break;
    case mty::error::FloatingPointError:
        fout << "FloatingPointError";
        break;
    case mty::error::GeneratorExit:
        fout << "GeneratorExit";
        break;
    case mty::error::ImportError:
        fout << "ImportError";
        break;
    case mty::error::IndexError:
        fout << "IndexError";
        break;
    case mty::error::KeyError:
        fout << "KeyError";
        break;
    case mty::error::KeyboardInterrupt:
        fout << "KeyboardInterrupt";
        break;
    case mty::error::MemoryError:
        fout << "MemoryError";
        break;
    case mty::error::ModelBuildingError:
        fout << "ModelBuildingError";
        break;
    case mty::error::NameError:
        fout << "NameError";
        break;
    case mty::error::NotImplementedError:
        fout << "NotImplementedError";
        break;
    case mty::error::OSError:
        fout << "OSError";
        break;
    case mty::error::OverflowError:
        fout << "OverflowError";
        break;
    case mty::error::PhysicsError:
        fout << "PhysicsError";
        break;
    case mty::error::ReferenceError:
        fout << "ReferenceError";
        break;
    case mty::error::RuntimeError:
        fout << "RuntimeError";
        break;
    case mty::error::StopIteration:
        fout << "StopIteration";
        break;
    case mty::error::SyntaxError:
        fout << "SyntaxError";
        break;
    case mty::error::IndentationError:
        fout << "IndentationError";
        break;
    case mty::error::TabError:
        fout << "TabError";
        break;
    case mty::error::SystemError:
        fout << "SystemError";
        break;
    case mty::error::SystemExit:
        fout << "SystemExit";
        break;
    case mty::error::TypeError:
        fout << "TypeError";
        break;
    case mty::error::UnboundLocalError:
        fout << "UnboundLocalError";
        break;
    case mty::error::UnicodeError:
        fout << "UnicodeError";
        break;
    case mty::error::UnicodeEncodeError:
        fout << "UnicodeEncodeError";
        break;
    case mty::error::UnicodeDecodeError:
        fout << "UnicodeDecodeError";
        break;
    case mty::error::UnicodeTranslateError:
        fout << "UnicodeTranslateError";
        break;
    case mty::error::ValueError:
        fout << "ValueError";
        break;
    case mty::error::ZeroDivisionError:
        fout << "ZeroDivisionError";
        break;
    default:
        fout << "Type " << (int) error << "unknown!";
        break;
    }

    return fout;
}

} // End of namespace mty::error

#define CallHEPError(error, spec)                                            \
    {                                                                        \
        std::cerr << error << " in function " << __func__ << "() (file "     \
                  << __FILE__ << " l." << __LINE__ << "): " << spec << '\n'; \
        throw error;                                                         \
    }

#define HEPAssert(condition, error, spec) \
    if (not(condition))                   \
    CallHEPError(error, spec)

inline std::ostream &printChain(std::ostream &fout)
{
    return fout;
}

template <typename T, typename... G>
std::ostream &printChain(std::ostream &fout, T firstSpec, G... followingSpec)
{
    fout << firstSpec << " ";
    printChain(fout, followingSpec...);
    return fout;
}

#endif
