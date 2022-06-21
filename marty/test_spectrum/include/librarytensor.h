#ifndef CSL_LIBRARYTENSOR_H_INCLUDED
#define CSL_LIBRARYTENSOR_H_INCLUDED

#include <vector>
#include <iostream>
#include "csl/default_move_cpy.h"

namespace csl {

    template<class Type>
    class LibraryTensor {

        public:

        DEFINE_DEFAULT_CPY_MV(LibraryTensor)

        using iterator       = typename std::vector<Type>::iterator;
        using const_iterator = typename std::vector<Type>::const_iterator;
        using reverse_iterator       
            = typename std::vector<Type>::reverse_iterator;
        using const_reverse_iterator 
            = typename std::vector<Type>::const_reverse_iterator;

        static size_t getTotalDimension(std::vector<size_t> const& dimensions)
        {
            size_t tot = 1;
            for (size_t d : dimensions)
                tot *= d;
            return tot;
        }

        LibraryTensor(std::vector<size_t> const& t_dimensions)
            :dimensions(t_dimensions),
            data(getTotalDimension(dimensions))
        {

        }

        LibraryTensor(std::vector<size_t> const& t_dimensions,
                      Type                const& filler)
            :dimensions(t_dimensions),
            data(getTotalDimension(dimensions), filler)
        {

        }

        LibraryTensor(std::vector<size_t> const& t_dimensions,
                      std::vector<Type>   const& t_data)
            :dimensions(t_dimensions),
            data(t_data)
        {
            if (getTotalDimension(dimensions) != data.size()) {
                std::cerr << "Bad initialization of LibraryTensor in file "
                    << __FILE__ << " (l. " << __LINE__ << "): expected "
                    << getTotalDimension(dimensions) << " elements, "
                    << data.size() << " given.\n";
                exit(123);
            }
        }
     
        std::vector<size_t> const& getDimensions() const {
            return dimensions;
        }
        std::vector<Type> const& toStdVector() const {
            return data;
        }

        size_t getIndex(std::vector<size_t> const& indices) const {
            if (dimensions.size() == 1)
                return indices[0];
            size_t index = 0;
            auto iter_index = indices.begin();
            auto iter_dim   = dimensions.begin();
            for (; iter_dim != dimensions.end(); ++iter_index, ++iter_dim) {
                index *= *iter_dim;
                index += *iter_index;
            }

            return index;
        }

        bool empty() const {
            return data.empty();
        }
        size_t size() const {
            return data.size();
        }

#ifndef CSL_LT_DISABLE_ITERATOR
        iterator begin() {
            return data.begin();
        }
        iterator end() {
            return data.end();
        }
        const_iterator begin() const {
            return data.begin();
        }
        const_iterator end() const {
            return data.end();
        }

        reverse_iterator rbegin() {
            return data.rbegin();
        }
        reverse_iterator rend() {
            return data.rend();
        }
        const_reverse_iterator rbegin() const {
            return data.rbegin();
        }
        const_reverse_iterator rend() const {
            return data.rend();
        }
#endif

        Type& operator[](size_t pos) {
            return data[pos];
        }
        Type const& operator[](size_t pos) const {
            return data[pos];
        }

        Type& operator[](std::vector<size_t> const& indices) {
            return data[getIndex(indices)];
        }
        Type const& operator[](std::vector<size_t> const& indices) const {
            return data[getIndex(indices)];
        }

        public:

        std::vector<size_t> dimensions;

        std::vector<Type>   data;
    };

} // End of namespace csl

#endif // ifndef CSL_LIBRARYTENSOR_H_INCLUDED
