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
#ifndef OBJECT_SYMMETRY_H_INCLUDED
#define OBJECT_SYMMETRY_H_INCLUDED

#include <iostream>
#include <csignal>
#include <vector>
#include <map>
#include "std_vector_implementation.h"

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class ObjectPermutation<T> declaration        //
/*************************************************/
///////////////////////////////////////////////////

template<class T>
class ObjectSymmetry;

#define Id_Perm(T) csl::ObjectPermutation<T>()
#define Id_Sym(T) csl::ObjectSymmetry<T>(Id_Perm(T))

template<class T>
class ObjectPermutation {

    public:

    ObjectPermutation();

    explicit
    ObjectPermutation(const std::map<T, T>& otherMap);

    explicit
    ObjectPermutation(std::map<T, T>&& otherMap);

    ObjectPermutation(T    a,
                      T    b,
                      bool symmetric = true);

    ObjectPermutation(const ObjectPermutation<T>& other)            = default;

    ObjectPermutation(ObjectPermutation<T>&& other)                 = default;

    ObjectPermutation& operator=(const ObjectPermutation<T>& other) = default;

    ObjectPermutation& operator=(ObjectPermutation<T>&& other)      = default;

    ~ObjectPermutation();

    size_t size() const {
        return permutation.size();
    }

    bool empty() const {
        return permutation.empty();
    }

    typename std::map<T, T>::iterator begin() {
        return permutation.begin();
    }

    typename std::map<T, T>::iterator end() {
        return permutation.end();
    }

    typename std::map<T, T>::const_iterator begin() const {
        return permutation.begin();
    }

    typename std::map<T, T>::const_iterator end() const {
        return permutation.end();
    }

    void clear() {
        rightForm = true;
        permutation.clear();
    }

    bool getRightForm() const {
        return rightForm;
    }

    std::map<T, T> getPermutation() const {
        return permutation;
    }

    void add(T a,
             T b);

    bool operator==(const ObjectPermutation<T>& other) const;

    ObjectSymmetry<T> operator+(const ObjectPermutation<T>& other) const;

    ObjectSymmetry<T> operator+(const ObjectSymmetry<T>& other) const;

    ObjectPermutation<T> operator*(const ObjectPermutation<T>& other) const;

    ObjectSymmetry<T> operator*(const ObjectSymmetry<T>& other) const;

    ObjectPermutation<T> inverse() const;

    T operator[](const T& key) const;

    std::vector<T> operator[](const std::vector<T>& keys) const;

    static void toggleDisplayMode();

    template<class U>
    friend
    std::ostream& operator<<(std::ostream&               fout,
                             const ObjectPermutation<U>& perm);

    private:

    void checkRightForm();

    private:

    bool       rightForm = true;

    std::map<T, T> permutation;

    static bool    displayMode;
};

template<class T>
bool ObjectPermutation<T>::displayMode = true;

///////////////////////////////////////////////////
/*************************************************/
// Class ObjectSymmetry<T> declaration           //
/*************************************************/
///////////////////////////////////////////////////

template<class T>
class ObjectSymmetry {

    IMPLEMENTS_STD_VECTOR_NO_PB(ObjectPermutation<T>, permutations)

    public:

    ObjectSymmetry();

    explicit
    ObjectSymmetry(const ObjectPermutation<T>& first);

    explicit
    ObjectSymmetry(const std::vector<ObjectPermutation<T>>& t_perm);

    explicit
    ObjectSymmetry(std::vector<ObjectPermutation<T>>&& t_perm);

    ObjectSymmetry(const ObjectSymmetry<T>& other)            = default;

    ObjectSymmetry(ObjectSymmetry<T>&& other)                 = default;

    ObjectSymmetry& operator=(const ObjectSymmetry<T>& other) = default;

    ObjectSymmetry& operator=(ObjectSymmetry<T>&& other)      = default;
   
    void push_back(T    a,
                   T    b,
                   bool symmetric = true);

    void push_back(const ObjectPermutation<T>& object);

    std::vector<std::vector<T>> operator[](const std::vector<T>& keys) const;

    ObjectSymmetry<T> operator+(const ObjectPermutation<T>& other) const;

    ObjectSymmetry<T> operator+(const ObjectSymmetry<T>& other) const;

    ObjectSymmetry<T> operator*(const ObjectSymmetry<T>& other) const;

    ObjectSymmetry<T> operator*(const ObjectPermutation<T>& other) const;

    template<class U>
    friend
    std::ostream& operator<<(std::ostream&            fout,
                             const ObjectSymmetry<U>& perm);

    private:

    std::vector<ObjectPermutation<T>> permutations;
};

///////////////////////////////////////////////////
/*************************************************/
// Class ObjectPermutation<T> definition         //
/*************************************************/
///////////////////////////////////////////////////

template<class T>
ObjectPermutation<T>::ObjectPermutation()
{

}

template<class T>
ObjectPermutation<T>::ObjectPermutation(T    a,
                                        T    b,
                                        bool symmetric)
{
    add(a, b);
    if (symmetric)
        add(b, a);
}

template<class T>
ObjectPermutation<T>::ObjectPermutation(const std::map<T, T>& otherMap)
    :permutation(otherMap)
{
    checkRightForm();
}

template<class T>
ObjectPermutation<T>::ObjectPermutation(std::map<T, T>&& otherMap)
    :permutation(otherMap)
{
    checkRightForm();
}

template<class T>
ObjectPermutation<T>::~ObjectPermutation()
{

}

template<class T>
void ObjectPermutation<T>::checkRightForm()
{
    for (const auto& pair : permutation)
        if (permutation.count(pair.second) != 1) {
            rightForm = false;
            return;
        }
    rightForm = true;
}

template<class T>
void ObjectPermutation<T>::add(T a,
                               T b)
{
    permutation[a] = b;
    checkRightForm();
}

template<class T>
bool ObjectPermutation<T>::operator==(const ObjectPermutation<T>& other) const
{
    return (rightForm == other.rightForm
        and permutation == other.permutation);
}

template<class T>
ObjectSymmetry<T> ObjectPermutation<T>::operator+(
        const ObjectPermutation<T>& other) const
{
    return ObjectSymmetry<T>({*this, other});
}

template<class T>
ObjectSymmetry<T> ObjectPermutation<T>::operator+(
        const ObjectSymmetry<T>& other) const
{
    return other + *this;
}

template<class T>
ObjectPermutation<T> ObjectPermutation<T>::operator*(
        const ObjectPermutation<T>& other) const
{
    std::map<T, T> newMap;
    for (const auto& pair : other.permutation)
        newMap[pair.first] = (*this)[pair.second];
    for (const auto& pair : permutation)
        if (other.permutation.find(pair.first) == other.permutation.end())
            newMap[pair.first] = pair.second;

    return ObjectPermutation<T>(std::move(newMap));
}

template<class T>
ObjectSymmetry<T> ObjectPermutation<T>::operator*(
        const ObjectSymmetry<T>& other) const
{
    ObjectSymmetry<T> newSym(other);
    for (auto& perm : other)
        perm = *this * perm;
}

template<class T>
ObjectPermutation<T> ObjectPermutation<T>::inverse() const
{
    ObjectPermutation<T> newPerm;
    newPerm.rightForm   = rightForm;
    newPerm.displayMode = displayMode;
    for (const auto& pair : *this)
        newPerm[pair.second] = pair.first;
}

template<class T>
T ObjectPermutation<T>::operator[](const T& key) const
{
    auto pos = permutation.find(key);
    if (pos != permutation.end())
        return pos->second;
    return key;
}

template<class T>
std::vector<T> ObjectPermutation<T>::operator[](
        const std::vector<T>& keys) const
{
    if (not rightForm) 
        std::cerr << "Warning: applying a non-cyclic permutation.";
    std::vector<T> res;
    for (const auto& key : keys)
        res.push_back((*this)[key]);

    return res;
}

template<class T>
void ObjectPermutation<T>::toggleDisplayMode()
{
    displayMode = not displayMode;
}

template<class T>
std::ostream& operator<<(std::ostream&               fout,
                         const ObjectPermutation<T>& perm)
{
    if (ObjectPermutation<T>::displayMode) {
        fout << "Permutation of " << perm.size() << " elements.\n";
        for (const auto& pair : perm.permutation)
            fout << pair.first << " --> " << pair.second << std::endl;
    }
    else {
        for (const auto& pair : perm.permutation)
            fout << ": ( " << pair.first << ", " << pair.second << " ) ";
    }

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Class ObjectSymmetry<T> definition            //
/*************************************************/
///////////////////////////////////////////////////

template<class T>
ObjectSymmetry<T>::ObjectSymmetry()
{

}

template<class T>
ObjectSymmetry<T>::ObjectSymmetry(const ObjectPermutation<T>& first)
    :permutations(1, first)
{

}

template<class T>
ObjectSymmetry<T>::ObjectSymmetry(
        const std::vector<ObjectPermutation<T>>& t_perm)
    :permutations(t_perm)
{

}

template<class T>
ObjectSymmetry<T>::ObjectSymmetry(
        std::vector<ObjectPermutation<T>>&& t_perm)
    :permutations(t_perm)
{

}

template<class T>
void ObjectSymmetry<T>::push_back(T    a,
                                  T    b,
                                  bool symmetric)
{
    ObjectPermutation<T> perm(a, b);
    if (symmetric)
        perm.add(b, a);
    push_back(perm);
}

template<class T>
void ObjectSymmetry<T>::push_back(const ObjectPermutation<T>& value)
{
    permutations.push_back(value);
}

template<class T>
std::vector<std::vector<T>> ObjectSymmetry<T>::operator[](
        const std::vector<T>& keys) const
{
    std::vector<std::vector<T>> res;
    for (const auto& perm : permutations)
        res.push_back(perm[keys]);

    return res;
}

template<class T>
ObjectSymmetry<T> ObjectSymmetry<T>::operator+(
        const ObjectPermutation<T>& other) const
{
    ObjectSymmetry<T> newSym(*this);
    newSym.push_back(other);

    return newSym;
}

template<class T>
ObjectSymmetry<T> ObjectSymmetry<T>::operator+(
        const ObjectSymmetry<T>& other) const
{
    ObjectSymmetry<T> newSym(*this);
    newSym.insert(newSym.end(), other.begin(), other.end());

    return newSym;
}

template<class T>
ObjectSymmetry<T> ObjectSymmetry<T>::operator*(
        const ObjectPermutation<T>& other) const
{
    ObjectSymmetry<T> newSym(*this);
    for (auto& perm : newSym)
        perm = perm * other;

    return newSym;
}

template<class T>
ObjectSymmetry<T> ObjectSymmetry<T>::operator*(
        const ObjectSymmetry<T>& other) const
{
    ObjectSymmetry<T> newSym;
    for (const auto& permSelf : *this)
        for (const auto& permOther : other)
            newSym.push_back(permSelf * permOther);

    return newSym;
}

template<class T>
std::ostream& operator<<(std::ostream&            fout,
                         const ObjectSymmetry<T>& sym)
{
    ObjectPermutation<T>::toggleDisplayMode();
    fout << "Symmetry of " << sym.size() << " elements.\n";
    for (const auto& perm : sym)
        fout << "   " << perm << std::endl;
    ObjectPermutation<T>::toggleDisplayMode();

    return fout;
}

} // End of namespace csl

#endif
