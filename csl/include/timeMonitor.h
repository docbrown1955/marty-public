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
 * @version 1.3
 
 * \brief
 */
#ifndef CSL_TIME_MONITOR_H_INCLUDED
#define CSL_TIME_MONITOR_H_INCLUDED 

#include <iostream>
#include <ctime>

namespace csl {

class TimeMonitor {

    public:

    static inline char separator = '\'';

    struct Time {

        std::size_t hour;
        std::size_t min;
        std::size_t sec;
    };

    static void print(std::size_t nSec, std::ostream& out);
    static Time processSec(std::size_t n_sec);

    public:

    TimeMonitor();

    void reset();
    void print(std::ostream & out);

    std::size_t getHour() const {
        return m_time.hour;
    }

    std::size_t getMin() const {
        return m_time.min;
    }

    std::size_t getSec() const {
        return m_time.sec;
    }

    std::size_t getTotalSec() const {
        return 3600 * m_time.hour + 60 * m_time.min + m_time.sec;
    }

    protected:

    private:

    std::clock_t m_start;
    Time         m_time;
};

}

#endif 
