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

 #pragma once

#ifdef DOC_BROWN_DEBUG_MODE

    #include "docbrown.h"

    #define CHECK_POINT_BUILD(Name, Type)\
        static std::shared_ptr<mty::doc::CheckPoint_Base> Name \
            = std::make_shared<mty::doc::CheckPoint<Type>>(#Name);\
        mty::doc::emmett->addCheckPoint(Name);\
        using type_check_point_data_Name = Type;

    #define CHECK_POINT_BUILD_ARGS(Name, Type, ...)\
        std::shared_ptr<mty::doc::CheckPoint_Base> Name \
            = std::make_shared<mty::doc::CheckPoint<Type>>(#Name, __VA_ARGS__);\
        mty::doc::emmett->addCheckPoint(Name);
        
    #define CHECK_POINT_ADD_TO(Name, data)\
        std::dynamic_pointer_cast<mty::doc::CheckPoint<\
            type_check_point_data_Name\
            >>(Name)->push_back(data);

    #define CHECK_POINT_CALL(Name, func)\
        std::dynamic_pointer_cast<mty::doc::CheckPoint<\
            type_check_point_data_Name\
        >>(Name)->func();
    #define CHECK_POINT_CALL_ARGS(Name, func, ...)\
        std::dynamic_pointer_cast<mty::doc::CheckPoint<\
            type_check_point_data_Name\
        >>(Name)->func(__VA_ARGS__);

#else

    #define CHECK_POINT_BUILD(...)
    #define CHECK_POINT_BUILD_ARGS(...)
    #define CHECK_POINT_ADD_TO(...)
    #define CHECK_POINT_CALL(...)
    #define CHECK_POINT_CALL_ARGS(...)

#endif
