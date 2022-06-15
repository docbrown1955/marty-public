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

#include <ctime>
#include <iostream>

namespace csl {

class TimeMonitor {

  public:
    static inline char separator = '\'';

    struct Time {

        size_t hour;
        size_t min;
        size_t sec;
    };

    static void print(size_t nSec, std::ostream &out);
    static Time processSec(size_t n_sec);

  public:
    TimeMonitor();

    void reset();
    void print(std::ostream &out);

    size_t getHour() const
    {
        return m_time.hour;
    }

    size_t getMin() const
    {
        return m_time.min;
    }

    size_t getSec() const
    {
        return m_time.sec;
    }

    size_t getTotalSec() const
    {
        return 3600 * m_time.hour + 60 * m_time.min + m_time.sec;
    }

  protected:
  private:
    std::clock_t m_start;
    Time         m_time;
};

} // namespace csl

#endif
