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
#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>

namespace csl {

class Timer {

  private:
    std::clock_t start;

  public:
    std::ostream &out = std::cout;

    Timer()
    {
        start = std::clock();
    }

    Timer(Timer const &other) : start(other.start), out(other.out)
    {
    }
    Timer &operator=(Timer const &other)
    {
        start = other.start;
        return *this;
    }

    ~Timer()
    {
        display();
    }

    void restart()
    {
        display();
        start = std::clock();
    }

    void display()
    {
        out << "Elapsed time : " << elapsedTime(std::clock() - start)
            << std::endl;
    }

  private:
    static int getSecondsFromTime(std::clock_t time)
    {
        return time * 1. / CLOCKS_PER_SEC;
    }

    static int getModulo(int &init, int modulo)
    {
        int res = init / modulo;
        init -= modulo * res;
        return res;
    }

    static std::string getStringFromTime(int number, int nDigits = 2)
    {
        std::ostringstream sout;
        if (number < std::pow(10, nDigits - 1))
            for (int i = 0; i != nDigits - 1; ++i)
                sout << 0;
        sout << number;
        return sout.str();
    }

    static std::string elapsedTime(std::clock_t time)
    {
        int nSec   = getSecondsFromTime(time);
        int nMili  = 1000 * (time * 1. / CLOCKS_PER_SEC - nSec);
        int nHours = getModulo(nSec, 3600);
        int nMin   = getModulo(nSec, 60);

        return getStringFromTime(nHours) + "h " + getStringFromTime(nMin)
               + "m " + getStringFromTime(nSec) + "s "
               + getStringFromTime(nMili, 2) + "ms";
    }
};

} // namespace csl

#endif
