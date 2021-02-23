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

#include "error.h"

namespace csl {

void callError(cslError::Error error, const std::string& caller) {
    callError(error, caller, 0);
}

}

std::ostream& operator<<(std::ostream& fout, CSLError error)
{
 switch(error) {
  case CSLError::AbstractCallError:   fout << "AbstractCallError";   break;
  case CSLError::AttributeError:      fout << "AttributeError";      break;
  case CSLError::EOFError:            fout << "EOFError";            break;
  case CSLError::FloatingPointError:  fout << "FloatingPointError";  break;
  case CSLError::GeneratorExit:       fout << "GeneratorExit";       break;
  case CSLError::IndexError:          fout << "IndexError";          break;
  case CSLError::KeyError:            fout << "KeyError";            break;
  case CSLError::MathError:           fout << "MathError";            break;
  case CSLError::NameError:           fout << "NameError";           break;
  case CSLError::NotImplementedError: fout << "NotImplementedError"; break;
  case CSLError::OverflowError:       fout << "OverflowError";       break;
  case CSLError::RuntimeError:        fout << "RuntimeError";        break;
  case CSLError::SyntaxError:         fout << "SyntaxError";         break;
  case CSLError::TypeError:           fout << "TypeError";           break;
  case CSLError::ValueError:          fout << "ValueError";          break;
  case CSLError::ZeroDivisionError:   fout << "ZeroDivisionError";   break;
  default: std::cout << "Error "; break;
 }
 return fout;
}

void CallErrorImpl(CSLError          error,
                   std::string_view func,
                   std::string_view file,
                   int              line)
{
    std::cerr << error << " in " << func << " (" << file << " l. " << line 
        << ").\n";
    std::raise(SIGTERM);
}

void CallErrorImpl(CSLError          error,
                   std::string_view func,
                   std::string_view file,
                   int              line,
                   std::string_view message)
{
    std::cerr << error << " in " << func << " (" << file << " l. " << line 
        << "): " << message << '\n';
    std::raise(SIGTERM);
}
