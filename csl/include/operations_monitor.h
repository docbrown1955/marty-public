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

 #pragma once

#ifdef MONITOR_OPERATIONS

#include <ctime>
#include "enum.h"
#include "abstract.h"

namespace csl {

struct FileDeleter {
    FileDeleter() { 
        [[maybe_unused]] int sysres = system("rm sum_monitor.txt prod_monitor.txt"); 
    }
};

inline const FileDeleter deleter;

struct SumMonitor {

    SumMonitor(std::size_t t_nArgs, int t_mode)
        :nArgs(t_nArgs),
        mode(t_mode),
        time(std::clock())
    {}

    ~SumMonitor() {
        std::fstream fout("sum_monitor.txt", std::ios::app);
        fout.precision(6);
        fout << (std::clock() - time) * 1000. / CLOCKS_PER_SEC  << ' '
             << nArgs << ' ' 
             << mode << ' ' 
             // << static_cast<int>(type) << ' '
             << indicial << ' '
             << simple << '\n';
    }

    std::size_t nArgs;
    int mode;
    csl::Type type { csl::Type::NoType };
    bool indicial { false };
    bool simple { false };
    std::clock_t time;
};

struct ProdMonitor {

    ProdMonitor(std::size_t t_nArgs, int t_mode)
        :nArgs(t_nArgs),
        mode(t_mode),
        time(std::clock())
    {}

    ProdMonitor(std::size_t t_nArgs, int t_mode, csl::Expr const &a, csl::Expr const &b)
        :nArgs(t_nArgs),
        mode(t_mode),
        time(std::clock())
    {
        if (a->getPrimaryType() == csl::PrimaryType::Numerical)
            ++n;
        if (b->getPrimaryType() == csl::PrimaryType::Numerical)
            ++n;
    }

    ~ProdMonitor() {
        std::fstream fout("prod_monitor.txt", std::ios::app);
        fout.precision(6);
        fout << (std::clock() - time) * 1000. / CLOCKS_PER_SEC  << ' '
             << nArgs << ' ' 
             << mode << ' ' 
             //<< static_cast<int>(type) << ' '
             << indicial << ' '
             << simple << ' '
             << op << ' '
             << n << '\n';
    }

    std::size_t nArgs;
    int mode;
    csl::Type type { csl::Type::NoType };
    bool indicial { false };
    bool op { false };
    bool simple { false };
    std::size_t n { 0 };
    std::clock_t time;
};

}

#endif
