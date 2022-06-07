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
 * @file debuglog.h
 * @brief Debug log utlities for SGL with log macros.
 * @author Grégoire Uhlrich
 * @version 
 * @date 2021-05-06
 */
#pragma once

#include <iostream>
#include <fstream>

namespace sgl {

    class DebugLog {

    public:

        DebugLog() = default;
        DebugLog(std::string_view name): fout(&name[0]) {}

        void activate(std::string_view name) {
            close();
            fout.open(&name[0]);
        }

        void out_impl()
        {}

        template<class First>
        void out_impl(First const &first)
        {
            if (fout) {
                fout << first;
            }
        }

        template<class First, class Second, class ...Args>
        void out_impl(First const &first, Second const &second, Args &&...args)
        {
            if (fout) {
                fout << first << ' ';
                out_impl(second, std::forward<Args>(args)...);
            }
        }

        template<class ...Args>
        void out(Args &&...args) 
        {
            if (fout) {
                out_impl(std::forward<Args>(args)...);
                fout << std::endl;
            }
        }

        void close() { if (fout) fout.close(); }

        ~DebugLog() { close(); }

    private:

        std::ofstream fout;
    };


#ifdef DEBUGLOG
    inline DebugLog log("log.txt");
    #define LOG(...) sgl::log.out(__VA_ARGS__);
    #define SCOPELOGNAME(name) ScopeLog _scope_log_name_;
    #define SCOPELOG           ScopeLog _scope_log_;
#else
    inline DebugLog log;
    #define LOG(...) 
    #define SCOPELOGNAME(name) 
    #define SCOPELOG             
#endif

    struct ScopeLog {
        static inline std::size_t r_general = 0;
        std::size_t r;
        ScopeLog(): r(r_general++)  { LOG() LOG("******************************** R", r) LOG() }
        ~ScopeLog() { /*--r_general;*/    LOG() LOG("******************************** R", r) LOG() }
    };
}
