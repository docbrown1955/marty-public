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
 * @file mrtlog.h
 * @brief 
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */

#pragma once
#include <fstream>

namespace mty::log {

    class LogStream {

        public:

        LogStream(std::string const& fileName)
            :out(fileName)
        {

        }

        void setLogFile(std::string const& fileName)
        {
            if (out)
                out.close();
            out.open(fileName);
        }

        virtual ~LogStream() { if (out) out.close(); };

        template<class Printable>
            LogStream& operator<<(Printable var) {
                if (out)
                    out << var;
                return *this;
            }

        LogStream& operator<<(std::ostream&(*f)(std::ostream&)) {
            f(out);
            return *this;
        }

        private:

        std::ofstream out;
    };
    
    inline std::ofstream out;

    inline void print()
    {

    }

    template<class T, class ...Args>
        void print(T first, Args&&... next)
        {
            if (out) {
                out << first << " ";
                print(std::forward<Args>(next)...);
                out << '\n';
            }
        }
}
