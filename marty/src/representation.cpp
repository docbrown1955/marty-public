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

#include "representation.h"
#include "mrtError.h"

using namespace std;

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class Irrep                                   //
/*************************************************/
///////////////////////////////////////////////////

Irrep::Irrep(): dim(0)
{
    rep.push_back(AlgebraState(vector<int>(1,0)));
    highestWeight = rep[0];
    mult = vector<int>(1,1);
}

Irrep::Irrep(const SemiSimpleAlgebra* t_algebra,
             const vector<AlgebraState>& t_rep,
             const vector<int>& multiplicities)
    :dim(0),
    algebra(t_algebra),
    highestWeight(t_rep[0]),
    rep(t_rep),
    mult(multiplicities)
{
    HEPAssert(t_rep.size() == multiplicities.size(),
            mty::error::ValueError,
            "Irrep initializated with incomptible arrays");

    for (const auto& m : multiplicities)
        dim += m;
}

std::size_t Irrep::size() const
{
    return rep.size();
}

bool Irrep::empty() const
{
    return end() == begin();
}

vector<AlgebraState>::iterator Irrep::begin() 
{
    return rep.begin();
}

vector<AlgebraState>::const_iterator Irrep::begin()  const
{
    return rep.begin();
}

vector<AlgebraState>::iterator Irrep::end() 
{
    return rep.end();
}

vector<AlgebraState>::const_iterator Irrep::end()  const
{
    return rep.end();
}

int Irrep::getDim() const
{
    return dim;
}

csl::Expr Irrep::getCharge() const
{
    if (algebra->getType() == algebra::Type::R) {
        return csl::intfraction_s(highestWeight[0], highestWeight[1]);
    }
    CallHEPError(mty::error::NotImplementedError, "Charge not implemented "
            + std::string("for algebra ") + toString(algebra->getType()));
    return nullptr;
}

const SemiSimpleAlgebra* Irrep::getAlgebra() const
{
    return algebra;
}

AlgebraState Irrep::getHighestWeight() const
{
    return highestWeight;
}

vector<AlgebraState> Irrep::getRep() const
{
    return rep;
}

vector<int> Irrep::getMult() const
{
    return mult;
}

Irrep Irrep::getConjugatedRep() const
{
    if (algebra->getType() == algebra::Type::R)
        return algebra->highestWeightRep(
                    {-highestWeight[0], highestWeight[1]});
    vector<int> labels = highestWeight.getLabels();
    vector<int> inverted(labels.size());
    auto jter = inverted.begin();
    for (auto iter = labels.rbegin(); iter != labels.rend(); ++iter, ++jter)
        *jter = *iter;
    return algebra->highestWeightRep(AlgebraState(inverted));
}

AlgebraState& Irrep::operator[](int i)
{
    HEPAssert(i >= 0 and i < int(rep.size()),
            mty::error::IndexError,
            "Index " + toString(i) + " out of bounds of rep with size "
            + toString(rep.size()));
    return rep[i];
}

AlgebraState Irrep::operator[](int i) const
{
    HEPAssert(i >= 0 and i < int(rep.size()),
            mty::error::IndexError,
            "Index " + toString(i) + " out of bounds of rep with size "
            + toString(rep.size()));
    return rep[i];
}

ostream& operator<<(ostream& fout, const Irrep& irrep)
{
    fout << "Representation " << irrep.highestWeight;
    fout << " of dimension " << irrep.dim << endl;

    return fout;
}

bool Irrep::operator<(const Irrep& other) const
{
    return dim < other.dim;
}

bool Irrep::operator>(const Irrep& other) const
{
    return other < *this;
}

bool Irrep::operator<=(const Irrep& other) const
{
    return dim <= other.dim;
}

bool Irrep::operator>=(const Irrep& other) const
{
    return other <= *this;
}

bool Irrep::operator==(const Irrep& other) const
{
    return algebra == other.algebra
        and highestWeight == other.highestWeight;
}

bool Irrep::operator!=(const Irrep& other) const
{
    return not operator==(other);
}


SumIrrep Irrep::operator+(const Irrep& other) const
{
    SumIrrep rep(*this);
    HEPAssert(algebra == other.getAlgebra(), mty::error::ValueError,
            "Sum of reps in different algebras");
    rep.insertSort(other);

    return rep;
}

SumIrrep Irrep::operator+(const SumIrrep& other) const
{
    return other+(*this);
}

SumIrrep Irrep::operator*(const Irrep& other) const
{
    HEPAssert(algebra == other.getAlgebra(), mty::error::ValueError,
            "Tensorial product of reps in different algebras");
    return SumIrrep(algebra->tensorProduct(*this, other));
}

SumIrrep Irrep::operator*(const SumIrrep& other) const
{
    return other*(*this);
}

///////////////////////////////////////////////////
/*************************************************/
// Class SumIrrep                                //
/*************************************************/
///////////////////////////////////////////////////

SumIrrep::SumIrrep()
{}

SumIrrep::SumIrrep(const vector<Irrep>& t_irrep)
{
    if (t_irrep.size() > 0) {
        algebra = t_irrep[0].getAlgebra();
        for (std::size_t i=1; i!=t_irrep.size(); ++i)
            HEPAssert(algebra == t_irrep[i].getAlgebra(),
                    mty::error::ValueError,
                    "Non identical algebras in direct sum !");
    }
    for (const auto& el : t_irrep)
        insertSort(el);
}

SumIrrep::SumIrrep(const Irrep& other) :vector<Irrep>({other}),
    algebra(other.getAlgebra())
{}

int SumIrrep::getDim() const
{
    int dim = 0;
    for (const auto& irrep : *this)
        dim += irrep.getDim();
    return dim;
}

const SemiSimpleAlgebra* SumIrrep::getAlgebra() const
{
    return algebra;
}

void SumIrrep::insertSort(const Irrep& other)
{
    if (size() == 0)
        algebra = other.getAlgebra();
    else
        HEPAssert(algebra == other.getAlgebra(), mty::error::ValueError,
                "Non identical algebras in sum");

    for (auto iter=begin(); iter!=end(); ++iter)
        if (other < *iter) {
            insert(iter, other);
            return;
        }
    insert(end(), other);
}

ostream& operator<<(ostream& fout, const SumIrrep& s)
{
    for (auto iter=s.begin(); iter!=s.end(); ++iter) {
        fout << iter->getDim();
        if (iter + 1 != s.end()) 
            fout << " + ";
    }
    fout << "  (Total dim = " << s.getDim() << ")\n";

    return fout;
}

SumIrrep SumIrrep::operator+(const Irrep& other) const
{
    SumIrrep rep(*this);
    rep.insertSort(other);

    return rep;
}

SumIrrep SumIrrep::operator+(const SumIrrep& other) const
{
    SumIrrep rep(*this);
    for (const auto& irrep : other)
        rep.insertSort(irrep);

    return rep;
}

SumIrrep SumIrrep::operator*(const Irrep& other) const
{
    SumIrrep rep;
    for (const auto& irrep : *this)
        rep = rep + (irrep*other);

    return rep;
}

SumIrrep SumIrrep::operator*(const SumIrrep&) const
{
    SumIrrep rep;
    for (const auto& irrep1 : *this)
        for (const auto& irrep2 : *this)
            rep = rep + irrep1*irrep2;

    return rep;
}

} // End of namespace mty
