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
 * @version 2.0

 * \brief
 */
#ifndef ALLOC_MONITOR_H_INCLUDED
#define ALLOC_MONITOR_H_INCLUDED

#include <ctime>
#include <iostream>
#include <map>

class __data_alloc {

    using Type = int;
    using Time = std::clock_t;
    struct data_alloc {

        int  count;
        Time time;
    };

  public:
    std::map<Type, data_alloc> DATA{};

    void record(Type type, Time const &time)
    {
        if (auto pos = DATA.find(type); pos == DATA.end()) {
            DATA[type] = {1, time};
        }
        else {
            ++DATA[type].count;
            DATA[type].time += time;
        }
    }

    void displayResults(std::ostream &out)
    {
        for (const auto &[key, data] : DATA) {
            out << key << ": \n";
            out << "   --> " << data.count << " allocations.\n";
            out << "   --> " << data.time / CLOCKS_PER_SEC
                << " s of total life time.\n";
        }
    }
};

inline __data_alloc __DATA;

class __alloc_monitor {

  private:
    std::clock_t __allocation_time;

  public:
    __alloc_monitor()
    {
        __allocation_time = std::clock();
    }

    void __record_data_alloc(int type)
    {
        __DATA.record(type, std::clock() - __allocation_time);
    }
};

#endif
