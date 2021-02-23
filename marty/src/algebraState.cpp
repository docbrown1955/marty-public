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

#include "algebraState.h"
#include "mrtError.h"

using namespace std;

namespace mty {

vector<int> AlgebraState::getLabels() const
{
    return static_cast<vector<int>>(*this);
}

AlgebraState AlgebraState::applyAnnihilator(const std::vector<int>& shifts) const
{
    HEPAssert(shifts.size() == size(),
            mty::error::ValueError,
            static_cast<string>("Shift has a wrong size in")
            +" AlgebraState::applyAnnihilator()");

    AlgebraState s(*this);
    auto iterShift = shifts.begin();
    for (auto iter=s.begin(); iter!=s.end(); ++iter, ++iterShift)
        *iter += *iterShift;
    return s;
}

int AlgebraState::eigenValue(const vector<int>&      root,
                             const std::vector<int>& normRoots,
                             int                     normLambda) const
{
    HEPAssert(root.size() == size(),
            mty::error::ValueError,(string)"CombOperator has a wrong size in"
            +" AlgebraState::eigenValue()");

    int eigenValue = 0;
    for (size_t i=0; i!=size(); ++i)
        eigenValue += root[i]*(*this)[i]*normRoots[i];

    return eigenValue/normLambda;
}

// bool AlgebraState::operator<(AlgebraState const &other) const
// {
//     if (size() < other.size()) 
//         return true;
//     else if (size() > other.size())
//         return false;
//     auto iter_self = begin();
//     auto other_self = other.begin();
//     while (iter_self != end()) {
//         if (*iter_self > *other_self)
//             return true;
//         else if (*other_self < *iter_self)
//             return false;
//         ++other_self;
//         ++iter_self;
//     }
//     return false;
// }

ostream& operator<<(ostream&            fout, 
                    const AlgebraState& state)
{
    fout << "|";
    for (auto iter=state.begin(); iter!=state.end(); ++iter) {
        fout << *iter;
        if (iter+1 != state.end())
            fout << ",";
    }
    fout << ">";

    return fout;
}

} // End of namespace mty
