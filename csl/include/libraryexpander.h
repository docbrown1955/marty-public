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
#ifndef CSL_LIBRARY_EXPANDER_H_INCLUDED
#define CSL_LIBRARY_EXPANDER_H_INCLUDED 

#include "abstract.h"
#include "numerical.h"
#include "index.h"
#include "space.h"

namespace csl {

struct Structure;
class LibraryExpander {

public:

    static void apply(
            Expr             &iprod,
            csl::Space const *space = &csl::Minkowski
            );

private:

    static bool expand(
            Expr             &iprod,
            csl::Space const *space
            );

    static bool toExpand(
            Expr       const &term,
            csl::Space const *space
            );

    static Structure parseTerm(
            Expr       const &iprod,
            csl::Space const *space
            ); 

    static std::vector<Structure> parse(
            Expr       const &isum,
            csl::Space const *space
            ); 

    static size_t nCommonIndices(
            Structure const &s1,
            Structure const &s2
            );

    static void merge(
            std::vector<Structure>       &A,
            std::vector<Structure> const &B
            );

    static std::vector<Structure> merge(
            std::vector<std::vector<Structure>> &init,
            csl::Space                    const *space
            );

    static Expr toExpr(
            Structure  const &init,
            csl::Space const *space
            );

    static Expr toExpr(
            std::vector<Structure> const &init,
            csl::Space             const *space
            );
};

struct Structure {

    struct ipair {

    private:
        csl::Index first;
        csl::Index second;

        inline void sort() {
            if (second < first)
                std::swap(first, second);
        }

    public:

        ipair() {}

        ipair(csl::Index t_first, 
              csl::Index t_second)
            :first(t_first),
            second(t_second)
        {
            sort();
        }

        inline csl::Index const &getFirst()  const { return first; }
        inline csl::Index const &getSecond() const { return second; }

        inline void setFirst(csl::Index const &index) {
            first = index;
            sort();
        }
        inline void setSecond(csl::Index const &index) {
            second = index;
            sort();
        }

        inline bool operator==(ipair const &other) const {
            return first == other.first
                and second == other.second;
        }
        inline bool operator!=(ipair const &other) const {
            return !(*this == other);
        }

        inline bool operator<(ipair const &other) const {
            if (first < other.first)
                return true;
            return (!(other.first < first)) ? second < other.second : false;
        }
    };

public:

    Structure();

    Structure(Structure const &other);
    Structure& operator=(Structure const &other);

    Structure(Structure &&other) = default;
    Structure& operator=(Structure &&other) = default;

    ~Structure() {}

    inline bool isZero() const {
        return factor == CSL_0;
    }

    void add(Expr const &t_factor);

    void mult(Expr const &t_factor);

    void mult(ipair const &p);
    
    void mult(
             ipair const &p,
             Expr  const &t_factor
             );

    void simplify();

    Structure& operator*=(Structure const &other);
    Structure  operator* (Structure const &other) const;

    bool operator==(Structure const &other) const;

    static int commonIndex(
            ipair const &p1,
            ipair const &p2
            );

public:
    size_t reserveStorage = 10;

    Expr factor;
    std::vector<ipair> indices;
};

}

#endif 
