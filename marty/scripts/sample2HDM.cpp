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

 #include <marty>

int main() {

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between 
    // 1 and 4 (inclusive)
    // Type 2 is the MSSM-like 2HDM
    ///////////////////////////////////////////////////

    constexpr int type = 2;

    mty::TwoHDM_Model<type> THDM;
    std::cout << THDM << std::endl;
    
    return 0;
}
