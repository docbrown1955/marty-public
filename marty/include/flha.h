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
 * @version 1.0
 
 * \brief
 */
#ifndef FLHA_H_INCLUDED
#define FLHA_H_INCLUDED

#include <csl.h>
#include "SM.h"

namespace mty::flha {

    class InputFlag {

        public:

        InputFlag(size_t t_id)
            :id(t_id),
            target(CSL_UNDEF)
        {

        }

        InputFlag(size_t      t_id,
                  csl::Expr const& t_target)
            :id(t_id),
            target(t_target)
        {

        }

        size_t getID() const
        {
            return id;
        }

        void readValue(long double value) const
        {
            if (target) {
                if (target == sm_input::alpha_em)
                    target->setValue(1L / value);
                else
                    target->setValue(value);
            }
        }

        void readValue(long double             value,
                       std::vector<int> const& pos) const
        {
            if (pos.empty()) 
                readValue(value);
            else if (target)
                target->getArgument(pos)->setValue(value);
        }

        bool operator==(InputFlag const& other) const
        {
            return id == other.id;
        }       

        bool operator!=(InputFlag const& other) const
        {
            return not (*this == other);
        }

        bool operator==(size_t t_id) const
        {
            return id == t_id;
        }

        bool operator!=(size_t t_id) const
        {
            return not (*this == t_id);
        }

        bool operator<(InputFlag const& other) const 
        {
            return id < other.id;
        }

        bool operator>(InputFlag const& other) const
        {
            return other < *this;
        }

        bool operator<=(InputFlag const& other) const
        {
            return *this < other or id == other.id;
        }

        bool operator>=(InputFlag const& other) const
        {
            return other <= *this;
        }

        protected:

        size_t id;

        csl::Expr   target;
    };

} // End of namespace flha

#endif
