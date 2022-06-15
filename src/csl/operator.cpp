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

#include "operator.h"
#include "utils.h"

namespace csl {

bool pullLeft(csl::vector_expr &argument, size_t pos, size_t &begin)
{
    bool blocked = false;
    if (pos > 0)
        for (size_t j = pos; j-- > begin;)
            if (*Commutation(argument[j], argument[pos]) != CSL_0) {
                blocked = true;
                break;
            }
    if (not blocked) {
        Expr foo2 = argument[pos];
        argument.erase(argument.begin() + pos);
        argument.insert(argument.begin() + begin, foo2);
        ++begin;
        return true;
    }
    return false;
}

bool pullRight(csl::vector_expr &argument, size_t &pos, size_t &end)
{
    bool blocked = false;
    if (pos > 0)
        for (size_t j = pos + 1; j < end; ++j)
            if (*Commutation(argument[j], argument[pos]) != CSL_0) {
                blocked = true;
                break;
            }
    if (not blocked) {
        Expr foo2 = argument[pos];
        argument.erase(argument.begin() + pos);
        argument.insert(argument.begin() + end - 1, foo2);
        --pos;
        --end;
        return true;
    }
    return false;
}

void getParts(const csl::vector_expr &argument,
              size_t                  begin,
              size_t                  end,
              Expr &                  left,
              Expr &                  mid,
              Expr &                  right)
{
    if (begin > 0)
        left = prod_s(
            csl::vector_expr(argument.begin(), argument.begin() + begin));
    else
        left = CSL_1;

    if (end - begin > 0)
        mid = prod_s(csl::vector_expr(argument.begin() + begin,
                                      argument.begin() + end));
    else
        mid = CSL_1;

    if (end != argument.size())
        right
            = prod_s(csl::vector_expr(argument.begin() + end, argument.end()));
    else
        right = CSL_1;
}

} // End of namespace csl
