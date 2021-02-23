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
#ifndef INDEX_H_INCLUDED
#define INDEX_H_INCLUDED

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include "std_vector_implementation.h"

namespace csl {
    class Space;
}

/*!
 * \namespace cslIndex
 * 
 * \brief Encapsulate types of indices and tensor in a small namespace.
 */
namespace cslIndex{

    /*!
     * \enum Type
     *
     * \brief Type of Index.
     */
    enum Type{

        Free, /*!< Free Index. */
        Dummy, /*!< Index contracted (summed). */
        Fixed, /*!< Index whose value is fixed to a given integer. */
    };

    /*!
     * \enum TensorType
     *
     * \brief Type of TensorElement.
     */
    enum TensorType{

        Generic,
        Metric, /*!< Metric tensor.*/
        Delta, /*!< Kronecker Delta tensor.*/
    };
}

namespace csl {

/*!
 * 
 *
 * \brief Index object that is used for indicial objects.
 */
class Index{

    public:

    using ID_type = unsigned int;

    private:

    /*!
     * \brief Space in which the Index lives.
     */
    const Space* space = nullptr;

    /*!
     * \brief Name of the index.
     */
    char nameOrValue;
    
    ID_type id;

    /*!
     * \brief Type of the index.
     */
    char type;

    public:

    /*!
     * \brief Default constructor
     */
    Index();

    /*!
     * \brief Constructor that initializes only the name of the Index.
     *
     * \param t_name Name of the Index.
     */
    explicit Index(const std::string& t_name,
                   unsigned short     t_id = 0);

    Index(char value);

    /*!
     * \brief Copy constructor = C++ default.
     *
     * \param index Index to copy.
     */
    Index(const Index& index) = default;

    /*!
     * \brief Constructor that initializes fully the Index. 
     *
     * \param t_name Name if the Index.
     * \param t_space Space in which the Index lives.
     */
    Index(const std::string& t_name,\
          const Space*       t_space,
          unsigned short     t_id = 0);

    /*!
     * \brief Destructor.
     */
    ~Index(){};

    std::string_view getName() const;

    std::string getIndexCodeName() const;

    inline ID_type getID() const {
        return id;
    }

    /*!
     * \return The value of the Index if it has one (\b value).
     * \return -1 else.
     */
    inline char getValue() const {
        return nameOrValue;
    }

    /*!
     * \return The sign of the Index.
     */
    bool getSign() const {
        return nameOrValue >= 0;
    }

    /*!
     * \return \b True if the Index is free.
     * \return \b False else.
     */
    inline bool getFree() const {
        return type == 0;
    }

    /*!
     * \return The type of the Index.
     */
    inline cslIndex::Type getType() const {
        return static_cast<cslIndex::Type>(type);
    }

    /*!
     * \return The dimension of the Space in which the Index lives.
     */
    int getMax() const;

    /*!
     * \return The Space in which the Index lives.
     */
    inline const Space* getSpace() const {
        return space;
    }

    Index rename() const;

    /*!
     * \return A copy of the Index with a flipped sign \b if the Space is 
     * signed.
     */
    Index getFlipped() const;

    /*!
     * \param t_name New name for the Index.
     */
    void setName(const std::string& t_name);

    void setID(ID_type t_id);

    /*!
     * \brief Changes the current space of the Index.
     * \param t_space New space for the Index.
     */
    void setSpace(const Space* t_space);

    /*!
     * \brief Sets the value of the Index and therefore sets its type to
     * cslIndex::Fixed.
     * \param t_value New value for the Index.
     */
    void setValue(char t_value);

    /*!
     * \param t_sign New sign for the Index.
     */
    void setSign(bool t_sign);

    void flipSign();

    /*!
     * \param t_free Sets the free-property of the Index. Free = true, Dummy = 
     * false.
     */
    void setFree(bool t_free);

    /*!
     * \param t_type Sets the type of the Index.
     */
    void setType(cslIndex::Type t_type);

    /*!
     * \brief Tests if the Index can be contracted with \b t_index. Basically,
     * returns true if the two indices are free and contractible with respect
     * to Einstein's conventions.
     *
     * \param t_index Index with which we test the contraction.
     *
     * \return 
     */
    bool testContraction(const Index& t_index) const;

    /*!
     * \brief Prints the Index in standard output.
     */
    void print() const;

    /*!
     * \brief Returns the LaTeX name of the Index (for now there is no
     * difference with the regular name).
     */
    std::string printLaTeX() const;

    void printDefinition(
            std::ostream &out,
            int           indentSize
            ) const;

    /*!
     * \brief operator=, copy the properties of \b index in the current object.
     *
     * \param index Index to copy.
     *
     * \return A reference to the current Index.
     */
    Index& operator=(const Index& index) = default;

    /*!
     * \brief Sets the value of the Index. See Index::setValue().
     * \param t_value New value for the Index.
     * \return A reference to the modified Index.
     */
    Index& operator=(int t_value);

    /*!
     * \brief Equivalent to Index::operator==() except for dummy indices for 
     * which the name does not count in the comparison.
     *
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are equal (to a renaming for dummy
     * indices).
     * \return \b False else.
     */
    bool compareWithDummy(const Index& t_index) const;

    /*!
     * \brief Tells if two indices are equal, independently of their sign if 
     * they live in a signed Space. For now equivalent to Index::operator==().
     *
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are equal (independently of their 
     * sign).
     * \return \b False else.
     */
    bool compareWithoutSign(const Index& t_index) const;

    /*!
     * \brief Tells if two indices are equal, taking their sign into account,
     * differently from the Index::compareWithoutSign() function.
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are strictly equal.
     * \return \b False else.
     */
    bool exactMatch(const Index& t_index) const;

    Index& operator++();

    Index operator++(int);

    /*!
     * \brief operator==, compares two indices and tells if they are equal, 
     * independently of their sign, i.e. two indices are equals if they have
     * the same name, space, and type. Equivalent to Index::compareWithoutSign()
     * .
     *
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are equal independantly of their sign.
     * \return \b False else.
     */
    bool operator==(const Index& t_index) const;


    /*!
     * \brief operator==, compares two indices and tells if they are equal, 
     * independently of their sign, i.e. two indices are equals if they have
     * the same name, space, and type. Equivalent to Index::compareWithoutSign()
     * .
     *
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are equal independantly of their sign.
     * \return \b False else.
     */
    bool operator==(int value) const;

    /*!
     * \brief operator==, compares two indices and tells if they are equal, 
     * independently of their sign, i.e. two indices are equals if they have
     * the same name, space, and type. Equivalent to Index::compareWithoutSign()
     * .
     *
     * \param t_index Index to compare.
     *
     * \return \b True if the two indices are equal independantly of their sign.
     * \return \b False else.
     */
    bool operator==(size_t value) const;

    /*!
     * \brief operator!=, opposite of the operator!=
     *
     * \param t_index Index to compare.
     *
     * \return not (operator==(t_index)).
     */
    bool operator!=(const Index& t_index) const;

    bool operator!=(int value) const;

    bool operator!=(size_t value) const;

    /*!
     * \brief operator|=, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if the two indices have the same simplicity.
     */
    bool operator|=(const Index& index) const;

    /*!
     * \brief operator&=, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if the two indices have different simplicities.
     */
    bool operator&=(const Index& index) const;

    /*!
     * \brief operator<, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if *this is simpler than \b index.
     */
    bool operator<(const Index& index) const;

    /*!
     * \brief operator>, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if \b index is simpler than *this.
     */
    bool operator>(const Index& index) const;

    /*!
     * \brief operator<=, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if < or |=.
     */
    bool operator<=(const Index& index) const;

    /*!
     * \brief operator>=, simplicity comparator.
     *
     * \param index Index to compare.
     *
     * \return \b True if > or |=.
     */
    bool operator>=(const Index& index) const;

    /*!
     * \brief operator<<, displays the Index \b index in output \b fout.
     *
     * \param fout Output flow.
     * \param index Index to display.
     *
     * \return The output flow modified.
     */
    friend std::ostream& operator<<(std::ostream& fout, const Index& index);
};

std::vector<Index> integerToIndices(const std::vector<int>& indices);

/*!
 * \brief operator!, unary operator on Index that change the free property of
 * the Index.
 *
 * \param index Index to copy.
 *
 * \return A free Index similar to \b index if it is dummy.
 * \return A dummy Index similar to \b index if it is free.
 */
Index operator!(const Index& index);

/*!
 * \brief operator+, unary operator on Index that returns a similar Index with
 * sign = 1 if the Space is signed, i.e. an Index up.
 *
 * \param index The Index to copy.
 *
 * \return An index similar to \b index with a sign 1 i.e. a up Index.
 */
Index operator+(const Index& index);

/*!
 * \brief operator-, unary operator on Index that returns a similar Index with
 * sign = 0 if the Space is signed, i.e. an Index down.
 *
 * \param index The Index to copy.
 *
 * \return An index similar to \b index with a sign 0 i.e. a down Index.
 */
Index operator-(const Index& index);

/*!
 * 
 *
 * \brief Manages a std::vector of Index, to be used by an TensorElement.
 */
class IndexStructure{

    IMPLEMENTS_STD_VECTOR_NO_OP(Index, index)

    private:

    /*!
     * \brief std::vector of Index objects. Represents the indicial structure
     * of an TensorElement.
     */
    std::vector<Index> index;

    public:

    /*!
     * \brief Default constructor, empty structure.
     */
    IndexStructure();

    /*!
     * \brief Constructor that initializes only the size of \b index.
     *
     * \param t_nIndices Number of indices in the IndexStructure.
     */
    explicit IndexStructure(int t_nIndices);

    /*!
     * \brief Copy constructor.
     *
     * \param structure IndexStructure to copy in initialization.
     */
    IndexStructure(const IndexStructure& structure);

    IndexStructure(IndexStructure&&) = default;

    IndexStructure& operator=(IndexStructure&&) = default;

    /*!
     * \brief Constructor with one parameter: the list of indices in the
     * structure.
     *
     * \param structure The list of indices initializating the IndexStructure.
     */
    explicit IndexStructure(const std::initializer_list<Index>& structure);

    /*!
     * \brief Constructor with one parameter: the list of indices in the
     * structure.
     *
     * \param structure A std::vector of Index initializating the
     * IndexStructure.
     */
    explicit IndexStructure(const std::vector<Index>& structure);

    /*!
     * \brief Destructor.
     */
    ~IndexStructure(){}

    /*!
     * \return The entire \b index vector.
     */
    std::vector<Index> const &getIndex() const;

    const std::vector<Index>& getIndexView() const;

    /*!
     * \return The entire \b index vector.
     */
    std::vector<Index>& getIndex();


    /*!
     * \brief Search for a particular Index in the structure. 
     *
     * \return A const_iterator corresponding to the position of \b t_index
     * in the structure if it is found.
     * \return The const_iterator IndexStructure::end() if \b t_index is not
     * found.
     */
    std::vector<Index>::const_iterator find(const Index& t_index) const;

    /*!
     * \return The part of the vector \b index that contains free indices.
     */
    IndexStructure getFreeStructure() const;

    /*!
     * \return A copy of the IndexStructure where indices in places \b i1 and
     * \b i2 have been swapped.
     */
    IndexStructure getSinglePermutation(int i1, int i2) const;

    /*!
     * \param permutation std::vector of integers of the size of the
     * IndexStructure that permutes indices.
     *
     * \return A copy of the IndexStructure where the permutation \b permutation
     * has been applied.
     */
    IndexStructure getPermutation(const std::vector<int>& permutation) const;

    /*!
     * \brief Sets the Index in position \b iIndex to \b newIndex.
     *
     * \param newIndex New Index to place in the IndexStructure.
     * \param iIndex Position where to place \b newIndex, default 0 if not given
     * by the user.
     */
    void setIndex(const Index& newIndex, int iIndex=0);

    void reset();

    /*!
     * \brief operator=, Copies the IndexStructure \b structure. C++ default.
     *
     * \param structure IndexStructure to copy.
     *
     * \return A reference to the modified IndexStructure.
     */
    IndexStructure& operator=(const IndexStructure& structure) = default;

    /*!
     * \brief operator+=, adds an index to the IndexStructure.
     *
     * \param newIndex Index to append at the end of the IndexStructure.
     *
     * \return A reference to the modified IndexStructure.
     */
    IndexStructure& operator+=(const Index& newIndex);

    /*!
     * \brief operator+=, adds another IndexStructure at the end of the current.
     * This function concatenates the two structures.
     *
     * \param structure IndexStructure to concatenate to *this.
     *
     * \return A reference to the modified IndexStructure.
     */
    IndexStructure& operator+=(const IndexStructure& structure);

    IndexStructure operator+(const csl::Index& index) const;

    /*!
     * \brief operator+, concatenates the current IndexStructure with \b 
     * structure and returns the result.
     *
     * \param structure IndexStructure with which *this is concatenated.
     *
     * \return The concatenation of *this and \b structure.
     */
    IndexStructure operator+(const IndexStructure& structure) const;

    /*!
     * \brief Compares the IndexStructure with \b structure. Each index must
     * match exactly (see Index::exactMatch()) with the Index at the same place
     * in the second structure.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if the two structures match exactly.
     * \return \b False else.
     */
    bool exactMatch(const IndexStructure& structure) const;
    
    /*!
     * \brief Compares *this with \b structure index by index (in order) using
     * the function Index::compareWithDummy().
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if the two structures are equal wrt 
     * Index::compareWithDummy().
     * \return \b False else.
     */
    bool compareWithDummy(const IndexStructure& structure) const;

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
     * \param structure IndexStructure to compare.
     * \param constraints Map of Index to Index storing constraints of 
     * correpondance between dummy indices.
     *
     * \return \b True if the two structures are equal wrt 
     * Index::compareWithDummy().
     * \return \b False else.
     */
    bool compareWithDummy(const IndexStructure& structure,
                          std::map<Index,Index>& constraints,
                          bool keepAllCosntraints = false) const;

    /*!
     * \brief operator==, compares the IndexStructure to \b structure in terms
     * of free structure, independently of the dummy indices or the order of
     * free indices.
     *
     * \param structure IndexStructure to compare with *this.
     *
     * \return \b True if the two structures are equivalent, i.e. are compatible
     * as two terms of a sum wrt Einstein's convention.
     * \return \b False else.
     */
    bool operator==(const IndexStructure& structure) const;

    /*!
     * \brief operator!=, returns the opposite of IndexStructure::operator==().
     *
     * \param structure IndexStructure to compares
     *
     * \return not IndexStructure::operator==(structure).
     */
    bool operator!=(const IndexStructure& structure) const;

    /*!
     * \brief operator|=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if the two indices have the same simplicity.
     */
    bool operator|=(const IndexStructure& structure) const;

    /*!
     * \brief operator&=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if the two indices have different simplicities.
     */
    bool operator&=(const IndexStructure& structure) const;

    /*!
     * \brief operator<, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if *this is simpler than \b structure.
     */
    bool operator<(const IndexStructure& structure) const;

    /*!
     * \brief operator>, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if \b structure is simpler than *this.
     */
    bool operator>(const IndexStructure& structure) const;

    /*!
     * \brief operator<=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if < or |=.
     */
    bool operator<=(const IndexStructure& structure) const;

    /*!
     * \brief operator>=, simplicity comparator using comparators between 
     * indices, starting by comparing the first ones.
     *
     * \param structure IndexStructure to compare.
     *
     * \return \b True if > or |=.
     */
    bool operator>=(const IndexStructure& structure) const;

    /*!
     * \brief operator[], accessor to the Index at position \b i.
     *
     * \param i Position of the Index to get.
     *
     * \return A copy of the Index at position \b i.
     */
    Index operator[](int i) const;

    /*!
     * \brief operator[], accessor to the Index at position \b i.
     *
     * \param i Position of the Index to get.
     *
     * \return A reference to the Index at position \b i (then modifiable).
     */
    Index& operator[](int i);

    /*!
     * \brief Displays the IndexStructure in output /b fout.
     *
     * \param fout Output flux.
     * \param structure IndexStructure to display.
     *
     * \return A reference to the modifed flux \b fout.
     */
    friend std::ostream& operator<<(std::ostream&         fout,
                                    const IndexStructure& structure);
};

/////
// Inline functions

inline IndexStructure::IndexStructure() {}
inline IndexStructure::IndexStructure(int t_nIndices)
    : index(std::vector<Index>(t_nIndices,Index())){}
inline IndexStructure::IndexStructure(const IndexStructure& t_index):
    index(t_index.index){}
inline IndexStructure::IndexStructure(const std::initializer_list<Index>& t_index):
    IndexStructure(std::vector<Index>(t_index)){}
/////


} // End of namespace csl

#endif
