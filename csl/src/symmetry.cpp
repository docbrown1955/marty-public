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

#include "abstract.h"
#include "space.h"
#include "error.h"
#include "utils.h"
#include "symmetry.h"

using namespace std;

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class Permutation                             //
/*************************************************/
///////////////////////////////////////////////////

Permutation::Permutation():
    order(0),
    sign(0),
    symmetry(0),
    permutation(vector<int>(1,0))
{

}

Permutation::Permutation(int n)
    :order(0), 
    sign(0), 
    symmetry(0),  
    permutation(std::vector<int>(n))
{
    for (int i=0; i!=n; ++i)
        permutation[i] = i;
}

Permutation::Permutation(const std::vector<int>& t_permutation)
    :order(0), 
    sign(0), 
    symmetry(0),
    permutation(t_permutation)
{

}

Permutation::Permutation(int n, const std::vector<int>& list)
    :Permutation(n,std::vector<std::vector<int>>({list}))
{}


Permutation::Permutation(
        int                               n,
        const std::vector<int>& list,
        int                               sym)
    :Permutation(n,{list})
{
    symmetry = sym;
}

Permutation::Permutation(
        int n,
        const std::vector<std::vector<int> >& list)
    :order(0), 
    sign(0), 
    symmetry(0)
{
    std::vector<int> indicesLeft(n);
    for (int i = 0; i != n; ++i)
        indicesLeft[i] = i;
    // initializes the permutation to the identity
    permutation = indicesLeft;
    for (auto cycle = list.begin(); cycle != list.end(); ++cycle) {
        if (cycle->size() > 0) {
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            /// Here just test of validity of the cycle
            for (auto element = cycle->begin()+1; element != cycle->end();
                                                  ++element) {
                if (indicesLeft[*element] < 0 or
                    indicesLeft[*element] >= n)
                    callError(cslError::OutOfBounds,
    "Permutation::Permutation(const init_list<init_list<int> >& list)",*element);
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            // If the cycle is valid : 
                // The current index has been used, it cannot appear a second
                // time in the permutation
                indicesLeft[*element] = -1;
                // The element at place i is placed at the index i-1
                permutation[*(element-1)] = *element;
            }
            if (cycle->size() > 1) 
                permutation[*(cycle->end()-1)] = *cycle->begin();
        }
    }
}

int Permutation::getOrder()
{
    if (order > 0) 
        return order;
    Permutation identity(size());
    order = 1;
    Permutation perm = *this;
    while(perm != identity) {
        perm = (*this)*perm;
        ++order;
    }

    return order;
}

int Permutation::getSign()
{
    if (sign != 0) 
        return sign;
    sign = 1;
    Permutation foo(permutation);
    for (size_t i = 0; i != size(); ++i) {
        if (foo[i] != (int)i) {
            for (size_t j = i+1; j != size(); ++j) {
                if (foo[j] == (int)i) {
                    std::swap(foo[i],foo[j]);
                    sign *= -1;
                    break;
                }
            }
        }
    }

    return sign;
}

int Permutation::getSymmetry() const {
    return symmetry;
}

void Permutation::setSymmetry(int t_symmetry) {
    if (t_symmetry != 1 and
        t_symmetry != 0 and 
        t_symmetry != -1)
        return;
    symmetry = t_symmetry;
}

void Permutation::adjustToSize(size_t newSize)
{
    if (size() < newSize) {
        for (size_t i = size(); i != newSize; ++i)
            push_back(i);
    }
}

void Permutation::applyRedefinition(const vector<size_t>& redefinition)
{
    if (not (redefinition.size() == size())) {
        cout << size() << "  " << redefinition.size() << endl;
        callError(cslError::UndefinedBehaviour,
                "Permutation::applyRedefinition()",
                "Applying redefinition on a permutation of different size!");
    }
    size_t max = 0;
    for (auto& element : *this) {
        if (redefinition[element] > max)
            max = redefinition[element];
        element = redefinition[element];
    }
    vector<int> newPermutation(max+1, -1);
    for (size_t i = 0; i != redefinition.size(); ++i)
        newPermutation[redefinition[i]] = permutation[i];
    if (max+1 > size())
        for (size_t i = 0; i != max+1; ++i)
            if (newPermutation[i] == -1)
                newPermutation[i] = i;

    permutation = std::move(newPermutation);
}

vector<int> Permutation::getPermutation() const {
    return permutation;
}

bool Permutation::operator==(const Permutation& t_permutation) const {
    if (size() != t_permutation.size()) 
        return false;
    for (size_t i = 0; i < size(); ++i)
        if (permutation[i] != t_permutation[i])
            return false;

    return true;
}

bool Permutation::operator!=(const Permutation& t_permutation) const {
    return (not (*this==t_permutation));
}

Permutation Permutation::operator*(const Permutation& t_permutation) const
{
    bool smaller;
    std::vector<int> newPerm;
    if (size() < t_permutation.size()) {
        smaller = true;
        newPerm = vector<int>(t_permutation.size());
    }
    else {
        smaller = false;
        newPerm = vector<int>(size());
    }
    for (size_t i = 0; i != size(); ++i)
        if (smaller and t_permutation[i] >= (int)size())
            newPerm[i] = t_permutation[i];
        else
            newPerm[i] = permutation[t_permutation[i]];

    Permutation rep = Permutation(newPerm);
    rep.setSymmetry(symmetry*t_permutation.getSymmetry());

    return rep;
}

std::ostream& operator<<(std::ostream& fout, const Permutation& permutation)
{
    fout<<"Permutation of "<<permutation.size()<<" elements: ";
    for (size_t i = 0; i != permutation.size(); ++i)
        fout<<permutation[i]<<" ";
    
    return fout;
}

///////////////////////////////////////////////////
// Utils for permutations, as for example getting
// all permutations spanned by an intial set.
///////////////////////////////////////////////////

// Returns all permutations of elements in a vector<int>
// Returns then a vector<vector<int> >.
// Ex permutations({1,2,3}) =
// { {1,2,3}, {1,3,2}, {2,1,3}, {2,3,1}, {3,1,2}, {3,2,1} }
vector<vector<int> > permutations(vector<int> init)
{
    // Recursive function

    const int n = init.size();
    if (n == 0) 
        return vector<vector<int> >(0);
    if (n == 1)
        return vector<vector<int> >(1,std::move(init));

    vector<vector<int> > rep(factorial(init.size()));
    rep[0] = init;
    size_t index = 0;
    while (std::next_permutation(init.begin(), init.end()))
        rep[++index] = init;

    return rep;

    // for (int i=0; i<n; ++i) {
    //     // the whole set of permutations is the set of vectors such that for 
    //     // each possible first integers the n-1 other integers span all 
    //     // the permutations of n-1 elements.
    //     //
    //     // Here we suppress the element i
    //     foo.erase(foo.begin()+i);
    //     // And get all the permutations of the n-1 other elements in 
    //     // intermediateRep.
    //     vector<vector<int> >intermediateRep = permutations(foo);
    //     for (size_t j=0; j<intermediateRep.size(); ++j)
    //         // We add as the first element of each found permutation the 
    //         // element we erased from foo, the n^th element
    //         intermediateRep[j].insert(intermediateRep[j].begin(), init[i]);
    //     // We replace the element we took out from foo, and at the next loop
    //     // we will suppress the next element.
    //     foo.insert(foo.begin()+i,init[i]);

    //     // We append the permutations we found in the vector rep that retrieves
    //     // all the permutations.
    //     rep.insert(rep.end(), intermediateRep.begin(), intermediateRep.end());
    // }

    // return rep;
}

vector<Permutation> permutations(const Permutation& init)
{
    // Here we simply reuse the function that calculates permuations for
    // vectors that is above.

    vector<vector<int> > intermediateRep = permutations(
            init.getPermutation());
    vector<Permutation> rep(intermediateRep.size());
    for (size_t i=0; i!=rep.size(); ++i)
        rep[i] = Permutation(intermediateRep[i]);

    return rep;
}

// Erase redundant permutations in the vector permutation
void reducePermutation(std::vector<Permutation >& permutation)
{
    for (size_t i=0; i<permutation.size()-1; ++i) 
        for (size_t j=i+1; j!=permutation.size(); ++j) 
            if (permutation[i] == permutation[j]) {
                permutation.erase(permutation.begin()+j);
                --j;
            }
}

// Returns all the elements of the permutation group spanned by init
std::vector<Permutation > getSpan(const std::vector<Permutation >& init)
{
    std::vector<Permutation > rep = init;
    reducePermutation(rep);
    std::vector<Permutation > newPermutation = rep;
    while (not newPermutation.empty()) {
        // While we get newPermutations by composing the ones we already had
        const size_t size = rep.size();
        for (size_t i=0; i!=newPermutation.size(); ++i) {
            // For each newPermutation and for each old permutation
            // we add the two possible composition A*B and B*A
            for (size_t j=0; j!=size; ++j) {
                rep.push_back(newPermutation[i]*rep[j]);
                rep.push_back(rep[j]*newPermutation[i]);
            }
        }
        // We reduce redundant permutations
        reducePermutation(rep);
        // If we got new permutations, they are after the position
        // rep.begin()+size in the vector rep.
        newPermutation = std::vector<Permutation >(rep.begin()+size, rep.end());
    }

    return rep;
}

// Same as before, but simply adding element in spanned. This function 
// makes the assumption that spanned is already complete
void getSpan(std::vector<Permutation >& spanned, 
             const Permutation&         element)
{
    // See comments of the above function
    spanned.push_back(element);
    std::vector<Permutation > newPermutation(1,element);
    while (not newPermutation.empty()) {
        const size_t size = spanned.size();
        for (size_t i=0; i!=newPermutation.size(); ++i) {
            for (size_t j=0; j!=size; ++j) {
                spanned.push_back(newPermutation[i]*spanned[j]);
                spanned.push_back(spanned[j]*newPermutation[i]);
            }
        }
        reducePermutation(spanned);
        newPermutation = std::vector<Permutation >(spanned.begin()+size, 
                                                   spanned.end());
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class Symmetry                                //
/*************************************************/
///////////////////////////////////////////////////

Symmetry::Symmetry(): dim(-1), permutation(std::vector<Permutation >(0)) {}

Symmetry::Symmetry(int t_dim)
    :dim(t_dim), 
    permutation(std::vector<Permutation >(0)) 
{}

int Symmetry::getDim() const {
    return dim;
}

size_t Symmetry::getNPermutation() const {
    return permutation.size();
}

vector<Permutation> Symmetry::getPermutation() const {
    return permutation;
}

int Symmetry::getSymmetryOf(int i, int j) const 
{
    if (dim < 2)
        return 0;
    std::vector<int> base(dim);
    for (int i = 0; i != dim; ++i)
        base[i] = i;
    std::swap(base[i], base[j]);
    for (const auto &perm : permutation) {
        auto iter = perm.begin();
        bool different = false;
        for (auto biter = base.begin(); biter != base.end(); 
                ++biter, ++iter)
            if (*biter != *iter) {
                different = true;
                break;
            }
        if (not different)
            return perm.getSymmetry();
    }
    return 0;
}

void Symmetry::addSymmetry(const Permutation& newPermutation, int sym)
{
    if (dim != -1 and dim < (int)newPermutation.size()) {
        callError(cslError::SymmetryMismatch,
    "Symmetry::addSymmetry(const Permutation& newPermutation, int sym)");
    }
    // If the object was not completely initialized we initialize it from
    // the size of the new (and first) permutation
    if (dim == -1)
        dim = newPermutation.size();

    if (sym > 0) 
        sym =  1;
    else         
        sym = -1;

    // If the symmetry is given in the permutation we prefer it to the value
    // of the paramter that has 1 as default value.
    if (newPermutation.getSymmetry() != 0) 
        sym = newPermutation.getSymmetry();
    auto pos = find(permutation.begin(), permutation.end(), newPermutation);
    if (pos != permutation.end()) {
        if (sym == (*pos).getSymmetry())
            return;
        callError(cslError::SymmetryMismatch,
    "Symmetry::addSymmetry(const Permutation& newPermutation, int sym)");
    }
    Permutation foo = newPermutation;
    foo.setSymmetry(sym);
    if (mustGetSpan)
        getSpan(permutation, foo);
    else
        push_back(foo);
    adjustPermutationSize();
}

void Symmetry::addSymmetry(const std::vector<int>& newPermutation, int sym)
{
    CSL_ASSERT_SPEC(dim != -1,
            CSLError::RuntimeError,
            "You should give the symmetry a dimension before calling this"
            " function");
    addSymmetry(Permutation(dim, newPermutation), sym);
}

void Symmetry::addSymmetry(
        const std::vector<std::vector<int>>& newPermutation, 
        int sym)
{
    CSL_ASSERT_SPEC(dim != -1,
            CSLError::RuntimeError,
            "You should give the symmetry a dimension before calling this"
            " function");
    addSymmetry(Permutation(dim, newPermutation), sym);
}

void Symmetry::setMustGetSpan(bool t_must)
{
    mustGetSpan = t_must;
}

void Symmetry::adjustPermutationSize()
{
    size_t max = 0;
    for (const auto& perm : *this)
        if (perm.size() > max)
            max = perm.size();

    for (auto& perm : *this)
        perm.adjustToSize(max);
}

bool Symmetry::operator==(const Symmetry& symmetry) const
{
    if (permutation.size() != symmetry.getNPermutation() 
            or dim != symmetry.getDim())
        return false;

    return comparePlaceIndependant(permutation, symmetry.getPermutation());
    // const vector<Permutation>& t_permutation = symmetry.getPermutation();
    // vector<int> indicesLeft(t_permutation.size());
    // for (size_t i=0; i!= indicesLeft.size(); ++i)
    //     indicesLeft[i] = i;

    // for (size_t i=0; i!=permutation.size(); ++i) {
    //     bool match = false;
    //     for (size_t j=0; j!=indicesLeft.size(); ++j) {
    //         if (permutation[i] == t_permutation[indicesLeft[j]]) {
    //             match = true;
    //             indicesLeft.erase(indicesLeft.begin()+j);
    //             break;
    //         }
    //     }
    //     if (not match)
    //         return false;
    // }

    // return true;
}

bool Symmetry::operator!=(const Symmetry& symmetry) const
{
    return (not operator==(symmetry));
}

Symmetry Symmetry::operator*(const Symmetry& other) const
{
    Symmetry res;
    res.setMustGetSpan(mustGetSpan);
    for (const auto& permSelf : *this)
        for (const auto& permOther : other)
            res.addSymmetry(permSelf * permOther,
                            permSelf.getSymmetry() * permOther.getSymmetry());
    res.adjustPermutationSize();

    return res;
}

std::ostream& operator<<(std::ostream& fout, const Symmetry& symmetry)
{
    fout<<symmetry.permutation.size()<<" symmetries:\n";
    for (const auto& perm : symmetry.permutation) {
        if (perm.getSymmetry() == 1)
            fout << "  -> Symmetry:     ";
        else if (perm.getSymmetry() == -1)
            fout << "  -> AntiSymmetry: ";
        else
            fout << "  -> Permutation: ";
        fout<<perm<<std::endl;
    }

    return fout;
}

///////////////////////////////////////////////////
/*************************************************/
// Class IndexedSymmetry                         //
/*************************************************/
///////////////////////////////////////////////////

IndexedSymmetry::IndexedSymmetry()
    :Symmetry(),
    init()
{
    mustGetSpan = false;
}

IndexedSymmetry::IndexedSymmetry(const IndexStructure& t_init)
    :Symmetry(),
    init(t_init)
{
    mustGetSpan = false;
}

IndexedSymmetry::IndexedSymmetry(const IndexStructure& t_init,
                                 const Symmetry&       initSym)
    :Symmetry(initSym),
    init(t_init)
{
    mustGetSpan = false;
}

IndexedSymmetry::IndexedSymmetry(const Index& i1,
                                 const Index& i2)
    :IndexedSymmetry()
{
    addSymmetry(make_pair(i1, i2));
}

void IndexedSymmetry::addSymmetry(const Index& i1,
                                  const Index& i2)
{
    addSymmetry(make_pair(i1, i2));
}

void IndexedSymmetry::addSymmetry(const pair<Index, Index>& perm,
                                  int                       symmetry)
{
    if (empty()) {
        init = IndexStructure({perm.first, perm.second});
        push_back(Permutation(2, {1, 0}, symmetry));
        return;
    }
    IndexedSymmetry other;
    other.addSymmetry(perm);
    *this *= other;
}

void IndexedSymmetry::addAntiSymmetry(const Index& i1,
                                      const Index& i2)
{
    addSymmetry(make_pair(i1, i2), -1);
}

void IndexedSymmetry::addAntiSymmetry(const pair<Index, Index>& perm)
{
    addSymmetry(perm, -1);
}


size_t IndexedSymmetry::findPos(const Index&    index,
                                size_t&         posMax,
                                IndexStructure& newInit) const
{
    if (auto pos = find_if(init.begin(), init.end(),
                           [&index](const Index& element) {
                               return (element.exactMatch(index));
                           });
            pos != init.end())
        return distance(init.begin(), pos);

    newInit += index;

    return posMax++;
}

pair<Symmetry, Symmetry> IndexedSymmetry::getCorrespondingSymmetries(
        IndexedSymmetry other,
        IndexStructure& newInit
        ) const
{
    vector<size_t> correspondance;
    size_t posMax = init.size();
    newInit = init;
    for (const auto& index : other.init)
        correspondance.push_back(findPos(index, posMax, newInit));

    for (auto& permutation : other)
        permutation.applyRedefinition(correspondance);

    if (posMax > size()) {
        IndexedSymmetry copy = *this;
        for (auto& permutation : copy)
            permutation.adjustToSize(posMax);
        return make_pair((Symmetry)(copy), (Symmetry)(other));
    }
    return make_pair((Symmetry)(*this), (Symmetry)(other));
}

IndexedSymmetry IndexedSymmetry::operator+(const IndexedSymmetry& other) const
{
    if (other.empty())
        return *this;
    if (empty())
        return other;
    IndexStructure initRes;
    pair<Symmetry, Symmetry> symmetries = 
        getCorrespondingSymmetries(other, initRes);
    symmetries.first.insert(symmetries.first.end(),
                            symmetries.second.begin(),
                            symmetries.second.end());

    return IndexedSymmetry(initRes, symmetries.first);
}

IndexedSymmetry& IndexedSymmetry::operator+=(const IndexedSymmetry& other) 
{
    return *this = (*this + other);
}

IndexedSymmetry IndexedSymmetry::operator*(const IndexedSymmetry& other) const
{
     IndexedSymmetry res = *this;
     return res *= other;
}

IndexedSymmetry& IndexedSymmetry::operator*=(const IndexedSymmetry& other)
{
    if (empty())
        *this = other;
    else if (other.empty())
        return *this;
    else {
        IndexStructure initRes;
        pair<Symmetry, Symmetry> symmetries = 
            getCorrespondingSymmetries(other, initRes);
        symmetries.first.setMustGetSpan(false);
        symmetries.second.setMustGetSpan(false);
        *this = IndexedSymmetry(initRes, symmetries.first * symmetries.second);
    }

    return *this;
}

csl::vector_expr IndexedSymmetry::applySymmetry(const Expr& expr) const
{
    if (empty() or init.empty())
        return csl::vector_expr(0);

    csl::vector_expr res(size());
    vector<Index> fooIndices(init.size());
    for (auto& foo : fooIndices)
        foo = init[0].getSpace()->generateIndex( "foo");
            
    for (size_t i = 0; i != size(); ++i) {
        res[i] = DeepCopy(expr);
        for (size_t j = 0; j != permutation[i].size(); ++j) 
            res[i] = ReplaceIndex(res[i], init[j],
                                 fooIndices[j]);
        for (size_t j = 0; j != permutation[i].size(); ++j) 
            res[i] = ReplaceIndex(res[i], fooIndices[j],
                                 init[permutation[i][j]]);
        res[i] = DeepRefreshed(res[i]);
    }

    return res;
}

ostream& operator<<(ostream&               fout,
                    const IndexedSymmetry& sym)
{
    fout << "Index symmetry of \n" << sym.init << endl;
    fout << sym.size() << " permutations: \n";
    for (const auto& perm : sym) {
        for (const auto& pos : perm)
            fout << sym.init[pos] << "  ";
        fout << endl;
    }

    return fout;
}

} // End of namespace csl
