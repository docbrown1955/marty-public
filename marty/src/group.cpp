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

#include "group.h"
#include "mrtError.h"
#include "quantumField.h"
#include "diracology.h"
#include "mrtUtils.h"
#include "colorSpace.h"
#include "gaugedGroup.h"

using namespace std;
using namespace csl;


namespace mty::group {
ostream& operator<<(ostream&     fout,
                    mty::group::Type type)
{
    switch(type) {
        case mty::group::Type::Trivial: fout << "Trivial"; break;
        case mty::group::Type::O      : fout << "O"      ; break;
        case mty::group::Type::SO     : fout << "SO"     ; break;
        case mty::group::Type::U1     : fout << "U1"     ; break;
        case mty::group::Type::SU     : fout << "SU"     ; break;
        case mty::group::Type::Sp     : fout << "Sp"     ; break;
        case mty::group::Type::E6     : fout << "E6"     ; break;
        case mty::group::Type::E7     : fout << "E7"     ; break;
        case mty::group::Type::E8     : fout << "E8"     ; break;
        case mty::group::Type::F4     : fout << "F4"     ; break;
        case mty::group::Type::G2     : fout << "G2"     ; break;
        case mty::group::Type::Lorentz: fout << "Lorentz"; break;
    }

    return fout;
}
} // End of namespace mty::group

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class AbstractGroup                           //
/*************************************************/
///////////////////////////////////////////////////

AbstractGroup::AbstractGroup(const string& t_name): dim(1), name(t_name)
{}

AbstractGroup::AbstractGroup(int t_dim, const string& t_name): dim(t_dim),
    name(t_name)
{}

int AbstractGroup::getDim() const
{
    return dim;
}

string AbstractGroup::getName() const
{
    return name;
}

SemiSimpleAlgebra* AbstractGroup::getAlgebra() const
{
    callError(cslError::AbstractFuncCalled,
            "AbstractGroup::getAlgebra() const");
    return nullptr;
}

mty::Irrep AbstractGroup::highestWeightRep(const vector<int>&) const
{
    CallHEPError(mty::error::NotImplementedError,
            "Taking highest-weight rep of non semi-simple group");
    return mty::Irrep();
}

mty::Irrep AbstractGroup::getAdjointRep() const
{
    CallHEPError(mty::error::NotImplementedError,
            "AbstractGroup::getAdjointRep()");
    return Irrep();
}

AlgebraState AbstractGroup::getHighestWeight(const csl::Space*) const
{
    callError(cslError::AbstractFuncCalled,
            "AbstractGroup::getHighestWeight(const csl::Space* t_space) const");
    return AlgebraState();
}

///////////////////////////////////////////////////
/*************************************************/
// Class SemiSimpleGroup                         //
/*************************************************/
///////////////////////////////////////////////////

SemiSimpleGroup::SemiSimpleGroup(const string& t_name): AbstractGroup(t_name)
{}

SemiSimpleGroup::SemiSimpleGroup(const SemiSimpleAlgebra& t_algebra,
                                 const string&            t_name)
    :AbstractGroup(t_name)
{
    setAlgebra(t_algebra);
}

SemiSimpleGroup::~SemiSimpleGroup()
{
    for (auto& s : vectorSpaces)
        delete s.second;
}

mty::Irrep SemiSimpleGroup::getTrivialRep() const
{
    return algebra->getTrivialRep();
}

SemiSimpleAlgebra* SemiSimpleGroup::getAlgebra() const
{
    return algebra.get();
}

void SemiSimpleGroup::setAlgebra(
    const SemiSimpleAlgebra& t_algebra)
{
    switch(t_algebra.getType()) {

        case algebra::Type::R:
        algebra.reset(new mty::algebra::R());
        break;
        case algebra::Type::A:
        algebra.reset(new mty::algebra::A(t_algebra.getOrderL()));
        break;
        case algebra::Type::B:
        algebra.reset(new mty::algebra::B(t_algebra.getOrderL()));
        break;
        case algebra::Type::C:
        algebra.reset(new mty::algebra::C(t_algebra.getOrderL()));
        break;
        case algebra::Type::D:
        algebra.reset(new mty::algebra::D(t_algebra.getOrderL()));
        break;
        case algebra::Type::E6:
        algebra.reset(new mty::algebra::E6());
        break;
        case algebra::Type::E7:
        algebra.reset(new mty::algebra::E7());
        break;
        case algebra::Type::E8:
        algebra.reset(new mty::algebra::E8());
        break;
        case algebra::Type::F4:
        algebra.reset(new mty::algebra::F4());
        break;
        case algebra::Type::G2:
        algebra.reset(new mty::algebra::G2());
        break;

        default:
        CallHEPError(mty::error::NotImplementedError,
            "SemiSimple Algebra not recognized in initialization of \
            SemiSimpleGroup.\n");
    }
}

mty::Irrep SemiSimpleGroup::highestWeightRep(const vector<int>& highestWeight) const
{
    return algebra->highestWeightRep(highestWeight);
}

SumIrrep SemiSimpleGroup::tensorProduct(const mty::Irrep& rep1,
                                        const mty::Irrep& rep2) const
{
    return algebra->tensorProduct(rep1, rep2);
}

SumIrrep SemiSimpleGroup::tensorProduct(const initializer_list<int>& rep1,
                                        const initializer_list<int>& rep2)
    const
{
    return algebra->tensorProduct(algebra->highestWeightRep(rep1),
                                  algebra->highestWeightRep(rep2));
}

const Space* SemiSimpleGroup::getVectorSpace(const mty::Irrep& irrep) const
{
    if (getType() == group::Type::U1 or irrep == getTrivialRep())
        return nullptr;
    AlgebraState state = irrep.getHighestWeight();
    auto iter = find_if(vectorSpaces.begin(), vectorSpaces.end(),
            [&state](const pair<AlgebraState,const Space*>& A) {
                return A.first == state;
            });
    if (iter == vectorSpaces.end()) {
        // This space has not been creted yet, we create it
        vectorSpaces.push_back(make_pair(
                    irrep.getHighestWeight(),
                    new color::ColorSpace(this,
                              name,
                              irrep.getDim(),
                              irrep.getDim() == dim)));
        return vectorSpaces[vectorSpaces.size()-1].second;
    }
    else
        return (*iter).second;
}

csl::Expr SemiSimpleGroup::getQuadraticCasimir(const AlgebraState& highest) const
{
    auto pos = quadraticCasimir.find(highest);
    if (pos != quadraticCasimir.end())
        return pos->second;

    quadraticCasimir[highest] = getAlgebra()
        ->getQuadraticCasimir(highest);
    return quadraticCasimir[highest];
}

AlgebraState SemiSimpleGroup::getHighestWeight(const csl::Space* t_space) const
{
    for (const auto& [state, space] : vectorSpaces)
        if (space == t_space)
            return state;
    CallHEPError(mty::error::RuntimeError,
            "Space not found in SemiSimpleGroup::getHighestWeight()");
    return AlgebraState();
}

vector<const Space*> SemiSimpleGroup::getAllVectorSpace() const
{
    vector<const Space*> result(vectorSpaces.size());
    int i = -1;
    for (const auto& pair : vectorSpaces)
        result[++i] = pair.second;

    return result;
}

Index SemiSimpleGroup::getCorrespondingIndex(
        const std::string&        index_name,
        const QuantumFieldParent& parent)
{
    return Index(index_name, getVectorSpace(parent.getGroupIrrep(this)));
}

} // End of namespace mty

namespace mty::group {
///////////////////////////////////////////////////
/*************************************************/
// Class U1                                      //
/*************************************************/
///////////////////////////////////////////////////

U1::U1(const string& t_name)
    :SemiSimpleGroup(mty::algebra::R(), t_name)
{}

mty::group::Type U1::getType() const
{
    return mty::group::Type::U1;
}

mty::Irrep U1::getAdjointRep() const
{
    return algebra->highestWeightRep({0,1});
}

///////////////////////////////////////////////////
/*************************************************/
// Class SU                                      //
/*************************************************/
///////////////////////////////////////////////////

SU::SU(int t_dim)
    :SemiSimpleGroup(mty::algebra::A(t_dim-1), "")
{
    dim = t_dim;
}

SU::SU(const string& t_name, 
       int           t_dim)
    :SemiSimpleGroup(mty::algebra::A(t_dim-1), t_name)
{
    dim = t_dim;
}

mty::group::Type SU::getType() const
{
    return mty::group::Type::SU;
}

mty::Irrep SU::getAdjointRep() const
{
    vector<int> labels(dim-1, 0);
    ++labels[0];
    ++labels[dim-2];
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class SO                                      //
/*************************************************/
///////////////////////////////////////////////////

SO::SO(int t_dim)
    :SemiSimpleGroup("")
{
    dim = t_dim;
    if (t_dim % 2 == 0)
        algebra.reset(new mty::algebra::D(t_dim/2));
    else
        algebra.reset(new mty::algebra::B((t_dim-1)/2));
}

SO::SO(const string& name,
       int           t_dim)
    :SemiSimpleGroup(name)
{
    dim = t_dim;
    if (t_dim % 2 == 0)
        algebra.reset(new mty::algebra::D(t_dim/2));
    else
        algebra.reset(new mty::algebra::B((t_dim-1)/2));
}

mty::group::Type SO::getType() const
{
    return mty::group::Type::SO;
}

mty::Irrep SO::getAdjointRep() const
{
    if (dim == 4)
        return algebra->highestWeightRep({2, 1});
    if (dim == 5)
        return algebra->highestWeightRep({0, 2});
    if (dim == 6)
        return algebra->highestWeightRep({0, 1, 1});
    vector<int> labels;
    if (dim % 2 == 0)
        labels = std::vector<int>(dim/2, 0);
    else
        labels = std::vector<int>((dim - 1)/2, 0);
    ++labels[1];
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class Sp                                      //
/*************************************************/
///////////////////////////////////////////////////

Sp::Sp(int t_dim): SemiSimpleGroup(mty::algebra::C(t_dim/2), "")
{
    dim = t_dim;
}

Sp::Sp(const string& t_name,
       int           t_dim)
    :SemiSimpleGroup(mty::algebra::C(t_dim/2), t_name)
{
    dim = t_dim;
}

mty::group::Type Sp::getType() const
{
    return mty::group::Type::Sp;
}

mty::Irrep Sp::getAdjointRep() const
{
    vector<int> labels(dim / 2, 0);
    labels[0] = 2;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E6                                      //
/*************************************************/
///////////////////////////////////////////////////

E6::E6(const string& t_name)
    :SemiSimpleGroup(mty::algebra::E6(), t_name)
{

}

mty::group::Type E6::getType() const
{
    return mty::group::Type::E6;
}

mty::Irrep E6::getAdjointRep() const
{
    vector<int> labels(6, 0);
    labels[5] = 1;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E7                                      //
/*************************************************/
///////////////////////////////////////////////////

E7::E7(const string& t_name)
    :SemiSimpleGroup(mty::algebra::E7(), t_name)
{

}

mty::group::Type E7::getType() const
{
    return mty::group::Type::E7;
}

mty::Irrep E7::getAdjointRep() const
{
    vector<int> labels(7, 0);
    labels[0] = 1;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E8                                      //
/*************************************************/
///////////////////////////////////////////////////

E8::E8(const string& t_name)
    :SemiSimpleGroup(mty::algebra::E8(), t_name)
{

}

mty::group::Type E8::getType() const
{
    return mty::group::Type::E8;
}

mty::Irrep E8::getAdjointRep() const
{
    vector<int> labels(8, 0);
    labels[0] = 1;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class F4                                      //
/*************************************************/
///////////////////////////////////////////////////

F4::F4(const string& t_name)
    :SemiSimpleGroup(mty::algebra::F4(), t_name)
{

}

mty::group::Type F4::getType() const
{
    return mty::group::Type::F4;
}

mty::Irrep F4::getAdjointRep() const
{
    vector<int> labels(4, 0);
    labels[0] = 1;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class G2                                      //
/*************************************************/
///////////////////////////////////////////////////

G2::G2(const string& t_name)
    :SemiSimpleGroup(mty::algebra::G2(), t_name)
{

}

mty::group::Type G2::getType() const
{
    return mty::group::Type::G2;
}

mty::Irrep G2::getAdjointRep() const
{
    vector<int> labels(2, 0);
    labels[1] = 1;
    return algebra->highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class Lorentz                                 //
/*************************************************/
///////////////////////////////////////////////////

Lorentz::Lorentz()
    :SemiSimpleGroup(mty::algebra::D(2), "")
{
    vectorSpaces.push_back(pair<mty::AlgebraState, const Space*>(
                mty::AlgebraState({1,0}),
                &mty::dirac4
                ));

    vectorSpaces.push_back(pair<mty::AlgebraState, const Space*>(
                mty::AlgebraState({0,1}),
                &mty::dirac4
                ));

    vectorSpaces.push_back(pair<mty::AlgebraState, const Space*>(
                mty::AlgebraState({1,1}),
                &Minkowski
                ));
}

mty::group::Type Lorentz::getType() const
{
    return mty::group::Type::Lorentz;
}
} // End of namespace mty::group

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Functions                                     //
/*************************************************/
///////////////////////////////////////////////////

unique_ptr<SemiSimpleGroup> createGroup(
        group::Type       type,
        std::string const& name,
        int                dim)
{
    if (type == group::Type::U1) {
        return make_unique<mty::group::U1>(name);
    }
    if (type == group::Type::SU) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "SU group");
        return make_unique<mty::group::SU>(name, dim);
    }
    if (type == group::Type::SO) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "SO group");
        return make_unique<mty::group::SO>(name, dim);
    }
    if (type == group::Type::Sp) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "Sp group");
        return make_unique<mty::group::Sp>(name, dim);
    }
    if (type == group::Type::E6) {
        return make_unique<mty::group::E6>(name);
    }
    if (type == group::Type::E7) {
        return make_unique<mty::group::E7>(name);
    }
    if (type == group::Type::E8) {
        return make_unique<mty::group::E8>(name);
    }
    if (type == group::Type::F4) {
        return make_unique<mty::group::F4>(name);
    }
    if (type == group::Type::G2) {
        return make_unique<mty::group::G2>(name);
    }

    std::ostringstream sout;
    sout << type;
    HEPAssert(false,
            mty::error::TypeError,
            "Function createGroup received unknown group "
            +(std::string)"type \"" + sout.str() + "\".");
    return nullptr;
}

unique_ptr<SemiSimpleGroup> createGroup(
        group::Type type,
        int          dim)
{
    if (type == group::Type::U1) {
        return make_unique<mty::group::U1>();
    }
    if (type == group::Type::SU) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "SU group");
        return make_unique<mty::group::SU>(dim);
    }
    if (type == group::Type::SO) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "SO group");
        return make_unique<mty::group::SO>(dim);
    }
    if (type == group::Type::Sp) {
        HEPAssert(dim != -1,
                mty::error::ValueError,
                "You must give a valid dimension (>= 2) when building a "
                "Sp group");
        return make_unique<mty::group::Sp>(dim);
    }

    std::ostringstream sout;
    sout << type;
    HEPAssert(false,
            mty::error::TypeError,
            "Function createGroup received unknown group "
            +(std::string)"type \"" + sout.str() + "\".");
    return nullptr;
}

ostream& operator<<(ostream&             fout,
                    const AbstractGroup& obj)
{
    fout << obj.getType();
    fout << "(" << obj.getDim() << ")";
    return fout;
}

group::Type stringToGroupType(string const& name)
{
    if (name == "U1")
        return group::Type::U1;
    if (name == "SU")
        return group::Type::SU;
    if (name == "SO")
        return group::Type::SO;
    if (name == "Sp")
        return group::Type::Sp;
    HEPAssert(false,
            mty::error::TypeError,
            "Type of group unknown \"" + name + "\".");
    return group::Type::U1;
}

} // End of namespace mty
