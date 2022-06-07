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
#ifndef ITERABLE_H_INCLUDED
#define ITERABLE_H_INCLUDED

#include <cstddef>
#include <vector>
#include "mrtError.h"
#include "mrtUtils.h"

namespace mty {

template<typename Value_Type, class Containor = std::vector<Value_Type>>
class iterable_view {

    typedef typename Containor::const_iterator         c_iter;
    typedef typename Containor::const_reverse_iterator c_riter;
    
    private:

    const Containor& iterable;

    const std::size_t custom_begin;
    const std::size_t custom_end;

    public:

    iterable_view(const Containor& t_iterable)
        :iterable(t_iterable),
        custom_begin(0), 
        custom_end(t_iterable.size())
    {

    }

    iterable_view(const Containor& t_iterable,
                  std::size_t           b,
                  std::size_t           e)
        :iterable(t_iterable),
        custom_begin(b), 
        custom_end(e)
    {
        if (not (0 <= b and b <= e and e <= iterable.size()))
            CallHEPError(mty::error::TypeError,
                    "Wrong positions in construction of custom_iterable_view()."
                    + toString(b) + " and " + toString(e) + " for a containor "
                    + "of size " + toString(iterable.size()));
    }

    bool empty() const {
        return end() == begin();
    }

    std::size_t size() const {
        return distance(begin(), end());
    }

    c_iter begin() const {
        return iterable.begin() + custom_begin;
    }

    c_iter end() const {
        return iterable.begin() + custom_end;
    }

    c_riter rbegin() const {
        return iterable.rbegin() + iterable.size() - custom_end;
    }

    c_riter rend() const {
        return iterable.rend() - custom_begin;
    }

    Value_Type operator[](std::size_t i) const {
        return iterable[custom_begin + i];
    }
};

} // End of namespace mty

#endif
