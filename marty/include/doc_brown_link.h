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
