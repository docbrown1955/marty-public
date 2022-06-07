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
#ifndef CSL_PROGRESS_BAR_H_INCLUDED
#define CSL_PROGRESS_BAR_H_INCLUDED 

#include <iostream>
#include <string>
#include "timeMonitor.h"

namespace csl {

class ProgressBar {

    public:

    static inline char onCharacter     = '=';
    static inline char lastOnCharacter = '>';
    static inline char offCharacter    = '.';
    static inline char leftSide        = '[';
    static inline char rightSide       = ']';
    static inline const std::size_t      defaultSizeBar  = 30;
    static inline const std::string defaultMessage  = "Progress : ";

    ProgressBar(std::size_t             t_max,
                std::string const& t_message = defaultMessage,
                std::size_t             t_sizeBar = defaultSizeBar,
                std::ostream     & t_out     = std::cout)
        :message(t_message),
        out(t_out),
        max(t_max),
        sizeBar(t_sizeBar),
        timeMonitoring(true)
    {

    }

    void setTimeMonitoring(bool value)
    {
        timeMonitoring = value;
        monitor.reset();
    }

    void reset()
    {
        monitor.reset();
    }

    void reset(std::size_t t_max)
    {
        max = t_max;
        monitor.reset();
    }

    void reset(std::size_t t_max, std::string const &t_message)
    {
        max = t_max;
        message = t_message;
        monitor.reset();
    }

    void progress(std::size_t pos) const 
    {
        enableCursor(false);
        std::size_t n = nChar(pos);
        out << message << " ";
        out << leftSide;
        for (std::size_t i = 0; i != sizeBar; ++i)
            if (i < n) 
                out << ((i+1 == n) ? lastOnCharacter : onCharacter);
            else
                out << offCharacter;
        out << rightSide;
        float percent = percentage(pos);
        out << " " << int(percent) << '%';
        if (timeMonitoring) {
            out << " , Elapsed time : ";
            monitor.print(out);
            if (percent > 0 and percent < 100) {
                out << " , Time left : ";
                double ratio = (100 - percent) * 1. / percent;
                TimeMonitor::print(ratio * monitor.getTotalSec(), out);
            }
        }
        out << ((pos + 1 == max) ? '\n' : '\r');
        enableCursor(true);
    }

    protected:

    std::size_t nChar(std::size_t pos) const 
    {
        return (pos+1) * sizeBar / max;
    }

    std::size_t percentage(std::size_t pos) const 
    {
        return (pos+1) * 100 / max;
    }

    void enableCursor(bool enable) const
    {
        out << ((enable) ? "\033[?25l" : "\033[?25h");
        out.flush();
    }

    private:

    std::string   message;
    std::ostream &out;
    std::size_t        max;
    std::size_t        sizeBar;
    bool          timeMonitoring;
    mutable TimeMonitor monitor;
};

} // End of namespace csl

#endif 
