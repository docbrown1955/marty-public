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
#ifndef FUNCTIONAL_H_INCLUDED
#define FUNCTIONAL_H_INCLUDED

#include <tuple>
#include "abstract.h"
#include "interface.h"
#include "utils.h"

namespace csl {

template<class ... Args>
class Functional {

    private:

    Expr init;

    std::tuple<Args...> args;

    public:

    Functional(Expr const& t_init,
               Args     ...t_args)
        :init(t_init)
    {
        args = std::tuple<Args...>(t_args...);
    }

    ~Functional() {}

    template<class ... T_Args>
    Expr operator()(T_Args ... t_args)
    {
        std::tuple<T_Args...> new_args(t_args...);
        static_assert(sizeof...(Args) >= sizeof...(T_Args));

        Expr res = DeepCopy(init);
        Replacer<Args...> rep;
        rep.template replace<0, T_Args...>(
                res,
                args,
                new_args);

        return res;
    }

    template<class ...T_Args>
    friend
    std::ostream& operator<<(
            std::ostream& out,
            Functional<T_Args...> func);

    template<class ...Arg1>
    struct Replacer {

    template<std::size_t i, class ...Arg2>
    Expr replace(
            Expr                & init,
            std::tuple<Arg1...> & oldArg,
            std::tuple<Arg2...> & newArg)
    {
        if constexpr (i == sizeof...(Arg2))
            return init;
        else {
            init = Replaced(init,
                           std::get<i>(oldArg),
                           std::get<i>(newArg));
            return replace<i + 1, Arg2...>(
                    init,
                    oldArg,
                    newArg);
        }
    }
    };
};

}

#endif
