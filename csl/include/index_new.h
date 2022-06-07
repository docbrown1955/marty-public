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
#ifndef INDEX_NEW_H_INCLUDED
#define INDEX_NEW_H_INCLUDED

#include "index.h"
#include "error.h"
#include "support.h"

namespace csl {

template<std::size_t N>
class IndexStructure_new;

class IndexStructureView {
    
    Index* first;
    Index* last;

    public:

    template<std::size_t N>
    IndexStructureView(IndexStructure_new<N> const& structure)
        :first(structure.begin()),
        last(structure.end())
    {

    }

    inline bool empty() const {
        return first == last;
    }

    inline std::size_t size() const {
        return (last - first);
    }

    inline const Index* begin() const {
        return first;
    }

    inline const Index* end() const {
        return last;
    }

    inline Index* begin() {
        return first;
    }

    inline Index* end() {
        return last;
    }
};


template<std::size_t N>
class IndexStructure_new {

    public:

    using iterator       
        = typename std::array<csl::Index, N>::iterator;
    using const_iterator 
        = typename std::array<csl::Index, N>::const_iterator;
    using reverse_iterator       
        = typename std::array<csl::Index, N>::reverse_iterator;
    using const_reverse_iterator 
        = typename std::array<csl::Index, N>::const_reverse_iterator;

    IndexStructure_new()  = default;
    ~IndexStructure_new() = default;
    IndexStructure_new(IndexStructure_new const&) = default;
    IndexStructure_new(IndexStructure_new&&)      = default;
    IndexStructure_new<N>& operator=(IndexStructure_new<N> const&) = default;
    IndexStructure_new<N>& operator=(IndexStructure_new<N>&&)      = default;

    explicit
    IndexStructure_new(csl::Index const& first) {
        static_assert(N == 1);
        indices[0] == first;
    }

    IndexStructure_new(IndexStructureView const& other) 
        :indices(other.begin(), other.end())
    {
        CSL_ASSERT_SPEC(other.size() == N,
                CSLError::ValueError,
                "Size mismatch for construction of IndexStructure, "
                + toString(other.size()) + " given, " + toString(N)
                + " given.");
    }

    explicit
    IndexStructure_new(std::array<csl::Index, N> const& t_indices)
        :indices(t_indices)
    {

    }

    explicit
    IndexStructure_new(std::array<csl::Index, N>&& t_indices)
        :indices(std::move(t_indices))
    {

    }

    constexpr bool empty() const {
        return N == 0;
    }

    constexpr std::size_t size() const {
        return N;
    }

    inline iterator begin() {
        return indices.begin();
    }

    inline iterator end() {
        return indices.end();
    }

    inline const_iterator begin() const {
        return indices.begin();
    }

    inline const_iterator end() const {
        return indices.end();
    }

    inline reverse_iterator rbegin() {
        return indices.rbegin();
    }

    inline reverse_iterator rend() {
        return indices.rend();
    }

    inline const_reverse_iterator rbegin() const {
        return indices.rbegin();
    }

    inline const_reverse_iterator rend() const {
        return indices.rend();
    }

    csl::Index operator[](std::size_t pos) const {
        return indices[pos];
    }

    csl::Index& operator[](std::size_t pos) {
        return indices[pos];
    }

    template<std::size_t M>
    IndexStructure_new<M+N> operator+(IndexStructure_new<M> const& other) const
    {
        IndexStructure_new<M+N> newStruct;
        auto iter = newStruct.begin();
        for (const auto& index : *this)
            iter++ = index;
        for (const auto& index : other)
            iter++ = index;
    }

    std::vector<csl::Index> getVectorIndex() const {
        return std::vector<csl::Index>(indices.begin(), indices.end());
    }

    void reset() {
        for (auto& index : indices) {
            index = index.rename();
            index.setFree(true);
        }
    }

    /*!
     * \brief Compares the IndexStructure_new<N> with \b structure. Each index must
     * match exactly (see Index::exactMatch()) with the Index at the same place
     * in the second structure.
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if the two structures match exactly.
     * \return \b False else.
     */
    bool exactMatch(const IndexStructure_new<N>& structure) const {
        auto self = begin();
        for (auto other = structure.begin(); other != structure.end();
                ++other, ++self)
            if (not self->exactMatch(*other))
                return false;
        return true;
    }
    
    /*!
     * \brief Compares *this with \b structure index by index (in order) using
     * the function Index::compareWithDummy().
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if the two structures are equal wrt 
     * Index::compareWithDummy().
     * \return \b False else.
     */
    bool compareWithDummy(const IndexStructure_new<N>& structure) const {
        std::map<csl::Index,csl::Index> constraints;
        return compareWithDummy(structure, constraints);
    }

    /*!
     * \brief Compares *this with \b structure index by index (in order) using
     * the function Index::compareWithDummy().
     * \details The map between indices contains matches between dummy indices.
     * Dummy indices can a priori match between each other independently of 
     * their names, but one an Index has been matched with another, the pair 
     * is put into \b constraints. This \b constraints map allows then to 
     * compare properly complex expressions containing dummy indices. The map
     * \b constraints is modified in the function.
     *
     * \param structure IndexStructure_new<N> to compare.
     * \param constraints Map of Index to Index storing constraints of 
     * correpondance between dummy indices.
     *
     * \return \b True if the two structures are equal wrt 
     * Index::compareWithDummy().
     * \return \b False else.
     */
    bool compareWithDummy(const IndexStructure_new<N>& structure,
                          std::map<Index,Index>& constraints,
                          bool keepAllCosntraints = false) const {
        auto self = begin();
        for (auto other = structure.begin(); other != structure.end(); 
                ++other, ++self) {
            // If the index is free, they must be the same
            if ((*self).getFree() or (*self).getType() == cslIndex::Fixed) {
                if (not (*self).exactMatch((*other)))
                    return false;
            }
            // Else we check if the dummie are already constrained, if the 
            // constraint is respected, or add a new constraint
            else {
                // (*other) must also be dummy here and must correspond (to a 
                // renaming) to (*self)
                if (not (*self).compareWithDummy((*other)))
                    return false;
                if ((*self) == (*other)) {
                    for (const auto& cons : constraints)
                        if (cons.second == (*self) and cons.first != cons.second)
                            return false;
                }
                if (constraints.find((*self)) == constraints.end()) {
                    if (constraints.find((*other)) == constraints.end()) {
                        // Here the dummies are not constrained: comparison
                        // is ok, and we add the new correspondance
                        constraints[(*self)] = (*other);
                    }
                    else {
                        if (constraints[(*other)] != (*self))
                            return false;
                        constraints[(*self)] = (*other);
                    }
                }
                else {
                    if (constraints.find(*other) == constraints.end()) {
                        if (not (constraints[*self] == *other))
                            return false;
                    }
                    else if (constraints[*self] != *other
                            or constraints[*other] != *self)
                        return false;
                    if (not keepAllCosntraints)
                        constraints.erase(*self);
                }
            }
        }

        return true;
    }   

    /*!
     * \brief operator==, compares the IndexStructure_new<N> to \b structure in terms
     * of free structure, independently of the dummy indices or the order of
     * free indices.
     *
     * \param structure IndexStructure_new<N> to compare with *this.
     *
     * \return \b True if the two structures are equivalent, i.e. are compatible
     * as two terms of a sum wrt Einstein')s convention.
     * \return \b False else.
     */
    bool operator==(const IndexStructure_new<N>& structure) const {
        int t_nIndices = structure.size();
        std::vector<int> indicesLeft(0);
        for (int i=0; i<t_nIndices; ++i)
            // comparing only free indices
            if (structure[i].getFree())
                indicesLeft.push_back(i);

        for (const auto& index : indices) {
            // Comparing only free indices
            if (index.getFree()) {
                bool match = false;
                for (std::size_t j=0; j!=indicesLeft.size(); ++j) {
                    // The free structure needs exact match to be correct
                    if (index.exactMatch(structure[indicesLeft[j]])) {
                        match = true;
                        indicesLeft.erase(indicesLeft.begin()+j);
                        break;
                    }
                }
                if (not match)
                    return false;
            }
        }

        if (not indicesLeft.empty())
            return false;

        return true;
    }

    /*!
     * \brief operator!=, returns the opposite of IndexStructure_new<N>::operator==().
     *
     * \param structure IndexStructure_new<N> to compares
     *
     * \return not IndexStructure_new<N>::operator==(structure).
     */
    bool operator!=(const IndexStructure_new<N>& structure) const {
        return not (*this == structure);
    }

    /*!
     * \brief operator<, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if *this is simpler than \b structure.
     */
    bool operator<(const IndexStructure_new<N>& structure) const {
        auto other = structure.begin();
        for (const auto& index : *this) {
            if (index < *other)
                return true;
            else if (*other < index)
                return false;
            ++other;
        }

        return false;
    }

    /*!
     * \brief operator>, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if \b structure is simpler than *this.
     */
    bool operator>(const IndexStructure_new<N>& structure) const {
        return structure < *this;
    }

    /*!
     * \brief operator<=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if < or |=.
     */
    bool operator<=(const IndexStructure_new<N>& structure) const {
        return (*this < structure or not (structure < *this));
    }

    /*!
     * \brief operator>=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure_new<N> to compare.
     *
     * \return \b True if > or |=.
     */
    bool operator>=(const IndexStructure_new<N>& structure) const {
        return structure <= *this;
    }

    private:

    std::array<csl::Index, N> indices;
};

// void f() {
//     IndexStructure_new<5> index;
//     for (auto& i : index)
//         i.setName("FRE");
// }

}

#endif
