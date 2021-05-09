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
#ifndef IMPLEMENTS_STD_VECTOR
#define IMPLEMENTS_STD_VECTOR_NO_OP_NO_PB(object, variable)  \
    public: \
\
    using iterator       = typename std::vector<object>::iterator;\
    using const_iterator = typename std::vector<object>::const_iterator;\
    using reverse_iterator \
        = typename std::vector<object>::reverse_iterator;\
    using const_reverse_iterator \
        = typename std::vector<object>::const_reverse_iterator;\
    \
    \
    size_t size() const { \
        return variable.size(); \
    } \
 \
    bool empty() const { \
        return begin() == end(); \
    } \
 \
    object const &back() const {\
        return *(end() - 1);\
    }\
    object &back() {\
        return *(end() - 1);\
    }\
    \
    typename std::vector<object>::const_iterator begin() const { \
        return variable.begin(); \
    } \
 \
    typename std::vector<object>::const_iterator end() const { \
        return variable.end(); \
    } \
 \
    typename std::vector<object>::iterator begin() { \
        return variable.begin(); \
    } \
 \
    typename std::vector<object>::iterator end() { \
        return variable.end(); \
    } \
 \
    typename std::vector<object>::const_reverse_iterator rbegin() const { \
        return variable.rbegin(); \
    } \
 \
    typename std::vector<object>::const_reverse_iterator rend() const { \
        return variable.rend(); \
    } \
 \
    typename std::vector<object>::reverse_iterator rbegin() { \
        return variable.rbegin(); \
    } \
 \
    typename std::vector<object>::reverse_iterator rend() { \
        return variable.rend(); \
    } \
    void clear() {\
        variable.clear();\
    }\
    typename std::vector<object>::iterator erase(\
            typename std::vector<object>::iterator iter) {\
        return variable.erase(iter);\
    }\
    typename std::vector<object>::const_iterator erase(\
            typename std::vector<object>::const_iterator iter) {\
        return variable.erase(iter);\
    }\
    typename std::vector<object>::iterator erase(\
            typename std::vector<object>::iterator first,\
            typename std::vector<object>::iterator last) {\
        return variable.erase(first, last);\
    }\
    typename std::vector<object>::const_iterator erase(\
            typename std::vector<object>::const_iterator first,\
            typename std::vector<object>::const_iterator last) {\
        return variable.erase(first, last);\
    }\
    typename std::vector<object>::const_iterator insert(\
            typename std::vector<object>::const_iterator pos,\
            const object&                       value) {\
        return variable.insert(pos, value);\
    }\
    typename std::vector<object>::const_iterator insert(\
            typename std::vector<object>::const_iterator pos,\
            object&&                            value) {\
        return variable.insert(pos, value);\
    }\
    void insert(typename std::vector<object>::iterator pos, \
                size_t                                 count,\
                const object&                          value) {\
        variable.insert(pos, count, value);\
    }\
    typename std::vector<object>::iterator insert(\
                typename std::vector<object>::const_iterator pos, \
                size_t                                       count,\
                const object&                                value) {\
        return variable.insert(pos, count, value);\
    }\
    template<class InputIt>\
    void insert(typename std::vector<object>::iterator pos,\
                InputIt                                first,\
                InputIt                                last) {\
        variable.insert(pos, first, last);\
    }\
    template<class InputIt>\
    typename std::vector<object>::iterator insert(\
                typename std::vector<object>::const_iterator pos,\
                InputIt                                      first,\
                InputIt                                      last) {\
        return variable.insert(pos, first, last);\
    }\
    typename std::vector<object>::iterator insert(\
            typename std::vector<object>::const_iterator pos,\
            std::initializer_list<object>                ilist) {\
        return variable.insert(pos, ilist);\
    }\

#define IMPLEMENTS_STD_VECTOR_NO_OP(object, variable)\
    IMPLEMENTS_STD_VECTOR_NO_OP_NO_PB(object, variable)\
    void push_back(const object& newElement) { \
        variable.push_back(newElement); \
    } \
     \
    void push_back(object&& newElement) { \
        variable.push_back(newElement); \
    } \

#define IMPLEMENTS_STD_VECTOR_NO_PB(object, variable) \
    IMPLEMENTS_STD_VECTOR_NO_OP_NO_PB(object, variable)\
 \
    object operator[](size_t i) const { \
        return variable[i]; \
    } \
 \
    object& operator[](size_t i) { \
        return variable[i]; \
    } \

#define IMPLEMENTS_STD_VECTOR(object, variable) \
    IMPLEMENTS_STD_VECTOR_NO_OP(object, variable)\
 \
    object operator[](size_t i) const { \
        return variable[i]; \
    } \
 \
    object& operator[](size_t i) { \
        return variable[i]; \
    } \

#endif
