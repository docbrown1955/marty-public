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

 * \brief Cyclic pool of index names of a Space, used to relabel indices.
 */
#ifndef SPACE_INDEX_POOL_H_INCLUDED
#define SPACE_INDEX_POOL_H_INCLUDED

#include "index.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace csl {

class Space;

/*!
 * \brief Gives the index names of a Space in turn and their IDs. The names are
 * taken cyclically from the names declared by the Space, and the ID is the
 * number of complete passes over these names already made. A pool starts at
 * the first name with ID 0, so two pools used in the same order always give
 * the same indices.
 */
class SpaceIndexPool {
  public:
    SpaceIndexPool(Space const *t_space, std::vector<std::string> t_names)
        : space(t_space), names(std::move(t_names))
    {
    }

    Space const *getSpace() const
    {
        return space;
    }

    /*!
     * \return The next name of the pool and its ID.
     */
    std::pair<std::string, Index::ID_type> next()
    {
        size_t const cycle = count / names.size();
        std::string  name  = names[count % names.size()];
        ++count;
        return {std::move(name), static_cast<Index::ID_type>(cycle)};
    }

  private:
    Space const             *space;
    std::vector<std::string> names;
    size_t                   count = 0;
};

} // namespace csl

#endif
