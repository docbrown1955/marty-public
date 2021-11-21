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

#include "timeMonitor.h"
#include <iomanip>

#define COMPLETE(n) ((n < 10) ? "0" : "")

namespace csl {

void TimeMonitor::print(size_t nSec,
                        std::ostream& out)
{
    TimeMonitor::Time time = TimeMonitor::processSec(nSec);
    if (time.hour != 0)
        out << COMPLETE(time.hour) << time.hour << 'h' << separator;
    if (time.min != 0 or time.hour != 0)
        out << COMPLETE(time.min) << time.min  << 'm' << separator;
    out << COMPLETE(time.sec) << time.sec  << 's';
}

TimeMonitor::TimeMonitor()
    :m_start(std::clock()),
    m_time{0, 0, 0}
{

}

void TimeMonitor::reset()
{
    m_start = std::clock();
}

void TimeMonitor::print(std::ostream & out)
{
    m_time = processSec((std::clock() - m_start) * 1. / CLOCKS_PER_SEC);
    print(getTotalSec(), out);
}

TimeMonitor::Time TimeMonitor::processSec(size_t t_sec)
{
    TimeMonitor::Time time;
    time.hour = t_sec / 3600;
    t_sec -= 3600 * time.hour;

    time.min = t_sec / 60;

    time.sec = t_sec - 60 * time.min;

    return time;
}

}
